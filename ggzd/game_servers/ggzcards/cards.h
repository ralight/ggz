/*
 * File: cards.h
 * Author: Rich Gade, Jason Short
 * Project: GGZCards GGZ Server
 * Date: 08/14/2000
 * Desc: Various useful deck manipulation routines for card games
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

/* a card */
typedef struct card_t {
#define UNKNOWN_FACE -1
#define JACK 11
#define QUEEN 12
#define KING 13
#define ACE_HIGH 14
#define ACE_LOW 1
#define JOKER1 0
#define JOKER2 1
	char face; /* -1: unknown, 1-13: A-K, 14: A
	            *	             for "none" suit, 0-1 are jokers */
#define UNKNOWN_SUIT -1
#define CLUBS 0
#define DIAMONDS 1
#define HEARTS 2
#define SPADES 3
#define NO_SUIT 4	
	char suit; /* -1: unknown, 0-3: clubs-spades; 4: none */
#define UNKNOWN_DECK -1
	char deck; /* -1: unknown, 0+: the deck number */
} card_t;
#define UNKNOWN_CARD (card_t){-1, -1, -1}

extern char* suit_names[4];

/* A hand structure */
typedef struct hand_t {
	char hand_size;
	card_t *cards; /* must be allocated; memory leaks will be rampant! */
} hand_t;

/* there should be an entry here for every kind of deck,
 * and cards.c should be able to handle them. */
#define GGZ_DECK_FULL		1 /* all 52 cards */
#define GGZ_DECK_LAPOCHA	2 /* missing 10, 9, 8 */
#define GGZ_DECK_SUARO		3 /* A-8 in each suit */
#define GGZ_DECK_EUCHRE		4 /* A-9 in each suit */
#define GGZ_DECK_SKAT		5 /* A-7 in each suit */

/* Exported functions */
extern void cards_create_deck(int which_deck);
extern void cards_destroy_deck();
extern void cards_shuffle_deck(void);
extern void cards_deal_hand(int handsize, hand_t *hand);
extern card_t cards_deal_card(); /* deals one card */
extern char cards_cut_for_trump(void);
extern int cards_suit_in_hand(hand_t *, char);
extern char cards_highest_in_suit(hand_t *, char);

extern int es_read_card(int, card_t *);
extern int es_write_card(int, card_t);

#define GGZ_CARDS_INCLUDED
#endif /*GGZ_CARDS_INCLUDED*/
