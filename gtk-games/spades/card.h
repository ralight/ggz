/*
 * File: card.h
 * Author: Brent Hendricks
 * Project: NetSpades
 * Date: 3/24/98
 *
 * This header defines the card type used in NetSpades. It also declares
 * various functions for operating on cards. Note: some of these functions
 * may be imlpemented as macros.  The cards are repesented by an unsigned
 * char which will take values from 0 to 51.  The value repesents
 * (SUIT)*13+RANK where SUIT ranges from 0-3 for hearts, clubs, diamonds,
 * or spades, respectively.  RANK ranges from 0-12 representing the cards
 * from two to ace, respectively.
 * 
 * Copyright (C) 1998 Brent Hendricks.
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


#ifndef _CARDS_H
#define _CARDS_H

#define LONG_NAME 0
#define SHORT_NAME 1

#define HEARTS 0
#define CLUBS  1
#define DIAMONDS 2
#define SPADES 3

#define JACK   9
#define QUEEN 10
#define KING  11
#define ACE   12

#define BLANK_CARD -1

typedef char Card;

/* Return card corresponding to suit and value*/
#define card_conv(r,s)  ((Card)((r)+(s)*13))

/* Return the rank from 0..12 */
#define card_rank(c)   ((c) % 13)

/* Return the face value from 1..13 */
int card_face(Card);

/* Return the suit as an int */
int card_suit_num(Card);

/* Return the suit as a single char */
char card_suit_char(Card);

/* Return the suit as a char string */
const char *card_suit_str(Card);

/* Return the name of the card as a string or 2 letter code*/
const char *card_name(Card, unsigned char format);

/* Decide which card (if either) beats the other */
int card_comp(Card, Card);

#endif
