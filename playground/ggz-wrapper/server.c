/*
 * File: server.c
 * Author: Brent Hendricks
 * Project: GGZ Text Client 
 * Date: 9/26/00
 * $Id: server.c 6470 2004-12-14 18:47:34Z josef $
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


#include "server.h"
#include "loop.h"
#include "game.h"

#include <ggz.h>
#include <ggzcore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

extern char *dst_nick;
extern char *game_name;

static void server_register(GGZServer *server);
static void server_process(void);

/* Hooks for server events */
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

static GGZHookReturn server_channel_connected(GGZServerEvent id, void*, void*);
static GGZHookReturn server_channel_ready(GGZServerEvent id, void*, void*);

static GGZHookReturn server_net_error(GGZServerEvent id, void*, void*);
static GGZHookReturn server_protocol_error(GGZServerEvent id, void*, void*);

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

GGZServer *server = NULL;
static int fd;
static int first_room_list = 0;
static int tableleft_once = 0;

extern GGZGame *game;

int server_init(char *host, int port, GGZLoginType type, char* login, char* password)
{
	server = ggzcore_server_new();
	ggzcore_server_set_hostinfo(server, host, port, 0);
	ggzcore_server_set_logininfo(server, type, login, password);
	server_register(server);

	return ggzcore_server_connect(server);
}


static void server_process(void)
{
	if (server) {
		int fd = ggzcore_server_get_fd(server);
		ggzcore_server_read_data(server, fd);
	}
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
	ggzcore_server_add_event_hook(server, GGZ_CHANNEL_CONNECTED,
		server_channel_connected);
	ggzcore_server_add_event_hook(server, GGZ_CHANNEL_READY,
		server_channel_ready);
}

static void room_register(GGZRoom *room)
{
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
}

static GGZHookReturn server_connected(GGZServerEvent id, void* event_data, 
				      void* user_data)
{

	fd = ggzcore_server_get_fd(server);
	loop_add_fd(fd, server_process, NULL);

	return GGZ_HOOK_OK;
}

static GGZHookReturn server_connect_fail(GGZServerEvent id, void* event_data,
					 void* user_data)
{
	fprintf(stderr, "Connection failed: %s\n", (char*)event_data);

	return GGZ_HOOK_OK;
}

static GGZHookReturn server_negotiated(GGZServerEvent id, void* event_data, 
				      void* user_data)
{
	ggzcore_server_login(server);
	return GGZ_HOOK_OK;
}

static GGZHookReturn server_login_ok(GGZServerEvent id, void* event_data, 
				     void* user_data)
{
	ggzcore_server_list_rooms(server, -1, 1);

	return GGZ_HOOK_OK;
}

static GGZHookReturn server_login_fail(GGZServerEvent id, void* event_data, void* user_data)
{
	fprintf(stderr, "Login failed: %s\n", (char*)event_data);

	/* For the time being disconnect at not to confuse us */
	ggzcore_server_logout(server);
	exit(0);
	return GGZ_HOOK_OK;
}

static GGZModule * pick_module(GGZGameType *gt)
{
	const char * name = ggzcore_gametype_get_name(gt);
	const char * engine = ggzcore_gametype_get_prot_engine(gt);
	const char * version = ggzcore_gametype_get_prot_version(gt);
	int num;

	num = ggzcore_module_get_num_by_type(name, engine, version);

	if (num == 0) {
		fprintf(stderr, "Game is not installed!\n");
		return NULL;
	}

	return ggzcore_module_get_nth_by_type(name, engine, version, 0);

}

static GGZHookReturn server_enter_ok(GGZServerEvent id, void* event_data, 
				     void* user_data)
{
	GGZRoom *room;

	room = ggzcore_server_get_cur_room(server);
	if (ggzcore_room_get_num_players(room) > 0) {
	} else { /* Get list from server */
		ggzcore_room_list_players(room);
	}
		
	return GGZ_HOOK_OK;
}

static GGZHookReturn server_enter_fail(GGZServerEvent id, void* event_data, void* user_data)
{
	fprintf(stderr, "Enter failed: %s\n", (char*)event_data);

	return GGZ_HOOK_OK;
}

static GGZHookReturn server_loggedout(GGZServerEvent id, void* event_data, void* user_data)
{
	exit(0);
	return GGZ_HOOK_OK;
}

static GGZHookReturn server_net_error(GGZServerEvent id, void* event_data, 
				      void* user_data)
{
	exit(0);
	return GGZ_HOOK_OK;
}

static GGZHookReturn server_protocol_error(GGZServerEvent id, void* event_data,
					   void* user_data)
{
	char *msg = event_data;
	fprintf(stderr, "Server error: %s disconnected\n", msg);
	exit(0);
	loop_remove_fd(fd);

	return GGZ_HOOK_OK;
}

static GGZHookReturn room_list_players(GGZRoomEvent id, void* event_data, void* user_data)
{
	GGZRoom *room;
	room=ggzcore_server_get_cur_room(server);
	ggzcore_room_list_tables(room,0,0);
	return GGZ_HOOK_OK;
}

