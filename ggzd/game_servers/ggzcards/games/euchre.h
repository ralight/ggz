/*
 * File: games/euchre.h
 * Author: Jason Short
 * Project: GGZCards Server
 * Date: 07/03/2001
 * Desc: Game-dependent game functions for Euchre
 * $Id: euchre.h 2189 2001-08-23 07:59:17Z jdorje $
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

/* special bids */
#define EUCHRE_PASS 1
#define EUCHRE_TAKE 2
#define EUCHRE_TAKE_SUIT 3

#define EUCHRE ( *(euchre_game_t *)(game.specific) )
typedef struct euchre_game_t {
	player_t maker;		/* just like the declarer */
	int dealer_gets_card;	/* does the dealer get the up-card? */
	card_t up_card;		/* the "up-card" */
	int alone;		/* is the dealer going alone? */
	int suit;		/* the suit of trump (TODO: is this necessary?) */
} euchre_game_t;
