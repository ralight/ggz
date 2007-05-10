/*
 * File: server.c
 * Author: Brent Hendricks
 * Project: GGZ Text Client 
 * Date: 9/26/00
 * $Id: server.c 9108 2007-05-10 10:11:38Z oojah $
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
#include "server.h"
#include "output.h"
#include "loop.h"
#include "motd.h"
#include "game.h"
#include "input.h"
#include "support.h"

#include <ggz.h>
#include <ggzcore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


/* Hooks for server events */
static void server_register(GGZServer * server);
static void server_process(void);
static void room_register(GGZRoom * room);

GGZServer *server = NULL;
int players_on_server = 0;
int players_in_room = 0;

static int fd;
static int first_room_list = 0;
static int workinprogress = 0;
static int tableleft_once = 0;

extern GGZGame *game;

void server_workinprogress(int command, int progress)
{
	workinprogress = progress;
	output_debug("queue: workinprogress: %i=%i", command, progress);
}

void server_progresswait(void)
{
	output_debug("queue: locked? %i", workinprogress);
	while (workinprogress)
		server_process();
	output_debug("queue: free!");
}

void server_init(char *host, int port, GGZLoginType type, char *login,
		 char *password)
{
	char *sessiondump;

	server = ggzcore_server_new();
	ggzcore_server_set_hostinfo(server, host, port, 0);
	ggzcore_server_set_logininfo(server, type, login, password, NULL);
	server_register(server);
	sessiondump =
	    ggzcore_conf_read_string("Debug", "SessionLog", NULL);
	ggzcore_server_log_session(server, sessiondump);
	if (sessiondump)
		ggz_free(sessiondump);

	ggzcore_server_connect(server);
}


void server_logout(void)
{
	server_progresswait();

	ggzcore_server_logout(server);
}


void server_disconnect(void)
{
	server_progresswait();

	if ((server) && (ggzcore_server_is_online(server))) {
		ggzcore_server_disconnect(server);

		output_text(_("--- Disconnected"));
		loop_remove_fd(fd);
	}
}


void server_destroy(void)
{
	ggzcore_server_free(server);
	server = NULL;
}


static void server_process(void)
{
	if (server) {
		int fd = ggzcore_server_get_fd(server);
		output_debug("Server_process: %d", fd);
		ggzcore_server_read_data(server, fd);
	}
}


static GGZHookReturn server_connected(GGZServerEvent id,
				      const void *event_data,
				      const void *user_data)
{
	output_text(_("--- Connected to %s."),
		    ggzcore_server_get_host(server));

	fd = ggzcore_server_get_fd(server);
	loop_add_fd(fd, server_process, server_destroy);

	return GGZ_HOOK_OK;
}


static GGZHookReturn server_connect_fail(GGZServerEvent id,
					 const void *event_data,
					 const void *user_data)
{
	const char *msg = event_data;

	server_workinprogress(COMMAND_CONNECT, 0);

	output_text(_("--- Connection failed: %s"), msg);

	/* For the time being disconnect at not to confuse us 
	   ggzcore_event_enqueue(GGZ_USER_LOGOUT, NULL, NULL); */
	return GGZ_HOOK_OK;
}


static GGZHookReturn server_negotiated(GGZServerEvent id,
				       const void *event_data,
				       const void *user_data)
{
	output_debug("--- Negotiated");
	ggzcore_server_login(server);

	return GGZ_HOOK_OK;
}

static GGZHookReturn server_login_ok(GGZServerEvent id,
				     const void *event_data,
				     const void *user_data)
{
	server_workinprogress(COMMAND_CONNECT, 0);

	output_text(_("--- Logged into %s."),
		    ggzcore_server_get_host(server));

	first_room_list = 1;
	server_workinprogress(COMMAND_LIST, 1);
	ggzcore_server_list_rooms(server, -1, 1);

	return GGZ_HOOK_OK;
}


static GGZHookReturn server_login_fail(GGZServerEvent id,
				       const void *event_data,
				       const void *user_data)
{
	const char *msg = event_data;

	server_workinprogress(COMMAND_CONNECT, 0);

	output_text(_("--- Login failed: %s"), msg);

	/* For the time being disconnect at not to confuse us */
	ggzcore_server_logout(server);
	return GGZ_HOOK_OK;
}


