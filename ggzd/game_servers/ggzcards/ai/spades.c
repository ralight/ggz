/* 
 * File: ai/spades.c
 * Author: Brent Hendricks, Jason Short
 * Project: GGZCards Server
 * Date: 8/4/99
 * Desc: NetSpades algorithms for Spades AI
 * $Id: spades.c 7268 2005-06-10 12:28:19Z josef $
 *
 * This file contains the AI functions for playing spades.
 * The AI routines were adapted from Britt Yenne's spades game for
 * the palm pilot.  Thanks Britt!
 *
 * Later, they were stolen from NetSpades and integrated directly into
 * GGZCards for use with the spades game here.  Thanks Brent!
 *
 * For some good advice on Spades strategy, see:
 *               http://www.masterspades.com/
 *
 * Copyright (C) 1998-2002 Brent Hendricks.
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
#include <string.h>

#include <ggz.h>

#include "shared.h"

#include "aicommon.h"
#include "client.h"
#include "game.h"
#include "spades.h"

#define nil_tricks_count 1	/* FIXME */
#define minimum_team_bid 3	/* FIXME */

#define IS_GOING_NIL(p) (bids[p].sbid.spec == SPADES_NIL || \
			 bids[p].sbid.spec == SPADES_DOUBLE_NIL)

/* #define USE_AI_TRICKS */

/* Aggressive bidding causes us to value our hand more aggressively.
   Unfortunately the AI cannot back this up with good play. */
/* #define AGGRESSIVE_BIDDING */

void start_hand(void);
void alert_bid(int p, bid_t bid);
void alert_play(int p, card_t card);
bid_t get_bid(bid_t * bid_choices, int bid_count);
card_t get_play(int play_hand, bool *valid_plays);

enum {
	ME = 0,
	L_OPP = 1,
	PARTNER = 2,
	R_OPP = 3
};

static int count_cards[4];	/* the number of "count" cards each player
				   has played.  Count cards are A/K of a
				   suit, A/K/Q of spades, or a fourth or more
				   spade.  Tracking these can help us to
				   determine what the player has left. */

static struct play {
	card_t card;		/* card's value */
	int trick;		/* likelyhood card will take this trick */
	int future;		/* likelyhood card will take future trick */
} play[13];
static int plays, high;

int has_bid[4];
bid_t bids[4];
static int partner_lead_spades;

static int count_spade_ruff_winners(void);
static int count_spade_strength_winners(void);
static int count_spade_tricks(void);
static int count_nonspade_tricks(char suit);
static char find_final_bid(int points);
static int consider_bidding_nil(int points);

static void Calculate(struct play *play, int agg);
static int SuitMap(int p, char suit);
static int PlayNil(void);
static int CoverNil(int agg);
static int SetNil(int agg);
static int PlayNormal(int agg, int lastTrick);
static int card_comp(card_t c1, card_t c2);

void start_hand(void)
{
	int p;
	
	ailib_start_hand();

	for (p = 0; p < 4; p++) {
		count_cards[p] = 0;
		has_bid[p] = FALSE;
	}

	partner_lead_spades = 0;
}

void alert_bid(int p, bid_t bid)
{
	ailib_alert_bid(p, bid);
	
	assert(p >= 0 && p < 4);

	has_bid[p] = 1;
	bids[p] = bid;
}

void alert_play(int p, card_t play)
{
#ifdef USE_AI_TRICKS
	card_t lead = ggzcards.players[get_leader()].table_card;
#endif
	ailib_alert_play(p, play);

	assert(p >= 0 && p < 4);

	/* Track count cards */
	if (play.face == ACE_HIGH || play.face == KING ||
	    (play.face == QUEEN && play.suit == SPADES) ||
	    (play.suit == SPADES
	     && libai_cards_played_in_suit_p(p, SPADES) >= 4)) {
		ggz_debug(DBG_AI,
			  "Counting %s of %s as a count card for player %d.",
			  get_face_name(play.face),
			  get_suit_name(play.suit), p);
		count_cards[p]++;
	}

	/* The problem with both these "tricks" below is that if a player has
	   a run of cards, he may as well play his low instead of his high -
	   which would confuse us a lot.  So although it is a good method
	   generally, in the form it's in here it's no good.  --JDS */

#ifdef USE_AI_TRICKS
	/* when a nil player sluffs, they'll sluff their highest card in that
	   suit */
	if (IS_GOING_NIL(p)
	    && ggzcards.players[p].tricks == 0 && play.suit != lead.suit
	    && play.suit != SPADES) {
		card_t card = play;
		for (card.face++; card.face <= ACE_HIGH; card.face++)
			libai_player_doesnt_have_card(p, card);
	}
#endif

#ifdef USE_AI_TRICKS
	/* a nil player will generally play their highest card beneath an
	   already-played card */
	if (IS_GOING_NIL(p) &&
	    game.players[p].tricks == 0 && play.suit == lead.suit) {
		if (!libai_might_player_have_card(p, play)) {
			/* If we didn't think this player could have this
			   card, then he's doing something funky and all bets
			   are off.  He might have anything. */
			libai_forget_players_hand(p, play.suit);
		} else if (play.face < -1	/* THE HIGHEST CARD PLAYED SO
						   FAR IN THIS SUIT */ ) {
			/* nil bids don't have any between this card and
			   highest play */
			card_t card = play;
			for (card.face++; card.face < -1	/* THE
								   HIGHEST
								   CARD
								   PLAYED SO
								   FAR IN
								   THIS SUIT */ ; card.face++)
				libai_player_doesnt_have_card(p, card);
		}
	}
#endif

	if (p == get_leader() && p == PARTNER && play.suit == SPADES)
		partner_lead_spades = 1;
}

void alert_trick(int winner)
{
	ailib_alert_trick(winner);
}


/* This counts the possible winners you'll get from ruffing (trumping in).
   It's only accurate with weak trumps; with strong spades it'll underbid. */
