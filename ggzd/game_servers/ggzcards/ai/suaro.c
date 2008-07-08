/* 
 * File: ai/suaro.c
 * Author: Jason Short
 * Project: GGZCards Server
 * Date: 10/14/2001
 * Desc: an AI for the game Suaro
 * $Id: suaro.c 10224 2008-07-08 18:17:29Z jdorje $
 *
 * This file contains the AI functions for playing Suaro.
 *
 * Copyright (C) 2001-2002 GGZ Development Team
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

#include <assert.h>
#include <stdlib.h>

#include <ggz.h>

#include "shared.h"

#include "client.h"

#include "game.h"
#include "suaro.h"
#include "aicommon.h"

/* 
 *
 *  This is a simple expert system.  It follows the following algorithm:
 *
 * BIDDING
 *
 * We basically count the number of "points" we have, where 100 points
 * equals one trick.
 *
 * - Find longest, strongest [1] suit.  Let this be trump.
 * - Assign yourself 20 points for a 3-card trump, 50 points for a 4+ card
 *   trump.
 * - For each suit, count the estimated number of points:
 *   - Figure that the opponent has a 60% chance of having any card
 *     we don't, and that they'll have 60% of the missing cards in the
 *     suit.  [This is conservative - experimenting with values in the
 *     50-60% range could be productive.]
 *   - Assign ourselves 100 points for any sure winners, and take off
 *     one of our opponent's cards.
 *   - For any cards we don't have, consider that the opponent has a
 *     60% chance of having that card.  Thus any cards we have below
 *     it have their values reduced by 60 points.  We also take off
 *     .6 opponents cards for such a play.  Of course, if the opponent
 *     is out of potential cards this won't take effect.
 *   - Examples: A-Q     => 1.4 tricks (60% chance opponent has K)
 *               K-Q     => 1.4 tricks (60% chance opponent has A)
 *               A-K-Q-8 => 4 tricks
 *               A-K-8   => 2.6 tricks (expext opponent to have 2.4 cards)
 * - Total up the number of points, and divide by 100 rounding down.
 * - This is our bid: suit and count.  If we can't make this bid (opponent
 *   already bid too high), just pass.
 *
 * Problems: it doesn't take into account opponents bidding.  There's no
 * chance of it bidding on the kitty.  It is too conservative, and always
 * makes its bid (but doesn't bid often enough).
 *
 *
 * PLAYING
 *
 * - If we're leading:
 *   * If we're the declarer and have more than one trump, lead trump.
 *   * Otherwise pick our longest, strongest [1] suit.
 *   * Lead the highest card in this suit.
 * - If we're not leading:
 *   * If we can't follow suit:
 *     - If we have any trump, play our lowest one.
 *     - Otherwise, play our lowest [2] card.
 *   * If we can follow suit:
 *     - Play our lowest card higher than the card lead, if possible.
 *     - Otherwise play our lowest card.
 *
 * Problems: it never throws.  It never considers splits, and will lead from
 * a K-J rather than a Q-J.  It never tries to put the opponent in the lead,
 * but always leads the highest card available.
 *
 */

static struct {
	int declarer;
	bid_t contract;
	char trump;
} ai;

enum {
	ME = 0,
	OPP = 2
};

static const char *short_suaro_suit_name(int suit)
{
	const char *names[] = { "lo", "C", "D", "H", "S", "hi" };

	assert(suit >= 0 && suit < 6);
	return names[suit];
}


/* this inits AI static data at the start of a hand */
void start_hand(void)
{
	ailib_start_hand();
	ai.declarer = -1;
	ai.trump = -1;
}

/* this alerts the ai to someone else's bid/play */
void alert_bid(int player, bid_t bid)
{
	ailib_alert_bid(player, bid);
	
	/* we really need to take advantage of this information! */

	if (bid.sbid.val > 0) {
		ai.declarer = player;
		ai.contract = bid;

		ai.contract.sbid.suit = bid.sbid.suit;
		if (ai.contract.sbid.suit == SUARO_HIGH
		    || ai.contract.sbid.suit == SUARO_LOW)
			ai.trump = -1;
		else
			ai.trump = ai.contract.sbid.suit - 1;
	}
}

