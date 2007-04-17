/* 
 * File: protocol.c
 * Author: Jason Short
 * Project: GGZCards Server/Client
 * Date: 06/26/2001
 * Desc: Enumerations for the ggzcards client-server protocol
 * $Id: protocol.c 9053 2007-04-17 03:16:36Z jdorje $
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

#include "protocol.h"

const char *get_server_opcode_name(server_msg_t opcode)
{
	switch (opcode) {
	case REQ_NEWGAME:
		return "REQ_NEWGAME";
	case MSG_NEWGAME:
		return "MSG_NEWGAME";
	case MSG_GAMEOVER:
		return "MSG_GAMEOVER";
	case MSG_PLAYERS:
		return "MSG_PLAYERS";
	case MSG_SCORES:
		return "MSG_SCORES";
	case MSG_TRICKS_COUNT:
		return "MSG_TRICKS_COUNT";
	case REQ_OPTIONS:
		return "REQ_OPTIONS";
	case MSG_NEWHAND:
		return "MSG_NEWHAND";
	case MSG_TRUMP:
		return "MSG_TRUMP";
	case MSG_HAND:
		return "MSG_HAND";
	case MSG_PLAYERS_STATUS:
		return "MSG_PLAYERS_STATUS";
	case REQ_PLAY:
		return "REQ_PLAY";
	case MSG_BADPLAY:
		return "MSG_BADPLAY";
	case MSG_PLAY:
		return "MSG_PLAY";
	case MSG_TRICK:
		return "MSG_TRICK";
	case REQ_BID:
		return "REQ_BID";
	case MSG_BID:
		return "MSG_BID";
	case MSG_TABLE:
		return "MSG_TABLE";
	case MSG_GAME_MESSAGE_TEXT:
		return "MSG_GAME_MESSAGE_TEXT";
	case MSG_GAME_MESSAGE_PLAYER:
		return "MSG_GAME_MESSAGE_PLAYER";
	case MSG_GAME_MESSAGE_CARDLIST:
		return "MSG_GAME_MESSAGE_CARDLIST";
	case MSG_GAME_SPECIFIC:
		return "MSG_GAME_SPECIFIC";
	}
	return "[unknown]";
}

const char *get_client_opcode_name(client_msg_t opcode)
{
	switch (opcode) {
	case MSG_LANGUAGE:
		return "MSG_LANGUAGE";
	case RSP_NEWGAME:
		return "RSP_NEWGAME";
	case RSP_OPTIONS:
		return "RSP_OPTIONS";
	case RSP_PLAY:
		return "RSP_PLAY";
	case RSP_BID:
		return "RSP_BID";
	case REQ_SYNC:
		return "REQ_SYNC";
	case REQ_OPEN_HAND:
		return "REQ_OPEN_HAND";
	}
	return "[unknown]";
}
