/* 
 * File: protocol.c
 * Author: Jason Short
 * Project: GGZCards Server/Client
 * Date: 06/26/2001
 * Desc: Enumerations for the ggzcards client-server protocol
 * $Id: protocol.c 3701 2002-03-28 03:22:32Z jdorje $
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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <assert.h>
#include <stdlib.h>

#include <ggz.h>		/* for easysock */

#include "protocol.h"

const char* get_server_opcode_name(server_msg_t opcode)
{
	switch (opcode) {
	case REQ_NEWGAME:  return "REQ_NEWGAME";
	case MSG_NEWGAME:  return "MSG_NEWGAME";
	case MSG_GAMEOVER: return "MSG_GAMEOVER";
	case MSG_PLAYERS:  return "MSG_PLAYERS";
	case REQ_OPTIONS:  return "REQ_OPTIONS";
	case MSG_NEWHAND:  return "MSG_NEWHAND";
	case MSG_HAND:     return "MSG_HAND";
	case REQ_PLAY:     return "REQ_PLAY";
	case MSG_BADPLAY:  return "MSG_BADPLAY";
	case MSG_PLAY:     return "MSG_PLAY";
	case MSG_TRICK:    return "MSG_TRICK";
	case REQ_BID:      return "REQ_BID";
	case MSG_BID:      return "MSG_BID"; 	
	case MSG_TABLE:    return "MSG_TABLE";
	case MESSAGE_GAME: return "MESSAGE_GAME";
	}
	return "[unknown]";
}

const char* get_game_message_name(game_message_t opcode)
{
	switch (opcode) {
	case GAME_MESSAGE_TEXT:     return "GAME_MESSAGE_TEXT";
	case GAME_MESSAGE_PLAYER:   return "GAME_MESSAGE_PLAYER";
	case GAME_MESSAGE_CARDLIST: return "GAME_MESSAGE_CARDLIST";
	case GAME_MESSAGE_GAME:     return "GAME_MESSAGE_GAME"; 	
	}
	return "[unknown]";
}

const char* get_client_opcode_name(client_msg_t opcode)
{
	switch (opcode) {
	case MSG_LANGUAGE: return "MSG_LANGUAGE";
	case RSP_NEWGAME:  return "RSP_NEWGAME";
	case RSP_OPTIONS:  return "RSP_OPTIONS";
	case RSP_PLAY:     return "RSP_PLAY";
	case RSP_BID:      return "RSP_BID";
	case REQ_SYNC:     return "REQ_SYNC"; 	
	}
	return "[unknown]";
}



int read_card(int fd, card_t * card)
{
	if (ggz_read_char(fd, &card->type) < 0 ||
	    ggz_read_char(fd, &card->face) < 0 ||
	    ggz_read_char(fd, &card->suit) < 0 ||
	    ggz_read_char(fd, &card->deck) < 0)
		return -1;
		
	/* We go ahead and check the card for validity. */
	if (is_valid_card(*card))
		return 0;
	
#if 0 /* This could be dangerous - anyone could crash us! */
	assert(FALSE);
#endif

#if 0 /* This probably makes the most sense, but... */
	return -1;
#endif

	*card = UNKNOWN_CARD;
	return 0;
}

int write_card(int fd, card_t card)
{
	/* Check for validity. */
	assert(is_valid_card(card));
	
	/* Note - the "type" should theoretically come first (and does, here),
	   but we list it last in the struct definition for
	   backwards-compatibility. */
	
	if (ggz_write_char(fd, card.type) < 0 ||
	    ggz_write_char(fd, card.face) < 0 ||
	    ggz_write_char(fd, card.suit) < 0 ||
	    ggz_write_char(fd, card.deck) < 0)
		return -1;
	return 0;
}

int read_bid(int fd, bid_t * bid)
{
	if (ggz_read_char(fd, &bid->sbid.val) < 0 ||
	    ggz_read_char(fd, &bid->sbid.suit) < 0 ||
	    ggz_read_char(fd, &bid->sbid.spec) < 0 ||
	    ggz_read_char(fd, &bid->sbid.spec2) < 0)
		return -1;
	return 0;
}

/** @brief Writes a bid to the socket.
 *  @param fd The file descriptor to which to read.
 *  @param bid A pointer to the bid data.
 *  @return 0 on success, -1 on failure. */
int write_bid(int fd, bid_t bid)
{
	if (ggz_write_char(fd, bid.sbid.val) < 0 ||
	    ggz_write_char(fd, bid.sbid.suit) < 0 ||
	    ggz_write_char(fd, bid.sbid.spec) < 0 ||
	    ggz_write_char(fd, bid.sbid.spec2) < 0)
		return -1;
	return 0;
}

int read_opcode(int fd, int *opcode)
{
	char op;
	if (ggz_read_char(fd, &op) < 0)
		return -1;
	*opcode = op;
	return 0;
}

int write_opcode(int fd, int opcode)
{
	char op = opcode;
	assert(opcode >= 0 && opcode < 128);
	return ggz_write_char(fd, op);
}

int read_seat(int fd, int *seat)
{
	char s;
	if (ggz_read_char(fd, &s) < 0)
		return -1;
	*seat = s;
	return 0;
}

int write_seat(int fd, int seat)
{
	char s = seat;
	assert(seat >= 0 && seat < 127);
	return ggz_write_char(fd, s);
}
