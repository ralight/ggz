/*
 * File: ggzdb_pgsql.c
 * Author: Josef Spillner
 * Project: GGZ Server
 * Date: 02.05.2002
 * Desc: Back-end functions for handling the postgresql style database
 * $Id: ggzdb_pgsql.c 10105 2008-06-29 13:40:20Z oojah $
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
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <ggz.h>
#ifdef PGSQL_IN_PGSQLDIR
#include <pgsql/libpq-fe.h>
#else
#include <postgresql/libpq-fe.h>
#endif

#include "err_func.h"
#include "ggzd.h"
#include "ggzdb.h"
#include "ggzdb_proto.h"
#include "ggzdb_util.h"
#include "rankings.h"


#define SQL_MAXCONNECTIONS 10
#define SQL_TIMEOUT 3

/* Internal variables */
static PGresult *iterres = NULL;
static int itercount;
static int pg_canonicalstr;

static pthread_mutex_t mutex;
static GGZList *list;

static const char *dbhost;
static const char *dbname;
static const char *dbusername;
static const char *dbpassword;
static int dbport;

static PGconn *reconfigurationconn = NULL;

/* Entry type for the connection pool */
typedef struct connection_t
{
	PGconn *conn;
	int used;
}
connection_t;

/* Internal functions */

/* Take a connection from the pool */
static PGconn *claimconnection()
{
	GGZListEntry *entry;
	connection_t *conn;
	int i = 0;
	time_t timeout;
	char conninfo[4096];

	timeout = time(NULL) + SQL_TIMEOUT;
	while(time(NULL) < timeout)
	{
		pthread_mutex_lock(&mutex);
		entry = ggz_list_head(list);
		while(entry)
		{
			conn = ggz_list_get_data(entry);
			if(!conn->used)
			{
				conn->used = 1;
				pthread_mutex_unlock(&mutex);
				if(PQstatus(conn->conn) == CONNECTION_BAD)
				{
					ggz_error_msg("Database connection lost, reconnecting.");
					PQreset(conn->conn);
					if((!conn->conn) || (PQstatus(conn->conn) == CONNECTION_BAD))
					{
						ggz_error_msg("Could not connect to database.");
						return NULL;
					}
				}
				return conn->conn;
			}
			entry = ggz_list_next(entry);
			i++;
		}
		pthread_mutex_unlock(&mutex);

		pthread_mutex_lock(&mutex);
		if(ggz_list_count(list) < SQL_MAXCONNECTIONS)
		{
			conn = ggz_malloc(sizeof(*conn));
			snprintf(conninfo, sizeof(conninfo),
				 "host=%s port=%i dbname=%s user=%s password=%s",
				 dbhost, dbport, dbname, dbusername, dbpassword);
			conn->conn = PQconnectdb(conninfo);
			if((!conn->conn) || (PQstatus(conn->conn) == CONNECTION_BAD))
			{
				ggz_error_msg("Could not connect to database.");
				pthread_mutex_unlock(&mutex);
				return NULL;
			}
			conn->used = 1;
			ggz_list_insert(list, conn);
			pthread_mutex_unlock(&mutex);
			return conn->conn;
		}
		pthread_mutex_unlock(&mutex);

		sleep(1);
	}

	ggz_error_msg("Number of database connections exceeded.");
	return NULL;
}

/* Put a connection back into the pool */
static void releaseconnection(PGconn *conn)
{
	GGZListEntry *entry;
	connection_t *conn2;
	int i = 0;

	if(!conn) return;
	pthread_mutex_lock(&mutex);
	entry = ggz_list_head(list);
	while(entry)
	{
		conn2 = ggz_list_get_data(entry);
		if(conn2->conn == conn)
		{
			conn2->used = 0;
			break;
		}
		entry = ggz_list_next(entry);
		i++;
	}
	pthread_mutex_unlock(&mutex);
}

/* Helper function: replace all patterns in a string */
static char *strreplace(const char *str, const char *pattern, const char *subst)
{
	char *ss, *sstmp;
	char *ptr;

	ss = ggz_strdup(str);

	while((ptr = strstr(ss, pattern)))
	{
		sstmp = ggz_strdup(ss);
		memcpy(sstmp, ss, ptr - ss);
		memcpy(sstmp + (ptr - ss), subst, strlen(subst));
		memcpy(sstmp + (ptr - ss) + strlen(subst), ptr + strlen(pattern), strlen(str) - strlen(pattern) - (ptr - ss) + 1);
		ggz_free(ss);
		ss = sstmp;
	}

	return ss;
}

/* Initialize the database tables from an external SQL schema file */
static int setupschema(PGconn *conn, const char *filename)
{
	char buffer[1024];
	PGresult *res;
	char *completebuffer = NULL;
	char *substbuffer;
	int len;
	int i;
	int rc = 1;

	FILE *f = fopen(filename, "r");
	if(!f)
	{
		ggz_error_msg("Schema read error from %s.", filename);
		return 0;
	}

	while(fgets(buffer, sizeof(buffer), f))
	{
		if(strlen(buffer) == 1)
		{
			substbuffer = strreplace(completebuffer, "%PREFIX%", "");
			res = PQexec(conn, substbuffer);
			if((PQresultStatus(res) != PGRES_EMPTY_QUERY)
			&& (PQresultStatus(res) != PGRES_COMMAND_OK))
			{
				ggz_error_msg("Table creation error %i.",
					PQresultStatus(res));
				rc = 0;
			}
			PQclear(res);

			ggz_free(substbuffer);
			ggz_free(completebuffer);
			completebuffer = NULL;
			continue;
		}

		buffer[strlen(buffer) - 1] = '\0';
		for(i = 0; i < strlen(buffer); i++)
		{
			if(buffer[i] == '\t') buffer[i] = ' ';
		}

		len = (completebuffer ? strlen(completebuffer) : 0);
		completebuffer = (char*)ggz_realloc(completebuffer,
			len + strlen(buffer) + 1);
		if(len)
			strncat(completebuffer, buffer, strlen(buffer) + 1);
		else
			strncpy(completebuffer, buffer, strlen(buffer) + 1);
	}

	if(completebuffer) ggz_free(completebuffer);

	fclose(f);

	return rc;
}

