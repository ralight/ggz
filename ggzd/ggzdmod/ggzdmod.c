/* 
 * File: ggzdmod.c
 * Author: GGZ Dev Team
 * Project: ggzdmod
 * Date: 10/14/01
 * Desc: GGZ game module functions
 * $Id: ggzdmod.c 2564 2001-10-14 10:01:42Z jdorje $
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
#include <stdlib.h>
#include <string.h>

#include "ggzdmod.h"

#define CHECK_GGZDMOD(ggzdmod)                             \
	(assert(ggzdmod && ggzdmod->magic == MAGIC_VALUE), \
	 ggzdmod)

#define GGZDMOD_NUM_HANDLERS 6

/* This is the actual structure, but it's only visible internally. */
typedef struct _GGZdMod {
	GGZdModType type;	/* ggz-end or game-end */
	int fd;			/* file descriptor */
	int num_seats;
	GGZSeat *seats;
	GGZdModHandler handlers[GGZDMOD_NUM_HANDLERS];
	/* etc. */
#ifdef DEBUG
#define MAGIC_VALUE 0xdeadbeef
	int magic;		/* for sanity checking */
#endif
} _GGZdMod;



/* 
 * Creating/destroying a ggzdmod object
 */

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

int ggzdmod_io_pending(GGZdMod * mod)
{
	assert(0);
	return -1;
}

int ggzdmod_loop(GGZdMod * mod)
{
	assert(0);
	return -1;
}

/* 
 * ggzd specific actions
 */

int ggzdmod_launch_game(GGZdMod * mod, char **args)
{
	assert(0);
	return -1;
}

int ggzdmod_halt_game(GGZdMod * mod)
{
	assert(0);
	return -1;
}

/* 
 * module specific actions
 */

int ggzdmod_connect(GGZdMod * mod)
{
	assert(0);
	return -1;
}

int ggzdmod_disconnect(GGZdMod * mod)
{
	assert(0);
	return -1;
}

int ggzdmod_log(GGZdMod * mod, char *fmt, ...)
{
	assert(0);
	return -1;
}
