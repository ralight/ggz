/*
 * File: ggzdb_db4.c
 * Author: Rich Gade
 * Project: GGZ Server
 * Date: 11/10/2000
 * Desc: Back-end functions for handling the db4 sytle database
 * $Id: ggzdb_db4.c 10660 2008-12-28 00:15:59Z oojah $
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

#ifdef HAVE_CONFIG_H
# include <config.h>		/* Site specific config */
#endif

#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include <db.h>

#include "err_func.h"
#include "ggzd.h"
#include "ggzdb.h"
#include "ggzdb_proto.h"


/* Internal variables */
static DB *db_p = NULL; /* player database (table) */
static DB *db_s = NULL; /* stats database (table) */
static DB_ENV *db_e;

typedef struct {
	int standalone;
	int inmemory;
} dboptions;


/* Function to initialize the db4 database system */
GGZReturn _ggzdb_init(ggzdbConnection connection, int set_standalone)
{
	u_int32_t flags;
	dboptions *dbopt;

	/* Verify that db version is cool with us */
	const char *suffix = "/ggzdb.ver";
	char fname[strlen(connection.datadir) + strlen(suffix) + 1];
	char vid[7];	/* Space for 123.45 */
	char version_ok = 0;
	FILE *vfile;

	snprintf(fname, sizeof(fname), "%s%s", connection.datadir, suffix);

	if((vfile = fopen(fname, "r")) == NULL) {
		/* File not found, so we can create it */
		if((vfile = fopen(fname, "w")) == NULL)
			ggz_error_sys_exit("fopen(w) failed in ggzdb_init()");
		fprintf(vfile, "%s", GGZDB_VERSION_ID);
		version_ok = 1;
	} else {
		/* File was found, so let's check it */
		fgets(vid, 7, vfile);
		if(!strncmp(GGZDB_VERSION_ID, vid, strlen(GGZDB_VERSION_ID)))
			version_ok = 1;
	}
	fclose(vfile);

	if (!version_ok) {
		ggz_error_msg_exit("Bad GGZ database version %s, expected %s.\n"
		       "Most likely this means you must upgrade your\n"
		       "database schema.  It may be possible to automate this;\n"
		       "see http://ggzgamingzone.org.\n"
		       "Database location: %s\n",
		       vid, GGZDB_VERSION_ID, fname);
	}


	dbopt = ggz_malloc(sizeof(dboptions));

	if(set_standalone) {
		flags = DB_INIT_MPOOL | DB_INIT_LOCK;
		dbopt->standalone = 1;
	} else {
		flags = DB_CREATE | DB_INIT_MPOOL | DB_INIT_LOCK | DB_THREAD;
		dbopt->standalone = 0;
	}

	if(db_env_create(&db_e, 0) != 0) {
		ggz_error_sys("db_env_create() failed in _ggzdb_init()");
		return GGZ_ERROR;
	} else if (db_e->open(db_e, connection.datadir, flags , 0600) != 0) {
		ggz_error_sys("db_e->open() failed in _ggzdb_init(%s)", connection.datadir);
		return GGZ_ERROR;
	}

	if(!ggz_strcmp(connection.option, "memory")) {
		dbopt->inmemory = 1;
	} else {
		dbopt->inmemory = 0;
	}

	db_e->app_private = dbopt;

	return GGZ_OK;
}


/* Function to deinitialize the db4 database system */
void _ggzdb_close(void)
{
	/* FIXME: Check the return codes */
	if(db_p) {
		db_p->close(db_p, 0);
		db_p = NULL;
	}

	if(db_s) {
		db_s->close(db_s, 0);
		db_s = NULL;
	}

	ggz_free(db_e->app_private);

	db_e->close(db_e, 0);
	db_e = NULL;
}


/* Function to enter the database */
void _ggzdb_enter(void)
{
	/* db4 doesn't need to set locks or anything */
}


/* Function to exit the database */
void _ggzdb_exit(void)
{
	/* db4 doesn't need to free locks or anything */
}


