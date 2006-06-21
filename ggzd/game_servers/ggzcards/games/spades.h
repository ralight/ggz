/* 
 * File: games/spades.h
 * Author: Jason Short
 * Project: GGZCards Server
 * Date: 07/02/2001
 * Desc: Game-dependent game data for Spades (for use by AI)
 * $Id: spades.h 8241 2006-06-21 18:44:11Z jdorje $
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

/* special bids */
enum {
	SPADES_BID,
	SPADES_NIL,
	SPADES_NO_BLIND,	/* a choice *not* to bid blind nil */
	SPADES_DNIL		/* double nil and blind nil are the same */
};

/* Each of these opcodes is used for a GAME_MESSAGE_GAME.  The opcode is sent 
   as a 1-byte char. */
enum {
	/* Sends the team scores, tricks, and bags. */
	SPD_MSG_SCOREDATA
};
