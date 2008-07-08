/* 
 * File: deck.c
 * Author: Rich Gade, Jason Short
 * Project: GGZCards Server
 * Date: 08/14/2000 (as cards.c)
 * Desc: Various useful deck manipulation routines for card games
 * $Id: deck.c 10190 2008-07-08 03:32:44Z jdorje $
 *
 * This file was originally taken from La Pocha by Rich Gade.
 *
 * Copyright (C) 2000-2002 Brent Hendricks.
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

#include <gcrypt.h>

#include "common.h"
#include "deck.h"

struct deck_t {
	card_t *cards;
	int size;
	int ptr;		/* a pointer into the deck used for dealing */
};

/* cards_create_deck() set up the deck of the given type This is more complex 
   than might seem necessary, but allows a game-designer to very easily
   invent new card deck types by merely setting the params */
deck_t *create_deck(deck_type_t which_deck)
{
	int face, suit, deck;
	int cardnum;
	char *deck_faces, *deck_suits, *deck_decks;
	cardset_type_t cardset_type = CARDSET_FRENCH;
	int deck_face_cnt = 13, deck_suit_cnt = 4, deck_deck_cnt = 1;

	char std_deck_faces[] =
		{ 2, 3, 4, 5, 6, 7, 8, 9, 10, JACK, QUEEN, KING, ACE_HIGH };
	char std_deck_suits[] = { CLUBS, DIAMONDS, HEARTS, SPADES };
	char std_deck_decks[] = { 0 };

	char double_deck_decks[] = { 0, 1 };
	char skat_deck_faces[] = { 7, 8, 9, 10, JACK, QUEEN, KING, ACE_HIGH };
	char suaro_deck_faces[] = { 8, 9, 10, JACK, QUEEN, KING, ACE_HIGH };
	char euchre_deck_faces[] = { 9, 10, JACK, QUEEN, KING, ACE_HIGH };
	char lap_deck_faces[] =
		{ 2, 3, 4, 5, 6, 10, JACK, QUEEN, KING, ACE_HIGH };
	char sueca_deck_faces[] =
		{ 2, 3, 4, 5, 6, 7, JACK, QUEEN, KING, ACE_HIGH };
	char domino_deck_faces[] = {0, 1, 2, 3, 4, 5, 6};

	deck_t *mydeck = ggz_malloc(sizeof(*mydeck));
	mydeck->ptr = -1;
	mydeck->size = 0;

	deck_faces = std_deck_faces;
	deck_suits = std_deck_suits;
	deck_decks = std_deck_decks;

	/* First set the deck parameters up */
	switch (which_deck) {
	case GGZ_DECK_DOUBLE:
		deck_decks = double_deck_decks;
		deck_deck_cnt = 2;
		break;
	case GGZ_DECK_LAPOCHA:
		/* La Pocha doesn't use 7,8,9 */
		deck_faces = lap_deck_faces;
		deck_face_cnt = 10;
		break;
	case GGZ_DECK_SKAT:
		deck_faces = skat_deck_faces;
		deck_face_cnt = 8;
		break;
	case GGZ_DECK_SUARO:
		deck_faces = suaro_deck_faces;
		deck_face_cnt = 7;
		break;
	case GGZ_DECK_EUCHRE:
		deck_faces = euchre_deck_faces;
		deck_face_cnt = 6;
		break;
	case GGZ_DECK_SUECA:
		deck_faces = sueca_deck_faces;
		deck_face_cnt = 10;
		break;
	case GGZ_DECK_FULL:
		/* use a full 52-card deck */
		break;
	case GGZ_DECK_DOMINOES:
		deck_faces = domino_deck_faces;
		deck_face_cnt = 7;
		deck_suits = domino_deck_faces;
		deck_suit_cnt = 7;
		cardset_type = CARDSET_DOMINOES;
		break;
	default:
		ggz_debug(DBG_MISC, "Unknown deck %d.", which_deck);
	}
	
	set_cardset_type(cardset_type);

	/* Now generate an in-order deck */
	mydeck->size = deck_face_cnt * deck_suit_cnt * deck_deck_cnt;
	mydeck->cards = ggz_malloc(mydeck->size * sizeof(*mydeck->cards));

	cardnum = 0;
	for (deck = 0; deck < deck_deck_cnt; deck++)
		for (suit = 0; suit < deck_suit_cnt; suit++)
			for (face = 0; face < deck_face_cnt; face++) {
				card_t card = {.face = deck_faces[face],
				               .suit = deck_suits[suit],
				               .deck = deck_decks[deck]};
				if (is_valid_card(card)) {
					/* HACK: for instance, in dominoes
					   we wish to exclude certain cards. */
					mydeck->cards[cardnum] = card;
					cardnum++;
				}
			}
			
	mydeck->size = cardnum;

	ggz_debug(DBG_MISC, "Built a deck of size %d.", mydeck->size);

	return mydeck;
}