/* Function to initialize the player table */
GGZDBResult _ggzdb_init_player(void)
{
	u_int32_t flags;
	ggzdbPlayerEntry marker;
	const char *dbname;

	if(((dboptions*)db_e->app_private)->standalone)
		flags = 0;
	else
		flags = DB_CREATE | DB_THREAD;

	if(((dboptions*)db_e->app_private)->inmemory)
		dbname = NULL;
	else
		dbname = "player.db";

	/* Open the database file */
	if(db_create(&db_p, db_e, 0) != 0)
		ggz_error_sys_exit("db_create() failed in _ggzdb_init_player()");
#if DB_VERSION_MINOR >= 1
	/*flags |= DB_AUTO_COMMIT;*/
	if (db_p->open(db_p, NULL, dbname, NULL, DB_BTREE, flags, 0600) != 0)
#else
	if (db_p->open(db_p, dbname, NULL, DB_BTREE, flags, 0600) != 0)
#endif
		ggz_error_sys_exit("db_p->open() failed in _ggzdb_init_player()");

	/* Add a marker entry for our next UID */
	marker.user_id = 1;
#if MAX_USER_NAME_LEN < 8
#error MAX_USER_NAME_LEN (see ggzd.h) must be at least 8 characters
#endif
	strcpy(marker.handle, "&nxtuid&");

	if (_ggzdb_player_add(&marker) != GGZDB_NO_ERROR) {
		/* FIXME: What to do? */
	}

	return GGZDB_NO_ERROR;
}


/* Function to add a player record */
GGZDBResult _ggzdb_player_add(ggzdbPlayerEntry *pe)
{
	int result;
	DBT key, data;

	/* Build the two DBT structures */
	memset(&key, 0, sizeof(DBT));
	memset(&data, 0, sizeof(DBT));
	key.data = pe->handle;
	key.size = strlen(pe->handle);
	data.data = pe;
	data.size = sizeof(ggzdbPlayerEntry);
	data.flags = DB_DBT_USERMEM;

	result = db_p->put(db_p, NULL, &key, &data, DB_NOOVERWRITE);
	if (result == DB_KEYEXIST)
		return GGZDB_ERR_DUPKEY;
	else if (result != 0) {
		ggz_error_sys("put failed in _ggzdb_player_add()");
		return GGZDB_ERR_DB;
	}

	/* FIXME: We won't have to do this once ggzd can exit */
	db_p->sync(db_p, 0);

	return GGZDB_NO_ERROR;
}


/* Function to retrieve a player record */
GGZDBResult _ggzdb_player_get(ggzdbPlayerEntry *pe)
{
	int result;
	DBT key, data;

	/* Build the two DBT structures */
	memset(&key, 0, sizeof(DBT));
	memset(&data, 0, sizeof(DBT));
	key.data = pe->handle;
	key.size = strlen(pe->handle);
	data.size = sizeof(ggzdbPlayerEntry);
	data.flags = DB_DBT_MALLOC;

	/* Perform the get */
	result = db_p->get(db_p, NULL, &key, &data, 0);
	if (result == DB_NOTFOUND)
		return GGZDB_ERR_NOTFOUND;
	else if (result != 0) {
		ggz_error_sys("get failed in _ggzdb_player_get()");
		return GGZDB_ERR_DB;
	}

	/* Copy it to the user data buffer */
	memcpy(pe, data.data, sizeof(ggzdbPlayerEntry));
	free(data.data); /* Allocated by db4? */

	return GGZDB_NO_ERROR;
}


/* Function to update a player record */
GGZDBResult _ggzdb_player_update(ggzdbPlayerEntry *pe)
{
	DBT key, data;

	/* Build the two DBT structures */
	memset(&key, 0, sizeof(DBT));
	memset(&data, 0, sizeof(DBT));
	key.data = pe->handle;
	key.size = strlen(pe->handle);
	data.data = pe;
	data.size = sizeof(ggzdbPlayerEntry);
	data.flags = DB_DBT_USERMEM;

	if (db_p->put(db_p, NULL, &key, &data, 0) != 0) {
		ggz_error_sys("put failed in _ggzdb_player_update()");
		return GGZDB_ERR_DB;
	}

	/* FIXME: We won't have to do this once ggzd can exit */
	db_p->sync(db_p, 0);

	return GGZDB_NO_ERROR;
}


