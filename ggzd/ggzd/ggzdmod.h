/*
 * File: ggzdmod.h
 * Author: GGZ Dev Team
 * Project: GGZD
 * Date: 8/28/01
 * Desc: GGZD game module functions
 * $Id: ggzdmod.h 2330 2001-08-31 03:54:57Z jdorje $
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

#ifndef _GGZDMOD_H
#define _GGZDMOD_H

/* This is duplicated with ggz_server.h in libggzdmod */
typedef enum
{
	GGZ_SEAT_OPEN = -1,   /**< The seat is open (unoccupied). */
	GGZ_SEAT_BOT = -2,    /**< The seat has a bot (AI) in it. */
	GGZ_SEAT_RESV = -3,   /**< The seat is reserved for a player. */
	GGZ_SEAT_NONE = -4,   /**< This seat does not exist. */
	GGZ_SEAT_PLAYER = -5  /**< The seat has a regular player in it. */
}
ggzd_assign_t;

/* not sure this should be publicly visible */
typedef struct {
	int fd;
	/* ... */
	void *table_data; /* pointer to other table data */
}
GGZdmod;

GGZdmod *ggzdmod_new(int fd, void *table_data);
void ggzdmod_free(GGZdmod *ggzdmod);

int ggzdmod_launch_game(GGZdmod *ggzdmod, int num_seats, int *types, char **reserves);

/* player join/leave */
int ggzdmod_player_join(GGZdmod *ggzdmod, int seat, char *name, int player_fd);
int ggzdmod_player_leave(GGZdmod *ggzdmod, char *name);

/* should be called anytime there's ggzdmod data waiting to be read on
 * the FD.  The data pointer should point to arbitrary table-specific
 * data (in this case, a GGZTable); it's passed along in the callbacks
 * (below).  --JDS */
int ggzdmod_dispatch(GGZdmod *ggzdmod);

/* pseudo-callback functions */
extern int table_game_launch(GGZdmod *ggzdmod, char status);
extern int table_game_join(GGZdmod *ggzdmod, char status);
extern int table_game_leave(GGZdmod *ggzdmod, char status);
extern int table_game_over(GGZdmod *ggzdmod);
extern int table_log(GGZdmod *ggzdmod, char *msg, int level, char debug);

extern void dbg_msg(const unsigned dbg_type, const char *fmt, ...);

#endif /* _GGZDMOD_H */
