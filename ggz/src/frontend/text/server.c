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
#include "loop.h"
#include "motd.h"
#include "game.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* Hooks for server events */
static void server_register(GGZServer *server);
static void server_process(void);
static GGZHookReturn server_connected(GGZServerEvent id, void*, void*);
static GGZHookReturn server_connect_fail(GGZServerEvent id, void*, void*);
static GGZHookReturn server_negotiated(GGZServerEvent id, void*, void*);
static GGZHookReturn server_login_ok(GGZServerEvent id, void*, void*);
static GGZHookReturn server_login_fail(GGZServerEvent id, void*, void*);
static GGZHookReturn server_list_rooms(GGZServerEvent id, void*, void*);
static GGZHookReturn server_list_types(GGZServerEvent id, void*, void*);
static GGZHookReturn server_enter_ok(GGZServerEvent id, void*, void*);
static GGZHookReturn server_enter_fail(GGZServerEvent id, void*, void*);
static GGZHookReturn server_loggedout(GGZServerEvent id, void*, void*);
static GGZHookReturn server_state_change(GGZServerEvent id, void*, void*);
static GGZHookReturn server_motd_loaded(GGZServerEvent id, void*, void*);

static GGZHookReturn server_net_error(GGZServerEvent id, void*, void*);
static GGZHookReturn server_protocol_error(GGZServerEvent id, void*, void*);

static GGZHookReturn room_chat_msg(GGZRoomEvent id, void*, void*);
static GGZHookReturn room_chat_prvmsg(GGZRoomEvent id, void*, void*);
static GGZHookReturn room_chat_beep(GGZRoomEvent id, void*, void*);
static GGZHookReturn room_chat_announce(GGZRoomEvent id, void*, void*);
static GGZHookReturn room_list_players(GGZRoomEvent id, void*, void*);
static GGZHookReturn room_list_tables(GGZRoomEvent id, void*, void*);
static GGZHookReturn room_enter(GGZRoomEvent id, void*, void*);
static GGZHookReturn room_leave(GGZRoomEvent id, void*, void*);
static GGZHookReturn room_table_launched(GGZRoomEvent id, void*, void*);
static GGZHookReturn room_table_launch_fail(GGZRoomEvent id, void*, void*);
static GGZHookReturn room_table_joined(GGZRoomEvent id, void*, void*);
static GGZHookReturn room_table_join_fail(GGZRoomEvent id, void*, void*);
static GGZHookReturn room_table_left(GGZRoomEvent id, void*, void*);
static GGZHookReturn room_table_leave_fail(GGZRoomEvent id, void*, void*);
static GGZHookReturn room_table_data(GGZRoomEvent id, void*, void*);


GGZServer *server;
static int fd;

extern GGZGame *game;

void server_init(char *host, int port, GGZLoginType type, char* login, char* password)
{
	char *sessiondump;

	server = ggzcore_server_new();
	ggzcore_server_set_hostinfo(server, host, port);
	ggzcore_server_set_logininfo(server, type, login, password);
	server_register(server);
	sessiondump = ggzcore_conf_read_string("Debug", "SessionLog", NULL);
	ggzcore_server_log_session(server, sessiondump);

	ggzcore_server_connect(server);
}


void server_logout(void)
{
	ggzcore_server_logout(server);
}


void server_disconnect(void)
{
	ggzcore_server_disconnect(server);

#ifdef DEBUG
	output_text("--- Disconnected");
#endif
	loop_remove_fd(fd);
}


void server_destroy(void)
{
	ggzcore_server_free(server);
	server = NULL;
}


static void server_process(void)
{
	if (server)
		ggzcore_server_read_data(server);
}


static void server_register(GGZServer *server)
{
	ggzcore_server_add_event_hook(server, GGZ_CONNECTED, 
				      server_connected);
	ggzcore_server_add_event_hook(server, GGZ_CONNECT_FAIL, 
				      server_connect_fail);
	ggzcore_server_add_event_hook(server, GGZ_NEGOTIATED, 
				      server_negotiated);
	ggzcore_server_add_event_hook(server, GGZ_NEGOTIATE_FAIL, 
				      server_connect_fail);
	ggzcore_server_add_event_hook(server, GGZ_LOGGED_IN, 
				      server_login_ok);
	ggzcore_server_add_event_hook(server, GGZ_LOGIN_FAIL, 
				      server_login_fail);
	ggzcore_server_add_event_hook(server, GGZ_ROOM_LIST, 
				      server_list_rooms);
	ggzcore_server_add_event_hook(server, GGZ_TYPE_LIST, 
				      server_list_types);
	ggzcore_server_add_event_hook(server, GGZ_ENTERED,
				      server_enter_ok);
	ggzcore_server_add_event_hook(server, GGZ_ENTER_FAIL, 
				      server_enter_fail);
	ggzcore_server_add_event_hook(server, GGZ_LOGOUT, 
				      server_loggedout);
	ggzcore_server_add_event_hook(server, GGZ_NET_ERROR, 
				      server_net_error);
	ggzcore_server_add_event_hook(server, GGZ_PROTOCOL_ERROR, 
				      server_protocol_error);
	ggzcore_server_add_event_hook(server, GGZ_STATE_CHANGE, 
				      server_state_change);
        ggzcore_server_add_event_hook(server, GGZ_MOTD_LOADED,
				      server_motd_loaded);
}


