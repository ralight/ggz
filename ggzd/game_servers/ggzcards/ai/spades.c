/*
 * File: ai.c
 * Author: Brent Hendricks
 * Project: GGZCards (NetSpades)
 * Date: 8/4/99
 *
 * This file contains the AI functions for playing spades.
 * The AI routines were adapted from Britt Yenne's spades game for
 * the palm pilot.  Thanks Britt!
 *
 * Later, they were stolen from NetSpades and integrated directly into
 * GGZCards for use with the spades game here.  Thanks Brent!
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


#include <config.h>         /* Site specific config */

#include <string.h>

#include "../cards.h"
#include "../common.h"
#include "../game.h"
#include "../ggz.h"

#include "../games/spades.h"


static void start_hand();
static void alert_bid(player_t p, bid_t bid);
static void alert_play(player_t p, card_t play);
static bid_t get_bid(player_t num);
static card_t get_play( player_t p, seat_t s);

struct ai_function_pointers spades_ai_funcs = {
	start_hand,
	alert_bid,
	alert_play,
	get_bid,
	get_play
};




static struct play {
  card_t card;		/* card's value */
  int trick;		/* likelyhood card will take this trick   */
  int future;		/* likelyhood card will take future trick */
} play[13];
static int plays, high, agg, lastTrick;

/* bitmask of played cards for each suit */
static int played[4];


static int ValidPlay(int, int);
static void Calculate(player_t num, struct play*);
static int SuitMap(seat_t, player_t, char );
static int PlayNil(int );
static int CoverNil(int );
static int SetNil(int );
static int PlayNormal(int );
static int card_comp(card_t c1, card_t c2);

static char suits[4][4];	/* information about what each of the 4 players holds in each of the 4 suits */

static int HasCard( seat_t s, card_t c) {
	/* TODO: avoid cheating! */
	int i;
	for( i = 0; i < game.seats[s].hand.hand_size; i++ )
		if( cards_equal(game.seats[s].hand.cards[i], c) )
			return 1;
	return 0;
}

static void start_hand()
{
	/* reset cards played */
	memset(played, 4*sizeof(int), 0);

	/* anyone _could_ have any card */
	memset(suits, 4 * 4 * sizeof(int), 1);
}

static void alert_bid(player_t p, bid_t bid)
{
	/* nothing... */
}

static void alert_play(player_t p, card_t play)
{
	/* there's a lot of information we can track from players' plays */
	card_t lead = game.seats[ game.players[game.leader].seat ].table;
	player_t op;

	/* first off, just remember which cards have been played. */
	played[(int)play.suit] |= 1 << play.face;

	/* if the player is void, remember it */
	if (play.suit != lead.suit)
		suits[p][(int)lead.suit] = 0;

#if 0
	/* when a nil player sluffs, they'll sluff their highest card in that suit */
	if (game.players[p].bid.sbid.spec == SPADES_NIL && game.players[p].tricks == 0 &&
	    play.suit != lead.suit && play.suit != SPADES)
		for (face = play.face+1; face <= ACE_HIGH; face++)
			suits[p][play.suit] &= ~(1<<face);
#endif

#if 0
	/* a nil player will generally play their highest card beneath an already-played card */
	if(game.players[p].bid.sbid.spec == SPADES_NIL &&
	   game.players[p].tricks == 0 &&
	   s == lead.suit) {
		if(!(suits[p][s] & (1<<r))) {
			/*
			 * If we didn't think this player could have this card, then he's
			 * doing something funky and all bets are off.  He might have anything.
			 */
			suits[p][s] = (~played[s]) & 0x7ffff;
		} else if( card.face < THE HIGHEST CARD PLAYED SO FAR IN THAT SUIT ) {
			/*
			 * nil bids don't have any between this card and highest play
			 */
			for(i = play.face + 1; i < trick[high].face; i++)
				suits[num][s] &= ~(1<<i);
		}
	}
#endif

	/* we now know that nobody has this card _anymore_ */
	for (op=0; op<4; op++)
		suits[op][(int)lead.suit] &= ~(1<<play.face);
	
	
}

