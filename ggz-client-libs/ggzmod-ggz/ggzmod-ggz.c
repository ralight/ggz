/* 
 * File: ggzmod-ggz.c
 * Author: GGZ Dev Team
 * Project: ggzmod
 * Date: 10/14/01
 * Desc: GGZ game module functions, GGZ side
 * $Id: ggzmod-ggz.c 9982 2008-05-12 10:17:19Z josef $
 *
 * This file contains the backend for the ggzmod library.  This
 * library facilitates the communication between the GGZ core client (ggz)
 * and game clients.  The file provides an interface similar to ggzmod.c,
 * but a different implementation.
 *
 * Copyright (C) 2001-2002 GGZ Development Team.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>			/* Site-specific config */
#endif

#include <assert.h>
#include <errno.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef HAVE_SYS_SELECT_H
# include <sys/select.h>
#endif
#ifdef HAVE_SYS_SOCKET_H
# include <sys/socket.h>
#endif
#ifdef HAVE_WINSOCK2_H
# include <winsock2.h>
#endif
#include <sys/time.h>
#include <sys/types.h>
#ifdef HAVE_SYS_WAIT_H
# include <sys/wait.h>
#endif
#include <unistd.h>

#include <ggz.h>

#include "ggzmod-ggz.h"
#include "mod-ggz.h"
#include "io-ggz.h"
#include "protocol.h"


/* 
 * internal function prototypes
 */

static void call_handler(GGZMod *ggzmod, GGZModEvent event, void *data);
static void call_transaction(GGZMod * ggzmod, GGZModTransaction t, void *data);
static int send_game_launch(GGZMod * ggzmod);
static int game_fork(GGZMod * ggzmod);
static int game_embedded(GGZMod * ggzmod);

static GGZSeat _ggzmod_ggz_get_seat(GGZMod *ggzmod, int num);
static GGZSpectatorSeat _ggzmod_ggz_get_spectator_seat(GGZMod * ggzmod, int num);
static int _ggzmod_ggz_handle_event(GGZMod * ggzmod, fd_set read_fds);
static void _ggzmod_ggz_set_state(GGZMod * ggzmod, GGZModState state);

/* Functions for manipulating seats */
static GGZSeat* seat_copy(GGZSeat *orig);
static int seat_compare(GGZSeat *a, GGZSeat *b);
static void seat_free(GGZSeat *seat);
static GGZSpectatorSeat* spectator_seat_copy(GGZSpectatorSeat *orig);
static int spectator_seat_compare(GGZSpectatorSeat *a, GGZSpectatorSeat *b);
static void spectator_seat_free(GGZSpectatorSeat *seat);

/* 
 * Creating/destroying a ggzmod object
 */

static int stats_compare(const void *p, const void *q)
{
	const GGZStat *s_p = p, *s_q = q;

	return s_p->number - s_q->number;
}

static int infos_compare(const void *p, const void *q)
{
	const GGZPlayerInfo *s_p = p, *s_q = q;

	return s_p->num - s_q->num;
}

/*
 * How a game is launched (incomplete): see ggzmod.c.
 */

/* Creates a new ggzmod object. */
GGZMod *ggzmod_ggz_new(GGZModType type)
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
	ggzmod->server_host = NULL;
	ggzmod->server_port = 0;
	ggzmod->server_handle = NULL;
	for (i = 0; i < GGZMOD_NUM_HANDLERS; i++)
		ggzmod->handlers[i] = NULL;
	ggzmod->gamedata = NULL;
	ggzmod->my_seat_num = -1;

	ggzmod->seats = ggz_list_create((ggzEntryCompare)seat_compare,
					(ggzEntryCreate)seat_copy,
					(ggzEntryDestroy)seat_free,
					GGZ_LIST_REPLACE_DUPS);
	ggzmod->spectator_seats =
		ggz_list_create((ggzEntryCompare)spectator_seat_compare,
				(ggzEntryCreate)spectator_seat_copy,
				(ggzEntryDestroy)spectator_seat_free,
				GGZ_LIST_REPLACE_DUPS);
	ggzmod->num_seats = ggzmod->num_spectator_seats = 0;

	ggzmod->stats = ggz_list_create(stats_compare, NULL, NULL,
					GGZ_LIST_ALLOW_DUPS);
	ggzmod->spectator_stats = ggz_list_create(stats_compare, NULL, NULL,
						  GGZ_LIST_ALLOW_DUPS);

	ggzmod->infos = ggz_list_create(infos_compare, NULL, NULL,
					GGZ_LIST_ALLOW_DUPS);

