/*
 * File: ggzd.h
 * Author: Brent Hendricks
 * Project: GGZ Server
 * Date: 3/8/00
 * Desc: Server defines
 * $Id: ggzd.h 4139 2002-05-03 03:17:08Z bmh $
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

/* Server limits */
#define MAX_USER_NAME_LEN 16
#define MAX_PATH_LEN 256
#define MAX_GAME_TYPES 35
#define MAX_TABLES  50
#define MAX_USERS 500
#define MAX_CHAT_LEN 512
#define MAX_GAME_DESC_LEN 512
#define MAX_CHAT_BUFFER 32
#define MAX_TABLE_SIZE 8
#define DEFAULT_MAX_ROOM_USERS 100
#define DEFAULT_MAX_ROOM_TABLES 50

typedef struct GGZClient GGZClient;

typedef enum {
	GGZ_CLIENT_GENERIC,
	GGZ_CLIENT_PLAYER,
	GGZ_CLIENT_CHANNEL
} GGZClientType;


typedef struct _GGZNetIO GGZNetIO;

/* Special UID values.  Eventually, every registered player
   will have a unique UID, but for now they are all just
   assigned a UID of 0. */
typedef enum {
	GGZ_UID_REGISTERED = 0,
	GGZ_UID_NONE = -1,
	GGZ_UID_ANON = -2
} GGZUIDType;

typedef enum {
	GGZ_PLAYER_NONE,
	GGZ_PLAYER_NORMAL,
	GGZ_PLAYER_GUEST,
	GGZ_PLAYER_ADMIN
} GGZPlayerType;

typedef struct _GGZPlayer GGZPlayer; 

typedef struct GGZTable GGZTable;


/* Specical game type values */
typedef enum {
	GGZ_TYPE_ALL	= -1,
	GGZ_TYPE_RES	= -2,
	GGZ_TYPE_OPEN	= -3,
} GGZTableFilter;

/* Player Handler return values */
typedef enum {
	GGZ_REQ_OK		= 0,
	GGZ_REQ_DISCONNECT	= -1,
	GGZ_REQ_FAIL		= 1,
	GGZ_REQ_TABLE_JOIN	= 2,
	GGZ_REQ_TABLE_LEAVE	= 3,
} GGZPlayerHandlerStatus;


/* GGZEventFunc return values */
/* Fixme: it's called "Return" here, but "Status" up above. */
typedef enum {
	GGZ_EVENT_ERROR	= -1,
	GGZ_EVENT_OK	= 0,
	GGZ_EVENT_DEFER	= 1
} GGZEventFuncReturn;


/*
 * Configuration options
 */

#endif