static bid_t get_bid(player_t num)
{
	int count, points, i, gap, p1, p2, prob;
	bid_t bid, pard;
	int suitCount[4], suitAvg[4];
	int voids = 0, singletons = 0, doubletons = 0;
	int nilrisk = 99;
	card_t card = {0, 0, 0}, c = {0, 0, 0};
	char s; /* suit */
	hand_t *hand = &game.seats[ game.players[num].seat ].hand;

	bid.bid = 0;

	/* Partners's bid */
	pard = game.players[(num+2)%4].bid;

	/*
	 * First count our cards in each suit, and determine the average
	 * cards the other players each have in each suit.  Also count our
	 * voids, singletons, and doubletons.
	 */
	for( s = 0; s < 4; s++ )
		suitCount[(int)s] = 0;
	for( i = 0; i < hand->hand_size; i++)
		suitCount[ (int) hand->cards[i].suit ]++;
	for( s = 0; s < 4; s++ ) {
		if( s != SPADES ) /* for off suits */
			switch(suitCount[(int)s]) {
				case 0:	voids++;	break;	
				case 1:	singletons++;	break;	
				case 2:	doubletons++;	break;	
			}
		suitAvg[(int)s] = (game.hand_size - suitCount[(int)s]) / (game.num_players-1);
	}

	/* --------------- TRUMPS -----------------
	 *
	 * Trumps serve two purposes -- first, any trump sequence down from
	 * some high trump may be automatic tricks.  The remaining trumps may
	 * either be useful as high trumps or for trumping other tricks if we
	 * have some voids, singletons, or doubletons.
	 *
	 * First, figure out the brute force points possible from trumps:
	 */
	p1 = p2 = 0;
	card.suit = SPADES;
	card.deck = 0;
	for( card.face = ACE_HIGH, gap = 0; card.face >= 0; card.face-- ) {
		if( HasCard(num, card) ) {
			if( gap > 0)
				gap--;
			else {
				p1 += 100;
#ifdef DEBUG_BID
				ggz_debug("AI-SPADES: Counting guaranteed spade as 100\n");
#endif
			}
		} else
			gap++;
	}


	/*
	 * If our trumps are losable (ie. p1 == 0) and we don't have many
	 * of them, then consider whether we have low enough cards in
	 * each of the other suits to risk bidding nil.
	 */
	if( p1 == 0 ) {
		nilrisk = 0;
		if( suitCount[SPADES] > 3 ) {
			nilrisk += suitCount[SPADES] - 2;
#ifdef DEBUG_BID
			ggz_debug("AI-SPADES: Inc. nilrisk for %d excess spades\n", suitCount[SPADES] -2 );
#endif
		}
		for( s = 0; s < 4; s++ ) {
			c.suit = s;

			/* check if we're void -- good! */
			if( suitCount[(int)s] == 0 ) {
				if( s != SPADES ) {
					nilrisk -= 2;
#ifdef DEBUG_BID
					ggz_debug("AI-SPADES: Dec. nilrisk for shortsuit in %s\n", suit_names[(int)s] );
#endif
				}
				continue; /* for( s = 0; s < 4; s++ ) */
			}

			/* count our low cards in the suit */
			for( count = 0, c.face = 2; c.face < 9; c.face++ ) {
				if( HasCard(num, c))
					 count++;		/* count low cards */
			}
			if (count >= 3)		/* suit is covered */
				continue;               /* for( s = 0; s < 4; s++ ) */
			nilrisk += suitCount[(int)s] - count;	/* risky high cards */
#ifdef DEBUG_BID
			ggz_debug("AI-SPADES: Inc. nilrisk for %d high %s\n", suitCount[(int)s]-count,
				suit_names[(int)s] );
#endif
			if (count == 0) { /* no low cards */
				nilrisk++;
#ifdef DEBUG_BID
				ggz_debug("AI-SPADES: Inc. nilrisk for no low cards\n");
#endif
			}

			/* check for the queen/king of spades */
			if( s == SPADES ) {
				c.face = KING;
				if( HasCard(num, c) ) {
					nilrisk += 2;
#ifdef DEBUG_BID
					ggz_debug("AI-SPADES: Inc. nilrisk for King of spades\n");
#endif
				}
				c.face = QUEEN;
				if( HasCard(num, c) ) {
					nilrisk += 1;
#ifdef DEBUG_BID
					ggz_debug("AI-SPADES: Inc. nilrisk for Queen of spades\n");
#endif
				}
			} else {
				c.face = KING;
				if( HasCard(num, c) && suitCount[(int)s] <= 3 ) {
					nilrisk += ((suitCount[(int)s] <= 2) ? 2 : 1);	/* king is riskier */
#ifdef DEBUG_BID
					ggz_debug("AI-SPADES: Inc. nilrisk by %d for King of %s\n",(suitCount[(int)s]<=2)? 2 :1,
						suit_names[(int)s]);
#endif
				}
				c.face = ACE_HIGH;
				if (HasCard(num, c) && suitCount[(int)s] <= 3) {
					nilrisk += ((suitCount[(int)s] <= 2) ? 3 : 2);	/* ace is riskiest */
#ifdef DEBUG_BID
					ggz_debug("AI-SPADES: Inc. nilrisk by %d for Ace of %s\n",(suitCount[(int)s]<=2)? 3 : 2,
					suit_names[(int)s]);
#endif
				}
			}
		}
	}


	/*
	 * That done, frob p1 to count probable trump tricks.
	 */
	if (p1 == 0) {
		/* King with 1+ covers */
		c.suit = SPADES;
		c.face = KING;
		if( HasCard(num, c) && suitCount[SPADES] >= 2) {
			p1 += ((suitCount[SPADES] >= 3) ? 100 : 60);
#ifdef DEBUG_BID
			ggz_debug("AI-SPADES: Counting King of Spades as %d\n",(suitCount[SPADES]>=3)? 100 : 60);
#endif
		}
		c.face = QUEEN;
		/* Queen with 2+ covers */
		if( HasCard(num, c) && suitCount[SPADES] >= 3) {
			p1 += ((suitCount[SPADES] >= 4) ? 100 : 30);
#ifdef DEBUG_BID
			ggz_debug("AI-SPADES: Counting Queen of Spades as %d\n",(suitCount[SPADES]>=4)? 100 :30);
#endif
		}
	}


	/*
	 * Now figure if we use our trumps on other suits:
	 */
	count = suitCount[SPADES];
	for( c.face = ACE_HIGH; c.face >= 2 && HasCard(num,c); c.face--) {
		p2 += 100;
		count--;
	}
	for( ; voids > 0 && count > 0; voids--) {
		if( count >= 3 ) {
			p2 += 225;
			count -= 2;
#ifdef DEBUG_BID
		ggz_debug("AI-SPADES: Counting shortsuit as 225\n");
#endif
		} else if(count >= 2) {
			p2 += 190;
			count -= 2;
#ifdef DEBUG_BID
			ggz_debug("AI-SPADES: Counting shortsuit as 190\n");
#endif
		} else {
			p2 += 90;
			count -= 1;
#ifdef DEBUG_BID
			ggz_debug("AI-SPADES: Counting shortsuit as 90\n");
#endif
		}
  	}
	for (; singletons > 0 && count > 0; singletons--) {
		if (count >= 2) {
			p2 += 175;
			count -= 2;
#ifdef DEBUG_BID
			ggz_debug("AI-SPADES: Counting singleton as 175\n");
#endif
		} else {
			p2 += 90;
			count -= 1;
#ifdef DEBUG_BID
			ggz_debug("AI-SPADES: Counting singleton as 90\n");
#endif
		}
	}
	for (; doubletons > 0 && count > 0; doubletons--) {
		p2 += 75;
		count -= 1;
#ifdef DEBUG_BID
		ggz_debug("AI-SPADES: Counting doubleton as 75\n");
#endif
	}

	if( count > suitAvg[SPADES] ) {
		/*
		 * Length DOES matter.  :-)
		 */
		p2 += (count - suitAvg[SPADES] - 1) * 80;
#ifdef DEBUG_BID
		ggz_debug("AI-SPADES: Counting extra spades as %d\n",(count - suitAvg[SPADES] - 1) * 80);
#endif
	}  else if( count == 0 ) {
		p2 -= 30;		/* be wary of exhausting all trump */
#ifdef DEBUG_BID
		ggz_debug("AI-SPADES: Removing 30 points for exhausting spades(?)\n");
#endif
	}


	/*
	 * Okay, choose the better of the two scores.
	 */
	points = (p1 > p2) ? p1 : p2;
#ifdef DEBUG_BID
	ggz_debug("AI-SPADES: Spade points: %d\n",points);
#endif

	/* --------------- OTHER SUITS -----------------
	 *
	 * We bid other suits using a confidence system.  We combine two
	 * factors:  the confidence we can make some high card good, and
	 * the probability of that trick not getting trumped/finessed.
	 */
	for( s = 0; s < 3; s++ ) {
		c.suit = s;
		c.face = ACE_HIGH;
		if( (count = suitAvg[(int)s]) > suitCount[(int)s])
			count = suitCount[(int)s];	/* the number of probable tricks w/o trump*/
			prob = 100;			/* probability of not getting finessed*/
			for (; count > 0; count--, c.face--) {
				if( HasCard(num, c)) {
					points += prob;
#ifdef DEBUG_BID
					ggz_debug("AI-SPADES: Counting %s of %s as %d\n", face_names[(int)c.face], suit_names[(int)c.suit], prob);
#endif
					prob = prob * 9 / 10;
				} else
					prob = prob * 8 / 10;
 			}
	}


	/* --------------- NIL BIDS -----------------
	 *
	 * Consider bidding nil.
	 */
	if( game.players[(num+1)%4].bid.sbid.spec == SPADES_NIL ||
	    game.players[(num+3)%4].bid.sbid.spec == SPADES_NIL ) {
		nilrisk -= 2;
#ifdef DEBUG_BID
		ggz_debug("AI-SPADES: Dec. nilrisk because opponent is kneeling\n");
#endif
	}

	if( (pard.sbid.val >= GSPADES.minimum_team_bid || game.bid_count < 2) &&
	    ( (nilrisk <= 0) ||
	    (nilrisk <= 1 && points < 250) ||
	    (nilrisk <= 2 && points < 150) ||
	    (pard.sbid.val >= 3 + nilrisk))) {
		bid.sbid.spec = SPADES_NIL;
#ifdef DEBUG_BID
		ggz_debug("AI-SPADES: Bidding nil\n");
#endif
	}



	/* --------------- NORMAL BIDS -----------------
	 *
	 * Okay, divide points by 100 and that's our bid.
	 */
	if( bid.bid == 0 ) {
		if( game.bid_count < 2 ) {
			prob = 70;
#ifdef DEBUG_BID
			ggz_debug("AI-SPADES: Adding 70 because partner hasn't bid\n");
#endif
		} else {
			prob = 30;
#ifdef DEBUG_BID
			ggz_debug("AI-SPADES: Adding 30 because partner has bidn\n");
#endif
		}

		bid.sbid.val = (points + prob) / 100;
#ifdef DEBUG_BID
		ggz_debug("AI-SPADES: Subtotal bid: %d\n",bid);
#endif

		/*
		 * Consider ramifications of others' bids if everyone else has
		 * bid.
		 */
#if 0 /* not taken into account yet */
    count = bid;
    for (i = 0; i < 4; i++) {
      if (i == p)
	continue;
      if (S.p[i].bid < 0)
	break;		/* hasn't bid yet -- break*/
      count += S.p[i].bid;
    }
    if (i >= 4) {		/* everyone has bid*/
      /*
       * If the count is real high, reconsider potential tricks.
       * If the count is high and they bid nil, consider bidding less.
       */
      if (count >= 11) {
	if (count >= 13)
	  bid = points / 100;
	else if (p1 == 0 || p2 == 0)
	  bid = points / 100;
      }
    }
#endif

		/* don't forget the minimum bid!!! */
		if( game.bid_count >= 2 && (bid.sbid.val+pard.sbid.val < GSPADES.minimum_team_bid) ) {
			bid.sbid.val = GSPADES.minimum_team_bid - pard.sbid.val;
#ifdef DEBUG_BID
			ggz_debug("AI-SPADES: Upping bid to meet minimum of %d\n", GSPADES.minimum_team_bid);
#endif
		}
	}

#ifdef DEBUG_BID
	ggz_debug("AI-SPADES: Final bid: %d\n",bid);
#endif
	return bid;
}


