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
#include <pthread.h>

#include <dbi/dbi.h>

#include "err_func.h"
#include "ggzd.h"
#include "ggzdb.h"
#include "ggzdb_proto.h"


/* Internal variables */
static dbi_conn conn = NULL;
static dbi_result iterresult = NULL;
static pthread_mutex_t mutex;

/* Function to initialize the database system */
GGZReturn _ggzdb_init(ggzdbConnection connection, int set_standalone)
{
	int rc;
	int numdrivers;

	if(conn) return GGZ_OK;

	if(!connection.option)
	{
		ggz_error_msg("Couldn't initialize database: no DBI type given.");
		return GGZ_ERROR;
	}

	numdrivers = dbi_initialize(NULL);
	if(numdrivers == -1)
	{
		ggz_error_msg("Couldn't initialize DBI drivers.");
		return GGZ_ERROR;
	}

	conn = dbi_conn_new(connection.option);
	if(conn == NULL)
	{
		ggz_error_msg("Could not load DBI connection driver of type '%s'.",
			connection.option);
		return GGZ_ERROR;
	}

	rc = 0;
	rc |= dbi_conn_set_option(conn, "host", connection.host);
	rc |= dbi_conn_set_option(conn, "username", connection.username);
	rc |= dbi_conn_set_option(conn, "password", connection.password);
	rc |= dbi_conn_set_option(conn, "dbname", connection.database);
	if(rc != 0)
	{
		ggz_error_msg("Could not set connection options.");
		return GGZ_ERROR;
	}

	rc = dbi_conn_connect(conn);
	if(rc != 0)
	{
		ggz_error_msg("Could not connect to the database.");
		return GGZ_ERROR;
	}

	pthread_mutex_init(&mutex, NULL);

	return GGZ_OK;
}


/* Function to deinitialize the database system */
void _ggzdb_close(void)
{
	if(!conn) return;

	dbi_conn_close(conn);
	dbi_shutdown();

	conn = NULL;
}


/* Function to enter the database */
void _ggzdb_enter(void)
{
	pthread_mutex_lock(&mutex);
}


/* Function to exit the database */
void _ggzdb_exit(void)
{
	pthread_mutex_unlock(&mutex);
}


/* Function to initialize the player table */
GGZDBResult _ggzdb_init_player(void)
{
	return GGZDB_NO_ERROR;
}


/* Function to add a player record */
GGZDBResult _ggzdb_player_add(ggzdbPlayerEntry *pe)
{
	dbi_result result;
	char query[4096];
	char *handle_quoted;
	char *password_quoted;
	char *name_quoted;
	char *email_quoted;

	handle_quoted = _ggzdb_escape(pe->handle);
	password_quoted = _ggzdb_escape(pe->password);
	name_quoted = _ggzdb_escape(pe->name);
	email_quoted = _ggzdb_escape(pe->email);

	snprintf(query, sizeof(query), "INSERT INTO users "
		"(handle, password, name, email, lastlogin, permissions, firstlogin) VALUES "
		"('%s', '%s', '%s', '%s', %li, %u, %li)",
		handle_quoted, password_quoted, name_quoted, email_quoted, pe->last_login, pe->perms,
		time(NULL));
	
	free(email_quoted);
	free(name_quoted);
	free(password_quoted);
	free(handle_quoted);

	result = dbi_conn_query(conn, query);

	if (!result) {
		return GGZDB_ERR_DUPKEY;
	}

	dbi_result_free(result);

	return GGZDB_NO_ERROR;
}


/* Function to retrieve a player record */
GGZDBResult _ggzdb_player_get(ggzdbPlayerEntry *pe)
{
	int rc;
	dbi_result result;
	char query[4096];
	int row;
	char *handle_quoted;

	handle_quoted = _ggzdb_escape(pe->handle);

	snprintf(query, sizeof(query), "SELECT "
		"id, password, name, email, lastlogin, permissions FROM users WHERE "
		"handle = '%s'",
		handle_quoted);
	
	free(handle_quoted);

	result = dbi_conn_query(conn, query);

	if (!result) {
		ggz_error_msg("Couldn't lookup player.");
		return GGZDB_ERR_DB;
	}

	/* FIXME: DBI doesn't provide field binding to pre-allocated space */
	/*rc = dbi_result_bind_fields(result,
		"password.%s name.%s email.%s lastlogin.%i permissions.%i",
		pe->password, pe->name, pe->email, &pe->last_login, &pe->perms);
	if(rc == -1)
	{
		ggz_error_msg("Column binding failed.");
		return GGZDB_ERR_DB;
	}*/

	row = dbi_result_first_row(result);

	if (row) {
		pe->user_id = dbi_result_get_long(result, "id");
		strncpy(pe->password, dbi_result_get_string(result, "password"), sizeof(pe->password));
		strncpy(pe->name, dbi_result_get_string(result, "name"), sizeof(pe->name));
		strncpy(pe->email, dbi_result_get_string(result, "email"), sizeof(pe->email));
		pe->last_login = dbi_result_get_longlong(result, "lastlogin");
		pe->perms = dbi_result_get_longlong(result, "permissions");
		rc = GGZDB_NO_ERROR;
	} else {
		rc = GGZDB_ERR_NOTFOUND;
	}

	dbi_result_free(result);

	return rc;
}


