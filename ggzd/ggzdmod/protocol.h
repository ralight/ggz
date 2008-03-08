/*
 * GGZDMOD - C implementation of the GGZ server-server protocol
 *
 * Copyright (C) 1999 Brent Hendricks
 * Copyright (C) 2000 - 2008 GGZ Gaming Zone Development Team
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef GGZDMOD_SERVER_PROTOCOL_H
#define GGZDMOD_SERVER_PROTOCOL_H

/*
 * Protocol opcode enumerations for the GGZDMOD game server (module) to
 * GGZ server (ggzd) communication.
 * This file is equally used by ggzdmod-ggz (ggzd) and ggzdmod-game.
 * Have a look at the GGZ server-server protocol specification for details
 * on the messages, their flow and other considerations.
 */

/** Messages sent from the game server to the GGZ server. */
typedef enum {
	/* Signals that a message from the game server is to be logged
	 * by ggzd.
	 * The packet is composed of:
	 * - An integer containing MSG_LOG
	 * - An easysock-formatted string containing the log message
	 */
	MSG_LOG,

	/* Tells the server of a needed change in game state.
	 * The packet is composed of:
	 * - An integer containing REQ_GAME_STATE.
	 * - A single byte (char) containing the new game state, which
	 *   is a GGZdModState enumerated value
	 * The server will send a RSP_GAME_STATE packet in response.
	 */
	REQ_GAME_STATE,

	/* Tells the server to modify the number of seats at the table.
	 * The packet is composed of:
	 * - an integer containing REQ_NUM_SEATS
	 * - an integer containing the number of seats needed for the table
	 */
	REQ_NUM_SEATS,

	/* Tells the server to boot a player from the table.
	 * The packet is composed of:
	 * - an integer containing REQ_BOOT
	 * - an easysock-formatted string containing the name of the player
	 */
	REQ_BOOT,

	/* Tells the server to put a bot into an empty seat.
	 * The packet is composed of:
	 * - an integer containing REQ_BOT
	 * - an integer with the seat number where to put the bot
	 */
	REQ_BOT,

	/* Tells the server to open up a seat currently occupied by a bot.
	 * The packet is composed of:
	 * - an integer containing REQ_OPEN
	 * - an integer with the seat number of the affected seat
	 */
	REQ_OPEN,

	/* Signals the start of a MSG_GAME_REPORT packet which transmits game
	 * results and statistics.
	 * The packet is composed of:
	 * - An integer containing MSG_GAME_REPORT
	 * - An integer for the number of players
	 * > For each player:
	 *   - An easysock-formatted string for the name
	 *   - An integer containing the player type
	 *   - An integer containing the team number
	 *     If no teams are defined, each player represents
	 *     a team on his own.
	 *   - An integer containing the result, e.g. win or loss
	 *   - An integer containing the score
	 */
	MSG_GAME_REPORT,

	/* Informs the server about the location for a potentially continuous
	 * savegame file for the currently running game.
	 * The packet is composed of:
	 * - an integer containing MSG_SAVEGAME_REPORT
	 * - an easysock-formatted string containing the path of the file
	 */
	MSG_SAVEGAME_REPORT
} TableToControl;

/** Messages sent from the ggz server to the game server. */
typedef enum {
	/* Signals the start of a MSG_GAME_LAUNCH packet which is sent
	 * to the table shortly after the game server executable has been
	 * launched. GGZdMod-game, on receiving this packet, updates its
	 * internal data and changes the state from CREATED to WAITING.  It
	 * then notifies the table and the GGZ server of these changes.
	 * The packet is composed of:
	 *  - An integer containing MSG_GAME_LAUNCH
	 *  - A string containing the game's name
	 *  - An integer containing the number of seats at the table
	 *  - An integer containing the number of spectators
	 *  > Seat data for every seat at the table:
	 *    - An integer containing the seat type (GGZSeatType)
	 *      Only GGZ_SEAT_OPEN, GGZ_SEAT_RESERVED and GGZ_SEAT_BOT
	 *      are allowed.
	 *    - If the seat type is GGZ_SEAT_RESERVED or GGZ_SEAT_BOT,
	 *      the name of the player at the seat (as an easysock-
	 *      formatted string).
	 */
	MSG_GAME_LAUNCH,

	/* Signals the start of a REQ_GAME_SEAT packet which is sent every
	 * time a seat is modified. It gives the table all of the information
	 * it needs about the new seat.
	 * The packet is composed of:
	 *  - An integer containing MSG_GAME_SEAT
	 *  - An integer containing the seat number of the changed seat
	 *  - An integer containing the GGZ_SEAT_TYPE
	 *  - An easysock-formatted string contining the player's name,
	 *    or "" if the player has no name.
	 *  - A file descriptor (as integer with ancillary data) containing
	 *    the already opened connection to the player's client socket,
	 *    iff the seat is occupied by a player.
	 *    Note: This file descriptor carries ancillary data
	 *    and cannot be read as an integer value.
	 */
	MSG_GAME_SEAT,

	/* Signals the start of a MSG_GAME_SPECTATOR_SEAT packet.
	 * The packet is composed of:
	 *  - An integer containing MSG_GAME_SPECTATOR_SEAT
	 *  - An integer containing the spectator seat number of the
	 *    changed spectator
	 *  - An easysock-formatted string containing the spectator's name,
	 *    or "" for an empty seat
	 *  - A file descriptor (as integer with ancillary data) containing
	 *    the already opened connection to the spectator's client socket.
	 *    Note: This file descriptor carries ancillary data
	 *    and cannot be read as an integer value.
	 */
	MSG_GAME_SPECTATOR_SEAT,

	/* Signals a player changing seats.
	 * The packet is composed of:
	 *  - An integer containing MSG_GAME_RESEAT.
	 *  - An integer, boolean, whether the player *was* a spectator.
	 *  - An integer, the (spectator) seat the player *was* at.
	 *  - An integer, boolean, whether the player *will be* a spectator.
	 *  - An integer, the (spectator) seat the player *will be* at.
	 */
	MSG_GAME_RESEAT,

	/* Signals the start of a RSP_GAME_STATE packet, which is sent in
	 * response to a REQ_GAME_STATE. Its only effect is to let the table
	 * know the information has been received.
	 * The packet is composed of:
	 *   - An integer containing RSP_GAME_STATE.
	 *  This packet is sent in response to a REQ_GAME_STATE.  It's
	 *  only effect is to let the table know the information has
	 */
	RSP_GAME_STATE,

	/** @brief Signals the start of a MSG_SAVEDGAMES packet.
	 *
	 *  The packed is composed of:
	 *    - An integer containing the saved games names count.
	 *    - An easysock-formated strings for each saved game.
	 */
	MSG_SAVEDGAMES
} ControlToTable;

#endif /* GGZDMOD_SERVER_PROTOCOL_H */
