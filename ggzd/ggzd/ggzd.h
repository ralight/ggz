/*
 * File: ggzd.h
 * Author: Brent Hendricks
 * Project: GGZ Server
 * Date: 3/8/00
 * Desc: Server defines
 * $Id: ggzd.h 2622 2001-10-28 23:36:54Z rgade $
 *
 * Copyright (C) 2000 Brent Hendricks.
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
#define MAX_GAME_NAME_LEN 24
#define MAX_GAME_VER_LEN  8
#define MAX_GAME_PROTOCOL_LEN 16
#define MAX_GAME_DESC_LEN 512
#define MAX_GAME_AUTH_LEN 32
#define MAX_GAME_WEB_LEN  128
#define MAX_PATH_LEN 256
#define MAX_GAME_TYPES 35
#define MAX_TABLES  50
#define MAX_USERS 500
#define MAX_CHAT_LEN 512
#define MAX_CHAT_BUFFER 32
#define MAX_TABLE_SIZE 8
#define MAX_MOTD_LINES 80
#define DEFAULT_MAX_ROOM_USERS 100
#define DEFAULT_MAX_ROOM_TABLES 50

/* Bitmasks for allowable player numbers */
#define GGZ_ALLOW_ZERO    0
#define GGZ_ALLOW_ONE     1
#define GGZ_ALLOW_TWO     (1 << 1)
#define GGZ_ALLOW_THREE   (1 << 2)
#define GGZ_ALLOW_FOUR    (1 << 3)
#define GGZ_ALLOW_FIVE    (1 << 4)
#define GGZ_ALLOW_SIX     (1 << 5)
#define GGZ_ALLOW_SEVEN   (1 << 6)
#define GGZ_ALLOW_EIGHT   (1 << 7)

/* Specical game type values */
#define GGZ_TYPE_ALL  -1
#define GGZ_TYPE_RES  -2
#define GGZ_TYPE_OPEN -3

/* Player Handler return values */
#define GGZ_REQ_OK           0
#define GGZ_REQ_DISCONNECT  -1
#define GGZ_REQ_FAIL         1
#define GGZ_REQ_TABLE_JOIN   2
#define GGZ_REQ_TABLE_LEAVE  3

/* Table state values */
#define GGZ_TABLE_ERROR     -1
#define GGZ_TABLE_CREATED    0
#define GGZ_TABLE_LAUNCHED   1
#define GGZ_TABLE_PLAYING    2
#define GGZ_TABLE_DONE       3

#endif

