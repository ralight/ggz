/*
 * File: ggzdb.h
 * Author: Rich Gade
 * Project: GGZ Server
 * Date: 06/11/2000
 * Desc: Front-end functions for handling database manipulation
 * $Id: ggzdb.h 5921 2004-02-14 18:38:55Z jdorje $
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

#include <time.h>

#include "ggzd.h"

/* Can't include twice */
#ifndef GGZDB_VERSION_ID

/* Structures to use to pass data to front end functions */

/* Anytime structures change, bump the version id */
/* Must be fewer than 7 characters. */
#define GGZDB_VERSION_ID	"0.6r1"

/* For ggzdb_player_XXX */
typedef struct {
	unsigned int user_id;			/* Numeric UID */
	char handle[MAX_USER_NAME_LEN+1];	/* Players nickname/handle */
	char password[33];			/* Players password (probably hashed) */
	char name[33];				/* Players real name */
	char email[33];				/* Players email address */
	time_t last_login;			/* Last login */
	unsigned int perms;			/* Permission settings */
} ggzdbPlayerEntry;

/* Connection information */
typedef struct {
	char *datadir;
	char *host;
	char *database;
	char *username;
	char *password;
	char *hashing;
} ggzdbConnection;

typedef struct {
	/* FIXME: It would make sense to access by the UID, but
	   that wouldn't easily allow tracking stats of bots. */
	char player[MAX_USER_NAME_LEN + 1];
	char game[MAX_GAME_NAME_LEN + 1];
	int wins;
	int losses;
	int ties;
	int forfeits;
	float rating;
	unsigned int ranking;
	long highest_score;
} ggzdbPlayerGameStats;


/* Error codes */
typedef enum {
	GGZDB_NO_ERROR,		/* All's well */
	GGZDB_ERR_INIT,		/* Engine not initialized */
	GGZDB_ERR_DUPKEY,	/* Tried to overwrite on add */
	GGZDB_ERR_NOTFOUND,	/* Couldn't find a record */
	GGZDB_ERR_DB		/* Uh oh.  A database error. */
} GGZDBResult;


/* Exported functions */
GGZReturn ggzdb_init(void);
void ggzdb_close(void);

GGZDBResult ggzdb_player_add(ggzdbPlayerEntry *);
GGZDBResult ggzdb_player_update(ggzdbPlayerEntry *);
GGZDBResult ggzdb_player_get(ggzdbPlayerEntry *);
/* GGZDBResult ggzdb_player_delete(const char *handle); */
unsigned int ggzdb_player_next_uid(void);

/* Look up a player's stats entry.  Note, you should probably use
 * stats_lookup() instead. */
GGZDBResult ggzdb_stats_lookup(ggzdbPlayerGameStats *stats);

/* Update a stats entry in the DB, adding it if necessary. */
GGZDBResult ggzdb_stats_update(ggzdbPlayerGameStats *stats);

/* Matches a password input against a (possibly hashed) password, returns 1 on
 * match, 0 on no match, -1 on error. */
int ggzdb_compare_password(const char *input, const char *password);

#endif

