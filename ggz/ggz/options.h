/*
 * File: datatypes.h
 * Author: Brent Hendricks
 * Project: NetGames
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


#ifndef _NG_TYPES
#define _NG_TYPES

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

/* Datatypes for server options*/
typedef struct {

	/* Options */
	char* local_conf;
	int log_level;
	char* log_file;
	char *user_name;
	char *server;
	int port;
	int login_type;


	char *password;
	int sock;	
	int game_type;
	int game_pid;
	int game_fd;
	int playerId;
	char connected;
	char playing;

	char remove_users;
	int user_inact_time;
	char clear_stats;
	int stat_clr_time;
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
  GameLaunchFunc* launch;
  char path[MAX_PATH_LEN];
} GameInfo;


/* Array of game-types and their mutex */
struct GameTypes {
  GameInfo info[MAX_GAME_TYPES];
  int count;
};


/* Info about a particular game-table */
typedef struct {
  int type_index;
  int num_seats;
  int num_humans;
  int open_seats;
  int num_reserves;
  unsigned char comp_players;
  unsigned char playing;
  int fd_to_game;
  int pid;
  int players[8];
  int reserve[8];
  int player_fd[8];
  void* options;
} TableInfo;
 

/* Array of game-tables, their mutex, and a counter */
struct GameTables {
  TableInfo info[MAX_TABLES];
  int count;
};
  

/* Info about a logged-in user */
typedef struct {
  int uid;
  char name[MAX_USER_NAME_LEN+1]; /* Room for \0 */
  int fd;
  int table_index;
} UserInfo;


/* Array of logged-in users, their mutex, and a counter */
struct Users {
  UserInfo info[MAX_USERS];
  int count;
};


#define NG_UID_NONE -1       /* Fixed invalid UID value */
#define NG_UID_ANON -2       /* UID for all anonymous players */

#define NG_TYPE_ALL  -1
#define NG_TYPE_RES  -2
#define NG_TYPE_OPEN -3

#endif
