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


#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#define GGZ_CS_PROTO_VERSION  3

typedef enum {
	REQ_LOGIN_NEW,
	REQ_LOGIN,
	REQ_LOGIN_ANON,
	REQ_LOGOUT,
	REQ_PREF_CHANGE,
	REQ_REMOVE_USER,

	REQ_LIST_PLAYERS,
	REQ_LIST_TYPES,
	REQ_LIST_TABLES,
	REQ_LIST_ROOMS,
	REQ_TABLE_OPTIONS,
	REQ_USER_STAT,

	REQ_TABLE_LAUNCH,
	REQ_TABLE_JOIN,
	REQ_TABLE_LEAVE,

	REQ_GAME,
	REQ_CHAT,
        REQ_MOTD,

	REQ_ROOM_JOIN
} GGZUserOp;


typedef enum {
	MSG_SERVER_ID,
	MSG_SERVER_FULL,
	MSG_MOTD,
	MSG_CHAT,
	MSG_UPDATE_PLAYERS,
	MSG_UPDATE_TYPES,
	MSG_UPDATE_TABLES,
	MSG_UPDATE_ROOMS,
	MSG_ERROR,

	RSP_LOGIN_NEW,
	RSP_LOGIN,
	RSP_LOGIN_ANON,
	RSP_LOGOUT,
	RSP_PREF_CHANGE,
	RSP_REMOVE_USER,

	RSP_LIST_PLAYERS,
	RSP_LIST_TYPES,
	RSP_LIST_TABLES,
	RSP_LIST_ROOMS,
	RSP_TABLE_OPTIONS,
	RSP_USER_STAT,

	RSP_TABLE_LAUNCH,
	RSP_TABLE_JOIN,
	RSP_TABLE_LEAVE,

	RSP_GAME,
	RSP_CHAT,
        RSP_MOTD,

	RSP_ROOM_JOIN
} GGZServerOp;

/* Chat subops */					/* PMCCCCCC */
typedef enum {
	GGZ_CHAT_NORMAL = 	0x40,	/* 01000000 */
	GGZ_CHAT_ANNOUNCE =	0x60,	/* 01100000 */
	GGZ_CHAT_BEEP = 	0x80,	/* 10000000 */
	GGZ_CHAT_PERSONAL =	0xC0,	/* 11000000 */
} GGZChatOp;

/* Chat subop bitmasks */

#define GGZ_CHAT_M_MESSAGE	0x40	/* X1XXXXXX */
#define GGZ_CHAT_M_PLAYER 	0x80	/* 1XXXXXXX */


/* Update opcodes */
#define GGZ_UPDATE_DELETE 0
#define GGZ_UPDATE_ADD    1
#define GGZ_UPDATE_LEAVE  2
#define GGZ_UPDATE_JOIN   3
#define GGZ_UPDATE_STATE   4


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

#endif /*__PROTOCOL_H__*/
