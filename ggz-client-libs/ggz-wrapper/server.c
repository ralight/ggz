/*
 * File: server.c
 * Author: Brent Hendricks
 * Project: GGZ Text Client 
 * Date: 9/26/00
 * $Id: server.c 6872 2005-01-24 04:14:57Z jdorje $
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

#ifdef HAVE_CONFIG_H
#  include <config.h>	/* Site-specific config */
#endif

#include "server.h"
#include "loop.h"
#include "game.h"

#include <ggz.h>
#include <ggzcore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define _(x) x

extern char *dst_nick;
extern char *game_name;

static void server_register(GGZServer * server);
static void server_process(void);

static void checkplayer(void);

/* Hooks for server events */
static GGZHookReturn server_connected(GGZServerEvent id, const void *,
				      const void *);
static GGZHookReturn server_connect_fail(GGZServerEvent id, const void *,
					 const void *);
static GGZHookReturn server_negotiated(GGZServerEvent id, const void *,
				       const void *);
static GGZHookReturn server_login_ok(GGZServerEvent id, const void *,
				     const void *);
static GGZHookReturn server_login_fail(GGZServerEvent id, const void *,
				       const void *);
static GGZHookReturn server_list_rooms(GGZServerEvent id, const void *,
				       const void *);
static GGZHookReturn server_list_types(GGZServerEvent id, const void *,
				       const void *);
static GGZHookReturn server_enter_ok(GGZServerEvent id, const void *,
				     const void *);
static GGZHookReturn server_enter_fail(GGZServerEvent id, const void *,
				       const void *);
static GGZHookReturn server_loggedout(GGZServerEvent id, const void *,
				      const void *);

static GGZHookReturn server_channel_connected(GGZServerEvent id,
					      const void *, const void *);
static GGZHookReturn server_channel_ready(GGZServerEvent id, const void *,
					  const void *);

static GGZHookReturn server_net_error(GGZServerEvent id, const void *,
				      const void *);
static GGZHookReturn server_protocol_error(GGZServerEvent id, const void *,
					   const void *);

static GGZHookReturn room_list_players(GGZRoomEvent id, const void *,
				       const void *);
static GGZHookReturn room_list_tables(GGZRoomEvent id, const void *,
				      const void *);
static GGZHookReturn room_enter(GGZRoomEvent id, const void *,
				const void *);
static GGZHookReturn room_leave(GGZRoomEvent id, const void *,
				const void *);
static GGZHookReturn room_table_launched(GGZRoomEvent id, const void *,
					 const void *);
static GGZHookReturn room_table_launch_fail(GGZRoomEvent id, const void *,
					    const void *);
static GGZHookReturn room_table_joined(GGZRoomEvent id, const void *,
				       const void *);
static GGZHookReturn room_table_join_fail(GGZRoomEvent id, const void *,
					  const void *);
static GGZHookReturn room_table_left(GGZRoomEvent id, const void *,
				     const void *);
static GGZHookReturn room_table_leave_fail(GGZRoomEvent id, const void *,
					   const void *);
static GGZHookReturn room_table_update(GGZRoomEvent id, const void *,
				       const void *);

GGZServer *server = NULL;
static int fd;
static int playing = 0;

extern GGZGame *game;

int server_init(char *host, int port, GGZLoginType type, char *login,
		char *password)
{
	GGZOptions opt;
	opt.flags = GGZ_OPT_PARSER | GGZ_OPT_MODULES;
	ggzcore_init(opt);

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
	ggzcore_server_add_event_hook(server, GGZ_CHANNEL_CONNECTED,
				      server_channel_connected);
	ggzcore_server_add_event_hook(server, GGZ_CHANNEL_READY,
				      server_channel_ready);
}

static void room_register(GGZRoom * room)
{
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
	ggzcore_room_add_event_hook(room, GGZ_TABLE_UPDATE,
				    room_table_update);
}

static GGZHookReturn server_connected(GGZServerEvent id,
				      const void *event_data,
				      const void *user_data)
{

	fd = ggzcore_server_get_fd(server);
	loop_add_fd(fd, server_process, NULL);

	return GGZ_HOOK_OK;
}

static GGZHookReturn server_connect_fail(GGZServerEvent id,
					 const void *event_data,
					 const void *user_data)
{
	const char *msg = event_data;

	fprintf(stderr, _("Connection failed: %s\n"), msg);

	return GGZ_HOOK_OK;
}

