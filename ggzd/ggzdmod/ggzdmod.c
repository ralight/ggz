/* 
 * File: ggzdmod.c
 * Author: GGZ Dev Team
 * Project: ggzdmod
 * Date: 10/14/01
 * Desc: GGZ game module functions
 * $Id: ggzdmod.c 3844 2002-04-07 22:27:57Z jdorje $
 *
 * This file contains the backend for the ggzdmod library.  This
 * library facilitates the communication between the GGZ server (ggzd)
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

#include <config.h>		/* site-specific config */

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

#include "ggzdmod.h"
#include "mod.h"
#include "io.h"
#include "protocol.h"


/* This checks the ggzdmod object for validity.  It could do more checking if
   desired. */
#define CHECK_GGZDMOD(ggzdmod) (ggzdmod)



/* 
 * internal function prototypes
 */

static void call_handler(GGZdMod *ggzdmod, GGZdModEvent event, void *data);
static int get_fd_max(GGZdMod * ggzdmod);
static fd_set get_active_fd_set(GGZdMod * ggzdmod);
static void _ggzdmod_set_num_seats(GGZdMod * ggzdmod, int num_seats);
static int strings_differ(char *s1, char *s2);
static void set_state(GGZdMod * ggzdmod, GGZdModState state);
static int handle_event(GGZdMod * ggzdmod, fd_set read_fds);
static int send_game_launch(GGZdMod * ggzdmod);
static int game_fork(GGZdMod * ggzdmod);

/* Functions for manipulating seats */
static GGZSeat* seat_copy(GGZSeat *orig);
static int seat_compare(GGZSeat *a, GGZSeat *b);
static int seat_find_player(GGZSeat *a, GGZSeat *b);
static void seat_free(GGZSeat *seat);

/* Debugging function (see also ggzdmod_check) */
static void seat_print(GGZdMod * ggzdmod, GGZSeat *seat);

/* Invokes handlers for the specefied event */
static void call_handler(GGZdMod * ggzdmod, GGZdModEvent event, void *data)
{
	if (ggzdmod->handlers[event])
		(*ggzdmod->handlers[event]) (ggzdmod, event, data);
	else {
		/* We'd like to send a debugging message if there's
		   no handler registered for an event.  But in the
		   case of the ERROR event, this can cause problems
		   since the problem is usually a missing connection!
		   So, we use this hack to avoid a recursive loop. */
		char *which = ggzdmod->type == GGZDMOD_GAME ? "game" : "ggz";
		if (event != GGZDMOD_EVENT_ERROR)
			ggzdmod_log(ggzdmod,
				    "GGZDMOD: unhandled event %d by %s.",
				    event, which);
		else
			fprintf(stderr,
				"GGZDMOD: unhandled event %d by %s.\n",
				event, which);
	}
}


/* Returns the highest-numbered FD used by ggzdmod. */
static int get_fd_max(GGZdMod * ggzdmod)
{
	int max = ggzdmod->fd;
	GGZListEntry *entry;
	GGZSeat *seat;

	/* If we don't have a player data handler set
	   up, we won't monitor the player data sockets. */
	if (ggzdmod->handlers[GGZDMOD_EVENT_PLAYER_DATA])
		for (entry = ggz_list_head(ggzdmod->seats);
		     entry != NULL;
		     entry = ggz_list_next(entry)) {

			seat = ggz_list_get_data(entry);
			if (seat->fd > max)
				max = seat->fd;
		}
	
	return max;
}


static fd_set get_active_fd_set(GGZdMod * ggzdmod)
{
	fd_set active_fd_set;
	GGZListEntry *entry;
	GGZSeat *seat;
	
	FD_ZERO(&active_fd_set);
	if (ggzdmod->fd != -1)
		FD_SET(ggzdmod->fd, &active_fd_set);

	/* Only monitory player fds if there is a PLAYER_DATA handler set */
	if (ggzdmod->handlers[GGZDMOD_EVENT_PLAYER_DATA]) {
		for (entry = ggz_list_head(ggzdmod->seats);
		     entry != NULL;
		     entry = ggz_list_next(entry)) {
			
			seat = ggz_list_get_data(entry);
			if (seat->fd != -1)
				FD_SET(seat->fd, &active_fd_set);
		}
	}
	
	/* FIXME: shouldn't we log an error if there are no available FD's? */
	
	return active_fd_set;
}


/* 
 * Creating/destroying a ggzdmod object
 */