static int count_spade_ruff_winners(void)
{
	int points = 0, count, suitlen;
	card_t c = { ACE_HIGH, SPADES, 0 };	/* suit=SPADES and deck=0 */
	char s;
	int shortsuits[3] = { 0, 0, 0 };	/* number of voids,
						   singletons, doubletons we
						   have */
#ifdef AGGRESSIVE_BIDDING
	int shortvalues[3] = { 90, 75, 50 };
#else
	int shortvalues[3] = { 90, 70, 40 };	/* percentage chance of
						   winning with a ruff */
#endif

	/* count our shortsuits */
	for (s = 0; s < 4; s++) {
		if (s == SPADES)
			continue;
		count = libai_count_suit(ME, s);
		if (count < 3)
			shortsuits[count]++;
	}

	/* count our spades */
	count = libai_count_suit(ME, SPADES);

	/* First count sure winners. */
	for (c.face = ACE_HIGH; c.face >= 2 && libai_is_card_in_hand(ME, c);
	     c.face--) {
		points += 100;
		count--;
	}

	/* Now figure if we use our trumps on other suits */
	for (suitlen = 0; suitlen < 3; suitlen++) {	/* voids, singletons,
							   doubletons */
		for (; shortsuits[suitlen] > 0 && count > 0;
		     shortsuits[suitlen]--) {
			/* each chance we get to trump in has a certain
			   chance of winning the trick and uses up one trump.
			 */
			points += shortvalues[suitlen];
			count--;
			ggz_debug(DBG_BID, "Count ruff on trick %d as %d.",
				  suitlen + 1, shortvalues[suitlen]);
			if (suitlen < 2)
				shortsuits[suitlen + 1]++;
		}
	}

	return points;
}

/* This method of calculating trump winners ONLY counts length and strength
   in spades.  It is most accurate when you have 4 or more trumps */
static int count_spade_strength_winners(void)
{
	int count = libai_count_suit(ME, SPADES);
	int points = 0;
	card_t card = { ACE_HIGH, SPADES, 0 };

	ggz_debug(DBG_BID, "Counted %d spades.", count);

	/* The ace/king/queen each get counted as one trick. */
	/* Note that if we have fewer than 3 trumps some points will be
	   subtracted later. */
	for (card.face = ACE_HIGH; card.face >= QUEEN; card.face--) {
		if (libai_is_card_in_hand(ME, card)) {
			ggz_debug(DBG_BID, "Counted %s of spades as 1 trick.",
				  get_face_name(card.face));
			points += 100;

			/* A special case tweak. */
			if (card.face == QUEEN && count <= 2)
				points -= 20;
		}
	}

	/* The fourth spade gets counted as a 50% trick. */
	if (count >= 4) {
#ifdef AGGRESSIVE_BIDDING
		ggz_debug(DBG_BID, "Counted 4th spade 1 trick.");
		points += 100;
#else
		ggz_debug(DBG_BID, "Counted 4th spade as 3/4 trick.");
		points += 75;
#endif
	}

	/* The 5th and every spade thereafter gets counted 1 trick. */
	if (count >= 5) {
#ifndef AGGRESSIVE_BIDDING
		/* also count the fourth spade as a full trick */
		ggz_debug(DBG_BID, "Counted 4th spade as another 1/4 trick.");
		points += 25;
#endif
		ggz_debug(DBG_BID, "Counted extra %d spades as %d tricks.",
			  count - 4, count - 4);
		points += 100 * (count - 4);
	}

	return points;
}


static int count_spade_tricks(void)
{
	int p1 = count_spade_ruff_winners();
	int p2 = count_spade_strength_winners();
	int points = (p1 > p2) ? p1 : p2;

	ggz_debug(DBG_BID,
		  "Counted ruff winners as %d and strength winners as %d.",
		  p1, p2);

	/* A special case: take off tricks for weak spades. Although it might
	   not make sense to have negative tricks in spades, having this
	   weakness will cause your partner to lose tricks. */
	switch (libai_count_suit(ME, SPADES)) {
	case 0:
		points -= 100;
		break;
	case 1:
		points -= 50;
		break;
	case 2:
		points -= 20;
		break;
	default:
		break;
	}

	ggz_debug(DBG_BID, "Final spade points: %d", points);
	return points;
}

static int count_nonspade_tricks(char suit)
{
	card_t ace = { ACE_HIGH, suit, 0 };
	card_t king = { KING, suit, 0 };
	card_t queen = { QUEEN, suit, 0 };
	int has_ace = libai_is_card_in_hand(ME, ace);
	int has_king = libai_is_card_in_hand(ME, king);
	int has_queen = libai_is_card_in_hand(ME, queen);
	int count = libai_count_suit(ME, suit);
	int spades = libai_count_suit(ME, SPADES);

	/* This is an extremely "expert" system to count probable tricks in a
	   non-spade suit.  It really needs to be tweaked; perhaps by studying
	   master spades players or by testing it head-to-head. Another idea is
	   to make a 3D array: countcards x count x spades, to count how many
	   tricks we expect to win for any given set of count cards, quantity
	   of cards in the suit, and quantity of spades.  But as it is, it
	   shouldn't be too scrappy. */

	if (has_ace) {
		if (has_king) {
			if (spades > 3)
				return 200;

			switch (count) {
			case 2:
			case 3:
				return 200;
			case 4:
				return 180;
			case 5:
				return 140;
			case 6:
				return 100;
			case 7:
				return 80;
			case 8:
				return 50;
			default:
				assert(count > 8);
				return 0;
			}
		}

		if (has_queen) {
			if (spades > 3)
				return 150;

			switch (count) {
			case 2:
			case 3:
				return 150;
			case 4:
				return 140;
			case 5:
				return 120;
			case 6:
				return 95;
			case 7:
				return 80;
			case 8:
				return 50;
			default:
				assert(count > 8);
				return 0;
			}
		}

		if (spades > 3)
			return 100;

		switch (count) {
		case 1:
		case 2:
		case 3:
		case 4:
			return 100;
		case 5:
			return 95;
		case 6:
			return 85;
		case 7:
			return 70;
		case 8:
			return 50;
		default:
			assert(count > 8);
			return 0;
		}
	}

	if (has_king) {
		if (has_queen) {
			switch (count) {
			case 2:
				return 110;
			case 3:
				return 165;
			case 4:
				return (spades > 3) ? 170 : 110;
			case 5:
				return (spades > 3) ? 150 : 70;
			case 6:
				return (spades > 3) ? 130 : 30;
			default:
				assert(count > 6);
				return (spades > 3) ? 100 : 0;
			}
		}

		switch (count) {
		case 1:
			return 30;
		case 2:
			return 80;
		case 3:
			return 90;
		case 4:
			return 70;
		case 5:
			return 30;
		default:
			assert(count > 5);
			return 0;
		}
	}

	return 0;
}

