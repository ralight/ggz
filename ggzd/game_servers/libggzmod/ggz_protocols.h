/*	$Id: ggz_protocols.h 2320 2001-08-29 06:57:37Z jdorje $	*/
/*
 * File: protocols.h
 * Author: Brent Hendricks
 * Project: GGZ
 * Date: 10/18/99
 * Desc: Protocol enumerations, etc.
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
	RSP_GAME_LAUNCH,	/**< sent in response to a game launch */
	RSP_GAME_JOIN,		/**< sent in response to a player join */
	RSP_GAME_LEAVE,		/**< sent in response to a player leave */
	MSG_LOG,		/**< a message to log */
	MSG_DBG,		/**< a debugging message */
	REQ_GAME_OVER,		/**< sent to tell of a game-over */
/* the following are not implemented */
	MSG_STATS,		/**< sent to tell of players winning/losing */
	REQ_SEAT_CHANGE		/**< sent to request a seat type change */
} TableToControl;

/** Messages sent from the ggz server to the game server. */
typedef enum {
	REQ_GAME_LAUNCH,	/**< sent on game launch */
	REQ_GAME_JOIN,		/**< sent on player join */
	REQ_GAME_LEAVE,		/**< sent on player leave */
	RSP_GAME_OVER,		/**< sent in response to a gameover */
/* the following are not implemented */
	RSP_SEAT_CHANGE,	/**< response to REQ_SEAT_CHANGE */
} ControlToTable;

#endif /* __GGZ_SERVER_PROTOCOL_H */