/* Function to get and update the next uid */
unsigned int _ggzdb_player_next_uid(void)
{
	ggzdbPlayerEntry marker;
	unsigned int nxt;

	/* Get the marker entry for our next UID */
	strcpy(marker.handle, "&nxtuid&");

	/* FIXME: We really should check for "impossible" failures below */
	_ggzdb_player_get(&marker);
	nxt = marker.user_id;
	marker.user_id++;
	_ggzdb_player_update(&marker);

	return nxt;
}


/* All functions below here are NOT THREADSAFE (at least yet) */
/* All functions below here are NOT THREADSAFE (at least yet) */
/* All functions below here are NOT THREADSAFE (at least yet) */
/* All functions below here are NOT THREADSAFE (at least yet) */

static DBC *db_c = NULL;
GGZDBResult _ggzdb_player_get_first(ggzdbPlayerEntry *pe)
{
	DBT key, data;

	if(db_c == NULL
	   && db_p->cursor(db_p, NULL, &db_c, 0) != 0) {
		ggz_error_sys("Failed to create db4 cursor");
		return GGZDB_ERR_DB;
	}

	/* Build the two DBT structures */
	memset(&key, 0, sizeof(DBT));
	memset(&data, 0, sizeof(DBT));
	key.data = pe->handle;
	key.size = sizeof(pe->handle);
	data.size = sizeof(ggzdbPlayerEntry);
	data.flags = DB_DBT_MALLOC;
	if(db_c->c_get(db_c, &key, &data, DB_FIRST) != 0) {
		ggz_error_sys("Failed to get DB_FIRST record");
		return GGZDB_ERR_DB;
	}

	memcpy(pe, data.data, sizeof(ggzdbPlayerEntry));
	free(data.data); /* Allocated by db4? */

	if(!strcmp(pe->handle, "&nxtuid&"))
		return _ggzdb_player_get_next(pe);
	return GGZDB_NO_ERROR;
}


GGZDBResult _ggzdb_player_get_next(ggzdbPlayerEntry *pe)
{
	int result;
	DBT key, data;

	if(db_c == NULL)
		ggz_error_sys_exit("get_first should be called before get_next");

	/* Build the two DBT structures */
	memset(&key, 0, sizeof(DBT));
	memset(&data, 0, sizeof(DBT));
	key.data = pe->handle;
	key.size = sizeof(pe->handle);
	data.size = sizeof(ggzdbPlayerEntry);
	data.flags = DB_DBT_MALLOC;
	result = db_c->c_get(db_c, &key, &data, DB_NEXT);
	if (result == DB_NOTFOUND)
		return GGZDB_ERR_NOTFOUND;
	else if (result != 0) {
		ggz_error_sys("Failed to get DB_NEXT record");
		return GGZDB_ERR_DB;
	}

	memcpy(pe, data.data, sizeof(ggzdbPlayerEntry));
	free(data.data); /* Allocated by db4? */

	return GGZDB_NO_ERROR;
}


void _ggzdb_player_drop_cursor(void)
{
	if(db_c == NULL)
		ggz_error_sys_exit("get_first should be called before drop_cursor");
	db_c->c_close(db_c);
	db_c = NULL;
}


GGZDBResult _ggzdb_init_stats(void)
{
	u_int32_t flags;
	const char *dbname;

	if(((dboptions*)db_e->app_private)->standalone)
		flags = 0;
	else
		flags = DB_CREATE | DB_THREAD;

	if(((dboptions*)db_e->app_private)->inmemory)
		dbname = NULL;
	else
		dbname = "stats.db";

	/* Open the database file */
	if (db_create(&db_s, db_e, 0) != 0)
		ggz_error_sys_exit("db_create() failed in _ggzdb_init_stats()");
#if DB_VERSION_MINOR >= 1
	/*flags |= DB_AUTO_COMMIT;*/
	if (db_s->open(db_s, NULL, dbname, NULL, DB_BTREE, flags, 0600) != 0)
#else
	if (db_s->open(db_s, dbname, NULL, DB_BTREE, flags, 0600) != 0)
#endif
		ggz_error_sys_exit("db_s->open() failed in _ggzdb_init_stats()");

	return GGZDB_NO_ERROR;
}


