/*
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

#include <stdlib.h>
#include <string.h>

#include <sqlite3.h>

#include "err_func.h"
#include "ggzd.h"
#include "ggzdb.h"
#include "ggzdb_proto.h"
#include "ggzdb_util.h"


/* Internal variables */
static sqlite3 *conn = NULL;
static sqlite3_stmt *res = NULL;


/* Function to initialize the database system */
GGZReturn _ggzdb_init(ggzdbConnection connection, int set_standalone)
{
	int rc;
	char query[4096];
	bool created = true;

	if(conn) return GGZ_OK;

	sqlite3_snprintf(sizeof(query), query, "%q/ggzd.sqlite", connection.datadir);
	rc = sqlite3_open(query, &conn);
	
	if(rc != SQLITE_OK)
	{
		ggz_error_msg("Couldn't initialize database.");
		return GGZ_ERROR;
	}

	sqlite3_snprintf(sizeof(query), query, "SELECT value FROM control WHERE key = 'version'");

	rc = sqlite3_prepare(conn, query, strlen(query), &res, NULL);

	if (rc != SQLITE_OK)
	{
		sqlite3_snprintf(sizeof(query), query, "CREATE TABLE users "
			"(id INTEGER PRIMARY KEY AUTOINCREMENT, handle TEXT, password TEXT, "
			"name TEXT, email TEXT, lastlogin INT, permissions INT)");

		rc = sqlite3_exec(conn, query, NULL, NULL, NULL);

		if(rc != SQLITE_OK)
			created = false;

		sqlite3_snprintf(sizeof(query), query, "CREATE TABLE control "
			"(key varchar(256), value varchar(256))");

		rc = sqlite3_exec(conn, query, NULL, NULL, NULL);

		if(rc != SQLITE_OK)
			created = false;

		sqlite3_snprintf(sizeof(query), query, "INSERT INTO control "
			"(key, value) VALUES ('version', '%q')", GGZDB_VERSION_ID);

		rc = sqlite3_exec(conn, query, NULL, NULL, NULL);

		if(rc != SQLITE_OK)
			created = false;

		if(!created)
		{
			ggz_error_msg("Couldn't create database tables.");
		}
	}

	/* Hack. */
	return GGZ_OK;
}


/* Function to deinitialize the database system */
void _ggzdb_close(void)
{
	if(!conn) return;

	sqlite3_close(conn);
	conn = NULL;
}


/* Function to enter the database */
void _ggzdb_enter(void)
{
	/*mysql_thread_init();*/
}


/* Function to exit the database */
void _ggzdb_exit(void)
{
	/*mysql_thread_end();*/
}


/* Function to initialize the player table */
GGZDBResult _ggzdb_init_player(void)
{
	return GGZDB_NO_ERROR;
}


/* Function to add a player record */
GGZDBResult _ggzdb_player_add(ggzdbPlayerEntry *pe)
{
	int rc;
	char query[4096];
	char *handle_canonical;

	handle_canonical = username_canonical(pe->handle);

	sqlite3_snprintf(sizeof(query), query, "INSERT INTO users "
		"(handle, password, name, email, lastlogin, permissions) VALUES "
		"('%q', '%q', '%q', '%q', %li, %u)",
		handle_canonical, pe->password, pe->name, pe->email, pe->last_login, pe->perms);
	
	ggz_free(handle_canonical);

	rc = sqlite3_exec(conn, query, NULL, NULL, NULL);

	if (rc != SQLITE_OK) {
		/* FIXME: is this correct?  If not, how do we detect a
		   duplicate entry - and notify the calling code?  --JDS */
		return GGZDB_ERR_DUPKEY;
	}

	return GGZDB_NO_ERROR;
}


/* Function to retrieve a player record */
GGZDBResult _ggzdb_player_get(ggzdbPlayerEntry *pe)
{
	int rc;
	char query[4096];
	char *handle_canonical;

	handle_canonical = username_canonical(pe->handle);

	sqlite3_snprintf(sizeof(query), query, "SELECT "
		"password, name, email, lastlogin, permissions FROM users WHERE "
		"handle = '%q'",
		handle_canonical);

	ggz_free(handle_canonical);

	rc = sqlite3_prepare(conn, query, strlen(query), &res, NULL);

	if (rc == SQLITE_OK) {
		rc = sqlite3_step(res);

		if (rc == SQLITE_ROW) {
			strncpy(pe->password, (char*)sqlite3_column_text(res, 0), sizeof(pe->password));
			strncpy(pe->name, (char*)sqlite3_column_text(res, 1), sizeof(pe->name));
			strncpy(pe->email, (char*)sqlite3_column_text(res, 2), sizeof(pe->email));
			pe->last_login = sqlite3_column_int(res, 3);
			pe->perms = sqlite3_column_int(res, 4);
			return GGZDB_NO_ERROR;
		} else {
			/* This is supposed to happen if we look up a
			   nonexistent player. */
			return GGZDB_ERR_NOTFOUND;
		}
	} else {
		ggz_error_msg("Couldn't lookup player.");
		return GGZDB_ERR_DB;
	}
}