static GGZHookReturn server_negotiated(GGZServerEvent id,
				       const void *event_data,
				       const void *user_data)
{
	ggzcore_server_login(server);
	return GGZ_HOOK_OK;
}

static GGZHookReturn server_login_ok(GGZServerEvent id,
				     const void *event_data,
				     const void *user_data)
{
	ggzcore_server_list_rooms(server, -1, 1);

	return GGZ_HOOK_OK;
}

static GGZHookReturn server_login_fail(GGZServerEvent id,
				       const void *event_data,
				       const void *user_data)
{
	const char *msg = event_data;

	fprintf(stderr, _("Login failed: %s\n"), msg);

	/* For the time being disconnect at not to confuse us */
	ggzcore_server_logout(server);
	exit(0);
	return GGZ_HOOK_OK;
}

static GGZModule *pick_module(GGZGameType * gt)
{
	const char *name = ggzcore_gametype_get_name(gt);
	const char *engine = ggzcore_gametype_get_prot_engine(gt);
	const char *version = ggzcore_gametype_get_prot_version(gt);
	int num;

	num = ggzcore_module_get_num_by_type(name, engine, version);

	if (num == 0) {
		fprintf(stderr, _("Game is not installed!\n"));
		return NULL;
	}

	return ggzcore_module_get_nth_by_type(name, engine, version, 0);
}

static GGZHookReturn server_enter_ok(GGZServerEvent id,
				     const void *event_data,
				     const void *user_data)
{
	GGZRoom *room;

	room = ggzcore_server_get_cur_room(server);
	if (ggzcore_room_get_num_players(room) > 0) {
	} else {	/* Get list from server */
		ggzcore_room_list_players(room);
	}

	return GGZ_HOOK_OK;
}

static GGZHookReturn server_enter_fail(GGZServerEvent id,
				       const void *event_data,
				       const void *user_data)
{
	const char *msg = event_data;

	fprintf(stderr, _("Enter failed: %s\n"), msg);

	return GGZ_HOOK_OK;
}

static GGZHookReturn server_loggedout(GGZServerEvent id,
				      const void *event_data,
				      const void *user_data)
{
	exit(0);
	return GGZ_HOOK_OK;
}

static GGZHookReturn server_net_error(GGZServerEvent id,
				      const void *event_data,
				      const void *user_data)
{
	exit(0);
	return GGZ_HOOK_OK;
}

static GGZHookReturn server_protocol_error(GGZServerEvent id,
					   const void *event_data,
					   const void *user_data)
{
	const char *msg = event_data;

	fprintf(stderr, _("Server error: %s disconnected\n"), msg);
	exit(0);

	return GGZ_HOOK_OK;
}

static GGZHookReturn room_list_players(GGZRoomEvent id,
				       const void *event_data,
				       const void *user_data)
{
	GGZRoom *room;
	room = ggzcore_server_get_cur_room(server);
	ggzcore_room_list_tables(room, 0, 0);
	return GGZ_HOOK_OK;
}

static GGZHookReturn room_list_tables(GGZRoomEvent id,
				      const void *event_data,
				      const void *user_data)
{
	checkplayer();
	return GGZ_HOOK_OK;
}

static void checkplayer()
{
	GGZRoom *room;
	GGZPlayer *player = NULL;
	GGZTable *table;
	GGZGameType *gt;
	GGZModule *module;
	int i, table_id, found;
	int bot;

	if (playing)
		return;
#ifdef HAVE_ALARM
	alarm(15);
#else
	/* FIXME */
#endif

	bot = 0;
	room = ggzcore_server_get_cur_room(server);

	if (!dst_nick) {
		table_id = -1;
	} else if (!strcmp(dst_nick, "")) {
		table_id = -1;
		bot = 1;
	} else {
		found = 0;
		for (i = 0; i < ggzcore_room_get_num_players(room); i++) {
			player = ggzcore_room_get_nth_player(room, i);
			if (strcmp
			    (ggzcore_player_get_name(player),
			     dst_nick) == 0) {
				found = 1;
				break;
			}
		}
		if (!found) {
			printf(_
			       ("checkplayer: The player %s could not be found.\n"),
			       dst_nick);
			return;
		}
		found = 0;
		table = ggzcore_player_get_table(player);
		if (!table) {
			printf(_("checkplayer: %s not at a table?\n"),
			       dst_nick);
			return;
		}
		table_id = ggzcore_table_get_id(table);
	}

	gt = ggzcore_room_get_gametype(room);
	if (!gt) {
		printf(_("checkplayer: room without gametype?\n"));
		return;
	}
	module = pick_module(gt);
	if (!module) {
		return;
	}
	printf(_("checkplayer: We're playing...\n"));
#ifdef HAVE_ALARM
	alarm(0);
#else
	/* FIXME */
#endif
	playing = 1;
	game_init(module, gt, table_id, NULL, bot);
	return;
}

