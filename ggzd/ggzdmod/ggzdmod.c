/* 
 * File: ggzdmod.c
 * Author: GGZ Dev Team
 * Project: ggzdmod
 * Date: 10/14/01
 * Desc: GGZ game module functions
 * $Id: ggzdmod.c 2767 2001-12-01 06:44:49Z bmh $
 *
 * This file contains the backend for the ggzdmod library.  This
 * library facilitates the communication between the GGZ server (ggzd)
 * and game servers.  This file provides backend code that can be
 * used at both ends.
 *
 * Copyright (C) 2001 GGZ Dev Team.
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
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <easysock.h>
#include <ggz.h>

#include "ggzdmod.h"
#include "mod.h"
#include "io.h"
#include "protocol.h"


/* This checks the ggzdmod object for validity.  It could do more checking if
   desired. */
#define CHECK_GGZDMOD(ggzdmod) (ggzdmod)



/* 
 * internal functions
 */


/* Functions for manipulating seats */
static GGZSeat* seat_copy(GGZSeat *orig);
static int seat_compare(GGZSeat *a, GGZSeat *b);
static int seat_find_player(GGZSeat *a, GGZSeat *b);
static void seat_free(GGZSeat *seat);
static void dump_seats(_GGZdMod *mod);
static int seats_open(_GGZdMod* mod);
static void seat_print(_GGZdMod* mod, GGZSeat *seat);

/* Invokes handlers for the specefied event */
static void call_handler(_GGZdMod *mod, GGZdModEvent event, void *data)
{
	if (mod->handlers[event])
		(*mod->handlers[event]) (mod, event, data);
}


/* Returns the highest-numbered FD used by ggzdmod. */
static int get_fd_max(_GGZdMod * mod)
{
	int max = mod->fd;
	GGZListEntry *entry;
	GGZSeat *seat;

	/* If we don't have a player data handler set
	   up, we won't monitor the player data handlers. */
	if (mod->handlers[GGZDMOD_EVENT_PLAYER_DATA])
		for (entry = ggz_list_head(mod->seats);
		     entry != NULL;
		     entry = ggz_list_next(entry)) {

			seat = ggz_list_get_data(entry);
			if (seat->fd > max)
				max = seat->fd;
		}
	
	return max;
}


