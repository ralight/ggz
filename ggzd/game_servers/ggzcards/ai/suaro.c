/* 
 * File: ai/suaro.c
 * Author: Jason Short
 * Project: GGZCards Server
 * Date: 10/14/2001
 * Desc: an AI for the game Suaro
 * $Id: suaro.c 3337 2002-02-12 01:40:29Z jdorje $
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

#ifdef HAVE_CONFIG_H
#  include <config.h>		/* Site-specific config */
#endif

#include <stdlib.h>

#include "ai.h"
#include "common.h"

#include "games/suaro.h"

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

/* This just gives an arbitrary impression of suit strength, sort-of
   accounting for cards that have already been played. */
static int count_suit_strength(seat_t seat, char suit, int lo)
{
	card_t card;
	int coefficient = 100;
	int strength = 0;
	int remaining = libai_cards_left_in_suit(suit);

	if (remaining == 0)
		return -1000;

	for (card.face = ACE_HIGH, card.suit = suit, card.deck = 0;
	     card.face >= 8; card.face--) {
		int value = lo ? ACE_HIGH - card.face : card.face;

		if (libai_is_card_played(card.suit, card.face))
			continue;

		if (libai_is_card_in_hand(seat, card)) {
			strength += coefficient * value;
		} else {
			strength -= coefficient * value;
		}

		coefficient = (coefficient * 9) / 10;
	}

	/* Scale depending on how many cards are left. */
	strength /= remaining;

	return strength;
}

/* Finds a normal suit (not a suaro suit) that is strongest, using
   count_suit_strength. */
static char find_best_suit(seat_t seat, int lo)
{
	int bestsuit = -1, bestsuitlength = -1, bestsuitstrength = -1;
	char suit;

	/* First find our longest suit.  This should be a libai function (?) */
	for (suit = 0; suit < 4; suit++) {
		char count = libai_count_suit(seat, suit);
		int suitstrength = count_suit_strength(seat, suit, lo);
		if (count > bestsuitlength
		    || (count == bestsuitlength
			&& suitstrength >= bestsuitstrength)) {
			bestsuitstrength = suitstrength;
			bestsuitlength = count;
			bestsuit = suit;
		}
	}

	assert(bestsuit >= 0);
	return bestsuit;
}

/* this gets a bid or play from the ai */
static bid_t get_bid(player_t p, bid_t * bid_choices, int bid_count)
{
	bid_t min_bid = bid_choices[0];	/* FIXME: assumes correct ordering */
	bid_t my_bid;
	int tricks = 0;		/* number of tricks; x100 */
	seat_t seat = game.players[p].seat;
	card_t c;
	char bidsuit;

	/* Our strategy is fairly simple: we figure out what we should bid
	   and we bid it.  If the opponent has already bid higher, we pass. */

	bidsuit = find_best_suit(seat, 0) + 1;
	ai_debug("Best suit is %s.", short_suaro_suit_names[(int) bidsuit]);

	/* This really needs to be more accurate.  It basically just affects
	   rounding (way below). */
	tricks = (libai_count_suit(seat, bidsuit - 1) > 3) ? 50 : 20;
	ai_debug("Counting %d points for trump strength.", tricks);

	/* Count expected tricks in each suit.  For any card that we don't
	   have, we figure the opponent has (at a conservative estimate) a
	   60% chance of holding the card.  Therefore if we have A-Q in a
	   suit, we'll expect to get 1.5 tricks.  Of course tricks shouldn't
	   really be _added_ in this way - a better way would be to calculate
	   probabilities - but it's a good start. */
	c.deck = 0;
	for (c.suit = 0; c.suit < 4; c.suit++) {
		int cardvalue = 0;

		/* We also track the number of cards the opponent might have
		   in the suit, and figure it to be up to 60% of the total
		   missing cards.  Thus if we have A-K-Q-8, all 4 of these
		   should be counted as winners.  If we have A-K-8, we figure 
		   the opponent has about 2.4 cards in the suit, so we'll
		   expect to get 2.6 tricks. */
		int ourcount = libai_count_suit(seat, c.suit);
		int oppcount = 60 * (7 - ourcount);

		int tricks_in_suit = 0;

		/* This actually counts badly because it doesn't consider
		   that _we_ might run out of cards.  Oh well. */

		ai_debug("We have %d cards in %s.  Figure opponent has %d.",
			 ourcount, suit_names[(int) c.suit], oppcount);

		for (c.face = ACE_HIGH; c.face >= 8; c.face--) {
			if (libai_is_card_in_hand(seat, c)) {
				int value;

				cardvalue += 100;
				value = MAX(cardvalue, 0);

				tricks_in_suit += value;

				ai_debug("Counting %s of %s as %d (%d) points.", face_names[(int) c.face], suit_names[(int) c.suit], value, cardvalue);

				cardvalue = MIN(cardvalue, 0);

				/* Pull an opponent's card. */
				oppcount -= 100;
			} else {
				if (oppcount > 0) {
					/* guess 60% chance opponent has card 
					 */
					oppcount -= 60;
					cardvalue -= 60;

					/* Watch for over-shooting */
					if (oppcount < 0)
						cardvalue += -oppcount;
				}
			}
		}

		ai_debug("Counting %d points in %s.",
			 tricks_in_suit, suit_names[(int) c.suit]);
		tricks += tricks_in_suit;

	}

	/* Now compute final # of tricks */
	ai_debug("Total number of points: %d.", tricks);
	tricks /= 100;

	my_bid.bid = 0;
	if (tricks > min_bid.sbid.val ||
	    (tricks == min_bid.sbid.val && bidsuit > min_bid.sbid.suit)) {
		my_bid.sbid.val = tricks;
		my_bid.sbid.suit = bidsuit;
		ai_debug("Bidding %d %s.", (int) my_bid.sbid.val,
			 short_suaro_suit_names[(int) my_bid.sbid.suit]);
	} else {
		my_bid.sbid.spec = SUARO_PASS;
		ai_debug("Passing.");
	}
	return my_bid;
}

