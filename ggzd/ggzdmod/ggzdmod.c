/* 
 * File: ggzdmod.c
 * Author: GGZ Dev Team
 * Project: ggzdmod
 * Date: 10/14/01
 * Desc: GGZ game module functions
 * $Id: ggzdmod.c 2574 2001-10-16 17:02:53Z jdorje $
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

#include <assert.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <easysock.h>
#include "../game_servers/libggzmod/ggz_protocols.h"	/* FIXME */

#include "ggzdmod.h"


/* debug level for ggzdmod_log() */
/* Moved out of ggz_server.h since it's not used outside the library.
   Imported from ggzd; make sure it stays consistent! A better alternative
   would be to #include ../../ggzd/err_func.h --JDS */
#define GGZ_DBG_TABLE	(unsigned) 0x00000010

#define CHECK_GGZDMOD(ggzdmod)                             \
	(assert(ggzdmod && ggzdmod->magic == MAGIC_VALUE), \
	 ggzdmod)

#define GGZDMOD_NUM_HANDLERS 6

/* This is the actual structure, but it's only visible internally. */
typedef struct _GGZdMod {
	GGZdModType type;	/* ggz-end or game-end */
	int fd;			/* file descriptor */
	fd_set active_fd_set;	/* set of active file descriptors */
	int num_seats;
	GGZSeat *seats;
	GGZdModHandler handlers[GGZDMOD_NUM_HANDLERS];
	int gameover;
	/* etc. */
#ifdef DEBUG
#define MAGIC_VALUE 0xdeadbeef
	int magic;		/* for sanity checking */
#endif
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
	_GGZdMod *ggzdmod = malloc(sizeof(*ggzdmod));
	if (!ggzdmod)
		return NULL;

	memset(ggzdmod, sizeof(*ggzdmod), 0);
	ggzdmod->type = type;
	ggzdmod->fd = -1;
	ggzdmod->seats = NULL;
	for (i = 0; i < GGZDMOD_NUM_HANDLERS; i++)
		ggzdmod->handlers[i] = NULL;
	/* TODO: other initialization */
#ifdef DEBUG
	ggzdmod->magic = MAGIC_VALUE;
#endif

	return ggzdmod;
}

/* Frees (deletes) a ggzdmod object */
void ggzdmod_free(GGZdMod * mod)
{
	_GGZdMod *ggzdmod = mod;
	if (!CHECK_GGZDMOD(ggzdmod))
		return;
	free(ggzdmod);
}

/* 
 * Accesor functions for GGZdMod
 */

int ggzdmod_get_fd(GGZdMod * mod)
{
	_GGZdMod *ggzdmod = mod;
	if (!CHECK_GGZDMOD(ggzdmod))
		return -1;
	return ggzdmod->fd;
}


GGZdModType ggzdmod_get_type(GGZdMod * mod)
{
	_GGZdMod *ggzdmod = mod;
	if (!CHECK_GGZDMOD(ggzdmod))
		return 0;	/* not very useful */
	return ggzdmod->type;
}


int ggzdmod_get_num_seats(GGZdMod * mod)
{
	_GGZdMod *ggzdmod = mod;
	if (!CHECK_GGZDMOD(ggzdmod))
		return 0;
	return ggzdmod->num_seats;
}

/* FIXME: this allows direct manipulation of the seat data by the user
   program.  It would be very bad if they did that. */
GGZSeat *ggzdmod_get_seat(GGZdMod * mod, int seat)
{
	_GGZdMod *ggzdmod = mod;
	if (!CHECK_GGZDMOD(ggzdmod) || seat < 0 || seat >= ggzdmod->num_seats) {
		assert(0);
		return NULL;
	}
	return &ggzdmod->seats[seat];
}


void ggzdmod_set_num_seats(GGZdMod * mod, int num_seats)
{
	_GGZdMod *ggzdmod = mod;
	if (!CHECK_GGZDMOD(ggzdmod))
		return;		/* not very useful */
	/* FIXME: we need some more checks here. */
	assert(ggzdmod->num_seats == 0);	/* FIXME */
	ggzdmod->num_seats = num_seats;
	ggzdmod->seats =
		realloc(ggzdmod->seats, num_seats * sizeof(*ggzdmod->seats));
	if (!ggzdmod->seats) {
		/* With no error return value, there's not much we can do. */
		ggzdmod->num_seats = 0;
	}
}

void ggzdmod_set_handler(GGZdMod * mod, GGZdModEvent e, GGZdModHandler func)
{
	_GGZdMod *ggzdmod = mod;
	if (!CHECK_GGZDMOD(ggzdmod) || e < 0 || e >= GGZDMOD_NUM_HANDLERS) {
		assert(0);
		return;		/* not very useful */
	}

	ggzdmod->handlers[e] = func;
}

void ggzdmod_set_seat(GGZdMod * mod, GGZSeat * seat)
{
	_GGZdMod *ggzdmod = mod;
	if (!CHECK_GGZDMOD(ggzdmod) || !seat || seat->num < 0
	    || seat->num >= ggzdmod->num_seats) {
		assert(0);
		return;		/* not very useful */
	}
	memcpy(&ggzdmod->seats[seat->num], seat, sizeof(ggzdmod->seats[0]));
}


/* 
 * Event/Data handling
 */

int ggzdmod_dispatch(GGZdMod * mod)
{
	assert(0);
	return -1;
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

/* FIXME: This function should be exported by the library! */
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
		    && ggzdmod->handlers[GGZ_GAME_PLAYER_DATA] != NULL)
			(*ggzdmod->handlers[GGZ_GAME_PLAYER_DATA]) (ggzdmod,
								    GGZ_GAME_PLAYER_DATA,
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
	if (!CHECK_GGZDMOD(ggzdmod))
		return -1;
	if (ggzdmod->type == GGZDMOD_GAME) {
		ggzdmod->gameover = 1;
	} else {
		assert(0);
		return -1;
	}
	return 0;
}

/* 
 * ggzd specific actions
 */

int ggzdmod_launch_game(GGZdMod * mod, char **args)
{
	assert(0);
	return -1;
}

/* 
 * module specific actions
 */

int ggzdmod_connect(GGZdMod * mod)
{
	_GGZdMod *ggzdmod = mod;
	if (!CHECK_GGZDMOD(ggzdmod) || ggzdmod->type != GGZDMOD_GAME)
		return -1;
	ggzdmod->fd = 3;

	if (ggzdmod_log(ggzdmod, "GGZDMOD: Connecting to GGZ server.") < 0) {
		ggzdmod->fd = -1;
		return -1;
	}

	FD_ZERO(&ggzdmod->active_fd_set);
	FD_SET(ggzdmod->fd, &ggzdmod->active_fd_set);

	return ggzdmod->fd;
}

int ggzdmod_disconnect(GGZdMod * mod)
{
	_GGZdMod *ggzdmod = mod;
	int response, p;
	if (!CHECK_GGZDMOD(ggzdmod) || ggzdmod->type != GGZDMOD_GAME)
		return -1;

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

	if (!CHECK_GGZDMOD(ggzdmod) || !fmt || ggzdmod->fd < 0) {
		/* This will happen when ggzdmod_log is called before
		   connection.  We could store the buffer for later, but... */
		return -1;
	}

	va_start(ap, fmt);
	vsnprintf(buf, sizeof(buf), fmt, ap);
	va_end(ap);

	if (es_write_int(ggzdmod->fd, MSG_DBG) < 0 ||
	    es_write_int(ggzdmod->fd, GGZ_DBG_TABLE) < 0 ||
	    es_write_string(ggzdmod->fd, buf) < 0)
		return -1;
	return 0;
}
