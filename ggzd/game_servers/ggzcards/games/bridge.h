/* 
 * File: games/bridge.h
 * Author: Jason Short
 * Project: GGZCards Server
 * Date: 07/03/2001
 * Desc: Game-dependent game functions for Bridge
 * $Id: bridge.h 3347 2002-02-13 04:17:07Z jdorje $
 *
 * Copyright (C) 2001-2002 Brent Hendricks.
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


/* special bids */
#define BRIDGE_PASS	1
#define BRIDGE_DOUBLE	2
#define BRIDGE_REDOUBLE	3

/* special suits */
	/* clubs-spades = 0-3 = regular values */
#define BRIDGE_NOTRUMP 4

#define BRIDGE ( *(bridge_game_t *)(game.specific) )
typedef struct bridge_game_t {
	int pass_count;		/* number of passes in a row */

	/* we ignore the regular player_t score field altogether */
	int points_above_line[2];
	int game_count;		/* number of games completed; 0-2 */
	int points_below_line[3][2];
	int vulnerable[2];	/* also represents # of games won */

	/* contract information */
	player_t opener[2][5];	/* records which player on each team first
				   opened in each suit */
	int contract;		/* value of the contract */
	int contract_suit;	/* suit of the contract; 0-4 */
	int bonus;		/* 1=regular; 2=doubled; 4=redoubled */
	player_t declarer;	/* player with the contract */
	player_t dummy;		/* dummy player; the declarer's partner */

	int dummy_revealed;
} bridge_game_t;