/* Upgrade a database */
static int upgrade(PGconn *conn, const char *oldversion, const char *newversion)
{
	char upgradefile[1024];

	snprintf(upgradefile, sizeof(upgradefile), "%s/upgrade_%s_%s.sql",
		GGZDDATADIR, oldversion, newversion);

	return setupschema(conn, upgradefile);
}

/* String canonicalization for comparison */
static const char *lower(void)
{
	if(pg_canonicalstr)
		return "canonicalstr";
	return "lower";
}

/* Exported functions */

/* Function to initialize the pgsql database system */
GGZReturn _ggzdb_init(ggzdbConnection connection, int set_standalone)
{
	PGconn *conn;
	PGresult *res;
	char query[4096];
	int rc, ret;
	int init;
	char *version;
	char schemafile[1024];

	dbhost = connection.host;
	dbport = connection.port;
	dbname = connection.database;
	dbusername = connection.username;
	dbpassword = connection.password;

	if(dbport <= 0)
		dbport = 5432;

	list = ggz_list_create(NULL, NULL, NULL, GGZ_LIST_ALLOW_DUPS);

	pthread_mutex_init(&mutex, NULL);

	conn = claimconnection();
	if (!conn)
		return GGZDB_ERR_DB;

	rc = GGZDB_NO_ERROR;

	/* Check whether the database is ok */
	init = 1;
	res = PQexec(conn, "SELECT value FROM control WHERE key = 'version'");
	if(PQresultStatus(res) == PGRES_TUPLES_OK)
	{
		if(PQntuples(res) == 1)
		{
			init = 0;
			version = PQgetvalue(res, 0, 0);
			if(strcmp(version, GGZDB_VERSION_ID))
			{
				/* Perform upgrade if possible */
				ggz_error_msg("Wrong database version: %s present, %s needed.\n", version, GGZDB_VERSION_ID);
				if(!upgrade(conn, version, GGZDB_VERSION_ID))
				{
					ggz_error_msg_exit("Database upgrade failed.\n");
					rc = GGZDB_ERR_DB;
				}
			}
		}
		PQclear(res);
	}

	/* Check if we have canonicalstr() available */
	pg_canonicalstr = 0;
	res = PQexec(conn, "SELECT COUNT(*) FROM pg_proc WHERE proname = 'canonicalstr'");
	if(PQresultStatus(res) == PGRES_TUPLES_OK)
	{
		if(PQntuples(res) == 1)
		{
			const char *value = PQgetvalue(res, 0, 0);
			if(strcmp(value, "0"))
			{
				pg_canonicalstr = 1;
			}
		}
		PQclear(res);
	}

	/* Initialize the database if needed */
	if(init)
	{
		snprintf(schemafile, sizeof(schemafile), "%s/ggz.sql", GGZDDATADIR);

		ret = setupschema(conn, schemafile);
		if(!ret) rc = GGZDB_ERR_DB;

		snprintf(query, sizeof(query), "INSERT INTO control "
			"(key, value) VALUES ('version', '%s')", GGZDB_VERSION_ID);

		res = PQexec(conn, query);

		if(PQresultStatus(res) != PGRES_COMMAND_OK) rc = GGZDB_ERR_DB;
		PQclear(res);

		if(rc == GGZDB_ERR_DB)
			ggz_error_msg_exit("Could not initialize the database (with %s).\n", schemafile);
	}

	releaseconnection(conn);

	return rc;
}


/* Function to deinitialize the pgsql database system */
void _ggzdb_close(void)
{
	GGZListEntry *entry;
	connection_t *conn2;

	if(reconfigurationconn)
		releaseconnection(reconfigurationconn);

	entry = ggz_list_head(list);
	while(entry)
	{
		conn2 = ggz_list_get_data(entry);
		PQfinish(conn2->conn);
		ggz_free(conn2);
		entry = ggz_list_next(entry);
	}
	ggz_list_free(list);
}


int _ggzdb_reconfiguration_fd(void)
{
	PGresult *res;
	char query[4096];

	reconfigurationconn = claimconnection();
	if (!reconfigurationconn) {
		ggz_error_msg("_ggzdb_reconfiguration_fd: couldn't claim connection");
		return -1;
	}

	snprintf(query, sizeof(query),
		"LISTEN ggzroomchange");

	res = PQexec(reconfigurationconn, query);
	if(PQresultStatus(res) != PGRES_COMMAND_OK) {
		return -1;
	}
	PQclear(res);

	return PQsocket(reconfigurationconn);
}


void _ggzdb_reconfiguration_load(void)
{
	PGconn *conn;
	PGresult *res;
	char query[4096];

	conn = claimconnection();
	if (!conn) {
		ggz_error_msg("_ggzdb_reconfiguration_load: couldn't claim connection");
		return;
	}

	snprintf(query, sizeof(query),
		"UPDATE rooms SET action = 'add' WHERE filebased = 'f'");

	res = PQexec(conn, query);
	PQclear(res);

	releaseconnection(conn);
}


