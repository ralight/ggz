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
	REQ_LOGIN_NEW,
	REQ_LOGIN,
	REQ_LOGIN_ANON,
	REQ_LOGOUT,
	REQ_PREF_CHANGE,
	REQ_REMOVE_USER,

	REQ_LIST_PLAYERS,
	REQ_LIST_TYPES,
	REQ_LIST_TABLES,
	REQ_TABLE_OPTIONS,
	REQ_USER_STAT,

	REQ_TABLE_LAUNCH,
	REQ_TABLE_JOIN,
	REQ_TABLE_LEAVE,

	REQ_GAME,
	REQ_CHAT,
	REQ_MOTD
} UserToControl;


typedef enum {
	MSG_SERVER_ID,
	MSG_SERVER_FULL,
	MSG_MOTD,
	MSG_CHAT,
	MSG_UPDATE_PLAYERS,
	MSG_UPDATE_TYPES,
	MSG_UPDATE_TABLES,
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
	RSP_TABLE_OPTIONS,
	RSP_USER_STAT,

	RSP_TABLE_LAUNCH,
	RSP_TABLE_JOIN,
	RSP_TABLE_LEAVE,

	RSP_GAME,
	RSP_CHAT,
	RSP_MOTD
} ControlToUser;


typedef enum {
	RSP_GAME_LAUNCH,
	MSG_GAME_OVER
} TableToControl;

typedef enum {
	REQ_GAME_LAUNCH,
	REQ_PLAYER_JOIN,
	REQ_PLAYER_LEAVE

} ControlToTable;

#define E_USR_LOOKUP   -1
#define E_BAD_OPTIONS  -2
#define E_ROOM_FULL    -3
#define E_TABLE_FULL   -4
#define E_TABLE_EMPTY  -5
#define E_LAUNCH_FAIL  -6
#define E_RESPOND_FAIL -7
