/*
 * File: protocol.h
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


#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__


#define GGZ_CS_PROTO_VERSION  5

/* Chat subop bitmasks */

#define GGZ_CHAT_M_MESSAGE	0x40	/* X1XXXXXX */
#define GGZ_CHAT_M_PLAYER 	0x80	/* 1XXXXXXX */


#define E_USR_LOOKUP   -1
#define E_BAD_OPTIONS  -2
#define E_ROOM_FULL    -3
#define E_TABLE_FULL   -4
#define E_TABLE_EMPTY  -5
#define E_LAUNCH_FAIL  -6
#define E_JOIN_FAIL    -7
#define E_NO_TABLE     -8
#define E_LEAVE_FAIL   -9
#define E_LEAVE_FORBIDDEN -10
#define E_ALREADY_LOGGED_IN -11
#define E_NOT_LOGGED_IN -12
#define E_NOT_IN_ROOM   -13
#define E_AT_TABLE     -14
#define E_IN_TRANSIT   -15

#endif /*__PROTOCOL_H__*/
