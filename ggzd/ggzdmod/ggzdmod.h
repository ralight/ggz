/* 
 * File: ggzdmod.h
 * Author: GGZ Dev Team
 * Project: ggzdmod
 * Date: 10/14/01
 * Desc: GGZ game module functions
 * $Id: ggzdmod.h 2574 2001-10-16 17:02:53Z jdorje $
 *
 * This file contains the main interface for the ggzdmod library.  This
 * library facilitates the communication between the GGZ server (ggzd)
 * and game servers.  This file provides a unified interface that can be
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

/* Common objects/functions */

/* Callback events: */
typedef enum {
	GGZ_GAME_STATUS,	/* Module status changed (status) */
	GGZ_GAME_JOIN,		/* Player joined (status) */
	GGZ_GAME_LEAVE,		/* Player left (status) */
	GGZ_GAME_LOG,		/* Module log request (level, msg) */
	GGZ_GAME_PLAYER_DATA,	/* Data avilable from player */
	GGZ_GAME_ERROR		/* Error (code) */
} GGZdModEvent;


typedef enum {
	GGZ_SEAT_OPEN = -1,	   /**< The seat is open (unoccupied). */
	GGZ_SEAT_BOT = -2,	   /**< The seat has a bot (AI) in it. */
	GGZ_SEAT_RESV = -3,	   /**< The seat is reserved for a player. */
	GGZ_SEAT_NONE = -4,	   /**< This seat does not exist. */
	GGZ_SEAT_PLAYER = -5	   /**< The seat has a regular player in it. */
} GGZdModSeat;


/* The "flavor" of GGZdmod object this is.  Affects what operations are
   allowed */
typedef enum {
	GGZDMOD_GGZ,
	GGZDMOD_GAME
} GGZdModType;

/* We'll just pass this around as a void* */
typedef void GGZdMod;

/* GGZdMod event handler prototype */
typedef void (*GGZdModHandler) (GGZdMod *, GGZdModEvent e, void *data);


typedef struct _GGZSeat {

	/* Seat index */
	int num;

	/* Type of seat */
	GGZdModSeat type;

	/* Name of player occupying seat */
	char *name;

	/* fd to communicate with seat occupant */
	int fd;
} GGZSeat;


/* Creating/destroying a ggzdmod object */
GGZdMod *ggzdmod_new(GGZdModType type);
void ggzdmod_free(GGZdMod * mod);

/* Accesor functions for GGZdMod */
int ggzdmod_get_fd(GGZdMod * mod);
GGZdModType ggzdmod_get_type(GGZdMod * mod);
int ggzdmod_get_num_seats(GGZdMod * mod);
GGZSeat *ggzdmod_get_seat(GGZdMod * mod, int seat);

void ggzdmod_set_num_seats(GGZdMod * mod, int num_seats);
void ggzdmod_set_handler(GGZdMod * mod, GGZdModEvent e, GGZdModHandler func);
void ggzdmod_set_seat(GGZdMod * mod, GGZSeat * seat);

/* Event/Data handling */
int ggzdmod_dispatch(GGZdMod * mod);
int ggzdmod_io_pending(GGZdMod * mod);
void ggzdmod_io_read(GGZdMod * mod);
int ggzdmod_loop(GGZdMod * mod);

/* Control functions */
int ggzdmod_halt_game(GGZdMod * mod);

/* ggzd specific actions */
int ggzdmod_launch_game(GGZdMod * mod, char **args);

/* module specific actions */
int ggzdmod_connect(GGZdMod * mod);
int ggzdmod_disconnect(GGZdMod * mod);
int ggzdmod_log(GGZdMod * mod, char *fmt, ...);
