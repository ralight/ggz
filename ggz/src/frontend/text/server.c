/*
 * File: server.c
 * Author: Brent Hendricks
 * Project: GGZ Text Client 
 * Date: 9/26/00
 *
 * Functions for handling server events
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
#include "server.h"
#include "output.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* Hooks for server events */
static GGZHookReturn server_login_ok(GGZEventID id, void*, void*);
static GGZHookReturn server_connect_fail(GGZEventID id, void*, void*);
static GGZHookReturn server_login_fail(GGZEventID id, void*, void*);
static GGZHookReturn server_chat_msg(GGZEventID id, void*, void*);
static GGZHookReturn server_chat_prvmsg(GGZEventID id, void*, void*);
static GGZHookReturn server_chat_beep(GGZEventID id, void*, void*);
static GGZHookReturn server_chat_announce(GGZEventID id, void*, void*);
static GGZHookReturn server_logout(GGZEventID id, void*, void*);
static GGZHookReturn server_room_enter(GGZEventID id, void*, void*);
static GGZHookReturn server_room_leave(GGZEventID id, void*, void*);
static GGZHookReturn server_list_rooms(GGZEventID id, void*, void*);
static GGZHookReturn server_list_players(GGZEventID id, void*, void*);


void server_register(void)
{
	ggzcore_event_add_hook(GGZ_SERVER_LOGIN, server_login_ok);
	ggzcore_event_add_hook(GGZ_SERVER_CONNECT_FAIL, server_connect_fail);
	ggzcore_event_add_hook(GGZ_SERVER_LOGIN_FAIL, server_login_fail);
	ggzcore_event_add_hook(GGZ_SERVER_LIST_ROOMS, server_list_rooms);
	ggzcore_event_add_hook(GGZ_SERVER_LIST_PLAYERS, server_list_players);
	ggzcore_event_add_hook(GGZ_SERVER_CHAT_MSG, server_chat_msg);
	ggzcore_event_add_hook(GGZ_SERVER_CHAT_ANNOUNCE, server_chat_announce);
	ggzcore_event_add_hook(GGZ_SERVER_CHAT_PRVMSG, server_chat_prvmsg);
	ggzcore_event_add_hook(GGZ_SERVER_CHAT_BEEP, server_chat_beep);
	ggzcore_event_add_hook(GGZ_SERVER_LOGOUT, server_logout);
	ggzcore_event_add_hook(GGZ_SERVER_ROOM_ENTER, server_room_enter);
	ggzcore_event_add_hook(GGZ_SERVER_ROOM_LEAVE, server_room_leave);
}


static GGZHookReturn server_login_ok(GGZEventID id, void* event_data, 
				     void* user_data)
{
#ifdef DEBUG
	output_text("--- Connected to %s.", ggzcore_state_get_profile_host());
#endif
	ggzcore_event_enqueue(GGZ_USER_LIST_ROOMS, NULL, NULL);
	return GGZ_HOOK_OK;
}

static GGZHookReturn server_chat_msg(GGZEventID id, void* event_data, 
				     void* user_data)
{
	char* player;
	char* message;

	player = ((char**)(event_data))[0];
	message = ((char**)(event_data))[1];
	output_chat(CHAT_MSG, player, message);
	return GGZ_HOOK_OK;
}

static GGZHookReturn server_chat_announce(GGZEventID id, void* event_data, void* user_data)
{
	char* player;
	char* message;

	player = ((char**)(event_data))[0];
	message = ((char**)(event_data))[1];
	output_chat(CHAT_ANNOUNCE, player, message);
	return GGZ_HOOK_OK;
}

static GGZHookReturn server_chat_prvmsg(GGZEventID id, void* event_data, void* user_data)
{
	char* player;
	char* message;

	player = ((char**)(event_data))[0];
	message = ((char**)(event_data))[1];
	output_chat(CHAT_PRVMSG, player, message);
	return GGZ_HOOK_OK;
}

static GGZHookReturn server_chat_beep(GGZEventID id, void* event_data, void* user_data)
{
	char* player;

	player = ((char**)(event_data))[0];

	output_text("--- You've been beeped by %s.\007", player);
	return GGZ_HOOK_OK;
}

static GGZHookReturn server_connect_fail(GGZEventID id, void* event_data, void* user_data)
{
	output_text("--- Connection failed: %s", (char*)event_data);

	/* For the time being disconnect at not to confuse us */
	ggzcore_event_enqueue(GGZ_USER_LOGOUT, NULL, NULL);
	return GGZ_HOOK_OK;
}


static GGZHookReturn server_login_fail(GGZEventID id, void* event_data, void* user_data)
{
	output_text("--- Connection failed: %s", (char*)event_data);

	/* For the time being disconnect at not to confuse us */
	ggzcore_event_enqueue(GGZ_USER_LOGOUT, NULL, NULL);
	return GGZ_HOOK_OK;
}


static GGZHookReturn server_logout(GGZEventID id, void* event_data, void* user_data)
{
#ifdef DEBUG
	output_text("--- Disconnected");
#endif
	return GGZ_HOOK_OK;
}

static GGZHookReturn server_room_enter(GGZEventID id, void* event_data, void* user_data)
{
	output_text("--> %s entered the room.", event_data);
	return GGZ_HOOK_OK;
}

static GGZHookReturn server_room_leave(GGZEventID id, void* event_data, void* user_data)
{
	output_text("<-- %s left the room.", event_data);
	return GGZ_HOOK_OK;
}


static GGZHookReturn server_list_rooms(GGZEventID id, void* event_data, void* user_data)
{
	output_rooms();
	return GGZ_HOOK_OK;
}

static GGZHookReturn server_list_players(GGZEventID id, void* event_data, void* user_data)
{
	output_players();
	return GGZ_HOOK_OK;
}