static GGZHookReturn room_list_tables(GGZRoomEvent id, void* event_data, void* user_data)
{
	GGZRoom *room;
	GGZPlayer *player;
	GGZTable *table;
	GGZGameType *gt;
	GGZModule *module;
	int i, a, table_id;

	room = ggzcore_server_get_cur_room(server);
	if(dst_nick) {
		for(i = 0; i < ggzcore_room_get_num_players(room); i++) {
				player=ggzcore_room_get_nth_player(room, i);
				if(strcmp(ggzcore_player_get_name(player), dst_nick) == 0)
					break;
		}
		if(i >= ggzcore_room_get_num_players(room)) {
			printf("Le joueur '%s' n'a pas été trouvé\n", dst_nick);
			exit(0);
			return GGZ_HOOK_OK;
		}
		if((table = ggzcore_player_get_table(player)) == NULL) {
			sleep(1);
			a++;
			if(a >= 10) {
				printf("%s n'a pas ete trouvé\n", dst_nick);
				exit(0);
			}
			ggzcore_room_list_players(room);
			return GGZ_HOOK_OK;
		}
		if(table == NULL) {
			printf("%s n'est point la :(", dst_nick);
			exit(0);
		}
		table_id = ggzcore_table_get_id(table);
	
	} else
		table_id = -1;

	gt = ggzcore_room_get_gametype(room);
	if(!gt) {
		printf("Pas dans une room??\n");
		return GGZ_HOOK_OK;
	}
	module = pick_module(gt);
	if(!module) {
		return GGZ_HOOK_OK;
	}
	game_init(module, gt, table_id, NULL);
	return GGZ_HOOK_OK;
}

static GGZHookReturn room_enter(GGZRoomEvent id, void* event_data, void* user_data)
{
	char *player;
	player=event_data;
	return GGZ_HOOK_OK;
}

static GGZHookReturn room_leave(GGZRoomEvent id, void* event_data, void* user_data)
{
	char *player;
	player=event_data;
	return GGZ_HOOK_OK;
}

static GGZHookReturn room_table_launched(GGZRoomEvent id, void* event_data, void* user_data)
{
	tableleft_once = 0;
	return GGZ_HOOK_OK;
}

static GGZHookReturn room_table_launch_fail(GGZRoomEvent id, void* event_data, void* user_data)
{
	char *err_msg = event_data;
	fprintf(stderr, "Table launch failed: %s\n", err_msg);

	game_quit();

	return GGZ_HOOK_OK;
}

static GGZHookReturn room_table_joined(GGZRoomEvent id, void* event_data, void* user_data)
{
	tableleft_once = 0;
	return GGZ_HOOK_OK;
}

static GGZHookReturn room_table_join_fail(GGZRoomEvent id, void* event_data, void* user_data)
{
	char *err_msg = event_data;
	fprintf(stderr, "Table join failed: %s\n", err_msg);

	game_quit();

	return GGZ_HOOK_OK;
}

static GGZHookReturn room_table_left(GGZRoomEvent id, void* event_data, void* user_data)
{
	if(tableleft_once) return GGZ_HOOK_OK;
	tableleft_once = 1;

	exit(0);

	return GGZ_HOOK_OK;
}

static GGZHookReturn room_table_leave_fail(GGZRoomEvent id, void* event_data, void* user_data)
{
	fprintf(stderr, "Table leave failed\n");
	return GGZ_HOOK_OK;
}
	
static GGZHookReturn server_list_rooms(GGZServerEvent id, void* event_data, void* user_data)
{
	int i, num;
	
	if(first_room_list == 1)
		first_room_list = 0;

	/* Register callbacks for all rooms */
	num = ggzcore_server_get_num_rooms(server);
	for (i = 0; i < num; i++)
		room_register(ggzcore_server_get_nth_room(server, i));
	ggzcore_server_list_gametypes(server, 0);
	return GGZ_HOOK_OK;
}

static GGZHookReturn server_list_types(GGZServerEvent id, void* event_data, void* user_data)
{
	int i, num;
	GGZRoom *room;
	GGZGameType *type;
	const char *name;

	num = ggzcore_server_get_num_rooms(server);
	for (i = 0; i < num; i++) {
		room = ggzcore_server_get_nth_room(server, i);
		type = ggzcore_room_get_gametype(room);
		if (type) {
			name = ggzcore_gametype_get_name(type);
			if (name && game_name && strcmp(name, game_name) == 0) {
				ggzcore_server_join_room(server, i);
			}
		}
	}
	return GGZ_HOOK_OK;
}

static GGZHookReturn server_channel_connected(GGZServerEvent id,
                                             void *event_data,
                                             void *user_data)
{
	game_channel_connected(ggzcore_server_get_channel(server));
	return GGZ_HOOK_OK;
}

static GGZHookReturn server_channel_ready(GGZServerEvent id, void* event_data, void* user_data)
{
	game_channel_ready(ggzcore_server_get_channel(server));
	return GGZ_HOOK_OK;
}

