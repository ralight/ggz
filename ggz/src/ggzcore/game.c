/*
 * File: game.c
 * Author: Brent Hendricks
 * Project: GGZ Core Client Lib
 * Date: 2/28/2001
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


#include "config.h"
#include "game.h"
#include "module.h"
#include "ggzcore.h"
#include "hook.h"

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
static char* _ggzcore_game_get_path(char **argv);


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

};


static void _ggzcore_game_handle_state(GGZMod *mod, GGZModEvent event, void *data);


/* Publicly exported functions */

GGZGame* ggzcore_game_new(void)
{
	return _ggzcore_game_new();
}


int ggzcore_game_init(GGZGame *game, GGZModule *module)
{
	if (game && module) {
		_ggzcore_game_init(game, module);
		return 0;
	}
	
	return -1;
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


int ggzcore_game_join(GGZGame *game)
{
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


void _ggzcore_game_init(struct _GGZGame *game, struct _GGZModule *module)
{
	int i;

     	game->module = module;

	ggz_debug("GGZCORE:GAME", "Initializing new game");

	/* Setup event hook list */
	for (i = 0; i < _ggzcore_num_events; i++)
		game->event_hooks[i] = _ggzcore_hook_list_init(i);

	/* Setup client module connection */
	game->client = ggzmod_new(GGZMOD_GGZ);
	ggzmod_set_gamedata(game->client, game);
	ggzmod_set_handler(game->client, GGZMOD_EVENT_STATE, &_ggzcore_game_handle_state);
	ggzmod_set_module(game->client, NULL, _ggzcore_module_get_argv(game->module));
}


static void _ggzcore_game_handle_state(GGZMod *mod, GGZModEvent event, void *data)
{
	GGZGame* game = ggzmod_get_gamedata(mod);
	GGZModState cur, prev;

	prev = *(GGZModState*)data;
	cur = ggzmod_get_state(mod);

	switch (cur) {
	case GGZMOD_STATE_WAITING:
		ggz_debug("GGZCORE:GAME", "Game now waiting");
		_ggzcore_game_event(game, GGZ_GAME_NEGOTIATED, NULL);
		break;

	case GGZMOD_STATE_PLAYING:
		ggz_debug("GGZCORE:GAME", "Game now playing");
		_ggzcore_game_event(game, GGZ_GAME_PLAYING, NULL);
		break;
		
	case GGZMOD_STATE_DONE:
		ggz_debug("GGZCORE:GAME", "Game now done");
		/*_ggzcore_game_event(game, GGZ_GAME_OVER, NULL);*/
		break;

	default:
		ggz_debug("GGZCORE:GAME", "Game now in state %d", cur);
		break;

	}
}


void _ggzcore_game_free(struct _GGZGame *game)
{
	int i;

	ggz_debug("GGZCORE:GAME", "Destroying game object");

	ggzmod_disconnect(game->client);
	ggzmod_free(game->client);

	for (i = 0; i < _ggzcore_num_events; i++)
		_ggzcore_hook_list_destroy(game->event_hooks[i]);

	ggz_free(game);
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
	ggz_debug("GGZCORE:GAME", "Result of reading from game: %d", status);

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

	ggz_debug("GGZCORE:GAME", "Launching game of %s",
		      _ggzcore_module_get_name(game->module));

	if ( (status = ggzmod_connect(game->client)) == 0) {
		ggz_debug("GGZCORE:GAME", "Launched game module");
		_ggzcore_game_event(game, GGZ_GAME_LAUNCHED, NULL);
	}
	else {
		ggz_debug("GGZCORE:GAME", "Failed to connect to game module");
		_ggzcore_game_event(game, GGZ_GAME_LAUNCH_FAIL, NULL);
	}
		
	return status;
}


int _ggzcore_game_join(struct _GGZGame *game) 
{
	return -1;
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


static char* _ggzcore_game_get_path(char **argv)
{
	char *mod_path;
	char *path;
	int len;

	mod_path = argv[0];
	
	if (mod_path[0] != '/') {
		ggz_debug("GGZCORE:GAME", "Module has relative path, prepending gamedir");
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

