/*
 * File: server.c
 * Author: Brent Hendricks
 * Project: GGZ Core Client Lib
 * Date: 9/16/00
 *
 * Code for responding to server events
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
#include <server.h>
#include <state.h>
#include <net.h>

#include <stdlib.h>
#include <string.h>

/* Callbacks */
static void _ggzcore_server_connect(GGZEventID, void*, void*);
static void _ggzcore_server_logout(GGZEventID, void*, void*);

/* ggzcore_server_register() - Register callbacks for server events
 *
 * Receives:
 *
 * Returns:
 */
void _ggzcore_server_register(void)
{
	ggzcore_event_connect(GGZ_SERVER_CONNECT, _ggzcore_server_connect);
/*	ggzcore_event_connect(GGZ_SERVER_LOGIN_OK, _ggzcore_server_login_ok);*/
	ggzcore_event_connect(GGZ_SERVER_LOGOUT, _ggzcore_server_logout);
}


/* server_connect() - Callback for server connect event
 *
 * Receives:
 * GGZEventID id    : ID code of triggered event
 * void* event_data : Event-specific data
 * void* user_data  : "User" data
 *
 * Returns:
 */
static void _ggzcore_server_connect(GGZEventID id, void* event_data, 
				    void* user_data)
{
	char* error;
	ggzcore_debug(GGZ_DBG_SERVER, "Executing server_connect");

	/* Errors are passed in via event_data */
	if (event_data) {
		error = strdup((char*)event_data);
		ggzcore_event_trigger(GGZ_SERVER_LOGIN_FAIL, error, free);
	}
	else {
		_ggzcore_net_send_login(_ggzcore_state.profile.type, 
					_ggzcore_state.profile.login, 
					_ggzcore_state.profile.password);
	}
}


/* server_logout() - Callback for server logout event
 *
 * Receives:
 * GGZEventID id    : ID code of triggered event
 * void* event_data : Event-specific data
 * void* user_data  : "User" data
 *
 * Returns:
 */
static void _ggzcore_server_logout(GGZEventID id, void* event_data, 
				   void* user_data)
{
	ggzcore_debug(GGZ_DBG_SERVER, "Executing server_logout");

	_ggzcore_net_disconnect();
}