static GGZHookReturn server_enter_ok(GGZServerEvent id,
				     const void *event_data,
				     const void *user_data)
{
	GGZRoom *room;

	server_workinprogress(COMMAND_JOIN, 0);

	room = ggzcore_server_get_cur_room(server);
	output_text(_("--- Entered room %s."), ggzcore_room_get_name(room));

#if 0
	ggzcore_event_enqueue(GGZ_USER_LIST_ROOMS, NULL, NULL);
#endif

	room = ggzcore_server_get_cur_room(server);
	if (ggzcore_room_get_num_players(room) > 0) {
		output_players();
	} else {	/* Get list from server */
		server_workinprogress(COMMAND_LIST, 1);
		ggzcore_room_list_players(room);
	}

	return GGZ_HOOK_OK;
}


static GGZHookReturn server_enter_fail(GGZServerEvent id,
				       const void *event_data,
				       const void *user_data)
{
	const char *msg = event_data;

	server_workinprogress(COMMAND_JOIN, 0);

	output_text(_("--- Enter failed: %s"), msg);

	return GGZ_HOOK_OK;
}


static GGZHookReturn server_state_change(GGZServerEvent id,
					 const void *event_data,
					 const void *user_data)
{
	output_status();

	return GGZ_HOOK_OK;
}


static GGZHookReturn server_loggedout(GGZServerEvent id,
				      const void *event_data,
				      const void *user_data)
{
	output_text(_("--- Disconnected"));
	loop_remove_fd(fd);

	return GGZ_HOOK_OK;
}


static GGZHookReturn server_channel_connected(GGZServerEvent id,
					      const void *event_data,
					      const void *user_data)
{
	output_debug("--- Channel connected");
	game_channel_connected(ggzcore_server_get_channel(server));
	return GGZ_HOOK_OK;
}


static GGZHookReturn server_channel_ready(GGZServerEvent id,
					  const void *event_data,
					  const void *user_data)
{
	output_debug("--- Channel ready");
	game_channel_ready(ggzcore_server_get_channel(server));
	return GGZ_HOOK_OK;
}


static GGZHookReturn server_net_error(GGZServerEvent id,
				      const void *event_data,
				      const void *user_data)
{
	output_text(_("--- Network error: disconnected"));
	loop_remove_fd(fd);

	return GGZ_HOOK_OK;
}


static GGZHookReturn server_protocol_error(GGZServerEvent id,
					   const void *event_data,
					   const void *user_data)
{
	const GGZErrorEventData *event = event_data;

	output_text(_("--- Server error: %s disconnected"), event->message);
	loop_remove_fd(fd);

	return GGZ_HOOK_OK;
}


static GGZHookReturn server_chat_fail(GGZServerEvent id,
				       const void *event_data,
				       const void *user_data)
{
	const GGZErrorEventData *error = event_data;

	output_text(_("--- Chat failed: %s"), error->message);

	return GGZ_HOOK_OK;
}


static GGZHookReturn room_chat(GGZRoomEvent id, const void *event_data,
			       const void *user_data)
{
	const GGZChatEventData *chat = event_data;

	output_chat(chat->type, chat->sender, chat->message);

	return GGZ_HOOK_OK;
}


static GGZHookReturn room_playercount(GGZRoomEvent id,
				      const void *event_data,
				      const void *user_data)
{
	GGZRoom *room;

	room = ggzcore_server_get_cur_room(server);
	if (!room)
		return GGZ_HOOK_OK;

	players_in_room = ggzcore_room_get_num_players(room);

	return GGZ_HOOK_OK;
}


static GGZHookReturn room_list_players(GGZRoomEvent id,
				       const void *event_data,
				       const void *user_data)
{
	server_workinprogress(COMMAND_LIST, 0);
	output_players();

	room_playercount(GGZ_PLAYER_COUNT, NULL, NULL);

	return GGZ_HOOK_OK;
}


static GGZHookReturn room_list_tables(GGZRoomEvent id,
				      const void *event_data,
				      const void *user_data)
{
	server_workinprogress(COMMAND_LIST, 0);
	output_tables();
	return GGZ_HOOK_OK;
}


static GGZHookReturn room_enter(GGZRoomEvent id, const void *event_data,
				const void *user_data)
{
	const GGZRoomChangeEventData *data = event_data;
	const char *player = data->player_name;

	output_text(_("--> %s entered the room."), player);
	return GGZ_HOOK_OK;
}


static GGZHookReturn room_leave(GGZRoomEvent id, const void *event_data,
				const void *user_data)
{
	const GGZRoomChangeEventData *data = event_data;
	const char *player = data->player_name;

	output_text(_("<-- %s left the room."), player);
	return GGZ_HOOK_OK;
}


static GGZHookReturn room_table_launched(GGZRoomEvent id,
					 const void *event_data,
					 const void *user_data)
{
	tableleft_once = 0;
	output_text(_("-- Table launched"));
	return GGZ_HOOK_OK;
}


