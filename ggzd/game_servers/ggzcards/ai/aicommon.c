/* 
 * File: ai/aicommon.c
 * Author: Jason Short
 * Project: GGZCards Server
 * Date: 07/03/2001
 * Desc: useful functions for AI bots
 * $Id: aicommon.c 2832 2001-12-09 21:41:07Z jdorje $
 *
 * This file contains the AI functions for playing any game.
 * The AI routines follow the none-too-successful algorithm of
 * always picking a random move.  At least they'll work with any
 * game!
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

#include <string.h>

#include "common.h"

#include "aicommon.h"

static int played[4];		/* bitmask of played cards for each suit */
static int playcount[4][4];	/* how many cards each player has played from 
				   each suit */
static int suits[4][4];		/* information about what each of the 4
				   players might hold in each of the 4 suits */

void ailib_start_hand(void)
{
	/* reset cards played */
	memset(played, 0, 4 * sizeof(int));
	memset(playcount, 0, 4 * 4 * sizeof(int));

	/* anyone _could_ have any card */
	memset(suits, 255, 4 * 4 * sizeof(int));
}

void ailib_alert_bid(player_t p, bid_t bid)
{

}

void ailib_alert_play(player_t p, card_t play)
{
	/* there's a lot of information we can track from players' plays */
	card_t lead = game.seats[game.players[game.leader].seat].table;
	player_t p2;

	/* remember which cards have been played. */
	played[(int) play.suit] |= 1 << play.face;
	playcount[p][(int) play.suit]++;

	/* if the player is void, remember it */
	if (play.suit != lead.suit)
		suits[p][(int) lead.suit] = 0;

	/* we now know that nobody has this card _anymore_ */
	for (p2 = 0; p2 < 4; p2++)
		suits[p2][(int) lead.suit] &= ~(1 << play.face);

}

int libai_is_card_played(char suit, char face)
{
	return played[(int) suit] & (1 << face);
}

void libai_player_doesnt_have_card(player_t p, card_t card)
{
	suits[p][(int) card.suit] &= ~(1 << card.face);
}

int libai_get_suit_map(player_t p, char suit)
{
	return suits[p][(int) suit];
}

int libai_might_player_have_card(player_t p, card_t card)
{
	return suits[p][(int) card.suit] & (1 << card.face);
}

int libai_is_card_in_hand(seat_t seat, card_t card)
{
	/* TODO: avoid cheating! */
	int i;
	for (i = 0; i < game.seats[seat].hand.hand_size; i++)
		if (cards_equal(game.seats[seat].hand.cards[i], card))
			return 1;
	return 0;
}

int libai_is_highest_in_suit(card_t card)
{
	char face = card.face;
	for (card.face = face + 1; card.face <= ACE_HIGH; card.face++)
		if (libai_is_card_played(card.suit, card.face))
			return 0;
	return 1;
}


int libai_cards_left_in_suit(char suit)
{
	/* This assumes we use exactly 2-ACE_HIGH */
	int n = 13;
	char face;
	for (face = 2; face <= ACE_HIGH; face++)
		if (libai_is_card_played(suit, face))
			n--;
	return n;
}


int libai_cards_played_in_suit(seat_t s, char suit)
{
	return playcount[s][(int) suit];
}


int libai_count_suit(seat_t seat, char suit)
{
	hand_t *hand = &game.seats[seat].hand;
	int i, total = 0;
	for (i = 0; i < hand->hand_size; i++)
		if (hand->cards[i].suit == suit)
			total++;
	return total;
}

void libai_forget_players_hand(player_t p, char suit)
{
	suits[p][(int) suit] = (~played[(int) suit]) & 0x7ffff;
}
