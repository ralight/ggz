/*
 * File: ai.c
 * Author: Brent Hendricks
 * Project: NetSpades
 * Date: 8/4/99
 * $Id: ai.c 2273 2001-08-27 06:48:01Z jdorje $
 *
 * This file contains the AI functions for playing spades.
 * The AI routines were adapted from Britt Yenne's spades game for
 * the palm pilot.  Thanks Britt!
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

#include <card.h>
#include <engine.h>


/* Global game information variable */
extern gameInfo_t gameInfo;

/* Global data */
extern int      bids[4];                     /* array of bids */
extern int    kneels[4];                   /* who is kneeling */
extern int     tally[4];                    /* Trick tally */

extern Card hands[4][13];
extern Card trick[4];

/* Globals for card counting */
extern int played[4];
extern int suits[4][4];

static int suit, high, pard, plays, agg, lastTrick;
static struct play {
  Card card;		/* card's value */
  int trick;		/* likelyhood card will take this trick   */
  int future;		/* likelyhood card will take future trick */
} play[13];


static int ValidPlay(int, int);
static void Calculate(int, struct play*);
static int SuitMap(int, int, int );
static int PlayNil(int );
static int CoverNil(int );
static int SetNil(int );
static int PlayNormal(int );
  
static int HasCard( int p, Card c) {
  
  int i;
  for( i = 0; i < 13; i++ )
    if( hands[p][i] == c )
      return 1;
  
  return 0;
}


