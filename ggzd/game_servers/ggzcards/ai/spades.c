/* 
 * File: ai/spades.c
 * Author: Brent Hendricks
 * Project: GGZCards Server
 * Date: 8/4/99
 * Desc: NetSpades algorithms for Spades AI
 * $Id: spades.c 2440 2001-09-10 08:19:13Z jdorje $
 *
 * This file contains the AI functions for playing spades.
 * The AI routines were adapted from Britt Yenne's spades game for
 * the palm pilot.  Thanks Britt!
 *
 * Later, they were stolen from NetSpades and integrated directly into
 * GGZCards for use with the spades game here.  Thanks Brent!
 *
 * Unfortunately, many of the routines follow pretty bad spades
 * strategies.  I'm in the process of implementing better ones.
 *
 * There's a big bug in here that causes the AI to be really stupid.
 * Oddly, it works pretty well for Hearts.
 *
 * Copyright (C) 1998 Brent Hendricks.
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
#include "../games/spades.h"

#include "aicommon.h"

/* #define USE_AI_TRICKS */


static char *get_name(player_t p);
static void start_hand();
static void alert_bid(player_t p, bid_t bid);
static void alert_play(player_t p, card_t play);
static bid_t get_bid(player_t num, bid_t * bid_choices, int bid_count);
static card_t get_play(player_t p, seat_t s);

struct ai_function_pointers spades_ai_funcs = {
	get_name,
	start_hand,
	alert_bid,
	alert_play,
	get_bid,
	get_play
};

static char *get_name(player_t p)
{
	char *name = "[invalid]";
	switch (p) {
	case 0:
		name = "Yenne-AI";
		break;
	case 1:
		name = "Crouton-AI";
		break;
	case 2:
		name = "Britt-AI";
		break;
	case 3:
		name = "Brent-AI";
		break;
	}
	return strdup(name);
}




static struct play {
	card_t card;		/* card's value */
	int trick;		/* likelyhood card will take this trick */
	int future;		/* likelyhood card will take future trick */
} play[13];
static int plays, high;

static void Calculate(player_t num, struct play *play, int agg);
static int SuitMap(seat_t, player_t, char);
static int PlayNil(player_t p);
static int CoverNil(player_t p, int agg);
static int SetNil(player_t p, int agg);
static int PlayNormal(player_t p, int agg, int lastTrick);
static int card_comp(card_t c1, card_t c2);

static void start_hand()
{
	/* nothing... */
}

static void alert_bid(player_t p, bid_t bid)
{
	/* nothing... */
}

static void alert_play(player_t p, card_t play)
{
#ifdef USE_AI_TRICKS
	card_t lead = game.seats[game.players[game.leader].seat].table;
	card_t card;
#endif

#ifdef USE_AI_TRICKS
	/* when a nil player sluffs, they'll sluff their highest card in that
	   suit */
	card = play;
	if (game.players[p].bid.sbid.spec == SPADES_NIL
	    && game.players[p].tricks == 0 && play.suit != lead.suit
	    && play.suit != SPADES)
		for (card.face++; card.face <= ACE_HIGH; card.face++)
			libai_player_doesnt_have_card(p, card);
#endif

#ifdef USE_AI_TRICKS
	/* a nil player will generally play their highest card beneath an
	   already-played card */
	if (game.players[p].bid.sbid.spec == SPADES_NIL &&
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
			for (card = play, card.face++; card.face < -1	/* THE 
									   HIGHEST 
									   CARD 
									   PLAYED 
									   SO 
									   FAR 
									   IN 
									   THIS 
									   SUIT 
									 */ ; card.face++)
				libai_player_doesnt_have_card(p, card);
		}
	}
#endif
}


/* This counts the possible winners you'll get from trumping in.  It should
   only be accurate when you have less than 3 trumps. */
