/*
 * File: types.h
 * Author: Jason Short
 * Project: GGZCards game module
 * Date: 06/29/2000
 * Desc: Special types for GGZ cards game data
 *
 * Copyright (C) 2001 Brent Hendricks.
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

#ifndef __TYPES_H__
#define __TYPES_H__

#include "cards.h"

/* in different games, bids may have different meanings.
 *   we'll just use this arbitrary data structure for it */
typedef union bid_t {
	/* this assumes a "long" is at least 32-bit. */
	long bid;
	struct special_bid_struct {
		/* this can be used for many different games
		 * that have unusual but similar bidding.
		 * Different games may use it differently. */
		char val;	/* the value of the bid */
		char suit;	/* the suit of the bid (generally trump) */
		char spec;	/* specialty bids (defined per-game) */
	} sbid;
} bid_t;

/* all players have seats, but every seat doesn't necessarily have a player.
 * some seats may be dummies or kitties */
/* the tricky thing is that GGZ knows only about _players_ while the
 * client game knows only about _seats_ */
#define MAX_MESSAGE_LENGTH 100
struct game_seat_t {
	hand_t hand;
	card_t table;
	struct ggz_seat_t * ggz; /* ggz seat data; probably points to something in ggz_seats */
	char message[MAX_MESSAGE_LENGTH];
};
typedef int seat_t; /* just to make things clearer */

struct game_player_t {
	int score;
	int bid_count;	/* the number of times this player has bid this hand */
	int age;	/* how long the player has been at the table; simply
			 * counts upward from 0 (the first player) */
	bid_t bid;
	int tricks;
	int seat;	/* the number of the player's seat */
	int ready;
};
typedef int player_t; /* just to make things clearer */

#endif /* __TYPES_H__ */


