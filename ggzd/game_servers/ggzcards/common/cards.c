/*
 * File: cards.c
 * Author: Jason Short
 * Project: GGZCards Server/Client
 * Date: 02/25/2002
 * Desc: Card data for the GGZCards client and server
 * $Id: cards.c 10190 2008-07-08 03:32:44Z jdorje $
 *
 * This contains card definitions common to both GGZCards client
 * and server.
 *
 * It should be identical between the server and all clients, and
 * the server contains the master copy.
 *
 * Copyright (C) 2002 Brent Hendricks.
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
#  include <config.h>
#endif

#include <stdlib.h>

#include "cards.h"
#include "shared.h"

static cardset_type_t cardset_type = UNKNOWN_CARDSET;

void set_cardset_type(cardset_type_t cardset)
{
	cardset_type = cardset;
}

cardset_type_t get_cardset_type(void)
{
	return cardset_type;
}

const card_t UNKNOWN_CARD = {.face = UNKNOWN_FACE,
                             .suit = UNKNOWN_SUIT,
                             .deck = UNKNOWN_DECK};

/* FIXME: a better system for returning names is needed.  It should
   be i18n'able, at least at the client. */

const char *get_suit_name(char suit)
{
	switch (cardset_type) {
	case CARDSET_FRENCH:
		switch (suit) {
		case CLUBS:
			return "clubs";
		case DIAMONDS:
			return "diamonds";
		case HEARTS:
			return "hearts";
		case SPADES:
			return "spades";
		}
		break;
	case CARDSET_DOMINOES:
		switch (suit) {
		case 0:
			return "blanks";
		case 1:
			return "ones";
		case 2:
			return "dueces";
		case 3:
			return "treys";
		case 4:
			return "fours";
		case 5:
			return "fives";
		case 6:
			return "sixes";
		}
		break;
	case UNKNOWN_CARDSET:
		break;
	}
	return "[invalid suit]";
}

const char *get_short_suit_name(char suit)
{
	switch (cardset_type) {
	case CARDSET_FRENCH:
		switch (suit) {
		case CLUBS:
			return "C";
		case DIAMONDS:
			return "D";
		case HEARTS:
			return "H";
		case SPADES:
			return "S";
		}
		break;
	case CARDSET_DOMINOES:
		switch (suit) {
		case 0:
			return "0";
		case 1:
			return "1";
		case 2:
			return "2";
		case 3:
			return "3";
		case 4:
			return "4";
		case 5:
			return "5";
		case 6:
			return "6";
		}
		break;
	case UNKNOWN_CARDSET:
		break;
	}
	return "X";
}

const char *get_face_name(char face)
{
	switch (cardset_type) {
	case CARDSET_FRENCH:
		switch (face) {
		case ACE_LOW:
		case ACE_HIGH:
			return "ace";
		case JACK:
			return "jack";
		case QUEEN:
			return "queen";
		case KING:
			return "king";
		case 2:
			return "two";
		case 3:
			return "three";
		case 4:
			return "four";
		case 5:
			return "five";
		case 6:
			return "six";
		case 7:
			return "seven";
		case 8:
			return "eight";
		case 9:
			return "nine";
		case 10:
			return "ten";
		}
		break;
	case CARDSET_DOMINOES:
		switch (face) {
		case 0:
			return "blank";
		case 1:
			return "one";
		case 2:
			return "duece";
		case 3:
			return "trey";
		case 4:
			return "four";
		case 5:
			return "five";
		case 6:
			return "six";
		}
		break;
	case UNKNOWN_CARDSET:
		break;	
	}
	return "[unknown face]";
}

const char *get_short_face_name(char face)
{
	switch (cardset_type) {
	case CARDSET_FRENCH:
		switch (face) {
		case ACE_LOW:
		case ACE_HIGH:
			return "A";
		case JACK:
			return "J";
		case QUEEN:
			return "Q";
		case KING:
			return "K";
		case 2:
			return "2";
		case 3:
			return "3";
		case 4:
			return "4";
		case 5:
			return "5";
		case 6:
			return "6";
		case 7:
			return "7";
		case 8:
			return "8";
		case 9:
			return "9";
		case 10:
			return "10";
		}
		break;
	case CARDSET_DOMINOES:
		switch (face) {
		case 0:
			return "0";
		case 1:
			return "1";
		case 2:
			return "2";
		case 3:
			return "3";
		case 4:
			return "4";
		case 5:
			return "5";
		case 6:
			return "6";
		}
		break;
	case UNKNOWN_CARDSET:
		break;	
	}
	return "X";
}

int is_valid_card(card_t card)
{
	switch (cardset_type) {
	case UNKNOWN_CARDSET:
		return card.face == UNKNOWN_FACE
		       && card.suit == UNKNOWN_SUIT
		       && card.deck == UNKNOWN_DECK;
	case CARDSET_FRENCH:
		return (card.face == UNKNOWN_FACE
		        || (card.face >= ACE_LOW && card.face <= ACE_HIGH))
		       && (card.suit == UNKNOWN_SUIT
		           || (card.suit >= CLUBS && card.suit <= SPADES))
		       && (card.deck == UNKNOWN_DECK
		           || (card.deck >= 0));
	case CARDSET_DOMINOES:
		if (card.deck != UNKNOWN_DECK && card.deck < 0)
			return FALSE;
		if (card.suit == -1 && card.face == -1)
			return TRUE;
		return card.suit != -1
		       && card.face != -1
		       && card.suit >= card.face;
	}
	return FALSE;
}


int are_cards_equal(card_t card1, card_t card2)
{
	return card1.suit == card2.suit
	       && card1.face == card2.face
	       && card1.deck == card2.deck
	       /* && card1.type == card2.type */;
}
