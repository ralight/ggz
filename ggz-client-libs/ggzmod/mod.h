/* 
 * File: io.h
 * Author: GGZ Dev Team
 * Project: ggzmod
 * Date: 11/18/01
 * Desc: Functions for reading/writing messages from/to game modules
 * $Id: mod.h 4264 2002-06-22 05:10:28Z bmh $
 *
 * This file contains the backend for the ggzmod library.  This
 * library facilitates the communication between the GGZ server (ggz)
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

#include "ggzmod.h"

/* The number of event handlers there are. */
#define GGZMOD_NUM_HANDLERS 7

/* This is the actual structure, but it's only visible internally. */
struct GGZMod {
	GGZModType type;	/* ggz-end or game-end */
	GGZModState state;	/* the state of the game */
	int fd;			/* file descriptor */
	GGZModHandler handlers[GGZMOD_NUM_HANDLERS];
	void *gamedata;         /* game-specific data */
	int server_fd;

	/* ggz-only data */
	pid_t pid;		/* process ID of table */
	char *pwd;		/* working directory for game */
	char **argv;            /* command-line arguments for launching module */

	/* etc. */
};

void _ggzmod_error(GGZMod *ggzmod, char* error);

/* GGZ side functions for handling various messages */
void _ggzmod_handle_state(GGZMod * ggzmod, GGZModState state);

/* Game side functions for handling various messages */
void _ggzmod_handle_launch(GGZMod * ggzmod);
void _ggzmod_handle_server(GGZMod * ggzmod, int fd);


#endif /* __GGZ_MOD_H__ */
