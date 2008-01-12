/*
 * File: datatypes.h
 * Author: Brent Hendricks
 * Project: GGZ Server
 * Date: 10/11/99
 * Desc: Datatypes used by server
 * $Id: datatypes.h 9525 2008-01-12 22:03:18Z josef $
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
#include <ggz.h>

#include "ggzd.h"
#include "util.h"

/* Datatypes for server options*/
typedef struct {
	bool conf_valid;
	const char *local_conf;
#if 0
	bool remove_users;
#endif
	int foreground;
	int user_inact_time;
	char clear_stats;
	int stat_clr_time;
	int main_port;
	const char *interface;
	const char *game_exec_dir;
	const char *conf_dir;
	const char *data_dir;
	const char *motd_file;
	const char *motd_web;
	const char *admin_name;
	const char *admin_email;
	const char *server_name;
	bool perform_lookups;
	ggztime_t ping_freq;
	ggztime_t lag_class_time[4];
	ggztime_t room_update_freq;
	const char *dbtype;
	const char *dbhost;
	const char *dbname;
	const char *dbusername;
	const char *dbpassword;
	const char *dbhashing;
	const char *dbhashencoding;
	bool tls_use;
	const char *tls_key;
	const char *tls_cert;
	const char *tls_password;
	const char *dump_file;
	bool announce_lan;
	const char *announce_metaserver;
	const char *metausername;
	const char *metapassword;
	bool reconfigure_rooms;
	const char *username_policy;
	bool registration_policy;
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
	pthread_rwlock_t lock; /* This lock is used though not needed. */
	ggz_intlstring *name;    /* Game name */
	char *version; /* Game version; cleanup() */
	char *game; /* Internal game name as per its .dsc file; cleanup() */

	/* Protocol data - the protocol engine is the name of the
	   protocol, and the version is the protocol version.  These
	   need not match up with the gametype. */
	char *p_engine;	/* cleanup() */
	char *p_version;/* cleanup() */

	char *data_dir;	/* The game's working directory; cleanup() */

	ggz_intlstring *desc;     /* Game description string; cleanup() */
	char *author;   /* String containing name(s) of author(s); cleanup() */
	char *homepage; /* Contains a web address for the game; cleanup() */
	
	/* Masks for how many overall players, bots and spectators are allowed
	   to be selected for the game. */
	GGZNumberList player_allow_list;
	GGZNumberList bot_allow_list;
	unsigned char allow_spectators;

	/* Are players allowed to leave mid-game?  (i.e. does the
	   game support this?) */
	bool allow_leave;

	/* Should we automatically kill the game server (table) when the
	   last player leaves? */
	bool kill_when_empty;

	/* Types of stats to keep */
	bool stats_records;
	bool stats_ratings;
#if 0
	bool stats_rankings;
#endif
	bool stats_highscores;

	/* Executable information: a NULL-terminated list of
	   command-line arguments for the program.  The first argument
	   is the executable itself. */
	char **exec_args;/* cleanup() */

	/* NULL-terminated list of (botname, botclass) entries */
	char ***named_bots;

	/* Can players request each other's hostnames? */
	bool allow_peers;
} GameInfo;


/* Global values. */
extern Options opt;
extern GGZState state;
extern struct GameInfo game_types[MAX_GAME_TYPES];

#endif