/* Function to update a player record */
GGZDBResult _ggzdb_player_update(ggzdbPlayerEntry *pe)
{
	int rc;
	char query[4096];
	char *handle_canonical;

	handle_canonical = username_canonical(pe->handle);


	sqlite3_snprintf(sizeof(query), query, "UPDATE users SET "
		"password = '%q', name = '%q', email = '%q', lastlogin = %li, permissions = %u WHERE "
		"handle = '%q'",
		pe->password, pe->name, pe->email, pe->last_login, pe->perms, handle_canonical);

	ggz_free(handle_canonical);

	rc = sqlite3_exec(conn, query, NULL, NULL, NULL);

	if(rc != SQLITE_OK) {
		ggz_error_msg("Couldn't update player.");
		return GGZDB_ERR_DB;
	}

	return GGZDB_NO_ERROR;
}


/* All functions below here are NOT THREADSAFE (at least yet) */
/* All functions below here are NOT THREADSAFE (at least yet) */
/* All functions below here are NOT THREADSAFE (at least yet) */
/* All functions below here are NOT THREADSAFE (at least yet) */

GGZDBResult _ggzdb_player_get_first(ggzdbPlayerEntry *pe)
{
	int result;
	char query[4096];

	sqlite3_snprintf(sizeof(query), query, "SELECT "
		"id, handle, password, name, email, lastlogin, permissions FROM users");

	result = sqlite3_prepare(conn, query, strlen(query), &res, NULL);

	if (result == SQLITE_OK) {
		result = sqlite3_step(res);

		if (result == SQLITE_ROW) {
			pe->user_id = sqlite3_column_int(res, 0);
			strncpy(pe->handle, (char*)sqlite3_column_text(res, 1), sizeof(pe->handle));
			strncpy(pe->password, (char*)sqlite3_column_text(res, 2), sizeof(pe->password));
			strncpy(pe->name, (char*)sqlite3_column_text(res, 3), sizeof(pe->name));
			strncpy(pe->email, (char*)sqlite3_column_text(res, 5), sizeof(pe->email));
			pe->last_login = sqlite3_column_int(res, 5);
			pe->perms = sqlite3_column_int(res, 6);
			return GGZDB_NO_ERROR;
		} else {
			ggz_error_msg("No entries found.");
			return GGZDB_NO_ERROR;
		}
	} else {
		ggz_error_msg("Couldn't lookup player.");
		return GGZDB_ERR_DB;
	}
}


GGZDBResult _ggzdb_player_get_next(ggzdbPlayerEntry *pe)
{
	int result = sqlite3_step(res);

	if(result == SQLITE_ROW) {
		pe->user_id = sqlite3_column_int(res, 0);
		strncpy(pe->handle, (char*)sqlite3_column_text(res, 1), sizeof(pe->handle));
		strncpy(pe->password, (char*)sqlite3_column_text(res, 2), sizeof(pe->password));
		strncpy(pe->name, (char*)sqlite3_column_text(res, 3), sizeof(pe->name));
		strncpy(pe->email, (char*)sqlite3_column_text(res, 4), sizeof(pe->email));
		pe->last_login = sqlite3_column_int(res, 5);
		pe->perms = sqlite3_column_int(res, 6);
		return GGZDB_NO_ERROR;
	} else {
		result = sqlite3_finalize(res);
		res = NULL;
		return GGZDB_ERR_NOTFOUND;
	}
}


void _ggzdb_player_drop_cursor(void)
{
}

unsigned int _ggzdb_player_next_uid(void)
{
	/* SQL handles id's automatically */
	return 0;
}

GGZDBResult _ggzdb_init_stats(void)
{
	/* Not implemented, but do not return error */
	return GGZDB_NO_ERROR;
}

GGZDBResult _ggzdb_stats_update(ggzdbPlayerGameStats *stats)
{
	/* Not implemented, but do not return error */
	return GGZDB_NO_ERROR;
}

GGZDBResult _ggzdb_stats_lookup(ggzdbPlayerGameStats *stats)
{
	/* Not implemented, but do not return error */
	return GGZDB_NO_ERROR;
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

/* These are not needed - use the sqlite3_snprintf() function with %q instead of %s to do escaping.
char *_ggzdb_escape(const char *str)
char *_ggzdb_unescape(const char *str)
*/
