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
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <easysock.h>
#include <stdio.h>
#include <signal.h>


/* Local functions */
static int _ggzcore_game_event_is_valid(GGZGameEvent event);
static GGZHookReturn _ggzcore_game_event(struct _GGZGame *game, 
					 GGZGameEvent id, void *data);
static char* _ggzcore_game_get_path(char **argv);
static void _ggzcore_game_exec(char *path, char **argv);


/* Array of GGZGame messages */
static char* _ggzcore_game_events[] = {
	"GGZ_GAME_LAUNCHED",
	"GGZ_GAME_LAUNCH_FAIL",
	"GGZ_GAME_NEGOTIATED",
	"GGZ_GAME_NEGOTIATE_FAIL",
	"GGZ_GAME_DATA",
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


int ggzcore_game_data_is_pending(GGZGame *game)
{
	if (game && game->fd != -1)
		return _ggzcore_game_data_is_pending(game);
	else
		return 0;
}


int ggzcore_game_read_data(GGZGame *game)
{
	if (game && game->fd != -1)
		return _ggzcore_game_read_data(game);
	else
		return -1;
}
		

int ggzcore_game_write_data(GGZGame *game)
{
	if (game && game->fd != -1)
		return _ggzcore_game_write_data(game);
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
	if (game && game->module)
		return _ggzcore_game_launch(game);
	else
		return -1;
}


int ggzcore_game_join(GGZGame *game)
{
	return -1;
}


int ggzcore_game_send_data(GGZGame *game, char *buffer)
{
	if (game && buffer)
		return _ggzcore_game_send_data(game, buffer);
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

	game = ggzcore_malloc(sizeof(struct _GGZGame));

	return game;
}


void _ggzcore_game_init(struct _GGZGame *game, struct _GGZModule *module)
{
	int i;

     	game->module = module;
	game->fd = -1;
	game->pid = -1;

	ggzcore_debug(GGZ_DBG_GAME, "Initializing new game");

	/* Setup event hook list */
	for (i = 0; i < _ggzcore_num_events; i++)
		game->event_hooks[i] = _ggzcore_hook_list_init(i);
}


void _ggzcore_game_free(struct _GGZGame *game)
{
	int i;

	ggzcore_debug(GGZ_DBG_GAME, "Destroying game object");
	if (game->file_name)
		ggzcore_free(game->file_name);

	if (game->fd != -1)
		close(game->fd);

	if (game->pid != -1)
		kill(game->pid, SIGTERM);

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


int _ggzcore_game_data_is_pending(struct _GGZGame *game)
{
	int status = 0;
	struct pollfd fd[1] = {{game->fd, POLLIN, 0}};
	
	ggzcore_debug(GGZ_DBG_POLL, "Checking for game events");	
	if ( (status = poll(fd, 1, 0)) < 0) {
		if (errno == EINTR) 
			/* Ignore interruptions */
			status = 0;
		else 
			ggzcore_error_sys_exit("poll failed in ggzcore_game_data_is_pending");
	}
	else if (status)
		ggzcore_debug(GGZ_DBG_POLL, "Found a game event!");

	return status;
}


int _ggzcore_game_read_data(struct _GGZGame *game)
{
	unsigned int size;
	char buf[4096];
	char *buf_offset;

	ggzcore_debug(GGZ_DBG_GAME, "Got game msg from game client");

	/* Leave room for storing 'size' in the first buf_offset bytes */
	buf_offset = buf + sizeof(size);
	size = read(game->fd, buf_offset, 4096); /* FIXME: check for error */


	/* If there's actual data */
	if (size > 0) {
		*(int*)buf = size;
		_ggzcore_game_event(game, GGZ_GAME_DATA, buf);
	}

	/* If the socket gets closed */
	if (size == 0) {
		_ggzcore_game_event(game, GGZ_GAME_OVER, NULL);
		close(game->fd);
	}

	/* Socket error */
	if (size < 0) {
		_ggzcore_game_event(game, GGZ_GAME_IO_ERROR, NULL);
		close(game->fd);
	}

	return 0;
}


int _ggzcore_game_write_data(struct _GGZGame *game)
{
	return 0;
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
	char *name, *path, **argv;
	struct stat file_status;
	
	name = _ggzcore_module_get_game(game->module);
	argv = _ggzcore_module_get_argv(game->module);
	path = _ggzcore_game_get_path(argv);

	/* Leave room for "/tmp/" + 6 digit PID + '\0' */
	game->file_name = ggzcore_malloc(strlen(name) + 12);

	ggzcore_debug(GGZ_DBG_GAME, "Launching game of %s", name);
	ggzcore_debug(GGZ_DBG_GAME, "Exec path is %s", path);

	/* Check for existence of game module */
	if (stat(path, &file_status) < 0) {
		_ggzcore_game_event(game, GGZ_GAME_LAUNCH_FAIL,
				    strerror(errno));
		ggzcore_debug(GGZ_DBG_GAME, "Bad path: %s", path);
		ggzcore_free(path);
		return -1;
	}
	
	/* Fork table process */
	if ( (pid = fork()) < 0) {
		ggzcore_error_sys_exit("fork failed");
	} else if (pid == 0) {

		/* Wait for parent to create Unix domain socket */
		sprintf(game->file_name, "/tmp/%s.%d", name, getpid());
		while (stat(game->file_name, &file_status) < 0)
			sleep(1);

		_ggzcore_game_exec(path, argv);
		/* If we get here, exec failed.  Bad news */
		_ggzcore_game_event(game, GGZ_GAME_LAUNCH_FAIL,
				    strerror(errno));
		ggzcore_debug(GGZ_DBG_GAME, "Exec failed");
		return -1;
	} else {
		ggzcore_free(path);
		/* Create Unix domain socket for communication*/

		/* FIXME: should use more secure way of getting filename */
		/* FIXME: this will go away once we pass fd using env */
		sprintf(game->file_name, "/tmp/%s.%d", name, pid);
		sock_accept = es_make_unix_socket(ES_SERVER, game->file_name);
		if (sock_accept < 0) {
			_ggzcore_game_event(game, GGZ_GAME_LAUNCH_FAIL,
					    strerror(errno));
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
		_ggzcore_game_event(game, GGZ_GAME_LAUNCHED, NULL);
		/* FIXME: for now, launch and negotiate are one and the same */
		_ggzcore_game_event(game, GGZ_GAME_NEGOTIATED, NULL);
		
	}

	return 0;
}


int _ggzcore_game_join(struct _GGZGame *game) 
{
	return -1;
}


int _ggzcore_game_send_data(struct _GGZGame *game, char *buffer)
{
	int size;
	char *buf_offset;

	/* Extract size from first bytes of buffer */
	size = *(int*)buffer;
	buf_offset = buffer + sizeof(size);

	if (es_writen(game->fd, buf_offset, size) < 0) {
		/* FIXME: game error */
		return -1;
	}

	return 0;
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
		ggzcore_debug(GGZ_DBG_GAME, "Module has relative path, prepending gamedir");
		/* Calcualate string length, leaving room for a slash 
		   and the trailing null */
		len = strlen(GAMEDIR) + strlen(mod_path) + 2;
		path = ggzcore_malloc(len);
		strcpy(path, GAMEDIR);
		strcat(path, "/");
		strcat(path, mod_path);
	}
	else
		path = ggzcore_strdup(mod_path);

	return path;
}


static void _ggzcore_game_exec(char *path, char **argv)
{
	ggzcore_debug(GGZ_DBG_GAME, "Process forked.  Game running");
	
	/* FIXME: Maybe pass over sock, rather than cmd-line? */
	execv(path, argv);
}


RETSIGTYPE _ggzcore_game_dead(int sig)
{
  	pid_t pid;
	int status;
  
	pid = waitpid(WAIT_ANY, &status, WNOHANG);
	if( pid > 0 )
		ggzcore_debug(GGZ_DBG_GAME, "Game module is dead");
	else if( pid == 0 )
		ggzcore_error_sys("No dead children found");
	else 
		ggzcore_error_sys("Waitpid failure");
}