static card_t get_play( player_t p, seat_t s)
{


	int i, chosen = -1;
	int myNeed, oppNeed, totTricks;
	int suit = -1;
	int num = p;
	player_t pard = (num + 2) % 4;
	card_t lead, hi_card;
	hand_t *hand = &game.seats[ num ].hand;

	high = -1;

	/*
	 * Determine the suit which was led and the highest card played
	 * so far.
	 */
	if( game.leader != num ) {
		lead = game.seats[ game.players[game.leader].seat ].table;
		suit = lead.suit;
		high = game.leader;
		hi_card = game.seats[ game.leader ].table;
		for( i = (game.leader+1)%4; i != num; i = (i+1) % 4 ) {
			card_t p_card = game.seats[ i ].table;
			if( ((p_card.suit == SPADES) && (hi_card.suit != SPADES)) ||
			    ((p_card.suit == hi_card.suit) && (p_card.face > hi_card.face)) ) {
				high = i;
				hi_card = p_card;
			}
		}
	}


#ifdef DEBUG_PLAY
  if( game.leader == num )
      ggz_debug("AI-SPADES: My lead\n");
  else
      ggz_debug("AI-SPADES: %s led. %s of %s is high \n", suit_names[(int)suit],
	     face_names[(int)hi_card.face], suit_names[(int)hi_card.suit]);
#endif


	/*
	 * Determine our "aggression" level.  We're more aggressive if we're
	 * trying to take tricks or set our opponents.  We're less aggressive
	 * if we're trying to bag'em.  Agressiveness is on a scale from 0 (least
	 * aggressive) to 100 (most aggressive).
	 */
	totTricks = 13 - game.players[0].tricks - game.players[1].tricks - game.players[2].tricks - game.players[3].tricks;
	myNeed = game.players[num].bid.sbid.val + game.players[pard].bid.sbid.val;
	if( game.players[num].bid.sbid.val > 0 )
		myNeed -= game.players[num].tricks;
	if( game.players[pard].bid.sbid.val > 0 )
		myNeed -= game.players[pard].tricks;

	if( myNeed < 0 )
		myNeed = 0;

	/* JDS: doesn't this assume that nil busts don't count toward the team? */
	i = (num + 1) % 2;
	oppNeed = game.players[i].bid.sbid.val + game.players[i+2].bid.sbid.val;
	if( game.players[i].bid.sbid.val > 0 )
		oppNeed -= game.players[i].tricks;
	if( game.players[i+2].bid.sbid.val > 0 )
		oppNeed -= game.players[i+2].tricks;
	if( oppNeed < 0 )
		oppNeed = 0;

#ifdef DEBUG_PLAY
	ggz_debug("AI-SPADES: We need %d and they need %d\n",myNeed, oppNeed);
#endif

	/* XXX agg = 0 for oppNeed == 0 && myNeed == totTricks &&
	 * this trick hopeless */

	if( myNeed == 0 && oppNeed == 0 )	/* can't set'em; bag'em instead */
		agg = 0;
	else if( totTricks < myNeed || totTricks < oppNeed ) /* not enough left */
		agg = 0;
	else if( totTricks - myNeed - oppNeed >= 3 )	/* lotsa bags */
		agg = (myNeed > 0) ? 25 : 0;
	else if( totTricks - myNeed - oppNeed >= 2 )	/* just be cautious */
		agg = 50;
	else if( totTricks - myNeed - oppNeed >= 1 )	/* need tricks */
		agg = 75;
	else						/* really need tricks */
		agg = 100;

#ifdef DEBUG_PLAY
	ggz_debug("AI-SPADES: Aggression set to %d\n", agg);
#endif


#if 0   /* not implemented yet*/
  /*
   * The lastTrick indicator is a special case when this is the last trick
   * we need to complete our bid, yet there are too many bags out there
   * to warrant taking the trick with the lowest possible card.
   */
  if (S.prefExpert && myNeed == 1
   && (agg < 50 || oppNeed == 0 || (oppNeed <= 3 && agg <= 50)))
    lastTrick = 1;
  else if (S.prefExpert && myNeed == 0 && oppNeed == totTricks)
    lastTrick = 1;
  else
#endif
    lastTrick = 0;


	/*
	 * Populate the play structure with our valid plays.
	 */
	plays = 0;
	for( i = 0; i < hand->hand_size; i++) {
		if( ValidPlay(num, i) == 0 ) {
			play[plays].card = hand->cards[i];
			Calculate(num, &play[plays]);
			plays++;
		}
	}

#ifdef DEBUG_PLAY
  ggz_debug("AI-SPADES: Calculate %d:\n", num);
  for( i = 0; i < plays; i++)
      ggz_debug("AI-SPADES:  %s of %s,%d,%d\n", face_names[(int)play[i].card.face], suit_names[(int)play[i].card.suit], play[i].trick,
	     play[i].future);
#endif

	/*
	 * Now determine our disposition for this trick.
	 */
	if( game.players[num].bid.sbid.spec == SPADES_NIL &&
	    (game.players[num].tricks == 0 || oppNeed <= 0 || agg < 75) )
		chosen = PlayNil(num);
	if (chosen < 0 && game.players[pard].bid.sbid.spec == SPADES_NIL && (agg <= 50 || game.players[pard].tricks == 0))
		chosen = CoverNil(num);
	if (chosen < 0 && agg < 100)
		chosen = SetNil(num);
	if (chosen < 0)
		chosen = PlayNormal(num);

#ifdef DEBUG_PLAY
  ggz_debug("AI-SPADES: Chosen play is %d\n", chosen);
#endif


	for( i=0; i<hand->hand_size; i++)
		if( cards_equal(play[chosen].card, hand->cards[i]) )
			break;

	return hand->cards[i];
}