static char find_final_bid(int points)
{
	int prob;		/* for rounding */
	char bid_val, total, other_total;
	int bid_count = has_bid[L_OPP] + has_bid[PARTNER] + has_bid[R_OPP];

	ggz_debug(DBG_BID, "Bid count is %d.", bid_count);

	/* --------------- NORMAL BIDS ----------------- */
	/* Okay, divide points by 100 and that's our bid. */
	switch (bid_count) {
	case 0:
		prob = 50;
		break;
	case 1:
		prob = 40;
		break;
	case 2:
		prob = 30;
		break;
	case 3:
		prob = 60;

		/* Total between "other" players. */
		other_total =
			bids[L_OPP].sbid.val + bids[PARTNER].sbid.val +
			bids[R_OPP].sbid.val;

		/* We attempt to balance the bid toward 10/11.  This could be
		   dangerous... */
		bid_val = (points + prob) / 100;
		total = bid_val + other_total;

		ggz_debug(DBG_BID, "Total bid sum is %d.", total);

		if (total < 8) {
			prob += 120;
		} else if (total == 8) {
			prob += 90;
		} else if (total == 9) {
			prob += 70;
		} else if (total == 10) {
			prob += 20;
		}

		if (total > 13) {
			/* Reduce a 14+ bid to 13. */
			prob = 100 * (13 - other_total) - points;
		} else if (total == 13) {
			prob -= 70;
		} else if (total == 12) {
			prob -= 30;
		}

		break;
	default:
		assert(FALSE);
		prob = 0;
		break;
	}
	ggz_debug(DBG_BID, "Adding %d points for rounding.", prob);

	bid_val = (points + prob) / 100;
	bid_val = MAX(bid_val, 0);
	ggz_debug(DBG_BID, "Subtotal bid: %d", bid_val);

	/* don't forget the minimum bid!!! */
	if (has_bid[PARTNER]
	    && (bid_val + bids[PARTNER].sbid.val < minimum_team_bid)) {
		bid_val = minimum_team_bid - bids[PARTNER].sbid.val;
		ggz_debug(DBG_BID, "Upping bid to meet minimum of %d",
			  minimum_team_bid);
	}

	/* And avoid bidding higher than 13 */
	if (has_bid[PARTNER] && bids[PARTNER].sbid.val + bid_val > 13)
		bid_val = 13 - bids[PARTNER].sbid.val;

	return bid_val;
}

/* Returns TRUE if we should bid nil, FALSE otherwise */
static int consider_bidding_nil(int points)
{
	int nilrisk = 0, count, suitCount[4];
	char s;
	card_t ace_of_spades = { ACE_HIGH, SPADES, 0 };

	/* First count our cards in each suit.  Also count our voids,
	   singletons, and doubletons. */
	for (s = 0; s < 4; s++)
		suitCount[(int) s] = libai_count_suit(ME, s);

	if (suitCount[SPADES] > 4 || libai_is_card_in_hand(ME, ace_of_spades)) {
		ggz_debug(DBG_BID,
			  "Our spades are too strong to consider bidding nil.");
		return 0;
	}

	if (suitCount[SPADES] == 4) {
		nilrisk += 4;
		ggz_debug(DBG_BID, "Inc. nilrisk by 4 for 4 spades");
	}
	for (s = 0; s < 4; s++) {
		card_t c = { 0, 0, 0 };	/* make sure it's deck 0 */
		c.suit = s;

		/* Voids are very helpful! */
		if (suitCount[(int) s] == 0) {
			nilrisk -= 2;
			ggz_debug(DBG_BID, "Dec. nilrisk for shortsuit in %s",
				  get_suit_name(s));
			continue;	/* for( s = 0; s < 4; s++ ) */
		}

		/* count our low cards in the suit */
		for (count = 0, c.face = 2; c.face < 9; c.face++) {
			if (libai_is_card_in_hand(ME, c))
				count++;	/* count low cards */
		}
		if (count >= 3)	/* suit is covered */
			continue;	/* for( s = 0; s < 4; s++ ) */

		nilrisk += suitCount[(int) s] - count;	/* risky high cards */
		ggz_debug(DBG_BID, "Inc. nilrisk for %d high %s",
			  suitCount[(int) s] - count, get_suit_name(s));
		if (count == 0) {	/* no low cards */
			nilrisk++;
			ggz_debug(DBG_BID, "Inc. nilrisk for no low cards");
		}

		if (s == SPADES) {
			/* check for high spades */
			/* Length will make these cards safer.  Of course,
			   length has its own dangers (see above). */
			for (c.face = KING; c.face >= 10; c.face--) {
				if (libai_is_card_in_hand(ME, c)) {
					int risk =
						(c.face - 8) -
						suitCount[SPADES];
					nilrisk += risk;
					ggz_debug(DBG_BID,
						  "Inc. nilrisk by %d for %s of spades",
						  risk,
						  get_face_name(c.face));
				}
			}
		} else {
			/* Check for high cards in other suits. */
			/* High cards are riskier when you have less
			   coverage. */
			int risk = 4 - suitCount[(int) s];

			c.face = KING;
			if (libai_is_card_in_hand(ME, c)
			    && suitCount[(int) s] <= 3) {
				nilrisk += risk;
				ggz_debug(DBG_BID,
					  "Inc. nilrisk by %d for King of %s",
					  risk, get_suit_name(s));
			}

			risk++;	/* Ace is riskier */
			c.face = ACE_HIGH;
			if (libai_is_card_in_hand(ME, c)
			    && suitCount[(int) s] <= 3) {
				nilrisk += risk;
				ggz_debug(DBG_BID,
					  "Inc. nilrisk by %d for Ace of %s",
					  risk, get_suit_name(s));
			}
		}
	}

	/* --------------- NIL BIDS ----------------- */
	/* Consider bidding nil. */
	if (IS_GOING_NIL(L_OPP) || IS_GOING_NIL(R_OPP)) {
		nilrisk -= 2;
		ggz_debug(DBG_BID,
			  "Dec. nilrisk because opponent is kneeling");
	}

	if ((!has_bid[PARTNER] || bids[PARTNER].sbid.val >= minimum_team_bid)
	    && ((nilrisk <= 0) || (nilrisk <= 1 && points < 250)
		|| (nilrisk <= 2 && points < 150)
		|| (bids[PARTNER].sbid.val >= 3 + nilrisk))) {
		return 1;
	}

	return 0;
}

