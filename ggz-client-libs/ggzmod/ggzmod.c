/* 
 * File: ggzmod.c
 * Author: GGZ Dev Team
 * Project: ggzmod
 * Date: 10/14/01
 * Desc: GGZ game module functions
 * $Id: ggzmod.c 4340 2002-08-07 06:31:34Z jdorje $
 *
 * This file contains the backend for the ggzmod library.  This
 * library facilitates the communication between the GGZ server (ggz)
 * and game servers.  This file provides backend code that can be
 * used at both ends.
 *
 * Copyright (C) 2001-2002 GGZ Development Team.
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
#  include <config.h>			/* Site-specific config */
#endif

#include <errno.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <ggz.h>

#include "ggzmod.h"
#include "mod.h"
#include "io.h"
#include "protocol.h"


/* 
 * internal function prototypes
 */

static void call_handler(GGZMod *ggzmod, GGZModEvent event, void *data);
static int _ggzmod_handle_event(GGZMod * ggzmod, fd_set read_fds);
static void _ggzmod_set_state(GGZMod * ggzmod, GGZModState state);
static int send_game_launch(GGZMod * ggzmod);
static int game_fork(GGZMod * ggzmod);

/* 
 * Creating/destroying a ggzmod object
 */

/* Creates a new ggzmod object. */
GGZMod *ggzmod_new(GGZModType type)
{
	int i;
	GGZMod *ggzmod;

	/* verify parameter */
	if (type != GGZMOD_GGZ && type != GGZMOD_GAME)
		return NULL;

	/* allocate */
	ggzmod = ggz_malloc(sizeof(*ggzmod));

	/* initialize */
	ggzmod->type = type;
	ggzmod->state = GGZMOD_STATE_CREATED;
	ggzmod->fd = -1;
	ggzmod->server_fd = -1;
	for (i = 0; i < GGZMOD_NUM_HANDLERS; i++)
		ggzmod->handlers[i] = NULL;
	ggzmod->gamedata = NULL;

	ggzmod->pid = -1;
	ggzmod->argv = NULL;

	return ggzmod;
}


/* Frees (deletes) a ggzmod object */
void ggzmod_free(GGZMod * ggzmod)
{
	int i;

	if (!ggzmod) {
		return;
	}
	
	if (ggzmod->fd != -1)
		(void)ggzmod_disconnect(ggzmod);
	
	ggzmod->type = -1;

	if (ggzmod->pwd)
		ggz_free(ggzmod->pwd);
	
	if (ggzmod->argv) {
		for (i = 0; ggzmod->argv[i]; i++)
			if (ggzmod->argv[i])
				ggz_free(ggzmod->argv[i]);
		ggz_free(ggzmod->argv);
	}

	/* Free the object */
	ggz_free(ggzmod);
}


/* 
 * Accesor functions for GGZMod
 */

/* The ggzmod FD is the main ggz<->game server communications socket. */
int ggzmod_get_fd(GGZMod * ggzmod)
{
	if (!ggzmod) {
		return -1;
	}
	return ggzmod->fd;
}


GGZModType ggzmod_get_type(GGZMod * ggzmod)
{
	if (!ggzmod) {
		return -1;	/* not very useful */
	}
	return ggzmod->type;
}


GGZModState ggzmod_get_state(GGZMod * ggzmod)
{
	if (!ggzmod) {
		return -1;	/* not very useful */
	}
	return ggzmod->state;
}


void* ggzmod_get_gamedata(GGZMod * ggzmod)
{
	if (!ggzmod) {
		return NULL;
	}
	return ggzmod->gamedata;
}


void ggzmod_set_module(GGZMod * ggzmod, const char *pwd, char **argv)
{
	int i;

	ggz_debug("GGZMOD", "Setting arguments");
	
	if (!ggzmod)
		return;

	if (ggzmod->type != GGZMOD_GGZ) {
		_ggzmod_error(ggzmod, "Cannot set module args from module");
		return;
	}
		
	/* Check parameters */
	if (!argv || !argv[0]) {
		_ggzmod_error(ggzmod, "Bad module arguments");
		return;
	}

	/* Count the number of args so we know how much to allocate */
	for (i = 0; argv[i]; i++) {}

	ggz_debug("GGZMOD", "Set %d arguments", i);
	
	ggzmod->argv = ggz_malloc(sizeof(char*)*(i+1));
	ggzmod->pwd = ggz_strdup(pwd);
	
	for (i = 0; argv[i]; i++) 
		ggzmod->argv[i] = ggz_strdup(argv[i]);
}


void ggzmod_set_gamedata(GGZMod * ggzmod, void * data)
{
	if (ggzmod)
		ggzmod->gamedata = data;
}


void ggzmod_set_server_fd(GGZMod * ggzmod, int fd)
{
	if (ggzmod && ggzmod->type == GGZMOD_GGZ) {
		ggzmod->server_fd = fd;

		/* If we're already connected, send the fd */
		if (ggzmod->state == GGZMOD_STATE_WAITING)
			_io_send_server(ggzmod->fd, fd);
	}
}


