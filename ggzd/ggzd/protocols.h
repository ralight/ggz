/*
 * File: protocols.h
 * Author: Brent Hendricks
 * Project: GGZ
 * Date: 10/18/99
 * Desc: Protocol enumerations, etc.
 * $Id: protocols.h 2746 2001-11-17 07:21:02Z bmh $
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


#define GGZ_CS_PROTO_VERSION  5

/* Chat subops */					/* PMCCCCCC */
#define GGZ_CHAT_NORMAL		(unsigned char) 0x40	/* 01000000 */
#define GGZ_CHAT_ANNOUNCE	(unsigned char) 0x60	/* 01100000 */
#define GGZ_CHAT_BEEP		(unsigned char) 0x80	/* 10000000 */
#define GGZ_CHAT_PERSONAL	(unsigned char) 0xC0	/* 11000000 */
/* Chat subop bitmasks */
#define GGZ_CHAT_M_MESSAGE	(unsigned char) 0x40	/* X1XXXXXX */
#define GGZ_CHAT_M_PLAYER	(unsigned char) 0x80	/* 1XXXXXXX */

/* Update opcodes */
#define GGZ_UPDATE_DELETE 0
#define GGZ_UPDATE_ADD    1
#define GGZ_UPDATE_LEAVE  2
#define GGZ_UPDATE_JOIN   3
#define GGZ_UPDATE_STATE  4
#define GGZ_UPDATE_LAG    5

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
#define E_NO_PERMISSION -16
#define E_BAD_XML       -17
