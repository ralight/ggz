/* 
 * File: ggzdmod.c
 * Author: GGZ Dev Team
 * Project: ggzdmod
 * Date: 10/14/01
 * Desc: GGZ game module functions
 * $Id: ggzdmod.c 2709 2001-11-09 02:57:46Z jdorje $
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
#include "protocol.h"


/* This checks the ggzdmod object for validity.  It could do more checking if
   desired. */
#define CHECK_GGZDMOD(ggzdmod) (ggzdmod)

/* The number of event handlers there are. */
#define GGZDMOD_NUM_HANDLERS 6

/* This is the actual structure, but it's only visible internally. */
typedef struct _GGZdMod {
	GGZdModType type;	/* ggz-end or game-end */
	GGZdModState state;	/* the state of the game */
	int fd;			/* file descriptor */
	int num_seats;
	GGZSeat *seats;
	GGZdModHandler handlers[GGZDMOD_NUM_HANDLERS];
	void *gamedata;         /* game-specific data */

	/* ggz-only data */
	pid_t pid;		/* process ID of table */
	char **argv;            /* command-line arguments for launching module */

	/* etc. */
} _GGZdMod;


/* 
 * internal functions
 */

/* Returns the highest-numbered FD used by ggzdmod. */
static int get_fd_max(_GGZdMod * ggzdmod)
{
	int max = ggzdmod->fd, seat;

	/* If we don't have a player data handler set
	   up, we won't monitor the player data handlers. */
	if (ggzdmod->handlers[GGZ_EVENT_PLAYER_DATA])
		for (seat = 0; seat < ggzdmod->num_seats; seat++)
			if (ggzdmod->seats[seat].fd > max)
				max = ggzdmod->seats[seat].fd;

	return max;
}

