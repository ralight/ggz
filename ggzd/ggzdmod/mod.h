/* 
 * File: mod.h
 * Author: GGZ Dev Team
 * Project: ggzdmod
 * Date: 11/18/01
 * Desc: Functions for reading/writing messages from/to game modules
 * $Id: mod.h 4403 2002-09-04 18:48:34Z dr_maux $
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
#define GGZDMOD_NUM_HANDLERS 9

/* This is the actual structure, but it's only visible internally. */
struct GGZdMod {
	GGZdModType type;	/* ggz-end or game-end */
	GGZdModState state;	/* the state of the game */
	int fd;			/* file descriptor */
	int num_seats;
	int num_open;
	int num_spectators;
	GGZList *seats;
	GGZList *spectators;
	GGZdModHandler handlers[GGZDMOD_NUM_HANDLERS];
	void *gamedata;         /* game-specific data */

	/* ggz-only data */
	pid_t pid;		/* process ID of table */
	char *pwd;		/* working directory for game */
	char **argv;            /* command-line arguments for launching module */

	/* etc. */
};

void _ggzdmod_error(GGZdMod *ggzdmod, char* error);

/* GGZ side functions for handling various messages */
void _ggzdmod_handle_join_response(GGZdMod * ggzdmod, char status);
void _ggzdmod_handle_leave_response(GGZdMod * ggzdmod, char status);
void _ggzdmod_handle_seat_response(GGZdMod * ggzdmod, char status);
void _ggzdmod_handle_spectator_join_response(GGZdMod * ggzdmod, char status);
void _ggzdmod_handle_spectator_leave_response(GGZdMod * ggzdmod, char status);
void _ggzdmod_handle_state(GGZdMod * ggzdmod, GGZdModState state);
void _ggzdmod_handle_log(GGZdMod * ggzdmod, char *msg);

/* Game side functions for handling various messages */
void _ggzdmod_handle_launch_begin(GGZdMod * ggzdmod, int num_seats, int num_spectators);
void _ggzdmod_handle_launch_seat(GGZdMod * ggzdmod, GGZSeat seat);
void _ggzdmod_handle_launch_end(GGZdMod * ggzdmod);
void _ggzdmod_handle_join(GGZdMod * ggzdmod, GGZSeat seat);
void _ggzdmod_handle_leave(GGZdMod * ggzdmod, char *name);
void _ggzdmod_handle_seat(GGZdMod * ggzdmod, GGZSeat seat);
void _ggzdmod_handle_spectator_join(GGZdMod * ggzdmod, GGZSpectator spectator);
void _ggzdmod_handle_spectator_leave(GGZdMod * ggzdmod, char *name);
void _ggzdmod_handle_state_response(GGZdMod * ggzdmod);


#endif /* __GGZ_MOD_H__ */