static int ValidPlay(player_t p, int index) {

	int i;
	char suit = -1;
	hand_t *hand = &game.seats[p].hand;
	card_t card = hand->cards[index];

	if (game.leader != p)
		suit = game.seats[game.leader].table.suit;


#if 0   /* we're not enforcing low club on first trick */
  /*
   * Low clubs must be the first trick.
   */
  if (S.prefClubTrick && S.p[p].card[12] >= 0) {
    tmp = -1;
    for (i = 0; i < 13; i++)
      if (card_suit_num(S.p[p].card[i]) == 2)
	if (tmp < 0 || S.p[p].card[i] < tmp)
	  tmp = S.p[p].card[i];
    if (tmp >= 0)
      return (card == tmp) ? 0 : invalidLowClub;
  }
#endif


	/*
	 * Cannot lead trump until broken.
	 */
	if (suit < 0) {
		if (game.must_break_trump && !game.trump_broken && card.suit == game.trump) {
			for (i = 0; i < hand->hand_size; i++)
				if (hand->cards[i].suit != game.trump)
					return -1;
		}
		return 0;
	}

	/*
	 * Must follow suit unless we lead
	 */
	if( suit != -1 && card.suit != suit ) {
		for(i = 0; i < hand->hand_size; i++)
			if( hand->cards[i].suit == suit)
				return -1;
	}



#if 0   /* don't use this option either */
  /*
   * Must beat the highest card if possible.
   */
  if (S.prefMustBeat && CardCmp(card, high) <= 0) {
    tmp = 0;
    for (i = 0; i < 13 && S.p[p].card[i] >= 0; i++)
      if (card_suit_num(S.p[p].card[i]) == suit) {
	tmp++;
	if (CardCmp(S.p[p].card[i], high) > 0)
	  return invalidMustBeat;
      }
    if (tmp == 0) {
      for (i = 0; i < 13 && S.p[p].card[i] >= 0; i++)
	if (card_suit_num(S.p[p].card[i]) == 0 && CardCmp(S.p[p].card[i], high) > 0)
	  return invalidMustBeat;
    }
  }
#endif

	/*
	 * Valid!
	*/
	return 0;
}