/* Creates a new ggzdmod object. */
GGZdMod *ggzdmod_new(GGZdModType type)
{
	int i;
	GGZdMod *ggzdmod;

	/* verify parameter */
	if (type != GGZDMOD_GGZ && type != GGZDMOD_GAME)
		return NULL;

	/* allocate */
	ggzdmod = ggz_malloc(sizeof(*ggzdmod));

	/* initialize */
	ggzdmod->type = type;
	ggzdmod->state = GGZDMOD_STATE_CREATED;
	ggzdmod->fd = -1;
	ggzdmod->seats = ggz_list_create((ggzEntryCompare)seat_compare,
					 (ggzEntryCreate)seat_copy,
					 (ggzEntryDestroy)seat_free,
					 GGZ_LIST_REPLACE_DUPS);

	for (i = 0; i < GGZDMOD_NUM_HANDLERS; i++)
		ggzdmod->handlers[i] = NULL;
	ggzdmod->gamedata = NULL;

	ggzdmod->pid = -1;
	ggzdmod->argv = NULL;
	/* Put any other necessary initialization here.  All fields
	   should be initialized.  Note NULL may not necessarily be 0
	   on all platforms. */

	return ggzdmod;
}


/* Frees (deletes) a ggzdmod object */
void ggzdmod_free(GGZdMod * ggzdmod)
{
	int i;

	if (!CHECK_GGZDMOD(ggzdmod)) {
		return;
	}
	
	if (ggzdmod->fd != -1)
		(void)ggzdmod_disconnect(ggzdmod);
	
	ggz_list_free(ggzdmod->seats);
	ggzdmod->num_seats = 0;

	ggzdmod->type = -1;
	
	if (ggzdmod->argv) {
		for (i = 0; ggzdmod->argv[i]; i++)
			if (ggzdmod->argv[i])
				ggz_free(ggzdmod->argv[i]);
		ggz_free(ggzdmod->argv);
	}

	/* Free the object */
	ggz_free(ggzdmod);
}


/* 
 * Accesor functions for GGZdMod
 */

/* The ggzdmod FD is the main ggzd<->game server communications socket. */
int ggzdmod_get_fd(GGZdMod * ggzdmod)
{
	if (!CHECK_GGZDMOD(ggzdmod)) {
		return -1;
	}
	return ggzdmod->fd;
}


GGZdModType ggzdmod_get_type(GGZdMod * ggzdmod)
{
	if (!CHECK_GGZDMOD(ggzdmod)) {
		return -1;	/* not very useful */
	}
	return ggzdmod->type;
}


GGZdModState ggzdmod_get_state(GGZdMod * ggzdmod)
{
	if (!CHECK_GGZDMOD(ggzdmod)) {
		return -1;	/* not very useful */
	}
	return ggzdmod->state;
}


int ggzdmod_get_num_seats(GGZdMod * ggzdmod)
{
	if (!CHECK_GGZDMOD(ggzdmod) || ggzdmod->fd < 0) {
		return -1;
	}
	/* Note: num_seats is initialized to 0 and isn't changed until the
	   launch event (GGZDMOD_EVENT_LAUNCH). */
	return ggzdmod->num_seats;
}


GGZSeat ggzdmod_get_seat(GGZdMod * ggzdmod, int num)
{
	GGZSeat seat = {num, GGZ_SEAT_NONE, NULL, -1};
	GGZListEntry *entry;

	if (CHECK_GGZDMOD(ggzdmod) && num >= 0 && num < ggzdmod->num_seats) {
		if ( (entry = ggz_list_search(ggzdmod->seats, &seat)))
			seat = *(GGZSeat*)ggz_list_get_data(entry);
	}
	
	return seat;
}


void* ggzdmod_get_gamedata(GGZdMod * ggzdmod)
{
	if (!CHECK_GGZDMOD(ggzdmod)) {
		return NULL;
	}
	return ggzdmod->gamedata;
}

static void _ggzdmod_set_num_seats(GGZdMod *ggzdmod, int num_seats)
{
	GGZSeat seat;
	int i, old_num;

	old_num = ggzdmod->num_seats;
	
	/* If the table size is increasing, add more seats */
	if (num_seats > old_num) {
		/* Initialize new seats */
		for (i = old_num; i < num_seats; i++) {
			seat.num = i;
			seat.type = GGZ_SEAT_OPEN;
			seat.name = NULL;
			seat.fd = -1;
			ggz_list_insert(ggzdmod->seats, &seat);
		}
	}
	/* If the table size is increasing, add more seats */
	else if (num_seats < old_num) {
		/* FIXME: delete extra seats */
	}
	
	ggzdmod->num_seats = num_seats;
}


void ggzdmod_set_num_seats(GGZdMod * ggzdmod, int num_seats)
{
	/* Check parameters */
	if (!CHECK_GGZDMOD(ggzdmod) || num_seats < 0
	    || ggzdmod->type != GGZDMOD_GGZ) {
		return;		/* not very useful */
	}
	
	_ggzdmod_set_num_seats(ggzdmod, num_seats);
}
			   
