/*
 * File: ggzdb_pgsql.c
 * Author: Josef Spillner
 * Project: GGZ Server
 * Date: 02.05.2002
 * Desc: Back-end functions for handling the postgresql style database
 * $Id: ggzdb_pgsql.c 4480 2002-09-09 03:24:42Z jdorje $
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

#include <config.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <limits.h>
#include <pthread.h>
#include <ggz.h>
#include <unistd.h>

#include <postgresql/libpq-fe.h>

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

static char *dbhost;
static char *dbname;
static char *dbusername;
static char *dbpassword;

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
				return conn->conn;
			}
			entry = ggz_list_next(entry);
			i++;
		}
		pthread_mutex_unlock(&mutex);

		pthread_mutex_lock(&mutex);
		if(ggz_list_count(list) < SQL_MAXCONNECTIONS)
		{
			conn = (connection_t*)malloc(sizeof(connection_t));
			snprintf(conninfo, sizeof(conninfo), "host=%s dbname=%s user=%s password=%s",
				dbhost, dbname, dbusername, dbpassword);
			conn->conn = PQconnectdb(conninfo);
			if((!conn->conn) || (PQstatus(conn->conn) == CONNECTION_BAD))
			{
				err_sys("Could not connect to database.\n");
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

	err_sys("Number of database connections exceeded.\n");
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
int _ggzdb_init(ggzdbConnection connection, int set_standalone)
{
	PGconn *conn;
	PGresult *res;
	char query[4096];
	int rc;

	dbhost = connection.host;
	dbname = connection.database;
	dbusername = connection.username;
	dbpassword = connection.password;

	list = ggz_list_create(NULL, NULL, NULL, GGZ_LIST_ALLOW_DUPS);

	pthread_mutex_init(&mutex, NULL);

	conn = claimconnection();
	if(!conn) return 1;

	/* Hack: Fire-and-forget table initialization. It might already be
	 * present, or the database user doesn't have the privileges. */
	snprintf(query, sizeof(query), "CREATE TABLE users "
		"(id serial, handle varchar(256), password varchar(256), name varchar(256), email varchar(256), "
		"lastlogin int8, permissions int8)");

	res = PQexec(conn, query);

	rc = !(PQresultStatus(res) == PGRES_COMMAND_OK);
	PQclear(res);

	releaseconnection(conn);

	/* Hack is here ;) */
	rc = 0;
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
		free(conn2);
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
int _ggzdb_init_player(char *datadir)
{
	return 0;
}


/* Function to add a player record */
int _ggzdb_player_add(ggzdbPlayerEntry *pe)
{
	int rc;
	PGconn *conn;
	PGresult *res;
	char query[4096];

	conn = claimconnection();

	if(!conn) rc = 1;
	else
	{
		snprintf(query, sizeof(query), "INSERT INTO users "
			"(handle, password, name, email, lastlogin, permissions) VALUES "
			"('%s', '%s', '%s', '%s', %li, %u)",
			pe->handle, pe->password, pe->name, pe->email, pe->last_login, pe->perms);

		res = PQexec(conn, query);

		rc = !(PQresultStatus(res) == PGRES_COMMAND_OK);
		PQclear(res);
	}

	if(rc) err_sys("Couldn't add player.");

	releaseconnection(conn);

	return rc;
}


/* Function to retrieve a player record */
int _ggzdb_player_get(ggzdbPlayerEntry *pe)
{
	int rc;
	PGconn *conn;
	PGresult *res;
	char query[4096];

	res = NULL;
	conn = claimconnection();
	if(conn)
	{

		snprintf(query, sizeof(query), "SELECT "
			"password, name, email, lastlogin, permissions FROM users WHERE "
			"handle = '%s'",
			pe->handle);

		res = PQexec(conn, query);

		rc = !(PQresultStatus(res) == PGRES_TUPLES_OK);
	}
	else rc = 1;

	if(!rc)
	{
		if(PQntuples(res) == 1)
		{
			strncpy(pe->password, PQgetvalue(res, 0, 0), sizeof(pe->password));
			strncpy(pe->name, PQgetvalue(res, 0, 1), sizeof(pe->name));
			strncpy(pe->email, PQgetvalue(res, 0, 2), sizeof(pe->email));
			pe->last_login = atol(PQgetvalue(res, 0, 3));
			pe->perms = atol(PQgetvalue(res, 0, 4));
		}
		else
		{
			err_sys("Not exactly one entry found.");
			rc = GGZDB_ERR_NOTFOUND;
		}
	}
	else err_sys("Couldn't lookup player.");
	if(res) PQclear(res);

	releaseconnection(conn);

	return rc;
}


