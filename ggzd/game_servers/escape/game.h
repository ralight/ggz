/*
 * File: game.h
 * Author: Rich Gade (Modified for Escape by Roger Light)
 * Project: GGZ Escape game module
 * Date: 28th June 2001
 * Desc: Game functions
 * $Id: game.h 6892 2005-01-25 04:09:21Z jdorje $
 *
 * Copyright (C) 2000 Brent Hendricks.
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

#include "ggzdmod.h"

#define GGZSPECTATORS
#define GGZSTATISTICS

/* GGZdMod callbacks */
void game_handle_ggz_state(GGZdMod *ggz,
			   GGZdModEvent event, const void *data);
void game_handle_ggz_seat(GGZdMod *ggz, GGZdModEvent event, const void *data);
//static void game_handle_ggz_leave(GGZdMod *ggz, GGZdModEvent event, const void *data);
#ifdef GGZSPECTATORS
void game_handle_ggz_spectator_join(GGZdMod *ggz,
				    GGZdModEvent event, const void *data);
void game_handle_ggz_spectator_leave(GGZdMod *ggz,
				     GGZdModEvent event, const void *data);
void game_handle_ggz_spectator(GGZdMod *ggz,
			       GGZdModEvent event, const void *data);
#endif
void game_handle_ggz_player(GGZdMod *ggz,
			    GGZdModEvent event, const void *data);


/* Escape game events */
#define ESCAPE_EVENT_LAUNCH      0
#define ESCAPE_EVENT_JOIN        1
#define ESCAPE_EVENT_LEAVE       2
#define ESCAPE_EVENT_OPTIONS     3
#define ESCAPE_EVENT_MOVE	   4

//typedef enum {
#define	dtEmpty    1
#define	dtPlayer1  2
#define	dtPlayer2  3
#define	dtBlocked  4
#define dtCorner   5
#define dtTieMove  6 
//} EscapeDot;


/* Madximum board sizes */
#define MAX_WALLWIDTH	10
#define MAX_GOALWIDTH	10
#define MAX_BOARDHEIGHT	20

void game_init(GGZdMod *ggz);

