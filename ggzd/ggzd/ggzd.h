/*
 * File: ggzd.h
 * Author: Brent Hendricks
 * Project: GGZ Server
 * Date: 3/8/00
 * Desc: Server defines
 * $Id: ggzd.h 4582 2002-09-16 06:07:30Z jdorje $
 *
 * Copyright (C) 2000-2002 Brent Hendricks.
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


#ifndef _GGZ_DEFS
#define _GGZ_DEFS

/* Server options */
#define UNLIMITED_SEATS
#define UNLIMITED_SPECTATORS

/* Server limits */
#define MAX_USER_NAME_LEN 16
#define MAX_PATH_LEN 256
#define MAX_GAME_TYPES 35
#define MAX_TABLES  50
#define MAX_USERS 500
#define MAX_CHAT_LEN 512
#define MAX_GAME_DESC_LEN 512
#define MAX_CHAT_BUFFER 32
#ifndef UNLIMITED_SEATS
#  define MAX_TABLE_SIZE 8
#endif
#ifndef UNLIMITED_SPECTATORS
#  define MAX_TABLE_SPECTATORS 4
#endif
#define DEFAULT_MAX_ROOM_USERS 100
#define DEFAULT_MAX_ROOM_TABLES 50

typedef struct GGZClient GGZClient;

typedef enum {
	GGZ_CLIENT_GENERIC,
	GGZ_CLIENT_PLAYER,
	GGZ_CLIENT_CHANNEL
} GGZClientType;
//typedef char* GGZClientType;
//#define GGZ_CLIENT_GENERIC "0"
//#define GGZ_CLIENT_PLAYER "1"
//#define GGZ_CLIENT_CHANNEL "2"

typedef struct _GGZNetIO GGZNetIO;

/* User login status */
typedef enum {
	GGZ_LOGIN_ANON = -1,
	GGZ_LOGIN_NONE = 0,
	GGZ_LOGIN_REGISTERED = 1
} GGZLoginStatus;
//typedef char* GGZLoginStatus;
//#define GGZ_LOGIN_ANON "-1"
//#define GGZ_LOGIN_NONE "0"
//#define GGZ_LOGIN_REGISTERED "1"

typedef enum {
	GGZ_PLAYER_NONE,
	GGZ_PLAYER_NORMAL,
	GGZ_PLAYER_GUEST,
	GGZ_PLAYER_ADMIN
} GGZPlayerType;
//typedef char* GGZPlayerType;
//#define GGZ_PLAYER_NONE "0"
//#define GGZ_PLAYER_NORMAL "1"
//#define GGZ_PLAYER_GUEST "2"
//#define GGZ_PLAYER_ADMIN "3"

typedef struct _GGZPlayer GGZPlayer; 

typedef struct GGZTable GGZTable;


/* Specical game type values */
typedef enum {
	GGZ_TYPE_ALL	= -1,
	GGZ_TYPE_RES	= -2,
	GGZ_TYPE_OPEN	= -3,
} GGZTableFilter;
//typedef char* GGZTableFilter;
//#define GGZ_TYPE_ALL "-1"
//#define GGZ_TYPE_RES "-2"
//#define GGZ_TYPE_OPEN "-3"

/* Player Handler return values */
typedef enum {
	GGZ_REQ_OK		= 0,
	GGZ_REQ_DISCONNECT	= -1,
	GGZ_REQ_FAIL		= 1,
	GGZ_REQ_TABLE_JOIN	= 2,
	GGZ_REQ_TABLE_LEAVE	= 3,
} GGZPlayerHandlerStatus;
//typedef char* GGZPlayerHandlerStatus;
//#define GGZ_REQ_OK "0"
//#define GGZ_REQ_DISCONNECT "-1"
//#define GGZ_REQ_FAIL "1"
//#define GGZ_REQ_TABLE_JOIN "2"
//#define GGZ_REQ_TABLE_LEAVE "3"


/* GGZEventFunc return values */
/* Fixme: it's called "Return" here, but "Status" up above. */
typedef enum {
	GGZ_EVENT_ERROR	= -1,
	GGZ_EVENT_OK	= 0,
	GGZ_EVENT_DEFER	= 1
} GGZEventFuncReturn;
//typedef char* GGZEventFuncReturn;
//#define GGZ_EVENT_ERROR "-1"
//#define GGZ_EVENT_OK "0"
//#define GGZ_EVENT_DEFER "1"


/* Many functions return 0 on success, -1 on error: this just
   formalizes it. */
//typedef enum {
//	GGZ_OK = 0,
//	GGZ_ERROR = -1
//} GGZReturn;
typedef char* GGZReturn;
#define GGZ_OK "0"
#define GGZ_ERROR "-1"

/*
 * Configuration options
 */

#endif