static void Calculate(int num, struct play *play) {

	int mask, map, count, danger, trump, n, cover, sCount;
	card_t high_card = game.seats[high].table;
	char suit = game.seats[ game.leader ].table.suit; /* the suit led */
	char s, r;
	player_t o, pard = (num + 2) % game.num_players;
	hand_t *hand = &game.seats[ num ].hand;

	/*
	 * The scale for all likelihoods runs as follows:
	 *
	 * -1      Guaranteed not to take trick
	 * 0..13   Unlikely to take trick (based on rank)
	 * 50..63  Likely to take trick (based on rank)
	 * 100     Guaranteed to take trick
	 */


	/*
	 * Calculate bitmap of ranks which beat this card
	 */
	mask = 0;
	for( r = play->card.face+1; r <= ACE_HIGH; r++)
		mask |= (1<<r);

	/*
	 * If this is the highest trump then its future is just as bright
	 * as its present.
	 */
	if(play->card.suit == SPADES && (played[SPADES] & mask) == mask) {
		play->future = 100;
		if( high >= 0 && high_card.suit == SPADES && high_card.face > play->card.face )
			play->trick = -1;
		else
			play->trick = 100;
		return;
	}

	/*
	 * Card's rank is its likelihood of taking this trick if the card
	 * is higher than the highest current card.
	 */
	if (high < 0
	    || ( (play->card.suit == SPADES && high_card.suit != SPADES) || (play->card.suit == high_card.suit && play->card.face > high_card.face) )
 	    /* || play->card == trick[high]*/ ) {
		count = sCount = 0;
		if( !cards_equal( game.seats[(num+1)%4].table, UNKNOWN_CARD) ) {  /* we're the last card */
 			play->trick = 100;
		} else {
			if( suit < 0 ) {  /* we're leading */
				s = play->card.suit;
				trump = 0;
			} else {
				s = suit;
				/* did we trump? */
				trump = (s != SPADES && play->card.suit == SPADES);
			}
			for(o=(num+1) % 4 ; o != num && cards_equal( game.seats[o].table, UNKNOWN_CARD ); o = (o+1) % 4) {
				if( o == pard && game.players[num].bid.sbid.val > 0 )
					continue;
				map = SuitMap(o, num, s);
				if( map == 0 && s != SPADES ) {	/* void in the lead suit */
					if( (map = SuitMap(o, num, SPADES)) != 0) {
						trump = 0;	/* our trump isn't guaranteed anymore */
						if( play->card.suit != SPADES || (map & mask))
						count++;
					}
				} else if( s == play->card.suit && (map & mask)) {
					count++;
					sCount++;
				}
			}
			if (trump)
				play->trick = 50;
			else if(count) {
				play->trick = play->card.face;
				if (sCount == 0 && agg <= 25)
					play->trick += 15;	/* opponents may not trump*/
			}  else
				play->trick = play->card.face + 60;
		}
	}  else
		play->trick = -1;


#if 0 /* we don't do this */
  /*
   * Special case for prefMustBeat -- if the highest is out there then this card
   * may not be good.
   */
  if (S.prefMustBeat) {
    s = play->card.suit;
    for (r = play->card._value + 1; r <= ACE_HIGH && play->trick >= 0; r++) {
      count = 0;
      for (o = (p+3) % 4; o != p && S.p[o].play >= 0; o = (o+3) % 4)
	  if (SuitMap(o, p, s) & (1<<r)) {
	    count++;	/* a player who's played may have this higher card*/
	    break;
	  }
      if (count == 0) {
	for (o = (p+1) % 4; o != p && S.p[o].play < 0; o = (o+1) % 4)
	    if (SuitMap(o, p, s) & (1<<r)) {
	      play->trick = -1;	/* they gotta beat it*/
	      break;
	    }
      }
    }
  }
#endif

	/*
	 * Figure out how many players probably still have cards in
	 * this suit, and determine likelihood of making card good in
	 * future tricks.
	 */
	s = play->card.suit;
	count = danger = trump = cover = 0;
	for (o = (num+1) % 4; o != num; o = (o+1) % 4) {
		if( (map = SuitMap(o, num, s)) != 0)
			count++;
		if( o == pard && game.players[num].bid.sbid.val > 0)
			continue;
		if(map & mask)
			danger++;
		else if( map == 0 && s != SPADES && SuitMap(o, num, SPADES) != 0)
			trump++;
	}
	if (trump)
		play->future = play->card.face;
	else {
		/*
		 * Count the cards in this suit we know about.  Then average
		 * the remaining cards per player.
		 */
		n = 13;
		for( r = 2; r <= ACE_HIGH; r++)
			if(played[(int)s] & (1<<r))
				n--;
		for( r = 0; r < hand->hand_size; r++) {
			if( cards_equal( hand->cards[(int)r], UNKNOWN_CARD ) )
				continue;
			if( hand->cards[(int)r].suit == s) {
				cover++;	/* XXX - does ace cover king? */
				n--;
			}
		}
		n = (count == 0) ? 0 : ((n * 100) / count);
		if (n <= 100)
			danger++;

		/*
		 * XXX this should be redone to use relative rank in remaining
		 * cards rather than hard-coded ace, king, queen.
		 */
		play->future = play->card.face + ((s == SPADES) ? 15 : 0);
		if( play->card.face == ACE_HIGH ) {	
			if (n > 100)
				play->future += 50;
		} else if( play->card.face == KING ) {
			if (!danger || (cover >= 1 && n > 200))
				play->future += 50;
			else if (cover >= 1 && n > 100)
				play->future += 30;
		} else if( play->card.face == QUEEN ) {	
			if (!danger || (cover >= 2 && n > 300))
				play->future += 30;
		}
	}
}