void ggzmod_set_handler(GGZMod * ggzmod, GGZModEvent e,
			 GGZModHandler func)
{
	if (!ggzmod || e < 0 || e >= GGZMOD_NUM_HANDLERS) {
		return;		/* not very useful */
	}

	ggzmod->handlers[e] = func;
}


int ggzmod_set_state(GGZMod * ggzmod, GGZModState state)
{
	if (!ggzmod)
		return -1;
	
	if (ggzmod->type == GGZMOD_GAME) {
		/* The game may only change the state from one of
		   these two states. */
		if (ggzmod->state != GGZMOD_STATE_WAITING &&
		    ggzmod->state != GGZMOD_STATE_PLAYING)
			return -1;

		/* The game may only change the state to one of
		   these three states. */
		if (state == GGZMOD_STATE_PLAYING ||
		    state == GGZMOD_STATE_WAITING ||
		    state == GGZMOD_STATE_DONE)
			_ggzmod_set_state(ggzmod, state);
		else
			return -1;
	} else {
		/* TODO: an extension to the communications protocol will be
		   needed for this to work ggz-side.  Let's get the rest
		   of it working first... */
		return -1;
	}
	return 0;
}


/* 
 * GGZmod actions
 */

int ggzmod_connect(GGZMod * ggzmod)
{
	if (!ggzmod)
		return -1;

	if (ggzmod->type == GGZMOD_GGZ) {
		/* For the ggz side, we fork the game and then send the launch message */
		
		if (game_fork(ggzmod) < 0) {
			_ggzmod_error(ggzmod, "Error: table fork failed");
			return -1;
		}
		
		if (send_game_launch(ggzmod) < 0) {
			_ggzmod_error(ggzmod, "Error sending launch to game");
			return -1;
		}

	} else
		ggzmod->fd = 3;
	
	return 0;
}


int ggzmod_dispatch(GGZMod * ggzmod)
{
	struct timeval timeout;
	fd_set read_fd_set;
	int status;

	if (!ggzmod)
		return -1;

	if (ggzmod->fd < 0)
		return -1;

	FD_ZERO(&read_fd_set);
	FD_SET(ggzmod->fd, &read_fd_set);

	timeout.tv_sec = timeout.tv_usec = 0;	/* is this really portable? */
	
	status = select(ggzmod->fd + 1, &read_fd_set, NULL, NULL, &timeout);
	
	if (status <= 0) {
		if (errno == EINTR)
			return 0;
		return -1;
	}
	
	return _ggzmod_handle_event(ggzmod, read_fd_set);
}


int ggzmod_disconnect(GGZMod * ggzmod)
{
	if (!ggzmod || ggzmod->fd < 0 ) {
		return -1;
	}

	if (ggzmod->type == GGZMOD_GGZ) {
		/* For the ggz side, we kill the game server and close the socket */
		
		/* Make sure game server is dead */
		if (ggzmod->pid > 0)
			kill(ggzmod->pid, SIGINT);
		ggzmod->pid = -1;
		
		_ggzmod_set_state(ggzmod, GGZMOD_STATE_DONE);
		/* FIXME: should we wait() to get an exit status?? */
		/* FIXME: what other cleanups should we do? */
	} else {
		/* For client the game side we send a game over message */
		
		/* First warn the server of halt (if we haven't already) */
		_ggzmod_set_state(ggzmod, GGZMOD_STATE_DONE);
		ggz_debug("GGZMOD", "Disconnected from GGZ server.");
	}
	
	/* We no longer free the seat data here.  It will stick around until
	   ggzmod_free is called or it is used again. */

	/* Clean up the ggzmod object.  In theory it could now reconnect for
	   a new game. */
	close(ggzmod->fd);
	ggzmod->fd = -1;

	return 0;
}



/* Returns -1 on error, the number of events handled on success. */
static int _ggzmod_handle_event(GGZMod * ggzmod, fd_set read_fds)
{
	int status = 0;
	
	if (FD_ISSET(ggzmod->fd, &read_fds)) {
		status = _io_read_data(ggzmod);
		if (status < 0) {
			_ggzmod_error(ggzmod, "Error reading data");
			/* FIXME: should be disconnect? */
			_ggzmod_set_state(ggzmod, GGZMOD_STATE_DONE);
		}
	}

	return status;
}


static void _ggzmod_set_state(GGZMod * ggzmod, GGZModState state)
{
	GGZModState old_state = ggzmod->state;
	if (state == ggzmod->state)
		return;		/* Is this an error? */

	/* The callback function retrieves the state from ggzmod_get_state.
	   It could instead be passed as an argument. */
	ggzmod->state = state;
	call_handler(ggzmod, GGZMOD_EVENT_STATE, &old_state);

	/* If we are the game module, send the new state to GGZ */
	if (ggzmod->type == GGZMOD_GAME) {
		ggz_debug("GGZMOD", "Game setting state to %d", 
			    state);
		if (_io_send_state(ggzmod->fd, state) < 0)
			/* FIXME: do some sort of error handling? */
			return;
	}
}




