/*
 * File: ggzdb_pgsql.c
 * Author: Josef Spillner
 * Project: GGZ Server
 * Date: 02.05.2002
 * Desc: Back-end functions for handling the postgresql style database
 * $Id: ggzdb_pgsql.c 7091 2005-04-08 14:10:51Z josef $
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


#define SQL_MAXCONNECTIONS 10
#define SQL_TIMEOUT 3

/* Internal variables */
static PGresult *iterres = NULL;
static int itercount;

static pthread_mutex_t mutex;
static GGZList *list;

static const char *dbhost;
static const char *dbname;
static const char *dbusername;
static const char *dbpassword;

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
					err_msg("Database connection lost, reconnecting.");
					PQreset(conn->conn);
					if((!conn->conn) || (PQstatus(conn->conn) == CONNECTION_BAD))
					{
						err_msg("Could not connect to database.");
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
				 "host=%s dbname=%s user=%s password=%s",
				 dbhost, dbname, dbusername, dbpassword);
			conn->conn = PQconnectdb(conninfo);
			if((!conn->conn) || (PQstatus(conn->conn) == CONNECTION_BAD))
			{
				err_msg("Could not connect to database.");
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

	err_msg("Number of database connections exceeded.");
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

/* Exported functions */

/* Function to initialize the pgsql database system */
GGZReturn _ggzdb_init(ggzdbConnection connection, int set_standalone)
{
	PGconn *conn;
	PGresult *res;
	char query[4096];
	int rc;
	int init;
	char *version;

	dbhost = connection.host;
	dbname = connection.database;
	dbusername = connection.username;
	dbpassword = connection.password;

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
				err_msg_exit("Wrong database version: %s present, %s needed.\n", version, GGZDB_VERSION_ID);
				rc = GGZDB_ERR_DB;
			}
		}
		PQclear(res);
	}

	/* Initialize the database if needed */
	if(init)
	{
		snprintf(query, sizeof(query), "CREATE TABLE users "
			"(id serial, handle varchar(256), password varchar(256), name varchar(256), email varchar(256), "
			"lastlogin int8, permissions int8)");

		res = PQexec(conn, query);

		if(PQresultStatus(res) != PGRES_COMMAND_OK) rc = GGZDB_ERR_DB;
		PQclear(res);

		snprintf(query, sizeof(query), "CREATE TABLE stats "
			"(id serial, handle varchar(256), game varchar(256), wins int8, losses int8, ties int8, forfeits int8, "
			"rating float, ranking int8, highscore int8)");

		res = PQexec(conn, query);

		if(PQresultStatus(res) != PGRES_COMMAND_OK) rc = GGZDB_ERR_DB;
		PQclear(res);

		snprintf(query, sizeof(query), "CREATE TABLE matches "
			"(id serial, date int8, game varchar(256), winner varchar(256))");

		res = PQexec(conn, query);

		if(PQresultStatus(res) != PGRES_COMMAND_OK) rc = GGZDB_ERR_DB;
		PQclear(res);

		snprintf(query, sizeof(query), "CREATE TABLE matchplayers "
			"(id serial, match int8, handle varchar(256), playertype varchar(256))");

		res = PQexec(conn, query);

		if(PQresultStatus(res) != PGRES_COMMAND_OK) rc = GGZDB_ERR_DB;
		PQclear(res);

		snprintf(query, sizeof(query), "CREATE TABLE control "
			"(key varchar(256), value varchar(256))");

		res = PQexec(conn, query);

		if(PQresultStatus(res) != PGRES_COMMAND_OK) rc = GGZDB_ERR_DB;
		PQclear(res);

		snprintf(query, sizeof(query), "INSERT INTO control "
			"(key, value) VALUES ('version', '%s')", GGZDB_VERSION_ID);

		res = PQexec(conn, query);

		if(PQresultStatus(res) != PGRES_COMMAND_OK) rc = GGZDB_ERR_DB;
		PQclear(res);

		if(rc == GGZDB_ERR_DB)
			err_msg_exit("Could not initialize the database.\n");
	}

	releaseconnection(conn);

	return rc;
}


