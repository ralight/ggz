/* 
 * File: protocol.h
 * Author: Jason Short
 * Project: GGZCards Server/Client
 * Date: 06/26/2001
 * Desc: Enumerations for the ggzcards client-server protocol
 * $Id: protocol.h 4077 2002-04-25 08:28:32Z jdorje $
 *
 * This just contains the communications protocol information.
 *
 * OK, I lied.  It's continuously expanded to contain _all_
 * common server-client code.  This will eventually be split up
 * into different files.
 *
 * It should be identical between the server and all clients, and
 * the server contains the master copy.
 *
 * Copyright (C) 2001-2002 Brent Hendricks.
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

#include "cards.h"


/* 
 * Protocol opcodes
 */

/* We don't need to define/check the protocol version since GGZ does this
   automatically.  What we may want to do is include capabilities or a minor
   version number so that slightly different protocols are supported
   compatibly. */

/* Note: in the below, we refer to "opcodes", "cards", "bids", and 
   "seat numbers" (or  "player numbers"). These are sent and received in a
   ggzcards-chosen format by the protocol functions in net_common.h. We'll
   also refer to data in the form of a "char", "int", or "string".  These 
   will be sent in easysock format (see the easysock library for more on 
   this). */

/* Messages from server */
typedef enum {
	/* Requests a newgame response (RSP_NEWGAME) from the client.  No
	   data. */
	REQ_NEWGAME,

	/* Tells the client of a start of a new game.  Followed by an
	   integer containing the cardset type. */
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
	   followed by n option requests.  Each option request consists of a
	   descriptive text for the option, an integer m telling how many
	   choices there are for this option, and integer in the range
	   0..(m-1) giving the default option choice, then  m strings
	   representing the option choices themselves.  For each of the n
	   options, the client must choose one of the m choices and send this
	   back to the server with a RSP_OPTIONS.  If m==1 then the option is
	   considered "boolean" and either a 0 or 1 may be sent.  It needs a
	   RSP_OPTIONS in response. */
	REQ_OPTIONS,
	
	/* Tells the client a new hand is starting.  No data. */
	MSG_NEWHAND,

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
	   n, then n bid choices.  Each bid choice consists of the bid
	   itself (see read_bid/write_bid) and two strings: a short bid
	   text, and a longer bid description.  The client must choose one 
	   of these bids and send a RSP_BID in response. */
	REQ_BID,
	
	/* Tells the client of a player's bid.  It is followed by a seat # for
	   the seat from which the bid comes, then the bid itself. */
	MSG_BID,

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

	/* Block data that may be game-specific.  It is followed by the
	   (string) name of the game, followed by an integer n followed
	   by n bytes of data. It is up to the client frontend to determine
	   what (if anything) to do  with this data; it'll be build at the
	   server end by the game module. */
	GAME_MESSAGE_GAME,
} game_message_t;

/** @brief Return a string description of the opcode. */
const char* get_game_message_name(game_message_t opcode);

/* Messages from client */
typedef enum {
	/* Notifies the server of the client's language.  Followed by a
	   string including the language. */
	MSG_LANGUAGE,

	/* A newgame response, sent in response to a REQ_NEWGAME.  No data. */
	RSP_NEWGAME,

	/* An options response, sent in response to a REQ_OPTIONS.  It
	   consists of a number n, followed by a list of n integers, each
	   representing an option selection.  The number of options n must
	   be what REQ_OPTIONS has requested. */
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

#endif /* __PROTOCOL_H__ */
