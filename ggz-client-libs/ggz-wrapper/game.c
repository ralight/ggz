/*
 * File: game.c
 * Author: Brent Hendricks
 * Project: GGZ Text Client 
 * Date: 3/1/01
 * $Id: game.c 6873 2005-01-24 04:18:31Z jdorje $
 *
 * Functions for handling game events
 *
 * Copyright (C) 2000-2002 Brent Hendricks.
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

#include "ggzcore.h"

#include "game.h"
#include "loop.h"
#include "server.h"

/* Hooks for game events */
static void game_process(void);
static GGZHookReturn game_launched(GGZGameEvent, const void *,
				   const void *);
static GGZHookReturn game_launch_fail(GGZGameEvent, const void *,
				      const void *);
static GGZHookReturn game_negotiated(GGZGameEvent, const void *,
				     const void *);
static GGZHookReturn game_negotiate_fail(GGZGameEvent, const void *,
					 const void *);
static GGZHookReturn game_playing(GGZGameEvent, const void *,
				  const void *);

static GGZGame *game = NULL;
static GGZGameType *gametype = NULL;
static int gameindex = -1;
static int fd = -1;
static const char *user;
static int readserver = 1;
static int usebot = 0;

void game_init(GGZModule * module, GGZGameType * type, int index,
	       const char *nick, int bot)
{
	if (game) {
		fprintf(stderr, "You're already playing a game!\n");
		return;
	}

	user = nick;
	usebot = bot;

	gametype = type;
	gameindex = index;

	game = ggzcore_game_new();
	ggzcore_game_init(game, server, module);
	game_register(game);
	ggzcore_game_launch(game);
}


void game_quit(void)
{
}


void game_destroy(void)
{
	if (game)
		ggzcore_game_free(game);
	game = NULL;
}


static void game_process(void)
{
	if (game)
		ggzcore_game_read_data(game);
}


static void channel_process(void)
{
	if (!readserver)
		return;
	if (server) {
		int fd = ggzcore_server_get_channel(server);
		ggzcore_server_read_data(server, fd);
	}
}


void game_channel_connected(int fd)
{
	loop_add_fd(fd, channel_process, NULL);
}


void game_channel_ready(int fd)
{
	ggzcore_game_set_server_fd(game, fd);
	readserver = 0;
}


void game_register(GGZGame * game)
{
	ggzcore_game_add_event_hook(game, GGZ_GAME_LAUNCHED,
				    game_launched);
	ggzcore_game_add_event_hook(game, GGZ_GAME_LAUNCH_FAIL,
				    game_launch_fail);
	ggzcore_game_add_event_hook(game, GGZ_GAME_NEGOTIATED,
				    game_negotiated);
	ggzcore_game_add_event_hook(game, GGZ_GAME_NEGOTIATE_FAIL,
				    game_negotiate_fail);
	ggzcore_game_add_event_hook(game, GGZ_GAME_PLAYING, game_playing);
}


static GGZHookReturn game_launched(GGZGameEvent id, const void *event_data,
				   const void *user_data)
{

	fd = ggzcore_game_get_control_fd(game);
	loop_add_fd(fd, game_process, game_destroy);

	return GGZ_HOOK_OK;
}


static GGZHookReturn game_launch_fail(GGZGameEvent id,
				      const void *event_data,
				      const void *user_data)
{
	fprintf(stderr, "Launch failed: %s\n", (char *)event_data);

	return GGZ_HOOK_OK;
}


static GGZHookReturn game_negotiated(GGZGameEvent id,
				     const void *event_data,
				     const void *user_data)
{

	ggzcore_server_create_channel(server);

	return GGZ_HOOK_OK;
}


static GGZHookReturn game_negotiate_fail(GGZGameEvent id,
					 const void *event_data,
					 const void *user_data)
{
	fprintf(stderr, "Negotiate failed: %s\n", (char *)event_data);

	return GGZ_HOOK_OK;
}


static GGZHookReturn game_playing(GGZGameEvent id, const void *event_data,
				  const void *user_data)
{
	GGZRoom *room;
	GGZTable *table;

	room = ggzcore_server_get_cur_room(server);

	if (gameindex < 0) {
		table = ggzcore_table_new();
		ggzcore_table_init(table, gametype, "Fun with gaim-ggz",
				   2);
		if (usebot) {
			ggzcore_table_set_seat(table, 1, GGZ_SEAT_BOT,
					       NULL);
		} else {
			if (user != NULL) {
				ggzcore_table_set_seat(table, 1,
						       GGZ_SEAT_RESERVED,
						       user);
			} else {
				ggzcore_table_set_seat(table, 1,
						       GGZ_SEAT_OPEN,
						       NULL);
			}
		}
		ggzcore_room_launch_table(room, table);
		ggzcore_table_free(table);
	} else {
		ggzcore_room_join_table(room, gameindex, 0);
	}

	return GGZ_HOOK_OK;
}
