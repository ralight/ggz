/* 
 * File: cards.h
 * Author: Rich Gade, Jason Short
 * Project: GGZCards Server
 * Date: 08/14/2000
 * Desc: Various useful deck manipulation routines for card games
 * $Id: cards.h 2418 2001-09-09 03:42:21Z jdorje $
 *
 * This file was originally taken from La Pocha by Rich Gade.
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

#ifndef GGZ_CARDS_INCLUDED

#include <config.h>		/* Site-specific config */

#include "protocol.h"		/* contains actual card structures */

extern char *suit_names[];
extern char *short_suit_names[];
extern char *face_names[];
extern char *short_face_names[];

/* A hand structure */
typedef struct hand_t {
	char full_hand_size;	/* the size of the hand, when it's full */
	char hand_size;		/* the current size of the hand */
	card_t *cards;		/* must be allocated; memory leaks will be
				   rampant! */
} hand_t;

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
} deck_type_t;

/* Exported functions */
void cards_create_deck(deck_type_t which_deck);
int cards_deck_size();
void cards_destroy_deck();
void cards_shuffle_deck(void);
void cards_deal_hand(int handsize, hand_t * hand);
void cards_sort_hand(hand_t * hand);
card_t cards_deal_card();	/* deals one card */
char cards_cut_for_trump(void);
int cards_suit_in_hand(hand_t *, char);
char cards_highest_in_suit(hand_t *, char);

int cards_equal(card_t, card_t);

#define GGZ_CARDS_INCLUDED
#endif /* GGZ_CARDS_INCLUDED */
