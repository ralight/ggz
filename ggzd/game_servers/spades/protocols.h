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


typedef enum {
	REQ_GAME_LAUNCH,	/**< sent on game launch */
	REQ_GAME_JOIN,		/**< sent on player join */
	REQ_GAME_LEAVE,		/**< sent on player leave */
	RSP_GAME_STATE,		/**< sent in response to a gameover */
} ControlToTable;

typedef enum {
	RSP_GAME_JOIN,		/**< sent in response to a player join */
	RSP_GAME_LEAVE,		/**< sent in response to a player leave */
	MSG_LOG,		/**< a message to log */
	REQ_GAME_STATE,		/**< sent to tell of a game-over */
} TableToControl;

typedef enum {
	GGZ_SEAT_OPEN = -1,	   /**< The seat is open (unoccupied). */
	GGZ_SEAT_BOT = -2,	   /**< The seat has a bot (AI) in it. */
	GGZ_SEAT_RESV = -3,	   /**< The seat is reserved for a player. */
	GGZ_SEAT_NONE = -4,	   /**< This seat does not exist. */
	GGZ_SEAT_PLAYER = -5	   /**< The seat has a regular player in it. */
} GGZdModSeat;

typedef enum {
	GGZDMOD_STATE_CREATED,	/**< Pre-launch; waiting for ggzdmod */
	GGZDMOD_STATE_WAITING,	/**< Ready and waiting to play. */
	GGZDMOD_STATE_PLAYING,	/**< Currently playing a game. */
	GGZDMOD_STATE_DONE		/**< Table halted, prepping to exit. */
} GGZdModState;