GGZDBResult _ggzdb_stats_lookup(ggzdbPlayerGameStats *stats)
{
	DBT key, data;
	int result;
	char key_string[MAX_USER_NAME_LEN + MAX_GAME_NAME_LEN + 2];

	snprintf(key_string, sizeof(key_string),
		 "%s|%s", stats->player, stats->game);

	/* Build the two DBT structures */
	memset(&key, 0, sizeof(key));
	memset(&data, 0, sizeof(data));
	key.data = key_string;
	key.size = strlen(key_string);
	data.size = sizeof(*stats);
	data.flags = DB_DBT_MALLOC;

	result = db_s->get(db_s, NULL, &key, &data, 0);
	if (result == DB_NOTFOUND)
		return GGZDB_ERR_NOTFOUND;
	else if (result != 0) {
		ggz_error_sys("get failed in _ggzdb_stats_lookup()");
		return GGZDB_ERR_DB;
	}

	/* Copy it to the user data buffer */
	memcpy(stats, data.data, sizeof(*stats));
	free(data.data); /* Allocated in get() */

	return GGZDB_NO_ERROR;
}

GGZDBResult _ggzdb_stats_update(ggzdbPlayerGameStats *stats)
{
	DBT key, data;
	char key_string[MAX_USER_NAME_LEN + MAX_GAME_NAME_LEN + 2];

	snprintf(key_string, sizeof(key_string),
		 "%s|%s", stats->player, stats->game);

	/* Build the two DBT structures */
	memset(&key, 0, sizeof(key));
	memset(&data, 0, sizeof(data));
	key.data = key_string;
	key.size = strlen(key_string);
	data.data = stats;
	data.size = sizeof(*stats);
	data.flags = DB_DBT_USERMEM;

	if (db_s->put(db_s, NULL, &key, &data, 0) != 0) {
		ggz_error_sys("put failed in _ggzdb_stats_update()");
		return GGZDB_ERR_DB;
	}

	/* FIXME: We won't have to do this once ggzd can exit */
	db_s->sync(db_s, 0);

	return GGZDB_ERR_DB;
}

GGZDBResult _ggzdb_stats_match(ggzdbPlayerGameStats *stats)
{
	/* Not implemented, but do not return error */
	return GGZDB_NO_ERROR;
}

GGZDBResult _ggzdb_stats_newmatch(const char *game, const char *winner, const char *savegame)
{
	/* Not implemented, but do not return error */
	return GGZDB_NO_ERROR;
}

GGZDBResult _ggzdb_stats_savegame(const char *game, const char *owner, const char *savegame, ggzdbStamp tableid)
{
	/* Not implemented, but do not return error */
	return GGZDB_NO_ERROR;
}

GGZDBResult _ggzdb_player_get_extended(ggzdbPlayerExtendedEntry *pe)
{
	/* Do return error here so we can clear out the values! */
	return GGZDB_ERR_DB;
}

GGZDBResult _ggzdb_stats_toprankings(const char *game, int number, ggzdbPlayerGameStats **rankings)
{
	/* Not implemented, but do not return error */
	return GGZDB_NO_ERROR;
}

GGZDBResult _ggzdb_stats_calcrankings(const char *game)
{
	/* Not implemented, but do not return error */
	return GGZDB_NO_ERROR;
}

GGZList *_ggzdb_savegames(const char *game, const char *owner)
{
	/* Not implemented, but do not return error */
	return NULL;
}

GGZList *_ggzdb_savegame_owners(const char *game)
{
	/* Not implemented, but do not return error */
	return NULL;
}

GGZDBResult _ggzdb_savegame_player(ggzdbStamp tableid, int seat, const char *name, int type)
{
	/* Not implemented, but do not return error */
	return GGZDB_NO_ERROR;
}

GGZDBResult _ggzdb_rooms(RoomStruct *rooms, int num)
{
	/* Not implemented, but do not return error */
	return GGZDB_NO_ERROR;
}

int _ggzdb_reconfiguration_fd(void)
{
	/* Not implemented, but do not return error */
	return -1;
}

void _ggzdb_reconfiguration_load(void)
{
	/* Not implemented, but do not return error */
}

RoomStruct *_ggzdb_reconfiguration_room(void)
{
	/* Not implemented, but do not return error */
	return NULL;
}

/* These are not needed.
char *_ggzdb_escape(const char *str)
char *_ggzdb_unescape(const char *str)
*/