/* Function to update a player record */
GGZDBResult _ggzdb_player_update(ggzdbPlayerEntry *pe)
{
	dbi_result result;
	char query[4096];
	char *handle_quoted;
	char *password_quoted;
	char *name_quoted;
	char *email_quoted;

	handle_quoted = _ggzdb_escape(pe->handle);
	password_quoted = _ggzdb_escape(pe->password);
	name_quoted = _ggzdb_escape(pe->name);
	email_quoted = _ggzdb_escape(pe->email);

	snprintf(query, sizeof(query), "UPDATE users SET "
		"password = '%s', name = '%s', email = '%s', lastlogin = %li, permissions = %u WHERE "
		"handle = '%s'",
		password_quoted, name_quoted, email_quoted, pe->last_login, pe->perms, handle_quoted);

	free(email_quoted);
	free(name_quoted);
	free(password_quoted);
	free(handle_quoted);

	result = dbi_conn_query(conn, query);

	if (!result) {
		ggz_error_msg("Couldn't update player.");
		return GGZDB_ERR_DB;
	}

	dbi_result_free(result);

	return GGZDB_NO_ERROR;
}


/* All functions below here are NOT THREADSAFE (at least yet) */
/* All functions below here are NOT THREADSAFE (at least yet) */
/* All functions below here are NOT THREADSAFE (at least yet) */
/* All functions below here are NOT THREADSAFE (at least yet) */

GGZDBResult _ggzdb_player_get_first(ggzdbPlayerEntry *pe)
{
	char query[4096];
	int row;
	int rc;

	if (iterresult) {
		dbi_result_free(iterresult);
	}

	snprintf(query, sizeof(query), "SELECT "
		"id, handle, password, name, email, lastlogin, permissions FROM users");

	iterresult = dbi_conn_query(conn, query);

	if (!iterresult) {
		ggz_error_msg("Couldn't lookup player.");
		return GGZDB_ERR_DB;
	}

	row = dbi_result_first_row(iterresult);

	if (row) {
		pe->user_id = dbi_result_get_long(iterresult, "id");
		strncpy(pe->handle, dbi_result_get_string(iterresult, "handle"), sizeof(pe->handle));
		strncpy(pe->password, dbi_result_get_string(iterresult, "password"), sizeof(pe->password));
		strncpy(pe->name, dbi_result_get_string(iterresult, "name"), sizeof(pe->name));
		strncpy(pe->email, dbi_result_get_string(iterresult, "email"), sizeof(pe->email));
		pe->last_login = dbi_result_get_longlong(iterresult, "last_login");
		pe->perms = dbi_result_get_longlong(iterresult, "permissions");
		rc = GGZDB_NO_ERROR;
	} else {
		rc = GGZDB_ERR_NOTFOUND;
	}

	return rc;
}


GGZDBResult _ggzdb_player_get_next(ggzdbPlayerEntry *pe)
{
	int row;

	row = dbi_result_next_row(iterresult);

	if (row) {
		pe->user_id = dbi_result_get_long(iterresult, "id");
		strncpy(pe->handle, dbi_result_get_string(iterresult, "handle"), sizeof(pe->handle));
		strncpy(pe->password, dbi_result_get_string(iterresult, "password"), sizeof(pe->password));
		strncpy(pe->name, dbi_result_get_string(iterresult, "name"), sizeof(pe->name));
		strncpy(pe->email, dbi_result_get_string(iterresult, "email"), sizeof(pe->email));
		pe->last_login = dbi_result_get_longlong(iterresult, "last_login");
		pe->perms = dbi_result_get_longlong(iterresult, "permissions");
		return GGZDB_NO_ERROR;
	} else {
		dbi_result_free(iterresult);
		iterresult = NULL;
		return GGZDB_ERR_NOTFOUND;
	}

	return GGZDB_NO_ERROR;
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

char *_ggzdb_escape(const char *str)
{
	char *to;

	if(!str) return strdup("");

	if(dbi_conn_quote_string_copy(conn, str, &to) > 0){
		return to;
	}else{
		return NULL;
	}
}

/* These are not needed.
char *_ggzdb_unescape(const char *str)
*/
