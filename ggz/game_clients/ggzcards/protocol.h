/* 
 * File: protocol.h
 * Author: Jason Short
 * Project: GGZCards Server/Client
 * Date: 06/26/2001
 * Desc: Enumerations for the ggzcards client-server protocol
 * $Id: protocol.h 3318 2002-02-11 06:40:29Z jdorje $
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

/* Note: in the below, we refer to "opcodes", "cards", and "seat numbers" (or 
   "player numbers"). These are sent and received in a ggzcards-chosen format 
   by the protocol functions at the bottom of this file. We'll also refer to
   data in the form of a "char", "int", or "string".  These will be sent in
   easysock format (see the easysock library for more on this). */

/* Messages from server */
typedef enum {
	/* Requests a newgame response (RSP_NEWGAME) from the client.  No
	   data. */
	REQ_NEWGAME,

	/* Tells the client of a start of a new game.  No data. */
	MSG_NEWGAME,

	/* Tells the client of a gameover situation.  It'll be followed by an 
	   integer n followed by a list of n players who are the winners of
	   the game. */
	MSG_GAMEOVER,

	/* Tells the client the list of players.  It'll be followd by an
	   integer n followed by data for all n players.  For each player, an 
	   integer giving the seat status (GGZSeat) and a string for the
	   player's name will be sent.  Note that the names may be invalid
	   (but never null) for OPEN seats. */
	MSG_PLAYERS,

	/* Requests options from the client.  It is followed by an integer n
	   followed by n option requests.  Each option request consists of an 
	   integer m followed by m strings representing the option choices.
	   For each of the n options, the client must choose one of the m
	   choices and send this back to the server with a RSP_OPTIONS.  If
	   m==1 then the option is considered "boolean" and either a 0 or 1
	   may be sent.  It needs a RSP_OPTIONS in response. */
	REQ_OPTIONS,

	/* Tells the client of a player's hand.  It'll be followed by a seat
	   # for whom the hand belongs to, followed by an integer n for the
	   hand size, followed by n cards. */
	MSG_HAND,

	/* Requests a play (of a card) from the client.  It'll be followd by
	   the seat # of the hand from which the client is supposed to play
	   (only a few games, like bridge, ever require a player to play from 
	   a hand that is not their own).  The client should send a RSP_PLAY
	   in response. */
	REQ_PLAY,

	/* Tells the player that they have made a bad play.  It is followed
	   by a string that is the error message.  It needs a RSP_PLAY in
	   response. */
	MSG_BADPLAY,

	/* Tells the client of a player playing a card onto the table.  It is 
	   followed by a seat # for the seat from which the card has been
	   played, then the card that was played from their hand. */
	MSG_PLAY,

	/* Tells the client of the end of a trick.  It is followed by a seat
	   # giving the player who won the trick. */
	MSG_TRICK,

	/* Requests a bid from the client.  It'll be followed by an integer
	   n, then n strings.  The client must choose one of these strings
	   and send a RSP_BID in response. */
	REQ_BID,

	/* Sends out the current table to the client.  It is followed by a
	   card for each player, in order.  Note that only one card per
	   player/seat may be on the table at a time. */
	MSG_TABLE,

	/* A game message.  See game_message_t below. */
	MESSAGE_GAME,
} server_msg_t;

/** @brief Return a string description of the opcode. */
const char* get_server_opcode_name(server_msg_t opcode);

/* Global message types */
/* Each MESSAGE_GAME will be followed by one of these opcodes.  This
   represents game-specific data being transmitted to the frontend. */
typedef enum {
	/* A simple text message, containing two strings: a "marker" and the
	   "message". */
	GAME_MESSAGE_TEXT,

	/* A text message for the player, consisting of a seat # followed by
	   a message string. */
	GAME_MESSAGE_PLAYER,

	/* A list of cards for each player.  First comes a "marker" string,
	   then (for each player) an integer n plus n cards for that player. */
	GAME_MESSAGE_CARDLIST,

	/* Block data that may be game-specific.  An integer representing the 
	   game followed by an integer n followed by n bytes of data. It is
	   up to the client frontend to determine what (if anything) to do
	   with this data; it'll be build at the server end by the game
	   module. */
	GAME_MESSAGE_GAME,
} game_message_t;

/** @brief Return a string description of the opcode. */
const char* get_game_message_name(game_message_t opcode);

/* Messages from client */
typedef enum {
	/* A newgame response, sent in response to a REQ_NEWGAME.  No data. */
	RSP_NEWGAME,

	/* An options response, sent in response to a REQ_OPTIONS.  It
	   consists of a list of n integers, each representing an option
	   selection (see REQ_OPTIONS). */
	RSP_OPTIONS,

	/* A play response, sent in response to a REQ_PLAY.  It is followed
	   by a card that the client/user wishes to play. */
	RSP_PLAY,

	/* A bid response, sent in response to a REQ_BID.  It consists of
	   only an integer giving the chosen bid (see REQ_BID). */
	RSP_BID,

	/* A sync request.  The server will send out all data again to sync
	   the client. */
	REQ_SYNC
} client_msg_t;

/** @brief Return a string description of the opcode. */
const char* get_client_opcode_name(client_msg_t opcode);


/* 
 * Card structure - must be the same between server and client
 */

/** Regular values for card faces.
 *  @note If a player does not know the card face, UNKNOWN will be sent.
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
 *  @note If a player does not know the card suit, UNKNOWN will be sent.
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

/** An entirely unknown card. */
extern const card_t UNKNOWN_CARD;

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
