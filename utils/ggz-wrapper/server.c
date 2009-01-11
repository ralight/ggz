/*
 * File: server.c
 * Author: Brent Hendricks
 * Project: GGZ Text Client 
 * Date: 9/26/00
 * $Id: server.c 10849 2009-01-11 09:25:29Z josef $
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <ggz.h>
#include <ggzcore.h>
#include <ggzcore_mainloop.h>

#include "server.h"
#include "loop.h"
#include "game.h"

#define _(x) x

typedef struct {
	GGZServer *server;
	const char *dst_nick;
	const char *frontend;
	const char *game_name;
} wrapper_t;

static void server_register(wrapper_t *wrapper);

static void checkplayer(wrapper_t *wrapper);

/* Hooks for server events */
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

#ifdef GGZ_ENABLE_DEPRECATED
static GGZHookReturn server_channel_connected(GGZServerEvent id,
					      const void *, const void *);
static GGZHookReturn server_channel_ready(GGZServerEvent id, const void *,
					  const void *);

#endif

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

static int playing = 0;

extern GGZGame *game;

wrapper_t global_wrapper;

static void mainloopfunc(GGZCoreMainLoopEvent id, const char *message, GGZServer *server)
{
	if(id == GGZCORE_MAINLOOP_READY) {
		printf("READY: %s\n", message);
		global_wrapper.server = server;
		server_register(&global_wrapper);
		ggzcore_server_list_rooms(server, 1);
	} else if(id == GGZCORE_MAINLOOP_WAIT) {
		fprintf(stderr, "WARNING: %s\n", message);
		exit(-1);
	} else if(id == GGZCORE_MAINLOOP_ABORT) {
		fprintf(stderr, "ERROR: %s\n", message);
		exit(-1);
	}
}

int server_init(const char *uri, const char *dst_nick, const char *frontend, const char *game_name)
{
	GGZCoreMainLoop mainloop;

	mainloop.uri = strdup(uri);
	mainloop.tls = 0;
	mainloop.async = 0;
	mainloop.reconnect = 0;
	mainloop.debug = 0;
	mainloop.loop = 1;
	mainloop.func = mainloopfunc;

	global_wrapper.dst_nick = dst_nick;
	global_wrapper.frontend = frontend;
	global_wrapper.game_name = game_name;

	return ggzcore_mainloop_start(mainloop);
}

static void server_register(wrapper_t *wrapper)
{
	GGZServer *server = wrapper->server;

	ggzcore_server_add_event_hook_full(server, GGZ_ROOM_LIST,
				      server_list_rooms, wrapper);
	ggzcore_server_add_event_hook_full(server, GGZ_TYPE_LIST,
				      server_list_types, wrapper);
	ggzcore_server_add_event_hook_full(server, GGZ_ENTERED,
				      server_enter_ok, wrapper);
	ggzcore_server_add_event_hook_full(server, GGZ_ENTER_FAIL,
				      server_enter_fail, wrapper);
	ggzcore_server_add_event_hook_full(server, GGZ_LOGOUT,
				      server_loggedout, wrapper);
#ifdef GGZ_ENABLE_DEPRECATED
	ggzcore_server_add_event_hook_full(server, GGZ_CHANNEL_CONNECTED,
				      server_channel_connected, wrapper);
	ggzcore_server_add_event_hook_full(server, GGZ_CHANNEL_READY,
				      server_channel_ready, wrapper);
#endif
}

static void room_register(wrapper_t *wrapper, GGZRoom *room)
{
	//GGZRoom *room = ggzcore_server_get_cur_room(wrapper->server);

	ggzcore_room_add_event_hook_full(room, GGZ_PLAYER_LIST,
				    room_list_players, wrapper);
	ggzcore_room_add_event_hook_full(room, GGZ_TABLE_LIST,
				    room_list_tables, wrapper);
	ggzcore_room_add_event_hook_full(room, GGZ_ROOM_ENTER, room_enter, wrapper);
	ggzcore_room_add_event_hook_full(room, GGZ_ROOM_LEAVE, room_leave, wrapper);
	ggzcore_room_add_event_hook_full(room, GGZ_TABLE_LAUNCHED,
				    room_table_launched, wrapper);
	ggzcore_room_add_event_hook_full(room, GGZ_TABLE_LAUNCH_FAIL,
				    room_table_launch_fail, wrapper);
	ggzcore_room_add_event_hook_full(room, GGZ_TABLE_JOINED,
				    room_table_joined, wrapper);
	ggzcore_room_add_event_hook_full(room, GGZ_TABLE_JOIN_FAIL,
				    room_table_join_fail, wrapper);
	ggzcore_room_add_event_hook_full(room, GGZ_TABLE_LEFT, room_table_left, wrapper);
	ggzcore_room_add_event_hook_full(room, GGZ_TABLE_LEAVE_FAIL,
				    room_table_leave_fail, wrapper);
	ggzcore_room_add_event_hook_full(room, GGZ_TABLE_UPDATE,
				    room_table_update, wrapper);
}

static GGZModule *pick_module(GGZGameType * gt, const char *frontend)
{
	const char *name = ggzcore_gametype_get_name(gt);
	const char *engine = ggzcore_gametype_get_prot_engine(gt);
	const char *version = ggzcore_gametype_get_prot_version(gt);
	const char *fe;
	int num, n, i;
	GGZModule *tmpmod;

	num = ggzcore_module_get_num_by_type(name, engine, version);

	if (num == 0) {
		fprintf(stderr, _("Game is not installed!\n"));
		return NULL;
	}

	n = 0;
	if (frontend) {
		for (i = 0; i < num; i++) {
			tmpmod = ggzcore_module_get_nth_by_type(name, engine, version, i);
			fe = ggzcore_module_get_frontend(tmpmod);
			if(!strcmp(frontend, fe))
				n = i;
		}
	}
	return ggzcore_module_get_nth_by_type(name, engine, version, n);
}

