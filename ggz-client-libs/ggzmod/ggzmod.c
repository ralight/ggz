/* 
 * File: ggzmod.c
 * Author: GGZ Dev Team
 * Project: ggzmod
 * Date: 10/14/01
 * Desc: GGZ game module functions
 * $Id: ggzmod.c 6151 2004-07-17 22:45:57Z josef $
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
#ifdef HAVE_WINSOCK_H
# include <winsock.h>
#endif
#include <sys/time.h>
#include <sys/types.h>
#ifdef HAVE_SYS_WAIT_H
# include <sys/wait.h>
#endif
#include <unistd.h>

#include <ggz.h>

#include "ggzmod.h"
#include "ggzmod-ggz.h"
#include "mod.h"
#include "io.h"
#include "protocol.h"


/* 
 * internal function prototypes
 */

static void call_handler(GGZMod *ggzmod, GGZModEvent event, void *data);
static void call_transaction(GGZMod * ggzmod, GGZModTransaction t, void *data);
static int _ggzmod_handle_event(GGZMod * ggzmod, fd_set read_fds);
static void _ggzmod_set_state(GGZMod * ggzmod, GGZModState state);
static int send_game_launch(GGZMod * ggzmod);
static int game_fork(GGZMod * ggzmod);

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

static int stats_compare(void *p, void *q) {
	return (((GGZStat*)p)->number == *(int*)q);
}

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

	ggzmod->stats = ggz_list_create(stats_compare, NULL, NULL, GGZ_LIST_ALLOW_DUPS);
	ggzmod->spectator_stats = ggz_list_create(stats_compare, NULL, NULL, GGZ_LIST_ALLOW_DUPS);

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
void ggzmod_free(GGZMod * ggzmod)
{
	int i;

	if (!ggzmod) {
		return;
	}
	
	if (ggzmod->fd != -1)
		(void)ggzmod_disconnect(ggzmod);
	
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
		ggz_error_msg("ggzmod_set_handler: "
			      "invalid params");
		return;		/* not very useful */
	}

	ggzmod->handlers[e] = func;
}


