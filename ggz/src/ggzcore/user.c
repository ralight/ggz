/*
 * File: user.c
 * Author: Brent Hendricks
 * Project: GGZ Core Client Lib
 * Date: 9/16/00
 *
 * Code for responding to user events
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
#include <user.h>
#include <net.h>
#include <state.h>
#include <protocol.h>

#include <stdlib.h>
#include <string.h>

/* Callbacks */
static void _ggzcore_user_login(GGZEventID, void*, void*);
static void _ggzcore_user_list_rooms(GGZEventID, void*, void*);
static void _ggzcore_user_join_room(GGZEventID, void*, void*);
static void _ggzcore_user_chat(GGZEventID, void*, void*);
static void _ggzcore_user_logout(GGZEventID, void*, void*);


/* ggzcore_user_register() - Register callbacks for UI events
 *
 * Receives:
 *
 * Returns:
 */
void _ggzcore_user_register(void)
{
	ggzcore_event_connect(GGZ_USER_LOGIN, _ggzcore_user_login);
	ggzcore_event_connect(GGZ_USER_LIST_ROOMS, _ggzcore_user_list_rooms);
	ggzcore_event_connect(GGZ_USER_JOIN_ROOM, _ggzcore_user_join_room);
	ggzcore_event_connect(GGZ_USER_CHAT, _ggzcore_user_chat);
	ggzcore_event_connect(GGZ_USER_LOGOUT, _ggzcore_user_logout);
}


/* _ggzcore_user_login() - Callback for user login events
 *
 * Receives:
 * GGZEventID id    : ID code of triggered event
 * void* event_data : Event-specific data
 * void* user_data  : "User" data
 *
 * Returns:
 */
static void _ggzcore_user_login(GGZEventID id, void* event_data, void* user_data)
				
{
	GGZProfile* profile = (GGZProfile*)event_data;

	ggzcore_debug(GGZ_DBG_USER, "Executing user_login");
	ggzcore_debug(GGZ_DBG_USER, "Profile name %s", profile->name);
	ggzcore_debug(GGZ_DBG_USER, "Profile host %s", profile->host);
	ggzcore_debug(GGZ_DBG_USER, "Profile port %d", profile->port);
	ggzcore_debug(GGZ_DBG_USER, "Profile type %d", profile->type);
	ggzcore_debug(GGZ_DBG_USER, "Profile login %s", profile->login);
	ggzcore_debug(GGZ_DBG_USER, "Profile password %s", profile->password);
	
	/* FIXME: if name is set, look up info in list */
	if (profile->name)
		_ggzcore_state.profile.name = strdup(profile->name);
	_ggzcore_state.profile.host = strdup(profile->host);
	_ggzcore_state.profile.port = profile->port;
	_ggzcore_state.profile.type = profile->type;
	_ggzcore_state.profile.login = strdup(profile->login);
	if (profile->password)
		_ggzcore_state.profile.password = strdup(profile->password);
	
	_ggzcore_net_connect(profile->host, profile->port);
}


/* _ggzcore_user_list_rooms() - Callback for user login events
 *
 * Receives:
 * GGZEventID id    : ID code of triggered event
 * void* event_data : Event-specific data
 * void* user_data  : "User" data
 *
 * Returns:
 */
static void _ggzcore_user_list_rooms(GGZEventID id, void* event_data, void* user_data)
{
	ggzcore_debug(GGZ_DBG_USER, "Executing user_list_rooms");	
	/* FIXME: get options from event_data */
	/* -1 means request rooms of all types */
	/* For now, always set verbose to FALSE */
	_ggzcore_net_send_list_rooms(-1, 0);
}


/* _ggzcore_user_join_room() - Callback for user "join room" event
 *
 * Receives:
 * GGZEventID id    : ID code of triggered event
 * void* event_data : Event-specific data
 * void* user_data  : "User" data
 *
 * Returns:
 */
static void _ggzcore_user_join_room(GGZEventID id, void* event_data, void* user_data)
{
	int *room;

	room = (int*)event_data;
	ggzcore_debug(GGZ_DBG_USER, "Executing user_join_room");	
	_ggzcore_net_send_join_room(*room);
}


/* _ggzcore_user_chat() - Callback for user chat events
 *
 * Receives:
 * GGZEventID id    : ID code of triggered event
 * void* event_data : Event-specific data
 * void* user_data  : "User" data
 *
 * Returns:
 */
static void _ggzcore_user_chat(GGZEventID id, void* event_data, void* user_data)
{
	ggzcore_debug(GGZ_DBG_USER, "Executing user_chat");
	if (event_data) {
		ggzcore_debug(GGZ_DBG_USER, "  data is %s", (char*)event_data);
		_ggzcore_net_send_chat(GGZ_CHAT_NORMAL, NULL, (char*)event_data);
	}
}


/* _ggzcore_user_logout() - Callback for user login events
 *
 * Receives:
 * GGZEventID id    : ID code of triggered event
 * void* event_data : Event-specific data
 * void* user_data  : "User" data
 *
 * Returns:
 */
static void _ggzcore_user_logout(GGZEventID id, void* event_data, void* user_data)
{
	ggzcore_debug(GGZ_DBG_USER, "Executing user_logout");	
	_ggzcore_net_send_logout();
}


