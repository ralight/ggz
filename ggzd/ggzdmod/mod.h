/* 
 * GGZDMOD - C implementation of the GGZ server-server protocol
 *
 * Copyright (C) 2001 - 2008 GGZ Development Team.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

/* This file contains callbacks for messages from both the GGZ server and the
 * game server. Usually, some preprocessing and restructuring of the data is
 * done to transform the low-level network representation into higher-level
 * data structures.
 */

#ifndef GGZDMOD_MOD_H
#define GGZDMOD_MOD_H

#include <ggz.h>

#include "ggzdmod.h"
#include "ggzdmod-ggz.h"

/* The number of event handlers there are. */
/* FIXME: hard-coding this is a sure way to get bugs! */
#define GGZDMOD_NUM_EVENTS (GGZDMOD_EVENT_ERROR_INTERNAL + 1)

/* This is the actual structure, but it's only visible internally. */
struct GGZdMod {
	GGZdModType type;	/* ggz-end or game-end */
	GGZdModState state;	/* the state of the game */
	int fd;			/* file descriptor */
	int num_seats;
	int num_open;
	int max_num_spectators;
	GGZList *seats;
	GGZList *spectators;
	GGZdModHandler handlers[GGZDMOD_NUM_EVENTS];
	void *gamedata;         /* game-specific data */

	/* Name of savegame this is restored from (or NULL) */
	const char *savegame;

	/* ggz-only data */
	pid_t pid;		/* process ID of table */
	char *pwd;		/* working directory for game */
	char **argv;	/* command-line arguments for launching module */
	char *game;		/* game name (e.g. tictactoe), from the .dsc file */

	/* etc. */
};

void _ggzdmod_error(GGZdMod *ggzdmod, char* error);

/* GGZ side functions for handling various messages */
void _ggzdmod_handle_state(GGZdMod * ggzdmod, GGZdModState state);
void _ggzdmod_handle_log(GGZdMod * ggzdmod, char *msg);
void _ggzdmod_handle_report(GGZdMod * ggzdmod,
			    int num_players, char **names, GGZSeatType *types,
			    int *teams, GGZGameResult *results, int *scores);
void _ggzdmod_handle_savegame(GGZdMod * ggzdmod, char *savegame);
void _ggzdmod_handle_num_seats_request(GGZdMod *ggzdmod, int num_seats);
void _ggzdmod_handle_boot_request(GGZdMod *ggzdmod, char *name);
void _ggzdmod_handle_bot_request(GGZdMod *ggzdmod, int seat_num);
void _ggzdmod_handle_open_request(GGZdMod *ggzdmod, int seat_num);

/* Game side functions for handling various messages */
void _ggzdmod_handle_launch_begin(GGZdMod * ggzdmod,
				  const char *game, const char *savedgame,
				  int num_seats, int num_spectators);
void _ggzdmod_handle_launch_seat(GGZdMod * ggzdmod, GGZSeat seat);
void _ggzdmod_handle_launch_end(GGZdMod * ggzdmod);
void _ggzdmod_handle_seat(GGZdMod * ggzdmod, GGZSeat *seat);
void _ggzdmod_handle_reseat(GGZdMod * ggzdmod,
			    int old_seat, int was_spectator,
			    int new_seat, int is_spectator);
void _ggzdmod_handle_spectator_seat(GGZdMod * ggzdmod, GGZSeat *seat);
void _ggzdmod_handle_state_response(GGZdMod * ggzdmod);

#endif /* GGZDMOD_MOD_H */
