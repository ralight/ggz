/*
 * File: games.h
 * Author: Jason Short
 * Project: GGZCards game module
 * Date: 06/20/2001
 * Desc: Data and functions related to all games
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



/* Game types.  Shouldn't really go here, but... */
/* NOTE: because of the way options are handled, these must start at 0 and
 * count up directly.   See game_req_gametype. */
/* They also must match with game_names, below. */
#define GGZ_GAME_SUARO		0 /* http://suaro.dhs.org */
	/* these others aren't supported yet... */
#define GGZ_GAME_SPADES		1
#define GGZ_GAME_HEARTS		2
#define GGZ_GAME_BRIDGE		3
#define GGZ_GAME_LAPOCHA	4
#define GGZ_GAME_EUCHRE		5
#define GGZ_GAME_ROOK		6
/* ... etc. ... */
#define GGZ_GAME_UNKNOWN	-1

/* this MUST correspond to the numbers above */
extern char* game_names[];

#define GGZ_DEFAULT_GAME	0       /* the default game */
#define GGZ_NUM_GAMES		7	/* the _number_ of valid games */

extern int game_types[];
