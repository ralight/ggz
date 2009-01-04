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
static sqlite3_stmt *iterres = NULL;

/* Internal functions */

static int ggz_sqlite_exec(void *dbconn, const char *query, char *error, size_t errorlen)
{
	int rc = 1;
	int sqlite_rc;
	char *sqlerror = NULL;

	sqlite_rc = sqlite3_exec(conn, query, NULL, NULL, &sqlerror);

	if(sqlite_rc != SQLITE_OK)
	{
		strncpy(error, sqlerror, errorlen);
		rc = 0;
	}
	if(sqlerror)
	{
		sqlite3_free(sqlerror);
		sqlerror = NULL;
	}

	return rc;
}

/* Upgrade a database */
static int upgrade(const char *oldversion, const char *newversion)
{
	char upgradefile[1024];

	snprintf(upgradefile, sizeof(upgradefile), "%s/sqlite_upgrade_%s_%s.sql",
		GGZDDATADIR, oldversion, newversion);

	return _ggz_setupschema(upgradefile, ggz_sqlite_exec, conn);
}


/* Function to initialize the database system */
GGZReturn _ggzdb_init(ggzdbConnection connection, int set_standalone)
{
	int rc;
	char query[4096];
	sqlite3_stmt *res = NULL;
	int init;
	char version[100];
	char schemafile[1024];
	int sqlite_rc;

	if(conn) return GGZ_OK;

	sqlite3_snprintf(sizeof(query), query, "%q/ggzd.sqlite", connection.datadir);
	rc = sqlite3_open(query, &conn);
	
	if(rc != SQLITE_OK)
	{
		ggz_error_msg("Couldn't initialize database.");
		return GGZ_ERROR;
	}

	/* Check whether the database is ok */
	init = 1;
	sqlite3_snprintf(sizeof(query), query, "SELECT `value` FROM `control` WHERE `key` = 'version'");
	sqlite_rc = sqlite3_prepare_v2(conn, query, strlen(query), &res, NULL);
	if(sqlite_rc == SQLITE_OK)
	{
		sqlite_rc = sqlite3_step(res);
		if(sqlite_rc == SQLITE_ROW)
		{
			init = 0;
			strncpy(version, (char*)sqlite3_column_text(res, 0), sizeof(version));
			if(strcmp(version, GGZDB_VERSION_ID))
			{
				/* Perform upgrade if possible */
				ggz_error_msg("Wrong database version: %s present, %s needed.\n", version, GGZDB_VERSION_ID);
				if(!upgrade(version, GGZDB_VERSION_ID))
				{
					ggz_error_msg_exit("Database upgrade failed.\n");
					rc = GGZDB_ERR_DB;
				}
			}
		}else{
			ggz_error_msg_exit("Error whilst talking to sqlite table `control`. This probably means there is a problem with your database. Please contact the ggz developers for help.\n");
		}
		sqlite3_finalize(res);
	}

	/* Initialize the database if needed */
	if(init)
	{
		snprintf(schemafile, sizeof(schemafile), "%s/sqlite_schema.sql", GGZDDATADIR);

		if(!_ggz_setupschema(schemafile, ggz_sqlite_exec, conn))
			rc = GGZDB_ERR_DB;

		sqlite3_snprintf(sizeof(query), query, "INSERT INTO `control` "
			"(`key`, `value`) VALUES ('version', '%q')", GGZDB_VERSION_ID);

		sqlite_rc = sqlite3_exec(conn, query, NULL, NULL, NULL);

		if(sqlite_rc != SQLITE_OK)
			rc = GGZDB_ERR_DB;

		if(rc == GGZDB_ERR_DB)
			ggz_error_msg_exit("Could not initialize the database (with %s).\n", schemafile);
	}

	return rc;
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

	sqlite3_snprintf(sizeof(query), query, "INSERT INTO `users` "
		"(`handle`, `password`, `name`, `email`, `lastlogin`, `permissions`, `firstlogin`, `confirmed`) VALUES "
		"('%q', '%q', '%q', '%q', %li, %u, %li, %u)",
		handle_canonical, pe->password, pe->name, pe->email, pe->last_login, pe->perms, time(NULL), pe->confirmed);
	
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
	sqlite3_stmt *res = NULL;

	handle_canonical = username_canonical(pe->handle);

	sqlite3_snprintf(sizeof(query), query, "SELECT "
		"`id`, `password`, `name`, `email`, `lastlogin`, `permissions`, `confirmed` FROM `users` WHERE "
		"`handle` = '%q'",
		handle_canonical);

	ggz_free(handle_canonical);

	rc = sqlite3_prepare(conn, query, strlen(query), &res, NULL);

	if (rc == SQLITE_OK) {
		rc = sqlite3_step(res);

		if (rc == SQLITE_ROW) {
			pe->user_id = sqlite3_column_int(res, 0);
			strncpy(pe->password, (char*)sqlite3_column_text(res, 1), sizeof(pe->password));
			strncpy(pe->name, (char*)sqlite3_column_text(res, 2), sizeof(pe->name));
			strncpy(pe->email, (char*)sqlite3_column_text(res, 3), sizeof(pe->email));
			pe->last_login = sqlite3_column_int(res, 4);
			pe->perms = sqlite3_column_int(res, 5);
			pe->confirmed = sqlite3_column_int(res, 6);
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


	sqlite3_snprintf(sizeof(query), query,
		"UPDATE `users` SET "
		"`password` = '%q', `name` = '%q', `email` = '%q', "
		"`lastlogin` = %li, `permissions` = %u, `confirmed` = %u WHERE "
		"`handle` = '%q'",
		pe->password, pe->name, pe->email,
		pe->last_login, pe->perms, pe->confirmed,
		handle_canonical);

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

	sqlite3_snprintf(sizeof(query), query,
		"SELECT "
		"`id`, `handle`, `password`, `name`, `email`, `lastlogin`, `permissions`,`confirmed` "
		"FROM `users`");

	result = sqlite3_prepare(conn, query, strlen(query), &iterres, NULL);

	if (result == SQLITE_OK) {
		result = sqlite3_step(iterres);

		if (result == SQLITE_ROW) {
			pe->user_id = sqlite3_column_int(iterres, 0);
			strncpy(pe->handle, (char*)sqlite3_column_text(iterres, 1), sizeof(pe->handle));
			strncpy(pe->password, (char*)sqlite3_column_text(iterres, 2), sizeof(pe->password));
			strncpy(pe->name, (char*)sqlite3_column_text(iterres, 3), sizeof(pe->name));
			strncpy(pe->email, (char*)sqlite3_column_text(iterres, 4), sizeof(pe->email));
			pe->last_login = sqlite3_column_int(iterres, 5);
			pe->perms = sqlite3_column_int(iterres, 6);
			pe->confirmed = sqlite3_column_int(iterres, 7);
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
	int result = sqlite3_step(iterres);

	if(result == SQLITE_ROW) {
		pe->user_id = sqlite3_column_int(iterres, 0);
		strncpy(pe->handle, (char*)sqlite3_column_text(iterres, 1), sizeof(pe->handle));
		strncpy(pe->password, (char*)sqlite3_column_text(iterres, 2), sizeof(pe->password));
		strncpy(pe->name, (char*)sqlite3_column_text(iterres, 3), sizeof(pe->name));
		strncpy(pe->email, (char*)sqlite3_column_text(iterres, 4), sizeof(pe->email));
		pe->last_login = sqlite3_column_int(iterres, 5);
		pe->perms = sqlite3_column_int(iterres, 6);
		pe->confirmed = sqlite3_column_int(iterres, 7);
		return GGZDB_NO_ERROR;
	} else {
		result = sqlite3_finalize(iterres);
		iterres = NULL;
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
