/* 
 * File: types.h
 * Author: Jason Short
 * Project: GGZCards Server
 * Date: 06/29/2000
 * Desc: Special types for GGZCards game data
 * $Id: types.h 2968 2001-12-20 08:25:05Z jdorje $
 *
 * These are a few random type definitions used all over the place.
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

#include <config.h>		/* Site-specific config */

#include "cards.h"

/* in different games, bids may have different meanings.  we'll just use this 
   arbitrary data structure for it */
typedef union bid_t {
	/* this assumes a "long" is at least 32-bit. */
	long bid;
	struct special_bid_struct {
		/* this can be used for many different games that have
		   unusual but similar bidding. Different games may use it
		   differently. */
		char val;	/* the value of the bid */
		char suit;	/* the suit of the bid (generally trump) */
		char spec;	/* specialty bids (defined per-game) */
	} sbid;
} bid_t;


/* This struct holds all data about a player's bidding status. */
typedef struct {
	int is_bidding;		/* is this player currently bidding? Are we
				   waiting for a bid from them? */

	/* FIXME: should this use libggz's list structure? */
	bid_t *bids;		/* An array holding the bid choices the
				   player will be/has been given */
	int bid_size;		/* The size of the array */
	int bid_count;		/* Number of bids in the array (some of the
				   array may be unused) */
} bid_data_t;

/* these are just used to make things more legible */
typedef int player_t;
typedef int seat_t;

struct game_seat_t {
	hand_t hand;
	card_t table;
	const char *name;	/* the name of the player at this seat, or
				   seat label */
	player_t player;	/* the number of the seat's player (-1 for
				   none) */
	char *pmessage;		/* the "player message" (really associated
				   with a seat) */
};

struct game_player_t {
	int score;
	int bid_count;		/* the number of times this player has bid
				   this hand */
	int age;		/* how long the player has been at the table; 
				   simply counts upward from 0 (the first
				   player) */
	bid_t bid;
	bid_t *allbids;		/* corresponds to game.bid_rounds and
				   max_bid_rounds */
	int tricks;
	seat_t seat;		/* the number of the player's seat */
	int ready;

	bid_data_t bid_data;	/* bidding information for this player */
};

#endif /* __TYPES_H__ */
