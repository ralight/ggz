/*
 * File: cards.c
 * Author: Rich Gade, Jason Short
 * Project: GGZCards GGZ Server
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

#include <stdlib.h>

#include "cards.h"
#include "ggz.h"

#include "game.h"		/* just for alloc */

static card_t *gamedeck = NULL;	/* must be allocated */
static int deck_size = 0;
static int deck_ptr = -1;	/* a pointer into the deck used for dealing */

char *suit_names[4] = { "clubs", "diamonds", "hearts", "spades" };
char *short_suit_names[4] = { "C", "D", "H", "S" };
char *face_names[15] =
	{ NULL, "ace", "two", "three", "four", "five", "six", "seven",
	"eight", "nine", "ten", "jack", "queen", "king", "ace"
};
char *short_face_names[15] =
	{ NULL, "A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q",
	"K", "A"
};

/* cards_create_deck()
 *   set up the deck of the given type
 *
 *   This is more complex than might seem necessary, but allows a game-designer
 *   to very easily invent new card deck types by merely setting the params
 */
void cards_create_deck(deck_type_t which_deck)
{
	int face, suit, deck;
	int cardnum;
	char *deck_faces, *deck_suits, *deck_decks;
	int deck_face_cnt = 13, deck_suit_cnt = 4, deck_deck_cnt = 1;
	card_t card;

	char std_deck_faces[] =
		{ 2, 3, 4, 5, 6, 7, 8, 9, 10, JACK, QUEEN, KING, ACE_HIGH };
	char std_deck_suits[] = { CLUBS, DIAMONDS, HEARTS, SPADES };
	char std_deck_decks[] = { 0 };

	char lap_deck_faces[] =
		{ 2, 3, 4, 5, 6, 10, JACK, QUEEN, KING, ACE_HIGH };
	char suaro_deck_faces[] = { 8, 9, 10, JACK, QUEEN, KING, ACE_HIGH };
	char euchre_deck_faces[] = { 9, 10, JACK, QUEEN, KING, ACE_HIGH };
	char skat_deck_faces[] = { 7, 8, 9, 10, JACK, QUEEN, KING, ACE_HIGH };
	char sueca_deck_faces[] =
		{ 2, 3, 4, 5, 6, 7, JACK, QUEEN, KING, ACE_HIGH };

	deck_faces = std_deck_faces;
	deck_suits = std_deck_suits;
	deck_decks = std_deck_decks;

	/* First set the deck parameters up */
	switch (which_deck) {
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
	default:
		ggz_debug("Unknown deck %d.", which_deck);
	}

	/* Now generate an in-order deck */
	deck_size = deck_face_cnt * deck_suit_cnt * deck_deck_cnt;
	gamedeck = (card_t *) alloc(deck_size * sizeof(card_t));

	cardnum = 0;
	for (deck = 0; deck < deck_deck_cnt; deck++)
		for (suit = 0; suit < deck_suit_cnt; suit++)
			for (face = 0; face < deck_face_cnt; face++) {
				card.face = deck_faces[face];
				card.suit = deck_suits[suit];
				card.deck = deck_decks[deck];
				gamedeck[cardnum] = card;
				cardnum++;
			}

	ggz_debug("Built a deck of size %d.", deck_size);
}

void cards_destroy_deck()
{
	if (gamedeck == NULL)
		ggz_debug
			("SERVER BUG: cards_destroy_deck called on a NULL deck.");
	free(gamedeck);
	deck_size = 0;
	deck_ptr = -1;
}

/* cards_shuffle_deck()
 *   shuffle the deck
 */
void cards_shuffle_deck()
{
	card_t temp;
	int i, j;

	ggz_debug("Shuffling deck.");

	/* Now we can randomize the deck order */
	/* Go through the deck, card by card */
	for (i = 0; i < deck_size; i++) {
		/* Pick any OTHER card */
		do {
			j = random() % deck_size;
		} while (j == i);
		/* And swap positions */
		temp = gamedeck[i];
		gamedeck[i] = gamedeck[j];
		gamedeck[j] = temp;
	}

	deck_ptr = 0;
}

/* cards_deal_hand
 *   This deals the cards out from a pre-shuffled deck into a hand structure
 *
 *   Game-nonspecific
 */
void cards_deal_hand(int handsize, hand_t * hand)
{
	int c;
	card_t card;

	ggz_debug("\tDealing out a hand of size %d.", handsize);

	if (hand == NULL) {
		ggz_debug("SERVER BUG: can't deal NULL hand.");
		exit(-1);
	}

	if (gamedeck == NULL) {
		ggz_debug("SERVER BUG: can't deal from NULL deck.");
		exit(-1);
	}

	/* Deal the cards out */
	hand->hand_size = hand->full_hand_size = handsize;
	for (c = 0; c < hand->hand_size; c++) {
		if (deck_ptr >= deck_size) {
			ggz_debug
				("SERVER BUG: too many cards being dealt out.");
			exit(-1);
		}
		card = gamedeck[deck_ptr];
		hand->cards[c] = card;
		deck_ptr++;
	}

	cards_sort_hand(hand);
}

int compare_cards(const void *c1, const void *c2)
{
	register card_t card1 = game.funcs->map_card(*(card_t *) c1);
	register card_t card2 = game.funcs->map_card(*(card_t *) c2);
	return game.funcs->compare_cards(card1, card2);
}

void cards_sort_hand(hand_t * hand)
{
	/* sort cards -- this should be in another function
	 * so it can be done later as well */
	qsort(hand->cards, hand->hand_size, sizeof(card_t), compare_cards);
}


/* cards_deal_card
 *   This deals one card out from the deck
 *
 *  Game-nonspecific
 */
card_t cards_deal_card()
{
	card_t card;
	if (deck_ptr >= deck_size) {
		ggz_debug("SERVER BUG: too many cards being handed out.");
		exit(-1);
	}
	card = gamedeck[deck_ptr];
	deck_ptr++;
	return card;
}

/* cards_suit_in_hand?
 *   This checks to see how many of a suit is contained in a hand
 */
int cards_suit_in_hand(hand_t * hand, char suit)
{
	int i, cnt = 0;

	for (i = 0; i < hand->hand_size; i++) {
		card_t card = game.funcs->map_card(hand->cards[i]);
		if (card.suit == suit)
			cnt++;
	}

	return cnt;
}


/* cards_highest_in_suit
 *   This checks a hand for the highest card value in the requested suit
 *   it returns the numeric value of the card (1-13)
 */
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

int cards_equal(card_t c1, card_t c2)
{
	/* not to take any chances with filler space... */
	return (c1.face == c2.face && c1.suit == c2.suit
		&& c1.deck == c2.deck);
}

/* none of this should go here, but that's okay for now */
#include "easysock.h"

int es_read_card(int fd, card_t * card)
{
	if (es_read_char(fd, &card->face) < 0)
		return -1;
	if (es_read_char(fd, &card->suit) < 0)
		return -1;
	if (es_read_char(fd, &card->deck) < 0)
		return -1;
	return 0;
}

int es_write_card(int fd, card_t card)
{
	if (es_write_char(fd, card.face) < 0)
		return -1;
	if (es_write_char(fd, card.suit) < 0)
		return -1;
	if (es_write_char(fd, card.deck) < 0)
		return -1;
	return 0;
}
