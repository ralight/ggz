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
#include <net.h>
#include <player.h>
#include <state.h>
#include <table.h>

#include <stdlib.h>
#include <string.h>

/* Callbacks */
static void _ggzcore_server_connect(GGZEventID, void*, void*);
static void _ggzcore_server_connect_fail(GGZEventID, void*, void*);
static void _ggzcore_server_login(GGZEventID, void*, void*);
static void _ggzcore_server_login_fail(GGZEventID, void*, void*);
static void _ggzcore_server_list_rooms(GGZEventID, void*, void*);
static void _ggzcore_server_room_join(GGZEventID, void*, void*);
static void _ggzcore_server_room_join_fail(GGZEventID, void*, void*);
static void _ggzcore_server_logout(GGZEventID, void*, void*);


/* ggzcore_server_register() - Register callbacks for server events
 *
 * Receives:
 *
 * Returns:
 */
void _ggzcore_server_register(void)
{
	ggzcore_event_add_callback(GGZ_SERVER_CONNECT, 
				   _ggzcore_server_connect);
	ggzcore_event_add_callback(GGZ_SERVER_CONNECT_FAIL, 
				   _ggzcore_server_connect_fail);
	ggzcore_event_add_callback(GGZ_SERVER_LOGIN, _ggzcore_server_login);
	ggzcore_event_add_callback(GGZ_SERVER_LOGIN_FAIL, 
				   _ggzcore_server_login_fail);
	ggzcore_event_add_callback(GGZ_SERVER_LIST_ROOMS, 
				   _ggzcore_server_list_rooms);
	ggzcore_event_add_callback(GGZ_SERVER_ROOM_JOIN, 
				   _ggzcore_server_room_join);
	ggzcore_event_add_callback(GGZ_SERVER_ROOM_JOIN_FAIL, 
				   _ggzcore_server_room_join_fail);
	ggzcore_event_add_callback(GGZ_SERVER_LOGOUT, _ggzcore_server_logout);
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
	ggzcore_debug(GGZ_DBG_SERVER, "Executing server_connect");
	/* Re-trigger user login event */
	ggzcore_event_enqueue(GGZ_USER_LOGIN, NULL, NULL);
}


/* server_connect_fail() - Callback for server connect-failure event
 *
 * Receives:
 * GGZEventID id    : ID code of triggered event
 * void* event_data : Event-specific data
 * void* user_data  : "User" data
 *
 * Returns:
 */
static void _ggzcore_server_connect_fail(GGZEventID id, void* event_data, 
					 void* user_data)
{
	ggzcore_debug(GGZ_DBG_SERVER, "Executing server_connect_fail");
}


/* server_login() - Callback for server login event
 *
 * Receives:
 * GGZEventID id    : ID code of triggered event
 * void* event_data : Event-specific data
 * void* user_data  : "User" data
 *
 * Returns:
 */
static void _ggzcore_server_login(GGZEventID id, void* event_data, 
				    void* user_data)
{
	ggzcore_debug(GGZ_DBG_SERVER, "Executing server_login");
}


/* server_login_fail() - Callback for server login-failure event
 *
 * Receives:
 * GGZEventID id    : ID code of triggered event
 * void* event_data : Event-specific data
 * void* user_data  : "User" data
 *
 * Returns:
 */
static void _ggzcore_server_login_fail(GGZEventID id, void* event_data, 
				       void* user_data)
{
	ggzcore_debug(GGZ_DBG_SERVER, "Executing server_login_fail");
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


/* server_list_rooms() - Callback for server logout event
 *
 * Receives:
 * GGZEventID id    : ID code of triggered event
 * void* event_data : Event-specific data
 * void* user_data  : "User" data
 *
 * Returns:
 */
static void _ggzcore_server_list_rooms(GGZEventID id, void* event_data, 
				   void* user_data)
{
	ggzcore_debug(GGZ_DBG_SERVER, "Executing server_list_rooms");
	

}


/* server_room_join() - Callback for server room-join event
 *
 * Receives:
 * GGZEventID id    : ID code of triggered event
 * void* event_data : Event-specific data
 * void* user_data  : "User" data
 *
 * Returns:
 */
static void _ggzcore_server_room_join(GGZEventID id, void* event_data, 
				      void* user_data)
{
	ggzcore_debug(GGZ_DBG_SERVER, "Executing room_join");
	_ggzcore_state.room = _ggzcore_state.trans_room;
	_ggzcore_player_list_clear();
	_ggzcore_table_list_clear();
}


/* server_room_join_fail() - Callback for server room-join-fail event
 *
 * Receives:
 * GGZEventID id    : ID code of triggered event
 * void* event_data : Event-specific data
 * void* user_data  : "User" data
 *
 * Returns:
 */
static void _ggzcore_server_room_join_fail(GGZEventID id, void* event_data, 
					   void* user_data)
{
	ggzcore_debug(GGZ_DBG_SERVER, "Executing room_join_fail");
	_ggzcore_state.trans_room = _ggzcore_state.room;
}


