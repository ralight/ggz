/* 
 * File: ggzdmod.c
 * Author: GGZ Dev Team
 * Project: ggzdmod
 * Date: 10/14/01
 * Desc: GGZ game module functions
 * $Id: ggzdmod.c 2602 2001-10-24 02:25:25Z jdorje $
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
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <easysock.h>
#include "../game_servers/libggzmod/ggz_protocols.h"	/* FIXME */

#include "ggzdmod.h"

/* This checks the ggzdmod object for validity.  It could do more checking if
   desired. */
#define CHECK_GGZDMOD(ggzdmod) (ggzdmod)

#define GGZDMOD_NUM_HANDLERS 6

/* The maximum length of a player name.  Does not include trailing \0. */
#define MAX_USER_NAME_LEN 16

/* This is the actual structure, but it's only visible internally. */
typedef struct _GGZdMod {
	GGZdModType type;	/* ggz-end or game-end */
	GGZdModState state;	/* the state of the game */
	int fd;			/* file descriptor */
	fd_set active_fd_set;	/* set of active file descriptors */
	int num_seats;
	GGZSeat *seats;
	GGZdModHandler handlers[GGZDMOD_NUM_HANDLERS];
	int gameover;
	/* etc. */
} _GGZdMod;


/* 
 * internal functions
 */

/* Returns the highest-numbered FD used by ggzdmod. */
static int ggzdmod_fd_max(_GGZdMod * ggzdmod)
{
	int max = ggzdmod->fd, seat;

	for (seat = 0; seat < ggzdmod->num_seats; seat++)
		if (ggzdmod->seats[seat].fd > max)
			max = ggzdmod->seats[seat].fd;

	return max;
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
	ggzdmod = malloc(sizeof(*ggzdmod));
	if (!ggzdmod)
		return NULL;

	/* initialize */
	memset(ggzdmod, sizeof(*ggzdmod), 0);
	ggzdmod->type = type;
	ggzdmod->state = GGZ_STATE_INIT;
	ggzdmod->fd = -1;
	ggzdmod->seats = NULL;
	for (i = 0; i < GGZDMOD_NUM_HANDLERS; i++)
		ggzdmod->handlers[i] = NULL;
	/* Put any other necessary initialization here.  All fields should be 
	   initialized. */

	return ggzdmod;
}

/* Frees (deletes) a ggzdmod object */
void ggzdmod_free(GGZdMod * mod)
{
	_GGZdMod *ggzdmod = mod;
	if (!CHECK_GGZDMOD(ggzdmod)) {
		return;
	}

	/* Free any fields the object contains */
	ggzdmod->type = -1;
	if (ggzdmod->seats)
		free(ggzdmod->seats);

	/* Free the object */
	free(ggzdmod);
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
		return 0;	/* not very useful */
	}
	return ggzdmod->type;
}


