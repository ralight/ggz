/* 
 * File: games/lapocha.h
 * Author: Rich Gade, Jason Short
 * Project: GGZCards Server
 * Date: 07/03/2001
 * Desc: Game-dependent game functions for La Pocha
 * $Id: lapocha.h 3347 2002-02-13 04:17:07Z jdorje $
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

#define LAPOCHA_TRUMP	1

/* la pocha specific data */
#define LAPOCHA ( *(lapocha_game_t *)(game.specific) )
typedef struct lapocha_game_t {
	int bid_sum;		/* what the sum of the bids is */
} lapocha_game_t;

/* Each of these opcodes is used for a GAME_MESSAGE_GAME.  The opcode is sent 
   as a 1-byte char. */
enum {
	/* Requests a trump bid.  No extra data. */
	LAP_REQ_TRUMP,

	/* Requests a normal bid.  No extra data. */
	LAP_REQ_BID,

	/* Tells who the dealer is.  Followed by an int for the dealer's
	   seat. */
	LAP_MSG_DEALER,

	/* Tells what trump is.  Followed by a char for the trump suit. */
	LAP_MSG_TRUMP,

	/* Tells of a player's bid.  Followed by an int for the player and an 
	   int for the bid. */
	LAP_MSG_BID,

	/* Give the scores of the game.  Followed by one int per player for
	   that player's score. */
	LAP_MSG_SCORES,
};
