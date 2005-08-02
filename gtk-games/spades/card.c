/*
 * File: card.c
 * Author: Brent Hendricks
 * Project: NetSpades
 * Date: 3/24/98
 *
 * This file contains the functions which operate on the type card. See the
 * header file card.h for a better description of these functions.  
 * 
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

#ifdef HAVE_CONFIG_H
#  include <config.h>	/* Site-specific config */
#endif

#include <card.h>
#include <stdlib.h>


/* Return the face value from 1..13 */
int card_face(Card c)
{
	return (c ==
		BLANK_CARD) ? BLANK_CARD : ((c % 13 ==
					     12) ? 1 : (c % 13 + 2));
}


/* Return the suit as an int */
int card_suit_num(Card c)
{
	return (c == BLANK_CARD) ? BLANK_CARD : (int)(c / 13);
}


/* Return the suit as a single char */
char card_suit_char(Card c)
{

	switch (card_suit_num(c)) {
	case 0:
		return 'H';
	case 1:
		return 'C';
	case 2:
		return 'D';
	case 3:
		return 'S';
	default:
		return -1;
	}
}

/* Return the suit as a char string */
const char *card_suit_str(Card c)
{

	switch (card_suit_num(c)) {
	case 0:
		return ("Hearts");
	case 1:
		return ("Clubs");
	case 2:
		return ("Diamonds");
	case 3:
		return ("Spades");
	default:
		return (NULL);
	}
}

/* Return the name of the card as a string or in code */
const char *card_name(Card c, unsigned char format)
{

	switch (c) {
	case -1:
		return (format == SHORT_NAME) ? "nil" : "Blank Card";
	case 0:
		return (format == SHORT_NAME) ? "2H" : "Two of Hearts";
	case 1:
		return (format == SHORT_NAME) ? "3H" : "Three of Hearts";
	case 2:
		return (format == SHORT_NAME) ? "4H" : "Four of Hearts";
	case 3:
		return (format == SHORT_NAME) ? "5H" : "Five of Hearts";
	case 4:
		return (format == SHORT_NAME) ? "6H" : "Six of Hearts";
	case 5:
		return (format == SHORT_NAME) ? "7H" : "Seven of Hearts";
	case 6:
		return (format == SHORT_NAME) ? "8H" : "Eight of Hearts";
	case 7:
		return (format == SHORT_NAME) ? "9H" : "Nine of Hearts";
	case 8:
		return (format == SHORT_NAME) ? "10H" : "Ten of Hearts";
	case 9:
		return (format == SHORT_NAME) ? "JH" : "Jack of Hearts";
	case 10:
		return (format == SHORT_NAME) ? "QH" : "Queen of Hearts";
	case 11:
		return (format == SHORT_NAME) ? "KH" : "King of Hearts";
	case 12:
		return (format == SHORT_NAME) ? "AH" : "Ace of Hearts";
	case 13:
		return (format == SHORT_NAME) ? "2C" : "Two of Clubs";
	case 14:
		return (format == SHORT_NAME) ? "3C" : "Three of Clubs";
	case 15:
		return (format == SHORT_NAME) ? "4C" : "Four of Clubs";
	case 16:
		return (format == SHORT_NAME) ? "5C" : "Five of Clubs";
	case 17:
		return (format == SHORT_NAME) ? "6C" : "Six of Clubs";
	case 18:
		return (format == SHORT_NAME) ? "7C" : "Seven of Clubs";
	case 19:
		return (format == SHORT_NAME) ? "8C" : "Eight of Clubs";
	case 20:
		return (format == SHORT_NAME) ? "9C" : "Nine of Clubs";
	case 21:
		return (format == SHORT_NAME) ? "10C" : "Ten of Clubs";
	case 22:
		return (format == SHORT_NAME) ? "JC" : "Jack of Clubs";
	case 23:
		return (format == SHORT_NAME) ? "QC" : "Queen of Clubs";
	case 24:
		return (format == SHORT_NAME) ? "KC" : "King of Clubs";
	case 25:
		return (format == SHORT_NAME) ? "AC" : "Ace of Clubs";
	case 26:
		return (format == SHORT_NAME) ? "2D" : "Two of Diamonds";
	case 27:
		return (format == SHORT_NAME) ? "3D" : "Three of Diamonds";
	case 28:
		return (format == SHORT_NAME) ? "4D" : "Four of Diamonds";
	case 29:
		return (format == SHORT_NAME) ? "5D" : "Five of Diamonds";
	case 30:
		return (format == SHORT_NAME) ? "6D" : "Six of Diamonds";
	case 31:
		return (format == SHORT_NAME) ? "7D" : "Seven of Diamonds";
	case 32:
		return (format == SHORT_NAME) ? "8D" : "Eight of Diamonds";
	case 33:
		return (format == SHORT_NAME) ? "9D" : "Nine of Diamonds";
	case 34:
		return (format == SHORT_NAME) ? "10D" : "Ten of Diamonds";
	case 35:
		return (format == SHORT_NAME) ? "JD" : "Jack of Diamonds";
	case 36:
		return (format == SHORT_NAME) ? "QD" : "Queen of Diamonds";
	case 37:
		return (format == SHORT_NAME) ? "KD" : "King of Diamonds";
	case 38:
		return (format == SHORT_NAME) ? "AD" : "Ace of Diamonds";
	case 39:
		return (format == SHORT_NAME) ? "2S" : "Two of Spades";
	case 40:
		return (format == SHORT_NAME) ? "3S" : "Three of Spades";
	case 41:
		return (format == SHORT_NAME) ? "4S" : "Four of Spades";
	case 42:
		return (format == SHORT_NAME) ? "5S" : "Five of Spades";
	case 43:
		return (format == SHORT_NAME) ? "6S" : "Six of Spades";
	case 44:
		return (format == SHORT_NAME) ? "7S" : "Seven of Spades";
	case 45:
		return (format == SHORT_NAME) ? "8S" : "Eight of Spades";
	case 46:
		return (format == SHORT_NAME) ? "9S" : "Nine of Spades";
	case 47:
		return (format == SHORT_NAME) ? "10S" : "Ten of Spades";
	case 48:
		return (format == SHORT_NAME) ? "JS" : "Jack of Spades";
	case 49:
		return (format == SHORT_NAME) ? "QS" : "Queen of Spades";
	case 50:
		return (format == SHORT_NAME) ? "KS" : "King of Spades";
	case 51:
		return (format == SHORT_NAME) ? "AS" : "Ace of Spades";
	default:
		return (NULL);
	}
}

/* Decide which card (if either) beats the other */
int card_comp(Card a, Card b)
{

	if (b == BLANK_CARD)
		return 1;
	else if (a == BLANK_CARD)
		return -1;
	else if (card_suit_num(a) == card_suit_num(b))
		return a - b;
	else if (card_suit_num(a) == SPADES)
		return 1;
	else if (card_suit_num(b) == SPADES)
		return -1;
	else
		return 0;
}