/*
 * "If you are going to bid aggressive you must play like an expert."
 *                          --Jay Tomlinson
 *
 * Unfortunately, we certainly don't play like an expert.  It seems easier to
 * adjust the bidding code to bid like an expert than to make the AI play like
 * an expert.
 */
bid_t get_bid(bid_t * bid_choices, int bid_count)
{
	int points;
	bid_t bid;
	char suit;

	bid.bid = 0;		/* reset bid */

	/* Count winners in all suits. */
	points = count_spade_tricks();

	for (suit = CLUBS; suit <= HEARTS; suit++) {
		int suit_points = count_nonspade_tricks(suit);
		ggz_debug(DBG_BID, "Counting %d points in %s.", suit_points,
			  get_suit_name(suit));
		points += suit_points;
	}

	/* Figure out how risky it would be to bid nil. */
	if (consider_bidding_nil(points)) {
		bid.sbid.spec = SPADES_NIL;
		ggz_debug(DBG_BID, "Bidding nil");
		return bid;
	}

	bid.sbid.val = find_final_bid(points);
	ggz_debug(DBG_BID, "Final bid: %d", bid.sbid.val);
	return bid;
}


card_t get_play(int play_seat, bool *valid_plays)
{
	int i, chosen = -1;
	int myNeed, oppNeed, totTricks;
	int agg, lastTrick;
	card_t lead, hi_card;
	hand_t *hand = &ggzcards.players[ME].hand;
	
	ailib_our_play(play_seat);

	assert(play_seat == 0);

	high = -1;
	lead.suit = 0;
	hi_card.suit = 0;
	hi_card.face = 0;

	/* Determine the suit which was led and the highest card played so
	   far. */
	/* FIXME: this should call an external game function? */
	if (get_leader() != ME) {
		lead = ggzcards.players[get_leader()].table_card;
		high = get_leader();
		hi_card = ggzcards.players[get_leader()].table_card;
		for (i = get_leader() + 1; i <= R_OPP; i++) {
			card_t p_card = ggzcards.players[i].table_card;
			if (((p_card.suit == SPADES)
			     && (hi_card.suit != SPADES))
			    || ((p_card.suit == hi_card.suit)
				&& (p_card.face > hi_card.face))) {
				high = i;
				hi_card = p_card;
			}
		}
	}

	if (get_leader() == ME)
		ggz_debug(DBG_PLAY, "My lead.");
	else
		ggz_debug(DBG_PLAY, "%s led. %s of %s is high ",
			  get_suit_name(lead.suit),
			  get_face_name(hi_card.face),
			  get_suit_name(hi_card.suit));


	/* Determine our "aggression" level.  We're more aggressive if we're
	   trying to take tricks or set our opponents.  We're less aggressive
	   if we're trying to bag'em.  Agressiveness is on a scale from 0
	   (least aggressive) to 100 (most aggressive). */

	/* First determine how many more tricks we need. */
	totTricks = ggzcards.players[ME].hand.hand_size;
	myNeed = bids[ME].sbid.val + bids[PARTNER].sbid.val;
	if (nil_tricks_count || bids[ME].sbid.spec == 0)
		myNeed -= get_tricks(ME);
	if (nil_tricks_count || bids[PARTNER].sbid.spec == 0)
		myNeed -= get_tricks(PARTNER);
	if (myNeed < 0)
		myNeed = 0;

	/* Now determine how many more tricks the opponent needs. */
	oppNeed = bids[L_OPP].sbid.val + bids[R_OPP].sbid.val;
	if (nil_tricks_count || bids[L_OPP].sbid.spec == 0)
		oppNeed -= get_tricks(L_OPP);
	if (nil_tricks_count || bids[R_OPP].sbid.spec == 0)
		oppNeed -= get_tricks(R_OPP);
	if (oppNeed < 0)
		oppNeed = 0;

	ggz_debug(DBG_PLAY, "We need %d and they need %d out of %d.", myNeed,
		  oppNeed, totTricks);

	if ((myNeed == 0 || totTricks < myNeed)
	    && (oppNeed == 0 || totTricks < oppNeed))
		/* everythings already been determined, so just throw bags. */
		agg = 0;
	else if (totTricks - myNeed - oppNeed >= 4)
		/* lotsa bags */
		agg = (myNeed > 0) ? 25 : 0;
	else if (totTricks - myNeed - oppNeed >= 3)
		/* some bags */
		agg = (myNeed > 0) ? 50 : 0;
	else if (totTricks - myNeed - oppNeed >= 2)
		/* just be cautious */
		agg = 75;
	else
		/* need tricks */
		agg = 100;

	ggz_debug(DBG_PLAY, "Aggression set to %d", agg);

#if 0				/* not implemented yet */

	/* The lastTrick indicator is a special case when this is the last
	   trick we need to complete our bid, yet there are too many bags out
	   there to warrant taking the trick with the lowest possible card. */
	if (S.prefExpert && myNeed == 1
	    && (agg < 50 || oppNeed == 0 || (oppNeed <= 3 && agg <= 50)))
		lastTrick = 1;
	else if (S.prefExpert && myNeed == 0 && oppNeed == totTricks)
		lastTrick = 1;
	else
#endif
		lastTrick = 0;


	/* Populate the play structure with our valid plays. */
	plays = 0;
	for (i = 0; i < hand->hand_size; i++) {
		if (valid_plays[i]) {
			play[plays].card = hand->cards[i];
			Calculate(&play[plays], agg);
			ggz_debug(DBG_PLAY,
				  "The %s of %s is calculated as %d / %d.",
				  get_face_name(play[plays].card.face),
				  get_suit_name(play[plays].card.suit),
				  play[plays].trick, play[plays].future);
			plays++;
		}
	}

#ifdef AI_DEBUG
	for (i = 0; i < plays; i++)
		ggz_debug(DBG_PLAY, " %s of %s,%d,%d",
			  get_face_name(play[i].card.face),
			  get_suit_name(play[i].card.suit), play[i].trick,
			  play[i].future);
#endif

	/* Now determine our disposition for this trick. */
	if (IS_GOING_NIL(ME) && get_tricks(ME) == 0)
		chosen = PlayNil();

	if (chosen < 0
	    && IS_GOING_NIL(PARTNER)
	    && get_tricks(PARTNER) == 0)
		chosen = CoverNil(agg);

	if (chosen < 0
	    && ((IS_GOING_NIL(L_OPP) && get_tricks(L_OPP) == 0)
	        || (IS_GOING_NIL(R_OPP) && get_tricks(R_OPP) == 0)))
		chosen = SetNil(agg);

	if (chosen < 0)
		chosen = PlayNormal(agg, lastTrick);

	ggz_debug(DBG_PLAY, "Chosen play is %d", chosen);
	return play[chosen].card;
}


