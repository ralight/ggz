/*
 * File: datatypes.h
 * Author: Brent Hendricks
 * Project: GGZ Server
 * Date: 10/11/99
 * Desc: Datatypes used by server
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

#include <pthread.h>
#include <time.h>

/* Server limits */
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
#define MAX_CHAT_LEN 512
#define MAX_CHAT_BUFFER 32
#define MAX_TABLE_SIZE 8

/* Bitmasks for allowable player numbers */
#define PLAY_ALLOW_ZERO    0
#define PLAY_ALLOW_ONE     1
#define PLAY_ALLOW_TWO     (1 << 1)
#define PLAY_ALLOW_THREE   (1 << 2)
#define PLAY_ALLOW_FOUR    (1 << 3)
#define PLAY_ALLOW_FIVE    (1 << 4)
#define PLAY_ALLOW_SIX     (1 << 5)
#define PLAY_ALLOW_SEVEN   (1 << 6)
#define PLAY_ALLOW_EIGHT   (1 << 7)

/* Bitmasks for allowable computer player numbers */
#define COMP_ALLOW_ZERO    0
#define COMP_ALLOW_ONE     1
#define COMP_ALLOW_TWO     (1 << 1)
#define COMP_ALLOW_THREE   (1 << 2)
#define COMP_ALLOW_FOUR    (1 << 3)
#define COMP_ALLOW_FIVE    (1 << 4)
#define COMP_ALLOW_SIX     (1 << 5)
#define COMP_ALLOW_SEVEN   (1 << 6)
#define COMP_ALLOW_EIGHT   (1 << 7)

/* Special UID values */
#define NG_UID_NONE -1
#define NG_UID_ANON -2

/* Specical game type values */
#define NG_TYPE_ALL  -1
#define NG_TYPE_RES  -2
#define NG_TYPE_OPEN -3

/* Handler return values */
#define NG_HANDLE_OK           0
#define NG_HANDLE_LOGOUT      -1
#define NG_HANDLE_GAME_START   1
#define NG_HANDLE_GAME_OVER    2

/* Special seat assignment values */
#define GGZ_SEAT_OPEN   -1
#define GGZ_SEAT_COMP   -2
#define GGZ_SEAT_RESV   -3
#define GGZ_SEAT_NONE   -4

/* Datatypes for server options*/
typedef struct {
	char *local_conf;
	int log_level;
	char *log_file;
	char remove_users;
	int user_inact_time;
	char clear_stats;
	int stat_clr_time;
	int main_port;
	char *game_dir;		/* CLEANUP CANDIDATE - should get free()'d */
} Options;


/* Init function type */
typedef void (*GameLaunchFunc) (void);


/* Info about a particular type of game*/
typedef struct {
	char name[MAX_GAME_NAME_LEN];
	char version[MAX_GAME_VER_LEN];
	char desc[MAX_GAME_DESC_LEN];
	char author[MAX_GAME_AUTH_LEN];
	char homepage[MAX_GAME_WEB_LEN];
	unsigned char num_play_allow;
	unsigned char comp_allow;
	int options_size;
	unsigned char enabled;
	GameLaunchFunc *launch;
	char path[MAX_PATH_LEN];
} GameInfo;


/* Array of game-types and their mutex */
struct GameTypes {
	GameInfo info[MAX_GAME_TYPES];
	int count;
	pthread_rwlock_t lock;
	time_t timestamp;
};


/* Info about a particular game-table */
typedef struct {
	int type_index;
	pthread_cond_t seats_cond;
	pthread_mutex_t seats_lock;
	unsigned char playing;
	int fd_to_game;
	int pid;
	int seats[MAX_TABLE_SIZE];
	int reserve[MAX_TABLE_SIZE];
	int player_fd[MAX_TABLE_SIZE];
	void *options;
} TableInfo;


/* Array of game-tables, their mutex, and a counter */
struct GameTables {
	TableInfo info[MAX_TABLES];
	int count;
	pthread_rwlock_t lock;
	time_t timestamp;
};


/* Info about a logged-in user */
typedef struct {
	int uid;
	char name[MAX_USER_NAME_LEN + 1];	/* Room for \0 */
	int fd;
	char playing;
	pthread_t pid;
	int table_index;
} UserInfo;


/* Array of logged-in users, their mutex, and a counter */
struct Users {
	UserInfo info[MAX_USERS];
	int count;
	pthread_rwlock_t lock;
	time_t timestamp;
};


/* A chat consists of the message and a flag of who hasn't read it*/
typedef struct {
	char msg[MAX_CHAT_LEN + 1];
	int p_index;
	char p_name[MAX_USER_NAME_LEN + 1];
	char unread[MAX_USERS];
	int unread_count;
} ChatInfo;


/* Array of chats in buffer, their mutex, and a counter */
struct Chats {
	ChatInfo info[MAX_CHAT_BUFFER];
	int count;
	int player_unread_count[MAX_USERS];
	pthread_rwlock_t lock;
};
	

/* Reservation info */
typedef struct {
	int game_index;
	int uid;
} ReserveInfo;


#endif