void ggzdmod_set_module(GGZdMod * ggzdmod, char **argv)
{
	int i;

	ggz_debug("GGZDMOD", "Setting arguments");
	
	if (!CHECK_GGZDMOD(ggzdmod))
		return;

	if (ggzdmod->type != GGZDMOD_GGZ) {
		_ggzdmod_error(ggzdmod, "Cannot set module args from module");
		return;
	}
		
	/* Check parameters */
	if (!argv || !argv[0]) {
		_ggzdmod_error(ggzdmod, "Bad module arguments");
		return;
	}

	/* Count the number of args so we know how much to allocate */
	for (i = 0; argv[i]; i++) {}

	ggz_debug("GGZDMOD", "Set %d arguments", i);
	
	ggzdmod->argv = ggz_malloc(sizeof(char*)*(i+1));
	
	for (i = 0; argv[i]; i++) 
		ggzdmod->argv[i] = ggz_strdup(argv[i]);
}


void ggzdmod_set_gamedata(GGZdMod * ggzdmod, void * data)
{
	/* Check parameters */
	if (!CHECK_GGZDMOD(ggzdmod)) {
		return;		/* not very useful */
	}

	ggzdmod->gamedata = data;
}


void ggzdmod_set_handler(GGZdMod * ggzdmod, GGZdModEvent e,
			 GGZdModHandler func)
{
	if (!CHECK_GGZDMOD(ggzdmod) || e < 0 || e >= GGZDMOD_NUM_HANDLERS) {
		return;		/* not very useful */
	}

	ggzdmod->handlers[e] = func;
}

/* returns 0 if s1 and s2 are the same, 1 otherwise.  NULL-safe. */
static int strings_differ(char *s1, char *s2)
{
	if (s1 == NULL && s2 == NULL)
		return 0;
	if (s1 == NULL && s2 != NULL)
		return 1;
	if (s1 != NULL && s2 == NULL)
		return 1;
	return strcmp(s1, s2);
}


static int _ggzdmod_set_seat(GGZdMod * ggzdmod, GGZSeat *seat)
{
	GGZSeat oldseat = ggzdmod_get_seat(ggzdmod, seat->num);
	
	ggz_debug("GGZDMOD", "Seat %d set to type %d (%s)",
		  seat->num, seat->type, seat->name);

	/* Note, some other parts of the code assume that if
	   ggzdmod-game changes seat information, the data is not sent back
	   to ggzdmod-ggz.  Specifically, the game server is allowed to change
	   the player FD (if it is -1) and also the name of bot players. */

	/* If we're connected to the game, send a message */
	if (ggzdmod->type == GGZDMOD_GGZ
	    && ggzdmod->state != GGZDMOD_STATE_CREATED) {
		
		/* Detect join case */
		if ( (oldseat.type == GGZ_SEAT_OPEN
		      || oldseat.type == GGZ_SEAT_RESERVED)
		    && seat->type == GGZ_SEAT_PLAYER) {
		        /* We could check the seat name for a reserved seat,
		           but we trust ggzd to do that instead. */
			if (_io_send_join(ggzdmod->fd, seat) < 0)
				_ggzdmod_error(ggzdmod,
					       "Error writing to game");

			/* We (GGZ) don't need the fd now */
			seat->fd = -1;
		}

		/* Detect leave case */
		else if (oldseat.type == GGZ_SEAT_PLAYER 
			 && seat->type == GGZ_SEAT_OPEN) {
			if (_io_send_leave(ggzdmod->fd, &oldseat) < 0)
				_ggzdmod_error(ggzdmod,
					       "Error writing to game");
		}
		
		/* All other seat changes */
		else {
			if (_io_send_seat_change(ggzdmod->fd, seat) < 0)
				_ggzdmod_error(ggzdmod,
					       "Error writing to game");
		}
	}

	ggz_list_insert(ggzdmod->seats, seat);
	
	return 0;
}


int ggzdmod_set_seat(GGZdMod * ggzdmod, GGZSeat *seat)
{
	GGZSeat oldseat;
	if (!CHECK_GGZDMOD(ggzdmod) || !seat || seat->num < 0
	    || seat->num >= ggzdmod->num_seats) {
		return -2;		
	}

	/* FIXME: check for correctness, (ie. name not NULL for reserved) */
	
	/* If there is no such seat, return error */
	oldseat = ggzdmod_get_seat(ggzdmod, seat->num);
	if (oldseat.type == GGZ_SEAT_NONE) {
		return -1;
	} 
	
	if (ggzdmod->type == GGZDMOD_GAME) {
		/* we allow changing the fd if the old one was -1.  Note
		   this will cause the FD to be monitored by ggzdmod-game,
		   and a GGZDMOD_EVENT_PLAYER_DATA may then be generated
		   for it. */
		if ( (oldseat.fd != -1 && seat->fd != oldseat.fd)
		     || seat->type != oldseat.type)
			return -1;

		/* For now, we allow games to change the names of Bot players,
		   but that's it.  This information is _not_ transmitted back
		   to ggzd (yet). */
		if (oldseat.type != GGZ_SEAT_BOT
		    && strings_differ(seat->name, oldseat.name))
			return -1;
	}

	return _ggzdmod_set_seat(ggzdmod, seat);
}