/* This function has a bug: it does not count the chance that our partner
   will win a trick if we don't. */
static void Calculate(struct play *play, int agg)
{
	int mask, map, count, danger, trump, n, cover, sCount, o;
	card_t high_card = ggzcards.players[high].table_card;
	char suit_lead = ggzcards.players[get_leader()].table_card.suit;
	char s, r;
	hand_t *hand = &ggzcards.players[ME].hand;

	/**
	 * The scale for all likelihoods runs as follows:
	 *
	 * -x      May lose future tricks
	 * 0       Guaranteed not to take trick
	 * 0..13   Unlikely to take trick (based on rank)
	 * 50..63  Likely to take trick (based on rank)
	 * 100     Guaranteed to take trick
	 * 101+    May earn bonus future tricks as well
	 */


	/* Calculate bitmap of ranks which beat this card */
	mask = 0;
	for (r = play->card.face + 1; r <= ACE_HIGH; r++)
		mask |= (1 << r);

	/* Card's rank is its likelihood of taking this trick if the card is
	   higher than the highest current card. */
	/* NOTE: we also calculate our partner's chance of winning with an
	   already-played card, so we must consider that case as well. */
	if (high < 0
	    || ((play->card.suit == SPADES && high_card.suit != SPADES)
		|| (play->card.suit == high_card.suit
		    && play->card.face >= high_card.face))
		) {
		count = sCount = 0;
		if (!are_cards_equal
		    (ggzcards.players[L_OPP].table_card, UNKNOWN_CARD)) {
			/* we're the last card */
			/* FIXME: above calculation is unnecessary */
			play->trick = 100;
		} else {
			if (suit_lead < 0) {	/* we're leading */
				s = play->card.suit;
				trump = 0;
			} else {
				s = suit_lead;
				/* did we trump? */
				trump = (s != SPADES
					 && play->card.suit == SPADES);
			}
			for (o = 1;
			     o < 4
			     && are_cards_equal(ggzcards.players[o].
						table_card, UNKNOWN_CARD);
			     o++) {
				if (o == PARTNER && bids[ME].sbid.val > 0)
					continue;
				map = SuitMap(o, s);
				if (map == 0 && s != SPADES) {	/* void in
								   the lead
								   suit */
					if ((map = SuitMap(o, SPADES)) != 0) {
						trump = 0;	/* our trump
								   isn't
								   guaranteed
								   anymore */
						if (play->card.suit != SPADES
						    || (map & mask))
							count++;
					}
				} else if (s == play->card.suit
					   && (map & mask)) {
					count++;
					sCount++;
				}
			}
			if (trump)
				play->trick = 50;
			else if (count) {
				play->trick = play->card.face;
				if (sCount == 0 && agg <= 25)
					play->trick += 15;	/* opponents
								   may not
								   trump */
			} else
				play->trick = play->card.face + 60;
		}
	} else
		play->trick = -1;


#if 0				/* we don't do this */

	/* Special case for prefMustBeat -- if the highest is out there then
	   this card may not be good. */
	if (S.prefMustBeat) {
		s = play->card.suit;
		for (r = play->card._value + 1;
		     r <= ACE_HIGH && play->trick >= 0; r++) {
			count = 0;
			for (o = (p + 3) % 4; o != p && S.p[o].play >= 0;
			     o = (o + 3) % 4)
				if (SuitMap(o, s) & (1 << r)) {
					count++;	/* a player who's
							   played may have
							   this higher card */
					break;
				}
			if (count == 0) {
				for (o = (p + 1) % 4;
				     o != p && S.p[o].play < 0;
				     o = (o + 1) % 4)
					if (SuitMap(o, s) & (1 << r)) {
						play->trick = -1;	/* they
									   gotta
									   beat
									   it */
						break;
					}
			}
		}
	}
#endif

	/* Figure out how many players probably still have cards in this
	   suit, and determine likelihood of making card good in future
	   tricks. */
	s = play->card.suit;
	count = danger = trump = cover = 0;
	for (o = L_OPP; o <= R_OPP; o++) {
		if ((map = SuitMap(o, s)) != 0)
			count++;
		if (o == PARTNER && bids[ME].sbid.val > 0)
			continue;
		if (map & mask)
			danger++;
		else if (map == 0 && s != SPADES && SuitMap(o, SPADES) != 0)
			trump++;
	}
	if (trump)
		play->future = play->card.face;
	else {
		/* Count the cards in this suit we know about.  Then average
		   the remaining cards per player. */
		n = 13 - libai_cards_played_in_suit(s);
		for (r = 0; r < hand->hand_size; r++) {
			if (are_cards_equal
			    (hand->cards[(int) r], UNKNOWN_CARD))
				continue;
			if (hand->cards[(int) r].suit == s) {
				cover++;	/* XXX - does ace cover king? */
				n--;
			}
		}
		n = (count == 0) ? 0 : ((n * 100) / count);
		if (n <= 100)
			danger++;

		/* XXX this should be redone to use relative rank in
		   remaining cards rather than hard-coded ace, king, queen. */
		play->future = play->card.face + ((s == SPADES) ? 15 : 0);
		if (play->card.face == ACE_HIGH) {
			if (n > 100)
				play->future += 50;
		} else if (play->card.face == KING) {
			if (!danger || (cover >= 1 && n > 200))
				play->future += 50;
			else if (cover >= 1 && n > 100)
				play->future += 30;
		} else if (play->card.face == QUEEN) {
			if (!danger || (cover >= 2 && n > 300))
				play->future += 30;
		}
	}

	/* This is a hack to tweak the AI into figuring out spades leads
	   correctly. */
	if (get_leader() == ME && play->card.suit == SPADES) {
		int opp_with_spades = 0;
		int pard_with_spades = 0;;

		if (libai_get_suit_map(L_OPP, SPADES))
			opp_with_spades++;
		if (libai_get_suit_map(PARTNER, SPADES))
			pard_with_spades++;
		if (libai_get_suit_map(R_OPP, SPADES))
			opp_with_spades++;

		ggz_debug(DBG_PLAY,
			  "Opp_with_spades: %d.  Pard_with_spades: %d.  %d have been played.",
			  opp_with_spades, pard_with_spades,
			  libai_cards_played_in_suit(SPADES));

		if (!opp_with_spades) {
			/* Well, our partner could beat it but... */
			play->future = 100;

			/* Playing this card could _cost_ us a trick. */
			play->trick = -50;
		} else if (partner_lead_spades) {
			ggz_debug(DBG_PLAY,
				  "Partner lead spades.  We should respond.");
			play->trick += 100;
		} else {
			/* Round up. */
			int avg_spades_out =
				(13 - libai_cards_played_in_suit(SPADES) -
				 libai_count_suit(ME,
						  SPADES) -
				 1) / (pard_with_spades + opp_with_spades) +
				1;
			int our_spades = libai_count_suit(ME, SPADES);
			int diff;

			ggz_debug(DBG_PLAY,
				  "We have %d spades.  The average is %d.",
				  libai_count_suit(ME, SPADES),
				  avg_spades_out);

			diff = 40 * (our_spades - avg_spades_out);

			ggz_debug(DBG_PLAY,
				  "Changing value of spade by %d for length.",
				  diff);

			play->trick += diff;
		}
	}


}