static int count_spade_ruff_winners(player_t num, int suitCount[4],
				    int suitAvg[4])
{
	int p1 = 0, p2 = 0, gap, count;
	int voids = 0, singletons = 0, doubletons = 0;
	card_t card, c;
	char s;

	for (s = 0; s < 4; s++) {
		if (s == SPADES)
			continue;
		switch (suitCount[(int) s]) {
		case 0:
			voids++;
			break;
		case 1:
			singletons++;
			break;
		case 2:
			doubletons++;
			break;
		}
	}

	/* --------------- TRUMPS ----------------- */
	/* Trumps serve two purposes -- first, any trump sequence down from
	   some high trump may be automatic tricks.  The remaining trumps may
	   either be useful as high trumps or for trumping other tricks if we
	   have some voids, singletons, or doubletons. */

	/* First, figure out the brute force points possible from trumps. */
	for (card.suit = SPADES, card.face = ACE_HIGH, card.deck = 0, gap = 0;
	     card.face >= 0; card.face--) {
		if (libai_is_card_in_hand(num, card)) {
			if (gap > 0)
				gap--;
			else {
				p1 += 100;
				ai_debug("Counting guaranteed spade as 100");
			}
		} else
			gap++;
	}

	/* That done, frob p1 to count probable trump tricks. */
	if (p1 == 0) {
		/* King with 1+ covers */
		c.suit = SPADES;
		c.face = KING;
		if (libai_is_card_in_hand(num, c) && suitCount[SPADES] >= 2) {
			p1 += ((suitCount[SPADES] >= 3) ? 100 : 60);
			ai_debug("Counting King of Spades as %d",
				 (suitCount[SPADES] >= 3) ? 100 : 60);
		}
		c.face = QUEEN;
		/* Queen with 2+ covers */
		if (libai_is_card_in_hand(num, c) && suitCount[SPADES] >= 3) {
			p1 += ((suitCount[SPADES] >= 4) ? 100 : 30);
			ai_debug("Counting Queen of Spades as %d",
				 (suitCount[SPADES] >= 4) ? 100 : 30);
		}
	}


	/* Now figure if we use our trumps on other suits: */
	count = suitCount[SPADES];
	for (c.face = ACE_HIGH; c.face >= 2 && libai_is_card_in_hand(num, c);
	     c.face--) {
		p2 += 100;
		count--;
	}
	for (; voids > 0 && count > 0; voids--) {
		if (count >= 3) {
			p2 += 225;
			count -= 2;
			ai_debug("Counting shortsuit as 225");
		} else if (count >= 2) {
			p2 += 190;
			count -= 2;
			ai_debug("Counting shortsuit as 190");
		} else {
			p2 += 90;
			count -= 1;
			ai_debug("Counting shortsuit as 90");
		}
	}
	for (; singletons > 0 && count > 0; singletons--) {
		if (count >= 2) {
			p2 += 175;
			count -= 2;
			ai_debug("Counting singleton as 175");
		} else {
			p2 += 90;
			count -= 1;
			ai_debug("Counting singleton as 90");
		}
	}
	for (; doubletons > 0 && count > 0; doubletons--) {
		p2 += 75;
		count -= 1;
		ai_debug("Counting doubleton as 75");
	}

	if (count > suitAvg[SPADES]) {
		/* Length DOES matter.  :-) */
		p2 += (count - suitAvg[SPADES] - 1) * 80;
		ai_debug("Counting extra spades as %d",
			 (count - suitAvg[SPADES] - 1) * 80);
	} else if (count == 0) {
		p2 -= 30;	/* be wary of exhausting all trump */
		ai_debug("Removing 30 points for exhausting spades(?)");
	}


	/* Okay, choose the better of the two scores. */
	return (p1 > p2) ? p1 : p2;
}

/* This method of calculating trump winners ONLY counts length and strength
   in spades.  It is most accurate when you have 4 or more trumps */