static card_t get_play(player_t p, seat_t seat)
{
	int leader = (game.leader == p);
	char trump = SUARO.contract_suit;	/* Suaro suit values */

	if (leader) {
		/* Pick a good lead. */
		int declarer = (SUARO.declarer == p);	/* are we the
							   declarer? */

		if (trump == SUARO_HIGH || trump == SUARO_LOW) {
			card_t card;

			card.suit = find_best_suit(seat, trump == SUARO_LOW);

			for (card.face = ACE_HIGH, card.deck = 0;
			     card.face >= 8; card.face--) {
				if (libai_is_card_in_hand(seat, card)) {
					/* Highest card in best suit - take
					   it. */
					return card;
				}
			}
		} else {
			card_t card;

			/* If we're the declarer, prefer to lead trump.
			   Otherwise just take our strongest suit. */
			if (declarer && libai_count_suit(seat, trump - 1) > 1)
				card.suit = trump - 1;
			else
				card.suit = find_best_suit(seat, 0);

			/* Look for the highest card. */
			for (card.face = ACE_HIGH, card.deck = 0;
			     card.face >= 8; card.face--)
				if (libai_is_card_in_hand(seat, card)) {
					/* Highest card in best suit - take
					   it. */
					return card;
				}
		}
	} else {
		card_t opp_card = game.seats[2 - seat].table;
		/* Pick a good response. */
		if (libai_count_suit(seat, opp_card.suit) == 0) {
			card_t card;

			/* Try to trump. */
			if (libai_count_suit(seat, trump - 1)) {
				card_t card;

				for (card.suit = trump - 1, card.face =
				     8, card.deck = 0; card.face <= ACE_HIGH;
				     card.face++) {
					if (libai_is_card_in_hand(seat, card)) {
						/* Lowest possible winner -
						   take it. */
						return card;
					}
				}
			}

			/* Throw. */
			for (card.suit = CLUBS, card.face = 8, card.deck = 0;
			     card.face <= ACE_HIGH;
			     card.suit++, card.face =
			     (card.suit > SPADES ? card.face + 1 : card.face),
			     card.suit %= 4) {
				if (libai_is_card_in_hand(seat, card)) {
					/* Lowest possible trash card - toss
					   it. */
					return card;
				}
			}
		} else {
			/* We must follow suit. */
			card_t card;

			/* Try to win trick. */
			for (card = opp_card; card.face <= ACE_HIGH;
			     card.face++) {
				if (libai_is_card_in_hand(seat, card)) {
					/* Lowest possible winner - take it. */
					return card;
				}
			}

			/* Otherwise try to lose trick. */
			for (card.face = 8, card.suit =
			     opp_card.suit, card.deck = 0;
			     card.face < opp_card.face; card.face++) {
				if (libai_is_card_in_hand(seat, card)) {
					/* Lowest possible loser - take it. */
					return card;
				}
			}
		}
	}

	assert(0);
	return UNKNOWN_CARD;
}
