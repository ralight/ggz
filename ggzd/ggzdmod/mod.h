/* 
 * File: io.h
 * Author: GGZ Dev Team
 * Project: ggzdmod
 * Date: 11/18/01
 * Desc: Functions for reading/writing messages from/to game modules
 * $Id: mod.h 2767 2001-12-01 06:44:49Z bmh $
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


#ifndef __GGZ_MOD_H__
#define __GGZ_MOD_H__

#include <ggz.h>

#include "ggzdmod.h"

/* The number of event handlers there are. */
#define GGZDMOD_NUM_HANDLERS 6

/* This is the actual structure, but it's only visible internally. */
typedef struct _GGZdMod {
	GGZdModType type;	/* ggz-end or game-end */
	GGZdModState state;	/* the state of the game */
	int fd;			/* file descriptor */
	int num_seats;
	int num_open;
	GGZList *seats;
	GGZdModHandler handlers[GGZDMOD_NUM_HANDLERS];
	void *gamedata;         /* game-specific data */

	/* ggz-only data */
	pid_t pid;		/* process ID of table */
	char **argv;            /* command-line arguments for launching module */

	/* etc. */
} _GGZdMod;

void _ggzdmod_error(_GGZdMod *mod, char* error);

/* GGZ side functions for handling various messages */
void _ggzdmod_handle_join_response(_GGZdMod * mod, char status);
void _ggzdmod_handle_leave_response(_GGZdMod * mod, char status);
void _ggzdmod_handle_state(_GGZdMod * mod, GGZdModState state);
void _ggzdmod_handle_log(_GGZdMod * mod, char *msg);

/* Game side functions for handling various messages */
void _ggzdmod_handle_launch_begin(_GGZdMod * mod, int num_seats);
void _ggzdmod_handle_launch_seat(_GGZdMod * mod, GGZSeat seat);
void _ggzdmod_handle_launch_end(_GGZdMod * mod);
void _ggzdmod_handle_join(_GGZdMod * mod, GGZSeat seat);
void _ggzdmod_handle_leave(_GGZdMod * mod, char *name);
void _ggzdmod_handle_state_response(_GGZdMod * ggzdmod);


#endif