static void room_register(GGZRoom *room)
{
	ggzcore_room_add_event_hook(room, GGZ_CHAT, room_chat_msg);
	ggzcore_room_add_event_hook(room, GGZ_ANNOUNCE, room_chat_announce);
	ggzcore_room_add_event_hook(room, GGZ_PRVMSG, room_chat_prvmsg);
	ggzcore_room_add_event_hook(room, GGZ_BEEP, room_chat_beep);
	ggzcore_room_add_event_hook(room, GGZ_PLAYER_LIST, room_list_players);
	ggzcore_room_add_event_hook(room, GGZ_TABLE_LIST, room_list_tables);
	ggzcore_room_add_event_hook(room, GGZ_ROOM_ENTER, room_enter);
	ggzcore_room_add_event_hook(room, GGZ_ROOM_LEAVE, room_leave);
	ggzcore_room_add_event_hook(room, GGZ_TABLE_LAUNCHED, room_table_launched);
	ggzcore_room_add_event_hook(room, GGZ_TABLE_LAUNCH_FAIL, room_table_launch_fail);
	ggzcore_room_add_event_hook(room, GGZ_TABLE_JOINED, room_table_joined);
	ggzcore_room_add_event_hook(room, GGZ_TABLE_JOIN_FAIL, room_table_join_fail);
	ggzcore_room_add_event_hook(room, GGZ_TABLE_LEFT, room_table_left);
	ggzcore_room_add_event_hook(room, GGZ_TABLE_LEAVE_FAIL, room_table_leave_fail);
	ggzcore_room_add_event_hook(room, GGZ_TABLE_DATA, room_table_data);

}


static GGZHookReturn server_connected(GGZServerEvent id, void* event_data, 
				      void* user_data)
{
	output_text("--- Connected to %s.", ggzcore_server_get_host(server));

	fd = ggzcore_server_get_fd(server);
	loop_add_fd(fd, server_process, server_destroy);

	return GGZ_HOOK_OK;
}


static GGZHookReturn server_connect_fail(GGZServerEvent id, void* event_data,
					 void* user_data)
{
	output_text("--- Connection failed: %s", (char*)event_data);

	/* For the time being disconnect at not to confuse us 
	ggzcore_event_enqueue(GGZ_USER_LOGOUT, NULL, NULL);*/
	return GGZ_HOOK_OK;
}


static GGZHookReturn server_negotiated(GGZServerEvent id, void* event_data, 
				      void* user_data)
{
	output_text("--- Negotiated");
	ggzcore_server_login(server);

	return GGZ_HOOK_OK;
}

static GGZHookReturn server_login_ok(GGZServerEvent id, void* event_data, 
				     void* user_data)
{
#ifdef DEBUG
	output_text("--- Logged into to %s.", ggzcore_server_get_host(server));
#endif
//	ggzcore_server_list_rooms(server, -1, 1);

	return GGZ_HOOK_OK;
}


static GGZHookReturn server_login_fail(GGZServerEvent id, void* event_data, void* user_data)
{
	output_text("--- Login failed: %s", (char*)event_data);

	/* For the time being disconnect at not to confuse us */
	ggzcore_server_logout(server);
	return GGZ_HOOK_OK;
}


static GGZHookReturn server_enter_ok(GGZServerEvent id, void* event_data, 
				     void* user_data)
{
	GGZRoom *room;

	room = ggzcore_server_get_cur_room(server);
#ifdef DEBUG
	output_text("--- Entered room", ggzcore_room_get_name(room));
		    
#endif
#if 0
	ggzcore_event_enqueue(GGZ_USER_LIST_ROOMS, NULL, NULL);
#endif

	return GGZ_HOOK_OK;
}


static GGZHookReturn server_enter_fail(GGZServerEvent id, void* event_data, void* user_data)
{
	output_text("--- Enter failed: %s", (char*)event_data);

	return GGZ_HOOK_OK;
}


static GGZHookReturn server_state_change(GGZServerEvent id, void *event_data, void *user_data)
{
	output_status();

	return GGZ_HOOK_OK;
}


static GGZHookReturn server_loggedout(GGZServerEvent id, void* event_data, void* user_data)
{
#ifdef DEBUG
	output_text("--- Disconnected");
#endif
	loop_remove_fd(fd);

	return GGZ_HOOK_OK;
}


