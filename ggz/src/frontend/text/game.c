/*
 * File: game.c
 * Author: Brent Hendricks
 * Project: GGZ Text Client 
 * Date: 3/1/01
 * $Id: game.c 4673 2002-09-23 22:22:53Z dr_maux $
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


#include "game.h"
#include "output.h"
#include "loop.h"

#include <ggzcore.h>
#include <stdlib.h>

/* Hooks for game events */
static void game_register(GGZGame *game);
static void game_process(void);
static GGZHookReturn game_launched(GGZGameEvent, void*, void*);
static GGZHookReturn game_launch_fail(GGZGameEvent, void*, void*);
static GGZHookReturn game_negotiated(GGZGameEvent, void*, void*);
static GGZHookReturn game_negotiate_fail(GGZGameEvent, void*, void*);
static GGZHookReturn game_playing(GGZGameEvent, void *, void*);
static GGZHookReturn game_over(GGZGameEvent, void *, void*);


GGZGame *game;
GGZGameType *gametype;
int gameindex;
static int fd = -1;

extern GGZServer *server;

void game_init(GGZModule *module, GGZGameType *type, int index)
{
	gametype = type;
	gameindex = index;

	game = ggzcore_game_new();
	ggzcore_game_init(game, module);
	game_register(game);
	ggzcore_game_launch(game);
}


void game_quit(void)
{
	if (fd != -1)
		loop_remove_fd(fd);
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


void game_channel(int fd)
{
	ggzcore_game_set_server_fd(game, fd);
}


static void game_register(GGZGame *game)
{
	ggzcore_game_add_event_hook(game, GGZ_GAME_LAUNCHED, game_launched);
	ggzcore_game_add_event_hook(game, GGZ_GAME_LAUNCH_FAIL, game_launch_fail);
	ggzcore_game_add_event_hook(game, GGZ_GAME_NEGOTIATED, game_negotiated);
	ggzcore_game_add_event_hook(game, GGZ_GAME_NEGOTIATE_FAIL, game_negotiate_fail);
	ggzcore_game_add_event_hook(game, GGZ_GAME_PLAYING, game_playing);
	ggzcore_game_add_event_hook(game, GGZ_GAME_OVER, game_over);
}


static GGZHookReturn game_launched(GGZGameEvent id, void* event_data, 
				   void* user_data)
{
	output_text("--- Launched game");
	
	fd = ggzcore_game_get_control_fd(game);
	loop_add_fd(fd, game_process, game_destroy);

	return GGZ_HOOK_OK;
}


static GGZHookReturn game_launch_fail(GGZGameEvent id, void* event_data,
				      void* user_data)
{
	output_text("--- Launch failed: %s", (char*)event_data);

	return GGZ_HOOK_OK;
}


static GGZHookReturn game_negotiated(GGZGameEvent id, void* event_data,
				     void* user_data)
{
	output_text("--- Negotiated game");

	ggzcore_server_create_channel(server);

	return GGZ_HOOK_OK;
}


static GGZHookReturn game_negotiate_fail(GGZGameEvent id, void* event_data,
				      void* user_data)
{
	output_text("--- Negotiate failed: %s", (char*)event_data);

	return GGZ_HOOK_OK;
}


static GGZHookReturn game_playing(GGZGameEvent id, void* event_data, void* user_data)
{
	GGZRoom *room;
	GGZTable *table;

	output_text("--- Game running");

	room = ggzcore_server_get_cur_room(server);

	if(gameindex < 0) {
		table = ggzcore_table_new();
		ggzcore_table_init(table, gametype, "Fun with ggz-txt", 2);
		ggzcore_table_set_seat(table, 1, GGZ_SEAT_BOT, NULL);

		ggzcore_room_launch_table(room, table);
		ggzcore_table_free(table);
	}
	else {
		ggzcore_room_join_table(room, gameindex, 0);
	}

	return GGZ_HOOK_OK;
}


static GGZHookReturn game_over(GGZGameEvent id, void* event_data, void* user_data)
{
	GGZRoom *room;

	output_text("--- Game over");

	game_quit();
	room = ggzcore_server_get_cur_room(server);
	ggzcore_room_leave_table(room, 0);

	return GGZ_HOOK_OK;
}

