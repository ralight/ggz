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

#include <stdio.h>
#include <pthread.h>
#include <time.h>

#include <ggzd.h>

/* Datatypes for server options*/
typedef struct {
	char *local_conf;
	char remove_users;
	int user_inact_time;
	char clear_stats;
	int stat_clr_time;
	int main_port;
	char *game_dir;		/* CLEANUP CANDIDATE - should get free()'d */
	char *admin_name;	/* cleanup() */
	char *admin_email;	/* cleanup() */
	int perform_lookups;
	int num_rooms;
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
	char state;
	pthread_cond_t state_cond;
	pthread_mutex_t state_lock;
	int transit;
	int transit_fd;
	char transit_flag;
	pthread_cond_t transit_cond;
	pthread_mutex_t transit_lock;
	int fd_to_game;
	int pid;
	int seats[MAX_TABLE_SIZE];
	int reserve[MAX_TABLE_SIZE];
	int player_fd[MAX_TABLE_SIZE];
	void *options;
	char desc[MAX_GAME_DESC_LEN + 1];
} TableInfo;


/* Array of game-tables, their mutex, and a counter */
struct GameTables {
	TableInfo info[MAX_TABLES];
	int count;
	pthread_rwlock_t lock;
	time_t timestamp;
};


/* A list item for chats */
typedef struct ChatItem {
	int reference_count;
	char *chat_sender;
	char *chat_msg;
	struct ChatItem *next;
} ChatItemStruct;


/* A Room Structure */
typedef struct {
	pthread_rwlock_t lock;
	int player_count;
	ChatItemStruct *chat_tail;
#ifdef DEBUG
	ChatItemStruct *chat_head;
#endif
} RoomStruct;


/* Info about a logged-in user */
typedef struct {
	int uid;
	char name[MAX_USER_NAME_LEN + 1];	/* Room for \0 */
	int fd;
	char playing;
	pthread_t pid;
	int table_index;
	char ip_addr[16];
	char *hostname;				/* cleanup() */
	int room;
	ChatItemStruct *chat_head;
	ChatItemStruct *personal_head;
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


/* MOTD info */
typedef struct {
	char *motd_file;			/* cleanup() */
	char use_motd;
	unsigned long startup_time;
	int motd_lines;
	char *motd_text[MAX_MOTD_LINES];	/* cleanup() */
	char *hostname;				/* cleanup() */
	char *sysname;				/* cleanup() */
	char *cputype;				/* cleanup() */
	char *port;				/* cleanup() */
} MOTDInfo;

/* Logfile info */
typedef struct {
	int log_initialized;
	int syslog_facility;
	unsigned options;
	char *log_fname;			/* cleanup() */
	FILE *logfile;
	unsigned log_types;
#ifdef DEBUG
	char *dbg_fname;			/* cleanup() */
	FILE *dbgfile;
	unsigned dbg_types;
#endif
} LogInfo;
#endif