RoomStruct *_ggzdb_reconfiguration_room(void)
{
	RoomStruct *rooms = NULL;
	PGnotify *notify;
	PGresult *res, *res2;
	char query[4096];
	int i;

	PQconsumeInput(reconfigurationconn);

	notify = PQnotifies(reconfigurationconn);
	if(notify != NULL) {
		if(!ggz_strcmp(notify->relname, "ggzroomchange")) {

			snprintf(query, sizeof(query),
			 "SELECT "
			 "name, description, gametype, maxplayers, maxtables, entryrestriction, action "
			 "FROM rooms WHERE filebased = 'f' AND action IS NOT NULL");

			res = PQexec(reconfigurationconn, query);

			if (PQresultStatus(res) == PGRES_TUPLES_OK) {
				rooms = ggz_malloc((PQntuples(res) + 1) * sizeof(RoomStruct));
				rooms[PQntuples(res)].name = NULL;
				rooms[PQntuples(res)].room = NULL;

				for(i = 0; i < PQntuples(res); i++) {
					const char *name = PQgetvalue(res, i, 0);
					const char *description = PQgetvalue(res, i, 1);
					const char *gametype = PQgetvalue(res, i, 2);
					int maxplayers = atoi(PQgetvalue(res, i, 3));
					int maxtables = atoi(PQgetvalue(res, i, 4));
					const char *entryrestriction = PQgetvalue(res, i, 5);
					const char *action = PQgetvalue(res, i, 6);
					int perms;
					int type;

					rooms[i].name = NULL;
					rooms[i].room = NULL;
					rooms[i].description = NULL;
					rooms[i].game_type = -1;
					rooms[i].max_players = 0;
					rooms[i].max_tables = 0;
					rooms[i].perms = 0;

					if(!ggz_strcmp(action, "add")) {
						snprintf(query, sizeof(query),
							"UPDATE rooms SET action = NULL WHERE name = '%s'",
							name);

						res2 = PQexec(reconfigurationconn, query);
						PQclear(res2);

						if(!ggz_strcmp(entryrestriction, "none"))
							perms = 0;
						else if(!ggz_strcmp(entryrestriction, "registered"))
							perms = 1 << GGZ_PERM_ROOMS_LOGIN;
						else if(!ggz_strcmp(entryrestriction, "admin"))
							perms = 1 << GGZ_PERM_ROOMS_ADMIN;
						else
							perms = 0;

						type = -1;
						if(!ggz_strcmp(gametype, NULL))
							type = -1;
						// FIXME: this is all fake

						/* The room's name equals its internal name */
						rooms[i].room = ggz_strdup(name);
						rooms[i].name = ggz_intlstring_fromstring(name);
						rooms[i].description = ggz_intlstring_fromstring(description);
						rooms[i].game_type = type;
						rooms[i].max_players = maxplayers;
						rooms[i].max_tables = maxtables;
						rooms[i].perms = perms;
					} else if(!ggz_strcmp(action, "delete")) {
						snprintf(query, sizeof(query),
							"DELETE FROM rooms WHERE name = '%s'",
							name);

						res2 = PQexec(reconfigurationconn, query);
						PQclear(res2);

						/* We use the room's internal name as its key for deletions */
						rooms[i].room = ggz_strdup(name);
						rooms[i].name = ggz_intlstring_fromstring(name);
					}
				}

			}
			PQclear(res);
		}
		PQfreemem(notify);
	}

	return rooms;
}


/* Function to enter the database */
void _ggzdb_enter(void)
{
	/* pgsql doesn't need to set locks or anything */
}


/* Function to exit the database */
void _ggzdb_exit(void)
{
	/* pgsql doesn't need to free locks or anything */
}


/* Function to initialize the player table */
GGZDBResult _ggzdb_init_player(void)
{
	return GGZDB_NO_ERROR;
}


/* Function to add a player record */
GGZDBResult _ggzdb_player_add(ggzdbPlayerEntry *pe)
{
	GGZDBResult rc = GGZDB_NO_ERROR;
	PGconn *conn;
	PGresult *res;
	char query[4096];
	char *handle_quoted;
	char *password_quoted;
	char *name_quoted;
	char *email_quoted;

	conn = claimconnection();
	if (!conn) {
		ggz_error_msg("_ggzdb_player_add: couldn't claim connection");
		return GGZDB_ERR_DB;
	}

	handle_quoted = _ggz_sql_escape(pe->handle);
	password_quoted = _ggz_sql_escape(pe->password);
	name_quoted = _ggz_sql_escape(pe->name);
	email_quoted = _ggz_sql_escape(pe->email);

	snprintf(query, sizeof(query), "INSERT INTO users "
		 "(handle, password, name, email, lastlogin, permissions, firstlogin, confirmed) "
		 "VALUES ('%s', '%s', '%s', '%s', %li, %u, %li, '%u')",
		 handle_quoted, password_quoted, name_quoted, email_quoted,
		 pe->last_login, pe->perms, time(NULL), pe->confirmed);

	if (handle_quoted)
		ggz_free(handle_quoted);
	if (password_quoted)
		ggz_free(password_quoted);
	if (name_quoted)
		ggz_free(name_quoted);
	if (email_quoted)
		ggz_free(email_quoted);

	res = PQexec(conn, query);

	handle_quoted = _ggz_sql_escape(pe->handle);

	/* FIXME: provide server-side function for Unicode-safe stringprep */
	/* FIXME: here and elsewhere (e.g. for ggzdb_mysql.c) */
	snprintf(query, sizeof(query), "DELETE FROM stats "
		 "WHERE %s(handle) = %s('%s')",
		 lower(), lower(), handle_quoted);

	if (handle_quoted)
		ggz_free(handle_quoted);

	res = PQexec(conn, query);

	/* FIXME: is this correct?  We won't overwrite, or make a duplicate
	   entry above?  This is *supposed* to fail if we try to.  --JDS */
	if (PQresultStatus(res) != PGRES_COMMAND_OK)
		rc = GGZDB_ERR_DUPKEY;
	PQclear(res);

	releaseconnection(conn);

	return rc;
}