/*
 * Determine what cards some player "p" may be holding in suit "s"
 * from the viewpoint of player "v".
 */
static int SuitMap(seat_t p, player_t v, char s)
{
	int map, i;
	hand_t *hand = &game.seats[ v ].hand;

	if( p == v ) { /* it's us! */
		map = 0;
		for( i = 0; i < hand->hand_size; i++ ) {
			if( hand->cards[i].suit == s)
				map |= 1 << hand->cards[i].face;
		}
	}  else {
		map = suits[p][(int)s];
		for( i = 0; i < hand->hand_size; i++) {
			if( hand->cards[i].suit == s)
				map &= ~(1 << hand->cards[i].face);
		}
	}
	return map;
}


static int PlayNil(player_t p)
{
	int i, chosen = -1;

#ifdef DEBUG_PLAY
	ggz_debug("AI-SPADES: Strategy: play nil\n");
#endif

	/*
	 * For nil bids, pick the card with highest potential that doesn't
	 * take the trick.
	 */
	for (i = 0; i < plays; i++) {
		if( chosen < 0 || play[i].trick < play[chosen].trick)
			chosen = i;
		else if (play[i].trick == play[chosen].trick) {
			if (play[i].future > play[chosen].future)
				chosen = i;
		}
	}
	return chosen;
}


