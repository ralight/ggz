/*
 * GGZMOD - C implementation of the GGZ client-client protocol
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

#ifndef GGZMOD_CLIENT_PROTOCOL_H
#define GGZMOD_CLIENT_PROTOCOL_H

/*
 * Protocol opcode enumerations for the GGZMOD game client (module) to
 * core client communication.
 * Must be kept synchronized between ggzmod-ggz and ggzmod-game!
 * Have a look at the GGZ client-client protocol specification for details
 * on the messages, their flow and other considerations.
 */

/** Messages sent from the game client to the GGZ core client. */
typedef enum {
	/* The game client informs about a state change, most likely from
	 * WAITING to PLAYING or from PLAYING to either WAITING or DONE.
	 * Packet content:
	 * - an integer containing MSG_GAME_STATE
	 * - a character containing the new state of type GGZModState
	 */
	MSG_GAME_STATE,

	/* The player requests to leave a seat in order to spectate.
	 * Packet content:
	 * - an integer containing REQ_STAND
	 */
	REQ_STAND,

	/* The player requests to sit down to become actively involved
	 * in the game.
	 * Packet content:
	 * - an integer containing REQ_SIT
	 * - an integer containing the seat number where to sit down
	 */
	REQ_SIT,

	/* The game's host player decided to boot a player.
	 * Packet content:
	 * - an integer containing REQ_BOOT
	 * - an easysock-formatted string containing the name of the player
	 *   in question
	 */
	REQ_BOOT,

	/* The game's host player decided to fill in a bot into an empty seat.
	 * Packet content:
	 * - an integer containing REQ_BOT
	 * - an integer containing the seat number where to put the bot
	 */
	REQ_BOT,

	/* The game's host decided to remove a bot to gain an empty seat.
	 * Packet content:
	 * - an integer containing REQ_OPEN
	 * - an integer containing the seat number associated with the bot
	 */
	REQ_OPEN,

	/* The player sends a chat message to the other players on the table.
	 * Packet content:
	 * - an integer containing REQ_CHAT
	 * - an easysock-formatted string containing the chat message
	 */
	REQ_CHAT,

	/* The player requests information on other players.
	 * Packet content:
	 * - an integer containing REQ_INFO
	 * - an integer containing the seat number, or alternatively -1 to
	 *   request information from all players
	 */
	REQ_INFO,

	/* The player requests ranking information for the entire room.
	 * Packet content:
	 * - an integer containing REQ_RANKINGS
	 */
	REQ_RANKINGS
} TableToControl;

/** Messages sent from the GGZ core client to the game client. */
typedef enum {
	/* The core client informs the game about the fact that is was just
	 * launched successfully.
	 * Packet content:
	 * - an integer containing MSG_GAME_LAUNCH
	 */
	MSG_GAME_LAUNCH,

	/* The core client informs about the location of the GGZ server on
	 * which the corresponding game server runs.
	 * Packet content:
	 * - an integer containing MSG_GAME_SERVER
	 * - an easysock-formatted string containing the host name
	 * - an integer containing the port number
	 * - an easysock-formatted string containing the player name
	 */
	MSG_GAME_SERVER,

	/* The core client informs about the file descriptor of the connection
	 * already established to the corresponding game server.
	 * Packet content:
	 * - an integer containing MSG_GAME_SERVER_FD
	 * - a file descriptor (as integer with ancillary data)
	 */
	MSG_GAME_SERVER_FD,

	/* The core client gives table information about the player who runs
	 * this game client.
	 * Packet content:
	 * - an integer containing MSG_GAME_PLAYER
	 * - an easysock-formatted string containing the player's name
	 * - an integer containing the spectator status
	 * - an integer containing the seat number
	 */
	MSG_GAME_PLAYER,

	/* The core client tells of a seat change on the table.
	 * Packet content:
	 * - an integer containing MSG_GAME_SEAT
	 * - an integer containing the new seat number
	 * - an integer containing the new seat type
	 * - an easysock-formatted string containing the new occupant's name
	 */
	MSG_GAME_SEAT,

	/* The core client tells of a spectator seat change on the table.
	 * Packet content:
	 * - an integer containing MSG_GAME_SPECTATOR_SEAT
	 * - an integer containing the new seat number
	 * - an easysock-formatted string containing the new occupant's name
	 */
	MSG_GAME_SPECTATOR_SEAT,

	/* The core client informs about an incoming chat message from another
	 * player.
	 * Packet content:
	 * - an integer containing MSG_GAME_CHAT
	 * - an easysock-formatted string containing the name of the player
	 * - an easysock-formatted string containing the chat message
	 */
	MSG_GAME_CHAT,

	/* The core client provides statistics about one or more players on
	 * the table.
	 * Packet content:
	 * - an integer containing MSG_GAME_STATS
	 * > for each player and for each spectator:
	 *   - an integer describing the existence of a record entry
	 *   - an integer describing the existence of a rating entry
	 *   - an integer describing the existence of a ranking entry
	 *   - an integer describing the existence of a highscore entry
	 *   - an integer for the number of wins (valid only if record)
	 *   - an integer for the number of losses (valid only if record)
	 *   - an integer for the number of ties (valid only if record)
	 *   - an integer for the number of forfeits (valid only if record)
	 *   - an integer for the rating (valid only if rating)
	 *   - an integer for the ranking (valid only if ranking)
	 *   - an integer for the highscore (valid only if highscore)
	 */
	MSG_GAME_STATS,

	/* The core client has received extended player information to be
	 * displayed by the game client.
	 * Packet content:
	 * - an integer containing MSG_GAME_INFO
	 * - an integer containing the number of information entries
	 * > for each entry:
	 *   - an integer containing the seat number
	 *   - an easysock-formatted string containing the player's real name
	 *   - an easysock-formatted string containing a photo URL
	 *   - an easysock-formatted string containing the player's host name
	 */
	MSG_GAME_INFO,

	/* The core client has received rankings information for the current
	 * room, including players who might not be logged in.
	 * Packet content:
	 * - an integer containing MSG_GAME_RANKINGS
	 * - an integer containing the number of information entries
	 * > for each entry:
	 *   - an easysock-formatted string containing the player's name
	 *   - an integer containing the position
	 *   - an integer containing the score
	 */
	MSG_GAME_RANKINGS
} ControlToTable;

#endif /* GGZMOD_CLIENT_PROTOCOL_H */