/* Function to deinitialize the pgsql database system */
void _ggzdb_close(void)
{
	GGZListEntry *entry;
	connection_t *conn2;

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
GGZDBResult _ggzdb_init_player(const char *datadir)
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

	conn = claimconnection();
	if (!conn) {
		err_msg("_ggzdb_player_add: couldn't claim connection");
		return GGZDB_ERR_DB;
	}

	snprintf(query, sizeof(query), "INSERT INTO users "
		 "(handle, password, name, email, lastlogin, permissions) "
		 "VALUES ('%s', '%s', '%s', '%s', %li, %u)",
		 pe->handle, pe->password, pe->name, pe->email,
		 pe->last_login, pe->perms);

	res = PQexec(conn, query);

	snprintf(query, sizeof(query), "DELETE FROM stats "
		 "WHERE lower(handle) = lower('%s')",
		 pe->handle);

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

	conn = claimconnection();
	if (!conn) {
		err_msg("_ggzdb_player_add: couldn't claim connection");
		return GGZDB_ERR_DB;
	}

	snprintf(query, sizeof(query),
		 "SELECT "
		 "password, name, email, lastlogin, permissions "
		 "FROM users WHERE lower(handle) = lower('%s')",
		 pe->handle);

	res = PQexec(conn, query);

	if (PQresultStatus(res) == PGRES_TUPLES_OK) {
		if(PQntuples(res) == 1)	{
			strncpy(pe->password, PQgetvalue(res, 0, 0), sizeof(pe->password));
			strncpy(pe->name, PQgetvalue(res, 0, 1), sizeof(pe->name));
			strncpy(pe->email, PQgetvalue(res, 0, 2), sizeof(pe->email));
			pe->last_login = atol(PQgetvalue(res, 0, 3));
			pe->perms = atol(PQgetvalue(res, 0, 4));
			rc = GGZDB_NO_ERROR;
		} else	{
			/* This is supposed to happen when we look up
			   nonexistent players. */
			rc = GGZDB_ERR_NOTFOUND;
		}
	} else {
		err_msg("Couldn't lookup player.");
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

	conn = claimconnection();
	if (!conn) {
		err_msg("_ggzdb_player_update: couldn't claim connection");
		return GGZDB_ERR_DB;
	}

	snprintf(query, sizeof(query),
		 "UPDATE users SET "
		 "password = '%s', name = '%s', email = '%s', "
		 "lastlogin = %li, permissions = %u WHERE "
		 "lower(handle) = lower('%s')",
		 pe->password, pe->name, pe->email, pe->last_login,
		 pe->perms, pe->handle);

	res = PQexec(conn, query);
	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		err_msg("Couldn't update player.");
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
		err_msg("_ggzdb_player_get_first: couldn't claim connection");
		return GGZDB_ERR_DB;
	}

	snprintf(query, sizeof(query),
		 "SELECT "
		 "id, handle, password, name, email, lastlogin, permissions "
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
			rc = GGZDB_NO_ERROR;
		} else {
			PQclear(iterres);
			iterres = NULL;
			rc = GGZDB_ERR_NOTFOUND;
		}
	} else {
		err_msg("Couldn't lookup player.");
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
		err_msg_exit("get_next called before get_first, dummy");
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
		/*err_msg_exit("drop_cursor called before get_first, dummy");*/
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

GGZDBResult _ggzdb_init_stats(ggzdbConnection connection)
{
	/* call ggzdb_init here, we use the same connection for playerdb and
	   statsdb, or assume it's already initialized */

	return GGZDB_NO_ERROR;
}

GGZDBResult _ggzdb_stats_lookup(ggzdbPlayerGameStats *stats)
{
	PGconn *conn;
	PGresult *res;
	char query[4096];
	GGZDBResult rc = GGZDB_ERR_DB;

	conn = claimconnection();
	if(!conn) {
		err_msg("_ggzdb_stats_lookup: couldn't claim connection");
		return rc;
	}

	snprintf(query, sizeof(query),
		"SELECT "
		"wins, losses, ties, forfeits, rating, ranking, highscore "
		"FROM stats WHERE lower(handle) = lower('%s') AND game = '%s'",
		stats->player, stats->game);

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
			err_msg("couldn't lookup player stats");
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

	conn = claimconnection();
	if (!conn) {
		err_msg("_ggzdb_stats_update: couldn't claim connection");
		return rc;
	}

	snprintf(query, sizeof(query),
		"UPDATE stats "
		"SET wins = %i, losses = %i, ties = %i, forfeits = %i, rating = %f, ranking = %u, highscore = %li "
		"WHERE lower(handle) = lower('%s') AND game = '%s'",
		stats->wins, stats->losses, stats->ties, stats->forfeits, stats->rating, stats->ranking, stats->highest_score,
		stats->player, stats->game);

	res = PQexec(conn, query);

	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		err_msg("couldn't update stats");
	} else {
		if (!strcmp(PQcmdTuples(res), "0")) {
			snprintf(query, sizeof(query),
				"INSERT INTO stats "
				"(handle, game, wins, losses, ties, forfeits, rating, ranking, highscore) VALUES "
				"('%s', '%s', %i, %i, %i, %i, %f, %u, %li)",
				stats->player, stats->game, stats->wins, stats->losses, stats->ties, stats->forfeits,
				stats->rating, stats->ranking, stats->highest_score);

			res2 = PQexec(conn, query);

			if (PQresultStatus(res) != PGRES_COMMAND_OK) {
				err_msg("couldn't insert stats");
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

	conn = claimconnection();
	if (!conn) {
		err_msg("_ggzdb_stats_match: couldn't claim connection");
		return rc;
	}

	snprintf(query, sizeof(query),
		"SELECT MAX(id) FROM matches");

	res = PQexec(conn, query);

	if (PQresultStatus(res) != PGRES_TUPLES_OK) {
		err_msg("couldn't read match");
		number = NULL;
	}
	else {
		number = PQgetvalue(res, 0, 0);
	}

	playertype = "";
	if(stats->player_type == GGZ_PLAYER_GUEST) playertype = "guest";
	else if(stats->player_type == GGZ_PLAYER_NORMAL) playertype = "registered";
	else if(stats->player_type == GGZ_PLAYER_BOT) playertype = "bot";

	snprintf(query, sizeof(query),
		"INSERT INTO matchplayers "
		"(match, handle, playertype) VALUES "
		"(%s, '%s', '%s')",
		number, stats->player, playertype);
	PQclear(res);

	res = PQexec(conn, query);

	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		err_msg("couldn't insert matchplayer");
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

	conn = claimconnection();
	if (!conn) {
		err_msg("_ggzdb_stats_newmatch: couldn't claim connection");
		return rc;
	}

	snprintf(query, sizeof(query),
		"DELETE FROM savegames "
		"WHERE game = '%s' AND savegame = '%s'",
		game, savegame);

	res = PQexec(conn, query);

	snprintf(query, sizeof(query),
		"INSERT INTO matches "
		"(date, game, winner, savegame) VALUES "
		"(%li, '%s', '%s', '%s')",
		time(NULL), game, winner, savegame);

	res = PQexec(conn, query);

	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		err_msg("couldn't insert match");
	}
	else rc = GGZDB_NO_ERROR;
	PQclear(res);

	releaseconnection(conn);

	return rc;
}

GGZDBResult _ggzdb_stats_savegame(const char *game, const char *owner, const char *savegame)
{
	PGconn *conn;
	PGresult *res;
	char query[4096];
	int rc = GGZDB_ERR_DB;

	conn = claimconnection();
	if (!conn) {
		err_msg("_ggzdb_stats_savegame: couldn't claim connection");
		return rc;
	}

	snprintf(query, sizeof(query),
		"INSERT INTO savegames"
		"(date, game, owner, savegame) VALUES "
		"(%li, '%s', '%s', '%s')",
		time(NULL), game, owner, savegame);

	res = PQexec(conn, query);

	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		err_msg("couldn't insert savegame");
	}
	else rc = GGZDB_NO_ERROR;
	PQclear(res);

	releaseconnection(conn);

	return rc;
}