int AIBid( int num ) {

  int count, points, i, c, s, gap, p1, p2, prob, bid = BID_INVALID;
  int suitCount[4], suitAvg[4];
  int voids = 0, singletons = 0, doubletons = 0;
  int nilrisk = 99;
  
  /* Card card; */

  /* Partners's bid */
  pard = bids[(num+2)%4];
  
  /*
   * First count our cards in each suit, and determine the average
   * cards the other players each have in each suit.  Also count our
   * voids, singletons, and doubletons.
   */
  for( s = 0; s < 4; s++ )
      suitCount[s] = 0;
  for( i = 0; i < 13; i++)
      suitCount[card_suit_num(hands[num][i])]++;
  for( s = 0; s < 4; s++ ) {
    if( s != SPADES ) /* for off suits */
	switch(suitCount[s]) {
	case 0:	voids++;	break;	
	case 1:	singletons++;	break;	
	case 2:	doubletons++;	break;	
	}
    suitAvg[s] = (13 - suitCount[s]) / 3;
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
  for( i = ACE, gap = 0; i >= 0; i-- ) {
    
    if( HasCard(num, card_conv(i,SPADES)) ) {
      if( gap > 0)
	  gap--;
      else {
	p1 += 100;
#ifdef DEBUG_BID
	ggzd_debug("Counting guaranteed spade as 100\n");
#endif
      }
    }
    else
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
      ggzd_debug("Inc. nilrisk for %d excess spades\n", suitCount[SPADES] -2 );
#endif
    }
    for( s = 0; s < 4; s++ ) {
      if( suitCount[s] == 0 ) {
	if( s != SPADES ) {
	  nilrisk -= 2;
#ifdef DEBUG_BID
	  ggzd_debug("Dec. nilrisk for shortsuit in %s\n", card_suit_str(s*13) );
#endif
	}
	continue; /* for( s = 0; s < 4; s++ ) */
      }
      c = card_conv(0,s);		/* the 2 of that suit */
      for( i = count = 0; i < 7; i++, c++ ) {
	if( HasCard(num, c))
	    count++;		/* count low cards */
      }
      if (count >= 3)		/* suit is covered */
	  continue;               /* for( s = 0; s < 4; s++ ) */
      nilrisk += suitCount[s] - count;	/* risky high cards */
#ifdef DEBUG_BID
      ggzd_debug("Inc. nilrisk for %d high %s\n", suitCount[s]-count,
	     card_suit_str(s*13) );
#endif
      if (count == 0) { /* no low cards */
	nilrisk++;
#ifdef DEBUG_BID
	ggzd_debug("Inc. nilrisk for no low cards\n");
#endif
      }
      if( s == SPADES ) {
	if( HasCard(num, card_conv(KING,SPADES)) ) {
	  nilrisk += 2;
#ifdef DEBUG_BID
	  ggzd_debug("Inc. nilrisk for King of spades\n");
#endif
	}
	if( HasCard(num, card_conv(QUEEN,SPADES)) ) {
	  nilrisk += 1;
#ifdef DEBUG_BID
	  ggzd_debug("Inc. nilrisk for Queen of spades\n");
#endif
	}
      }
      else {
	if( HasCard(num, card_conv(KING,s)) && suitCount[s] <= 3 ) {
	  nilrisk += ((suitCount[s] <= 2) ? 2 : 1);	/* king is riskier */
#ifdef DEBUG_BID
	  ggzd_debug("Inc. nilrisk by %d for King of %s\n",(suitCount[s]<=2)? 2 :1,
		 card_suit_str(s*13));
#endif
	}
	
	if (HasCard(num, card_conv(ACE,s)) && suitCount[s] <= 3) {
	  nilrisk += ((suitCount[s] <= 2) ? 3 : 2);	/* ace is riskiest */
#ifdef DEBUG_BID
	  ggzd_debug("Inc. nilrisk by %d for Ace of %s\n",(suitCount[s]<=2)? 3 : 2,
		 card_suit_str(s*13));
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
    if( HasCard(num, card_conv(KING,SPADES)) && suitCount[SPADES] >= 2) {
      p1 += ((suitCount[SPADES] >= 3) ? 100 : 60);
#ifdef DEBUG_BID
      ggzd_debug("Counting King of Spades as %d\n",(suitCount[SPADES]>=3)? 100 : 60);
#endif
    }
    /* Queen with 2+ covers */
    if( HasCard(num, card_conv(QUEEN,SPADES)) && suitCount[SPADES] >= 3) {
      p1 += ((suitCount[SPADES] >= 4) ? 100 : 30);
#ifdef DEBUG_BID
      ggzd_debug("Counting Queen of Spades as %d\n",(suitCount[SPADES]>=4)? 100 :30);
#endif
    }
  }


  /*
   * Now figure if we use our trumps on other suits:
   */
  count = suitCount[SPADES];
  for( c = ACE; c >= 0 && HasCard(num,card_conv(c,SPADES)); c--) {
    p2 += 100;
    count--;
  }
  for( ; voids > 0 && count > 0; voids--) {
    if( count >= 3 ) {
      p2 += 225;
      count -= 2;
#ifdef DEBUG_BID
      ggzd_debug("Counting shortsuit as 225\n");
#endif
    }
    else if(count >= 2) {
      p2 += 190;
      count -= 2;
#ifdef DEBUG_BID
      ggzd_debug("Counting shortsuit as 190\n");
#endif
    }
    else {
      p2 += 90;
      count -= 1;
#ifdef DEBUG_BID
      ggzd_debug("Counting shortsuit as 90\n");
#endif
    }
  }
  for (; singletons > 0 && count > 0; singletons--) {
    if (count >= 2) {
      p2 += 175;
      count -= 2;
#ifdef DEBUG_BID
      ggzd_debug("Counting singleton as 175\n");
#endif
    }
    else {
      p2 += 90;
      count -= 1;
#ifdef DEBUG_BID
      ggzd_debug("Counting singleton as 90\n");
#endif
    }
  }
  for (; doubletons > 0 && count > 0; doubletons--) {
    p2 += 75;
    count -= 1;
#ifdef DEBUG_BID
      ggzd_debug("Counting doubleton as 75\n");
#endif
  }
  if( count > suitAvg[SPADES] ) {
    /*
     * Length DOES matter.  :-)
     */
    p2 += (count - suitAvg[SPADES] - 1) * 80;
#ifdef DEBUG_BID
    ggzd_debug("Counting extra spades as %d\n",(count - suitAvg[SPADES] - 1) * 80);
#endif
  }
  else if( count == 0 ) {
    p2 -= 30;		/* be wary of exhausting all trump */
#ifdef DEBUG_BID
    ggzd_debug("Removing 30 points for exhausting spades(?)\n");
#endif
  }


  /*
   * Okay, choose the better of the two scores.
   */
  points = (p1 > p2) ? p1 : p2;
#ifdef DEBUG_BID
  ggzd_debug("Spade points: %d\n",points);
#endif

  /* --------------- OTHER SUITS -----------------
   *
   * We bid other suits using a confidence system.  We combine two
   * factors:  the confidence we can make some high card good, and
   * the probability of that trick not getting trumped/finessed.
   */
  for( s = 0; s < 3; s++ ) {
    c = card_conv(ACE,s);
    if( (count = suitAvg[s]) > suitCount[s])
	count = suitCount[s];	/* the number of probable tricks w/o trump*/
    prob = 100;			/* probability of not getting finessed*/
    for (; count > 0; count--, c--) {
      if( HasCard(num, c)) {
	points += prob;
#ifdef DEBUG_BID
	ggzd_debug("Counting %s as %d\n", card_name(c,LONG_NAME),prob);
#endif
	prob = prob * 9 / 10;
      }
      else
	prob = prob * 8 / 10;
    }
  }


  /* --------------- NIL BIDS -----------------
   *
   * Consider bidding nil.
   */
  if( bids[(num+1)%4] == BID_NIL || bids[(num+3)%4] == BID_NIL ) {
    nilrisk -= 2;
#ifdef DEBUG_BID
    ggzd_debug("Dec. nilrisk because opponent is kneeling\n");
#endif
  }

  if( (pard >= gameInfo.opt.minBid || pard == BID_INVALID) &&
      ( (nilrisk <= 0) ||
	(nilrisk <= 1 && points < 250) ||
	(nilrisk <= 2 && points < 150) ||
	(pard >= 3 + nilrisk))) {

    bid = BID_NIL;
#ifdef DEBUG_BID
    ggzd_debug("Bidding nil\n");
#endif
  }



  /* --------------- NORMAL BIDS -----------------
   *
   * Okay, divide points by 100 and that's our bid.
   */
  if( bid == BID_INVALID ) {
    if( pard == BID_INVALID ) {
      prob = 70;
#ifdef DEBUG_BID
      ggzd_debug("Adding 70 because partner hasn't bid\n");
#endif
    }
    else {
      prob = 30;
#ifdef DEBUG_BID
      ggzd_debug("Adding 30 because partner has bidn\n");
#endif
    }

    bid = (points + prob) / 100;
#ifdef DEBUG_BID
    ggzd_debug("Subtotal bid: %d\n",bid);
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
    if( pard == BID_NIL ) /* nil is 0 for purposes of minimum bid */
      pard = 0;
    if( (pard != BID_INVALID) && (bid+pard < gameInfo.opt.minBid) ) {
      bid = gameInfo.opt.minBid-pard;
#ifdef DEBUG_BID
      ggzd_debug("Upping bid to meet minimum of %d\n", gameInfo.opt.minBid);
#endif
    }
  }

#ifdef DEBUG_BID
  ggzd_debug("Final bid: %d\n",bid);
#endif
  return bid;
}


int AIPlay( int num, int lead ) {


  int i, chosen = -1;
  int myNeed, oppNeed, totTricks;

  /*
   * Determine the suit which was led and the highest card played
   * so far.
   */
  pard = (num + 2) % 4;
  suit = high = -1;
  if( lead != num ) {
    suit = card_suit_num( trick[lead] );
    high = lead;
    for( i = (lead+1)%4; i != num; i = (i+1) % 4 )
	if( card_comp(trick[i], trick[high]) > 0 )  {
	  high = i;
	}
  }


#ifdef DEBUG_PLAY
  if( lead == num )
      ggzd_debug("My lead\n");
  else
      ggzd_debug("%s led. %s is high \n", card_suit_str(suit*13),
	     card_name(trick[high],LONG_NAME));
#endif


  /*
   * Determine our "aggression" level.  We're more aggressive if we're
   * trying to take tricks or set our opponents.  We're less aggressive
   * if we're trying to bag'em.  Agressiveness is on a scale from 0 (least
   * aggressive) to 100 (most aggressive).
   */
  totTricks = 13 - tally[0] - tally[1] - tally[2] - tally[3];
  myNeed = bids[num] + bids[pard];
  if( bids[num] > 0 )
    myNeed -= tally[num];
  if( bids[pard] > 0 )
    myNeed -= tally[pard];
  if( myNeed < 0 )
    myNeed = 0;

  i = (num + 1) % 2;
  oppNeed = bids[i] + bids[i+2];
  if( bids[i] > 0 )
    oppNeed -= tally[i];
  if( bids[i+2] > 0 )
    oppNeed -= tally[i+2];
  if( oppNeed < 0 )
    oppNeed = 0;

#ifdef DEBUG_PLAY
  ggzd_debug("We need %d and they need %d\n",myNeed, oppNeed);
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
  ggzd_debug("Aggression set to %d\n", agg);
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
  for( i = 0; i < 13; i++) {
    if( hands[num][i] == BLANK_CARD ) continue;
    if( ValidPlay(num, i) == 0 ) {
      play[plays].card = hands[num][i];
      Calculate(num, &play[plays]);
      plays++;
    }
  }

#ifdef DEBUG_PLAY
  ggzd_debug("Calculate %d:\n", num);
  for( i = 0; i < plays; i++)
      ggzd_debug(" %s,%d,%d\n", card_name(play[i].card,LONG_NAME), play[i].trick,
	     play[i].future);
#endif

  /*
   * Now determine our disposition for this trick.
   */
  if( kneels[num] == 1 && (tally[num] == 0 || oppNeed <= 0 || agg < 75) )
      chosen = PlayNil(num);
  if (chosen < 0 && kneels[pard] == 1 && (agg <= 50 || tally[pard] == 0))
      chosen = CoverNil(num);
  if (chosen < 0 && agg < 100)
      chosen = SetNil(num);
  if (chosen < 0)
      chosen = PlayNormal(num);

#ifdef DEBUG_PLAY
  ggzd_debug("Chosen play is %d\n", chosen);
#endif


  for( i=0; i<13; i++)
      if( play[chosen].card == hands[num][i] )
	  break;

  return i;

}


static int ValidPlay(int p, int index) {

  int i, status = 0;
  /* int tmp; */


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


#if 0   /* we don't use this option */
  /*
   * Cannot lead trump until broken.
   */
  if (suit < 0) {
    if (S.prefSpadeBreak && card_suit_num(card) == 0 && played[0] == 0) {
      for (i = 0; i < 13 && S.p[p].card[i] >= 0; i++)
	if (card_suit_num(S.p[p].card[i]) != 0)
	  return invalidTrump;
    }
    return 0;
  }
#endif

  /*
   * Must follow suit unless we lead
   */
  if( (suit != -1) && ( card_suit_num(hands[p][index]) != suit) ) {
    for(i = 0; i < 13; i++)
      if( card_suit_num(hands[p][i]) == suit)
	status = -1;
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
  return status;
}


static void Calculate(int num, struct play *play) {

  int mask, r, o, map, s, count, danger, trump, n, cover, sCount;
  /* int i; */

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
  for( r = card_rank(play->card)+1; r < 13; r++)
    mask |= (1<<r);

  /*
   * If this is the highest trump then its future is just as bright
   * as its present.
   */
  if(card_suit_num(play->card) == SPADES && (played[SPADES] & mask) == mask) {
    play->future = 100;
    if( high >= 0 && card_comp(trick[high], play->card) > 0 )
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
      || card_comp( play->card, trick[high] ) > 0
      || play->card == trick[high] ) {
    count = sCount = 0;
    o = (num+1) % 4;		    /* next player */
    if( trick[o] != BLANK_CARD ) {  /* we're the last card */
      play->trick = 100;
    }
    else {
      if( suit < 0 ) {  /* we're leading */
	s = card_suit_num(play->card);
	trump = 0;
      }
      else {
	s = suit;
	/* did we trump? */
	trump = (s != SPADES && card_suit_num(play->card) == SPADES);
      }
      for( ; o != num && trick[o] == BLANK_CARD; o = (o+1) % 4) {
	if( o == pard && bids[num] > 0 )
	    continue;
	map = SuitMap(o, num, s);
	if( map == 0 && s != SPADES ) {	/* void in the lead suit */
	  if( (map = SuitMap(o, num, SPADES)) != 0) {
	    trump = 0;	/* our trump isn't guaranteed anymore */
	    if( card_suit_num(play->card) != SPADES || (map & mask))
		count++;
	  }
	}
	else if( s == card_suit_num(play->card) && (map & mask)) {
	  count++;
	  sCount++;
	}
      }
      if (trump)
	  play->trick = 50;
      else if(count) {
	play->trick = card_rank(play->card);
	if (sCount == 0 && agg <= 25)
	    play->trick += 15;	/* opponents may not trump*/
      }
      else
	  play->trick = card_rank(play->card) + 60;
    }
  }
  else
      play->trick = -1;


#if 0 /* we don't do this */
  /*
   * Special case for prefMustBeat -- if the highest is out there then this card
   * may not be good.
   */
  if (S.prefMustBeat) {
    s = card_suit_num(play->card);
    for (r = play->card._value + 1; r < 13 && play->trick >= 0; r++) {
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
  s = card_suit_num(play->card);
  count = danger = trump = cover = 0;
  for (o = (num+1) % 4; o != num; o = (o+1) % 4) {
    if( (map = SuitMap(o, num, s)) != 0)
	count++;
    if( o == pard && bids[num] > 0)
	continue;
    if(map & mask)
	danger++;
    else if( map == 0 && s != SPADES && SuitMap(o, num, SPADES) != 0)
	trump++;
  }
  if (trump)
      play->future = card_rank(play->card);
  else {
    /*
     * Count the cards in this suit we know about.  Then average
     * the remaining cards per player.
     */
    n = 13;
    for( r = 0; r < 13; r++)
	if(played[s] & (1<<r))
	    n--;
    for( r = 0; r < 13; r++) {
      if( hands[num][r] == BLANK_CARD ) continue;
      if( card_suit_num(hands[num][r]) == s) {
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
    play->future = card_rank(play->card) + ((s == SPADES) ? 15 : 0);
    if( card_rank(play->card) == ACE ) {	
      if (n > 100)
	  play->future += 50;
    }
    else if( card_rank(play->card) == KING ) {
      if (!danger || (cover >= 1 && n > 200))
	  play->future += 50;
      else if (cover >= 1 && n > 100)
	  play->future += 30;
    }
    else if( card_rank(play->card) == QUEEN ) {	
      if (!danger || (cover >= 2 && n > 300))
	  play->future += 30;
    }
  }
}


/*
 * Determine what cards some player "p" may be holding in suit "s"
 * from the viewpoint of player "v".
 */
static int SuitMap(int p, int v, int s) {

  int map, i;

  if( p == v ) { /* it's us! */
    map = 0;
    for( i = 0; i < 13; i++ ) {
      if( hands[v][i] == BLANK_CARD ) continue;
      if( card_suit_num(hands[v][i]) == s)
	  map |= (1 << card_rank(hands[v][i]));
    }
  }
  else {
    map = suits[p][s];
    for( i = 0; i < 13; i++) {
      if( hands[v][i] == BLANK_CARD ) continue;
      if( card_suit_num(hands[v][i]) == s)
	  map &= ~(1 << card_rank(hands[v][i]));
    }
  }
  return map;
}


static int PlayNil(int p) {

  int i, chosen = -1;

#ifdef DEBUG_PLAY
  ggzd_debug("Strategy: play nil\n");
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


static int CoverNil(int p) {

  int i, chosen = -1, mask, r, danger = 0, sluff = 0;
  int map[4];
  int s = -1; /* avoid compiler warning */

#ifdef DEBUG_PLAY
  ggzd_debug("Strategy: cover nil\n");
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
	if( card_comp(play[i].card, trick[pard]) > 0)
	    if (chosen < 0 || card_comp(play[i].card, play[chosen].card) < 0)
		chosen = i;
  }

  /*
   * Determine if there's any danger of our pard taking this trick.
   */
  if( high >= 0 && trick[pard] == BLANK_CARD
     && card_suit_num(trick[high]) == suit) {
    for( r = card_rank(trick[high]) + 1; r < 13; r++)
	if(map[suit] & (1<<r))
	    danger++;
  }

  /*
   * If there's no danger or we can't cover him, sluff intelligently.
   */
  if(chosen < 0 && danger == 0 && high >= 0) {
    if (agg <= 50)
	sluff = 1;
    /* else we'll drop through and play normally */
  }
  else if (chosen < 0) {
    /*
     * If we're here, then our pard hasn't played yet and there's some danger of
     * his taking the trick.  First things first -- always try to lead to a void.
     */
    if (high < 0) {
      for (i = 0; i < plays; i++)
	  if( map[card_suit_num(play[i].card)] == 0) {
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
	s = card_suit_num(play[i].card);
	if( high >= 0 && card_comp(play[i].card, trick[high]) <= 0)
	    continue;	/* don't consider cards which don't beat high card*/

	if (high < 0 || s == suit) {
	  for (mask = 0, r = card_rank(play[i].card) + 1; r < 13; r++)
	      mask |= (1<<r);	  /* bitmap of ranks which beat this card*/
	  if (map[s] & mask)
	    continue;
	}
	if (chosen < 0)
	  chosen = i;
	else if (agg < 30) {
	  if (s == suit) {
	    if (play[i].trick < play[chosen].trick)
	      chosen = i;
	  }
	  else {
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
	}
	else if (agg >= 30) {
	  if (s != suit) {
	    if (play[i].trick < play[chosen].trick)
		chosen = i;	/* both must be trumps -- choose lower*/
	    else if (play[i].trick == play[chosen].trick)
		if (play[i].future < play[chosen].future)
		    chosen = i;
	  }
	  else if (play[i].future <= play[i].trick) {
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
	  if (suit < 0 || card_suit_num(play[i].card) == suit)
	      if (chosen < 0 || card_rank(play[i].card) > card_rank(play[chosen].card))
		  chosen = i;
    }

    /*
     * If this card is essentially one card greater than the current
     * high, then just sluff.
     */
    if (chosen >= 0 && high >= 0
	&& card_suit_num(play[chosen].card) == card_suit_num(trick[high])) {
      for (r = card_rank(trick[high]) + 1; played[s] & (1<<r); r++)
	  continue;
      if (r == card_rank(play[chosen].card)) {
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


#ifdef DEBUG_PLAY
  ggzd_debug("Strategy: set nil\n");
#endif
/*
   * If one of our opponents bid nil and either hasn't played or has played
   * the high card, try to get under it.
   */
  for (pp = (p + 1) % 4; pp != p && chosen < 0; pp = (pp + 1) % 4) {
    if (pp == pard || kneels[pp] == 0 || tally[pp] != 0) continue;
    if (high < 0 && agg <= 50) {
      /*
       * If we're leading, lead something small.
       */
      for (i = 0; i < plays; i++) {
	for (mask = 0, r = card_rank(play[i].card) + 1; r < 13; r++)
	    mask |= (1<<r);	     /* bitmap of ranks which beat this card*/
	if ((SuitMap(pp, p, card_suit_num(play[i].card)) & mask) == 0)
	    continue;			/* he can't beat this one -- skip it*/
	if (chosen < 0 || card_rank(play[i].card) < card_rank(play[chosen].card))
	    chosen = i;
      }
    }
    else if (high == pp) {
      /*
       * Try to get under the high card.
       */
      for (i = 0; i < plays; i++)
	  if (card_comp(play[i].card, trick[high]) <= 0
	      && play[i].future < 40
	      && (chosen < 0 || (play[i].future > play[chosen].future)))
	      chosen = i;
    }
    else if (high >= 0 && trick[pp] < 0) {
      /*
       * Count how many cards we have under the high.
       */
      count = 0;
      for (i = 0; i < plays; i++)
	  if (card_comp(play[i].card, trick[high]) <= 0)
	  count++;

      /*
       * Try to get just over the high card under certain conditions:
       * 1. our aggression is high and our pard's not the high card
       * 2. we don't have at least two cards under the high card
       * 3. the high card is a face card
       * 4. the nil bidder can't beat the high card
       */
      s = card_suit_num(trick[high]);
      r = card_rank(trick[high]);
      map = SuitMap(pp, p, suit);
      for (mask = 0, i = r + 1; i < 13; i++)
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
	for (r++; r < 13; r++) {
	  if (!(played[s] & (1<<r))) {
	    for (i = 0; i < plays; i++)
	      if (play[i].card == (s * 13 + r)) {
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
	      if (card_suit_num(play[i].card) == SPADES)
		  if (map != 0 || card_rank(play[i].card) <= 6)
		      if (chosen < 0 || play[i].card > play[chosen].card)
			  chosen = i;
	}
      }
      /*
       * Otherwise, try to get under the high card.
       */
      for (i = 0; i < plays; i++)
	if (card_comp(play[i].card, trick[high]) <= 0
	 && play[i].future < 40
	 && (chosen < 0 || (play[i].future > play[chosen].future)))
	  chosen = i;
    }
  }

  return chosen;
}


static int PlayNormal(int p ) {

  int i, chosen = -1, n, r, s;
  struct play pCard;
  /* int tmp, pmap[4], omap[4]; */

#ifdef DEBUG_PLAY
  ggzd_debug("Strategy: play normal\n");
#endif

  /*
   * If our pard has played, calculate the chances that his card has.
   */
  if (high == pard) {
    pCard.card = trick[pard];
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
      if( (card_suit_num(play[i].card) == card_suit_num(trick[high]) )
	  && play[i].card > trick[high]
	  && (chosen < 0 || play[i].card < play[chosen].card))
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
      s = card_suit_num(play[i].card);
      if (pmap[s] == 0 && omap[s] != 0)
	  if (tmp < 0 || card_rank(play[i].card) < card_rank(play[tmp].card))
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
	    if (chosen < 0 || card_rank(play[i].card) < card_rank(play[chosen].card))
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
    s = card_suit_num(play[chosen].card);
    for (;;) {
      /*
       * Find the next highest card in suit.
       */
      n = -1;
      for (i = 0; i < plays; i++)
	if ( card_suit_num(play[i].card) == s
	     && play[i].card > play[chosen].card
	     && (n < 0 || play[i].card < play[n].card))
	  n = i;
      if (n < 0)
	break;

      /*
       * Determine the highest rank equivalence of the chosen card,
       * based on which cards have been played.
       */
      r = card_rank(play[chosen].card) + 1;
      while (r < 13 && (played[s] & (1<<r))
	     && (high < 0 || (s*13+r) != trick[high]))
	  r++;
      if (r != card_rank(play[n].card)		/* cards are equivalent */
	  && ((play[n].future >= 50)		/* card has a good future*/
	      || (play[n].future >= 20 && agg >= 50)))  /* card has a decent future*/
	break;
      chosen = n;
    }
  }

  return chosen;
}