static fd_set get_active_fd_set(_GGZdMod *ggzdmod)
{
	fd_set active_fd_set;
	
	FD_ZERO(&active_fd_set);
	if (ggzdmod->fd != -1)
		FD_SET(ggzdmod->fd, &active_fd_set);
	if (ggzdmod->handlers[GGZ_EVENT_PLAYER_DATA]) {
		int i;
		for (i=0; i<ggzdmod->num_seats; i++)
			if (ggzdmod->seats[i].fd != -1)
				FD_SET(ggzdmod->seats[i].fd, &active_fd_set);
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
	ggzdmod->state = GGZ_STATE_CREATED;
	ggzdmod->fd = -1;
	ggzdmod->seats = NULL;
	for (i = 0; i < GGZDMOD_NUM_HANDLERS; i++)
		ggzdmod->handlers[i] = NULL;
	ggzdmod->gamedata = NULL;

	ggzdmod->pid = -1;
	ggzdmod->argv = NULL;
	/* Put any other necessary initialization here.  All fields should be
	   initialized.  Note NULL may not necessarily be 0 on all platforms. */

	return ggzdmod;
}

/* I'm not sure when the seats should be freed.  Should it be done
   in ggzdmod_disconnect, or just in ggzdmod_free?  --JDS */
static void ggzdmod_free_seats(_GGZdMod * ggzdmod)
{
	int p;
	/* FIXME: will this work for ggzd?  It uses a different thread to
	   monitor each player FD, so it might be Bad to close them without
	   checking with the other threads.  --JDS */

	if (!ggzdmod->seats)
		return;
	
	/* close all file descriptors */
	for (p = 0; p < ggzdmod->num_seats; p++) {
		if (ggzdmod->seats[p].fd != -1) {
			close(ggzdmod->seats[p].fd);
			ggzdmod->seats[p].fd = -1;
		}
		if (ggzdmod->seats[p].name) {
			ggz_free(ggzdmod->seats[p].name);
			ggzdmod->seats[p].name = NULL;
		}
	}
	ggz_free(ggzdmod->seats);
	ggzdmod->seats = NULL;
	ggzdmod->num_seats = 0;
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
	
	ggzdmod_free_seats(ggzdmod);

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

/* FIXME: this allows direct manipulation of the seat data by the user
   program.  It would be very bad if they did that. */
GGZSeat ggzdmod_get_seat(GGZdMod * mod, int seat)
{
	_GGZdMod *ggzdmod = mod;
	if (!CHECK_GGZDMOD(ggzdmod) || seat < 0 || seat >= ggzdmod->num_seats) {
		GGZSeat dummy;
		dummy.type = GGZ_SEAT_NONE, dummy.fd = -1, dummy.num = seat, dummy.name = NULL;
		return dummy;
	}
	return ggzdmod->seats[seat];
}


void* ggzdmod_get_gamedata(GGZdMod * mod)
{
	_GGZdMod *ggzdmod = mod;
	if (!CHECK_GGZDMOD(ggzdmod)) {
		return NULL;
	}
	return ggzdmod->gamedata;
}


void ggzdmod_set_num_seats(GGZdMod * mod, int num_seats)
{
	_GGZdMod *ggzdmod = mod;
	int seat;

	/* Check parameters */
	if (!CHECK_GGZDMOD(ggzdmod) || num_seats < 0
	    || ggzdmod->type != GGZDMOD_GGZ) {
		return;		/* not very useful */
	}

	/* (Re)allocate seats. */
	ggzdmod->seats =
		ggz_realloc(ggzdmod->seats,
			    num_seats * sizeof(*ggzdmod->seats));

	/* Initialize new seats (if any) */
	for (seat = ggzdmod->num_seats; seat < num_seats; seat++) {
		ggzdmod->seats[seat].num = seat;
		ggzdmod->seats[seat].type = GGZ_SEAT_OPEN;
		ggzdmod->seats[seat].name = NULL;
		ggzdmod->seats[seat].fd = -1;
	}
	ggzdmod->num_seats = num_seats;
}


void ggzdmod_set_module(GGZdMod * mod, char **argv)
{
	_GGZdMod *ggzdmod = mod;
	int i;

	/* Check parameters */
	if (!CHECK_GGZDMOD(ggzdmod) || ggzdmod->type != GGZDMOD_GGZ || !argv || !argv[1]) {
		return;		/* not very useful */
	}

	/* Count the number of args so we know how much to allocate */
	for (i = 0; argv[i]; i++) {}
	
	ggzdmod->argv = ggz_malloc(sizeof(char*)*(i+1));
	
	for (i = 0; argv[i]; i++) 
		ggzdmod->argv[i] = argv[i];
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

int ggzdmod_set_seat(GGZdMod * mod, GGZSeat * seat)
{
	_GGZdMod *ggzdmod = mod;
	GGZSeat *oldseat;
	if (!CHECK_GGZDMOD(ggzdmod) || !seat || seat->num < 0
	    || seat->num >= ggzdmod->num_seats) {
		return -2;		
	}
	
	oldseat = &ggzdmod->seats[seat->num];
	
	if (oldseat->num != seat->num)
		ggzdmod_log(ggzdmod, "GGZDMOD: ERROR: Seat number doesn't match.");
	
	if (ggzdmod->type != GGZDMOD_GGZ) {
		if (seat->fd != oldseat->fd || seat->type != oldseat->type)
			return -1;
		/* For now, we allow games to change the names of Bot players,
		   but that's it.  This information is _not_ transmitted back
		   to ggzd (yet). */
		if (oldseat->type != GGZ_SEAT_BOT && strings_differ(seat->name, oldseat->name))
			return -1;
	}
	
	oldseat->fd = seat->fd;
	oldseat->type = seat->type;
	
	/* We don't actually care if the strings are equal, but if they actually
	   point to the same block of data we don't want to free/strdup. */
	if (oldseat->name != seat->name) {
		if (oldseat->name)
			ggz_free(oldseat->name);
		oldseat->name = seat->name;
		if (oldseat->name)
			oldseat->name = ggz_strdup(oldseat->name);
	}
	
	return 0;
}


/* 
 * Event/Data handling
 */

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

static void set_state(_GGZdMod * ggzdmod, GGZdModState state)
{
	GGZdModState old_state = ggzdmod->state;
	if (state == ggzdmod->state)
		return;		/* Is this an error? */

	/* The callback function retrieves the state from ggzdmod_get_state.
	   It could instead be passed as an argument. */
	ggzdmod->state = state;
	if (ggzdmod->handlers[GGZ_EVENT_STATE])
		(*ggzdmod->handlers[GGZ_EVENT_STATE]) (ggzdmod,
						       GGZ_EVENT_STATE, &old_state);
}

/* Game-side event: launch event received from ggzd */
static void game_launch(_GGZdMod * ggzdmod)
{
	int i, status = 0, bots = 0, len;
#define NUM_BOT_NAMES (sizeof(bot_names)/sizeof(bot_names[0]))
	char *rand_bot_names[NUM_BOT_NAMES];

	randomize_names(bot_names, rand_bot_names, NUM_BOT_NAMES);

	if (es_read_int(ggzdmod->fd, &ggzdmod->num_seats) < 0)
		return;
	if (ggzdmod->num_seats <= 0) {
		ggzdmod_log(ggzdmod,
			    "GGZDMOD: ERROR: %d seats sent upon launch.",
			    ggzdmod->num_seats);
		return;
	}

	ggzdmod->seats = ggz_malloc(ggzdmod->num_seats * sizeof(*ggzdmod->seats));
	if (ggzdmod->seats == NULL)
		return;

	for (i = 0; i < ggzdmod->num_seats; i++) {
		char* name;
		if (es_read_int(ggzdmod->fd, &ggzdmod->seats[i].type) < 0)
			return;
		ggzdmod->seats[i].fd = -1;
		if (ggzdmod->seats[i].type == GGZ_SEAT_RESV) {
			if (es_read_string_alloc(ggzdmod->fd, &name) < 0)
				return;
			/* This is a hack so that we use libggz memory management for everything. */
			ggzdmod->seats[i].name = ggz_strdup(name);
			free(name);
		}
	}

	for (i = 0; i < ggzdmod->num_seats; i++)
		switch (ggzdmod->seats[i].type) {
		case GGZ_SEAT_OPEN:
			ggzdmod_log(ggzdmod, "GGZDMOD: Seat %d is open", i);
			break;
		case GGZ_SEAT_BOT:
			if (ggzdmod->seats[i].name) {
				ggzdmod_log(ggzdmod,
					    "GGZDMOD: ERROR: non-null name on launch.");
				free(ggzdmod->seats[i].name);
			}
			len = strlen(rand_bot_names[bots]) + 4;
			ggzdmod->seats[i].name = ggz_malloc(len);
			snprintf(ggzdmod->seats[i].name, len, "%s-AI",
				 rand_bot_names[bots]);
			ggzdmod_log(ggzdmod,
				    "GGZDMOD: Seat %d is a bot named %s", i,
				    rand_bot_names[bots]);
			bots++;
			break;
		case GGZ_SEAT_RESV:
			ggzdmod_log(ggzdmod,
				    "GGZDMOD: Seat %d reserved for %s", i,
				    ggzdmod->seats[i].name);
			break;
		default:	/* prevent compiler warning */
			/* We ignore other values (?) --JDS */
		}

	if (es_write_int(ggzdmod->fd, RSP_GAME_LAUNCH) < 0
	    || es_write_char(ggzdmod->fd, status) < 0)
		return;

	set_state(ggzdmod, GGZ_STATE_WAITING);
}

/* game-side event: player join event received from ggzd */
static void game_join(_GGZdMod * ggzdmod)
{
	int seat;
	char *name;

	if (es_read_int(ggzdmod->fd, &seat) < 0 ||
	    es_read_string_alloc(ggzdmod->fd, &name) < 0
	    || es_read_fd(ggzdmod->fd, &ggzdmod->seats[seat].fd) < 0
	    || es_write_int(ggzdmod->fd, RSP_GAME_JOIN) < 0
	    || es_write_char(ggzdmod->fd, 0))
		return;
	
	/* This is a hack so that we use libggz memory management for everything. */
	ggzdmod->seats[seat].name = ggz_strdup(name);
	free(name);

	ggzdmod->seats[seat].type = GGZ_SEAT_PLAYER;
	ggzdmod_log(ggzdmod, "%s on %d in seat %d", ggzdmod->seats[seat].name,
		    ggzdmod->seats[seat].fd, seat);

	if (ggzdmod->handlers[GGZ_EVENT_JOIN] != NULL)
		(*ggzdmod->handlers[GGZ_EVENT_JOIN]) (ggzdmod, GGZ_EVENT_JOIN,
						      &seat);
}

/* game-side event: player leave received from ggzd */
static void game_leave(_GGZdMod * ggzdmod)
{
	int seat;
	char status = 0;
	char *name;

	if (es_read_string_alloc(ggzdmod->fd, &name) < 0)
		return;

	for (seat = 0; seat < ggzdmod->num_seats; seat++)
		if (ggzdmod->seats[seat].name &&
		    !strcmp(name, ggzdmod->seats[seat].name))
			break;

	if (seat == ggzdmod->num_seats)	/* player not found */
		status = -1;
	else {
		ggzdmod->seats[seat].fd = -1;
		ggzdmod->seats[seat].name = NULL;
		ggzdmod->seats[seat].type = GGZ_SEAT_OPEN;
		ggzdmod_log(ggzdmod, "Removed %s from seat %d",
			    ggzdmod->seats[seat].name, seat);
	}
	
	free(ggzdmod->seats[seat].name);

	if (es_write_int(ggzdmod->fd, RSP_GAME_LEAVE) < 0 ||
	    es_write_char(ggzdmod->fd, status) < 0 || status != 0)
		return;

	if (ggzdmod->handlers[GGZ_EVENT_LEAVE] != NULL)
		(*ggzdmod->handlers[GGZ_EVENT_LEAVE]) (ggzdmod,
						       GGZ_EVENT_LEAVE,
						       &seat);
}

/* game-side event: game over received from ggzd */
static void game_over(_GGZdMod * ggzdmod)
{
	ggzdmod_halt_table(ggzdmod);
	set_state(ggzdmod, GGZ_STATE_DONE);
}

static void ggz_rsp_launch(_GGZdMod * ggzdmod)
{
	char status;
	if (es_read_char(ggzdmod->fd, &status) < 0)
		return;
	set_state(ggzdmod, GGZ_STATE_WAITING);
}

static void ggz_rsp_join(_GGZdMod * ggzdmod)
{
	char status;
	if (es_read_char(ggzdmod->fd, &status) < 0)
		return;
	if (ggzdmod->handlers[GGZ_EVENT_JOIN])
		(*ggzdmod->handlers[GGZ_EVENT_JOIN]) (ggzdmod, GGZ_EVENT_JOIN,
						      &status);
}

static void ggz_rsp_leave(_GGZdMod * ggzdmod)
{
	char status;
	if (es_read_char(ggzdmod->fd, &status) < 0)
		return;
	if (ggzdmod->handlers[GGZ_EVENT_LEAVE])
		(*ggzdmod->handlers[GGZ_EVENT_LEAVE]) (ggzdmod,
						       GGZ_EVENT_LEAVE,
						       &status);
}

static void ggz_req_gameover(_GGZdMod * ggzdmod)
{
	es_write_int(ggzdmod->fd, RSP_GAME_OVER);	/* ignore error */
	set_state(ggzdmod, GGZ_STATE_DONE);	/* Is this right? has the
						   gameover happened yet? */
}

static void ggz_log(_GGZdMod * ggzdmod)
{
	char *msg;

	/* We used to have different levels of logging, but I've gotten rid
	   of all that. */
	if (es_read_string_alloc(ggzdmod->fd, &msg) < 0)
		return;

	if (ggzdmod->handlers[GGZ_EVENT_LOG])
		(*ggzdmod->handlers[GGZ_EVENT_LOG]) (ggzdmod, GGZ_EVENT_LOG,
						     msg);

	free(msg);
}

/* Returns -1 on error (?). */
static int handle_ggzdmod_data(GGZdMod * mod)
{
	_GGZdMod *ggzdmod = mod;
	int op;

	if (!CHECK_GGZDMOD(ggzdmod)) {
		return -1;
	}

	if (es_read_int(ggzdmod->fd, &op) < 0) {
		ggzdmod_halt_table(ggzdmod);
		return -1;
	}

	if (ggzdmod->type == GGZDMOD_GAME) {
		switch (op) {
		case REQ_GAME_LAUNCH:
			game_launch(ggzdmod);
			break;
		case REQ_GAME_JOIN:
			game_join(ggzdmod);
			break;
		case REQ_GAME_LEAVE:
			game_leave(ggzdmod);
			break;
		case RSP_GAME_OVER:
			game_over(ggzdmod);
			break;
		}
	} else {
		switch (op) {
		case RSP_GAME_LAUNCH:
			ggz_rsp_launch(ggzdmod);
			break;
		case RSP_GAME_JOIN:
			ggz_rsp_join(ggzdmod);
			break;
		case RSP_GAME_LEAVE:
			ggz_rsp_leave(ggzdmod);
			break;
		case REQ_GAME_OVER:
			ggz_req_gameover(ggzdmod);
		case MSG_LOG:
			ggz_log(ggzdmod);
		default:
			break;
		}
	}

	return 0;
}

static int handle_event(_GGZdMod * ggzdmod, fd_set read_fd_set)
{
	int seat, count = 0;

	if (FD_ISSET(ggzdmod->fd, &read_fd_set)) {
		handle_ggzdmod_data(ggzdmod);
		count++;
	}

	for (seat = 0; seat < ggzdmod->num_seats; seat++) {
		int fd = ggzdmod->seats[seat].fd;
		if (fd != -1 && FD_ISSET(fd, &read_fd_set)) {
			if (ggzdmod->handlers[GGZ_EVENT_PLAYER_DATA] != NULL) {
				(*ggzdmod->
				 handlers[GGZ_EVENT_PLAYER_DATA]) (ggzdmod,
								   GGZ_EVENT_PLAYER_DATA,
								   &seat);
			}
			count++;
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
	while (ggzdmod->state != GGZ_STATE_DONE) {
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
		/* FIXME: do we really want to call the event handler
		   function in this case? */
		set_state(ggzdmod, GGZ_STATE_DONE);
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
static int send_game_launch(_GGZdMod * ggzdmod)
{
	int seat;
	/* Send packet header and # of seats. */
	if (es_write_int(ggzdmod->fd, REQ_GAME_LAUNCH) < 0 ||
	    es_write_int(ggzdmod->fd, ggzdmod->num_seats) < 0)
		return -1;

	/* Send seat assignments */
	for (seat = 0; seat < ggzdmod->num_seats; seat++) {
		GGZdModSeat type = ggzdmod->seats[seat].type;
		char *name = ggzdmod->seats[seat].name;
		if (es_write_int(ggzdmod->fd, type) < 0)
			return -1;
		if (type == GGZ_SEAT_RESV) {
			if (!name || es_write_string(ggzdmod->fd, name) < 0)
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
	if (mod->argv == NULL || mod->argv[0] == NULL)
		return -1;

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
		ggz_error_sys_exit("exec failed");	/* we still can't send error messages */
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
		if (game_fork(ggzdmod) < 0 || send_game_launch(ggzdmod) < 0) {
			/* FIXME: this might result in a forked but unused table. */
			return -1;
		} else
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
	int response;
	if (!CHECK_GGZDMOD(ggzdmod) || ggzdmod->fd < 0 ) {
		return -1;
	}

	if (ggzdmod->type == GGZDMOD_GGZ) {
		/* For the ggz side, we kill the game server and close the socket */
		
		/* Make sure game server is dead */
		if (ggzdmod->pid > 0)
			kill(ggzdmod->pid, SIGINT);
		ggzdmod->pid = -1;
		
		/* FIXME: should we wait() to get an exit status?? */
		/* FIXME: what other cleanups should we do? */
	} else {
		/* For client the game side we send a game over message */
		
		/* first send a gameover message (request) */
		if (es_write_int(ggzdmod->fd, REQ_GAME_OVER) < 0)
			return -1;
		
		/* The server will send a message in response; we should wait for it. 
		 */
		if (es_read_int(ggzdmod->fd, &response) < 0)
			return -1;
		if (response != RSP_GAME_OVER) {
			/* what else can we do? */
			ggzdmod_log(ggzdmod,
				    "GGZDMOD: ERROR: "
				    "Got %d response while waiting for RSP_GAME_OVER.",
				    response);
		}
		
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

	/* We used to have different levels of logging and allow debugging
	   and logging messages, but I've gotten rid of all that. */
	if (es_write_int(ggzdmod->fd, MSG_LOG) < 0 ||
	    es_write_string(ggzdmod->fd, buf) < 0)
		return -1;
	return 0;
}