GGZdModState ggzdmod_get_state(GGZdMod * mod)
{
	_GGZdMod *ggzdmod = mod;
	if (!CHECK_GGZDMOD(ggzdmod)) {
		return 0;	/* not very useful */
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
GGZSeat *ggzdmod_get_seat(GGZdMod * mod, int seat)
{
	_GGZdMod *ggzdmod = mod;
	if (!CHECK_GGZDMOD(ggzdmod) || seat < 0 || seat >= ggzdmod->num_seats) {
		return NULL;
	}
	return &ggzdmod->seats[seat];
}


void ggzdmod_set_num_seats(GGZdMod * mod, int num_seats)
{
	_GGZdMod *ggzdmod = mod;
	int seat;

	/* Check parameters */
	if (!CHECK_GGZDMOD(ggzdmod) || num_seats < 0) {
		return;		/* not very useful */
	}

	/* (Re)allocate seats. */
	ggzdmod->seats =
		realloc(ggzdmod->seats, num_seats * sizeof(*ggzdmod->seats));
	if (!ggzdmod->seats) {
		/* With no error return value, there's not much we can do. */
		ggzdmod->num_seats = 0;
		return;
	}

	/* Initialize new seats (if any) */
	for (seat = ggzdmod->num_seats; seat < num_seats; seat++) {
		ggzdmod->seats[seat].num = seat - 1;
		ggzdmod->seats[seat].type = GGZ_SEAT_OPEN;
		ggzdmod->seats[seat].name = NULL;
		ggzdmod->seats[seat].fd = -1;
	}
	ggzdmod->num_seats = num_seats;
}

void ggzdmod_set_handler(GGZdMod * mod, GGZdModEvent e, GGZdModHandler func)
{
	_GGZdMod *ggzdmod = mod;
	if (!CHECK_GGZDMOD(ggzdmod) || e < 0 || e >= GGZDMOD_NUM_HANDLERS) {
		return;		/* not very useful */
	}

	ggzdmod->handlers[e] = func;
}

void ggzdmod_set_seat(GGZdMod * mod, GGZSeat * seat)
{
	_GGZdMod *ggzdmod = mod;
	if (!CHECK_GGZDMOD(ggzdmod) || !seat || seat->num < 0
	    || seat->num >= ggzdmod->num_seats) {
		return;		/* not very useful */
	}
	memcpy(&ggzdmod->seats[seat->num], seat, sizeof(ggzdmod->seats[0]));
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
	if (state == ggzdmod->state)
		return;		/* Is this an error? */

	/* The callback function retrieves the state from ggzdmod_get_state.
	   It could instead be passed as an argument. */
	ggzdmod->state = state;
	if (ggzdmod->handlers[GGZ_EVENT_STATE])
		(*ggzdmod->handlers[GGZ_EVENT_STATE]) (ggzdmod,
						       GGZ_EVENT_STATE, NULL);
}

/* Game-side event: launch event received from ggzd */
static void game_launch(_GGZdMod * ggzdmod)
{
	int i, status = 0, bots = 0;
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

	ggzdmod->seats = calloc(ggzdmod->num_seats, sizeof(*ggzdmod->seats));
	if (ggzdmod->seats == NULL)
		return;

	for (i = 0; i < ggzdmod->num_seats; i++) {
		if (es_read_int(ggzdmod->fd, &ggzdmod->seats[i].type) < 0)
			return;
		ggzdmod->seats[i].fd = -1;
		if (ggzdmod->seats[i].type == GGZ_SEAT_RESV
		    && es_read_string(ggzdmod->fd, ggzdmod->seats[i].name,
				      (MAX_USER_NAME_LEN + 1)))
			return;
	}

	for (i = 0; i < ggzdmod->num_seats; i++)
		switch (ggzdmod->seats[i].type) {
		case GGZ_SEAT_OPEN:
			ggzdmod_log(ggzdmod, "GGZDMOD: Seat %d is open", i);
			break;
		case GGZ_SEAT_BOT:
			/* FIXME: we should truncate the name not the AI */
			snprintf(ggzdmod->seats[i].name,
				 MAX_USER_NAME_LEN + 1, "%s-AI",
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

	set_state(ggzdmod, GGZ_STATE_LAUNCHED);
}

/* game-side event: player join event received from ggzd */
static void game_join(_GGZdMod * ggzdmod)
{
	int seat;

	if (es_read_int(ggzdmod->fd, &seat) < 0 ||
	    es_read_string(ggzdmod->fd, ggzdmod->seats[seat].name,
			   MAX_USER_NAME_LEN + 1) < 0
	    || es_read_fd(ggzdmod->fd, &ggzdmod->seats[seat].fd) < 0
	    || es_write_int(ggzdmod->fd, RSP_GAME_JOIN) < 0)
		return;

	ggzdmod->seats[seat].type = GGZ_SEAT_PLAYER;
	ggzdmod_log(ggzdmod, "%s on %d in seat %d", ggzdmod->seats[seat].name,
		    ggzdmod->seats[seat].fd, seat);

	FD_SET(ggzdmod->seats[seat].fd, &ggzdmod->active_fd_set);

	if (ggzdmod->handlers[GGZ_EVENT_JOIN] != NULL)
		(*ggzdmod->handlers[GGZ_EVENT_JOIN]) (ggzdmod, GGZ_EVENT_JOIN,
						      &seat);
}

/* game-side event: player leave received from ggzd */
static void game_leave(_GGZdMod * ggzdmod)
{
	int seat;
	char status = 0;
	char name[MAX_USER_NAME_LEN + 1];

	if (es_read_string(ggzdmod->fd, name, (MAX_USER_NAME_LEN + 1)) < 0)
		return;

	for (seat = 0; seat < ggzdmod->num_seats; seat++)
		if (!strcmp(name, ggzdmod->seats[seat].name))
			break;

	if (seat == ggzdmod->num_seats)	/* player not found */
		status = -1;
	else {
		FD_CLR(ggzdmod->seats[seat].fd, &ggzdmod->active_fd_set);
		ggzdmod->seats[seat].fd = -1;
		ggzdmod->seats[seat].type = GGZ_SEAT_OPEN;
		status = 0;
		ggzdmod_log(ggzdmod, "Removed %s from seat %d",
			    ggzdmod->seats[seat].name, seat);
	}

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
	ggzdmod_halt_game(ggzdmod);
	set_state(ggzdmod, GGZ_STATE_GAMEOVER);
}

static void ggz_rsp_launch(_GGZdMod * ggzdmod)
{
	char status;
	if (es_read_char(ggzdmod->fd, &status) < 0)
		return;
	set_state(ggzdmod, GGZ_STATE_LAUNCHED);
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
	set_state(ggzdmod, GGZ_STATE_GAMEOVER);	/* Is this right? has the
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
int ggzdmod_dispatch(GGZdMod * mod)
{
	_GGZdMod *ggzdmod = mod;
	int op;

	if (!CHECK_GGZDMOD(ggzdmod)) {
		return -1;
	}

	if (es_read_int(ggzdmod->fd, &op) < 0) {
		ggzdmod_halt_game(ggzdmod);
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
		case REQ_SEAT_CHANGE:
			/* case MSG_STATS: */
		default:
			break;
		}
	}

	return 0;
}

/* Checks the ggzdmod FD and all player FD's for pending data; returns TRUE
   iff there is such data */
int ggzdmod_io_pending(GGZdMod * mod)
{
	fd_set read_fd_set;
	_GGZdMod *ggzdmod = mod;
	int status;
	struct timeval timeout;

	if (!CHECK_GGZDMOD(ggzdmod)) {
		return -1;
	}

	read_fd_set = ggzdmod->active_fd_set;

	/* is this really portable? */
	timeout.tv_sec = timeout.tv_usec = 0;

	status = select(ggzdmod_fd_max(ggzdmod) + 1,
			&read_fd_set, NULL, NULL, &timeout);
	if (status <= 0) {
		/* FIXME: handle error */
	}

	/* the return value of select indicates the # of FD's with pending
	   data */
	return (status > 0);
}

void ggzdmod_io_read(GGZdMod * mod)
{
	fd_set read_fd_set;
	int seat, status;
	_GGZdMod *ggzdmod = mod;

	if (!CHECK_GGZDMOD(ggzdmod)) {
		return;
	}

	read_fd_set = ggzdmod->active_fd_set;

	/* we have to select so that we can determine what file descriptors
	   are waiting to be read. */
	status = select(ggzdmod_fd_max(ggzdmod) + 1,
			&read_fd_set, NULL, NULL, NULL);
	if (status <= 0) {
		if (errno != EINTR) {
			/* FIXME: handle error */
		}
		return;
	}

	status = 0;

	if (FD_ISSET(ggzdmod->fd, &read_fd_set))
		ggzdmod_dispatch(ggzdmod);

	for (seat = 0; seat < ggzdmod->num_seats; seat++) {
		int fd = ggzdmod->seats[seat].fd;
		if (fd != -1 && FD_ISSET(fd, &read_fd_set)
		    && ggzdmod->handlers[GGZ_EVENT_PLAYER_DATA] != NULL)
			(*ggzdmod->handlers[GGZ_EVENT_PLAYER_DATA]) (ggzdmod,
								     GGZ_EVENT_PLAYER_DATA,
								     &seat);
	}
}

/* unlike the old ggzd_main_loop() this one doesn't connect/disconnect. */
int ggzdmod_loop(GGZdMod * mod)
{
	_GGZdMod *ggzdmod = mod;
	if (!CHECK_GGZDMOD(ggzdmod)) {
		return -1;
	}
	while (!ggzdmod->gameover) {
		ggzdmod_io_read(mod);
	}
	return 0;		/* should handle errors */
}

int ggzdmod_halt_game(GGZdMod * mod)
{
	_GGZdMod *ggzdmod = mod;
	if (!CHECK_GGZDMOD(ggzdmod)) {
		return -1;
	}
	if (ggzdmod->type == GGZDMOD_GAME) {
		ggzdmod->gameover = 1;
	} else {
		/* TODO: not implemented */
		return -1;
	}
	return 0;
}

/* 
 * ggzd specific actions
 */

int ggzdmod_launch_game(GGZdMod * mod, char **args)
{
	/* TODO: not implemented */
	return -1;
}

/* 
 * module specific actions
 */

int ggzdmod_connect(GGZdMod * mod)
{
	_GGZdMod *ggzdmod = mod;
	if (!CHECK_GGZDMOD(ggzdmod) || ggzdmod->type != GGZDMOD_GAME) {
		return -1;
	}
	ggzdmod->fd = 3;

	if (ggzdmod_log(ggzdmod, "GGZDMOD: Connecting to GGZ server.") < 0) {
		ggzdmod->fd = -1;
		return -1;
	}

	/* We maintain active_fd_set as the set of active file descriptors. */
	FD_ZERO(&ggzdmod->active_fd_set);
	FD_SET(ggzdmod->fd, &ggzdmod->active_fd_set);

	return ggzdmod->fd;
}

int ggzdmod_disconnect(GGZdMod * mod)
{
	_GGZdMod *ggzdmod = mod;
	int response, p;
	if (!CHECK_GGZDMOD(ggzdmod) || ggzdmod->type != GGZDMOD_GAME) {
		return -1;
	}

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

	/* close all file descriptors */
	for (p = 0; p < ggzdmod->num_seats; p++)
		if (ggzdmod->seats[p].fd != -1) {
			close(ggzdmod->seats[p].fd);
			ggzdmod->seats[p].fd = -1;
		}

	/* Clean up the ggzdmod object.  In theory it could now reconnect for 
	   a new game. */
	close(ggzdmod->fd);
	ggzdmod->fd = -1;
	FD_ZERO(&ggzdmod->active_fd_set);
	free(ggzdmod->seats);
	ggzdmod->seats = NULL;

	return 0;
}

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