static GGZHookReturn room_table_launch_fail(GGZRoomEvent id,
					    const void *event_data,
					    const void *user_data)
{
	const char *err_msg = event_data;
	output_text(_("-- Table launch failed: %s"), err_msg);

	game_quit();

	return GGZ_HOOK_OK;
}


static GGZHookReturn room_table_joined(GGZRoomEvent id,
				       const void *event_data,
				       const void *user_data)
{
	tableleft_once = 0;
	server_workinprogress(COMMAND_JOIN, 0);
	output_text(_("-- Table joined"));
	return GGZ_HOOK_OK;
}


static GGZHookReturn room_table_join_fail(GGZRoomEvent id,
					  const void *event_data,
					  const void *user_data)
{
	const char *err_msg = event_data;
	output_text(_("-- Table join failed: %s"), err_msg);

	game_quit();

	return GGZ_HOOK_OK;
}


static GGZHookReturn room_table_left(GGZRoomEvent id,
				     const void *event_data,
				     const void *user_data)
{
	if (tableleft_once)
		return GGZ_HOOK_OK;
	tableleft_once = 1;
	output_enable(1);
	loop_add_fd(STDIN_FILENO, input_command, NULL);

	output_text(_("-- Left table"));

	game_quit();

	return GGZ_HOOK_OK;
}


static GGZHookReturn room_table_leave_fail(GGZRoomEvent id,
					   const void *event_data,
					   const void *user_data)
{
	output_text(_("-- Table leave failed"));
	return GGZ_HOOK_OK;
}


static GGZHookReturn server_list_rooms(GGZServerEvent id,
				       const void *event_data,
				       const void *user_data)
{
	int i, num;

	if (first_room_list == 1)
		first_room_list = 0;
	else
		output_rooms();

	/* Register callbacks for all rooms */
	num = ggzcore_server_get_num_rooms(server);
	for (i = 0; i < num; i++)
		room_register(ggzcore_server_get_nth_room(server, i));

	server_workinprogress(COMMAND_LIST, 0);

	return GGZ_HOOK_OK;
}


static GGZHookReturn server_list_types(GGZServerEvent id,
				       const void *event_data,
				       const void *user_data)
{
	server_workinprogress(COMMAND_LIST, 0);
	output_types();

	return GGZ_HOOK_OK;
}


static GGZHookReturn server_motd_loaded(GGZServerEvent id,
					const void *event_data,
					const void *user_data)
{
	const GGZMotdEventData *motd = event_data;

	motd_print_line(motd->motd);

	return GGZ_HOOK_OK;
}


static GGZHookReturn server_playercount(GGZServerEvent id,
					const void *event_data,
					const void *user_data)
{
	players_on_server = ggzcore_server_get_num_players(server);

	return GGZ_HOOK_OK;
}


static void room_register(GGZRoom * room)
{
	ggzcore_room_add_event_hook(room, GGZ_CHAT_EVENT, room_chat);
	ggzcore_room_add_event_hook(room, GGZ_PLAYER_LIST,
				    room_list_players);
	ggzcore_room_add_event_hook(room, GGZ_TABLE_LIST,
				    room_list_tables);
	ggzcore_room_add_event_hook(room, GGZ_ROOM_ENTER, room_enter);
	ggzcore_room_add_event_hook(room, GGZ_ROOM_LEAVE, room_leave);
	ggzcore_room_add_event_hook(room, GGZ_TABLE_LAUNCHED,
				    room_table_launched);
	ggzcore_room_add_event_hook(room, GGZ_TABLE_LAUNCH_FAIL,
				    room_table_launch_fail);
	ggzcore_room_add_event_hook(room, GGZ_TABLE_JOINED,
				    room_table_joined);
	ggzcore_room_add_event_hook(room, GGZ_TABLE_JOIN_FAIL,
				    room_table_join_fail);
	ggzcore_room_add_event_hook(room, GGZ_TABLE_LEFT, room_table_left);
	ggzcore_room_add_event_hook(room, GGZ_TABLE_LEAVE_FAIL,
				    room_table_leave_fail);
	ggzcore_room_add_event_hook(room, GGZ_PLAYER_COUNT,
				    room_playercount);
}


static void server_register(GGZServer * server)
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
	ggzcore_server_add_event_hook(server, GGZ_CHANNEL_CONNECTED,
				      server_channel_connected);
	ggzcore_server_add_event_hook(server, GGZ_CHANNEL_READY,
				      server_channel_ready);
	ggzcore_server_add_event_hook(server, GGZ_SERVER_PLAYERS_CHANGED,
				      server_playercount);
	ggzcore_server_add_event_hook(server, GGZ_CHAT_FAIL,
				      server_chat_fail);
}