/* This information could be tracked every time  a player
   leaves/joins, but that's not really necessary at this point. */
/* Note also that this function doesn't directly access any
   ggzdmod data at all - it could just as easily be written
   by the game, or included in a wrapper library.  But every
   game we have uses it, so it's here for now... */
int ggzdmod_count_seats(GGZdMod *ggzdmod, GGZSeatType seat_type)
{
	int count = 0;
#if 0 /* This form could be used as part of a wrapper library. */
	int i;
	for (i=0; i<ggzdmod_get_num_seats(mod); i++)
		if (ggzdmod_get_seat(mod, i).type == seat_type)
			count++;
	return count;
#else /* This form is better and must be ggzdmod-internal. */
	GGZListEntry *entry;
	GGZSeat *seat;
	
	if (!CHECK_GGZDMOD(ggzdmod))
		return -1;

	for (entry = ggz_list_head(ggzdmod->seats);
	     entry != NULL;
	     entry = ggz_list_next(entry)) {
		
		seat = ggz_list_get_data(entry);
		if (seat->type == seat_type)
			count++;
	}
	
	return count;
#endif
	/* Note we could do even better if we tracked these
	   values on player join/leave. */
}


/* 
 * Event/Data handling
 */

#if 0
/* If anyone is left out or would prefer a different name, go right ahead and 
   change it.  No longer than 13 characters.  --JDS */
static char *bot_names[] = {
	"bcox", "Crouton", "Boffo", "Bugg", "DJH", "Dobey",
	"Dr. Maux", "jDorje", "Jzaun", "Oojah", "Perdig", "RGade",
	"riq", "rpd"
};

/* This function randomizes the order of the names assigned to bots. It is
   ENTIRELY UNNECESSARY but entertaining.  --JDS */
static void randomize_names(char **names, char **randnames, int num)
{
	char *rnames2[num];
	int i, choice;

	/* copy names array to rnames2 array */
	for (i = 0; i < num; i++)
		rnames2[i] = names[i];

	/* now pick names from rnames2 */
	for (i = 0; i < num; i++) {
		choice = random() % (num - i);
		randnames[i] = rnames2[choice];
		rnames2[choice] = rnames2[num - i - 1];
	}
}
#endif

/* FIXME: this assumes that ggzdmod-game is always deciding
   the state, and informing ggzdmod-ggz afterwards.  Once
   ggzd is able to change the state (within limited values),
   this function will no longer be viable and should be
   replaced by (I think) _ggzdmod_set_state, which can be
   called from the network/io code. */
static void set_state(GGZdMod * ggzdmod, GGZdModState state)
{
	GGZdModState old_state = ggzdmod->state;
	if (state == ggzdmod->state)
		return;		/* Is this an error? */

	/* The callback function retrieves the state from ggzdmod_get_state.
	   It could instead be passed as an argument. */
	ggzdmod->state = state;
	call_handler(ggzdmod, GGZDMOD_EVENT_STATE, &old_state);

	/* If we are the game module, send the new state to GGZ */
	if (ggzdmod->type == GGZDMOD_GAME) {
		ggzdmod_log(ggzdmod, "GGZDMOD: Game setting state to %d", 
			    state);
		if (_io_send_state(ggzdmod->fd, state) < 0)
			/* FIXME: do some sort of error handling? */
			return;
	}
}

/* Returns -1 on error, the number of events handled on success. */
static int handle_event(GGZdMod * ggzdmod, fd_set read_fds)
{
	int status, count = 0;
	GGZListEntry *entry;
	GGZSeat *seat;

	if (FD_ISSET(ggzdmod->fd, &read_fds)) {
		status = _io_read_data(ggzdmod);
		if (status < 0) {
			_ggzdmod_error(ggzdmod, "Error reading data");
			/* FIXME: should be disconnect? */
			ggzdmod_set_state(ggzdmod, GGZDMOD_STATE_DONE);
		}
		count++;
	}

	/* Only monitory player fds if there is a PLAYER_DATA handler set */
	if (ggzdmod->handlers[GGZDMOD_EVENT_PLAYER_DATA]) {
		for (entry = ggz_list_head(ggzdmod->seats);
		     entry != NULL;
		     entry = ggz_list_next(entry)) {
			
			seat = ggz_list_get_data(entry);
			if (seat->fd != -1 && FD_ISSET(seat->fd, &read_fds)) {
				call_handler(ggzdmod,
					     GGZDMOD_EVENT_PLAYER_DATA,
					     &seat->num);
				
				count++;
			}
		}
	}

	return count;
}


