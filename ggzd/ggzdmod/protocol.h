/*
 * File: protocols.h
 * Author: Brent Hendricks
 * Project: GGZ
 * Date: 10/18/99
 * Desc: Protocol enumerations, etc.
 * $Id: protocol.h 3108 2002-01-14 00:06:53Z jdorje $
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


#ifndef __GGZ_SERVER_PROTOCOL_H
#define __GGZ_SERVER_PROTOCOL_H

/*
 * Protocols for GGZDMOD game server <-> ggzd communication.
 */

/** Messages sent from the game server to the ggz server. */
typedef enum {
	/** @brief Signals the start of a RSP_GAME_JOIN packet.
	 *
	 *  The packet is composed of:
	 *    - An integer containing RSP_GAME_JOIN.
	 *    - A single byte (char) containing the status;
	 *      a negative status indicates an error.
	 *  The packet is sent in response to a REQ_GAME_JOIN, and
	 *  indicates that the game server has handled the join event.
	 */
	RSP_GAME_JOIN,

	/** @brief Signals the start of a RSP_GAME_LEAVE packet.
	 *
	 *  The packet is composed of:
	 *    - An integer containing RSP_GAME_LEAVE.
	 *    - A single byte (char) containing the status;
	 *      a negative status indicates an error.
	 *  The packet is sent in response to a REQ_GAME_LEAVE, and
	 *  indicates that the game server has handled the leave event.
	 */
	RSP_GAME_LEAVE,

	/** @brief Signals the start of a MSG_LOG packet.
	 *
	 *  The packet is composed of:
	 *    - An integer containing MSG_LOG.
	 *    - An easysock-formatted string.
	 *  The packet may be sent at any time by the game server.  It
	 *  tells ggzd to log the message string.
	 */
	MSG_LOG,

	/** @brief Signals the start of a REQ_GAME_STATE packet.
	 *
	 *  The packet is composed of:
	 *    - An integer containing REQ_GAME_STATE.
	 *    - A single byte (char) containing the new game state.
	 *      The state is a GGZdModState enumerated value.
	 *  This packet tells the server of a change in game state.
	 *  The server will send a RSP_GAME_STATE packet in response.
	 */
	REQ_GAME_STATE,
} TableToControl;

/** Messages sent from the ggz server to the game server. */
typedef enum {
	/** @brief Signals the start of a MSG_GAME_LAUNCH packet.
	 *
	 *  The packet is composed of:
	 *    - An integer containing MSG_GAME_LAUNCH.
	 *    - An integer containing the number of seats at the table.
	 *    - Seat data for every seat at the table:
	 *      - An integer containing the seat type (GGZSeatType).
	 *      - If the seat type is GGZ_SEAT_RESERVED, the name of the
	 *        player at the seat (as an easysock-formatted string).
	 *  This packet is sent to the table shortly after the table
	 *  executable has been launched.  GGZdMod-game, on receiving
	 *  this packet, updates its internal data and changes the
	 *  state from CREATED to WAITING.  It then notifies the table
	 *  and GGZ of these changes.
	 */
	MSG_GAME_LAUNCH,

	/** @brief Signals the start of a REQ_GAME_JOIN packet.
	 *
	 *  The packet is composed of:
	 *    - An integer containing REQ_GAME_JOIN.
	 *    - An integer containing the seat number of the joining
	 *      player.
	 *    - An easysock-formatted string contining the player's name.
	 *    - An easysock-formatted FD containing the FD for the
	 *      player's client socket.
	 *  This packet is sent any time a player joins the table.  It
	 *  gives the table all of the information it needs about the
	 *  player.  The table should send a RSP_GAME_JOIN packet in
	 *  response.
	 */
	REQ_GAME_JOIN,

	/** @brief Signals the start of a REQ_GAME_LEAVE packet.
	 *
	 *  The packet is composed of:
	 *    - An integer containing REQ_GAME_LEAVE.
	 *    - An easysock-formatted string contining the player's name.
	 *  This packet is sent any time a player leaves the table.  The
	 *  table (GGZdMod-game) is responsible for finding and removing
	 *  the player.  The table should send a RSP_GAME_LEAVE packet
	 *  in response.
	 */
	REQ_GAME_LEAVE,

	/** @brief Signals the start of a RSP_GAME_STATE packet.
	 *
	 *  The packet is composed of:
	 *    - An integer containing RSP_GAME_STATE.
	 *  This packet is sent in response to a REQ_GAME_STATE.  It's
	 *  only effect is to let the table know the information has
	 *  been received.
	 */
	RSP_GAME_STATE,
} ControlToTable;

#endif /* __GGZ_SERVER_PROTOCOL_H */
