/*
 * Front-end functions for handling database manipulation (ggzd <-> ggzdb)
 * Copyright (C) 2000 Brent Hendricks.
 * Copyright (C) 2001 - 2008 GGZ Development Team.
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
#include <ggz.h>
#include <ggz_common.h>

#include "ggzd.h"
#include "room.h"

/* Can't include twice */
#ifndef GGZDB_VERSION_ID

/* Structures to use to pass data to front end functions */

/* Anytime structures change, bump the version id */
/* Must be fewer than 7 characters. */
#define GGZDB_VERSION_ID	"0.9"

/* For ggzdb_player_XXX */
typedef struct {
	unsigned int user_id;			/* Numeric UID */
	char handle[MAX_USER_NAME_LEN+1];	/* Players nickname/handle */
	char password[41];			/* Players password (probably hashed) */
	char name[33];				/* Players real name */
	char email[33];				/* Players email address */
	time_t last_login;			/* Last login */
	unsigned int perms;			/* Permission settings */
	unsigned int confirmed;			/* Confirmation status */
} ggzdbPlayerEntry;

typedef struct {
	unsigned int user_id;			/* Numeric UID */
	char handle[MAX_USER_NAME_LEN+1];	/* Players nickname/handle */
	char photo[65];				/* Player photo URL */
	/* TODO: add country etc. */
} ggzdbPlayerExtendedEntry;

/* Connection information */
typedef struct {
	const char *type;
	const char *option;
	const char *datadir;
	const char *host;
	int port;
	const char *database;
	const char *username;
	const char *password;
	const char *hashing;
	const char *hashencoding;
} ggzdbConnection;

typedef struct {
	/* FIXME: It would make sense to access by the UID, but
	   that wouldn't easily allow tracking stats of bots. */
	char player[MAX_USER_NAME_LEN + 1];
	char game[MAX_GAME_NAME_LEN + 1];
	int player_type;
	int wins;
	int losses;
	int ties;
	int forfeits;
	float rating;
	unsigned int ranking;
	long highest_score;
} ggzdbPlayerGameStats;

typedef struct {
	char *owner;
	char *savegame;
	int count;
	GGZSeatType *types;
	char **names;
} ggzdbSavegamePlayers;

/* Error codes */
typedef enum {
	GGZDB_NO_ERROR,		/* All's well */
	GGZDB_ERR_INIT,		/* Engine not initialized */
	GGZDB_ERR_DUPKEY,	/* Tried to overwrite on add */
	GGZDB_ERR_NOTFOUND,	/* Couldn't find a record */
	GGZDB_ERR_DB		/* Uh oh.  A database error. */
} GGZDBResult;

/* Temporal and spatial unique value */
typedef struct {
	unsigned long thread;	/* = pthread_t, i.e. 32 or 64 bits long */
	long starttime;		/* = time_t, i.e. 32 or 64 bits long */
} ggzdbStamp;

ggzdbStamp unique_thread_id(void);

/* Exported functions */
GGZReturn ggzdb_init(ggzdbConnection connection, bool standalone);
void ggzdb_close(void);

const char *ggzdb_get_default_backend(void);

GGZDBResult ggzdb_player_add(ggzdbPlayerEntry *);
GGZDBResult ggzdb_player_update(ggzdbPlayerEntry *);
GGZDBResult ggzdb_player_get(ggzdbPlayerEntry *);
GGZDBResult ggzdb_player_get_extended(ggzdbPlayerExtendedEntry *);
/* GGZDBResult ggzdb_player_delete(const char *handle); */
unsigned int ggzdb_player_next_uid(void);

GGZDBResult ggzdb_player_get_first(ggzdbPlayerEntry *pe);
GGZDBResult ggzdb_player_get_next(ggzdbPlayerEntry *pe);
void ggzdb_player_drop_cursor(void);

/* Look up a player's stats entry.  Note, you should probably use
 * stats_lookup() instead. */
GGZDBResult ggzdb_stats_lookup(ggzdbPlayerGameStats *stats);

/* Update a stats entry in the DB, adding it if necessary. */
GGZDBResult ggzdb_stats_update(ggzdbPlayerGameStats *stats);

/* Matches a password input against a (possibly hashed) password, returns 1 on
 * match, 0 on no match, -1 on error. */
int ggzdb_compare_password(const char *input, const char *password);

/* Create a new match statistics entry */
GGZDBResult ggzdb_stats_newmatch(const char *game, const char *winner, const char *savegame);

/* Register a savegame entry temporarily */
GGZDBResult ggzdb_stats_savegame(const char *game, const char *owner, const char *savegame, ggzdbStamp tableid);

/* Report the top-N players for a certain game type */
GGZDBResult ggzdb_stats_toprankings(const char *game, int number, ggzdbPlayerGameStats **rankings);

/* Recalculate statistics for a certain game type */
GGZDBResult ggzdb_stats_calcrankings(const char *game);

/* Loading saved games. The list must be deallocated afterwards. */
GGZList *ggzdb_savegames(const char *game, const char *owner);

/* Loading owners of saved games. The list must be deallocated afterwards. */
GGZList *ggzdb_savegame_owners(const char *game);

/* Report a table seat change to the database */
GGZDBResult ggzdb_savegameplayer(ggzdbStamp tableid, int seat, const char *name, int type);

/* Register all rooms */
GGZDBResult ggzdb_rooms(RoomStruct *rooms, int num);

int ggzdb_reconfiguration_fd(void);
RoomStruct* ggzdb_reconfiguration_room(void);
void ggzdb_reconfiguration_load(void);

#endif