void alert_play(int player, card_t card)
{
	ailib_alert_play(player, card);
	/* nothing else */
}

void alert_trick(int winner)
{
	ailib_alert_trick(winner);
}

/* This just gives an arbitrary impression of suit strength, sort-of
   accounting for cards that have already been played. */
static int count_suit_strength(char suit, bool lo)
{
	card_t card;
	int coefficient = 100;
	int strength = 0;
	int remaining = 7 - libai_cards_played_in_suit(suit);

	if (remaining == 0)
		return -1000;

	for (card.face = ACE_HIGH, card.suit = suit, card.deck = 0;
	     card.face >= 8; card.face--) {
		int value = lo ? ACE_HIGH - card.face : card.face;

		if (libai_is_card_played(card.suit, card.face))
			continue;

		if (libai_is_card_in_hand(ME, card)) {
			strength += coefficient * value;
		} else {
			strength -= coefficient * value;
		}

		coefficient = (coefficient * 9) / 10;
	}

	/* Scale depending on how many cards are left. */
	strength = (strength * 7) / remaining;

	return strength;
}

/* Finds a normal suit (not a suaro suit) that is strongest, using
   count_suit_strength. */
static char find_best_suit(bool lo)
{
	int bestsuit = -1, bestsuitlength = -1, bestsuitstrength = -1;
	char suit;

	/* First find our longest suit.  This should be a libai function (?) */
	for (suit = 0; suit < 4; suit++) {
		char count = libai_count_suit(ME, suit);
		int suitstrength = count_suit_strength(suit, lo);
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
bid_t get_bid(bid_t * bid_choices, int bid_count)
{
	bid_t min_bid;
	bid_t my_bid;
	int tricks = 0;		/* number of tricks; x100 */
	int i;
	card_t c;
	char bidsuit;
	
	min_bid.sbid.val = 10;
	for (i = 0; i < bid_count; i++) {
		bid_t bid = bid_choices[i];
		if (bid.sbid.spec == SUARO_BID)
			if (bid.sbid.val < min_bid.sbid.val
			    || (bid.sbid.val == min_bid.sbid.val
			        && bid.sbid.suit < min_bid.sbid.suit))
				min_bid = bid;
			
	}

	/* Our strategy is fairly simple: we figure out what we should bid
	   and we bid it.  If the opponent has already bid higher, we pass. */

	bidsuit = find_best_suit(FALSE) + 1;
	ggz_debug(DBG_BID, "Best suit is %s.",
		  short_suaro_suit_name(bidsuit));

	/* This really needs to be more accurate.  It basically just affects
	   rounding (way below). */
	tricks = (libai_count_suit(0, bidsuit - 1) > 3) ? 50 : 20;
	ggz_debug(DBG_BID, "Counting %d points for trump strength.", tricks);

	/* Count expected tricks in each suit.  For any card that we don't
	   have, we figure the opponent has (at a conservative estimate) a
	   60% chance of holding the card.  Therefore if we have A-Q in a
	   suit, we'll expect to get 1.4 tricks.  Of course tricks shouldn't
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
		int ourcount = libai_count_suit(ME, c.suit);
		int oppcount = 60 * (7 - ourcount);

		int tricks_in_suit = 0;

		/* This actually counts badly because it doesn't consider
		   that _we_ might run out of cards.  Oh well. */

		ggz_debug(DBG_BID,
			  "We have %d cards in %s.  Figure opponent has %d.",
			  ourcount, get_suit_name(c.suit), oppcount);

		for (c.face = ACE_HIGH; c.face >= 8; c.face--) {
			if (libai_is_card_in_hand(ME, c)) {
				int value;

				cardvalue += 100;
				value = MAX(cardvalue, 0);

				tricks_in_suit += value;

				ggz_debug(DBG_BID,
					  "Counting %d of %s as %d (%d) points.",
					  c.face, get_suit_name(c.suit),
					  value, cardvalue);

				cardvalue = MIN(cardvalue, 0);

				/* Pull an opponent's card. */
				oppcount -= 100;
			} else {
				if (oppcount > 0) {
					/* guess 60% chance opponent has card */
					oppcount -= 60;
					cardvalue -= 60;

					/* Watch for over-shooting */
					if (oppcount < 0)
						cardvalue += -oppcount;
				}
			}
		}

		ggz_debug(DBG_BID, "Counting %d points in %s.",
			  tricks_in_suit, get_suit_name(c.suit));
		tricks += tricks_in_suit;

	}

	/* Now compute final # of tricks */
	ggz_debug(DBG_BID, "Total number of points: %d.", tricks);
	tricks /= 100;

	my_bid.bid = 0;
	if (tricks > min_bid.sbid.val ||
	    (tricks == min_bid.sbid.val && bidsuit > min_bid.sbid.suit)) {
		my_bid.sbid.val = tricks;
		my_bid.sbid.suit = bidsuit;
		ggz_debug(DBG_BID, "Bidding %d %s.", (int) my_bid.sbid.val,
			  short_suaro_suit_name(my_bid.sbid.suit));
	} else {
		my_bid.sbid.spec = SUARO_PASS;
		ggz_debug(DBG_BID, "Passing.");
	}
	return my_bid;
}

card_t get_play(int play_seat, bool *valid_plays)
{
	assert(play_seat == ME);
	
	ailib_our_play(play_seat);

	if (get_leader() == ME) {
		/* Pick a good lead. */
		char suit;

		/* If we're the declarer and there is a trump, we want to
		   pull trump. Otherwise just pick a strong suit. */
		if (ai.declarer == ME
		    && ai.trump >= 0 && libai_count_suit(ME, ai.trump) > 1)
			suit = ai.trump;
		else
			suit = find_best_suit(ai.contract.sbid.suit ==
					      SUARO_LOW);
		
		ggz_debug(DBG_PLAY, "I'm leading.  My strongest suit is %s.",
		          get_suit_name(suit));

		return libai_get_highest_card_in_suit(ME, suit);
	} else {
		/* Pick a good response. */

		card_t opp_card = ggzcards.players[OPP].table_card;
		
		ggz_debug(DBG_PLAY, "I'm following the %s of %s.",
		          get_face_name(opp_card.face),
		          get_suit_name(opp_card.suit));

		/* FIXME: none of this section can deal with low bids. */

		if (libai_count_suit(ME, opp_card.suit) == 0) {
			card_t card;
			
			ggz_debug(DBG_PLAY, "Looks like I'm out of %s.",
			          get_suit_name(opp_card.suit));

			/* Try to trump. */
			if (ai.trump >= 0 && libai_count_suit(ME, ai.trump) > 0) {
				for (card.suit = ai.trump, card.face =
				     8, card.deck = 0; card.face <= ACE_HIGH;
				     card.face++) {
					if (libai_is_card_in_hand(ME, card)) {
						/* Lowest possible winner -
						   take it. */
						return card;
					}
				}
			}
			
			ggz_debug(DBG_PLAY, "Looks like I'm out of trump, too.");

			/* Throw. */
			for (card.suit = CLUBS, card.face = 8, card.deck = 0;
			     card.face <= ACE_HIGH;
			     card.suit++, card.face =
			     (card.suit > SPADES ? card.face + 1 : card.face),
			     card.suit %= 4) {
				if (libai_is_card_in_hand(ME, card)) {
					/* Lowest possible trash card - toss
					   it. */
					return card;
				}
			}
		} else {
			/* We must follow suit. */
			card_t card;
			
			ggz_debug(DBG_PLAY, "I must follow suit.");

			/* Try to win trick. */
			for (card = opp_card; card.face <= ACE_HIGH;
			     card.face++) {
				if (libai_is_card_in_hand(ME, card)) {
					/* Lowest possible winner - take it. */
					return card;
				}
			}
			
			ggz_debug(DBG_PLAY, "Looks like I can't win.");

			/* Otherwise try to lose trick. */
			for (card.face = 8, card.suit =
			     opp_card.suit, card.deck = 0;
			     card.face < opp_card.face; card.face++) {
				if (libai_is_card_in_hand(ME, card)) {
					/* Lowest possible loser - take it. */
					return card;
				}
			}
		}
	}
	
	ggz_debug(DBG_PLAY, "Uh oh!  Could not find a card to play!");

	assert(FALSE);
	return UNKNOWN_CARD;
}
