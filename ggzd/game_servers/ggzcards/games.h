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

#ifndef __GAMES_H__
#define __GAMES_H__

/* Game types. */
/* NOTE: because of the way options are handled, these must start at 0 and
 * count up directly.   See games_req_gametype. */
/* They also must match with game_names, below */
typedef enum game_type_t {
	GGZ_GAME_UNKNOWN = -1,
	GGZ_GAME_SUARO,		/* http://suaro.dhs.org */
	GGZ_GAME_SPADES,
	GGZ_GAME_HEARTS,
	GGZ_GAME_BRIDGE,
	GGZ_GAME_LAPOCHA,
	GGZ_GAME_EUCHRE,
	GGZ_GAME_ROOK,
	GGZ_GAME_SKAT
} game_type_t;

/* this MUST correspond to the numbers above */
extern char* game_names[];

#define GGZ_NUM_GAMES		8	/* the _number_ of valid games */

extern int game_types[];

extern int games_get_gametype(char*);	/* which game is this? */
extern int games_req_gametype();	/* what do you want to play today? */
extern int games_valid_game(int);		/* is the game valid? */

#endif /* __GAMES_H__ */
