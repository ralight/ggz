/*
 * File: protocols.h
 * Author: Brent Hendricks
 * Project: GGZ
 * Date: 10/18/99
 * Desc: Protocol enumerations, etc.
 * $Id: protocols.h 5267 2002-11-25 19:16:38Z dr_maux $
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
#define GGZ_CS_PROTO_VERSION 8

/* Update opcodes */
typedef enum {
	GGZ_PLAYER_UPDATE_ADD, /* player enters room */
	GGZ_PLAYER_UPDATE_DELETE, /* player leaves room */
	GGZ_PLAYER_UPDATE_LAG, /* player's lag status changed */
	GGZ_PLAYER_UPDATE_STATS /* player's stats changed */
} GGZPlayerUpdateType;

/* Update opcodes */
typedef enum {
	GGZ_TABLE_UPDATE_ADD, /* table is added to room */
	GGZ_TABLE_UPDATE_DELETE, /* table is deleted from room */
	GGZ_TABLE_UPDATE_JOIN, /* player joins table */
	GGZ_TABLE_UPDATE_LEAVE, /* player leaves table */
	GGZ_TABLE_UPDATE_SEAT, /* misc. table seat change */
	GGZ_TABLE_UPDATE_SPECTATOR_LEAVE, /* spectator leaves table */
	GGZ_TABLE_UPDATE_SPECTATOR_JOIN, /* spectator joins table */
	GGZ_TABLE_UPDATE_STATE, /* table state changes */
	GGZ_TABLE_UPDATE_DESC, /* table desc changes */
	GGZ_TABLE_UPDATE_RESIZE /* change size of table */
} GGZTableUpdateType;

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
