/*
 * File: datatypes.h
 * Author: Brent Hendricks
 * Project: GGZ Server
 * Date: 10/11/99
 * Desc: Datatypes used by server
 * $Id: datatypes.h 4299 2002-07-13 10:20:00Z dr_maux $
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

#include <config.h>

#include <stdio.h>
#include <pthread.h>

#include <ggzd.h>


/* Datatypes for server options*/
typedef struct {
	char *local_conf;
	char remove_users;
	char foreground;
	int user_inact_time;
	char clear_stats;
	int stat_clr_time;
	int main_port;
	char *game_dir;		/* CLEANUP CANDIDATE - should get free()'d */
	char *tmp_dir;		/* cleanup() */
	char *conf_dir;		/* cleanup() */
	char *data_dir;		/* cleanup() */
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
	char *name;    /* Game name */
	char *version; /* Game version */
	
	/* Protocol data - the protocol engine is the name of the
	   protocol, and the version is the protocol version.  These
	   need not match up with the gametype. */
	char *p_engine;
	char *p_version;

	char *data_dir;	/* The game's working directory. */
	
	char *desc;     /* Game description string. */
	char *author;   /* String containing name(s) of author(s) */
	char *homepage; /* Contains a web address for the game */
	
	/* Masks for how many overall players and bots are allowed
	   to be selected for the game. */
	unsigned char player_allow_mask;
	unsigned char bot_allow_mask;
	
	/* Are players allowed to leave mid-game?  (i.e. does the
	   game support this?) */
	unsigned char allow_leave;
	
	/* Should we automatically kill the game server (table) when the
	   last player leaves? */
	unsigned char kill_when_empty;
	
	/* Executable information: a NULL-terminated list of
	   command-line arguments for the program.  The first argument
	   is the executable itself. */
	char **exec_args;
} GameInfo;


#endif
