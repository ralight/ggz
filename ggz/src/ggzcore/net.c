/*
 * File: net.c
 * Author: Brent Hendricks
 * Project: GGZ Core Client Lib
 * Date: 9/22/00
 *
 * Code for performing network I/O
 *
 * Copyright (C) 2000 Brent Hendricks.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */


#include <config.h>
#include <ggzcore.h>
#include <net.h>
#include <protocol.h>
#include <state.h>
#include <easysock.h>

#include <stdlib.h>
#include <unistd.h>
#include <poll.h>
#include <string.h>
#include <errno.h>


/* Handlers for various server commands */
static void _ggzcore_net_handle_server_id(void);
static void _ggzcore_net_handle_login_anon(void);
static void _ggzcore_net_handle_motd(void);
static void _ggzcore_net_handle_logout(void);

/* Error function for Easysock */
static void _ggzcore_net_err_func(const char *, const EsOpType, 
				  const EsDataType);
			 

void _ggzcore_net_init(void)
{
	/* Install socket error handler */
	es_err_func_set(_ggzcore_net_err_func);
}


/* FIXME: Handle threaded I/O */
/* FIXME: set a timeout for connecting */
void _ggzcore_net_connect(const char* server, const unsigned int port)
{
	int sock;

	ggzcore_debug(GGZ_DBG_NET, "Connecting to %s:%d", server, port);
	if ( (sock = es_make_socket(ES_CLIENT, port, server)) >= 0) {
		_ggzcore_state.sock = sock;
	}
}


void _ggzcore_net_disconnect(void)
{
	ggzcore_debug(GGZ_DBG_NET, "Disconnecting");
	close(_ggzcore_state.sock);
	_ggzcore_state.sock = -1;
}


int _ggzcore_net_ispending(void)
{
	int status = 0;
	struct pollfd fd[1] = {{_ggzcore_state.sock, POLLIN, 0}};

	if (_ggzcore_state.sock == -1)
		return 0;

	ggzcore_debug(GGZ_DBG_POLL, "Checking for net events");	
	if (_ggzcore_state.sock >= 0 && (status = poll(fd, 1, 0)) < 0)
		ggzcore_error_sys_exit("poll failed in _ggzcore_net_pending");

	if (status)
		ggzcore_debug(GGZ_DBG_POLL, "Found a net event!");

	return status;
}


int _ggzcore_net_process(void)
{
	GGZServerOp opcode;

	ggzcore_debug(GGZ_DBG_NET, "Processing net events");
	if (es_read_int(_ggzcore_state.sock, (int*)&opcode) < 0)
		ggzcore_error_sys_exit("read failed in _ggzcore_net_process");

	switch(opcode) {
	case MSG_SERVER_ID:
		_ggzcore_net_handle_server_id();
		break;
	case RSP_LOGIN_ANON:
		_ggzcore_net_handle_login_anon();
		break;
	case MSG_MOTD:
		_ggzcore_net_handle_motd();
		break;
	case RSP_LOGOUT:
		_ggzcore_net_handle_logout();
		break;
	default:
		/* not implemented yet! */
		break;
	}
	
	return 0;
}


void _ggzcore_net_send_login(GGZLoginType type, const char* login, 
			     const char* pass)
{

	ggzcore_debug(GGZ_DBG_NET, "Executing net login");
	switch (type) {
	case GGZ_LOGIN:
		es_write_int(_ggzcore_state.sock, REQ_LOGIN);
		es_write_string(_ggzcore_state.sock, _ggzcore_state.profile.login);
		es_write_string(_ggzcore_state.sock, _ggzcore_state.profile.password);
		break;
	case GGZ_LOGIN_GUEST:
		es_write_int(_ggzcore_state.sock, REQ_LOGIN_ANON);
		es_write_string(_ggzcore_state.sock, _ggzcore_state.profile.login);
		break;
	case GGZ_LOGIN_NEW:
		es_write_int(_ggzcore_state.sock, REQ_LOGIN_NEW);
		es_write_string(_ggzcore_state.sock, _ggzcore_state.profile.login);
		break;
	}
}


void _ggzcore_net_send_logout(void)
{
	ggzcore_debug(GGZ_DBG_NET, "Executing net logout");	
	es_write_int(_ggzcore_state.sock, REQ_LOGOUT);
}


static void _ggzcore_net_err_func(const char * msg, const EsOpType op, 
				  const EsDataType data)
{
	switch(op) {
	case ES_CREATE:
		ggzcore_event_trigger(GGZ_SERVER_CONNECT, (void*)msg, NULL);
		break;
	default:
		break;
	}
}
			 

static void _ggzcore_net_handle_server_id(void)
{
	unsigned int size;
	unsigned int version;
	char* status;
	char msg[4096];

	ggzcore_debug(GGZ_DBG_NET, "Handling MSG_SERVER_ID");

	status = malloc(sizeof(char));
	*status = 0;

	if (es_read_string(_ggzcore_state.sock, (char*)&msg, 4096) < 0
	    || es_read_int(_ggzcore_state.sock, &version) < 0
	    || es_read_int(_ggzcore_state.sock, &size) < 0) {
		*status = -1;
		ggzcore_event_trigger(GGZ_SERVER_CONNECT, "Network Error", NULL);
	}
	else {
		ggzcore_debug(GGZ_DBG_NET, "%s : proto %d: chat %d", msg, 
			      version, size);
		ggzcore_event_trigger(GGZ_SERVER_CONNECT, NULL, NULL);
	}
}


static void _ggzcore_net_handle_login_anon(void)
{
	char* status;
	int checksum;

	ggzcore_debug(GGZ_DBG_NET, "Handling RSP_LOGIN_ANON");

	status = malloc(sizeof(char));
	*status = 0;

	if (es_read_char(_ggzcore_state.sock, status) < 0)
		*status = -1;

	if (status == 0 && es_read_int(_ggzcore_state.sock, &checksum) < 0)
		*status = -1;

	ggzcore_debug(GGZ_DBG_NET, "RSP_LOGIN_ANON from server : %d", *status);
	
	if (*status < 0)
		ggzcore_event_trigger(GGZ_SERVER_LOGIN_FAIL, "Login denied", 
				      NULL);
	else 
		ggzcore_event_trigger(GGZ_SERVER_LOGIN_OK, NULL, NULL);
}


static void _ggzcore_net_handle_motd(void)
{
	int i, lines;
	char buf[4096];

	ggzcore_debug(GGZ_DBG_NET, "Handling RSP_MOTD");

	/* FIXME: check for errors */
	es_read_int(_ggzcore_state.sock, &lines);
	for (i = 0; i < lines; i++)
		es_read_string(_ggzcore_state.sock, buf, 4096);

	/* FIXME: trigger MOTD event */
}


static void _ggzcore_net_handle_logout(void)
{
	char* status;

	ggzcore_debug(GGZ_DBG_NET, "Handling RSP_LOGOUT");

	status = malloc(sizeof(char));
	*status = 0;

	if (es_read_char(_ggzcore_state.sock, status) < 0)
		*status = -1;

	ggzcore_debug(GGZ_DBG_NET, "RSP_LOGOUT from server : %d", *status);
	
	ggzcore_event_trigger(GGZ_SERVER_LOGOUT, status, free);
}
