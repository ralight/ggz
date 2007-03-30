/* 
 * File: deck.h
 * Author: Rich Gade, Jason Short
 * Project: GGZCards Server
 * Date: 08/14/2000 (as cards.h)
 * Desc: Various useful deck manipulation routines for card games
 * $Id: deck.h 9016 2007-03-30 00:09:05Z jdorje $
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

#ifndef __DECK_H__
#define __DECK_H__

#include "protocol.h"		/* contains actual card structures */

/* A deck structure.  It's declared in deck.c. */
typedef struct deck_t deck_t;

/* there should be an entry here for every kind of deck, and cards.c should
   be able to handle them. */
typedef enum deck_type_t {
	GGZ_DECK_FULL,		/* normal deck; all 52 cards */
	GGZ_DECK_DOUBLE,	/* double deck; 104 cards */
	GGZ_DECK_SKAT,		/* A-7 in each suit */
	GGZ_DECK_SUARO,		/* A-8 in each suit */
	GGZ_DECK_EUCHRE,	/* A-9 in each suit */
	GGZ_DECK_LAPOCHA,	/* missing 9, 8, 7 */
	GGZ_DECK_SUECA,		/* missing 10, 9, 8 */
	GGZ_DECK_DOMINOES,	/* A standard double-sixes dominoes deck */
} deck_type_t;

/** Create a deck of the given type. */
deck_t *create_deck(deck_type_t which_deck);

/** Deallocate the given deck. */
void destroy_deck(deck_t * deck);

/** Return the size (number of cards in) the (full) deck. */
int get_deck_size(deck_t * deck);

/** Return all cards to the deck and shuffle it. */
void shuffle_deck(deck_t * deck);

/** Deal out a hand from the deck. */
void deal_hand(deck_t * deck, int handsize, hand_t * hand);

/** Deal out a single card. */
card_t deal_card(deck_t * deck);

/** Pull a single card from the deck, then return it. */
/* card_t deck_cut(deck_t * deck); */


/*
 * The rest don't rightfully belong here...
 */

void cards_sort_hand(hand_t * hand);

int cards_suit_in_hand(hand_t * hand, char suit);

char cards_highest_in_suit(hand_t * hand, char suit);

unsigned int myrand(unsigned int size);

#endif /* __DECK_H__ */