/* Determine what cards some player "p" may be holding in suit "s" from the
   viewpoint of player "v". */
/* FIXME: this should be a ai-common function. */
static int SuitMap(int p, char suit)
{
	int map = 0, i;

	hand_t *hand = &ggzcards.players[p].hand;

	for (i = 0; i < hand->hand_size; i++)
		if (hand->cards[i].suit == suit)
			map |= 1 << hand->cards[i].face;

	return map;
}


static int PlayNil(void)
{
	int i, chosen = -1;

	ggz_debug(DBG_PLAY, "Strategy: play nil");

	/* For nil bids, pick the card with highest potential that doesn't
	   take the trick. */
	for (i = 0; i < plays; i++) {
		if (chosen < 0 || play[i].trick < play[chosen].trick)
			chosen = i;
		else if (play[i].trick == play[chosen].trick) {
			if (play[i].future > play[chosen].future)
				chosen = i;
		}
	}
	return chosen;
}


static int CoverNil(int agg)
{
	int i, chosen = -1, mask, r, danger = 0, sluff = 0;
	int map[4];
	char suit_lead = ggzcards.players[get_leader()].table_card.suit;
	card_t pard_card = ggzcards.players[PARTNER].table_card;
	card_t high_card = ggzcards.players[high].table_card;

	ggz_debug(DBG_PLAY, "Strategy: cover nil");

	/* Construct pard's suitmaps. */
	for (i = 0; i < 4; i++)
		map[i] = SuitMap(PARTNER, i);

	/* If our pard has the highest card, try to beat it. */
	if (PARTNER == high) {
		for (i = 0; i < plays; i++)
			if (card_comp(play[i].card, pard_card) > 0)
				if (chosen < 0
				    || card_comp(play[i].card,
						 play[chosen].card) < 0)
					chosen = i;
	}

	/* Determine if there's any danger of our pard taking this trick. */
	if (high >= 0 && pard_card.suit == -1 && high_card.suit == suit_lead) {
		for (r = high_card.face + 1; r <= ACE_HIGH; r++)
			if (map[(int) suit_lead] & (1 << r))
				danger++;
	}

	/* If there's no danger or we can't cover him, sluff intelligently. */
	if (chosen < 0 && danger == 0 && high >= 0) {
		if (agg <= 50)
			sluff = 1;
		/* else we'll drop through and play normally */
	} else if (chosen < 0) {
		/* If we're here, then our pard hasn't played yet and there's
		   some danger of his taking the trick.  First things first --
		   always try to lead to a void. */
		if (high < 0) {
			for (i = 0; i < plays; i++)
				if (map[(int) play[i].card.suit] == 0) {
					if (chosen < 0)
						chosen = i;
					else if (agg < 30
						 && play[i].trick <
						 play[chosen].trick)
						chosen = i;
					else if (agg >= 30
						 && play[i].future <=
						 play[i].trick) {
						if (play[chosen].future >
						    play[chosen].trick)
							chosen = i;
						else if (play[i].trick >
							 play[chosen].trick)
							chosen = i;
						else if (play[i].trick ==
							 play[chosen].trick) {
							if (play[i].future <
							    play[chosen].
							    future)
								chosen = i;
						}
					}
				}
		}

		/* Try to pick something he can't beat. */
		if (chosen < 0) {
			for (i = 0; i < plays; i++) {
				char s = play[i].card.suit;
				if (high >= 0
				    && card_comp(play[i].card,
						 high_card) <= 0)
					continue;	/* don't consider
							   cards which don't
							   beat high card */

				if (high < 0 || s == suit_lead) {
					for (mask = 0, r =
					     play[i].card.face + 1;
					     r < ACE_HIGH; r++)
						mask |= (1 << r);	/* bitmap
									   of
									   ranks
									   which
									   beat
									   this
									   card */
					if (map[(int) s] & mask)
						continue;
				}
				if (chosen < 0)
					chosen = i;
				else if (agg < 30) {
					if (s == suit_lead) {
						if (play[i].trick <
						    play[chosen].trick)
							chosen = i;
					} else {
						/* If we're here then we're
						   trumping.  Normally we'd
						   throw our highest trump at
						   this low aggression level,
						   but we need to cover our
						   pard's trumps later. */
						/* XXX maybe throw a higher
						   trump if pard is covered? */
						if (play[i].trick <
						    play[chosen].trick)
							chosen = i;	/* for
									   now,
									   choose
									   lowest
									   trump */
						else if (play[i].trick ==
							 play[chosen].trick)
							if (play[i].future <
							    play[chosen].
							    future)
								chosen = i;
					}
				} else if (agg >= 30) {
					if (s != suit_lead) {
						if (play[i].trick <
						    play[chosen].trick)
							chosen = i;	/* both
									   must
									   be
									   trumps
									   --
									   choose
									   lower */
						else if (play[i].trick ==
							 play[chosen].trick)
							if (play[i].future <
							    play[chosen].
							    future)
								chosen = i;
					} else if (play[i].future <=
						   play[i].trick) {
						if (play[chosen].future >
						    play[chosen].trick)
							chosen = i;
						else if (play[i].trick >
							 play[chosen].trick)
							chosen = i;
						else if (play[i].trick ==
							 play[chosen].trick) {
							if (play[i].future <
							    play[chosen].
							    future)
								chosen = i;
						}
					}
				}
			}
		}

		/* If we still haven't chosen one, then we're in a load of
		   trouble. Play our highest card. */
		if (chosen < 0 && sluff == 0) {
			sluff = 1;
			for (i = 0; i < plays; i++)
				if (suit_lead < 0
				    || play[i].card.suit == suit_lead)
					if (chosen < 0
					    || play[i].card.face >
					    play[chosen].card.face)
						chosen = i;
		}

		/* If this card is essentially one card greater than the
		   current high, then just sluff. */
		if (chosen >= 0 && high >= 0
		    && play[chosen].card.suit == high_card.suit) {
			for (r = high_card.face + 1;
			     libai_is_card_played(high_card.suit, r); r++)
				continue;
			if (r == play[chosen].card.face) {
				sluff = 1;
				chosen = -1;
			}
		}
	}

	if (chosen < 0 && sluff) {
		/* XXX sluff something intelligently */
		for (i = 0; i < plays; i++)
			if (chosen < 0
			    || play[i].future < play[chosen].future)
				chosen = i;
	}

	/* XXX can you upgrade the chosen card to one with a better chance
	   because you know that your pard doesn't have anything between this
	   card and the higher one? */

	return chosen;
}