/* Function to retrieve a player record */
GGZDBResult _ggzdb_player_get(ggzdbPlayerEntry *pe)
{
	GGZDBResult rc;
	PGconn *conn;
	PGresult *res;
	char query[4096];
	char *handle_quoted;

	conn = claimconnection();
	if (!conn) {
		ggz_error_msg("_ggzdb_player_add: couldn't claim connection");
		return GGZDB_ERR_DB;
	}

	handle_quoted = _ggz_sql_escape(pe->handle);

	snprintf(query, sizeof(query),
		 "SELECT "
		 "password, name, email, lastlogin, permissions, confirmed "
		 "FROM users WHERE %s(handle) = %s('%s')",
		 lower(), lower(), handle_quoted);

	ggz_free(handle_quoted);

	res = PQexec(conn, query);

	if (PQresultStatus(res) == PGRES_TUPLES_OK) {
		if(PQntuples(res) == 1)	{
			strncpy(pe->password, PQgetvalue(res, 0, 0), sizeof(pe->password));
			strncpy(pe->name, PQgetvalue(res, 0, 1), sizeof(pe->name));
			strncpy(pe->email, PQgetvalue(res, 0, 2), sizeof(pe->email));
			pe->last_login = atol(PQgetvalue(res, 0, 3));
			pe->perms = atol(PQgetvalue(res, 0, 4));
			pe->confirmed = (!ggz_strcmp(PQgetvalue(res, 0, 5), "t") ? 1 : 0);
			rc = GGZDB_NO_ERROR;
		} else	{
			/* This is supposed to happen when we look up
			   nonexistent players. */
			rc = GGZDB_ERR_NOTFOUND;
		}
	} else {
		ggz_error_msg("Couldn't lookup player.");
		rc = GGZDB_ERR_DB;
	}
	PQclear(res);

	releaseconnection(conn);

	return rc;
}


/* Function to update a player record */
GGZDBResult _ggzdb_player_update(ggzdbPlayerEntry *pe)
{
	GGZDBResult rc = GGZDB_NO_ERROR;
	PGconn *conn;
	PGresult *res;
	char query[4096];
	char *handle_quoted;
	char *password_quoted;
	char *name_quoted;
	char *email_quoted;

	conn = claimconnection();
	if (!conn) {
		ggz_error_msg("_ggzdb_player_update: couldn't claim connection");
		return GGZDB_ERR_DB;
	}

	handle_quoted = _ggz_sql_escape(pe->handle);
	password_quoted = _ggz_sql_escape(pe->password);
	name_quoted = _ggz_sql_escape(pe->name);
	email_quoted = _ggz_sql_escape(pe->email);

	snprintf(query, sizeof(query),
		 "UPDATE users SET "
		 "password = '%s', name = '%s', email = '%s', "
		 "lastlogin = %li, permissions = %u, confirmed = '%u' WHERE "
		 "%s(handle) = %s('%s')",
		 password_quoted, name_quoted, email_quoted,
		 pe->last_login, pe->perms, pe->confirmed,
		 lower(), lower(), handle_quoted);

	if (handle_quoted)
		ggz_free(handle_quoted);
	if (password_quoted)
		ggz_free(password_quoted);
	if (name_quoted)
		ggz_free(name_quoted);
	if (email_quoted)
		ggz_free(email_quoted);

	res = PQexec(conn, query);
	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		ggz_error_msg("Couldn't update player.");
		rc = GGZDB_ERR_DB;
	}
	PQclear(res);

	releaseconnection(conn);

	return rc;
}


/* Function to retrieve an extended player record */
GGZDBResult _ggzdb_player_get_extended(ggzdbPlayerExtendedEntry *pe)
{
	GGZDBResult rc;
	PGconn *conn;
	PGresult *res;
	char query[4096];
	char *handle_quoted;

	conn = claimconnection();
	if (!conn) {
		ggz_error_msg("_ggzdb_player_add: couldn't claim connection");
		return GGZDB_ERR_DB;
	}

	handle_quoted = _ggz_sql_escape(pe->handle);

	snprintf(query, sizeof(query),
		 "SELECT "
		 "id, photo "
		 "FROM userinfo WHERE %s(handle) = %s('%s')",
		 lower(), lower(), handle_quoted);

	ggz_free(handle_quoted);

	res = PQexec(conn, query);

	if (PQresultStatus(res) == PGRES_TUPLES_OK) {
		if(PQntuples(res) == 1)	{
			pe->user_id = atol(PQgetvalue(res, 0, 0));
			strncpy(pe->photo, PQgetvalue(res, 0, 1), sizeof(pe->photo));
			rc = GGZDB_NO_ERROR;
		} else	{
			rc = GGZDB_ERR_NOTFOUND;
		}
	} else {
		ggz_error_msg("Couldn't lookup player.");
		rc = GGZDB_ERR_DB;
	}
	PQclear(res);

	releaseconnection(conn);

	return rc;
}


