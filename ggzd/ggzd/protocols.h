/*
 * File: protocols.h
 * Author: Brent Hendricks
 * Project: GGZ
 * Date: 10/18/99
 * Desc: Protocol enumerations, etc.
 * $Id: protocols.h 8071 2006-05-29 07:34:31Z josef $
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

#ifndef _GGZ_PROTOCOL_H
#define _GGZ_PROTOCOL_H

/* I thought of putting this into ggz_common (in libggz) along with the
   protocol opcodes, but it really needs to stay tied to the network code
   itself (in ggzd and ggzcore). */
#define GGZ_CS_PROTO_VERSION 10

/* Some code is used by several of the following update types.
   Therefore, they must not overlap. */

/* Update opcodes */
typedef enum {
	GGZ_PLAYER_UPDATE_ADD = 1, /* player enters room */
	GGZ_PLAYER_UPDATE_DELETE = 2, /* player leaves room */
	GGZ_PLAYER_UPDATE_LAG = 3, /* player's lag status changed */
	GGZ_PLAYER_UPDATE_STATS = 4 /* player's stats changed */
} GGZPlayerUpdateType;

/* Update opcodes */
typedef enum {
	GGZ_TABLE_UPDATE_ADD = 10, /* table is added to room */
	GGZ_TABLE_UPDATE_DELETE = 11, /* table is deleted from room */
	GGZ_TABLE_UPDATE_JOIN = 12, /* player joins table */
	GGZ_TABLE_UPDATE_LEAVE = 13, /* player leaves table */
	GGZ_TABLE_UPDATE_SEAT = 14, /* misc. table seat change */
	GGZ_TABLE_UPDATE_SPECTATOR_LEAVE = 15, /* spectator leaves table */
	GGZ_TABLE_UPDATE_SPECTATOR_JOIN = 16, /* spectator joins table */
	GGZ_TABLE_UPDATE_STATE = 17, /* table state changes */
	GGZ_TABLE_UPDATE_DESC = 18, /* table desc changes */
	GGZ_TABLE_UPDATE_RESIZE = 19 /* change size of table */
} GGZTableUpdateType;

/* Update opcodes. */
typedef enum {
	GGZ_ROOM_UPDATE_ADD = 20, /* room is added to server */
	GGZ_ROOM_UPDATE_DELETE = 21, /* room is deleted from server */
	GGZ_ROOM_UPDATE_CLOSE = 22, /* room is deleted soon */
	GGZ_ROOM_UPDATE_PLAYER_COUNT = 23 /* Number of players in room changes */
} GGZRoomUpdateType;

typedef enum {
	GGZ_RESEAT_SIT,
	GGZ_RESEAT_STAND,
	GGZ_RESEAT_MOVE
} GGZReseatType;

typedef enum {
	GGZ_JOIN_LAUNCH,
	GGZ_JOIN_REQUEST
} GGZJoinType;

#endif /*_GGZ_PROTOCOL_H*/
