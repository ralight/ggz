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
static void _ggzcore_net_handle_list_rooms(void);
static void _ggzcore_net_handle_room_join(void);
static void _ggzcore_net_handle_rsp_chat(void);
static void _ggzcore_net_handle_chat(void);


/* Error function for Easysock */
static void _ggzcore_net_err_func(const char *, const EsOpType, 
				  const EsDataType);
			 

/* Datatype for a message from the server */
struct _GGZServerMsg {

	/* Opcode from server */
	GGZServerOp opcode;

	/* Name of opcode */
	char* name;

	/* Handler */
	void (*handler)(void); 
};

/* Array of all server messages */
static struct _GGZServerMsg ggz_server_msgs[] = {
	{MSG_SERVER_ID,      "msg_server_id", _ggzcore_net_handle_server_id},
	{MSG_SERVER_FULL,    "msg_server_full", NULL },
	{MSG_MOTD,           "msg_motd", _ggzcore_net_handle_motd},
	{MSG_CHAT,           "msg_chat", _ggzcore_net_handle_chat},
	{MSG_UPDATE_PLAYERS, "msg_update_players", NULL},
	{MSG_UPDATE_TYPES,   "msg_update_types", NULL},
	{MSG_UPDATE_TABLES,  "msg_update_tables", NULL},
	{MSG_UPDATE_ROOMS,   "msg_update_rooms", NULL},
	{MSG_ERROR,          "msg_error", NULL},
	{RSP_LOGIN_NEW,      "rsp_login_new", NULL},
	{RSP_LOGIN,          "rsp_login", NULL},
	{RSP_LOGIN_ANON,     "rsp_login_anon", _ggzcore_net_handle_login_anon},
	{RSP_LOGOUT,         "rsp_logout", _ggzcore_net_handle_logout},
	{RSP_PREF_CHANGE,    "rsp_pref_change", NULL},
	{RSP_REMOVE_USER,    "rsp_remove_user", NULL},
	{RSP_LIST_PLAYERS,   "rsp_list_players", NULL},
	{RSP_LIST_TYPES,     "rsp_list_types", NULL},
	{RSP_LIST_TABLES,    "rsp_list_tables", NULL},
	{RSP_LIST_ROOMS,     "rsp_list_rooms", _ggzcore_net_handle_list_rooms},
	{RSP_TABLE_OPTIONS,  "rsp_table_options", NULL},
	{RSP_USER_STAT,      "rsp_user_stat", NULL},
	{RSP_TABLE_LAUNCH,   "rsp_table_launch", NULL},
	{RSP_TABLE_JOIN,     "rsp_table_join", NULL},
	{RSP_TABLE_LEAVE,    "rsp_table_leave", NULL},
	{RSP_GAME,           "rsp_game", NULL},
	{RSP_CHAT,           "rsp_chat", _ggzcore_net_handle_rsp_chat},
        {RSP_MOTD,           "rsp_motd", NULL},
	{RSP_ROOM_JOIN,      "rsp_room_join", _ggzcore_net_handle_room_join}
};

/* Total number of recognized messages */
static unsigned int num_messages;