static int CoverNil(player_t p)
{
	int i, chosen = -1, mask, r, danger = 0, sluff = 0;
	int map[4];
	char suit = game.seats[ game.leader ].table.suit; /* the suit led */
	player_t pard = (p + 2) % 4;
	card_t pard_card = game.seats[ pard ].table;
	card_t high_card = game.seats[ high ].table;

#ifdef DEBUG_PLAY
	ggz_debug("AI-SPADES: Strategy: cover nil\n");
#endif

	/*
	 * Construct pard's suitmaps.
	 */
	for (i = 0; i < 4; i++)
		map[i] = SuitMap(pard, p, i);

	/*
	 * If our pard has the highest card, try to beat it.
	 */
	if (pard == high) {
		for (i = 0; i < plays; i++)
			if( card_comp(play[i].card, pard_card) > 0)
				if (chosen < 0 || card_comp(play[i].card, play[chosen].card) < 0)
					chosen = i;
	}

	/*
	 * Determine if there's any danger of our pard taking this trick.
	 */
	if( high >= 0 && pard_card.suit == -1
	    && high_card.suit == suit) {
		for( r = high_card.face + 1; r <= ACE_HIGH; r++)
			if(map[(int)suit] & (1<<r))
				danger++;
	}

	/*
	 * If there's no danger or we can't cover him, sluff intelligently.
	 */
	if(chosen < 0 && danger == 0 && high >= 0) {
		if (agg <= 50)
			sluff = 1;
		/* else we'll drop through and play normally */
	} else if (chosen < 0) {
		/*
		 * If we're here, then our pard hasn't played yet and there's some danger of
		 * his taking the trick.  First things first -- always try to lead to a void.
		 */
		if (high < 0) {
			for (i = 0; i < plays; i++)
				if( map[(int)play[i].card.suit] == 0) {
					if (chosen < 0)
						chosen = i;
					else if (agg < 30 && play[i].trick < play[chosen].trick)
						chosen = i;
					else if (agg >= 30 && play[i].future <= play[i].trick) {
						if (play[chosen].future > play[chosen].trick)
							chosen = i;
						else if (play[i].trick > play[chosen].trick)
							chosen = i;
						else if (play[i].trick == play[chosen].trick) {
							if (play[i].future < play[chosen].future)
								chosen = i;
						}
					}
				}
	}

	/*
	 * Try to pick something he can't beat.
	 */
	if (chosen < 0) {
		for(i = 0; i < plays; i++) {
			char s = play[i].card.suit;
			if( high >= 0 && card_comp(play[i].card, high_card) <= 0)
				continue;	/* don't consider cards which don't beat high card*/

			if (high < 0 || s == suit) {
				for (mask = 0, r = play[i].card.face + 1; r < ACE_HIGH; r++)
					mask |= (1<<r);	  /* bitmap of ranks which beat this card*/
				if (map[(int)s] & mask)
					continue;
			}
			if (chosen < 0)
				chosen = i;
			else if (agg < 30) {
				if (s == suit) {
					if (play[i].trick < play[chosen].trick)
						chosen = i;
				}  else {
					/* If we're here then we're trumping.  Normally we'd throw
					 * our highest trump at this low aggression level, but we
					 * need to cover our pard's trumps later.
					 *
					 * XXX maybe throw a higher trump if pard is covered?*/
					if (play[i].trick < play[chosen].trick)
						chosen = i;	/* for now, choose lowest trump*/
					else if (play[i].trick == play[chosen].trick)
						if (play[i].future < play[chosen].future)
							chosen = i;
				}
			} else if (agg >= 30) {
				if (s != suit) {
					if (play[i].trick < play[chosen].trick)
						chosen = i;	/* both must be trumps -- choose lower*/
					else if (play[i].trick == play[chosen].trick)
						if (play[i].future < play[chosen].future)
							chosen = i;
				} else if (play[i].future <= play[i].trick) {
					if (play[chosen].future > play[chosen].trick)
						chosen = i;
					else if (play[i].trick > play[chosen].trick)
						chosen = i;
					else if (play[i].trick == play[chosen].trick) {
						if (play[i].future < play[chosen].future)
							chosen = i;
					}
				}
			}
		}
	}

	/*
	 * If we still haven't chosen one, then we're in a load of trouble.
	 * Play our highest card.
	 */
	if (chosen < 0 && sluff == 0) {
		sluff = 1;
		for (i = 0; i < plays; i++)
			if (suit < 0 || play[i].card.suit == suit)
				if (chosen < 0 || play[i].card.face > play[chosen].card.face)
					chosen = i;
	}

    /*
     * If this card is essentially one card greater than the current
     * high, then just sluff.
     */
    if (chosen >= 0 && high >= 0
	&& play[chosen].card.suit == high_card.suit) {
      for (r = high_card.face + 1; played[(int)high_card.suit] & (1<<r); r++)
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
	if (chosen < 0 || play[i].future < play[chosen].future)
	    chosen = i;
  }

  /* XXX can you upgrade the chosen card to one with a better chance
   * because you know that your pard doesn't have anything between this
   * card and the higher one?*/

  return chosen;
}


static int SetNil(int p) {

	int i, pp, chosen = -1, r, mask, s, map, count;
	player_t pard = (p+2) % 4;
	card_t high_card = (high == -1) ? UNKNOWN_CARD : game.seats[ high ].table;
	char suit = game.seats[ game.leader ].table.suit; /* the suit led */


#ifdef DEBUG_PLAY
  ggz_debug("AI-SPADES: Strategy: set nil\n");
#endif
/*
   * If one of our opponents bid nil and either hasn't played or has played
   * the high card, try to get under it.
   */
  for (pp = (p + 1) % 4; pp != p && chosen < 0; pp = (pp + 1) % 4) {
    if (pp == pard || game.players[pp].bid.sbid.spec != SPADES_NIL || game.players[pp].tricks != 0) continue;
    if (high < 0 && agg <= 50) {
      /*
       * If we're leading, lead something small.
       */
      for (i = 0; i < plays; i++) {
	for (mask = 0, r = play[i].card.face + 1; r <= ACE_HIGH; r++)
	    mask |= (1<<r);	     /* bitmap of ranks which beat this card*/
	if ((SuitMap(pp, p, play[i].card.suit) & mask) == 0)
	    continue;			/* he can't beat this one -- skip it*/
	if (chosen < 0 || play[i].card.face < play[chosen].card.face)
	    chosen = i;
      }
    }
    else if (high == pp) {
      /*
       * Try to get under the high card.
       */
      for (i = 0; i < plays; i++)
	  if (card_comp(play[i].card, high_card) <= 0
	      && play[i].future < 40
	      && (chosen < 0 || (play[i].future > play[chosen].future)))
	      chosen = i;
    }
    else if (high >= 0 && game.seats[pp].table.suit < 0) {
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
	mask |= (1<<i);		/* bitmap of ranks which beat high card */

      if ((agg > 50 && high != pard)
       || (count < 2)
       || (r >= 9 && high != pard)
       || (s != suit || (map & mask) == 0)) {
	if (s != suit) {	/* high player trumped*/
	  if (map == 0)
	    map = SuitMap(pp, p, 0);
	  else
	    map = 0;		/* play our highest trump*/
	}
	for (r++; r <= ACE_HIGH; r++) {
	  if (!(played[s] & (1<<r))) {
	    for (i = 0; i < plays; i++)
	      if (play[i].card.suit == s && play[i].card.face == r) {
		chosen = i;
		break;
	      }
	  }
	  if (map & (1<<r))
	    break;
	}
	/*
	 * If we can't get above the high card in suit, maybe we can
	 * trump it.
	 */
	if (chosen < 0 && s == suit && (map & mask) == SPADES) {
	  for (i = 0; i < plays; i++)
	      if (play[i].card.suit == SPADES)
		  if (map != 0 || play[i].card.face <= 6)
		      if (chosen < 0 || play[i].card.face > play[chosen].card.face)
			  chosen = i;
	}
      }
      /*
       * Otherwise, try to get under the high card.
       */
      for (i = 0; i < plays; i++)
	if (card_comp(play[i].card, high_card) <= 0
	 && play[i].future < 40
	 && (chosen < 0 || (play[i].future > play[chosen].future)))
	  chosen = i;
    }
  }

  return chosen;
}


static int PlayNormal(int p ) {

  int i, chosen = -1, n, r, s /*, tmp*/;
  struct play pCard;
  /* int pmap[4], omap[4]; */
	player_t pard = (p + 2) % 4;

#ifdef DEBUG_PLAY
  ggz_debug("AI-SPADES: Strategy: play normal\n");
#endif

  /*
   * If our pard has played, calculate the chances that his card has.
   */
  if (high == pard) {
    pCard.card = game.seats[pard].table;
    Calculate(p, &pCard);
  }
  else
    pCard.trick = pCard.future = -1;

  /*
   * For normal play, our aggression level determines the desire we
   * have to take this trick.
   */
  if (lastTrick && pCard.trick >= 100)
    return PlayNil(p);
  if (agg == 0) {
    if (pard > 0 && pard == high && pCard.trick > 30) {
      /*
       * Our pard's winning the trick -- beat him if we can.
       */
    }
    else
      return PlayNil(p);
  }

  /*
   * If our aggression is zero then we're here because our pard is pro'ly
   * gonna win this trick -- win it for him instead with our highest and
   * bestest card.
   *
   * For any other aggression, find the card that can win the trick but
   * has the lowest future value.
   */
  for (i = 0; i < plays; i++) {
    if (agg > 0 && (play[i].future > play[i].trick ||
		    (play[i].future == play[i].trick && (play[i].trick < 30 || !lastTrick))))
	continue;
    if (chosen < 0 || play[i].trick > play[chosen].trick)
	chosen = i;
    else if (play[i].trick == play[chosen].trick) {
      if (agg == 0 || (lastTrick && play[i].trick >= 50)) {
	if (play[i].future > play[chosen].future)
	    chosen = i;
      }
      else if (play[i].future < play[chosen].future)
	  chosen = i;
    }
  }
  
  /*
   * Watch the finesse.  Beat an opponent's best card if we can do so.
   */
  if (chosen < 0 && high >= 0 && high != pard) {
    for (i = 0; i < plays; i++) {
      if( (play[i].card.suit == game.seats[high].table.suit )
	  && play[i].card.face > game.seats[high].table.face
	  && (chosen < 0 || play[i].card.face < play[chosen].card.face))
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
      && (SysRandom(0) % 100) > agg)
      chosen = -1;
  
  /*
   * Consider leading to our pard's void.
   */
  if(high < 0 && SuitMap(pard, p, 0) != 0
     && (chosen < 0 || (SysRandom(0) % 2) == 0)) {
    tmp = -1;
    for (s = 0; s < 4; s++) {
      pmap[s] = SuitMap(pard, p, s);
      omap[s] = SuitMap((p+3)%4, p, s);
    }
    for (i = 0; i < plays; i++) {
      s = play[i].card.suit;
      if (pmap[s] == 0 && omap[s] != 0)
	  if (tmp < 0 || play[i].card.face < play[tmp].card.face)
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
	    if (chosen < 0 || play[i].card.face < play[chosen].card.face)
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
    }
    else if (play[i].future < play[chosen].future)
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
	if ( play[i].card.suit == s
	     && play[i].card.face > play[chosen].card.face
	     && (n < 0 || play[i].card.face < play[n].card.face))
	  n = i;
      if (n < 0)
	break;

      /*
       * Determine the highest rank equivalence of the chosen card,
       * based on which cards have been played.
       */
      r = play[chosen].card.face + 1;
      while (r <= ACE_HIGH && (played[s] & (1<<r))
	     && (high < 0 || !(game.seats[high].table.suit == s && game.seats[high].table.face == r) ))
	  r++;
      if (r != play[n].card.face		/* cards are equivalent */
	  && ((play[n].future >= 50)		/* card has a good future*/
	      || (play[n].future >= 20 && agg >= 50)))  /* card has a decent future*/
	break;
      chosen = n;
    }
  }

  return chosen;
}

/* return -1, 0, 1 if c1 is less than, equal to, or greater than c2 */
static int card_comp(card_t c1, card_t c2)
{
	if (c1.suit == c2.suit) return c1.face - c2.face;
	if (c1.suit == SPADES) return 1;
	if (c2.suit == SPADES) return -1;
	return 0;
}

