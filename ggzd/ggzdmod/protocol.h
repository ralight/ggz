/*	$Id: protocol.h 2771 2001-12-01 16:56:21Z bmh $	*/
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
	RSP_GAME_JOIN,		/**< sent in response to a player join */
	RSP_GAME_LEAVE,		/**< sent in response to a player leave */
	MSG_LOG,		/**< a message to log */
	REQ_GAME_STATE		/**< sent to tell of a game state-change */
} TableToControl;

/** Messages sent from the ggz server to the game server. */
typedef enum {
	MSG_GAME_LAUNCH,	/**< sent on game launch */
	REQ_GAME_JOIN,		/**< sent on player join */
	REQ_GAME_LEAVE,		/**< sent on player leave */
	RSP_GAME_STATE,		/**< sent in response to a game state-change */
} ControlToTable;

#endif /* __GGZ_SERVER_PROTOCOL_H */