int count_spade_strength_winners(player_t p)
{
	int count = libai_count_suit(p, SPADES);
	int points = 0;
	card_t ace = { ACE_HIGH, SPADES, 0 };
	card_t king = { KING, SPADES, 0 };
	card_t queen = { QUEEN, SPADES, 0 };

	ai_debug("Counted %d spades for player %d.", count, p);

	/* The ace/king/queen each get counted as one trick. */
	/* Note that if we have fewer than 3 trumps some points will be
	   subtracted later. */
	if (libai_is_card_in_hand(p, ace)) {
		ai_debug("Counted ACE as 1 trick.");
		points += 100;
	}
	if (libai_is_card_in_hand(p, king)) {
		ai_debug("Counted KING as 1 trick.");
		points += 100;
	}
	if (libai_is_card_in_hand(p, queen)) {
		ai_debug("Counted QUEEN as 1 trick.");
		points += 100;
	}

	/* The fourth spade gets counted as a 50% trick. */
	if (count >= 4) {
		ai_debug("Counted 4th spade as 1/2 trick.");
		points += 50;
	}

	/* The 5th and every spade thereafter gets counted 1 trick. */
	if (count >= 5) {
		ai_debug("Counted extra %d spades as %d tricks.", count - 4,
			 count - 4);
		points += 100 * (count - 4);
	}

	/* If we have more than 6 spades, you've got to figure some of our
	   earlier ones will be winners. */
	if (count >= 6) {
		ai_debug("Counted 6th spade as 1/2 bonus trick.");
		points += 50;
	}

	return points;
}


