/*
 * File: protocols.h
 * Author: Brent Hendricks
 * Project: GGZ
 * Date: 10/18/99
 * Desc: Protocol enumerations, etc.
 * $Id: protocols.h 3433 2002-02-21 04:01:18Z bmh $
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


#define GGZ_CS_PROTO_VERSION  6

/* Chat subops */					/* PMCCCCCC */
#define GGZ_CHAT_NORMAL		(unsigned char) 0x40	/* 01000000 */
#define GGZ_CHAT_ANNOUNCE	(unsigned char) 0x60	/* 01100000 */
#define GGZ_CHAT_BEEP		(unsigned char) 0x80	/* 10000000 */
#define GGZ_CHAT_PERSONAL	(unsigned char) 0xC0	/* 11000000 */
/* Chat subop bitmasks */
#define GGZ_CHAT_M_MESSAGE	(unsigned char) 0x40	/* X1XXXXXX */
#define GGZ_CHAT_M_PLAYER	(unsigned char) 0x80	/* 1XXXXXXX */

/* Update opcodes */
typedef enum {
	GGZ_UPDATE_DELETE = 0,
	GGZ_UPDATE_ADD	  = 1,
	GGZ_UPDATE_LEAVE  = 2,
	GGZ_UPDATE_JOIN	  = 3,
	GGZ_UPDATE_STATE  = 4,
	GGZ_UPDATE_LAG	  = 5,
	GGZ_UPDATE_SEAT   = 6,
	GGZ_UPDATE_DESC   = 7
} GGZUpdateOpcode;

/* These opcodes must be in sync with the client,
   so they should not be changed. */
typedef enum {
	E_USR_LOOKUP	    = -1,
	E_BAD_OPTIONS	    = -2,
	E_ROOM_FULL	    = -3,
	E_TABLE_FULL	    = -4,
	E_TABLE_EMPTY	    = -5,
	E_LAUNCH_FAIL	    = -6,
	E_JOIN_FAIL	    = -7,
	E_NO_TABLE	    = -8,
	E_LEAVE_FAIL	    = -9,
	E_LEAVE_FORBIDDEN   = -10,
	E_ALREADY_LOGGED_IN = -11,
	E_NOT_LOGGED_IN	    = -12,
	E_NOT_IN_ROOM	    = -13,
	E_AT_TABLE	    = -14,
	E_IN_TRANSIT	    = -15,
	E_NO_PERMISSION	    = -16,
	E_BAD_XML	    = -17,
} GGZClientReqError;
