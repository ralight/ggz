/* 
 * File: io.h
 * Author: GGZ Dev Team
 * Project: ggzmod
 * Date: 11/18/01
 * Desc: Functions for reading/writing messages from/to game modules
 * $Id: mod.h 5945 2004-02-16 22:19:52Z jdorje $
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

#ifdef HAVE_WINSOCK_H
# include <winsock.h>
#endif

#include <ggz.h>

#include "ggzmod.h"
#include "ggzmod-ggz.h"

/* The number of event handlers there are. */
#define GGZMOD_NUM_HANDLERS (GGZMOD_EVENT_ERROR + 1)
#define GGZMOD_NUM_TRANSACTIONS (GGZMOD_TRANSACTION_BOT + 1)

/* This is the actual structure, but it's only visible internally. */
struct GGZMod {
	GGZModType type;	/* ggz-end or game-end */
	GGZModState state;	/* the state of the game */
	int fd;			/* file descriptor */
	GGZModHandler handlers[GGZMOD_NUM_HANDLERS];
	void *gamedata;         /* game-specific data */
	int server_fd;

	const char *my_name;
	int i_am_spectator;
	int my_seat_num;

	/* Seat and spectator seat data. */
	int num_seats;
	GGZList *seats;
	int num_spectator_seats;
	GGZList *spectator_seats;

	/* ggz-only data */
#ifdef HAVE_FORK
	pid_t pid;	/* process ID of table */
#else
	HANDLE process;
#endif
	char *pwd;	/* working directory for game */
	char **argv;	/* command-line arguments for launching module */
	GGZModTransactionHandler thandlers[GGZMOD_NUM_TRANSACTIONS];

	/* etc. */
};

void _ggzmod_error(GGZMod *ggzmod, char* error);

/* GGZ side functions for handling various messages */
void _ggzmod_handle_state(GGZMod * ggzmod, GGZModState state);
void _ggzmod_handle_stand_request(GGZMod *ggzmod);
void _ggzmod_handle_sit_request(GGZMod *ggzmod, int seat_num);
void _ggzmod_handle_boot_request(GGZMod *ggzmod, char *name);
void _ggzmod_handle_bot_request(GGZMod *ggzmod, int seat_num);
void _ggzmod_handle_open_request(GGZMod *ggzmod, int seat_num);

/* Game side functions for handling various messages */
void _ggzmod_handle_launch(GGZMod * ggzmod);
void _ggzmod_handle_server(GGZMod * ggzmod, int fd);
void _ggzmod_handle_player(GGZMod *ggzmod,
			   const char *name, int is_spectator, int seat_num);
void _ggzmod_handle_seat(GGZMod *ggzmod, GGZSeat *seat);
void _ggzmod_handle_spectator_seat(GGZMod *ggzmod, GGZSpectatorSeat *seat);


#endif /* __GGZ_MOD_H__ */
