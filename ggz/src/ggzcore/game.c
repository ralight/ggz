/*
 * File: game.c
 * Author: Brent Hendricks
 * Project: GGZ Core Client Lib
 * Date: 2/28/2001
 * $Id: game.c 4915 2002-10-14 22:08:49Z jdorje $
 *
 * This fils contains functions for handling games being played
 *
 * Copyright (C) 1998 Brent Hendricks.
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
#  include <config.h>
#endif

#include "game.h"
#include "module.h"
#include "ggzcore.h"
#include "hook.h"
#include "room.h"
#include "server.h"

#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <errno.h>
#include <ggz.h>
#include <ggzmod.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>


/* Local functions */
static int _ggzcore_game_event_is_valid(GGZGameEvent event);
static GGZHookReturn _ggzcore_game_event(struct _GGZGame *game, 
					 GGZGameEvent id, void *data);
#if 0 /* currently unused */
static char* _ggzcore_game_get_path(char **argv);
#endif


/* Array of GGZGame messages */
static char* _ggzcore_game_events[] = {
	"GGZ_GAME_LAUNCHED",
	"GGZ_GAME_LAUNCH_FAIL",
	"GGZ_GAME_NEGOTIATED",
	"GGZ_GAME_NEGOTIATE_FAIL",
	"GGZ_GAME_PLAYING",
	"GGZ_GAME_OVER",
	"GGZ_GAME_IO_ERROR",
	"GGZ_GAME_PROTO_ERROR"
};

/* Total number of server events messages */
static unsigned int _ggzcore_num_events = sizeof(_ggzcore_game_events)/sizeof(_ggzcore_game_events[0]);


/*
 * The GGZGame object manages information about a particular running game
 */
struct _GGZGame {

	/* Pointer to module this game is playing */
	struct _GGZModule *module;

	/* Room events */
	struct _GGZHookList *event_hooks[sizeof(_ggzcore_game_events)/sizeof(_ggzcore_game_events[0])];

	/* GGZ Game module connection */
	GGZMod *client;

	/* The server data for this game. */
	GGZServer *server;

	/* Are we playing or watching at our table? */
	int spectating;

	/* What's our seat number? */
	int seat_num;
	
	/* Which room this game is in. */
	int room_num;

	/* The table ID for this game. */
	int table_num;
};


static void _ggzcore_game_handle_state(GGZMod *mod, GGZModEvent event, void *data);


/* Publicly exported functions */

GGZGame* ggzcore_game_new(void)
{
	return _ggzcore_game_new();
}


int ggzcore_game_init(GGZGame *game, GGZServer *server, GGZModule *module)
{
	if (!game
	    || !module
	    || !server
	    || !_ggzcore_server_get_cur_room(server)
	    || _ggzcore_server_get_cur_game(server))
		return -1;

	_ggzcore_game_init(game, server, module);
	
	return 0;
}


void ggzcore_game_free(GGZGame *game)
{
	if (game)
		_ggzcore_game_free(game);
}


/* Functions for attaching hooks to GGZGame events */
int ggzcore_game_add_event_hook(GGZGame *game,
				const GGZGameEvent event, 
				const GGZHookFunc func)
{
	if (game && _ggzcore_game_event_is_valid(event)
	    && game->event_hooks[event])
		return _ggzcore_game_add_event_hook_full(game, event, func, NULL);
	else
		return -1;
}


int ggzcore_game_add_event_hook_full(GGZGame *game,
				     const GGZGameEvent event, 
				     const GGZHookFunc func,
				     void *data)
{
	if (game && _ggzcore_game_event_is_valid(event)
	    && game->event_hooks[event])
		return _ggzcore_game_add_event_hook_full(game, event, func, data);
	else
		return -1;
}


/* Functions for removing hooks from GGZGame events */
int ggzcore_game_remove_event_hook(GGZGame *game,
				   const GGZGameEvent event, 
				   const GGZHookFunc func)
{
	if (game && _ggzcore_game_event_is_valid(event)
	    && game->event_hooks[event])
		return _ggzcore_game_remove_event_hook(game, event, func);
	else 
		return -1;
}


int ggzcore_game_remove_event_hook_id(GGZGame *game,
				      const GGZGameEvent event, 
				      const unsigned int hook_id)
{
	if (game && _ggzcore_game_event_is_valid(event)
	    && game->event_hooks[event])
		return _ggzcore_game_remove_event_hook_id(game, event, hook_id);
	else
		return -1;
}


int ggzcore_game_get_control_fd(GGZGame *game)
{
	if (game)
		return _ggzcore_game_get_control_fd(game);
	else
		return -1;
}