/* Returns number of events that occured. */
int ggzdmod_dispatch(GGZdMod * ggzdmod)
{
	struct timeval timeout;
	fd_set read_fd_set;
	int status;
	int fd_max;

	if (!CHECK_GGZDMOD(ggzdmod)) {
		return -1;
	}

	/* get_fd_max returns -1 if there are no FD's to be read. */
	fd_max = get_fd_max(ggzdmod);
	if (fd_max < 0) {
		return -1;	
	}

	read_fd_set = get_active_fd_set(ggzdmod);
	timeout.tv_sec = timeout.tv_usec = 0;	/* is this really portable? */
	
	status = select(fd_max + 1, &read_fd_set, NULL, NULL, &timeout);

	/* FIXME: if you don't have a player event handler registered, this
	   function may errantly return 0 sometimes when it should have
	   handled data.  This can be fixed later. */

	if (status <= 0) {
		if (errno == EINTR)
			return 0;
		return -1;
	}

	return handle_event(ggzdmod, read_fd_set);
}

/* unlike the old ggzd_main_loop() this one doesn't connect/disconnect. */
int ggzdmod_loop(GGZdMod * ggzdmod)
{
	if (!CHECK_GGZDMOD(ggzdmod)) {
		return -1;
	}
	while (ggzdmod->state != GGZDMOD_STATE_DONE) {
		fd_set read_fd_set;
		int status, fd_max;

		/* get_fd_max returns -1 if there are no FD's to be read. */
		fd_max = get_fd_max(ggzdmod);
		if (fd_max < 0)
			return -1;
			
		read_fd_set = get_active_fd_set(ggzdmod);

		/* we have to select so that we can determine what file
		   descriptors are waiting to be read. */
		status = select(fd_max + 1, &read_fd_set, NULL, NULL, NULL);
		if (status <= 0) {
			if (errno != EINTR) {
				/* FIXME: handle error */
			}
			continue;
		}

		(void) handle_event(ggzdmod, read_fd_set);
	}
	return 0;		/* should handle errors */
}