#ifdef HAVE_FORK
	ggzmod->pid = -1;
#else
	ggzmod->process = INVALID_HANDLE_VALUE;
#endif
	ggzmod->argv = NULL;
	for (i = 0; i < GGZMOD_NUM_TRANSACTIONS; i++)
		ggzmod->thandlers[i] = NULL;

	return ggzmod;
}


/* Frees (deletes) a ggzmod object */
void ggzmod_ggz_free(GGZMod * ggzmod)
{
	int i;

	if (!ggzmod) {
		return;
	}

	if (ggzmod->fd != -1)
		(void)ggzmod_ggz_disconnect(ggzmod);
	
	if (ggzmod->server_host) ggz_free(ggzmod->server_host);
	if (ggzmod->server_handle) ggz_free(ggzmod->server_handle);

	ggzmod->type = -1;

	if (ggzmod->my_name)
		ggz_free(ggzmod->my_name);

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

/* The ggzmod FD is the main ggz<->game client communications socket. */
int ggzmod_ggz_get_fd(GGZMod * ggzmod)
{
	if (!ggzmod) {
		return -1;
	}
	return ggzmod->fd;
}


GGZModState ggzmod_ggz_get_state(GGZMod * ggzmod)
{
	if (!ggzmod) {
		return -1;	/* not very useful */
	}
	return ggzmod->state;
}


void* ggzmod_ggz_get_gamedata(GGZMod * ggzmod)
{
	if (!ggzmod) {
		return NULL;
	}
	return ggzmod->gamedata;
}


void ggzmod_ggz_set_module(GGZMod * ggzmod, const char *pwd, char **argv)
{
	int i;

	ggz_debug("GGZMOD", "Setting arguments");
	
	if (!ggzmod)
		return;

	if (ggzmod->type != GGZMOD_GGZ) {
		_ggzmod_ggz_error(ggzmod, "Cannot set module args from module");
		return;
	}
		
	/* Check parameters */
	if (!argv || !argv[0]) {
		_ggzmod_ggz_error(ggzmod, "Bad module arguments");
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


void ggzmod_ggz_set_gamedata(GGZMod * ggzmod, void * data)
{
	if (ggzmod)
		ggzmod->gamedata = data;
}


void ggzmod_ggz_set_server_host(GGZMod * ggzmod,
			    const char *host, unsigned int port,
			    const char *handle)
{
	if (ggzmod && ggzmod->type == GGZMOD_GGZ) {
		/* If we're already connected, send the connection info */
		if (ggzmod->state == GGZMOD_STATE_CONNECTED)
			_io_ggz_send_server(ggzmod->fd, host, port, handle);
		ggzmod->server_host = ggz_strdup(host);
		ggzmod->server_port = port;
		ggzmod->server_handle = ggz_strdup(handle);
	}
}


void ggzmod_ggz_set_server_fd(GGZMod * ggzmod, int fd)
{
	if (ggzmod && ggzmod->type == GGZMOD_GGZ) {
		/* If we're already connected, send the fd */
		if (ggzmod->state == GGZMOD_STATE_CONNECTED)
			_io_ggz_send_server_fd(ggzmod->fd, fd);
	}
}


void ggzmod_ggz_set_handler(GGZMod * ggzmod, GGZModEvent e,
			 GGZModHandler func)
{
	if (!ggzmod || e < 0 || e >= GGZMOD_NUM_HANDLERS) {
		ggz_error_msg("ggzmod_ggz_set_handler: "
			      "invalid params");
		return;		/* not very useful */
	}

	ggzmod->handlers[e] = func;
}


void ggzmod_ggz_set_transaction_handler(GGZMod * ggzmod,
				    GGZModTransaction t,
				    GGZModTransactionHandler func)
{
	if (!ggzmod
	    || t < 0 || t >= GGZMOD_NUM_TRANSACTIONS
	    || ggzmod->type != GGZMOD_GGZ) {
		ggz_error_msg("ggzmod_ggz_set_transaction_handler: "
			      "invalid params");
		return;
	}

	ggzmod->thandlers[t] = func;
}

static void _ggzmod_ggz_set_player(GGZMod *ggzmod,
			       const char *name,
			       int is_spectator, int seat_num)
{
	if (ggzmod->my_name)
		ggz_free(ggzmod->my_name);
	ggzmod->my_name = ggz_strdup(name);

	ggzmod->i_am_spectator = is_spectator;
	ggzmod->my_seat_num = seat_num;
}

int ggzmod_ggz_set_player(GGZMod *ggzmod, const char *name,
		      int is_spectator, int seat_num)
{
	if (!ggzmod
	    || ggzmod->type != GGZMOD_GGZ)
		return -1;

	_ggzmod_ggz_set_player(ggzmod, name, is_spectator, seat_num);

	if (ggzmod->state != GGZMOD_STATE_CREATED)
		_io_ggz_send_player(ggzmod->fd, name, is_spectator, seat_num);

	return 0;
}

/*
 * Seats and spectator seats.
 */

static GGZSeat *seat_copy(GGZSeat *orig)
{
	GGZSeat *seat;

	seat = ggz_malloc(sizeof(*seat));

	seat->num = orig->num;
	seat->type = orig->type;
	seat->name = ggz_strdup(orig->name);

	return seat;
}

static int seat_compare(GGZSeat *a, GGZSeat *b)
{
	return a->num - b->num;
}

static void seat_free(GGZSeat *seat)
{
	if (seat->name)
		ggz_free(seat->name);
	ggz_free(seat);
}

static GGZSpectatorSeat* spectator_seat_copy(GGZSpectatorSeat *orig)
{
	GGZSpectatorSeat *seat;

	seat = ggz_malloc(sizeof(*seat));

	seat->num = orig->num;
	seat->name = ggz_strdup(orig->name);

	return seat;
}

static int spectator_seat_compare(GGZSpectatorSeat *a, GGZSpectatorSeat *b)
{
	return a->num - b->num;
}

static void spectator_seat_free(GGZSpectatorSeat *seat)
{
	if (seat->name)
		ggz_free(seat->name);

	ggz_free(seat);
}

static void _ggzmod_ggz_set_seat(GGZMod *ggzmod, GGZSeat *seat)
{
	if (seat->num >= ggzmod->num_seats)
		ggzmod->num_seats = seat->num + 1;
	ggz_list_insert(ggzmod->seats, seat);
}

static GGZSeat _ggzmod_ggz_get_seat(GGZMod *ggzmod, int num)
{
	GGZSeat seat = {
		.num = num,
		.type = GGZ_SEAT_NONE,
		.name = NULL
	};

	if (num >= 0 && num < ggzmod->num_seats) {
		GGZListEntry *entry;
		entry = ggz_list_search(ggzmod->seats, &seat);
		if (entry)
			seat = *(GGZSeat*)ggz_list_get_data(entry);
	}

	return seat;
}

int ggzmod_ggz_set_seat(GGZMod * ggzmod, GGZSeat *seat)
{
	GGZSeat oldseat;

	if (ggzmod->type == GGZMOD_GAME)
		return -1;

	if (!seat || seat->num < 0) {
		return -2;
	}

	/* If there is no such seat, return error */
	oldseat = _ggzmod_ggz_get_seat(ggzmod, seat->num);

	if (oldseat.type == seat->type
	    && ggz_strcmp(oldseat.name, seat->name) == 0) {
		/* No change. */
		return 0;
	}

	if (ggzmod->state != GGZMOD_STATE_CREATED) {
		if (_io_ggz_send_seat(ggzmod->fd, seat) < 0)
			_ggzmod_ggz_error(ggzmod, "Error writing to game");
	}

	_ggzmod_ggz_set_seat(ggzmod, seat);

	return 0;
}

static void _ggzmod_ggz_set_spectator_seat(GGZMod * ggzmod, GGZSpectatorSeat *seat)
{
	if (seat->name) {
		if (seat->num >= ggzmod->num_spectator_seats)
			ggzmod->num_spectator_seats = seat->num + 1;
		ggz_list_insert(ggzmod->spectator_seats, seat);
	} else {
		/* Non-occupied seats are just empty entries in the list. */
		GGZListEntry *entry = ggz_list_search(ggzmod->spectator_seats,
						      seat);
		ggz_list_delete_entry(ggzmod->spectator_seats, entry);

		/* FIXME: reduce num_spectator_seats */
	}
}

static GGZSpectatorSeat _ggzmod_ggz_get_spectator_seat(GGZMod * ggzmod, int num)
{
	GGZSpectatorSeat seat = {
		.num = num,
		.name = NULL
	};

	if (num >= 0 && num < ggzmod->num_spectator_seats) {
		GGZListEntry *entry;
		entry = ggz_list_search(ggzmod->spectator_seats, &seat);
		if (entry)
		  seat = *(GGZSpectatorSeat*)ggz_list_get_data(entry);
	}

	return seat;
}

int ggzmod_ggz_set_spectator_seat(GGZMod * ggzmod, GGZSpectatorSeat *seat)
{
	if (!seat) return -1;
	if (ggzmod->type == GGZMOD_GAME) return -2;
	if (seat->num < 0) return -3;

	if (ggzmod->state != GGZMOD_STATE_CREATED) {
		GGZSpectatorSeat old_seat;
		old_seat = _ggzmod_ggz_get_spectator_seat(ggzmod, seat->num);
		if (ggz_strcmp(seat->name, old_seat.name)
		    && _io_ggz_send_spectator_seat(ggzmod->fd, seat) < 0) {
			_ggzmod_ggz_error(ggzmod, "Error writing to game");
			return -4;
		}
	}

	_ggzmod_ggz_set_spectator_seat(ggzmod, seat);

	return 0;
}

int ggzmod_ggz_inform_chat(GGZMod * ggzmod, const char *player, const char *msg)
{
	if (_io_ggz_send_msg_chat(ggzmod->fd, player, msg) < 0) {
		return -1;
	}
	return 0;
}


/* 
 * GGZmod actions
 */

int ggzmod_ggz_connect(GGZMod * ggzmod)
{
	if (!ggzmod)
		return -1;

	if (ggzmod->type == GGZMOD_GGZ) {
		/* For the ggz side, we fork the game and then send the launch message */
		
		if (ggzmod->argv) {
			if (game_fork(ggzmod) < 0) {
				_ggzmod_ggz_error(ggzmod, "Error: table fork failed");
				return -1;
			}
		} else {
			ggz_debug("GGZMOD", "Running embedded game (no fork)");
			if (game_embedded(ggzmod) < 0) {
				_ggzmod_ggz_error(ggzmod, "Error: embedded table failed");
				return -1;
			}
		}
		
		if (send_game_launch(ggzmod) < 0) {
			_ggzmod_ggz_error(ggzmod, "Error sending launch to game");
			return -1;
		}
	}
	
	return 0;
}


int ggzmod_ggz_dispatch(GGZMod * ggzmod)
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

	if (status == 0) {
		/* Nothing to read. */
		return 0;
	} else if (status < 0) {
		if (errno == EINTR)
			return 0;
		return -1;
	}
	
	return _ggzmod_ggz_handle_event(ggzmod, read_fd_set);
}

int ggzmod_ggz_disconnect(GGZMod * ggzmod)
{
	if (!ggzmod) {
		return -1;
	}
	if (ggzmod->fd < 0) {
		/* This isn't an error; it usually means
		   we already disconnected.  The invariant is that the
		   process (ggzmod->pid) exists iff the socket (ggzmod->fd)
		   exists. */
		return 0;
	}

	if (ggzmod->type == GGZMOD_GGZ) {
		/* For the ggz side, we kill the game client and close the socket */
		
#ifdef HAVE_KILL
		/* Make sure game client is dead */
		if (ggzmod->pid > 0) {
			kill(ggzmod->pid, SIGINT);
			/* This will block waiting for the child to exit.
			   This could be a problem if there is an error
			   (or if the child refuses to exit...). */
			(void) waitpid(ggzmod->pid, NULL, 0);
		}
		ggzmod->pid = -1;
#else
#  ifdef HAVE_WINSOCK2_H
		if (ggzmod->process != INVALID_HANDLE_VALUE) {
			TerminateProcess(ggzmod->process, 0);
			CloseHandle(ggzmod->process);
			ggzmod->process = INVALID_HANDLE_VALUE;
		}
#  endif
#endif
		
		_ggzmod_ggz_set_state(ggzmod, GGZMOD_STATE_DONE);
		/* FIXME: what other cleanups should we do? */
	}
	
	/* We no longer free the seat data here.  It will stick around until
	   ggzmod_ggz_free is called or it is used again. */

	/* Clean up the ggzmod object.  In theory it could now reconnect for
	   a new game. */
	ggz_close_socket(ggzmod->fd);
	ggzmod->fd = -1;

	return 0;
}



/* Returns -1 on error, the number of events handled on success. */
static int _ggzmod_ggz_handle_event(GGZMod * ggzmod, fd_set read_fds)
{
	int status = 0;
	
	if (FD_ISSET(ggzmod->fd, &read_fds)) {
		status = _io_ggz_read_data(ggzmod);
		if (status < 0) {
			_ggzmod_ggz_error(ggzmod, "Error reading data");
			/* FIXME: should be disconnect? */
			_ggzmod_ggz_set_state(ggzmod, GGZMOD_STATE_DONE);
		}
	}

	return status;
}


static void _ggzmod_ggz_set_state(GGZMod * ggzmod, GGZModState state)
{
	GGZModState old_state = ggzmod->state;
	if (state == ggzmod->state)
		return;		/* Is this an error? */

	/* The callback function retrieves the state from ggzmod_ggz_get_state.
	   It could instead be passed as an argument. */
	ggzmod->state = state;
	call_handler(ggzmod, GGZMOD_EVENT_STATE, &old_state);
}




/* 
 * ggz specific actions
 */

/* Sends a game launch packet to ggzmod-game. A negative return value
   indicates a serious (fatal) error. */
static int send_game_launch(GGZMod * ggzmod)
{
	GGZListEntry *entry;

	if (_io_ggz_send_player(ggzmod->fd,
			    ggzmod->my_name,
			    ggzmod->i_am_spectator,
			    ggzmod->my_seat_num) < 0)
		return -2;

	for (entry = ggz_list_head(ggzmod->seats);
	     entry;
	     entry = ggz_list_next(entry)) {
		GGZSeat *seat = ggz_list_get_data(entry);
		if (_io_ggz_send_seat(ggzmod->fd, seat) < 0)
			return -3;
	}
	for (entry = ggz_list_head(ggzmod->spectator_seats);
	     entry;
	     entry = ggz_list_next(entry)) {
		GGZSpectatorSeat *seat = ggz_list_get_data(entry);
		if (_io_ggz_send_spectator_seat(ggzmod->fd, seat) < 0)
			return -4;
	}

	if (_io_ggz_send_launch(ggzmod->fd) < 0)
		return -1;

	/* If the server fd or connection info has already been set, send that too */
	if (ggzmod->server_fd != -1)
		if (_io_ggz_send_server_fd(ggzmod->fd, ggzmod->server_fd) < 0)
			return -5;

	if (ggzmod->server_host)
		if (_io_ggz_send_server(ggzmod->fd, ggzmod->server_host,
				    ggzmod->server_port,
				    ggzmod->server_handle) < 0)
			return -5;

	return 0;
}

/* Sets an environment variable (always overwrites) */
static void ggz_setenv(const char *name, const char *value)
{
#ifdef HAVE_SETENV
	setenv(name, value, 1);
#else
	SetEnvironmentVariable(name, value);
#endif
}

/* Common setup for normal mode and embedded mode.  This does partial
   prep work for the GGZ socket connection between game client and GGZ
   client.
   
   There are two possible results of this function:

   1. The GGZ end of the socket goes into fd_pair[0] and the game end
   into fd_pair[1].  *sock is set to -1.  No further work is needed
   although GGZ should close unused sockets if a fork is done.  The
   game end socket number is put into the GGZSOCKET environment variable
   and GGZPORT is left unset or set at -1.

   2. The GGZ end of the socket is created as a local socket and listened
   to.  The game end is not created, but the GGZPORT environment variable
   contains the port number that must be connected to to create it.  The
   game end must be finalized later by connecting to this socket while
   the GGZ end is found by calling accept() later.  fd_pair values are
   both set to -1.

   Currently which is done depends on the presence of socketpair which
   is required for the first method - but this should not be assumed and
   the later steps should instead check the values of the GGZSOCKET and
   GGZPORT environment variables and the values of fd_pair and sock.

   The function returns 0 on success, -1 on error (which is fatal to
   the launch process). */
static int game_prepare(int fd_pair[2], int *sock)
{
	char buf[100];
#ifndef USE_SOCKETPAIR
	int port;
#endif

	fd_pair[0] = fd_pair[1] = *sock = -1;

#ifdef USE_SOCKETPAIR
	if (socketpair(PF_LOCAL, SOCK_STREAM, 0, fd_pair) < 0) {
		ggz_error_sys("socketpair failed");
		return -1;
	}
	snprintf(buf, sizeof(buf), "%d", fd_pair[1]);
	ggz_setenv("GGZSOCKET", buf);
	ggz_setenv("GGZPORT", "-1");
#else

	/* Winsock implementation: see ggzmod_ggz_connect. */
	port = 5898;
	do {
		port++;
		*sock = ggz_make_socket(GGZ_SOCK_SERVER, port, NULL);
	} while (*sock < 0 && port < 7000);
	if (*sock < 0) {
		ggz_error_msg("Could not bind socket.");
		return -1;
	}
	if (listen(*sock, 1) < 0) {
		ggz_close_socket(*sock);
		ggz_error_msg("Could not listen on socket.");
		return -1;
	}
	ggz_setenv("GGZSOCKET", "-1");
	snprintf(buf, sizeof(buf), "%d", port);
	ggz_setenv("GGZPORT", buf);
#endif

	ggz_setenv("GGZMODE", "true");
	return 0;
}

/* Forks the game.  A negative return value indicates a serious error. */
/* No locking should be necessary within this function. */
static int game_fork(GGZMod * ggzmod)
{
	int fd_pair[2], sock, sock2;
#ifdef HAVE_FORK
	int pid;
#else
	char cmdline[1024] = "";
	int i;
	PROCESS_INFORMATION pi;
	STARTUPINFO si;
#endif

	/* If there are no args, we don't know what to run! */
	if (ggzmod->argv == NULL || ggzmod->argv[0] == NULL) {
		_ggzmod_ggz_error(ggzmod, "No arguments");
		return -1;
	}

	if(game_prepare(fd_pair, &sock) < 0)
		return -1;

#ifdef HAVE_FORK
	if ((pid = fork()) < 0)
		ggz_error_sys_exit("fork failed");
	else if (pid == 0) {
		/* child */

		if (fd_pair[0] >= 0) ggz_close_socket(fd_pair[0]);
		if (sock >= 0) ggz_close_socket(sock);

		/* FIXME: Close all other fd's? */
		/* FIXME: Not necessary to close other fd's if we use
		   CLOSE_ON_EXEC */

		/* Set working directory */
		if (ggzmod->pwd
		    && chdir(ggzmod->pwd) < 0) {
			/* FIXME: what to do? */
		}

		/* FIXME: can we call ggzmod_ggz_log() from here? */
		if (ggzmod->argv[0][0] == '/') {
			/* run specified executable */
			execv(ggzmod->argv[0], ggzmod->argv);
		} else {
			/* run game in PATH */
			execvp(ggzmod->argv[0], ggzmod->argv);
		}

		/* We should never get here.  If we do, it's an eror */
		/* we still can't send error messages... */
		ggz_error_sys_exit("exec of %s failed", ggzmod->argv[0]);
	} else {
		/* parent */
		if (fd_pair[1] >= 0) ggz_close_socket(fd_pair[1]);

		ggzmod->pid = pid;
		
		/* FIXME: should we delete the argv arguments? */
		
		/* That's all! */
	}
#else
	for (i = 0; ggzmod->argv[i]; i++) {
		snprintf(cmdline + strlen(cmdline),
			 sizeof(cmdline) - strlen(cmdline),
			 "%s ", ggzmod->argv[i]);
	}

	ZeroMemory(&si, sizeof(si));
	if (!CreateProcess(NULL, cmdline, NULL, NULL, TRUE,
			   DETACHED_PROCESS | NORMAL_PRIORITY_CLASS,
			   NULL, NULL, &si, &pi)) {
		return -1;
	}
	CloseHandle(pi.hThread);
	ggzmod->process = pi.hProcess;
#endif

	/* Finalize socket */
	if (fd_pair[0] >= 0) {
		ggzmod->fd = fd_pair[0];
	} else {
		/* FIXME: we need to select, with a maximum timeout. */
		/* FIXME: this is insecure; it should be restricted
		   to local connections. */
		sock2 = accept(sock, NULL, NULL);
		if (sock2 < 0) {
			/* FIXME: need to close child in the FORK case */
			ggz_error_sys("Listening to socket failed.");
			return -1;
		}
		ggz_debug("GGZMOD", "Socket accepted as %d.\n", sock2);
		ggz_close_socket(sock);
		ggzmod->fd = sock2;
	}

	return 0;
}


static char *ggz_getenv(const char *name)
{
#ifdef HAVE_GETENV
	return getenv(name);
#else
	/* This is NOT the correct way to call this function. But,
	   it seems getenv is available on windows so this bit of code is
	   never reached.  It is left here to trigger a compile error if that
	   case ever comes up. */
	return GetEnvironmentVariable(name);
#endif
}


/* If the socket is not provided but a port is provided, connect
   to that port.  Used for windows OS by default. */
static int ggzmod_connect_port(void)
{
	int sock, port;
	char *ggzportstr = ggz_getenv("GGZPORT");
	char buf[100];

	ggz_debug("GGZMOD", "GGZPORT '%s'\n", ggzportstr);

	if (!ggzportstr
	    || sscanf(ggzportstr, "%d", &port) == 0
	    || port < 0) {
		ggz_error_msg_exit("Could not determine port.");
	}

	sock = ggz_make_socket(GGZ_SOCK_CLIENT, port, "localhost");
	if (sock < 0) {
		ggz_error_msg_exit("Could not connect to port.");
	}

	snprintf(buf, sizeof(buf), "%d", sock);
	ggz_setenv("GGZSOCKET", buf);

	return sock;
}


/* Similar to game_fork(), but runs the game embedded */
static int game_embedded(GGZMod * ggzmod)
{
	int fd_pair[2], sock, sock2;

	if(game_prepare(fd_pair, &sock) < 0)
		return -1;

	if (fd_pair[0] >= 0) {
		ggzmod->fd = fd_pair[0];
	} else {
		(void) ggzmod_connect_port();

		/* FIXME: we need to select, with a maximum timeout. */
		/* FIXME: this is insecure; it should be restricted
		   to local connections. */
		sock2 = accept(sock, NULL, NULL);
		if (sock2 < 0) {
			ggz_error_sys("Listening to socket failed.");
			return -1;
		}
		ggz_close_socket(sock);
		ggzmod->fd = sock2;
	}

	ggz_debug("GGZMOD", "pair %d/%d sock %d fd %d\n",
		  fd_pair[0], fd_pair[1], sock, ggzmod->fd);

#ifdef HAVE_FORK
	ggzmod->pid = -1; /* FIXME: use -1 for embedded ggzcore? getpid()? */
#else
	ggzmod->process = INVALID_HANDLE_VALUE;
#endif

	return 0;
}


/**** Internal library functions ****/

/* Invokes handlers for the specified event */
static void call_handler(GGZMod *ggzmod, GGZModEvent event, void *data)
{
	if (ggzmod->handlers[event])
		(*ggzmod->handlers[event]) (ggzmod, event, data);
}


static void call_transaction(GGZMod * ggzmod, GGZModTransaction t, void *data)
{
	if (!ggzmod->thandlers[t]) {
		ggz_error_msg("Unhandled transaction %d.", t);
		return;
	}

	if (ggzmod->type != GGZMOD_GGZ) {
		ggz_error_msg("The game can't handle transactions!");
		return;
	}

	(*ggzmod->thandlers[t])(ggzmod, t, data);
}


void _ggzmod_ggz_error(GGZMod *ggzmod, char* error)
{
	call_handler(ggzmod, GGZMOD_EVENT_ERROR, error);
}


void _ggzmod_ggz_handle_state(GGZMod * ggzmod, GGZModState state)
{
	/* There's only certain ones the game is allowed to set it to,
	   and they can only change it if the state is currently
	   WAITING or PLAYING. */
	switch (state) {
	case GGZMOD_STATE_CREATED:
	case GGZMOD_STATE_CONNECTED:
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
		_ggzmod_ggz_set_state(ggzmod, state);
		return;
	}
	_ggzmod_ggz_error(ggzmod,
		      "Game requested incorrect state value");

	/* Is this right? has the gameover happened yet? */
}

void _ggzmod_ggz_handle_stand_request(GGZMod *ggzmod)
{
	call_transaction(ggzmod, GGZMOD_TRANSACTION_STAND, NULL);
}

void _ggzmod_ggz_handle_sit_request(GGZMod *ggzmod, int seat_num)
{
	call_transaction(ggzmod, GGZMOD_TRANSACTION_SIT, &seat_num);
}

void _ggzmod_ggz_handle_boot_request(GGZMod *ggzmod, char *name)
{
	call_transaction(ggzmod, GGZMOD_TRANSACTION_BOOT, name);
}

void _ggzmod_ggz_handle_bot_request(GGZMod *ggzmod, int seat_num)
{
	call_transaction(ggzmod, GGZMOD_TRANSACTION_BOT, &seat_num);
}

void _ggzmod_ggz_handle_open_request(GGZMod *ggzmod, int seat_num)
{
	call_transaction(ggzmod, GGZMOD_TRANSACTION_OPEN, &seat_num);
}

void _ggzmod_ggz_handle_chat_request(GGZMod *ggzmod, char *chat_msg)
{
	call_transaction(ggzmod, GGZMOD_TRANSACTION_CHAT, chat_msg);
}

void _ggzmod_ggz_handle_info_request(GGZMod *ggzmod, int seat_num)
{
	call_transaction(ggzmod, GGZMOD_TRANSACTION_INFO, &seat_num);
}

void _ggzmod_ggz_handle_rankings_request(GGZMod *ggzmod)
{
	call_transaction(ggzmod, GGZMOD_TRANSACTION_RANKINGS, NULL);
}

int ggzmod_ggz_set_stats(GGZMod *ggzmod, GGZStat *player_stats,
		     GGZStat *spectator_stats)
{
	if (!player_stats
	    || !ggzmod
	    || (!spectator_stats && ggzmod->num_spectator_seats > 0)
	    || ggzmod->type != GGZMOD_GGZ
	    || ggzmod->state == GGZMOD_STATE_CREATED) {
		return -1;
	}

	return _io_ggz_send_stats(ggzmod->fd, ggzmod->num_seats, player_stats,
			      ggzmod->num_spectator_seats, spectator_stats);
}

int ggzmod_ggz_set_info(GGZMod *ggzmod, int num,
		        GGZList *infos)
{
	if (!ggzmod) {
		return -1;
	}

	return _io_ggz_send_msg_info(ggzmod->fd, num, infos);
}

int ggzmod_ggz_set_rankings(GGZMod *ggzmod, int num,
		        GGZList *rankings)
{
	if (!ggzmod) {
		return -1;
	}

	return _io_ggz_send_msg_rankings(ggzmod->fd, num, rankings);
}

