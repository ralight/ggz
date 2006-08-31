/*
 * File: cards.h
 * Author: Jason Short
 * Project: GGZCards Server/Client
 * Date: 02/25/2002
 * Desc: Card data for the GGZCards client and server
 * $Id: cards.h 8558 2006-08-31 06:34:19Z jdorje $
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

#ifndef __CARDS_H__
#define __CARDS_H__

/** @brief The type of deck.
 *
 *  In theory, many different types of decks are possible.  Currently
 *  only the standard French deck is used.
 */
typedef enum {
	UNKNOWN_CARDSET = -1,

	/** A standard (French) card deck (A/K/Q/J/10..2). */
	CARDSET_FRENCH,

	/** A set of dominoes.  Here each domino has two sides, each of
	 *  which has a number.  For storage purposes, the higher
	 *  side will be considered the "suit" and the lower side the
	 *  "face". */
	CARDSET_DOMINOES
} cardset_type_t;

void set_cardset_type(cardset_type_t cardset_type);
cardset_type_t get_cardset_type(void);

/*
 * Card structure - must be the same between server and client
 */

/** Regular values for card faces.
 *  @note If a player does not know the card face, UNKNOWN will be sent.
 *  @note Values 2-10 have their face value.
 *  @see card_t::face
 *  @note This only applies for French (standard) decks.
 */
enum card_face_enum {
	UNKNOWN_FACE = -1,	/**< An unknown face on a card */
	JACK = 11,		/**< A "jack" card face */
	QUEEN = 12,		/**< A "queen" card face */
	KING = 13,		/**< A "king" card face */
	ACE_HIGH = 14,		/**< An "ace" (high) card face */
	ACE_LOW = 1,		/**< An "ace" (low) card face */
	JOKER1 = 0,		/**< A "joker 1" card face */
	JOKER2 = 1		/**< A "joker 2" card face */
};

/** Regular values for card suits.
 *  @note If a player does not know the card suit, UNKNOWN will be sent.
 *  @note For dominoes games, regular numbers (0-6) are used.
 *  @see card_t::suit */
enum card_suit_enum {
	UNKNOWN_SUIT = -1,	/**< An unknown suit of a card */
	CLUBS = 0,		/**< The clubs (lowest) suit */
	DIAMONDS = 1,		/**< The diamonds (second) suit */
	HEARTS = 2,		/**< The hearts (third) suit */
	SPADES = 3,		/**< The spades (highest) suit */
	NO_SUIT = -2		/**< A no-suit used for jokers, etc. */
};

/** Regular values for card decks.
 *  @note If a player does not know the card deck, UNKNOWN will be sent.
 *  @note Values 0+ are used for normal known decks.
 *  @see card_t::deck */
enum card_deck_enum {
	UNKNOWN_DECK = -1	/**< An unknown deck of a card */
};

/** A card. */
typedef struct card_t {
	/** The face of the card.
	 *  @see card_face_enum */
	char face;
	/** The suit of the card.
	 *  @see card_suit_enum */
	char suit;
	/** The deck number of the card.
	 *  @see card_deck_enum */
	char deck;
} card_t;

const char *get_suit_name(char suit);
const char *get_short_suit_name(char suit);
const char *get_face_name(char face);
const char *get_short_face_name(char face);

int are_cards_equal(card_t card1, card_t card2);
int is_valid_card(card_t card);


/* A hand structure */
typedef struct hand_t {
	/* the size of the hand, when it's full.  This may or may not
	   actually be used (client-side it is currently unused). */
	int full_hand_size;

	/* the current size of the hand (number of cards currently in
	   the hand. */
	int hand_size;

	/* A variable-sized array containing the cards present. */
	card_t *cards;
} hand_t;


/* in different games, bids may have different meanings.  we'll just use this
   arbitrary data structure for it */
/* This shouldn't really go here, but... */
typedef union bid_t {
	int bid;		/* DO NOT USE */
	struct special_bid_struct {
		/* this can be used for many different games that have
		   unusual but similar bidding. Different games may use it
		   differently. */
		char val;	/* the value of the bid */
		char suit;	/* the suit of the bid (generally trump) */
		char spec;	/* specialty bids (defined per-game) */
		char spec2;	/* More specialty bids (just to round things out) */
	} sbid;
} bid_t;


/** An entirely unknown card. */
extern const card_t UNKNOWN_CARD;

#endif /* __CARDS_H__ */