static int count_spade_tricks(player_t num, int suitCount[4], int suitAvg[4])
{
	int p1 = count_spade_ruff_winners(num, suitCount, suitAvg);
	int p2 = count_spade_strength_winners(num);
	int points = (p1 > p2) ? p1 : p2;

	ai_debug("Counted ruff winners as %d and strength winners as %d.", p1,
		 p2);

	/* A special case: take off tricks for weak spades. Although it might 
	   not make sense to have negative tricks in spades, having this
	   weakness will cause your partner to lose tricks. */
	switch (libai_count_suit(num, SPADES)) {
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

	ai_debug("Spade points: %d", points);
	return points;
}

static bid_t get_bid(player_t num, bid_t * bid_choices, int bid_count)
{
	int count, points, prob;
	bid_t bid, pard;
	int suitCount[4], suitAvg[4];
	int nilrisk = 99;
	card_t c = { 0, 0, 0 };
	char s;			/* suit */

	bid.bid = 0;

	/* Partners's bid */
	pard = game.players[(num + 2) % 4].bid;

	/* First count our cards in each suit, and determine the average
	   cards the other players each have in each suit.  Also count our
	   voids, singletons, and doubletons. */
	for (s = 0; s < 4; s++) {
		suitCount[(int) s] = libai_count_suit(num, s);
		suitAvg[(int) s] =
			(game.hand_size -
			 suitCount[(int) s]) / (game.num_players - 1);
	}


	points = count_spade_tricks(num, suitCount, suitAvg);

	/* If our trumps are losable (ie. p1 == 0) and we don't have many of
	   them, then consider whether we have low enough cards in each of
	   the other suits to risk bidding nil. */
	if (points <= 50) {
		nilrisk = 0;
		if (suitCount[SPADES] > 3) {
			nilrisk += suitCount[SPADES] - 2;
			ai_debug("Inc. nilrisk for %d excess spades",
				 suitCount[SPADES] - 2);
		}
		for (s = 0; s < 4; s++) {
			c.suit = s;

			/* check if we're void -- good! */
			if (suitCount[(int) s] == 0) {
				if (s != SPADES) {
					nilrisk -= 2;
					ai_debug("Dec. nilrisk for shortsuit in %s", suit_names[(int) s]);
				}
				continue;	/* for( s = 0; s < 4; s++ ) */
			}

			/* count our low cards in the suit */
			for (count = 0, c.face = 2; c.face < 9; c.face++) {
				if (libai_is_card_in_hand(num, c))
					count++;	/* count low cards */
			}
			if (count >= 3)	/* suit is covered */
				continue;	/* for( s = 0; s < 4; s++ ) */
			nilrisk += suitCount[(int) s] - count;	/* risky high
								   cards */
			ai_debug("Inc. nilrisk for %d high %s",
				 suitCount[(int) s] - count,
				 suit_names[(int) s]);
			if (count == 0) {	/* no low cards */
				nilrisk++;
				ai_debug("Inc. nilrisk for no low cards");
			}

			/* check for the queen/king of spades */
			if (s == SPADES) {
				c.face = KING;
				if (libai_is_card_in_hand(num, c)) {
					nilrisk += 2;
					ai_debug("Inc. nilrisk for King of spades");
				}
				c.face = QUEEN;
				if (libai_is_card_in_hand(num, c)) {
					nilrisk += 1;
					ai_debug("Inc. nilrisk for Queen of spades");
				}
			} else {
				c.face = KING;
				if (libai_is_card_in_hand(num, c)
				    && suitCount[(int) s] <= 3) {
					nilrisk += ((suitCount[(int) s] <= 2) ? 2 : 1);	/* king
											   is
											   riskier 
											 */
					ai_debug("Inc. nilrisk by %d for King of %s", (suitCount[(int) s] <= 2) ? 2 : 1, suit_names[(int) s]);
				}
				c.face = ACE_HIGH;
				if (libai_is_card_in_hand(num, c)
				    && suitCount[(int) s] <= 3) {
					nilrisk += ((suitCount[(int) s] <= 2) ? 3 : 2);	/* ace
											   is
											   riskiest 
											 */
					ai_debug("Inc. nilrisk by %d for Ace of %s", (suitCount[(int) s] <= 2) ? 3 : 2, suit_names[(int) s]);
				}
			}
		}
	}

	/* --------------- OTHER SUITS ----------------- */
	/* We bid other suits using a confidence system.  We combine two
	   factors: the confidence we can make some high card good, and the
	   probability of that trick not getting trumped/finessed. */
	for (s = 0; s < 3; s++) {
		c.suit = s;
		c.face = ACE_HIGH;
		if ((count = suitAvg[(int) s]) > suitCount[(int) s])
			count = suitCount[(int) s];	/* the number of
							   probable tricks
							   w/o trump */
		prob = 100;	/* probability of not getting finessed */
		for (; count > 0; count--, c.face--) {
			if (libai_is_card_in_hand(num, c)) {
				points += prob;
				ai_debug("Counting %s of %s as %d",
					 face_names[(int) c.face],
					 suit_names[(int) c.suit], prob);
				prob = prob * 9 / 10;
			} else
				prob = prob * 8 / 10;
		}
	}


	/* --------------- NIL BIDS ----------------- */
	/* Consider bidding nil. */
	if (game.players[(num + 1) % 4].bid.sbid.spec == SPADES_NIL ||
	    game.players[(num + 3) % 4].bid.sbid.spec == SPADES_NIL) {
		nilrisk -= 2;
		ai_debug("Dec. nilrisk because opponent is kneeling");
	}

	if ((pard.sbid.val >= GSPADES.minimum_team_bid || game.bid_count < 2)
	    && ((nilrisk <= 0) || (nilrisk <= 1 && points < 250)
		|| (nilrisk <= 2 && points < 150)
		|| (pard.sbid.val >= 3 + nilrisk))) {
		bid.sbid.spec = SPADES_NIL;
		ai_debug("Bidding nil");
	}



	/* --------------- NORMAL BIDS ----------------- */
	/* Okay, divide points by 100 and that's our bid. */
	if (bid.bid == 0) {
		if (game.bid_count < 2) {
			prob = 70;
			ai_debug("Adding 70 because partner hasn't bid");
		} else {
			prob = 30;
			ai_debug("Adding 30 because partner has bidn");
		}

		bid.sbid.val = (points + prob) / 100;
		ai_debug("Subtotal bid: %d", bid);

#if 0
		/* Consider ramifications of others' bids if everyone else
		   has bid. */
		/* not taken into account yet */
		count = bid;
		for (i = 0; i < 4; i++) {
			if (i == p)
				continue;
			if (S.p[i].bid < 0)
				break;	/* hasn't bid yet -- break */
			count += S.p[i].bid;
		}
		if (i >= 4) {	/* everyone has bid */
			/* If the count is real high, reconsider potential
			   tricks. If the count is high and they bid nil,
			   consider bidding less. */
			if (count >= 11) {
				if (count >= 13)
					bid = points / 100;
				else if (p1 == 0 || p2 == 0)
					bid = points / 100;
			}
		}
#endif

		/* don't forget the minimum bid!!! */
		if (game.bid_count >= 2
		    && (bid.sbid.val + pard.sbid.val <
			GSPADES.minimum_team_bid)) {
			bid.sbid.val =
				GSPADES.minimum_team_bid - pard.sbid.val;
			ai_debug("Upping bid to meet minimum of %d",
				 GSPADES.minimum_team_bid);
		}
	}
	ai_debug("Final bid: %d", bid);
	return bid;
}


static card_t get_play(player_t p, seat_t s)
{
	int i, chosen = -1;
	int myNeed, oppNeed, totTricks;
	int agg, lastTrick;
	int num = p;
	player_t pard = (num + 2) % 4;
	card_t lead, hi_card;
	hand_t *hand = &game.seats[num].hand;

	high = -1;

	/* Determine the suit which was led and the highest card played so
	   far. */
	if (game.leader != num) {
		lead = game.seats[game.players[game.leader].seat].table;
		high = game.leader;
		hi_card = game.seats[game.leader].table;
		for (i = (game.leader + 1) % 4; i != num; i = (i + 1) % 4) {
			card_t p_card = game.seats[i].table;
			if (((p_card.suit == SPADES)
			     && (hi_card.suit != SPADES))
			    || ((p_card.suit == hi_card.suit)
				&& (p_card.face > hi_card.face))) {
				high = i;
				hi_card = p_card;
			}
		}
	}
	if (game.leader == num)
		ai_debug("My lead");
	else
		ai_debug("%s led. %s of %s is high ",
			 suit_names[(int) lead.suit],
			 face_names[(int) hi_card.face],
			 suit_names[(int) hi_card.suit]);


	/* Determine our "aggression" level.  We're more aggressive if we're
	   trying to take tricks or set our opponents.  We're less aggressive
	   if we're trying to bag'em.  Agressiveness is on a scale from 0
	   (least aggressive) to 100 (most aggressive). */
	totTricks = game.seats[s].hand.hand_size;
	myNeed = game.players[num].bid.sbid.val +
		game.players[pard].bid.sbid.val;
	if (GSPADES.nil_tricks_count || game.players[num].bid.sbid.val > 0)
		myNeed -= game.players[num].tricks;
	if (GSPADES.nil_tricks_count || game.players[pard].bid.sbid.val > 0)
		myNeed -= game.players[pard].tricks;

	if (myNeed < 0)
		myNeed = 0;

	i = (num + 1) % 2;
	oppNeed = game.players[i].bid.sbid.val +
		game.players[i + 2].bid.sbid.val;
	if (GSPADES.nil_tricks_count || game.players[i].bid.sbid.val > 0)
		oppNeed -= game.players[i].tricks;
	if (GSPADES.nil_tricks_count || game.players[i + 2].bid.sbid.val > 0)
		oppNeed -= game.players[i + 2].tricks;
	if (oppNeed < 0)
		oppNeed = 0;

	ai_debug("We need %d and they need %d out of %d.", myNeed, oppNeed,
		 totTricks);

	/* XXX agg = 0 for oppNeed == 0 && myNeed == totTricks && this trick
	   hopeless */

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

	ai_debug("Aggression set to %d", agg);

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
		if (game.funcs->verify_play(hand->cards[i]) == NULL) {
			play[plays].card = hand->cards[i];
			Calculate(num, &play[plays], agg);
			plays++;
		}
	}

	ai_debug("Calculate %d:", num);
#ifdef AI_DEBUG
	for (i = 0; i < plays; i++)
		ai_debug(" %s of %s,%d,%d",
			 face_names[(int) play[i].card.face],
			 suit_names[(int) play[i].card.suit], play[i].trick,
			 play[i].future);
#endif

	/* Now determine our disposition for this trick. */
	if (game.players[num].bid.sbid.spec == SPADES_NIL &&
	    (game.players[num].tricks == 0 || oppNeed <= 0 || agg < 75))
		chosen = PlayNil(num);
	if (chosen < 0 && game.players[pard].bid.sbid.spec == SPADES_NIL
	    && (agg <= 50 || game.players[pard].tricks == 0))
		chosen = CoverNil(num, agg);
	if (chosen < 0 && agg < 100)
		chosen = SetNil(num, agg);
	if (chosen < 0)
		chosen = PlayNormal(num, agg, lastTrick);

	ai_debug("Chosen play is %d", chosen);


	for (i = 0; i < hand->hand_size; i++)
		if (cards_equal(play[chosen].card, hand->cards[i]))
			break;

	return hand->cards[i];
}


