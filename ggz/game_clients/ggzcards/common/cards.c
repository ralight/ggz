/*
 * File: cards.c
 * Author: Jason Short
 * Project: GGZCards Server/Client
 * Date: 02/25/2002
 * Desc: Card data for the GGZCards client and server
 * $Id: cards.c 3989 2002-04-15 05:29:20Z jdorje $
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

const card_t UNKNOWN_CARD = {face: UNKNOWN_FACE,
                             suit: UNKNOWN_SUIT,
                             deck: UNKNOWN_DECK,
                             type: CARDSET_FRENCH};

int is_valid_card(card_t card)
{
	if (card.type == CARDSET_FRENCH
	    && (card.face == UNKNOWN_FACE
	        || (card.face >= ACE_LOW && card.face <= ACE_HIGH))
	    && (card.suit == UNKNOWN_SUIT
	       || (card.suit >= CLUBS && card.suit <= SPADES))
	    && (card.deck == UNKNOWN_DECK
	       || (card.deck >= 0 && card.deck <= 1 /* ? */)))
		return TRUE;
	
	return FALSE;
}


int are_cards_equal(card_t card1, card_t card2)
{
	return card1.suit == card2.suit
	       && card1.face == card2.face
	       && card1.deck == card2.deck
	       /* && card1.type == card2.type */;
}

const char *suit_names[4] = { "clubs", "diamonds", "hearts", "spades" };
const char *short_suit_names[4] = { "C", "D", "H", "S" };
const char *face_names[15] =
	{ NULL, "ace", "two", "three", "four", "five", "six", "seven",
	"eight", "nine", "ten", "jack", "queen", "king", "ace"
};
const char *short_face_names[15] =
	{ NULL, "A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q",
	"K", "A"
};
