/*
 * File: cards.c
 * Author: Rich Gade
 * Project: LaPocha GGZ Server
 * Date: 08/14/2000
 * Desc: Various useful deck manipulate functions for card games
 *
 * Copyright (C) 2000 Brent Hendricks.
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
#  include <config.h>			/* Site-specific config */
#endif

#include <stdlib.h>

#include "cards.h"

static char deck[GGZ_MAX_DECKSIZE];
static int deck_size;
static int card_ptr=0;


/* cards_shuffle_deck()
 *   Setup and shuffle a deck of type
 *
 *   This is more complex than might seem necessary, but allows a game-designer
 *   to very easily invent new card deck types by merely setting the params
 */
void cards_shuffle_deck(char type)
{
	char *deck_cards, *deck_suits;
	char *c, *s;
	int card, card_value, suit_value;
	int i, j;
	char temp;

	/* Changing the following two is not as simple as it looks */
	/* all other functions will need updating appropriately    */
	char *std_cards = "A234567890JQK";
	char *std_suits = "CDHS";

	/* First set the deck parameters up */
	switch(type) {
		case GGZ_DECK_LAPOCHA:
			/* La Pocha doesn't uses 7,8,9 */
			deck_cards = "A234560JQK";
			deck_suits = "CDHS";
			break;
		case GGZ_DECK_STANDARD:
		default:
			deck_cards = "A234567890JQK";
			deck_suits = "CDHS";
			break;
	}

	/* Now generate an in-order deck */
	card = 0;
	s = deck_suits;
	while(*s != '\0') {
		for(suit_value=0;
		    std_suits[suit_value] != *s;
		    suit_value++)
			;
		c = deck_cards;
		while(*c != '\0') {
			for(card_value=0;
			    std_cards[card_value] != *c;
			    card_value++)
				;
			deck[card] = card_value + (13 * suit_value);
			c++;
			card++;
		}
		s++;
	}
	deck_size = card;

	/* Now we can randomize the deck order */
	/* Go through the deck, card by card */
	for(i=0; i<deck_size; i++) {
		/* Pick any OTHER card */
		do {
			j = random() % deck_size;
		} while(j == i);
		/* And swap positions */
		temp = deck[i];
		deck[i] = deck[j];
		deck[j] = temp;
	}
}


/* cards_deal_hands
 *   This deals the cards out from a pre-shuffled deck into hand structures
 */
void cards_deal_hands(int num_players, int hand_size, struct hand_t *hand)
{
	int i, j, player;
	char c1, c2;
	int card=0;
	char temp;

	/* Deal the cards out */
	for(i=0; i<hand_size; i++)
		for(player=0; player<num_players; player++)
			hand[player].card[i] = deck[card++];
	card_ptr = card;

	/* Store the handsize and sort cards */
	for(player=0; player<num_players; player++) {
		hand[player].hand_size = hand_size;
		for(i=0; i<hand_size-1; i++)
			for(j=i+1; j<hand_size; j++) {
				/* Mangle so ace comes out high */
				c1 = hand[player].card[i]-1;
				c2 = hand[player].card[j]-1;
				if((c1+1) % 13 == 0)
					c1 += 13;
				if((c2+1) % 13 == 0)
					c2 += 13;
				if(c1 > c2){
					temp = hand[player].card[i];
					hand[player].card[i] =
					                   hand[player].card[j];
					hand[player].card[j] = temp;
				}
			}
	}
}


/* cards_cut_for_trump
 *   This cuts the remaining cards and returns a suit for trump
 */
char cards_cut_for_trump(void)
{
	int card;
	char trump;

	card = (random() % (deck_size - card_ptr)) + card_ptr;
	trump = deck[card] / 13;

	return trump;
}


/* cards_suit_in_hand?
 *   This checks to see if a suit is contained in a hand
 */
char cards_suit_in_hand(struct hand_t *hand, char suit)
{
	int i;

	for(i=0; i<hand->hand_size; i++) {
		if(hand->card[i] != -1
		   && hand->card[i]/13 == suit)
			return 1;
	}

	return 0;
}


/* cards_highest_in_suit
 *   This checks a hand for the highest card value in the requested suit
 *   it returns the numeric value of the card (1-13)
 */
char cards_highest_in_suit(struct hand_t *hand, char suit)
{
	int i;
	char hi_card = 0;

	for(i=0; i<hand->hand_size; i++) {
		if(hand->card[i] != -1
		   && hand->card[i]/13 == suit
		   && card_value(hand->card[i]) > hi_card)
			hi_card = card_value(hand->card[i]);
	}

	return hi_card;
}


/* card_value
 *   Simply returns the value of a card from 1-13, wrapping Ace high
 */
char card_value(char card)
{
	char val;

	if((val = (card % 13)) == 0)
		val += 13;

	return val;
}