static GGZHookReturn room_enter(GGZRoomEvent id, const void *event_data,
				const void *user_data)
{
	checkplayer();
	return GGZ_HOOK_OK;
}

static GGZHookReturn room_leave(GGZRoomEvent id, const void *event_data,
				const void *user_data)
{
	return GGZ_HOOK_OK;
}

static GGZHookReturn room_table_launched(GGZRoomEvent id,
					 const void *event_data,
					 const void *user_data)
{
	checkplayer();
	return GGZ_HOOK_OK;
}

static GGZHookReturn room_table_launch_fail(GGZRoomEvent id,
					    const void *event_data,
					    const void *user_data)
{
	const char *err_msg = event_data;

	fprintf(stderr, _("Table launch failed: %s\n"), err_msg);
	game_quit();

	return GGZ_HOOK_OK;
}

static GGZHookReturn room_table_joined(GGZRoomEvent id,
				       const void *event_data,
				       const void *user_data)
{
	return GGZ_HOOK_OK;
}

static GGZHookReturn room_table_join_fail(GGZRoomEvent id,
					  const void *event_data,
					  const void *user_data)
{
	const char *err_msg = event_data;

	fprintf(stderr, _("Table join failed: %s\n"), err_msg);
	game_quit();
	return GGZ_HOOK_OK;
}

static GGZHookReturn room_table_left(GGZRoomEvent id,
				     const void *event_data,
				     const void *user_data)
{
	exit(0);
	return GGZ_HOOK_OK;
}

static GGZHookReturn room_table_update(GGZRoomEvent id,
				       const void *event_data,
				       const void *user_data)
{
	checkplayer();
	return GGZ_HOOK_OK;
}

static GGZHookReturn room_table_leave_fail(GGZRoomEvent id,
					   const void *event_data,
					   const void *user_data)
{
	fprintf(stderr, _("Table leave failed\n"));
	return GGZ_HOOK_OK;
}

static GGZHookReturn server_list_rooms(GGZServerEvent id,
				       const void *event_data,
				       const void *user_data)
{
	int i, num;

	/* Register callbacks for all rooms */
	num = ggzcore_server_get_num_rooms(server);
	for (i = 0; i < num; i++)
		room_register(ggzcore_server_get_nth_room(server, i));
	ggzcore_server_list_gametypes(server, 0);
	return GGZ_HOOK_OK;
}

static GGZHookReturn server_list_types(GGZServerEvent id,
				       const void *event_data,
				       const void *user_data)
{
	int i, num;
	GGZRoom *room;
	GGZGameType *type;
	const char *name;
	int found;

	num = ggzcore_server_get_num_rooms(server);
	found = 0;
	for (i = 0; i < num; i++) {
		room = ggzcore_server_get_nth_room(server, i);
		type = ggzcore_room_get_gametype(room);
		if (type) {
			name = ggzcore_gametype_get_name(type);
			if (name && game_name
			    && strcmp(name, game_name) == 0) {
				ggzcore_server_join_room(server, i);
				found = 1;
				break;
			}
		}
	}
	if (!found) {
		fprintf(stderr,
			_("Game type %s not found on the server\n"),
			game_name);
		exit(0);
	}
	return GGZ_HOOK_OK;
}

static GGZHookReturn server_channel_connected(GGZServerEvent id,
					      const void *event_data,
					      const void *user_data)
{
	game_channel_connected(ggzcore_server_get_channel(server));
	return GGZ_HOOK_OK;
}

static GGZHookReturn server_channel_ready(GGZServerEvent id,
					  const void *event_data,
					  const void *user_data)
{
	game_channel_ready(ggzcore_server_get_channel(server));
	return GGZ_HOOK_OK;
}
