/* 
 * File: games/suaro.h
 * Author: Jason Short
 * Project: GGZCards Server
 * Date: 07/02/2001
 * Desc: Game-dependent game functions for Suaro
 * $Id: suaro.h 2561 2001-10-14 06:56:17Z jdorje $
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


#include "../types.h"

/* special bids */
#define SUARO_PASS	1
#define SUARO_DOUBLE	2
#define SUARO_REDOUBLE	3
#define SUARO_KITTY	4

/* special suits */
#define SUARO_LOW 0
	/* clubs-spades = 1-4 = regular values + 1 */
#define SUARO_HIGH 5

#define SUARO ( *(suaro_game_t *)(game.specific) )
typedef struct suaro_game_t {
	/* options */
	int shotgun;		/* are we playing shotgun suaro? */
	int unlimited_redoubling;	/* can doubling continue
					   indefinitely? */
	int persistent_doubles;	/* a double isn't negated by another bid */

	int pass_count;		/* number of passes in a row */

	/* contract information */
	int contract;		/* value of the contract */
	int kitty;		/* 0=>no kitty; 1=>kitty.  Only applicable in 
				   shotgun suaro. */
	int contract_suit;	/* 0=>low, 5=>high, as above */
	int bonus;		/* 1 = regular; 2 = doubled; 4 = redoubled;
				   etc. */
	player_t declarer;	/* player with the contract */

	int kitty_revealed;
} suaro_game_t;

extern char *short_suaro_suit_names[];
