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

#include <unistd.h>
#include <poll.h>


#define SA struct sockaddr

static void net_server_id(void);


int _ggzcore_net_connect(const char* server, const unsigned int port)
{
	return es_make_socket(ES_CLIENT, port, server);
}


int _ggzcore_net_pending(void)
{
	int status = 0;
	struct pollfd fd[1] = {{_ggzcore_state.sock, POLLIN, 0}};

	ggzcore_debug("Checking for net events");	

	if (_ggzcore_state.sock >= 0 && (status = poll(fd, 1, 0)) < 0)
		ggzcore_error_sys_exit("poll failed in _ggzcore_net_pending");
	
	return status;
}


int _ggzcore_net_process(void)
{
	GGZServerOp opcode;

	if (es_read_int(_ggzcore_state.sock, (int*)&opcode) < 0)
		ggzcore_error_sys_exit("read failed in _ggzcore_net_process");

	switch(opcode) {
	case MSG_SERVER_ID:
		net_server_id();
		break;
	default:
		/* not implemented yet! */
		break;
	}
	
	return 0;
}


static void net_server_id(void)
{
	unsigned int size;
	unsigned int version;
	char msg[4096];

	ggzcore_debug("Handling MSG_SERVER_ID");

	es_read_string(_ggzcore_state.sock, (char*)&msg, 4096);
	es_read_int(_ggzcore_state.sock, &version);
	es_read_int(_ggzcore_state.sock, &size);

	ggzcore_debug("%s : proto %d: chat %d", msg, version, size);

	ggzcore_event_trigger(GGZ_SERVER_CONNECT, NULL, 0);
}