/* All functions below here are NOT THREADSAFE (at least yet) */
/* All functions below here are NOT THREADSAFE (at least yet) */
/* All functions below here are NOT THREADSAFE (at least yet) */
/* All functions below here are NOT THREADSAFE (at least yet) */

GGZDBResult _ggzdb_player_get_first(ggzdbPlayerEntry *pe)
{
	GGZDBResult rc;
	char query[4096];
	PGconn *conn;

	if(iterres)
	{
		PQclear(iterres);
	}

	conn = claimconnection();
	if (!conn) {
		ggz_error_msg("_ggzdb_player_get_first: couldn't claim connection");
		return GGZDB_ERR_DB;
	}

	snprintf(query, sizeof(query),
		 "SELECT "
		 "id, handle, password, name, email, lastlogin, permissions, confirmed "
		 "FROM users");
	iterres = PQexec(conn, query);

	if (PQresultStatus(iterres) == PGRES_TUPLES_OK) {
		if(PQntuples(iterres) > 0) {
			pe->user_id = atoi(PQgetvalue(iterres, 0, 0));
			strncpy(pe->handle, PQgetvalue(iterres, 0, 1), sizeof(pe->handle));
			strncpy(pe->password, PQgetvalue(iterres, 0, 2), sizeof(pe->password));
			strncpy(pe->name, PQgetvalue(iterres, 0, 3), sizeof(pe->name));
			strncpy(pe->email, PQgetvalue(iterres, 0, 4), sizeof(pe->email));
			pe->last_login = atol(PQgetvalue(iterres, 0, 5));
			pe->perms = atol(PQgetvalue(iterres, 0, 6));
			pe->confirmed = (!ggz_strcmp(PQgetvalue(iterres, 0, 7), "t") ? 1 : 0);
			rc = GGZDB_NO_ERROR;
		} else {
			PQclear(iterres);
			iterres = NULL;
			rc = GGZDB_ERR_NOTFOUND;
		}
	} else {
		ggz_error_msg("Couldn't lookup player.");
		PQclear(iterres);
		iterres = NULL;
		rc = GGZDB_ERR_DB;
	}

	itercount = 0;

	releaseconnection(conn);

	return rc;
}


GGZDBResult _ggzdb_player_get_next(ggzdbPlayerEntry *pe)
{
	if (!iterres) {
		ggz_error_msg_exit("get_first should be called before get_next");
	}

	if (itercount < PQntuples(iterres) - 1) {
		itercount++;
		pe->user_id = atoi(PQgetvalue(iterres, itercount, 0));
		strncpy(pe->handle, PQgetvalue(iterres, itercount, 1), sizeof(pe->handle));
		strncpy(pe->password, PQgetvalue(iterres, itercount, 2), sizeof(pe->password));
		strncpy(pe->name, PQgetvalue(iterres, itercount, 3), sizeof(pe->name));
		strncpy(pe->email, PQgetvalue(iterres, itercount, 4), sizeof(pe->email));
		pe->last_login = atol(PQgetvalue(iterres, itercount, 5));
		pe->perms = atol(PQgetvalue(iterres, itercount, 6));
		pe->confirmed = (!ggz_strcmp(PQgetvalue(iterres, 0, 7), "t") ? 1 : 0);

		return GGZDB_NO_ERROR;
	} else {
		PQclear(iterres);
		iterres = NULL;

		return GGZDB_ERR_NOTFOUND;
	}
}


void _ggzdb_player_drop_cursor(void)
{
	if (!iterres) {
		/* This isn't an error; since we clear the cursor at the end
		   of _ggzdb_player_get_next we should expect to end up
		   here.  --JDS */
		/*ggz_error_msg_exit("get_first should be called before drop_cursor");*/
		return;
	}

	PQclear(iterres);
	iterres = NULL;
}

unsigned int _ggzdb_player_next_uid(void)
{
	/* SQL handles id's automatically */
	/* But does that mean 0 should be returned?  GGZ might need to
	   know more?  --JDS */
	return 0;
}

GGZDBResult _ggzdb_init_stats(void)
{
	return GGZDB_NO_ERROR;
}

GGZDBResult _ggzdb_stats_lookup(ggzdbPlayerGameStats *stats)
{
	PGconn *conn;
	PGresult *res;
	char query[4096];
	GGZDBResult rc = GGZDB_ERR_DB;
	char *player_quoted;

	conn = claimconnection();
	if(!conn) {
		ggz_error_msg("_ggzdb_stats_lookup: couldn't claim connection");
		return rc;
	}

	player_quoted = _ggz_sql_escape(stats->player);

	snprintf(query, sizeof(query),
		"SELECT "
		"wins, losses, ties, forfeits, rating, ranking, highscore "
		"FROM stats WHERE %s(handle) = %s('%s') AND game = '%s'",
		lower(), lower(), player_quoted, stats->game);

	ggz_free(player_quoted);

	res = PQexec(conn, query);
	if(PQresultStatus(res) == PGRES_TUPLES_OK) {
		if(PQntuples(res) == 1) {
			rc = GGZDB_NO_ERROR;
			stats->wins = atoi(PQgetvalue(res, 0, 0));
			stats->losses = atoi(PQgetvalue(res, 0, 1));
			stats->ties = atoi(PQgetvalue(res, 0, 2));
			stats->forfeits = atoi(PQgetvalue(res, 0, 3));
			stats->rating = atof(PQgetvalue(res, 0, 4));
			stats->ranking = atol(PQgetvalue(res, 0, 5));
			stats->highest_score = atol(PQgetvalue(res, 0, 6));
		} else {
			ggz_error_msg("couldn't lookup player stats");
			rc = GGZDB_ERR_NOTFOUND;
		}
	}
	PQclear(res);

	releaseconnection(conn);

	return rc;
}