/* 
 * ggz specific actions
 */

/* Sends a game launch packet to ggzmod-game. A negative return value
   indicates a serious (fatal) error. */
static int send_game_launch(GGZMod * ggzmod)
{
	if (_io_send_launch(ggzmod->fd) < 0) {
		_ggzmod_error(ggzmod, "Error writing to game");
		return -1;
	}

	/* If the server fd has already been set, send that too */
	if (ggzmod->server_fd != -1) {
		if (_io_send_server(ggzmod->fd, ggzmod->server_fd) < 0) {
			_ggzmod_error(ggzmod, "Error writing to game");
			return -1;
		}
	}

	return 0;
}


/* Forks the game.  A negative return value indicates a serious error. */
/* No locking should be necessary within this function. */
static int game_fork(GGZMod * ggzmod)
{
	int pid;
	int fd_pair[2];		/* socketpair */

	/* If there are no args, we don't know what to run! */
	if (ggzmod->argv == NULL || ggzmod->argv[0] == NULL) {
		_ggzmod_error(ggzmod, "No arguments");
		return -1;
	}

	if (socketpair(PF_LOCAL, SOCK_STREAM, 0, fd_pair) < 0)
		ggz_error_sys_exit("socketpair failed");

	if ( (pid = fork()) < 0)
		ggz_error_sys_exit("fork failed");
	else if (pid == 0) {
		/* child */
		close(fd_pair[0]);

		/* debugging message??? */

		/* Now we copy one end of the socketpair to fd 3 */
		if (fd_pair[1] != 3) {
			/* We'd like to send an error message if either of
			   these fail, but we can't.  --JDS */
			if (dup2(fd_pair[1], 3) != 3 || close(fd_pair[1]) < 0)
				ggz_error_sys_exit("dup failed");
		}

		/* FIXME: Close all other fd's? */
		/* FIXME: Not necessary to close other fd's if we use
		   CLOSE_ON_EXEC */

		/* Set working directory */
		if (ggzmod->pwd
		    && chdir(ggzmod->pwd) < 0) {
			/* FIXME: what to do? */
		}

		/* FIXME: can we call ggzmod_log() from here? */
		execv(ggzmod->argv[0], ggzmod->argv);	/* run game */

		/* We should never get here.  If we do, it's an eror */
		/* we still can't send error messages... */
		ggz_error_sys_exit("exec of %s failed", ggzmod->argv[0]);
	} else {
		/* parent */
		close(fd_pair[1]);

		ggzmod->fd = fd_pair[0];
		ggzmod->pid = pid;
		
		/* FIXME: should we delete the argv arguments? */
		
		/* That's all! */
	}
	return 0;
}


/**** Internal library functions ****/

/* Invokes handlers for the specified event */
static void call_handler(GGZMod *ggzmod, GGZModEvent event, void *data)
{
	if (ggzmod->handlers[event])
		(*ggzmod->handlers[event]) (ggzmod, event, data);
}


void _ggzmod_error(GGZMod *ggzmod, char* error)
{
	call_handler(ggzmod, GGZMOD_EVENT_ERROR, error);
}


void _ggzmod_handle_state(GGZMod * ggzmod, GGZModState state)
{
	/* There's only certain ones the game is allowed to set it to,
	   and they can only change it if the state is currently
	   WAITING or PLAYING. */
	switch (state) {
	case GGZMOD_STATE_WAITING:
	case GGZMOD_STATE_PLAYING:
	case GGZMOD_STATE_DONE:
		/* In contradiction to what I say above, the game
		   actually _is_ allowed to change its state from
		   CREATED to WAITING.  When ggzmod-ggz sends a
		   launch packet to ggzmod-game, ggzmod-game
		   automatically changes the state from CREATED
		   to WAITING.  When this happens, it tells
		   ggzmod-ggz of this change and we end up back
		   here.  So, although it's a bit unsafe, we have
		   to allow this for now.  The alternative would
		   be to have ggzmod-ggz and ggzmod-game both
		   separately change states when the launch packet
		   is sent. */
		_ggzmod_set_state(ggzmod, state);
		break;
	default:
		_ggzmod_error(ggzmod,
			       "Game requested incorrect state value");
	}
	
	/* Is this right? has the gameover happened yet? */   
}


void _ggzmod_handle_launch(GGZMod * ggzmod)
{
	/* Normally we let the game control its own state, but
	   we control the transition from CREATED to WAITING. */
        _ggzmod_set_state(ggzmod, GGZMOD_STATE_WAITING);
}


void _ggzmod_handle_server(GGZMod * ggzmod, int fd)
{
	ggzmod->server_fd = fd;
	call_handler(ggzmod, GGZMOD_EVENT_SERVER, &fd);
}
