/* 
 * File: protocol.h
 * Author: Jason Short
 * Project: GGZCards Server/Client
 * Date: 06/26/2001
 * Desc: Enumerations for the ggzcards client-server protocol
 * $Id: protocol.h 2741 2001-11-13 22:52:40Z jdorje $
 *
 * This just contains the communications protocol information.
 *
 * It should be identical between the server and all clients, and
 * the server contains the master copy.
 *
 * Copyright (C) 1999 Brent Hendricks.
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

/* NOTE: This file should only be changed in the server. */

#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#ifdef HAVE_CONFIG
#  include <config.h>		/* Site-specific config */
#endif


/* 
 * Protocol opcodes
 */

/* We don't need to define/check the protocol version since GGZ does this
   automatically.  What we may want to do is include capabilities or a minor
   version number so that slightly different protocols are supported
   compatibly. */

/* Messages from server */
typedef enum {
	REQ_NEWGAME,
	MSG_NEWGAME,
	MSG_GAMEOVER,
	MSG_PLAYERS,
	MSG_HAND,
	REQ_BID,
	REQ_PLAY,
	MSG_BADPLAY,
	MSG_PLAY,
	MSG_TRICK,
	MESSAGE_GLOBAL,
	MESSAGE_PLAYER,
	REQ_OPTIONS,
	MSG_TABLE
} server_msg_t;

/* Global message types */
typedef enum {
	GL_MESSAGE_TEXT,	/* A simple text message, containing only a
				   single string. */
	GL_MESSAGE_CARDLIST,	/* A list of cards for each player, sent as
				   (for each player) a length n plus n cards
				   for that player. */
	GL_MESSAGE_BLOCK,	/* Block data that may be game-specific.  An
				   integer (n) followed by n bytes of data.
				   It is up to the client frontend to
				   determine what (if anything) to do with
				   this data. */
} message_type_t;

/* Messages from client */
typedef enum {
	RSP_NEWGAME,
	RSP_OPTIONS,
	RSP_PLAY,
	RSP_BID,
	REQ_SYNC
} client_msg_t;


/* 
 * Card structure - must be the same between server and client
 */

/** Regular values for card faces.
 *  @note Values 2-10 have their face value.
 *  @see card_t::face */
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
 *  @see card_t::suit */
enum card_suit_enum {
	UNKNOWN_SUIT = -1,	/**< An unknown suit of a card */
	CLUBS = 0,		/**< The clubs (lowest) suit */
	DIAMONDS = 1,		/**< The diamonds (second) suit */
	HEARTS = 2,		/**< The hearts (third) suit */
	SPADES = 3,		/**< The spades (highest) suit */
	NO_SUIT = 4		/**< A no-suit used for jokers, etc. */
};

/** Regular values for card decks.
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

/** An entirely unknown card. */
#define UNKNOWN_CARD (card_t){-1, -1, -1}


/* 
 * Communication functions
 */

/** @brief Reads a card from the socket.
 *  @param fd The file descriptor from which to read.
 *  @param card A pointer to the card data.
 *  @return 0 on success, -1 on failure. */
int read_card(int fd, card_t * card);

/** @brief Wites a card to the socket.
 *  @param fd The file desciptor to which to write.
 *  @param card The card to be written.
 *  @return 0 on success, -1 on failure. */
int write_card(int fd, card_t card);

/** @brief Reads an opcode from the socket.
 *  @param fd The file descriptor from which to read.
 *  @param op A pointer to the opcode data.
 *  @return 0 on success, negative value on failure.
 *  @see enum server_msg_t, enum client_msg_t */
int read_opcode(int fd, int *op);

/** @brief Writes an opcode to the socket.
 *  @param fd The file descriptor to which to write.
 *  @param op The opcode data.
 *  @return 0 on success, negative value on failure.
 *  @see enum server_msg_t, enum client_msg_t */
int write_opcode(int fd, int op);

/** @brief Reads a seat from the socket.
 *  @param fd The file descriptor from which to read.
 *  @param seat A pointer to the seat number.
 *  @return 0 on success, negative value on failure. */
int read_seat(int fd, int *seat);

/** @brief Writes a seat number to the socket.
 *  @param fd The file descriptor to which to write.
 *  @param seat The seat number.
 *  @return 0 on success, negative value on failure. */
int write_seat(int fd, int seat);

#endif /* __PROTOCOL_H__ */