static int SetNil(int agg)
{
	int i, pp, chosen = -1, r, mask, map, count;
	card_t high_card =
		(high ==
		 -1) ? UNKNOWN_CARD : ggzcards.players[high].table_card;
	char suit_lead = ggzcards.players[get_leader()].table_card.suit, s;
	
	if (agg >= 100)
		return -1;

	ggz_debug(DBG_PLAY, "Strategy: set nil");

	/* If one of our opponents bid nil and either hasn't played or has
	   played the high card, try to get under it. */
	for (pp = L_OPP; pp <= R_OPP && chosen < 0; pp++) {
		if (pp == PARTNER || !IS_GOING_NIL(pp)
		    || get_tricks(pp) != 0)
			continue;
		if (high < 0 && agg <= 50) {
			/*
			 * If we're leading, lead something small.
			 */
			ggz_debug(DBG_PLAY, "Set nil: lead small.");
			for (i = 0; i < plays; i++) {
				for (mask = 0, r = play[i].card.face + 1;
				     r <= ACE_HIGH; r++)
					mask |= (1 << r);	/* bitmap of
								   ranks
								   which beat
								   this card */
				if ((SuitMap(pp, play[i].card.suit) & mask)
				    == 0)
					continue;	/* he can't beat this
							   one -- skip it */
				if (chosen < 0
				    || play[i].card.face <
				    play[chosen].card.face)
					chosen = i;
			}
		} else if (high == pp) {
			/*
			 * Try to get under the high card.
			 */
			ggz_debug(DBG_PLAY, "Set nil: duck the nil.");
			for (i = 0; i < plays; i++)
				if (card_comp(play[i].card, high_card) <= 0
				    && play[i].future < 40
				    && (chosen < 0
					|| (play[i].future >
					    play[chosen].future)))
					chosen = i;
		} else if (high >= 0
			   && ggzcards.players[pp].table_card.suit < 0) {
			ggz_debug(DBG_PLAY, "Set nil: follow.");
			
			/*
			 * Count how many cards we have under the high.
			 */
			count = 0;
			for (i = 0; i < plays; i++)
				if (card_comp(play[i].card, high_card) <= 0)
					count++;
			ggz_debug(DBG_PLAY, "We have %d losers.", count);

			/*
			 * Try to get just over the high card under certain conditions:
			 * 1. our aggression is high and our pard's not the high card
			 * 2. we don't have at least two cards under the high card
			 * 3. the high card is a face card
			 * 4. the nil bidder can't beat the high card
			 */
			s = high_card.suit;
			map = SuitMap(pp, suit_lead);
			mask = 0;
			for (r = high_card.face + 1; r <= ACE_HIGH; r++)
				mask |= (1 << r);	/* bitmap of ranks
							   which beat high
							   card */

			if ((agg > 50 && high != PARTNER)
			    || (count < 2)
			    || (r >= JACK && high != PARTNER)
			    || (s != suit_lead || (map & mask) == 0)) {
				ggz_debug(DBG_PLAY, "Set nil: cover.");
				
				if (s != suit_lead) {	/* high player
							   trumped */
					if (map == 0)
						map = SuitMap(pp, 0);	/* ??
									   --
									   JDS
									 */
					else
						map = 0;	/* play our
								   highest
								   trump */
				}
				for (r = high_card.face + 1; r <= ACE_HIGH; r++) {
					if (!libai_is_card_played(s, r)) {
						for (i = 0; i < plays; i++)
							if (play[i].card.
							    suit == s
							    && play[i].card.
							    face == r) {
								chosen = i;
								break;
							}
					}
					if (map & (1 << r))
						break;
				}
				/*
				 * If we can't get above the high card in suit, maybe we can
				 * trump it.
				 */
				if (chosen < 0 && s == suit_lead
				    && (map & mask) == SPADES) {
					for (i = 0; i < plays; i++)
						if (play[i].card.suit ==
						    SPADES)
							if (map != 0
							    || play[i].card.
							    face <= 6)
								if (chosen < 0
								    ||
								    play[i].
								    card.
								    face >
								    play
								    [chosen].
								    card.face)
									chosen = i;
				}
			}
			/*
			 * Otherwise, try to get under the high card.
			 */
			for (i = 0; i < plays; i++)
				if (card_comp(play[i].card, high_card) <= 0
				    && play[i].future < 40
				    && (chosen < 0
					|| (play[i].future >
					    play[chosen].future)))
					chosen = i;
		} else {
			ggz_debug(DBG_PLAY, "Set nil: nothing...");
		}
	}

	return chosen;
}


