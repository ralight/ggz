/* 
 * File: games/euchre.h
 * Author: Jason Short
 * Project: GGZCards Server
 * Date: 07/03/2001
 * Desc: Game-dependent game functions for Euchre
 * $Id: euchre.h 2704 2001-11-09 01:44:56Z jdorje $
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
enum {
	EUCHRE_PASS,		/* A simple pass */

	/* The "take" bid means you take the bid, and the up-card suit
	   becomes trump.  The exact naming may differ depending on who makes 
	   the bid; the dealer's opponents "order it up", the dealer's
	   partner "assists", and the dealer "takes".  This will most likely
	   be implemented in the bid_text routines rather than by having
	   separate bid types. */
	EUCHRE_TAKE,

	/* The "take suit" bid means you take the bid after the up-card has
	   been turned over.  You now get to choose the trump.  Consequently
	   the bid's "suit" field will include the suit type. */
	EUCHRE_TAKE_SUIT
};

#define EUCHRE ( *(euchre_game_t *)(game.specific) )
typedef struct euchre_game_t {
	player_t maker;		/* just like the declarer */
	int dealer_gets_card;	/* does the dealer get the up-card? */
	card_t up_card;		/* the "up-card" */
	int alone;		/* is the dealer going alone? */
	int suit;		/* the suit of trump (TODO: is this
				   necessary?) */

	/* options */
	int screw_the_dealer;
} euchre_game_t;