GGZDBResult _ggzdb_stats_update(ggzdbPlayerGameStats *stats)
{
	PGconn *conn;
	PGresult *res, *res2;
	char query[4096];
	int rc = GGZDB_ERR_DB;
	char *player_quoted;

	conn = claimconnection();
	if (!conn) {
		ggz_error_msg("_ggzdb_stats_update: couldn't claim connection");
		return rc;
	}

	player_quoted = _ggz_sql_escape(stats->player);

	snprintf(query, sizeof(query),
		"UPDATE stats "
		"SET wins = %i, losses = %i, ties = %i, forfeits = %i, "
		"rating = %f, ranking = %u, highscore = %li "
		"WHERE %s(handle) = %s('%s') AND game = '%s'",
		stats->wins, stats->losses, stats->ties, stats->forfeits,
		stats->rating, stats->ranking, stats->highest_score,
		lower(), lower(), player_quoted, stats->game);

	ggz_free(player_quoted);

	res = PQexec(conn, query);

	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		ggz_error_msg("couldn't update stats");
	} else {
		if (!strcmp(PQcmdTuples(res), "0")) {
			player_quoted = _ggz_sql_escape(stats->player);

			snprintf(query, sizeof(query),
				"INSERT INTO stats "
				"(handle, game, wins, losses, ties, forfeits, rating, ranking, highscore) VALUES "
				"('%s', '%s', %i, %i, %i, %i, %f, %u, %li)",
				player_quoted, stats->game,
				stats->wins, stats->losses, stats->ties, stats->forfeits,
				stats->rating, stats->ranking, stats->highest_score);

			ggz_free(player_quoted);

			res2 = PQexec(conn, query);

			if (PQresultStatus(res) != PGRES_COMMAND_OK) {
				ggz_error_msg("couldn't insert stats");
			}
			else rc = GGZDB_NO_ERROR;
			PQclear(res2);
		}
		else rc = GGZDB_NO_ERROR;
	}
	PQclear(res);

	releaseconnection(conn);

	_ggzdb_stats_match(stats);

	return rc;
}

GGZDBResult _ggzdb_stats_match(ggzdbPlayerGameStats *stats)
{
	PGconn *conn;
	PGresult *res;
	char query[4096];
	int rc = GGZDB_ERR_DB;
	char *number, *playertype;
	char *player_quoted;

	conn = claimconnection();
	if (!conn) {
		ggz_error_msg("_ggzdb_stats_match: couldn't claim connection");
		return rc;
	}

	snprintf(query, sizeof(query),
		"SELECT MAX(id) FROM matches");

	res = PQexec(conn, query);

	if (PQresultStatus(res) != PGRES_TUPLES_OK) {
		ggz_error_msg("couldn't read match");
		number = NULL;
	}
	else {
		number = PQgetvalue(res, 0, 0);
	}

	playertype = "";
	if(stats->player_type == GGZ_PLAYER_GUEST) playertype = "guest";
	else if(stats->player_type == GGZ_PLAYER_NORMAL) playertype = "registered";
	else if(stats->player_type == GGZ_PLAYER_BOT) playertype = "bot";

	player_quoted = _ggz_sql_escape(stats->player);

	snprintf(query, sizeof(query),
		"INSERT INTO matchplayers "
		"(match, handle, playertype) VALUES "
		"(%s, '%s', '%s')",
		number, player_quoted, playertype);

	ggz_free(player_quoted);

	PQclear(res);
	res = PQexec(conn, query);

	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		ggz_error_msg("couldn't insert matchplayer");
	}
	else rc = GGZDB_NO_ERROR;
	PQclear(res);

	releaseconnection(conn);

	return rc;
}

GGZDBResult _ggzdb_stats_newmatch(const char *game, const char *winner, const char *savegame)
{
	PGconn *conn;
	PGresult *res;
	char query[4096];
	int rc = GGZDB_ERR_DB;
	char *winner_quoted;

	conn = claimconnection();
	if (!conn) {
		ggz_error_msg("_ggzdb_stats_newmatch: couldn't claim connection");
		return rc;
	}

	snprintf(query, sizeof(query),
		"DELETE FROM savegames "
		"WHERE game = '%s' AND savegame = '%s'",
		game, savegame);

	res = PQexec(conn, query);
	PQclear(res);

	winner_quoted = _ggz_sql_escape(winner);

	snprintf(query, sizeof(query),
		"INSERT INTO matches "
		"(date, game, winner, savegame) VALUES "
		"(%li, '%s', '%s', '%s')",
		time(NULL), game, winner_quoted, savegame);

	ggz_free(winner_quoted);

	res = PQexec(conn, query);

	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		ggz_error_msg("couldn't insert match");
	}
	else rc = GGZDB_NO_ERROR;
	PQclear(res);

	releaseconnection(conn);

	return rc;
}