int ggzcore_game_read_data(GGZGame *game)
{
	if (game)
		return _ggzcore_game_read_data(game);
	else
		return -1;
}   


void ggzcore_game_set_server_fd(GGZGame *game, unsigned int fd)
{
	if (game)
		return _ggzcore_game_set_server_fd(game, fd);
}


GGZModule* ggzcore_game_get_module(GGZGame *game)
{
	if (game)
		return _ggzcore_game_get_module(game);
	else
		return NULL;
}
	

int ggzcore_game_launch(GGZGame *game)
{
	if (game && game->module)
		return _ggzcore_game_launch(game);
	else
		return -1;
}


/* 
 * Internal library functions (prototypes in room.h) 
 * NOTE:All of these functions assume valid inputs!
 */


struct _GGZGame* _ggzcore_game_new(void)
{
	struct _GGZGame *game;

	game = ggz_malloc(sizeof(struct _GGZGame));

	return game;
}


void _ggzcore_game_init(struct _GGZGame *game,
			GGZServer *server,
			struct _GGZModule *module)
{
	int i;
	GGZRoom *room = _ggzcore_server_get_cur_room(server);

	game->server = server;
	game->room_num = _ggzcore_room_get_id(room);
	game->table_num = -1;

	_ggzcore_server_set_cur_game(server, game);

     	game->module = module;

	ggz_debug(GGZCORE_DBG_GAME, "Initializing new game");

	/* Setup event hook list */
	for (i = 0; i < _ggzcore_num_events; i++)
		game->event_hooks[i] = _ggzcore_hook_list_init(i);

	/* Setup client module connection */
	game->client = ggzmod_new(GGZMOD_GGZ);
	ggzmod_set_gamedata(game->client, game);
	ggzmod_set_handler(game->client, GGZMOD_EVENT_STATE,
			   &_ggzcore_game_handle_state);
	ggzmod_set_module(game->client, NULL,
			  _ggzcore_module_get_argv(game->module));
	ggzmod_set_player(game->client,
			  _ggzcore_server_get_handle(server),
			  0, -1);
}


static void _ggzcore_game_handle_state(GGZMod *mod, GGZModEvent event, void *data)
{
	GGZGame* game = ggzmod_get_gamedata(mod);
	GGZModState cur, prev;

	prev = *(GGZModState*)data;
	cur = ggzmod_get_state(mod);

	switch (cur) {
	case GGZMOD_STATE_WAITING:
		ggz_debug(GGZCORE_DBG_GAME, "Game now waiting");
		_ggzcore_game_event(game, GGZ_GAME_NEGOTIATED, NULL);
		break;

	case GGZMOD_STATE_PLAYING:
		ggz_debug(GGZCORE_DBG_GAME, "Game now playing");
		_ggzcore_game_event(game, GGZ_GAME_PLAYING, NULL);
		break;
		
	case GGZMOD_STATE_DONE:
		ggz_debug(GGZCORE_DBG_GAME, "Game now done");
		/*_ggzcore_game_event(game, GGZ_GAME_OVER, NULL);*/
		break;

	default:
		ggz_debug(GGZCORE_DBG_GAME, "Game now in state %d", cur);
		break;

	}
}


void _ggzcore_game_free(struct _GGZGame *game)
{
	int i;

	ggz_debug(GGZCORE_DBG_GAME, "Destroying game object");

	ggzmod_disconnect(game->client);
	ggzmod_free(game->client);

	for (i = 0; i < _ggzcore_num_events; i++)
		_ggzcore_hook_list_destroy(game->event_hooks[i]);

	_ggzcore_server_set_cur_game(game->server, NULL);

	ggz_free(game);
}

void _ggzcore_game_set_table(GGZGame *game, int room_num, int table_num)
{
	GGZRoom *room;
	GGZTable *table;
	int num_seats, i;

	/* FIXME */
	assert(game->room_num == room_num);
	assert(game->table_num < 0);

	room = ggzcore_server_get_cur_room(game->server);
	assert(_ggzcore_room_get_id(room) == room_num);

	game->table_num = table_num;
	table = ggzcore_room_get_nth_table(room, table_num);

	num_seats = _ggzcore_table_get_num_seats(table);
	for (i = 0; i < num_seats; i++) {
		struct _GGZSeat *seat;
		GGZSeat mseat;
		seat = _ggzcore_table_get_nth_seat(table, i);
		mseat.num = seat->index;
		mseat.type = seat->type;
		mseat.name = seat->name;
		ggzmod_set_seat(game->client, &mseat);
	}
}


