/*
 * File: datatypes.h
 * Author: Brent Hendricks
 * Project: GGZ client
 * Date: 2/19/00
 * Desc: Datatypes used by client
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

#ifndef _GGZ_TYPES
#define _GGZ_TYPES

#include <stdlib.h>

/* Server limits */
/* FIXME: Which of these do we really need ?*/
#define MAX_USER_NAME_LEN 8
#define MAX_GAME_NAME_LEN 16
#define MAX_GAME_VER_LEN  8
#define MAX_GAME_DESC_LEN 512
#define MAX_GAME_AUTH_LEN 32
#define MAX_GAME_WEB_LEN  128
#define MAX_PATH_LEN 256
#define MAX_GAME_TYPES 5
#define MAX_TABLES  50
#define MAX_USERS 500
#define MAX_TABLE_SIZE 8

/* Defines for allowable players */
#define PLAY_ALLOW_ZERO    0
#define PLAY_ALLOW_ONE     1
#define PLAY_ALLOW_TWO     (1 << 1)
#define PLAY_ALLOW_THREE   (1 << 2)
#define PLAY_ALLOW_FOUR    (1 << 3)
#define PLAY_ALLOW_FIVE    (1 << 4)
#define PLAY_ALLOW_SIX     (1 << 5)
#define PLAY_ALLOW_SEVEN   (1 << 6)
#define PLAY_ALLOW_EIGHT   (1 << 7)

/* Defines for allowable computer players*/
#define COMP_ALLOW_ZERO    0
#define COMP_ALLOW_ONE     1
#define COMP_ALLOW_TWO     (1 << 1)
#define COMP_ALLOW_THREE   (1 << 2)
#define COMP_ALLOW_FOUR    (1 << 3)
#define COMP_ALLOW_FIVE    (1 << 4)
#define COMP_ALLOW_SIX     (1 << 5)
#define COMP_ALLOW_SEVEN   (1 << 6)
#define COMP_ALLOW_EIGHT   (1 << 7)

/* Login types */
#define GGZ_LOGIN      0
#define GGZ_LOGIN_ANON 1
#define GGZ_LOGIN_NEW  2

/* special seat assignment values */
#define GGZ_SEAT_OPEN   -1
#define GGZ_SEAT_COMP   -2
#define GGZ_SEAT_RESV   -3
#define GGZ_SEAT_NONE   -4

/* Info about a connection */
struct ConnectInfo {
	gchar *server;
	guint port;
	gchar *username;
	gchar *password;
	guchar login_type;
	gint sock;
	guchar connected;
	guchar playing;
	gint cur_room;
	gint new_room;
};


/* Info about the game we're playing */
struct Game {
	pid_t pid;
	guint type;
	gint fd;
};


/* Init function type */
typedef void (*GameLaunchFunc) (void);

/* Info about a particular type of game*/
typedef struct {
	gchar name[MAX_GAME_NAME_LEN];
	gchar version[MAX_GAME_VER_LEN];
	gchar desc[MAX_GAME_DESC_LEN];
	gchar author[MAX_GAME_AUTH_LEN];
	gchar web[MAX_GAME_WEB_LEN];
	guchar num_play_allow;
	gint index;
} GameInfo;


/* Array of game-types and their mutex */
struct GameTypes {
	GameInfo info[MAX_GAME_TYPES];
	gint count;
};


/* Info about a particular game-table */
typedef struct {
	gint table_index;
	gint type_index;
	guchar playing;
	gint seats[MAX_TABLE_SIZE];
	gchar names[MAX_USER_NAME_LEN + 1];
	gchar desc[1024];
} TableInfo;


/* Array of game-tables, their mutex, and a counter */
struct GameTables {
	TableInfo info[MAX_TABLES];
	gint count;
};


/* Info about a logged-in user */
typedef struct {
	gint uid;
	gchar name[MAX_USER_NAME_LEN + 1];	/* Room for \0 */
	gint fd;
	gint table_index;
	gint chat_color;
} UserInfo;


/* Array of logged-in users, their mutex, and a counter */
struct Users {
	UserInfo info[MAX_USERS];
	gint count;
};

typedef struct {
	char *name;
	char *desc;
	int game_type;
	int num;
} RoomInfo;

struct Rooms {
	RoomInfo *info;
	gint count;
};

#endif