/* This function has a bug: it does not count the chance that our partner
   will win a trick if we don't. */
static void Calculate(int num, struct play *play, int agg)
{

	int mask, map, count, danger, trump, n, cover, sCount;
	card_t high_card = game.seats[high].table;
	char suit = game.seats[game.leader].table.suit;	/* the suit led */
	char s, r;
	player_t o, pard = (num + 2) % game.num_players;
	hand_t *hand = &game.seats[num].hand;

	/**
	 * The scale for all likelihoods runs as follows:
	 *
	 * -1      Guaranteed not to take trick
	 * 0..13   Unlikely to take trick (based on rank)
	 * 50..63  Likely to take trick (based on rank)
	 * 100     Guaranteed to take trick
	 */


	/* Calculate bitmap of ranks which beat this card */
	mask = 0;
	for (r = play->card.face + 1; r <= ACE_HIGH; r++)
		mask |= (1 << r);

	/* If this is the highest trump then its future is just as bright as
	   its present. */
	if (play->card.suit == SPADES && libai_is_highest_in_suit(play->card)) {
		play->future = 100;
		if (high >= 0 && high_card.suit == SPADES
		    && high_card.face > play->card.face)
			play->trick = -1;
		else
			play->trick = 100;
		return;
	}

	/* Card's rank is its likelihood of taking this trick if the card is
	   higher than the highest current card. */
	/* NOTE: we also calculate our partner's chance of winning with an
	   already-played card, so we must consider that case as well. */
	/* FIXME: there's still a bug here.  Sometimes we'll trump in on our
	   partner's aces! */
	if (high < 0
	    || ((play->card.suit == SPADES && high_card.suit != SPADES)
		|| (play->card.suit == high_card.suit
		    && play->card.face >= high_card.face))
		) {
		count = sCount = 0;
		if (!cards_equal(game.seats[(num + 1) % 4].table, UNKNOWN_CARD)) {	/* we're
											   the
											   last
											   card */
			play->trick = 100;
		} else {
			if (suit < 0) {	/* we're leading */
				s = play->card.suit;
				trump = 0;
			} else {
				s = suit;
				/* did we trump? */
				trump = (s != SPADES
					 && play->card.suit == SPADES);
			}
			for (o = (num + 1) % 4;
			     o != num
			     && cards_equal(game.seats[o].table,
					    UNKNOWN_CARD); o = (o + 1) % 4) {
				if (o == pard
				    && game.players[num].bid.sbid.val > 0)
					continue;
				map = SuitMap(o, num, s);
				if (map == 0 && s != SPADES) {	/* void in
								   the lead
								   suit */
					if ((map =
					     SuitMap(o, num, SPADES)) != 0) {
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
				if (SuitMap(o, p, s) & (1 << r)) {
					count++;	/* a player who's
							   played may have
							   this higher card */
					break;
				}
			if (count == 0) {
				for (o = (p + 1) % 4;
				     o != p && S.p[o].play < 0;
				     o = (o + 1) % 4)
					if (SuitMap(o, p, s) & (1 << r)) {
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
	for (o = (num + 1) % 4; o != num; o = (o + 1) % 4) {
		if ((map = SuitMap(o, num, s)) != 0)
			count++;
		if (o == pard && game.players[num].bid.sbid.val > 0)
			continue;
		if (map & mask)
			danger++;
		else if (map == 0 && s != SPADES
			 && SuitMap(o, num, SPADES) != 0)
			trump++;
	}
	if (trump)
		play->future = play->card.face;
	else {
		/* Count the cards in this suit we know about.  Then average
		   the remaining cards per player. */
		n = libai_cards_left_in_suit(s);
		for (r = 0; r < hand->hand_size; r++) {
			if (cards_equal(hand->cards[(int) r], UNKNOWN_CARD))
				continue;
			if (hand->cards[(int) r].suit == s) {
				cover++;	/* XXX - does ace cover king? 
						 */
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
}


/* Determine what cards some player "p" may be holding in suit "s" from the
   viewpoint of player "v". */
static int SuitMap(seat_t p, player_t v, char s)
{
	int map, i;
	hand_t *hand = &game.seats[v].hand;

	if (p == v) {		/* it's us! */
		map = 0;
		for (i = 0; i < hand->hand_size; i++) {
			if (hand->cards[i].suit == s)
				map |= 1 << hand->cards[i].face;
		}
	} else {
		map = libai_get_suit_map(p, s);
		for (i = 0; i < hand->hand_size; i++) {
			if (hand->cards[i].suit == s)
				map &= ~(1 << hand->cards[i].face);
		}
	}
	return map;
}


static int PlayNil(player_t p)
{
	int i, chosen = -1;

	ai_debug("Strategy: play nil");

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


static int CoverNil(player_t p, int agg)
{
	int i, chosen = -1, mask, r, danger = 0, sluff = 0;
	int map[4];
	char suit = game.seats[game.leader].table.suit;	/* the suit led */
	player_t pard = (p + 2) % 4;
	card_t pard_card = game.seats[pard].table;
	card_t high_card = game.seats[high].table;

	ai_debug("Strategy: cover nil");

	/* Construct pard's suitmaps. */
	for (i = 0; i < 4; i++)
		map[i] = SuitMap(pard, p, i);

	/* If our pard has the highest card, try to beat it. */
	if (pard == high) {
		for (i = 0; i < plays; i++)
			if (card_comp(play[i].card, pard_card) > 0)
				if (chosen < 0
				    || card_comp(play[i].card,
						 play[chosen].card) < 0)
					chosen = i;
	}

	/* Determine if there's any danger of our pard taking this trick. */
	if (high >= 0 && pard_card.suit == -1 && high_card.suit == suit) {
		for (r = high_card.face + 1; r <= ACE_HIGH; r++)
			if (map[(int) suit] & (1 << r))
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

				if (high < 0 || s == suit) {
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
					if (s == suit) {
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
					if (s != suit) {
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
				if (suit < 0 || play[i].card.suit == suit)
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


static int SetNil(player_t p, int agg)
{

	int i, pp, chosen = -1, r, mask, s, map, count;
	player_t pard = (p + 2) % 4;
	card_t high_card =
		(high == -1) ? UNKNOWN_CARD : game.seats[high].table;
	char suit = game.seats[game.leader].table.suit;	/* the suit led */

	ai_debug("Strategy: set nil");

	/* If one of our opponents bid nil and either hasn't played or has
	   played * the high card, try to get under it. */
	for (pp = (p + 1) % 4; pp != p && chosen < 0; pp = (pp + 1) % 4) {
		if (pp == pard || game.players[pp].bid.sbid.spec != SPADES_NIL
		    || game.players[pp].tricks != 0)
			continue;
		if (high < 0 && agg <= 50) {
			/* 
			 * If we're leading, lead something small.
			 */
			for (i = 0; i < plays; i++) {
				for (mask = 0, r = play[i].card.face + 1;
				     r <= ACE_HIGH; r++)
					mask |= (1 << r);	/* bitmap of
								   ranks
								   which beat
								   this card */
				if ((SuitMap(pp, p, play[i].card.suit) & mask)
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
			for (i = 0; i < plays; i++)
				if (card_comp(play[i].card, high_card) <= 0
				    && play[i].future < 40
				    && (chosen < 0
					|| (play[i].future >
					    play[chosen].future)))
					chosen = i;
		} else if (high >= 0 && game.seats[pp].table.suit < 0) {
			/* 
			 * Count how many cards we have under the high.
			 */
			count = 0;
			for (i = 0; i < plays; i++)
				if (card_comp(play[i].card, high_card) <= 0)
					count++;

			/* 
			 * Try to get just over the high card under certain conditions:
			 * 1. our aggression is high and our pard's not the high card
			 * 2. we don't have at least two cards under the high card
			 * 3. the high card is a face card
			 * 4. the nil bidder can't beat the high card
			 */
			s = high_card.suit;
			r = high_card.face;
			map = SuitMap(pp, p, suit);
			for (mask = 0, i = r + 1; i <= ACE_HIGH; i++)
				mask |= (1 << i);	/* bitmap of ranks
							   which beat high
							   card */

			if ((agg > 50 && high != pard)
			    || (count < 2)
			    || (r >= 9 && high != pard)
			    || (s != suit || (map & mask) == 0)) {
				if (s != suit) {	/* high player
							   trumped */
					if (map == 0)
						map = SuitMap(pp, p, 0);
					else
						map = 0;	/* play our
								   highest
								   trump */
				}
				for (r++; r <= ACE_HIGH; r++) {
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
				if (chosen < 0 && s == suit
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
		}
	}

	return chosen;
}


static int PlayNormal(player_t p, int agg, int lastTrick)
{

	int i, chosen = -1, n, r, s /* , tmp */ ;
	struct play pCard;	/* partner's card */
	/* int pmap[4], omap[4]; */
	player_t pard = (p + 2) % 4;

	ai_debug("Strategy: play normal");

	/* 
	 * If our pard has played, calculate the chances that his card is a winner.
	 */
	if (high == pard) {
		pCard.card = game.seats[pard].table;
		Calculate(p, &pCard, agg);
		ai_debug("Pard is winning with %d of %s; chance of winning trick is %d.", pCard.card.face, suit_names[(int) pCard.card.suit], pCard.trick);
	} else
		pCard.trick = pCard.future = -1;

	/* For normal play, our aggression level determines the desire we
	   have to take this trick. */
	if (lastTrick && pCard.trick >= 100)
		return PlayNil(p);
	if (agg == 0) {
		if (pard > 0 && pard == high && pCard.trick > 30) {
			/* Our pard's winning the trick -- beat him if we
			   can. */
		} else
			return PlayNil(p);
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
	if (chosen < 0 && high >= 0 && high != pard) {
		for (i = 0; i < plays; i++) {
			if ((play[i].card.suit == game.seats[high].table.suit)
			    && play[i].card.face > game.seats[high].table.face
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
				   || !(game.seats[high].table.suit == s
					&& game.seats[high].table.face == r)))
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
