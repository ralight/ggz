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


#include "game.h"
#include "module.h"
#include "ggzcore.h"
#include "hook.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <easysock.h>
#include <stdio.h>


/* Local functions */
static int _ggzcore_game_event_is_valid(GGZGameEvent event);
static GGZHookReturn _ggzcore_game_event(struct _GGZGame *game, 
					 GGZGameEvent id, void *data);
static void _ggzcore_game_exec(char *path);

/* Array of GGZGame messages */
static char* _ggzcore_game_events[] = {
	"GGZ_LAUNCHED",
	"GGZ_GAME_READY",
};

/* Total number of server events messages */
static unsigned int _ggzcore_num_events = sizeof(_ggzcore_game_events)/sizeof(_ggzcore_game_events[0]);


/*
 * The GGZGame object manages information about a particular running game
 */
struct _GGZGame {

	/* File descriptor for communication with game process */
	int fd;

	/* Filename used for Unix domain socket */
	char *file_name;

	/* PID of running game process */
	pid_t pid;
	
	/* Pointer to module this game is playing */
	struct _GGZModule *module;

	/* Room events */
	struct _GGZHookList *event_hooks[sizeof(_ggzcore_game_events)/sizeof(_ggzcore_game_events[0])];

};



/* Publicly exported functions */

GGZGame* ggzcore_game_new(void)
{
	return _ggzcore_game_new();
}


int ggzcore_game_init(GGZGame *game, GGZModule *module)
{
	if (!game)
		return -1;

	_ggzcore_game_init(game, module);
	return 0;
}


void ggzcore_game_free(GGZGame *game)
{
	if (!game)
		_ggzcore_game_free(game);
}


/* Functions for attaching hooks to GGZGame events */
int ggzcore_game_add_event_hook(GGZGame *game,
				const GGZGameEvent event, 
				const GGZHookFunc func)
{
	if (game && _ggzcore_game_event_is_valid(event))
		return _ggzcore_game_add_event_hook_full(game, event, func, NULL);
	else
		return -1;
}


int ggzcore_game_add_event_hook_full(GGZGame *game,
				     const GGZGameEvent event, 
				     const GGZHookFunc func,
				     void *data)
{
	if (game && _ggzcore_game_event_is_valid(event))
		return _ggzcore_game_add_event_hook_full(game, event, func, data);
	else
		return -1;
}


/* Functions for removing hooks from GGZGame events */
int ggzcore_game_remove_event_hook(GGZGame *game,
				   const GGZGameEvent event, 
				   const GGZHookFunc func)
{
	if (game && _ggzcore_game_event_is_valid(event))
		return _ggzcore_game_remove_event_hook(game, event, func);
	else 
		return -1;
}


int ggzcore_game_remove_event_hook_id(GGZGame *game,
				      const GGZGameEvent event, 
				      const unsigned int hook_id)
{
	if (game && _ggzcore_game_event_is_valid(event))
		return _ggzcore_game_remove_event_hook_id(game, event, hook_id);
	else
		return -1;
}


int ggzcore_game_get_fd(GGZGame *game)
{
	if (game)
		return _ggzcore_game_get_fd(game);
	else
		return -1;
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
	if (game)
		return _ggzcore_game_launch(game);
	else
		return -1;
}


int ggzcore_game_join(GGZGame *game);


/* 
 * Internal library functions (prototypes in room.h) 
 * NOTE:All of these functions assume valid inputs!
 */


struct _GGZGame* _ggzcore_game_new(void)
{
	struct _GGZGame *game;

	game = ggzcore_malloc(sizeof(struct _GGZGame));

	return game;
}


void _ggzcore_game_init(struct _GGZGame *game, struct _GGZModule *module)
{
	int i;

     	game->module = module;

	ggzcore_debug(GGZ_DBG_GAME, "Initializing new game");

	/* Setup event hook list */
	for (i = 0; i < _ggzcore_num_events; i++)
		game->event_hooks[i] = _ggzcore_hook_list_init(i);
}


void _ggzcore_game_free(struct _GGZGame *game)
{
	int i;

	if (game->file_name)
		ggzcore_free(game->file_name);

	for (i = 0; i < _ggzcore_num_events; i++)
		_ggzcore_hook_list_destroy(game->event_hooks[i]);


	ggzcore_free(game);
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


int _ggzcore_game_get_fd(struct _GGZGame *game)
{
	return game->fd;
}


struct _GGZModule* _ggzcore_game_get_module(struct _GGZGame *game)
{
	return game->module;
}


int _ggzcore_game_launch(struct _GGZGame *game)
{
	pid_t pid;
	int fd, sock_accept;
	char *name, *path;
	struct stat file_status;
	
	name = _ggzcore_module_get_game(game->module);
	path = _ggzcore_module_get_path(game->module);

	ggzcore_debug(GGZ_DBG_GAME, "Launching game of %s", name);
	ggzcore_debug(GGZ_DBG_GAME, "Exec path is  %s", path);

	if (stat(path, &file_status) < 0) {
		ggzcore_debug(GGZ_DBG_GAME, "Bad path: %s", path);
		return -1;
	}
	
	
	/* Fork table process */
	if ( (pid = fork()) < 0) {
		ggzcore_error_sys_exit("fork failed");
	} else if (pid == 0) {
		_ggzcore_game_exec(path);
		/* If we get here, exec failed.  Bad news */
		ggzcore_error_sys_exit("exec failed");
	} else {
		/* Create Unix domain socket for communication*/

		/* Leave room for "/tmp/" + PID + '\0' */
		game->file_name = ggzcore_malloc(strlen(name + 12));
		sprintf(game->file_name, "/tmp/%s.%d", name, pid);
		
		sock_accept = es_make_unix_socket(ES_SERVER, game->file_name);
		if (sock_accept < 0) {
			ggzcore_debug(GGZ_DBG_GAME, "Failed to create Unix socket");
			return -1;
		}

		if (listen(sock_accept, 1) < 0)
			ggzcore_error_sys_exit("listen falied");

		if ( (fd = accept(sock_accept, NULL, NULL)) < 0)
			ggzcore_error_sys_exit("accept failed");
		
		/* Key info about game */
		game->pid = pid;
		game->fd = fd;

		ggzcore_debug(GGZ_DBG_GAME, "Successful launch");
		_ggzcore_game_event(game, GGZ_LAUNCHED, NULL);
	}

	return 0;
}


int _ggzcore_game_join(struct _GGZGame *game);


/* Static functions internal to this file */

static int _ggzcore_game_event_is_valid(GGZGameEvent event)
{
	return (event >= 0 && event < _ggzcore_num_events);
}


static GGZHookReturn _ggzcore_game_event(struct _GGZGame *game, GGZGameEvent id, void *data)
{
	return _ggzcore_hook_list_invoke(game->event_hooks[id], data);
}


static void _ggzcore_game_exec(char *path)
{
	ggzcore_debug(GGZ_DBG_GAME, "Process forked.  Game running");
	
	/* FIXME: Maybe pass over sock, rather than cmd-line? */
	execl(path, path, "-o", NULL);
	
#if 0
	if (flag)
		execl(path, g_basename(path), "-o", NULL);
	else
		execl(path, g_basename(path), NULL);
#endif
}





