/*
 * GGZMOD-GGZ - C implementation of the GGZ client-client protocol (GGZ side).
 * This file is part of the package ggz-base-libs.
 *
 * mod-ggz.h: Reading/writing messages from/to game modules, GGZ side.
 *
 * Copyright (C) 2001 - 2008 GGZ Gaming Zone Development Team
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

/*
 * This file contains the backend for the ggzmod library.  This
 * library facilitates the communication between the GGZ server (ggz)
 * and game servers. The file must be kept synchronized with the game's
 * library part (ggzmod.h, mod.h).
 */

#ifndef __GGZ_MOD_GGZ_H__
#define __GGZ_MOD_GGZ_H__

#ifdef HAVE_WINSOCK2_H
# include <winsock2.h>
#endif

#include <ggz.h>

#include "ggzmod-ggz.h"

/* The number of event handlers there are. */
#define GGZMOD_NUM_HANDLERS (GGZMOD_EVENT_ERROR + 1)
/* The number of transaction handlers there are. */
#define GGZMOD_NUM_TRANSACTIONS (GGZMOD_TRANSACTION_RANKINGS + 1)

/* This is the actual structure, but it's only visible internally. */
struct GGZMod {
	GGZModType type;	/* ggz-end or game-end */
	GGZModState state;	/* the state of the game */
	int fd;			/* file descriptor */
	GGZModHandler handlers[GGZMOD_NUM_HANDLERS];
	void *gamedata;         /* game-specific data */

	int server_fd;
	const char *server_host;
	unsigned int server_port;
	const char *server_handle;

	const char *my_name;
	int i_am_spectator;
	int my_seat_num;

	/* Seat and spectator seat data. */
	int num_seats;
	GGZList *seats;
	GGZList *stats;
	GGZList *infos;
	int num_spectator_seats;
	GGZList *spectator_seats;
	GGZList *spectator_stats;

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

/* GGZ+game side error function */
void _ggzmod_ggz_error(GGZMod *ggzmod, char* error);

/* GGZ+game side functions for handling various messages */
void _ggzmod_ggz_handle_state(GGZMod * ggzmod, GGZModState state);

/* GGZ side functions for handling various messages */
void _ggzmod_ggz_handle_stand_request(GGZMod *ggzmod);
void _ggzmod_ggz_handle_sit_request(GGZMod *ggzmod, int seat_num);
void _ggzmod_ggz_handle_boot_request(GGZMod *ggzmod, char *name);
void _ggzmod_ggz_handle_bot_request(GGZMod *ggzmod, int seat_num);
void _ggzmod_ggz_handle_open_request(GGZMod *ggzmod, int seat_num);
void _ggzmod_ggz_handle_chat_request(GGZMod *ggzmod, char *chat_msg);
void _ggzmod_ggz_handle_info_request(GGZMod *ggzmod, int seat_num);
void _ggzmod_ggz_handle_rankings_request(GGZMod *ggzmod);

#endif /* __GGZ_MOD_GGZ_H__ */
