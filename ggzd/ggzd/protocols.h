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
	REQ_NEW_LOGIN,
	REQ_LOGIN,
	REQ_ANON_LOGIN,
	REQ_MOTD,
	REQ_LOGOUT,
	REQ_USER_LIST,
	REQ_PREF_CHANGE,
	REQ_REMOVE_USER,
	REQ_GAME_TYPES,
	REQ_TABLE_LIST,
	REQ_TABLE_OPTIONS,
	REQ_LAUNCH_GAME,
	REQ_JOIN_GAME,
	REQ_USER_STAT,
	REQ_GAME,
	REQ_CHAT
} UserToControl;


typedef enum {
	MSG_SERVER_ID,
	MSG_SERVER_FULL,
	MSG_CHAT,
	MSG_USERS_UPDATE,
	MSG_TYPES_UPDATE,
	MSG_TABLES_UPDATE,
	RSP_NEW_LOGIN,
	RSP_LOGIN,
	RSP_ANON_LOGIN,
	RSP_MOTD,
	RSP_LOGOUT,
	RSP_USER_LIST,
	RSP_PREF_CHANGE,
	RSP_REMOVE_USER,
	RSP_GAME_TYPES,
	RSP_TABLE_LIST,
	RSP_TABLE_OPTIONS,
	RSP_LAUNCH_GAME,
	RSP_JOIN_GAME,
	RSP_USER_STAT,
	RSP_GAME,
	RSP_CHAT,
	RSP_ERROR
} ControlToUser;


typedef enum {
	RSP_GAME_LAUNCH,
	MSG_GAME_OVER
} TableToControl;

typedef enum {
	REQ_GAME_LAUNCH
} ControlToTable;

#define E_USR_LOOKUP   -1
#define E_BAD_OPTIONS  -2
#define E_ROOM_FULL    -3
#define E_TABLE_FULL   -4
#define E_TABLE_EMPTY  -5
#define E_LAUNCH_FAIL  -6
#define E_RESPOND_FAIL -7