void ggzmod_set_transaction_handler(GGZMod * ggzmod,
				    GGZModTransaction t,
				    GGZModTransactionHandler func)
{
	if (!ggzmod
	    || t < 0 || t >= GGZMOD_NUM_TRANSACTIONS
	    || ggzmod->type != GGZMOD_GGZ) {
		ggz_error_msg("ggzmod_set_transaction_handler: "
			      "invalid params");
		return;
	}

	ggzmod->thandlers[t] = func;
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

static void _ggzmod_set_player(GGZMod *ggzmod,
			       const char *name,
			       int is_spectator, int seat_num)
{
	if (ggzmod->my_name)
		ggz_free(ggzmod->my_name);
	ggzmod->my_name = ggz_strdup(name);

	ggzmod->i_am_spectator = is_spectator;
	ggzmod->my_seat_num = seat_num;
}

void _ggzmod_handle_player(GGZMod *ggzmod,
			   const char *name,
			   int is_spectator, int seat_num)
{
	/* FIXME: better event data */
	int old[2] = {ggzmod->i_am_spectator, ggzmod->my_seat_num};

	assert(ggzmod->type == GGZMOD_GAME);
	_ggzmod_set_player(ggzmod, name, is_spectator, seat_num);

	if (ggzmod->state != GGZMOD_STATE_CREATED)
		call_handler(ggzmod, GGZMOD_EVENT_PLAYER, old);
}

const char *ggzmod_get_player(GGZMod *ggzmod,
			      int *is_spectator, int *seat_num)
{
	if (ggzmod->state == GGZMOD_STATE_CREATED)
		ggz_error_msg("ggzmod_get_my_seat:"
			      " can't call when state is CREATED.");

	*is_spectator = ggzmod->i_am_spectator;
	*seat_num = ggzmod->my_seat_num;

	return ggzmod->my_name;
}

int ggzmod_set_player(GGZMod *ggzmod, const char *name,
		      int is_spectator, int seat_num)
{
	if (!ggzmod
	    || ggzmod->type != GGZMOD_GGZ)
		return -1;

	_ggzmod_set_player(ggzmod, name, is_spectator, seat_num);

	if (ggzmod->state != GGZMOD_STATE_CREATED)
		_io_send_player(ggzmod->fd, name, is_spectator, seat_num);

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

int ggzmod_get_num_seats(GGZMod * ggzmod)
{
	/* Note: num_seats is initialized to 0 and isn't
	   changed until we hear differently from GGZ. */
	return ggzmod->num_seats;
}

int ggzmod_get_num_spectator_seats(GGZMod * ggzmod)
{
	/* Note: num_spectator_seats is initialized to 0 and isn't
	   changed until we hear differently from GGZ. */
	return ggzmod->num_spectator_seats;
}

GGZSeat ggzmod_get_seat(GGZMod *ggzmod, int num)
{
	GGZSeat seat = {num: num,
			type: GGZ_SEAT_NONE,
			name: NULL};

	if (num >= 0 && num < ggzmod->num_seats) {
		GGZListEntry *entry;
		entry = ggz_list_search(ggzmod->seats, &seat);
		if (entry)
			seat = *(GGZSeat*)ggz_list_get_data(entry);
	}

	return seat;
}

GGZSpectatorSeat ggzmod_get_spectator_seat(GGZMod * ggzmod, int num)
{
	GGZSpectatorSeat seat = {num: num, name: NULL};

	if (num >= 0 && num < ggzmod->num_spectator_seats) {
		GGZListEntry *entry;
		entry = ggz_list_search(ggzmod->spectator_seats, &seat);
		if (entry)
		  seat = *(GGZSpectatorSeat*)ggz_list_get_data(entry);
	}

	return seat;
}

static void _ggzmod_set_seat(GGZMod *ggzmod, GGZSeat *seat)
{
	if (seat->num >= ggzmod->num_seats)
		ggzmod->num_seats = seat->num + 1;
	ggz_list_insert(ggzmod->seats, seat);
}

void _ggzmod_handle_seat(GGZMod * ggzmod, GGZSeat *seat)
{
	GGZSeat *old_seat;
	GGZListEntry *entry;

	/* Copy current seat to old_seat */
	entry = ggz_list_search(ggzmod->seats, &seat);
	if (!entry) {
		GGZSeat myseat = {num:seat->num,
				  type:GGZ_SEAT_NONE,
				  name: NULL};
		old_seat = seat_copy(&myseat);
	} else {
		old_seat = ggz_list_get_data(entry);
		old_seat = seat_copy(old_seat);
	}

	/* Place the new seat into the list */
	_ggzmod_set_seat(ggzmod, seat);

	/* Invoke the handler */
	if (ggzmod->state != GGZMOD_STATE_CREATED)
		call_handler(ggzmod, GGZMOD_EVENT_SEAT, old_seat);

	/* Free old_seat */
	seat_free(old_seat);
}

int ggzmod_set_seat(GGZMod * ggzmod, GGZSeat *seat)
{
	GGZSeat oldseat;

	if (ggzmod->type == GGZMOD_GAME)
		return -1;

	if (!seat || seat->num < 0) {
		return -2;
	}

	/* If there is no such seat, return error */
	oldseat = ggzmod_get_seat(ggzmod, seat->num);

	if (oldseat.type == seat->type
	    && ggz_strcmp(oldseat.name, seat->name) == 0) {
		/* No change. */
		return 0;
	}

	if (ggzmod->state != GGZMOD_STATE_CREATED) {
		if (_io_send_seat(ggzmod->fd, seat) < 0)
			_ggzmod_error(ggzmod, "Error writing to game");
	}

	_ggzmod_set_seat(ggzmod, seat);

	return 0;
}

static void _ggzmod_set_spectator_seat(GGZMod * ggzmod, GGZSpectatorSeat *seat)
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

void _ggzmod_handle_spectator_seat(GGZMod * ggzmod, GGZSpectatorSeat * seat)
{
	GGZSpectatorSeat *old_seat;
	GGZListEntry *entry;

	/* Copy current seat to old_seat */
	entry = ggz_list_search(ggzmod->spectator_seats, seat);
	if (!entry) {
		GGZSpectatorSeat myseat = {num:seat->num,
					   name: NULL};

		old_seat = spectator_seat_copy(&myseat);
	} else {
		old_seat = ggz_list_get_data(entry);
		old_seat = spectator_seat_copy(old_seat);
	}

	/* Place the new seat into the list */
	_ggzmod_set_spectator_seat(ggzmod, seat);

	/* Invoke the handler */
	if (ggzmod->state != GGZMOD_STATE_CREATED)
		call_handler(ggzmod, GGZMOD_EVENT_SPECTATOR_SEAT, old_seat);

	/* Free old_seat */
	spectator_seat_free(old_seat);
}

void _ggzmod_handle_chat(GGZMod *ggzmod, char *player, char *chat_msg)
{
	GGZChat chat;

	chat.player = player;
	chat.message = chat_msg;

	call_handler(ggzmod, GGZMOD_EVENT_CHAT, &chat);
}

void _ggzmod_handle_stats(GGZMod *ggzmod, GGZStat *player_stats,
			  GGZStat *spectator_stats)
{
	GGZListEntry *entry;
	GGZSeat *seat;
	GGZSpectatorSeat *spectator;
	GGZStat stat;
	int i;

	i = 0;
	for(entry = ggz_list_head(ggzmod->seats); entry; entry = ggz_list_next(entry)) {
		seat = ggz_list_get_data(entry);
		stat = player_stats[i];
		stat.number = seat->num;
		ggz_list_insert(ggzmod->stats, &stat);
		i++;
	}

	i = 0;
	for(entry = ggz_list_head(ggzmod->spectator_seats); entry; entry = ggz_list_next(entry)) {
		spectator = ggz_list_get_data(entry);
		stat = spectator_stats[i];
		stat.number = spectator->num;
		ggz_list_insert(ggzmod->spectator_stats, &stat);
		i++;
	}
}

int ggzmod_set_spectator_seat(GGZMod * ggzmod, GGZSpectatorSeat *seat)
{
	if (!seat) return -1;
	if (ggzmod->type == GGZMOD_GAME) return -2;
	if (seat->num < 0) return -3;

	if (ggzmod->state != GGZMOD_STATE_CREATED) {
		GGZSpectatorSeat old_seat;
		old_seat = ggzmod_get_spectator_seat(ggzmod, seat->num);
		if (ggz_strcmp(seat->name, old_seat.name)
		    && _io_send_spectator_seat(ggzmod->fd, seat) < 0) {
			_ggzmod_error(ggzmod, "Error writing to game");
			return -4;
		}
	}

	_ggzmod_set_spectator_seat(ggzmod, seat);

	return 0;
}

int ggzmod_inform_chat(GGZMod * ggzmod, const char *player, const char *msg)
{
	if (_io_send_msg_chat(ggzmod->fd, player, msg) < 0) {
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
		
#ifdef HAVE_KILL
		/* Make sure game server is dead */
		if (ggzmod->pid > 0) {
			kill(ggzmod->pid, SIGINT);
			/* This will block waiting for the child to exit.
			   This could be a problem if there is an error
			   (or if the child refuses to exit...). */
			(void) waitpid(ggzmod->pid, NULL, 0);
		}
		ggzmod->pid = -1;
#else
		if (ggzmod->process != INVALID_HANDLE_VALUE) {
			TerminateProcess(ggzmod->process, 0);
			CloseHandle(ggzmod->process);
			ggzmod->process = INVALID_HANDLE_VALUE;
		}
#endif
		
		_ggzmod_set_state(ggzmod, GGZMOD_STATE_DONE);
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
#ifdef HAVE_WINSOCK_H
	closesocket(ggzmod->fd);
#else
	close(ggzmod->fd);
#endif
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
	GGZListEntry *entry;

	if (_io_send_player(ggzmod->fd,
			    ggzmod->my_name,
			    ggzmod->i_am_spectator,
			    ggzmod->my_seat_num) < 0)
		return -2;

	for (entry = ggz_list_head(ggzmod->seats);
	     entry;
	     entry = ggz_list_next(entry)) {
		GGZSeat *seat = ggz_list_get_data(entry);
		if (_io_send_seat(ggzmod->fd, seat) < 0)
			return -3;
	}
	for (entry = ggz_list_head(ggzmod->spectator_seats);
	     entry;
	     entry = ggz_list_next(entry)) {
		GGZSpectatorSeat *seat = ggz_list_get_data(entry);
		if (_io_send_spectator_seat(ggzmod->fd, seat) < 0)
			return -4;
	}

	if (_io_send_launch(ggzmod->fd) < 0)
		return -1;

	/* If the server fd has already been set, send that too */
	if (ggzmod->server_fd != -1)
		if (_io_send_server(ggzmod->fd, ggzmod->server_fd) < 0)
			return -5;

	return 0;
}


/* Forks the game.  A negative return value indicates a serious error. */
/* No locking should be necessary within this function. */
static int game_fork(GGZMod * ggzmod)
{
	int fd_pair[2];		/* socketpair */
#ifdef HAVE_FORK
	int pid;
#else
	char cmdline[1024] = "";
	int i;
	PROCESS_INFORMATION pi;
#endif

	/* If there are no args, we don't know what to run! */
	if (ggzmod->argv == NULL || ggzmod->argv[0] == NULL) {
		_ggzmod_error(ggzmod, "No arguments");
		return -1;
	}

	if (socketpair(PF_LOCAL, SOCK_STREAM, 0, fd_pair) < 0)
		ggz_error_sys_exit("socketpair failed");

#ifdef HAVE_FORK
	if ((pid = fork()) < 0)
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
#else
	for (i = 0; ggzmod->argv[i]; i++) {
		snprintf(cmdline + strlen(cmdline),
			 sizeof(cmdline) - strlen(cmdline),
			 "%s ", ggzmod->argv[i]);
	}

	if (!CreateProcess(NULL, cmdline, NULL, NULL, TRUE,
			   DETACHED_PROCESS | NORMAL_PRIORITY_CLASS,
			   NULL, NULL, NULL, &pi)) {
		return -1;
	}
	CloseHandle(pi.hThread);
	ggzmod->process = pi.hProcess;
#endif
	return 0;
}


/**** Transaction requests  ****/

void ggzmod_request_stand(GGZMod * ggzmod)
{
	_io_send_req_stand(ggzmod->fd);
}

void ggzmod_request_sit(GGZMod * ggzmod, int seat_num)
{
	_io_send_req_sit(ggzmod->fd, seat_num);
}

void ggzmod_request_boot(GGZMod * ggzmod, const char *name)
{
	_io_send_req_boot(ggzmod->fd, name);
}

void ggzmod_request_bot(GGZMod * ggzmod, int seat_num)
{
	_io_send_request_bot(ggzmod->fd, seat_num);
}

void ggzmod_request_open(GGZMod * ggzmod, int seat_num)
{
	_io_send_request_open(ggzmod->fd, seat_num);
}

void ggzmod_request_chat(GGZMod *ggzmod, const char *chat_msg)
{
	_io_send_request_chat(ggzmod->fd, chat_msg);
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

void _ggzmod_handle_stand_request(GGZMod *ggzmod)
{
	call_transaction(ggzmod, GGZMOD_TRANSACTION_STAND, NULL);
}

void _ggzmod_handle_sit_request(GGZMod *ggzmod, int seat_num)
{
	call_transaction(ggzmod, GGZMOD_TRANSACTION_SIT, &seat_num);
}

void _ggzmod_handle_boot_request(GGZMod *ggzmod, char *name)
{
	call_transaction(ggzmod, GGZMOD_TRANSACTION_BOOT, name);
}

void _ggzmod_handle_bot_request(GGZMod *ggzmod, int seat_num)
{
	call_transaction(ggzmod, GGZMOD_TRANSACTION_BOT, &seat_num);
}

void _ggzmod_handle_open_request(GGZMod *ggzmod, int seat_num)
{
	call_transaction(ggzmod, GGZMOD_TRANSACTION_OPEN, &seat_num);
}

void _ggzmod_handle_chat_request(GGZMod *ggzmod, char *chat_msg)
{
	call_transaction(ggzmod, GGZMOD_TRANSACTION_CHAT, chat_msg);
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
  
int ggzmod_set_stats(GGZMod *ggzmod, GGZStat *player_stats,
		     GGZStat *spectator_stats)
{
	if (!player_stats
	    || !ggzmod
	    || (!spectator_stats && ggzmod->num_spectator_seats > 0)
	    || ggzmod->type != GGZMOD_GGZ
	    || ggzmod->state != GGZMOD_STATE_CREATED) {
		return -1;
	}

	return _io_send_stats(ggzmod->fd, ggzmod->num_seats, player_stats,
			      ggzmod->num_spectator_seats, spectator_stats);
}

int ggzmod_player_get_record(GGZMod *ggzmod, GGZSeat *seat,
			     int *wins, int *losses,
			     int *ties, int *forfeits)
{
	GGZListEntry *entry = ggz_list_search(ggzmod->stats, &seat->num);
	GGZStat *stat = ggz_list_get_data(entry);
	if(!stat) return 0;
	*wins = stat->wins;
	*losses = stat->losses;
	*ties = stat->ties;
	*forfeits = stat->forfeits;
	return 1;
}

int ggzmod_player_get_rating(GGZMod *ggzmod, GGZSeat *seat, int *rating)
{
	GGZListEntry *entry = ggz_list_search(ggzmod->stats, &seat->num);
	GGZStat *stat = ggz_list_get_data(entry);
	if(!stat) return 0;
	*rating = stat->rating;
	return 1;
}

int ggzmod_player_get_ranking(GGZMod *ggzmod, GGZSeat *seat, int *ranking)
{
	GGZListEntry *entry = ggz_list_search(ggzmod->stats, &seat->num);
	GGZStat *stat = ggz_list_get_data(entry);
	if(!stat) return 0;
	*ranking = stat->ranking;
	return 1;
}

int ggzmod_player_get_highscore(GGZMod *ggzmod, GGZSeat *seat, int *highscore)
{
	GGZListEntry *entry = ggz_list_search(ggzmod->stats, &seat->num);
	GGZStat *stat = ggz_list_get_data(entry);
	if(!stat) return 0;
	*highscore = stat->highscore;
	return 1;
}

int ggzmod_spectator_get_record(GGZMod *ggzmod, GGZSpectatorSeat *seat,
				int *wins, int *losses,
				int *ties, int *forfeits)
{
	GGZListEntry *entry = ggz_list_search(ggzmod->spectator_stats, &seat->num);
	GGZStat *stat = ggz_list_get_data(entry);
	if(!stat) return 0;
	*wins = stat->wins;
	*losses = stat->losses;
	*ties = stat->ties;
	*forfeits = stat->forfeits;
	return 1;
}

int ggzmod_spectator_get_rating(GGZMod *ggzmod, GGZSpectatorSeat *seat, int *rating)
{
	GGZListEntry *entry = ggz_list_search(ggzmod->spectator_stats, &seat->num);
	GGZStat *stat = ggz_list_get_data(entry);
	if(!stat) return 0;
	*rating = stat->rating;
	return 1;
}

int ggzmod_spectator_get_ranking(GGZMod *ggzmod, GGZSpectatorSeat *seat, int *ranking)
{
	GGZListEntry *entry = ggz_list_search(ggzmod->spectator_stats, &seat->num);
	GGZStat *stat = ggz_list_get_data(entry);
	if(!stat) return 0;
	*ranking = stat->ranking;
	return 1;
}

int ggzmod_spectator_get_highscore(GGZMod *ggzmod, GGZSpectatorSeat *seat, int *highscore)
{
	GGZListEntry *entry = ggz_list_search(ggzmod->spectator_stats, &seat->num);
	GGZStat *stat = ggz_list_get_data(entry);
	if(!stat) return 0;
	*highscore = stat->highscore;
	return 1;
}