GGZDBResult _ggzdb_stats_savegame(const char *game, const char *owner, const char *savegame, ggzdbStamp tableid)
{
	PGconn *conn;
	PGresult *res;
	char query[4096];
	int rc = GGZDB_ERR_DB;
	char *owner_quoted;

	conn = claimconnection();
	if (!conn) {
		ggz_error_msg("_ggzdb_stats_savegame: couldn't claim connection");
		return rc;
	}

	snprintf(query, sizeof(query),
		"DELETE FROM savegames "
		"WHERE game = '%s' AND owner = '%s'",
		game, owner);

	res = PQexec(conn, query);
	PQclear(res);

	owner_quoted = _ggz_sql_escape(owner);

	snprintf(query, sizeof(query),
		"INSERT INTO savegames "
		"(date, game, owner, savegame, tableid, stamp) VALUES "
		"(%li, '%s', '%s', '%s', %li, %li)",
		time(NULL), game, owner, savegame, tableid.thread, tableid.starttime);

	ggz_free(owner_quoted);

	res = PQexec(conn, query);

	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		ggz_error_msg("couldn't insert savegame");
	}
	else rc = GGZDB_NO_ERROR;
	PQclear(res);

	releaseconnection(conn);

	return rc;
}

GGZDBResult _ggzdb_stats_toprankings(const char *game, int number, ggzdbPlayerGameStats **rankings)
{
	PGconn *conn;
	PGresult *res;
	char query[4096];
	int rc = GGZDB_ERR_DB;
	ggzdbPlayerGameStats *stats;
	int i;

	conn = claimconnection();
	if (!conn) {
		ggz_error_msg("_ggzdb_stats_toplist: couldn't claim connection");
		return rc;
	}

	snprintf(query, sizeof(query),
		"SELECT wins, losses, ties, forfeits, rating, ranking, highscore, handle FROM stats "
		"WHERE game = '%s' AND ranking <> 0 ORDER BY ranking ASC LIMIT %i",
		game, number);

	res = PQexec(conn, query);

	if (PQresultStatus(res) != PGRES_TUPLES_OK) {
		ggz_error_msg("couldn't read rankings");
	} else {
		rc = GGZDB_NO_ERROR;
		for(i = 0; i < PQntuples(res); i++) {
			stats = rankings[i];
			stats->wins = atoi(PQgetvalue(res, i, 0));
			stats->losses = atoi(PQgetvalue(res, i, 1));
			stats->ties = atoi(PQgetvalue(res, i, 2));
			stats->forfeits = atoi(PQgetvalue(res, i, 3));
			stats->rating = atof(PQgetvalue(res, i, 4));
			stats->ranking = atol(PQgetvalue(res, i, 5));
			stats->highest_score = atol(PQgetvalue(res, i, 6));
			snprintf(stats->player, MAX_USER_NAME_LEN, PQgetvalue(res, i, 7));
		}
	}
	PQclear(res);

	releaseconnection(conn);

	return rc;
}

GGZDBResult _ggzdb_stats_calcrankings(const char *game)
{
	PGconn *conn;
	char dscpath[1024];

	snprintf(dscpath, sizeof(dscpath), "%s/games", GGZDCONFDIR);

	conn = claimconnection();
	if (!conn) {
		ggz_error_msg("_ggzdb_stats_calcrankings: couldn't claim connection");
		return GGZDB_ERR_DB;
	}

	GGZRankings *rankings = rankings_init();
	rankings_loadmodels(rankings, dscpath);
	rankings_setconnection(rankings, conn);
	rankings_recalculate_game(rankings, game);
	rankings_destroy(rankings);

	releaseconnection(conn);

	return GGZDB_NO_ERROR;
}

static void strfree(void *str)
{
	ggzdbSavegamePlayers *sp = str;
	int i;

	ggz_free(sp->owner);
	ggz_free(sp->savegame);
	for(i = 0; i < sp->count; i++) {
		ggz_free(sp->names[i]);
	}
	if(sp->names)
		ggz_free(sp->names);
	if(sp->types)
		ggz_free(sp->types);
	ggz_free(sp);
}

GGZList *_ggzdb_savegame_owners(const char *game)
{
	PGconn *conn;
	PGresult *res, *res2;
	GGZList *owners = NULL;

	char query[4096];
	char *game_quoted;
	char *owner;
	char *savegame;
	ggzdbStamp tableid;
	int i, j;
	ggzdbSavegamePlayers *sp;

	conn = claimconnection();
	if (!conn) {
		ggz_error_msg("ggzdb_savegame_owners: couldn't claim connection");
		return NULL;
	}

	game_quoted = _ggz_sql_escape(game);
	snprintf(query, sizeof(query),
		"SELECT owner, tableid, stamp, savegame FROM savegames WHERE game = '%s'",
		game_quoted);
	ggz_free(game_quoted);

	res = PQexec(conn, query);
	if (PQresultStatus(res) != PGRES_TUPLES_OK) {
		ggz_error_msg("couldn't read savegame owners");
	} else {
		owners = ggz_list_create(NULL, NULL, (ggzEntryDestroy)strfree, GGZ_LIST_ALLOW_DUPS);
		for(i = 0; i < PQntuples(res); i++) {
			owner = ggz_strdup(PQgetvalue(res, i, 0));
			tableid.thread = atol(ggz_strdup(PQgetvalue(res, i, 1)));
			tableid.starttime = atol(ggz_strdup(PQgetvalue(res, i, 2)));
			savegame = ggz_strdup(PQgetvalue(res, i, 3));

			sp = ggz_malloc(sizeof(ggzdbSavegamePlayers));
			sp->owner = owner;
			sp->count = 0;
			sp->names = NULL;
			sp->types = NULL;
			sp->savegame = savegame;

			snprintf(query, sizeof(query),
				"SELECT seat, seattype, handle FROM savegameplayers "
				"WHERE tableid = %li AND stamp = %li ORDER BY seat ASC",
				tableid.thread, tableid.starttime);

			res2 = PQexec(conn, query);
			if (PQresultStatus(res2) != PGRES_TUPLES_OK) {
				ggz_error_msg("couldn't read savegame players");
			} else {
				sp->count = PQntuples(res2);
				sp->types = ggz_malloc(sp->count * sizeof(int));
				sp->names = ggz_malloc(sp->count * sizeof(char*));
				for(j = 0; j < sp->count; j++) {
					sp->types[j] = ggz_string_to_seattype(PQgetvalue(res2, j, 1));
					sp->names[j] = ggz_strdup(PQgetvalue(res2, j, 2));
				}
			}
			PQclear(res2);

			ggz_list_insert(owners, sp);

			snprintf(query, sizeof(query),
				"DELETE FROM savegameplayers WHERE tableid = %li AND stamp = %li",
				tableid.thread, tableid.starttime);

			res2 = PQexec(conn, query);
			PQclear(res2);

			snprintf(query, sizeof(query),
				"DELETE FROM savegames WHERE tableid = %li AND stamp = %li",
				tableid.thread, tableid.starttime);

			res2 = PQexec(conn, query);
			PQclear(res2);
		}
	}
	PQclear(res);

	releaseconnection(conn);

	return owners;
}

