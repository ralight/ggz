/* 
 * File: ai/suaro.c
 * Author: Jason Short
 * Project: GGZCards Server
 * Date: 10/14/2001
 * Desc: an AI for the game Suaro
 * $Id: suaro.c 2732 2001-11-13 06:56:14Z jdorje $
 *
 * This file contains the AI functions for playing Suaro.
 *
 * Copyright (C) 2001 GGZ Dev Team.
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

#include <stdlib.h>

#include "../ai.h"
#include "../common.h"

#include "../games/suaro.h"

#include "aicommon.h"

static char *get_name(player_t p);
static void start_hand(void);
static void alert_bid(player_t p, bid_t bid);
static void alert_play(player_t p, card_t card);
static bid_t get_bid(player_t p, bid_t * bid_choices, int bid_count);
static card_t get_play(player_t p, seat_t s);

struct ai_function_pointers suaro_ai_funcs = {
	get_name,
	start_hand,
	alert_bid,
	alert_play,
	get_bid,
	get_play
};

static char *get_name(player_t p)
{
	return NULL;
}

/* this inits AI static data at the start of a hand */
static void start_hand(void)
{

}

/* this alerts the ai to someone else's bid/play */
static void alert_bid(player_t p, bid_t bid)
{
	/* we really need to take advantage of this information! */
}

static void alert_play(player_t p, card_t card)
{

}

/* this gets a bid or play from the ai */
static bid_t get_bid(player_t p, bid_t * bid_choices, int bid_count)
{
	bid_t min_bid = bid_choices[0];	/* FIXME: assumes correct ordering */
	bid_t my_bid;
	int tricks = 0;		/* number of tricks; x100 */
	seat_t s = game.players[p].seat;
	card_t c;
	char suit;
	char bestsuit = -1, bestsuitlength = -1;

	/* Our strategy is fairly simple: we figure out what we should bid
	   and we bid it.  If the opponent has already bid higher, we pass. */

	/* First find our longest suit.  This should be a libai function (?) */
	for (suit = 0; suit < 4; suit++) {
		char count = libai_count_suit(s, suit);
		if (count >= bestsuitlength) {
			bestsuitlength = count;
			bestsuit = suit;
		}
	}
	if (bestsuit < 0)
		abort();
	bestsuit++;		/* suit 0 is lo */
	ai_debug("Best suit is %s.", short_suaro_suit_names[(int) bestsuit]);

	/* Count expected tricks in each suit. */
	c.deck = 0;
	for (c.suit = 0; c.suit < 4; c.suit++) {
		int gap = 0;
		int oppcount = 7 - libai_count_suit(s, c.suit);
		oppcount = oppcount * 60;
		for (c.face = ACE_HIGH; c.face >= 8; c.face--) {
			if (libai_is_card_in_hand(s, c)) {
				gap -= 100;
				if (gap <= 0) {
					int diff = -gap;
					ai_debug("Counting %s of %s as %d points.", face_names[(int) c.face], suit_names[(int) c.suit], diff);
					tricks += diff;
					gap = 0;
				}
			} else {
				if (oppcount > 0) {
					gap += 60;	/* guess 60% chance
							   opponent has card */
				}
			}
			oppcount -= 60;	/* opponent plays one card */
		}
	}

	/* Now compute final # of tricks */
	tricks /= 100;
	ai_debug("Total number of points: %d.", tricks);

	my_bid.bid = 0;
	if (tricks > min_bid.sbid.val ||
	    (tricks == min_bid.sbid.val && bestsuit > min_bid.sbid.suit)) {
		my_bid.sbid.val = tricks;
		my_bid.sbid.suit = bestsuit;
		ai_debug("Bidding %d %s.", (int) my_bid.sbid.val,
			 short_suaro_suit_names[(int) my_bid.sbid.suit]);
	} else {
		my_bid.sbid.spec = SUARO_PASS;
		ai_debug("Passing.");
	}
	return my_bid;
}

static card_t get_play(player_t p, seat_t s)
{
	hand_t *hand = &game.seats[s].hand;
	int choices[hand->hand_size], i, max;

	for (i = 0; i < hand->hand_size; i++)
		choices[i] = i;

	/* This clever algorithm will search at most n times to find at
	   random a correct element from the set of n */
	for (max = hand->hand_size; max > 0; max--) {
		int choice = random() % max;
		card_t selection = hand->cards[choices[choice]];
		char *error = game.funcs->verify_play(selection);
		if (error == NULL)
			return selection;

		choices[choice] = choices[max - 1];
	}

	ai_debug("get_play: " "no valid play found.");
	return UNKNOWN_CARD;
}
