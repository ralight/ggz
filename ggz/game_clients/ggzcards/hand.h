/*
 * File: hand.h
 * Author: Rich Gade
 * Project: GGZCards Client
 * Date: 08/14/2000
 * Desc: Functions to get cards from server
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

#ifndef __HAND_H
#define __HAND_H

#define MAX_HAND_SIZE 13

/* a card; this should go in a different file
 * as it's copied verbatim from the server code */
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

#define UNKNOWN_CARD (card_t){-1, -1, -1}

struct hand_t {
	int hand_size; 							/* the number of cards in the hand */
	card_t card[MAX_HAND_SIZE]; /* the list of cards */
	char selected_card;					/* the currently selected (highlighted) card (index) */
	char in_play_card_num;			/* the currently played card (index) */
	card_t in_play_card_val; 		/* ""											"" (value) */
};

extern int hand_read_hand(void);

/* copied directly from server code */
extern int es_read_card(int fd, card_t *card);
extern int es_write_card(int fd, card_t card);

#endif /* __HAND_H */