void _ggzcore_net_init(void)
{
	/* Install socket error handler */
	es_err_func_set(_ggzcore_net_err_func);

	/* Obtain accurate message count */
	num_messages = sizeof(ggz_server_msgs)/sizeof(struct _GGZServerMsg);
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
	void (*handler)(void);

	ggzcore_debug(GGZ_DBG_NET, "Processing net events");
	if (es_read_int(_ggzcore_state.sock, (int*)&opcode) < 0)
		ggzcore_error_sys_exit("read failed in _ggzcore_net_process");

	if (opcode < 0 || opcode >= num_messages)
		/* FIXME: should trigger an error event here */
		return -1;
	
	/* Call handler function */
	if (!(handler = ggz_server_msgs[opcode].handler))
		ggzcore_debug(GGZ_DBG_NET, "Server msg: %s not handled yet", 
			      ggz_server_msgs[opcode].name);
	else {
		ggzcore_debug(GGZ_DBG_NET, "Handling %s", 
			      ggz_server_msgs[opcode].name);
		handler();
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
	ggzcore_debug(GGZ_DBG_NET, "Sending REQ_LOGOUT");	
	es_write_int(_ggzcore_state.sock, REQ_LOGOUT);
}

void _ggzcore_net_send_list_rooms(const int type, const char verbose)
{	
	ggzcore_debug(GGZ_DBG_NET, "Sending REQ_LIST_ROOMS");	
	/* FIXME: check for errors */
	es_write_int(_ggzcore_state.sock, REQ_LIST_ROOMS);
	es_write_int(_ggzcore_state.sock, type);
	es_write_char(_ggzcore_state.sock, verbose);
}


void _ggzcore_net_send_join_room(const int room)
{
	ggzcore_debug(GGZ_DBG_NET, "Sending REQ_ROOM_JOIN");	
	/* FIXME: check for errors */
	es_write_int(_ggzcore_state.sock, REQ_ROOM_JOIN);
	es_write_int(_ggzcore_state.sock, room);
}


void _ggzcore_net_send_chat(const GGZChatOp op, const char* player, 
			    const char* msg)
{
	ggzcore_debug(GGZ_DBG_NET, "Sending REQ_CHAT");	
	/* FIXME: check for errors */
	es_write_int(_ggzcore_state.sock, REQ_CHAT);
	es_write_char(_ggzcore_state.sock, op);
	
	if (op & GGZ_CHAT_M_PLAYER)
		es_write_string(_ggzcore_state.sock, player);

	if (op & GGZ_CHAT_M_MESSAGE)
		es_write_string(_ggzcore_state.sock, msg);
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

	/* FIXME: check for errors */
	es_read_int(_ggzcore_state.sock, &lines);
	for (i = 0; i < lines; i++)
		es_read_string(_ggzcore_state.sock, buf, 4096);

	/* FIXME: trigger MOTD event */
}


static void _ggzcore_net_handle_logout(void)
{
	char* status;

	status = malloc(sizeof(char));
	*status = 0;

	if (es_read_char(_ggzcore_state.sock, status) < 0)
		*status = -1;

	ggzcore_debug(GGZ_DBG_NET, "RSP_LOGOUT from server : %d", *status);
	
	ggzcore_event_trigger(GGZ_SERVER_LOGOUT, status, free);
}


static void _ggzcore_net_handle_list_rooms(void)
{
	int i, rooms, id, game;
	char name[4096];
	
	es_read_int(_ggzcore_state.sock, &rooms);

	for (i = 0; i < rooms; i++) {
		es_read_int(_ggzcore_state.sock, &id);
		es_read_string(_ggzcore_state.sock, name, 4096);
		es_read_int(_ggzcore_state.sock, &game);
		ggzcore_debug(GGZ_DBG_NET, "Room: %d plays %d %s", id, game, 
			      name);
	}
	
	/* FIXME: read in information and actually pass back to client */
	ggzcore_event_trigger(GGZ_SERVER_LIST_ROOMS, NULL, NULL);
}


static void _ggzcore_net_handle_room_join(void)
{
	char *status;
	
	status = malloc(sizeof(char));
	*status = 0;

	/* FIXME: need to trigger network error event */
	if (es_read_char(_ggzcore_state.sock, status) < 0)
		*status = -1;

	ggzcore_event_trigger(GGZ_SERVER_ROOM_JOIN, status, free);
}


static void _ggzcore_net_handle_rsp_chat(void)
{
	char *status;
	
	status = malloc(sizeof(char));
	*status = 0;

	/* FIXME: need to trigger network error event */
	if (es_read_char(_ggzcore_state.sock, status) < 0)
		*status = -1;
}


static void _ggzcore_net_handle_chat(void)
{
	char subop;
	char** data;

	if (!(data = calloc(2, sizeof(char*))))
		ggzcore_error_sys_exit("calloc() failed in net_handle_chat");

	es_read_char(_ggzcore_state.sock, &subop);
	es_read_string_alloc(_ggzcore_state.sock, &(data[0]));

	if (subop & GGZ_CHAT_M_MESSAGE) {
		es_read_string_alloc(_ggzcore_state.sock, &(data[1]));
		/* FIXME: come up with a function to free data */
		ggzcore_event_trigger(GGZ_SERVER_CHAT_MSG, data, NULL);
	}
}