static GGZHookReturn server_net_error(GGZServerEvent id, void* event_data, 
				      void* user_data)
{
#ifdef DEBUG
	output_text("--- Network error: disconnected");
#endif
	loop_remove_fd(fd);

	return GGZ_HOOK_OK;
}


static GGZHookReturn server_protocol_error(GGZServerEvent id, void* event_data,
					   void* user_data)
{
#ifdef DEBUG
	output_text("--- Server error: %s disconnected", event_data);
#endif
	loop_remove_fd(fd);

	return GGZ_HOOK_OK;
}


static GGZHookReturn room_chat_msg(GGZRoomEvent id, void* event_data, 
				   void* user_data)
{
	char* player;
	char* message;

	player = ((char**)(event_data))[0];
	message = ((char**)(event_data))[1];
	output_chat(CHAT_MSG, player, message);
	return GGZ_HOOK_OK;
}


static GGZHookReturn room_chat_announce(GGZRoomEvent id, void* event_data, void* user_data)
{
	char* player;
	char* message;

	player = ((char**)(event_data))[0];
	message = ((char**)(event_data))[1];
	output_chat(CHAT_ANNOUNCE, player, message);
	return GGZ_HOOK_OK;
}


static GGZHookReturn room_chat_prvmsg(GGZRoomEvent id, void* event_data, void* user_data)
{
	char* player;
	char* message;

	player = ((char**)(event_data))[0];
	message = ((char**)(event_data))[1];
	output_chat(CHAT_PRVMSG, player, message);
	return GGZ_HOOK_OK;
}


static GGZHookReturn room_chat_beep(GGZRoomEvent id, void* event_data, void* user_data)
{
	char* player;

	player = ((char**)(event_data))[0];

	output_text("--- You've been beeped by %s.", player);
	printf("\007");
	return GGZ_HOOK_OK;
}


static GGZHookReturn room_list_players(GGZRoomEvent id, void* event_data, void* user_data)
{
	output_players();
	return GGZ_HOOK_OK;
}


static GGZHookReturn room_list_tables(GGZRoomEvent id, void* event_data, void* user_data)
{
	output_tables();
	return GGZ_HOOK_OK;
}


static GGZHookReturn room_enter(GGZRoomEvent id, void* event_data, void* user_data)
{
	output_text("--> %s entered the room.", event_data);
	return GGZ_HOOK_OK;
}


static GGZHookReturn room_leave(GGZRoomEvent id, void* event_data, void* user_data)
{
	output_text("<-- %s left the room.", event_data);
	return GGZ_HOOK_OK;
}


static GGZHookReturn room_table_launched(GGZRoomEvent id, void* event_data, void* user_data)
{
	output_text("-- Table launched");
	return GGZ_HOOK_OK;
}


static GGZHookReturn room_table_launch_fail(GGZRoomEvent id, void* event_data, void* user_data)
{
	output_text("-- Table launch failed: %s", event_data);

	game_quit();

	return GGZ_HOOK_OK;
}


static GGZHookReturn room_table_joined(GGZRoomEvent id, void* event_data, void* user_data)
{
	output_text("-- Table joined");
	return GGZ_HOOK_OK;
}


static GGZHookReturn room_table_join_fail(GGZRoomEvent id, void* event_data, void* user_data)
{
	output_text("-- Table join failed: %s", event_data);

	game_quit();

	return GGZ_HOOK_OK;
}


static GGZHookReturn room_table_left(GGZRoomEvent id, void* event_data, void* user_data)
{
	output_text("-- Left table");

	game_quit();

	return GGZ_HOOK_OK;
}


static GGZHookReturn room_table_leave_fail(GGZRoomEvent id, void* event_data, void* user_data)
{
	output_text("-- Table leave failed");
	return GGZ_HOOK_OK;
}
	

static GGZHookReturn room_table_data(GGZRoomEvent id, void* event_data, void* user_data)
{
	output_text("--- Data from server");
	ggzcore_game_send_data(game, event_data);
	return GGZ_HOOK_OK;
}


static GGZHookReturn server_list_rooms(GGZServerEvent id, void* event_data, void* user_data)
{
	int i, num;
	
	output_rooms();
	
	/* Register callbacks for all rooms */
	num = ggzcore_server_get_num_rooms(server);
	for (i = 0; i < num; i++)
		room_register(ggzcore_server_get_nth_room(server, i));

	return GGZ_HOOK_OK;
}


static GGZHookReturn server_list_types(GGZServerEvent id, void* event_data, void* user_data)
{
	output_types();
	
	return GGZ_HOOK_OK;
}


static GGZHookReturn server_motd_loaded(GGZServerEvent id, void* event_data, void* user_data)
{
        int i;
        char **motd;

        motd = event_data;

        for(i = 0; motd[i] != NULL; i++)
                motd_print_line(motd[i]);

        return GGZ_HOOK_OK;
}