static int PlayNormal(int agg, int lastTrick)
{

	int i, chosen = -1, n, r, s /* , tmp */ ;
	struct play pCard;	/* partner's card */
	/* int pmap[4], omap[4]; */

	ggz_debug(DBG_PLAY, "Strategy: play normal");

	/*
	 * If our pard has played, calculate the chances that his card is a winner.
	 */
	if (high == PARTNER) {
		pCard.card = ggzcards.players[PARTNER].table_card;
		Calculate(&pCard, agg);
		ggz_debug(DBG_PLAY,
			  "Pard is winning with %d of %s; chance of winning trick is %d.",
			  pCard.card.face, get_suit_name(pCard.card.suit),
			  pCard.trick);
	} else
		pCard.trick = pCard.future = -1;

	/* For normal play, our aggression level determines the desire we
	   have to take this trick. */
	if (lastTrick && pCard.trick >= 100)
		return PlayNil();
	if (agg == 0) {
		if (PARTNER == high && pCard.trick > 30) {
			/* Our pard's winning the trick -- beat him if we
			   can. */
		} else
			return PlayNil();
	}

	/* If our aggression is zero then we're here because our pard is
	   pro'ly gonna win this trick -- win it for him instead with our
	   highest and bestest card. */
	/* For any other aggression, find the card that can win the trick but
	   * has the lowest future value. */
	for (i = 0; i < plays; i++) {
		if (agg > 0 && (play[i].future > play[i].trick ||
				(play[i].future == play[i].trick
				 && (play[i].trick < 30 || !lastTrick))))
			continue;
		if (chosen < 0 || play[i].trick > play[chosen].trick)
			chosen = i;
		else if (play[i].trick == play[chosen].trick) {
			if (agg == 0 || (lastTrick && play[i].trick >= 50)) {
				if (play[i].future > play[chosen].future)
					chosen = i;
			} else if (play[i].future < play[chosen].future)
				chosen = i;
		}
	}

	/* 
	 * Watch the finesse.  Beat an opponent's best card if we can do so.
	 */
	if (chosen < 0 && high >= 0 && high != 2) {
		for (i = 0; i < plays; i++) {
			if ((play[i].card.suit ==
			     ggzcards.players[high].table_card.suit)
			    && play[i].card.face >
			    ggzcards.players[high].table_card.face
			    && (chosen < 0
				|| play[i].card.face <
				play[chosen].card.face))
				chosen = i;
		}
	}

	/* 
	 * Don't play over our pard's good lead under normal circumstances.
	 */
	if (agg > 25 && !lastTrick && chosen >= 0 && pCard.trick >= 50)
		chosen = -1;

#if 0
	/* 
	 * Randomly elect not to take this trick.  If we can't afford to lose
	 * one then our aggression would be higher.
	 */
	if (agg > 0 && agg < 75 && chosen >= 0 && (pard < 0 || high != pard)
	    && (random() % 100) > agg)
		chosen = -1;

	/* 
	 * Consider leading to our pard's void.
	 */
	if (high < 0 && SuitMap(pard, p, 0) != 0
	    && (chosen < 0 || (random() % 2) == 0)) {
		int tmp = -1;
		for (s = 0; s < 4; s++) {
			pmap[s] = SuitMap(pard, p, s);
			omap[s] = SuitMap((p + 3) % 4, p, s);
		}
		for (i = 0; i < plays; i++) {
			s = play[i].card.suit;
			if (pmap[s] == 0 && omap[s] != 0)
				if (tmp < 0
				    || play[i].card.face <
				    play[tmp].card.face)
					tmp = i;
		}
		if (tmp >= 0)
			chosen = tmp;
	}
#endif



	/* 
	 * If our aggression is pretty high and we still haven't chosen a
	 * card, then consider playing a precious trump.
	 */
	if (chosen < 0 && agg >= 75 && pCard.trick < 50) {
		for (i = 0; i < plays; i++)
			if (play[i].trick >= 50)
				if (chosen < 0
				    || play[i].card.face <
				    play[chosen].card.face)
					chosen = i;
	}

	/* 
	 * If we haven't found a card yet, then nothing we have can take this
	 * trick.  For a zero aggression, sluff our card which has the best
	 * future.  For any other aggression, sluff our lowest card.
	 */
	if (chosen < 0) {
		for (i = 0; i < plays; i++)
			if (chosen < 0)
				chosen = i;
			else if (agg == 0) {
				if (play[i].future > play[chosen].future)
					chosen = i;
			} else if (play[i].future < play[chosen].future)
				chosen = i;
	}

	/* 
	 * Now, for mid-level aggressions we may opt to play a higher
	 * card if we're pro'ly gonna lose this trick anyway.  This gets rid
	 * of those pesky middle cards which are bag-machines later in the hand.
	 */
	if (agg > 0 && agg < 75) {
		s = play[chosen].card.suit;
		for (;;) {
			/* 
			 * Find the next highest card in suit.
			 */
			n = -1;
			for (i = 0; i < plays; i++)
				if (play[i].card.suit == s
				    && play[i].card.face >
				    play[chosen].card.face && (n < 0
							       || play[i].
							       card.face <
							       play[n].card.
							       face))
					n = i;
			if (n < 0)
				break;

			/* 
			 * Determine the highest rank equivalence of the chosen card,
			 * based on which cards have been played.
			 */
			r = play[chosen].card.face + 1;
			while (r <= ACE_HIGH && libai_is_card_played(s, r)
			       && (high < 0
				   || !(ggzcards.players[high].table_card.
					suit == s
					&& ggzcards.players[high].table_card.
					face == r)))
				r++;
			if (r != play[n].card.face	/* cards are
							   equivalent */
			    && ((play[n].future >= 50)	/* card has a good
							   future */
				||(play[n].future >= 20 && agg >= 50)))	/* card 
									   has 
									   a
									   decent 
									   future 
									 */
				break;
			chosen = n;
		}
	}

	return chosen;
}

/* return -1, 0, 1 if c1 is less than, equal to, or greater than c2 */
static int card_comp(card_t c1, card_t c2)
{
	if (c1.suit == c2.suit)
		return c1.face - c2.face;
	if (c1.suit == SPADES)
		return 1;
	if (c2.suit == SPADES)
		return -1;
	return 0;
}