void destroy_deck(deck_t * deck)
{
	assert(deck && deck->cards);
	ggz_free(deck->cards);
	ggz_free(deck);
}

int get_deck_size(deck_t * deck)
{
	return deck->size;
}

/* Returns a random number in the range 0..size-1. */
unsigned int myrand(unsigned int size)
{
	if (size == 1) {
		return 0;
	} else if (size <= 256) {
		unsigned char new_rand;
		const unsigned int divisor = 256 / size;
		const unsigned int max = size * divisor - 1;

		do {
			gcry_randomize(&new_rand, sizeof(new_rand),
				       GCRY_STRONG_RANDOM);
		} while (new_rand > max);

		return new_rand / divisor;
	} else {
		unsigned int new_rand;
		const unsigned int rand_max = -1;
		const unsigned int divisor = rand_max / size;
		const unsigned int max = size * divisor - 1;

		/* Avoid bias. */
		do {
			gcry_randomize(&new_rand, sizeof(new_rand),
				       GCRY_STRONG_RANDOM);
		} while (new_rand > max);

		return new_rand / divisor;
	}
}

/* shuffle the deck */
void shuffle_deck(deck_t * deck)
{
	card_t temp;
	int i, j;

	ggz_debug(DBG_MISC, "Shuffling deck.");

	/* "Knuth shuffle" */
	for (i = 0; i < deck->size; i++) {
		/* Pick any position */
		j = i + myrand(deck->size - i);

		/* And swap positions */
		temp = deck->cards[i];
		deck->cards[i] = deck->cards[j];
		deck->cards[j] = temp;
	}

	deck->ptr = 0;
}

/* This deals the cards out from a pre-shuffled deck into a
   hand structure Game-nonspecific */
void deal_hand(deck_t * deck, int handsize, hand_t * hand)
{
	int c;
	card_t card;

	ggz_debug(DBG_MISC, "\tDealing out a hand of size %d.", handsize);

	assert(hand && deck && deck->cards);

	/* Deal the cards out */
	hand->hand_size = hand->full_hand_size = handsize;
	for (c = 0; c < hand->hand_size; c++) {
		assert(deck->ptr < deck->size);
		card = deck->cards[deck->ptr];
		hand->cards[c] = card;
		deck->ptr++;
	}

	cards_sort_hand(hand);
}

/* cards_deal_card This deals one card out from the deck Game-nonspecific */
card_t deal_card(deck_t * deck)
{
	card_t card;

	assert(deck->ptr < deck->size);

	card = deck->cards[deck->ptr];
	deck->ptr++;
	return card;
}


static int compare_cards(const void *c1, const void *c2)
{
	register card_t card1 = game.data->map_card(*(card_t *) c1);
	register card_t card2 = game.data->map_card(*(card_t *) c2);
	return game.data->compare_cards(card1, card2);
}

void cards_sort_hand(hand_t * hand)
{
	/* sort cards -- this should be in another function so it can be done 
	   later as well */
	qsort(hand->cards, hand->hand_size, sizeof(card_t), compare_cards);
}

/* cards_suit_in_hand? This checks to see how many of a suit is contained in
   a hand */
int cards_suit_in_hand(hand_t * hand, char suit)
{
	int i, cnt = 0;

	for (i = 0; i < hand->hand_size; i++) {
		card_t card = game.data->map_card(hand->cards[i]);
		if (card.suit == suit)
			cnt++;
	}

	return cnt;
}


/* cards_highest_in_suit This checks a hand for the highest card value in the 
   requested suit it returns the numeric value of the card (1-13) */
char cards_highest_in_suit(hand_t * hand, char suit)
{
	int i;
	char hi_card = 0;

	for (i = 0; i < hand->hand_size; i++) {
		if (hand->cards[i].suit == suit
		    && hand->cards[i].face > hi_card)
			hi_card = hand->cards[i].face;
	}

	return hi_card;
}