/* Function to update a player record */
int _ggzdb_player_update(ggzdbPlayerEntry *pe)
{
	int rc;
	PGconn *conn;
	PGresult *res;
	char query[4096];

	res = NULL;
	conn = claimconnection();
	if(conn)
	{
		snprintf(query, sizeof(query), "UPDATE users SET "
			"password = '%s', name = '%s', email = '%s', lastlogin = %li, permissions = %u WHERE "
			"handle = '%s'",
			pe->password, pe->name, pe->email, pe->last_login, pe->perms, pe->handle);

		res = PQexec(conn, query);
		rc = !(PQresultStatus(res) == PGRES_COMMAND_OK);
		PQclear(res);
	}
	else rc = 1;

	if(rc) err_sys("Couldn't update player.");

	releaseconnection(conn);

	return rc;
}


/* All functions below here are NOT THREADSAFE (at least yet) */
/* All functions below here are NOT THREADSAFE (at least yet) */
/* All functions below here are NOT THREADSAFE (at least yet) */
/* All functions below here are NOT THREADSAFE (at least yet) */

int _ggzdb_player_get_first(ggzdbPlayerEntry *pe)
{
	int rc;
	char query[4096];
	PGconn *conn;

	if(iterres)
	{
		PQclear(iterres);
	}

	conn = claimconnection();

	if(conn)
	{
		snprintf(query, sizeof(query), "SELECT "
			"id, handle, password, name, email, lastlogin, permissions FROM users");
		iterres = PQexec(conn, query);

		rc = !(PQresultStatus(iterres) == PGRES_TUPLES_OK);
		if(!rc)
		{
			if(PQntuples(iterres) > 0)
			{
				pe->user_id = atoi(PQgetvalue(iterres, 0, 0));
				strncpy(pe->handle, PQgetvalue(iterres, 0, 1), sizeof(pe->handle));
				strncpy(pe->password, PQgetvalue(iterres, 0, 2), sizeof(pe->password));
				strncpy(pe->name, PQgetvalue(iterres, 0, 3), sizeof(pe->name));
				strncpy(pe->email, PQgetvalue(iterres, 0, 4), sizeof(pe->email));
				pe->last_login = atol(PQgetvalue(iterres, 0, 5));
				pe->perms = atol(PQgetvalue(iterres, 0, 6));
			}
			else
			{
				err_sys("No entries found.");
				rc = GGZDB_ERR_NOTFOUND;
				PQclear(iterres);
				iterres = NULL;
			}
		}
		else
		{
			err_sys("Couldn't lookup player.");
			PQclear(iterres);
			iterres = NULL;
		}
	}
	else rc = 1;

	itercount = 0;

	releaseconnection(conn);

	return rc;
}


int _ggzdb_player_get_next(ggzdbPlayerEntry *pe)
{
	int rc;

	if(!iterres)
	{
		err_sys_exit("get_next called before get_first, dummy");
	}

	rc = 0;
	if(itercount < PQntuples(iterres) - 1)
	{
		itercount++;
		pe->user_id = atoi(PQgetvalue(iterres, itercount, 0));
		strncpy(pe->handle, PQgetvalue(iterres, itercount, 1), sizeof(pe->handle));
		strncpy(pe->password, PQgetvalue(iterres, itercount, 2), sizeof(pe->password));
		strncpy(pe->name, PQgetvalue(iterres, itercount, 3), sizeof(pe->name));
		strncpy(pe->email, PQgetvalue(iterres, itercount, 4), sizeof(pe->email));
		pe->last_login = atol(PQgetvalue(iterres, itercount, 5));
		pe->perms = atol(PQgetvalue(iterres, itercount, 6));
	}
	else
	{
		rc = GGZDB_ERR_NOTFOUND;
		PQclear(iterres);
		iterres = NULL;
	}

	return rc;
}


void _ggzdb_player_drop_cursor(void)
{
	if(!iterres)
	{
		/*err_sys_exit("drop_cursor called before get_first, dummy");*/
		return;
	}

	PQclear(iterres);
	iterres = 0;
}

unsigned int _ggzdb_player_next_uid(void)
{
	/* SQL handles id's automatically */
	return 0;
}