GGZList *_ggzdb_savegames(const char *game, const char *owner)
{
	PGconn *conn;
	PGresult *res;
	GGZList *savegames = NULL;
	
	char query[4096];
	char *game_quoted;
	char *owner_quoted;
	char *savegame;
	int i;

	conn = claimconnection();
	if (!conn) {
		ggz_error_msg("ggzdb_savegames: couldn't claim connection");
		return NULL;
	}

	game_quoted = _ggz_sql_escape(game);
	owner_quoted = _ggz_sql_escape(owner);
	snprintf(query, sizeof(query),
		"SELECT savegame FROM savegames "
		"WHERE game = '%s' AND owner = '%s'",
		game_quoted, owner_quoted);
	ggz_free(game_quoted);
	ggz_free(owner_quoted);

	res = PQexec(conn, query);
	if (PQresultStatus(res) != PGRES_TUPLES_OK) {
		ggz_error_msg("couldn't read savegames");
	} else {
		savegames = ggz_list_create(NULL, NULL, (ggzEntryDestroy)strfree, GGZ_LIST_ALLOW_DUPS);
		for(i = 0; i < PQntuples(res); i++) {
			savegame = ggz_strdup(PQgetvalue(res, i, 0));
			ggz_list_insert(savegames, savegame);
		}
	}
	PQclear(res);

	releaseconnection(conn);

	return savegames;
}

GGZDBResult _ggzdb_savegame_player(ggzdbStamp tableid, int seat, const char *name, int type)
{
	PGconn *conn;
	PGresult *res;
	char query[4096];
	int rc = GGZDB_ERR_DB;
	char *name_quoted;

	conn = claimconnection();
	if (!conn) {
		ggz_error_msg("_ggzdb_savegame_player: couldn't claim connection");
		return rc;
	}

	snprintf(query, sizeof(query),
		"DELETE FROM savegameplayers "
		"WHERE tableid = %li AND stamp = %li AND seat = %i",
		tableid.thread, tableid.starttime, seat);

	res = PQexec(conn, query);
	PQclear(res);

	name_quoted = _ggz_sql_escape(name);

	snprintf(query, sizeof(query),
		"INSERT INTO savegameplayers "
		"(tableid, stamp, seat, handle, seattype) VALUES "
		"(%li, %li, %i, '%s', '%s')",
		tableid.thread, tableid.starttime, seat, name_quoted, ggz_seattype_to_string(type));

	if(name_quoted)
		ggz_free(name_quoted);

	res = PQexec(conn, query);

	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		ggz_error_msg("couldn't insert savegame player");
	}
	else rc = GGZDB_NO_ERROR;
	PQclear(res);

	releaseconnection(conn);

	return rc;
}

GGZDBResult _ggzdb_rooms(RoomStruct *rooms, int num)
{
	PGconn *conn;
	PGresult *res;
	char query[8192];
	int rc = GGZDB_ERR_DB;
	char *name_quoted, *description_quoted;
	int i;

	conn = claimconnection();
	if (!conn) {
		ggz_error_msg("_ggzdb_rooms: couldn't claim connection");
		return rc;
	}

	/* FIXME: what do we do with invalidated derived rooms? */
	snprintf(query, sizeof(query),
		"DELETE FROM rooms WHERE filebased = 't'");

	res = PQexec(conn, query);
	PQclear(res);

	for(i = 0; i < num; i++) {
		RoomStruct room = rooms[i];

		name_quoted = _ggz_sql_escape(ggz_intlstring_translated(room.name, NULL));
		description_quoted = _ggz_sql_escape(ggz_intlstring_translated(room.description, NULL));

		/* FIXME: evaluate room->perms and somehow also room->game_type */
		snprintf(query, sizeof(query),
			"INSERT INTO rooms "
			"(filebased, name, description, gametype, maxplayers, maxtables, entryrestriction) VALUES "
			"('t', '%s', '%s', '%s', %i, %i, '%s')",
			name_quoted, description_quoted, "???",
			room.max_players, room.max_tables, "none");

		if(name_quoted)
			ggz_free(name_quoted);
		if(description_quoted)
			ggz_free(description_quoted);

		res = PQexec(conn, query);

		if (PQresultStatus(res) != PGRES_COMMAND_OK) {
			ggz_error_msg("couldn't insert room %s",
				ggz_intlstring_translated(room.name, NULL));
			PQclear(res);
			break;
		}
		else rc = GGZDB_NO_ERROR;
		PQclear(res);
	}

	releaseconnection(conn);

	return rc;
}