static fd_set get_active_fd_set(_GGZdMod *mod)
{
	fd_set active_fd_set;
	GGZListEntry *entry;
	GGZSeat *seat;
	
	FD_ZERO(&active_fd_set);
	if (mod->fd != -1)
		FD_SET(mod->fd, &active_fd_set);

	/* Only monitory player fds if there is a PLAYER_DATA handler set */
	if (mod->handlers[GGZDMOD_EVENT_PLAYER_DATA]) {
		for (entry = ggz_list_head(mod->seats);
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
	_GGZdMod *ggzdmod;

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
void ggzdmod_free(GGZdMod * mod)
{
	int i;
	_GGZdMod *ggzdmod = mod;

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
int ggzdmod_get_fd(GGZdMod * mod)
{
	_GGZdMod *ggzdmod = mod;
	if (!CHECK_GGZDMOD(ggzdmod)) {
		return -1;
	}
	return ggzdmod->fd;
}


GGZdModType ggzdmod_get_type(GGZdMod * mod)
{
	_GGZdMod *ggzdmod = mod;
	if (!CHECK_GGZDMOD(ggzdmod)) {
		return -1;	/* not very useful */
	}
	return ggzdmod->type;
}


GGZdModState ggzdmod_get_state(GGZdMod * mod)
{
	_GGZdMod *ggzdmod = mod;
	if (!CHECK_GGZDMOD(ggzdmod)) {
		return -1;	/* not very useful */
	}
	return ggzdmod->state;
}


int ggzdmod_get_num_seats(GGZdMod * mod)
{
	_GGZdMod *ggzdmod = mod;
	if (!CHECK_GGZDMOD(ggzdmod)) {
		return -1;
	}
	return ggzdmod->num_seats;
}


GGZSeat ggzdmod_get_seat(GGZdMod * mod, int num)
{
	GGZSeat seat = {num, GGZ_SEAT_NONE, NULL, -1};
	GGZListEntry *entry;

	_GGZdMod *ggzdmod = mod;
	if (CHECK_GGZDMOD(ggzdmod) && num >= 0 && num < ggzdmod->num_seats) {
		if ( (entry = ggz_list_search(ggzdmod->seats, &seat)))
			seat = *(GGZSeat*)ggz_list_get_data(entry);
	}
	
	return seat;
}


void* ggzdmod_get_gamedata(GGZdMod * mod)
{
	_GGZdMod *ggzdmod = mod;
	if (!CHECK_GGZDMOD(ggzdmod)) {
		return NULL;
	}
	return ggzdmod->gamedata;
}

static void _ggzdmod_set_num_seats(_GGZdMod *mod, int num_seats)
{
	GGZSeat seat;
	int i, old_num;

	old_num = mod->num_seats;
	
	/* If the table size is increasing, add more seats */
	if (num_seats > old_num) {
		/* Initialize new seats */
		for (i = old_num; i < num_seats; i++) {
			seat.num = i;
			seat.type = GGZ_SEAT_OPEN;
			seat.name = NULL;
			seat.fd = -1;
			ggz_list_insert(mod->seats, &seat);
		}
	}
	/* If the table size is increasing, add more seats */
	else if (num_seats < old_num) {
		/* FIXME: delete extra seats */
	}
	
	mod->num_seats = num_seats;
}


void ggzdmod_set_num_seats(GGZdMod * mod, int num_seats)
{
	_GGZdMod *ggzdmod = mod;
	
	/* Check parameters */
	if (!CHECK_GGZDMOD(ggzdmod) || num_seats < 0
	    || ggzdmod->type != GGZDMOD_GGZ) {
		return;		/* not very useful */
	}
	
	_ggzdmod_set_num_seats(mod, num_seats);
}
			   
void ggzdmod_set_module(GGZdMod * mod, char **argv)
{
	_GGZdMod *ggzdmod = mod;
	int i;

	ggz_debug("GGZDMOD", "Setting arguments");
	
	if (!CHECK_GGZDMOD(ggzdmod))
		return;

	if (ggzdmod->type != GGZDMOD_GGZ) {
		_ggzdmod_error(mod, "Cannot set module args from module");
		return;
	}
		
	/* Check parameters */
	if (!argv || !argv[0]) {
		_ggzdmod_error(mod, "Bad module arguments");
		return;
	}

	/* Count the number of args so we know how much to allocate */
	for (i = 0; argv[i]; i++) {}

	ggz_debug("GGZDMOD", "Set %d arguments", i);
	
	ggzdmod->argv = ggz_malloc(sizeof(char*)*(i+1));
	
	for (i = 0; argv[i]; i++) 
		ggzdmod->argv[i] = ggz_strdup(argv[i]);
}


void ggzdmod_set_gamedata(GGZdMod * mod, void * data)
{
	_GGZdMod *ggzdmod = mod;

	/* Check parameters */
	if (!CHECK_GGZDMOD(ggzdmod)) {
		return;		/* not very useful */
	}

	ggzdmod->gamedata = data;
}


void ggzdmod_set_handler(GGZdMod * mod, GGZdModEvent e, GGZdModHandler func)
{
	_GGZdMod *ggzdmod = mod;

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


static int _ggzdmod_set_seat(_GGZdMod * mod, GGZSeat *seat)
{
	GGZSeat oldseat = ggzdmod_get_seat(mod, seat->num);
	
	/* If we're connected to the game, send a message */
	if (mod->type == GGZDMOD_GGZ && mod->state != GGZDMOD_STATE_CREATED) {
		
		if (oldseat.type == GGZ_SEAT_OPEN 
		    && seat->type == GGZ_SEAT_PLAYER) {
			if (_io_send_join(mod->fd, seat) < 0)
				_ggzdmod_error(mod, "Error writing to game");
		}
		else if (oldseat.type == GGZ_SEAT_PLAYER 
			 && seat->type == GGZ_SEAT_OPEN) {
			if (_io_send_leave(mod->fd, &oldseat) < 0)
				_ggzdmod_error(mod, "Error writing to game");
		}
	}

	ggz_list_insert(mod->seats, seat);
	
	
	return 0;
}


int ggzdmod_set_seat(GGZdMod * mod, GGZSeat *seat)
{
	_GGZdMod *ggzdmod = mod;
	GGZSeat oldseat;
	if (!CHECK_GGZDMOD(ggzdmod) || seat->num < 0 
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
		if (seat->fd != oldseat.fd || seat->type != oldseat.type)
			return -1;
		/* For now, we allow games to change the names of Bot players,
		   but that's it.  This information is _not_ transmitted back
		   to ggzd (yet). */
		if (oldseat.type != GGZ_SEAT_BOT && strings_differ(seat->name, oldseat.name))
			return -1;
	}

	return _ggzdmod_set_seat(mod, seat);
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

static void set_state(_GGZdMod * ggzdmod, GGZdModState state)
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

/* Returns -1 on error (?). */

static int handle_event(_GGZdMod * mod, fd_set read_fds)
{
	int status, count = 0;
	GGZListEntry *entry;
	GGZSeat *seat;

	if (FD_ISSET(mod->fd, &read_fds)) {
		status = _io_read_data(mod);
		if (status < 0) {
			_ggzdmod_error(mod, "Error reading data");
			/* FIXME: should be disconnect? */
			ggzdmod_halt_table(mod);
		}
		count++;
	}

	/* Only monitory player fds if there is a PLAYER_DATA handler set */
	if (mod->handlers[GGZDMOD_EVENT_PLAYER_DATA]) {
		for (entry = ggz_list_head(mod->seats);
		     entry != NULL;
		     entry = ggz_list_next(entry)) {
			
			seat = ggz_list_get_data(entry);
			if (seat->fd != -1 && FD_ISSET(seat->fd, &read_fds)) {
				call_handler(mod, GGZDMOD_EVENT_PLAYER_DATA, 
					     &seat->num);
				
				count++;
			}
		}
	}

	return count;
}


/* Returns number of events that occured. */
int ggzdmod_dispatch(GGZdMod * mod)
{
	_GGZdMod *ggzdmod = mod;
	struct timeval timeout;
	fd_set read_fd_set;
	int status;

	if (!CHECK_GGZDMOD(ggzdmod)) {
		return -1;
	}

	read_fd_set = get_active_fd_set(ggzdmod);
	timeout.tv_sec = timeout.tv_usec = 0;	/* is this really portable? */

	status = select(get_fd_max(ggzdmod) + 1,
			&read_fd_set, NULL, NULL, &timeout);

	/* FIXME: if you don't have a player event handler registered, this
	   function may errantly return 0 sometimes when it should have
	   handled data.  This can be fixed later. */

	if (status <= 0) {
		if (errno == EINTR)
			return 1;
		return 0;
	}

	return handle_event(ggzdmod, read_fd_set);
}

/* unlike the old ggzd_main_loop() this one doesn't connect/disconnect. */
int ggzdmod_loop(GGZdMod * mod)
{
	_GGZdMod *ggzdmod = mod;
	if (!CHECK_GGZDMOD(ggzdmod)) {
		return -1;
	}
	while (ggzdmod->state != GGZDMOD_STATE_DONE) {
		fd_set read_fd_set;
		int status;

		read_fd_set = get_active_fd_set(ggzdmod);

		/* we have to select so that we can determine what file
		   descriptors are waiting to be read. */
		status = select(get_fd_max(ggzdmod) + 1,
				&read_fd_set, NULL, NULL, NULL);
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

int ggzdmod_halt_table(GGZdMod * mod)
{
	_GGZdMod *ggzdmod = mod;
	if (!CHECK_GGZDMOD(ggzdmod) || ggzdmod->type != GGZDMOD_GAME) {
		return -1;
	}
	if (ggzdmod->type == GGZDMOD_GAME) {
		set_state(ggzdmod, GGZDMOD_STATE_DONE);
	} else {
		/* TODO: an extension to the communications protocol will be
		   needed for halt_game to work ggz-side.  Let's get the rest 
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
static int send_game_launch(_GGZdMod * mod)
{
	GGZListEntry *entry;
	GGZSeat *seat;
		

	if (_io_send_launch(mod->fd, mod->num_seats) < 0) {
		_ggzdmod_error(mod, "Error writing to game");
		return -1;
	}

	for (entry = ggz_list_head(mod->seats);
	     entry != NULL;
	     entry = ggz_list_next(entry)) {
		
		seat = ggz_list_get_data(entry);
		if (_io_send_seat(mod->fd, seat) < 0) {
			_ggzdmod_error(mod, "Error writing to game");
			return -1;
		}
	}
	
	return 0;
}


/* Forks the game.  A negative return value indicates a serious error. */
/* No locking should be necessary within this function. */
static int game_fork(_GGZdMod * mod)
{
	int pid;
	int fd_pair[2];		/* socketpair */

	/* If there are no args, we don't know what to run! */
	if (mod->argv == NULL || mod->argv[0] == NULL) {
		_ggzdmod_error(mod, "No arguments");
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
		execv(mod->argv[0], mod->argv);	/* run game */

		/* We should never get here.  If we do, it's an eror */
		ggz_error_sys_exit("exec of %s failed", mod->argv[0]);	/* we still can't send error messages */
	} else {
		/* parent */
		close(fd_pair[1]);

		mod->fd = fd_pair[0];
		mod->pid = pid;
		
		/* FIXME: should we delete the argv arguments? */
		
		/* That's all! */
	}
	return 0;
}


int ggzdmod_connect(GGZdMod * mod)
{
	_GGZdMod *ggzdmod = mod;
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
			return -1;
		}
		
		return 0;
		
	} else {
		/* For the game side we setup the fd */
		ggzdmod->fd = 3;
		
		if (ggzdmod_log(ggzdmod, "GGZDMOD: Connecting to GGZ server.") < 0) {
			ggzdmod->fd = -1;
			return -1;
		}
		
		return ggzdmod->fd;
	}
}

int ggzdmod_disconnect(GGZdMod * mod)
{
	_GGZdMod *ggzdmod = mod;
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
		ggzdmod_halt_table(ggzdmod);
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

int ggzdmod_log(GGZdMod * mod, char *fmt, ...)
{
	_GGZdMod *ggzdmod = mod;
	char buf[4096];
	va_list ap;

	if (!CHECK_GGZDMOD(ggzdmod) || !fmt || ggzdmod->fd < 0
	    || ggzdmod->type != GGZDMOD_GAME) {
		/* This will happen when ggzdmod_log is called before
		   connection.  We could store the buffer for later, but... */
		return -1;
	}

	va_start(ap, fmt);
	vsnprintf(buf, sizeof(buf), fmt, ap);
	va_end(ap);

	if (_io_send_log(ggzdmod->fd, buf) < 0) {
		_ggzdmod_error(ggzdmod, "Error writing to GGZ");
		return -1;
	}

	return 0;
}


/**** Internal library functions ****/

void _ggzdmod_error(_GGZdMod *mod, char* error)
{
	call_handler(mod, GGZDMOD_EVENT_ERROR, error);
}


void _ggzdmod_handle_join_response(_GGZdMod * mod, char status)
{
	if (status == 0)
		call_handler(mod, GGZDMOD_EVENT_JOIN, NULL);
	else
		_ggzdmod_error(mod, "Player failed to join");
}


void _ggzdmod_handle_leave_response(_GGZdMod * mod, char status)
{
	if (status == 0)
		call_handler(mod, GGZDMOD_EVENT_LEAVE, &status);
	else
		_ggzdmod_error(mod, "Player failed to leave");
}


void _ggzdmod_handle_state(_GGZdMod * mod, GGZdModState state)
{
	_io_respond_state(mod->fd);

	/* There's only certain ones the game is allowed to set it to */
	switch (state) {
	case GGZDMOD_STATE_WAITING:
	case GGZDMOD_STATE_PLAYING:
	case GGZDMOD_STATE_DONE:
		set_state(mod, state);
		break;
	default:
		_ggzdmod_error(mod, "Game requested incorrect state value");
	}
	
	/* Is this right? has the gameover happened yet? */   
}


void _ggzdmod_handle_log(_GGZdMod * mod, char *msg)
{
	call_handler(mod, GGZDMOD_EVENT_LOG, msg);
}


/* Game-side event: launch event received from ggzd */
void _ggzdmod_handle_launch_begin(_GGZdMod * mod, int num_seats)
{
#if 0
	int bots = 0;
#define NUM_BOT_NAMES (sizeof(bot_names)/sizeof(bot_names[0]))
	char *rand_bot_names[NUM_BOT_NAMES];

	randomize_names(bot_names, rand_bot_names, NUM_BOT_NAMES);
#endif

	if (num_seats <= 0) {
		_ggzdmod_error(mod, "Error: %d seats received from GGZ");
		ggzdmod_log(mod, "GGZDMOD: ERROR: %d seats sent upon launch.",
			    num_seats);
		return;
	}

	_ggzdmod_set_num_seats(mod, num_seats);
}


void _ggzdmod_handle_launch_seat(_GGZdMod * mod, GGZSeat seat)
{

	switch (seat.type) {
	case GGZ_SEAT_OPEN:
		ggzdmod_log(mod, "GGZDMOD: Seat %d is open", seat.num);
		break;

	case GGZ_SEAT_BOT:
		seat.name = ggz_strdup("AI");
#if 0		
		seat.name = "AI";
		len = strlen(rand_bot_names[bots]) + 4;
		seat.name = malloc(len);

		snprintf(seat.name, len, "%s-AI",
			 rand_bot_names[bots]);
		bots++;
#endif
		ggzdmod_log(mod, "GGZDMOD: Seat %d is a bot named %s", 
			    seat.num, seat.name);
		break;

	case GGZ_SEAT_RESV:
		ggzdmod_log(mod, "GGZDMOD: Seat %d reserved for %s", seat.num,
			    seat.name);
		break;
	default:
		_ggzdmod_error(mod, "Error: received unknown seat from GGZ");
		ggzdmod_log(mod, "GGZDMOD: Unknown seat type %d", 
			    seat.type);
	}


	if (_ggzdmod_set_seat(mod, &seat) < 0) {
		_ggzdmod_error(mod, "Error setting seat");
		ggzdmod_log(mod, "GGZDMOD: Error setting seat");
		seat_print(mod, &seat);
	}
}


void _ggzdmod_handle_launch_end(_GGZdMod * mod)
{
        set_state(mod, GGZDMOD_STATE_WAITING);
}


/* game-side event: player join event received from ggzd */
void _ggzdmod_handle_join(_GGZdMod * mod, GGZSeat seat)
{
	_ggzdmod_set_seat(mod, &seat);
	ggzdmod_log(mod, "GGZDMOD: %s on %d in seat %d", seat.name, seat.fd, 
		    seat.num);
	
	call_handler(mod, GGZDMOD_EVENT_JOIN, &seat.num);

	if (_io_respond_join(mod->fd) < 0) {
		_ggzdmod_error(mod, "Error sending data to GGZ");
		return;
	}

	/* FIXME: in the future, leave this to game module? */
	/* If all seats are full now, set state to playing */
	if (!seats_open(mod))
		set_state(mod, GGZDMOD_STATE_PLAYING);
}


/* game-side event: player leave received from ggzd */
void _ggzdmod_handle_leave(_GGZdMod * mod, char *name)
{
	char status = -1;
	GGZListEntry *entry;
	GGZSeat seat;

	/* Set seat name, and see if we find anybody who matches */
	seat.name = name;
	entry = ggz_list_search_alt(mod->seats, &seat, 
				    (ggzEntryCompare)seat_find_player);
	
	if (entry) {
		status = 0;
		seat = *(GGZSeat*)ggz_list_get_data(entry);
		ggzdmod_log(mod, "GGZDMOD: Removed %s from seat %d", 
			    seat.name, seat.num);
			    
		/* Reset seat to open state, and reinsert into list */
		seat.fd = -1;
		seat.name = NULL;
		seat.type = GGZ_SEAT_OPEN;
		ggz_list_insert(mod->seats, &seat);
		call_handler(mod, GGZDMOD_EVENT_LEAVE, &seat.num);
	
		/* If the table isn't done, set to wait for more players */
		/* FIXME: let game module do this? */
		if (mod->state != GGZDMOD_STATE_DONE)
			set_state(mod, GGZDMOD_STATE_WAITING);
	}

	else
		_ggzdmod_error(mod, "Error: non-existant player tried to leave");


	if (_io_respond_leave(mod->fd, status) < 0)
		_ggzdmod_error(mod, "Error sending data to GGZ");

}


/* game-side event: game state response received from ggzd */
void _ggzdmod_handle_state_response(_GGZdMod * ggzdmod)
{
	/* FIXME: what do we do here? */
}






static void dump_seats(_GGZdMod *mod)
{
	GGZListEntry *entry;
	GGZSeat *seat;

	ggzdmod_log(mod, "GGZDMOD: Begin Seat dump (%d):", 
		    ggzdmod_get_num_seats(mod));
	for (entry = ggz_list_head(mod->seats);
	     entry != NULL;
	     entry = ggz_list_next(entry)) {
		
		seat = ggz_list_get_data(entry);
		seat_print(mod, seat);
	}
	ggzdmod_log(mod, "GGZDMOD: End Seat dump");
}


static int seats_open(_GGZdMod* mod)
{
	GGZListEntry *entry;
	GGZSeat *seat;

	for (entry = ggz_list_head(mod->seats);
	     entry != NULL;
	     entry = ggz_list_next(entry)) {
		
		seat = ggz_list_get_data(entry);
		if (seat->type == GGZ_SEAT_OPEN)
			return 1;
	}
	
	return 0;
}


static void seat_print(_GGZdMod* mod, GGZSeat *seat)
{
	char *type = NULL;

	switch (seat->type) {
	case GGZ_SEAT_OPEN: type = "OPEN"; break;
	case GGZ_SEAT_BOT: type = "BOT"; break;		
	case GGZ_SEAT_RESV: type = "RESV"; break;		
	case GGZ_SEAT_NONE: type = "NONE"; break;		
	case GGZ_SEAT_PLAYER: type = "PLAYER"; break;
	}
	ggzdmod_log(mod, "GGZDMOD: Seat %d is %s (%s) on %d",
		    seat->num, type, seat->name, seat->fd);
}


/* Create a new copy of a seat object */
static GGZSeat* seat_copy(GGZSeat *orig)
{
	GGZSeat *seat;

	seat = ggz_malloc(sizeof(GGZSeat));

	seat->num = orig->num;
	seat->type = orig->type;
	seat->name = ggz_strdup(orig->name);
	seat->fd = orig->fd;
	
	return seat;
}


static int seat_compare(GGZSeat *a, GGZSeat *b)
{
	return a->num - b->num;
}


static int seat_find_player(GGZSeat *a, GGZSeat *b)
{
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