int ggzdmod_set_state(GGZdMod * ggzdmod, GGZdModState state)
{
	if (!CHECK_GGZDMOD(ggzdmod))
		return -1;
	
	if (ggzdmod->type == GGZDMOD_GAME) {
		/* The game may only change the state from one of
		   these two states. */
		if (ggzdmod->state != GGZDMOD_STATE_WAITING &&
		    ggzdmod->state != GGZDMOD_STATE_PLAYING)
			return -1;

		/* The game may only change the state to one of
		   these three states. */
		if (state == GGZDMOD_STATE_PLAYING ||
		    state == GGZDMOD_STATE_WAITING ||
		    state == GGZDMOD_STATE_DONE)
			set_state(ggzdmod, state);
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
 * ggzd specific actions
 */

/* Sends a game launch packet to ggzdmod-game. A negative return value
   indicates a serious (fatal) error. */
/* No locking should be necessary within this function. */
static int send_game_launch(GGZdMod * ggzdmod)
{
	GGZListEntry *entry;
	GGZSeat *seat;
		

	if (_io_send_launch(ggzdmod->fd, ggzdmod->num_seats) < 0) {
		_ggzdmod_error(ggzdmod, "Error writing to game");
		return -1;
	}

	for (entry = ggz_list_head(ggzdmod->seats);
	     entry != NULL;
	     entry = ggz_list_next(entry)) {
		
		seat = ggz_list_get_data(entry);
		if (_io_send_seat(ggzdmod->fd, seat) < 0) {
			_ggzdmod_error(ggzdmod, "Error writing to game");
			return -1;
		}
	}
	
	return 0;
}


/* Forks the game.  A negative return value indicates a serious error. */
/* No locking should be necessary within this function. */
static int game_fork(GGZdMod * ggzdmod)
{
	int pid;
	int fd_pair[2];		/* socketpair */

	/* If there are no args, we don't know what to run! */
	if (ggzdmod->argv == NULL || ggzdmod->argv[0] == NULL) {
		_ggzdmod_error(ggzdmod, "No arguments");
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

		/* FIXME: can we call ggzdmod_log() from here? */
		execv(ggzdmod->argv[0], ggzdmod->argv);	/* run game */

		/* We should never get here.  If we do, it's an eror */
		/* we still can't send error messages... */
		ggz_error_sys_exit("exec of %s failed", ggzdmod->argv[0]);
	} else {
		/* parent */
		close(fd_pair[1]);

		ggzdmod->fd = fd_pair[0];
		ggzdmod->pid = pid;
		
		/* FIXME: should we delete the argv arguments? */
		
		/* That's all! */
	}
	return 0;
}


int ggzdmod_connect(GGZdMod * ggzdmod)
{
	if (!CHECK_GGZDMOD(ggzdmod)) {
		return -1;
	}

	if (ggzdmod->type == GGZDMOD_GGZ) {
		/* For the ggz side, we fork the game and then send the launch message */
		
		if (game_fork(ggzdmod) < 0) {
			_ggzdmod_error(ggzdmod, "Error: table fork failed");
			return -1;
		}
		
		if (send_game_launch(ggzdmod) < 0) {
			_ggzdmod_error(ggzdmod, "Error sending launch to game");
			/* FIXME: this might result in a forked but unused table. */
			/* FIXME: it might also result in a ggzdmod with
			   initialized but wrong data in it. */
			return -1;
		}
	} else {
		/* For the game side we setup the fd */
		ggzdmod->fd = 3;
		
		if (ggzdmod_log(ggzdmod, "GGZDMOD: Connecting to GGZ server.") < 0) {
			ggzdmod->fd = -1;
			/* I'm not entirely sure if we _should_ be printing
			   this stuff here, but it certainly is convenient
			   to not have to do it for every game. */
			fprintf(stderr, "\nCouldn't connect to GGZ!\n\n"
					"Most likely this occurred because you ran\n"
					"the server from the command line.  Instead\n"
					"you should connect to a GGZ server and\n"
					"launch a game through the client.\n\n");
			return -1;
		}
	}
	
	/* We used to return the GGZdmod file descriptor, we really
	   don't even want to tempt games to use this socket. */
	return 0; /* success */
}

int ggzdmod_disconnect(GGZdMod * ggzdmod)
{
	if (!CHECK_GGZDMOD(ggzdmod) || ggzdmod->fd < 0 ) {
		return -1;
	}

	if (ggzdmod->type == GGZDMOD_GGZ) {
		/* For the ggz side, we kill the game server and close the socket */
		
		/* Make sure game server is dead */
		if (ggzdmod->pid > 0)
			kill(ggzdmod->pid, SIGINT);
		ggzdmod->pid = -1;
		
		set_state(ggzdmod, GGZDMOD_STATE_DONE);
		/* FIXME: should we wait() to get an exit status?? */
		/* FIXME: what other cleanups should we do? */
	} else {
		/* For client the game side we send a game over message */
		
		/* First warn the server of halt (if we haven't already) */
		ggzdmod_set_state(ggzdmod, GGZDMOD_STATE_DONE);
		ggzdmod_log(ggzdmod, "GGZDMOD: Disconnected from GGZ server.");
	}
	
	/* We no longer free the seat data here.  It will stick around until
	   ggzdmod_free is called or it is used again. */

	/* Clean up the ggzdmod object.  In theory it could now reconnect for
	   a new game. */
	close(ggzdmod->fd);
	ggzdmod->fd = -1;

	return 0;
}


/* 
 * module specific actions
 */

int ggzdmod_log(GGZdMod * ggzdmod, char *fmt, ...)
{
	char buf[4096];
	va_list ap;

	if (!CHECK_GGZDMOD(ggzdmod) || !fmt ||
	    (ggzdmod->type == GGZDMOD_GAME && ggzdmod->fd < 0)) {
		/* This will happen when ggzdmod_log is called before
		   connection.  We could store the buffer for later, but... */
		return -1;
	}

	va_start(ap, fmt);
	vsnprintf(buf, sizeof(buf), fmt, ap);
	va_end(ap);
	
	if (ggzdmod->type == GGZDMOD_GAME) {
		if (_io_send_log(ggzdmod->fd, buf) < 0) {
			_ggzdmod_error(ggzdmod, "Error writing to GGZ");
			return -1;
		}
	} else {
		/* This is here mainly so that ggzdmod can use
		   ggzdmod_log internally. */
		_ggzdmod_handle_log(ggzdmod, buf);
	}

	return 0;
}


/**** Internal library functions ****/

void _ggzdmod_error(GGZdMod *ggzdmod, char* error)
{
	call_handler(ggzdmod, GGZDMOD_EVENT_ERROR, error);
}


void _ggzdmod_handle_join_response(GGZdMod * ggzdmod, char status)
{
	if (status == 0)
		call_handler(ggzdmod, GGZDMOD_EVENT_JOIN, NULL);
	else
		_ggzdmod_error(ggzdmod, "Player failed to join");
}


void _ggzdmod_handle_leave_response(GGZdMod * ggzdmod, char status)
{
	if (status == 0)
		call_handler(ggzdmod, GGZDMOD_EVENT_LEAVE, &status);
	else
		_ggzdmod_error(ggzdmod, "Player failed to leave");
}


void _ggzdmod_handle_seat_response(GGZdMod * ggzdmod, char status)
{
	ggz_debug("GGZDMOD", "Handling RSP_GAME_SEAT");
	if (status == 0)
		call_handler(ggzdmod, GGZDMOD_EVENT_SEAT, NULL);
	else
		_ggzdmod_error(ggzdmod, "Seat change failed");
}


void _ggzdmod_handle_state(GGZdMod * ggzdmod, GGZdModState state)
{
	_io_respond_state(ggzdmod->fd);

	/* There's only certain ones the game is allowed to set it to,
	   and they can only change it if the state is currently
	   WAITING or PLAYING. */
	switch (state) {
	case GGZDMOD_STATE_WAITING:
	case GGZDMOD_STATE_PLAYING:
	case GGZDMOD_STATE_DONE:
		/* In contradiction to what I say above, the game
		   actually _is_ allowed to change its state from
		   CREATED to WAITING.  When ggzdmod-ggz sends a
		   launch packet to ggzdmod-game, ggzdmod-game
		   automatically changes the state from CREATED
		   to WAITING.  When this happens, it tells
		   ggzdmod-ggz of this change and we end up back
		   here.  So, although it's a bit unsafe, we have
		   to allow this for now.  The alternative would
		   be to have ggzdmod-ggz and ggzdmod-game both
		   separately change states when the launch packet
		   is sent. */
		set_state(ggzdmod, state);
		break;
	default:
		_ggzdmod_error(ggzdmod,
			       "Game requested incorrect state value");
	}
	
	/* Is this right? has the gameover happened yet? */   
}


void _ggzdmod_handle_log(GGZdMod * ggzdmod, char *msg)
{
	call_handler(ggzdmod, GGZDMOD_EVENT_LOG, msg);
}


/* Game-side event: launch event received from ggzd */
void _ggzdmod_handle_launch_begin(GGZdMod * ggzdmod, int num_seats)
{
#if 0
	int bots = 0;
#define NUM_BOT_NAMES (sizeof(bot_names)/sizeof(bot_names[0]))
	char *rand_bot_names[NUM_BOT_NAMES];

	randomize_names(bot_names, rand_bot_names, NUM_BOT_NAMES);
#endif

	if (num_seats <= 0) {
		_ggzdmod_error(ggzdmod, "Error: %d seats received from GGZ");
		ggzdmod_log(ggzdmod,
			    "GGZDMOD: ERROR: %d seats sent upon launch.",
			    num_seats);
		return;
	}

	_ggzdmod_set_num_seats(ggzdmod, num_seats);
}


void _ggzdmod_handle_launch_seat(GGZdMod * ggzdmod, GGZSeat seat)
{
	switch (seat.type) {
	case GGZ_SEAT_OPEN:
		ggzdmod_log(ggzdmod, "GGZDMOD: Seat %d is open", seat.num);
		break;

	case GGZ_SEAT_BOT:
		seat.name = ggz_strdup("AI");
#if 0		
		seat.name = "AI";
		len = strlen(rand_bot_names[bots]) + 4;
		seat.name = ggz_malloc(len);

		snprintf(seat.name, len, "%s-AI",
			 rand_bot_names[bots]);
		bots++;
#endif
		ggzdmod_log(ggzdmod, "GGZDMOD: Seat %d is a bot named %s",
			    seat.num, seat.name);
		break;

	case GGZ_SEAT_RESERVED:
		ggzdmod_log(ggzdmod, "GGZDMOD: Seat %d reserved for %s",
			    seat.num, seat.name);
		break;
	default:
		_ggzdmod_error(ggzdmod,
			       "Error: received unknown seat from GGZ");
		ggzdmod_log(ggzdmod, "GGZDMOD: Unknown seat type %d",
			    seat.type);
	}


	if (_ggzdmod_set_seat(ggzdmod, &seat) < 0) {
		_ggzdmod_error(ggzdmod, "Error setting seat");
		ggzdmod_log(ggzdmod, "GGZDMOD: Error setting seat");
		seat_print(ggzdmod, &seat);
	}
}


void _ggzdmod_handle_launch_end(GGZdMod * ggzdmod)
{
	/* Normally we let the game control its own state, but
	   we control the transition from CREATED to WAITING. */
        set_state(ggzdmod, GGZDMOD_STATE_WAITING);
}


/* game-side event: player join event received from ggzd */
void _ggzdmod_handle_join(GGZdMod * ggzdmod, GGZSeat seat)
{
	if (_ggzdmod_set_seat(ggzdmod, &seat) < 0) {
		/* Fatal error. */
		ggzdmod_log(ggzdmod, "GGZDMOD: _ggzdmod_handle_join: "
				     "_ggzdmod_set_seat unsuccessful.");
		_ggzdmod_error(ggzdmod, "Failed join.");
	}
	ggzdmod_log(ggzdmod, "GGZDMOD: %s on fd %d in seat %d",
		    seat.name, seat.fd, seat.num);
	
	call_handler(ggzdmod, GGZDMOD_EVENT_JOIN, &seat.num);

	if (_io_respond_join(ggzdmod->fd) < 0) {
		_ggzdmod_error(ggzdmod, "Error sending data to GGZ");
		return;
	}

	/* Most games will want to change their status to PLAYING
	   at this point, but we leave that entirely up to them
	   (via ggzdmod_set_state()). */
}


/* game-side event: player leave received from ggzd */
void _ggzdmod_handle_leave(GGZdMod * ggzdmod, char *name)
{
	char status = -1;
	GGZListEntry *entry;
	GGZSeat seat;

	/* Set seat name, and see if we find anybody who matches */
	seat.name = name;
	entry = ggz_list_search_alt(ggzdmod->seats, &seat,
				    (ggzEntryCompare)seat_find_player);
	
	if (entry) {
		status = 0;
		seat = *(GGZSeat*)ggz_list_get_data(entry);
		ggzdmod_log(ggzdmod, "GGZDMOD: Removed %s from seat %d",
			    seat.name, seat.num);
			    
		/* Reset seat to open state, and reinsert into list */
		seat.fd = -1;
		seat.name = NULL;
		seat.type = GGZ_SEAT_OPEN;
		ggz_list_insert(ggzdmod->seats, &seat);
		call_handler(ggzdmod, GGZDMOD_EVENT_LEAVE, &seat.num);
	

		/* Most games will want to change their status to
		   WAITING at this point, but we leave that entirely up
		   to them(via ggzdmod_set_state()). */
	}

	else
		_ggzdmod_error(ggzdmod,
			       "Error: non-existant player tried to leave");


	if (_io_respond_leave(ggzdmod->fd, status) < 0)
		_ggzdmod_error(ggzdmod, "Error sending data to GGZ");

}


/* game-side event: seat change event received from ggzd */
void _ggzdmod_handle_seat(GGZdMod * ggzdmod, GGZSeat seat)
{
	if (_ggzdmod_set_seat(ggzdmod, &seat) < 0) {
		/* Fatal error. */
		ggzdmod_log(ggzdmod, "GGZDMOD: _ggzdmod_handle_seat: "
			    "_ggzdmod_set_seat unsuccessful.");
		_ggzdmod_error(ggzdmod, "Failed join.");
	}
	
	ggzdmod_log(ggzdmod, "Seat %d is now %s (%s)", seat.num, 
		    ggz_seattype_to_string(seat.type), seat.name);

	call_handler(ggzdmod, GGZDMOD_EVENT_SEAT, &seat.num);
 
	if (_io_respond_seat(ggzdmod->fd, 0) < 0) {
		_ggzdmod_error(ggzdmod, "Error sending data to GGZ");
		return;
	}
}


/* game-side event: game state response received from ggzd */
void _ggzdmod_handle_state_response(GGZdMod * ggzdmod)
{
	/* FIXME: what do we do here? */
}


/* Create a new copy of a seat object */
static GGZSeat* seat_copy(GGZSeat *orig)
{
	GGZSeat *seat;

	seat = ggz_malloc(sizeof(GGZSeat));

	seat->num = orig->num;
	seat->type = orig->type;
	seat->fd = orig->fd;
	
	/* The name may be NULL if unknown. */
	seat->name = orig->name ? ggz_strdup(orig->name) : NULL;
	
	return seat;
}


static int seat_compare(GGZSeat *a, GGZSeat *b)
{
	return a->num - b->num;
}


/* Because of the way ggz_list is set up, this function
   is supposed to return 0 if seats a and b match,
   1 otherwise. */
static int seat_find_player(GGZSeat *a, GGZSeat *b)
{
	/* It is possible that one of the seats will have a
	   NULL name.  In this case, we do _not_ want to
	   return a match.  This wouldn't be an issue if we
	   handled things the "right" way and earched only
	   by seat number or player UID.  --JDS */
	if (!a->name || !b->name)
		return 1;

	return strcmp(a->name, b->name);
}


static void seat_free(GGZSeat *seat)
{
	if (seat->fd != -1)
		close(seat->fd);
	if (seat->name)
		ggz_free(seat->name);

	ggz_free(seat);
}

static void seat_print(GGZdMod * ggzdmod, GGZSeat * seat)
{
	char *type = ggz_seattype_to_string(seat->type);
	ggzdmod_log(ggzdmod, "GGZDMOD: Seat %d is %s (%s) on %d",
		    seat->num, type, seat->name, seat->fd);
}


void ggzdmod_check(GGZdMod *ggzdmod)
{
	GGZListEntry *entry;
	if (!CHECK_GGZDMOD(ggzdmod))
		return;
		
	ggzdmod_log(ggzdmod, "--- GGZDMOD CHECK ---");
	ggzdmod_log(ggzdmod, "    TYPE: %s",
		    ggzdmod->type == GGZDMOD_GAME ? "GGZDMOD_GAME" :
		    ggzdmod->type == GGZDMOD_GGZ ?  "GGZDMOD_GGZ"  :
		    "UNKNOWN");
	ggzdmod_log(ggzdmod, "    FD: %d.", ggzdmod->fd);

	for (entry = ggz_list_head(ggzdmod->seats);
	     entry != NULL;
	     entry = ggz_list_next(entry)) {
		
		GGZSeat *seat = ggz_list_get_data(entry);
	
		ggzdmod_log(ggzdmod, "    Seat %d | type=%s | name=%s | fd=%d",
			    seat->num,
			    ggz_seattype_to_string(seat->type),
			    seat->name ? seat->name : "<none>",
			    seat->fd);
		
	}
	
	ggzdmod_log(ggzdmod, "--- GGZDMOD CHECK COMPLETE ---");
}