static GGZHookReturn server_enter_ok(GGZServerEvent id,
				     const void *event_data,
				     const void *user_data)
{
	wrapper_t *wrapper = (wrapper_t*)user_data;
	GGZRoom *room = ggzcore_server_get_cur_room(wrapper->server);

	if (ggzcore_room_get_num_players(room) > 0) {
		/* Player list is already present */
	} else {
		/* Get list from server */
		ggzcore_room_list_players(room);
	}

	return GGZ_HOOK_OK;
}

static GGZHookReturn server_enter_fail(GGZServerEvent id,
				       const void *event_data,
				       const void *user_data)
{
	const GGZErrorEventData *event = event_data;

	fprintf(stderr, _("Enter failed: %s\n"), event->message);
	/* FIXME: do something here? */

	return GGZ_HOOK_OK;
}

static GGZHookReturn server_loggedout(GGZServerEvent id,
				      const void *event_data,
				      const void *user_data)
{
	exit(0);
	return GGZ_HOOK_OK;
}

static GGZHookReturn room_list_players(GGZRoomEvent id,
				       const void *event_data,
				       const void *user_data)
{
	wrapper_t *wrapper = (wrapper_t*)user_data;
	GGZRoom *room = ggzcore_server_get_cur_room(wrapper->server);

	ggzcore_room_list_tables(room);

	return GGZ_HOOK_OK;
}

static GGZHookReturn room_list_tables(GGZRoomEvent id,
				      const void *event_data,
				      const void *user_data)
{
	checkplayer((wrapper_t*)user_data);
	return GGZ_HOOK_OK;
}

static void checkplayer(wrapper_t *wrapper)
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
	room = ggzcore_server_get_cur_room(wrapper->server);

	if (!wrapper->dst_nick) {
		table_id = -1;
	} else if (!strcmp(wrapper->dst_nick, "")) {
		table_id = -1;
		bot = 1;
	} else {
		found = 0;
		for (i = 0; i < ggzcore_room_get_num_players(room); i++) {
			player = ggzcore_room_get_nth_player(room, i);
			if (strcmp
			    (ggzcore_player_get_name(player),
			     wrapper->dst_nick) == 0) {
				found = 1;
				break;
			}
		}
		if (!found) {
			printf(_("checkplayer: The player %s could not be found.\n"),
			       wrapper->dst_nick);
			return;
		}
		found = 0;
		table = ggzcore_player_get_table(player);
		if (!table) {
			printf(_("checkplayer: %s not at a table?\n"),
			       wrapper->dst_nick);
			return;
		}
		table_id = ggzcore_table_get_id(table);
	}

	gt = ggzcore_room_get_gametype(room);
	if (!gt) {
		printf(_("checkplayer: room without gametype?\n"));
		return;
	}
	module = pick_module(gt, wrapper->frontend);
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
	game_init(wrapper->server, module, gt, table_id, NULL, bot);
	return;
}

static GGZHookReturn room_enter(GGZRoomEvent id, const void *event_data,
				const void *user_data)
{
	checkplayer((wrapper_t*)user_data);
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
	checkplayer((wrapper_t*)user_data);
	return GGZ_HOOK_OK;
}

static GGZHookReturn room_table_launch_fail(GGZRoomEvent id,
					    const void *event_data,
					    const void *user_data)
{
	const GGZErrorEventData *event = event_data;

	fprintf(stderr, _("Table launch failed: %s\n"), event->message);
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
	checkplayer((wrapper_t*)user_data);
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

	wrapper_t *wrapper = (wrapper_t*)user_data;

	/* Register callbacks for all rooms */
	num = ggzcore_server_get_num_rooms(wrapper->server);
	for (i = 0; i < num; i++)
		room_register(wrapper, ggzcore_server_get_nth_room(wrapper->server, i));
	ggzcore_server_list_gametypes(wrapper->server, 0);
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

	wrapper_t *wrapper = (wrapper_t*)user_data;

	num = ggzcore_server_get_num_rooms(wrapper->server);
	found = 0;
	for (i = 0; i < num; i++) {
		room = ggzcore_server_get_nth_room(wrapper->server, i);
		type = ggzcore_room_get_gametype(room);
		if (type) {
			name = ggzcore_gametype_get_name(type);
			if (name && wrapper->game_name
			    && strcmp(name, wrapper->game_name) == 0) {
				ggzcore_server_join_room(wrapper->server, room);
				found = 1;
				break;
			}
		}
	}
	if (!found) {
		fprintf(stderr,
			_("Game type %s not found on the server\n"),
			wrapper->game_name);
		exit(0);
	}
	return GGZ_HOOK_OK;
}

#ifdef GGZ_ENABLE_DEPRECATED
static GGZHookReturn server_channel_connected(GGZServerEvent id,
					      const void *event_data,
					      const void *user_data)
{
	wrapper_t *wrapper = (wrapper_t*)user_data;
	game_channel_connected(wrapper->server);
	return GGZ_HOOK_OK;
}

static GGZHookReturn server_channel_ready(GGZServerEvent id,
					  const void *event_data,
					  const void *user_data)
{
	wrapper_t *wrapper = (wrapper_t*)user_data;
	game_channel_ready(ggzcore_server_get_channel(wrapper->server));
	return GGZ_HOOK_OK;
}
#endif
