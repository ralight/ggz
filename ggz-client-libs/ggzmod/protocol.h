/*
 * File: protocols.h
 * Author: Brent Hendricks
 * Project: GGZ
 * Date: 10/18/99
 * Desc: Protocol enumerations, etc.
 * $Id: protocol.h 4264 2002-06-22 05:10:28Z bmh $
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
	MSG_GAME_STATE,
} TableToControl;

/** Messages sent from the ggz server to the game server. */
typedef enum {
	MSG_GAME_LAUNCH,
	MSG_GAME_SERVER,
} ControlToTable;

#endif /* __GGZ_SERVER_PROTOCOL_H */
