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

	game = ggz_malloc(sizeof(struct _GGZGame));

	return game;
}


void _ggzcore_game_init(struct _GGZGame *game, struct _GGZModule *module)
{
	int i;

     	game->module = module;
	game->fd = -1;
	game->pid = -1;

	ggz_debug("GGZCORE:GAME", "Initializing new game");

	/* Setup event hook list */
	for (i = 0; i < _ggzcore_num_events; i++)
		game->event_hooks[i] = _ggzcore_hook_list_init(i);
}


void _ggzcore_game_free(struct _GGZGame *game)
{
	int i;

	ggz_debug("GGZCORE:GAME", "Destroying game object");

	if (game->fd != -1)
		close(game->fd);

	if (game->pid != -1)
		kill(game->pid, SIGTERM);

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


int _ggzcore_game_data_is_pending(struct _GGZGame *game)
{
	int status = 0;
	struct pollfd fd[1] = {{game->fd, POLLIN, 0}};
	
	ggz_debug("GGZCORE:POLL", "Checking for game events");	
	if ( (status = poll(fd, 1, 0)) < 0) {
		if (errno == EINTR) 
			/* Ignore interruptions */
			status = 0;
		else 
			ggz_error_sys_exit("poll failed in ggzcore_game_data_is_pending");
	}
	else if (status)
		ggz_debug("GGZCORE:POLL", "Found a game event!");

	return status;
}


int _ggzcore_game_read_data(struct _GGZGame *game)
{
	unsigned int size;
	char buf[4096];
	char *buf_offset;

	ggz_debug("GGZCORE:GAME", "Got game msg from game client");

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
	int sfd[2];
	char *path, **argv;
	struct stat file_status;

	argv = _ggzcore_module_get_argv(game->module);
	path = _ggzcore_game_get_path(argv);

	ggz_debug("GGZCORE:GAME", "Launching game of %s",
		      _ggzcore_module_get_name(game->module));
	ggz_debug("GGZCORE:GAME", "Exec path is %s", path);

	/* Check for existence of game module */
	if (stat(path, &file_status) < 0) {
		_ggzcore_game_event(game, GGZ_GAME_LAUNCH_FAIL,
				    strerror(errno));
		ggz_debug("GGZCORE:GAME", "Bad path: %s", path);
		ggz_free(path);
		return -1;
	}

	/* Set up socket pair for ggz<->game communication */
	if (socketpair(PF_LOCAL, SOCK_STREAM, 0, sfd) < 0)
		ggz_error_sys_exit("socketpair failed"); 	
	
	/* Fork table process */
	if ( (pid = fork()) < 0) {
		ggz_error_sys_exit("fork failed");
	} else if (pid == 0) {
		/* child */
		close(sfd[0]);

		/* NOTE: after we close FD 3, below, we can't send any
		 * more communications to this FD.  Although this seems
		 * to be safe with ggzcore_debug right now, I'm not
		 * going to count on it.  --JDS */

		/* libggzmod expects FD 3 to be the socket's FD */
		if(sfd[1] != 3) {
			if (dup2(sfd[1], 3) != 3)
				exit(-1);
			if (close(sfd[1]) < 0)
				exit(-1);
		}

		/* It's tempting to put this in its own function, but
		 * since we can't use debugging it wouldn't be safe. */
		execv(path, argv);

		/* If we get here, exec failed.  Bad news */
		/* we still can't write debug info here!  But, we could write
		 * game info to socket 3. */
		exit(-1);
	} else {
		/* parent */
		ggz_free(path);
		close(sfd[1]);
		
		/* Key info about game */
		game->pid = pid;
		game->fd = sfd[0];

		ggz_debug("GGZCORE:GAME", "Successful launch");
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

	if (ggz_writen(game->fd, buf_offset, size) < 0) {
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


RETSIGTYPE _ggzcore_game_dead(int sig)
{
  	pid_t pid;
	int status;
  
	pid = waitpid(WAIT_ANY, &status, WNOHANG);
	if( pid > 0 )
		ggz_debug("GGZCORE:GAME", "Game module is dead");
	else if( pid == 0 )
		ggz_error_sys("No dead children found");
	else 
		ggz_error_sys("Waitpid failure");
}
