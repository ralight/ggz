/* 
 * File: ai/aicommon.c
 * Author: Jason Short
 * Project: GGZCards Server
 * Date: 07/03/2001
 * Desc: useful functions for AI bots
 * $Id: aicommon.c 3489 2002-02-27 08:40:53Z jdorje $
 *
 * This file contains the AI functions for playing any game.
 * The AI routines follow the none-too-successful algorithm of
 * always picking a random move.  At least they'll work with any
 * game!
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
#include <string.h>

#include "client.h"

#include "aicommon.h"

/* FIXME: this whole thing assumes 4 seats and a standard deck.  But
   rewriting it to be general might be more trouble than its worth... */

static int played[4];		/* bitmask of played cards for each suit */
static int playcount[4][4];	/* how many cards each player has played from 
				   each suit */
static int suits[4][4];		/* information about what each of the 4
				   players might hold in each of the 4 suits */

static int leader = -1;
static int tricks[4];

void ailib_start_hand(void)
{
	int p, suit, face;

	/* reset cards played */
	memset(played, 0, 4 * sizeof(int));
	memset(playcount, 0, 4 * 4 * sizeof(int));

	memset(tricks, 0, 4 * sizeof(int));
	leader = -1;

	/* anyone _could_ have any card */
	for (p = 0; p < 4; p++)
		for (suit = 0; suit < 4; suit++) {
			suits[p][suit] = 0;
			for (face = 2; face <= ACE_HIGH; face++)
				suits[p][suit] |= 1 << face;
		}
}

void ailib_alert_trick(int p)
{
	leader = -1;
	tricks[p]++;
}

void ailib_alert_bid(int p, bid_t bid)
{

}

void ailib_alert_play(int p, card_t play)
{
	/* there's a lot of information we can track from players' plays */
	card_t lead;
	int p2;

	if (leader < 0)
		leader = p;

	lead = ggzcards.players[leader].table_card;

	/* remember which cards have been played. */
	played[(int) play.suit] |= 1 << play.face;
	playcount[p][(int) play.suit]++;

	/* if the player is void, remember it */
	if (play.suit != lead.suit)
		suits[p][(int) lead.suit] = 0;

	/* we now know that nobody has this card _anymore_ */
	for (p2 = 0; p2 < 4; p2++)
		suits[p2][(int) play.suit] &= ~(1 << play.face);

}

void ailib_our_play(int play_hand)
{
	if (leader < 0)
		leader = play_hand;
}

int get_tricks(int player)
{
	assert(player >= 0 && player < 4);
	return tricks[player];
}

int get_leader(void)
{
	assert(leader >= 0);
	return leader;
}

int libai_is_card_played(char suit, char face)
{
	return played[(int) suit] & (1 << face);
}

void libai_player_doesnt_have_card(int p, card_t card)
{
	suits[p][(int) card.suit] &= ~(1 << card.face);
}

int libai_get_suit_map(int p, char suit)
{
	return suits[p][(int) suit];
}

int libai_might_player_have_card(int p, card_t card)
{
	return suits[p][(int) card.suit] & (1 << card.face);
}

int libai_is_card_in_hand(int seat, card_t card)
{
	hand_t *hand = &ggzcards.players[seat].hand;

	/* TODO: avoid cheating! */
	int i;
	for (i = 0; i < hand->hand_size; i++)
		if (are_cards_equal(hand->cards[i], card))
			return 1;
	return 0;
}

card_t libai_get_highest_card_in_suit(int seat, char suit)
{
	card_t highest = UNKNOWN_CARD;
	int i;
	hand_t *hand = &ggzcards.players[seat].hand;

	for (i = 0; i < hand->hand_size; i++) {
		card_t card = hand->cards[i];
		if (card.suit == suit && card.face > highest.face)
			highest = card;
	}

	return highest;
}

int libai_is_highest_in_suit(card_t card)
{
	char face = card.face;
	for (card.face = face + 1; card.face <= ACE_HIGH; card.face++)
		if (libai_is_card_played(card.suit, card.face))
			return 0;
	return 1;
}


int libai_cards_played_in_suit(char suit)
{
	/* This assumes we use exactly 2-ACE_HIGH */
	int n = 0;
	char face;
	for (face = 2; face <= ACE_HIGH; face++)
		if (libai_is_card_played(suit, face))
			n++;
	return n;
}


int libai_cards_played_in_suit_p(int s, char suit)
{
	return playcount[s][(int) suit];
}


int libai_count_suit(int seat, char suit)
{
	hand_t *hand = &ggzcards.players[seat].hand;
	int i, total = 0;
	for (i = 0; i < hand->hand_size; i++)
		if (hand->cards[i].suit == suit)
			total++;
	return total;
}

void libai_forget_players_hand(int p, char suit)
{
	suits[p][(int) suit] = (~played[(int) suit]) & 0x7ffff;
}