void _ggzcore_game_set_seat(GGZGame *game, struct _GGZSeat *seat)
{
	GGZSeat mseat = {num: seat->index,
			 type: seat->type,
			 name: seat->name};
	ggzmod_set_seat(game->client, &mseat);
}

void _ggzcore_game_set_spectator_seat(GGZGame *game, struct _GGZSeat *seat)
{
	GGZSpectatorSeat mseat = {num: seat->index,
				  name: seat->name};
	ggzmod_set_spectator_seat(game->client, &mseat);
}

void _ggzcore_game_set_player(GGZGame *game, int is_spectator, int seat_num)
{
	if (game->spectating == is_spectator && game->seat_num == seat_num)
		return;

	game->spectating = is_spectator;
	game->seat_num = seat_num;

	if (ggzmod_set_player(game->client,
			      _ggzcore_server_get_handle(game->server),
			      is_spectator, seat_num) < 0)
		assert(0);
}


int _ggzcore_game_is_spectator(GGZGame *game)
{
	return game->spectating;
}


int _ggzcore_game_get_seat_num(GGZGame *game)
{
	return game->seat_num;
}


int _ggzcore_game_get_room_num(GGZGame *game)
{
	return game->room_num;
}


int _ggzcore_game_get_table_num(GGZGame *game)
{
	return game->table_num;
}


int _ggzcore_game_add_event_hook_full(struct _GGZGame *game,
				      const GGZGameEvent event, 
				      const GGZHookFunc func,
				      void *data)
{
	return _ggzcore_hook_add_full(game->event_hooks[event], func, data);
}


/* Functions for removing hooks from struct _GGZGame events */
int _ggzcore_game_remove_event_hook(struct _GGZGame *game,
				    const GGZGameEvent event, 
				    const GGZHookFunc func)
{
	return _ggzcore_hook_remove(game->event_hooks[event], func);
}


int _ggzcore_game_remove_event_hook_id(struct _GGZGame *game,
				       const GGZGameEvent event, 
				       const unsigned int hook_id)
{
	return _ggzcore_hook_remove_id(game->event_hooks[event], hook_id);
}


int _ggzcore_game_get_control_fd(struct _GGZGame *game)
{
	return ggzmod_get_fd(game->client);
}

int _ggzcore_game_read_data(struct _GGZGame *game)
{
	int status;

	status = ggzmod_dispatch(game->client);
	ggz_debug(GGZCORE_DBG_GAME, "Result of reading from game: %d", status);

	if (status < 0) {
		_ggzcore_game_event(game, GGZ_GAME_OVER, NULL);
		ggzmod_disconnect(game->client);
	}

	return status;
}


void _ggzcore_game_set_server_fd(struct _GGZGame *game, int fd)
{
	ggzmod_set_server_fd(game->client, fd);
}


struct _GGZModule* _ggzcore_game_get_module(struct _GGZGame *game)
{
	return game->module;
}


int _ggzcore_game_launch(struct _GGZGame *game)
{
	int status;

	ggz_debug(GGZCORE_DBG_GAME, "Launching game of %s",
		      _ggzcore_module_get_name(game->module));

	if ( (status = ggzmod_connect(game->client)) == 0) {
		ggz_debug(GGZCORE_DBG_GAME, "Launched game module");
		_ggzcore_game_event(game, GGZ_GAME_LAUNCHED, NULL);
	}
	else {
		ggz_debug(GGZCORE_DBG_GAME,
			  "Failed to connect to game module");
		_ggzcore_game_event(game, GGZ_GAME_LAUNCH_FAIL, NULL);
	}
		
	return status;
}


/* Static functions internal to this file */

static int _ggzcore_game_event_is_valid(GGZGameEvent event)
{
	return (event >= 0 && event < _ggzcore_num_events);
}


static GGZHookReturn _ggzcore_game_event(struct _GGZGame *game, GGZGameEvent id, void *data)
{
	return _ggzcore_hook_list_invoke(game->event_hooks[id], data);
}


#if 0 /* currently unused */
static char* _ggzcore_game_get_path(char **argv)
{
	char *mod_path;
	char *path;
	int len;

	mod_path = argv[0];
	
	if (mod_path[0] != '/') {
		ggz_debug(GGZCORE_DBG_GAME,
			  "Module has relative path, prepending gamedir");
		/* Calcualate string length, leaving room for a slash 
		   and the trailing null */
		len = strlen(GAMEDIR) + strlen(mod_path) + 2;
		path = ggz_malloc(len);
		strcpy(path, GAMEDIR);
		strcat(path, "/");
		strcat(path, mod_path);
	}
	else
		path = ggz_strdup(mod_path);

	return path;
}
#endif

