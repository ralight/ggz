/*
 * File: cards.h
 * Author: Rich Gade
 * Project: LaPocha GGZ Server
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

/* A hand structure */
struct hand_t {
	char hand_size;
	char card[13];
};

/* Deck types */
#define GGZ_DECK_STANDARD	1
#define GGZ_DECK_LAPOCHA	2
#define GGZ_MAX_DECKSIZE	52

/* Exported functions */
extern void cards_shuffle_deck(char);
extern void cards_deal_hands(int, int, struct hand_t *);
extern char cards_cut_for_trump(void);
extern char cards_suit_in_hand(struct hand_t *, char);
extern char cards_highest_in_suit(struct hand_t *, char);
extern char card_value(char);

#define GGZ_CARDS_INCLUDED
#endif /*GGZ_CARDS_INCLUDED*/
