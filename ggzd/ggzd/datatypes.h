/*
 * File: datatypes.h
 * Author: Brent Hendricks
 * Project: GGZ Server
 * Date: 10/11/99
 * Desc: Datatypes used by server
 * $Id: datatypes.h 5340 2003-01-22 13:50:38Z dr_maux $
 *
 * Copyright (C) 1999-2002 Brent Hendricks.
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

#include <ggz_common.h>

#include "ggzd.h"


/* Datatypes for server options*/
typedef struct {
	char *local_conf;
	char remove_users;
	char foreground;
	int user_inact_time;
	char clear_stats;
	int stat_clr_time;
	int main_port;
	char *game_dir;		/* cleanup() */
	char *tmp_dir;		/* cleanup() */
	char *conf_dir;		/* cleanup() */
	char *data_dir;		/* cleanup() */
	char *motd_file;	/* cleanup() */
	char *admin_name;	/* cleanup() */
	char *admin_email;	/* cleanup() */
	char *server_name;	/* cleanup() */
	int perform_lookups;
	int ping_freq;
	int lag_class[4];
	char *dbhost;
	char *dbname;
	char *dbusername;
	char *dbpassword;
	char *dbhashing;
	int tls_use;
	char *tls_key;
	char *tls_cert;
	char *tls_password;
	char *dump_file;
} Options;


/* Data type for server state statistics */
typedef struct GGZState {
	pthread_rwlock_t lock;
	unsigned int rooms;
	unsigned int players;
	unsigned int tables;
	unsigned int types;
} GGZState;


/* Info about a particular type of game*/
typedef struct GameInfo {
	pthread_rwlock_t lock;
	char name[MAX_GAME_NAME_LEN + 1];    /* Game name */
	char *version; /* Game version; cleanup() */
	
	/* Protocol data - the protocol engine is the name of the
	   protocol, and the version is the protocol version.  These
	   need not match up with the gametype. */
	char *p_engine;	/* cleanup() */
	char *p_version;/* cleanup() */

	char *data_dir;	/* The game's working directory; cleanup() */

	char *desc;     /* Game description string; cleanup() */
	char *author;   /* String containing name(s) of author(s); cleanup() */
	char *homepage; /* Contains a web address for the game; cleanup() */
	
	/* Masks for how many overall players, bots and spectators are allowed
	   to be selected for the game. */
	GGZNumberList player_allow_list;
	GGZNumberList bot_allow_list;
	unsigned char allow_spectators;
	
	/* Are players allowed to leave mid-game?  (i.e. does the
	   game support this?) */
	unsigned char allow_leave;
	
	/* Should we automatically kill the game server (table) when the
	   last player leaves? */
	unsigned char kill_when_empty;

	/* Types of stats to keep */
	unsigned char stats_records;
	unsigned char stats_ratings;
#if 0
	unsigned char stats_rankings;
	unsigned char stats_highscores;
#endif
	
	/* Executable information: a NULL-terminated list of
	   command-line arguments for the program.  The first argument
	   is the executable itself. */
	char **exec_args;/* cleanup() */
} GameInfo;


#endif
