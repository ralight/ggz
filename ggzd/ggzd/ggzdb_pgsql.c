/*
 * File: ggzdb_pgsql.c
 * Author: Josef Spillner
 * Project: GGZ Server
 * Date: 02.05.2002
 * Desc: Back-end functions for handling the postgresql style database
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

#include <postgresql/libpq-fe.h>

#include "ggzd.h"
#include "ggzdb.h"
#include "err_func.h"

/* Internal variables */
static PGconn *conn = NULL;
static PGresult *res = NULL;
static PGresult *iterres = NULL;
static int itercount;
static char query[4096];

#define PGSQLCONNECTION "user=ggzd password=ggzd dbname=ggz"

/* Internal functions */


/* Function to initialize the pgsql database system */
int _ggzdb_init(char *datadir, int set_standalone)
{
	conn = PQconnectdb(PGSQLCONNECTION);
	if((!conn) || (PQstatus(conn) == CONNECTION_BAD))
	{
		err_sys("Couldn't initialize database.");
		return 1;
	}
	return 0;
}


/* Function to deinitialize the pgsql database system */
void _ggzdb_close(void)
{
	PQfinish(conn);
	conn = NULL;
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
	int rc;

	/* Hack: Fire-and-forget table initialization. It might already be
	 * present, or the database user doesn't have the privileges. */
	snprintf(query, sizeof(query), "CREATE TABLE users "
		"(id int4, handle varchar(256), password varchar(256), name varchar(256), email varchar(256), "
		"lastlogin int8, permissions int8)");
	res = PQexec(conn, query);

	rc = !(PQresultStatus(res) == PGRES_COMMAND_OK);
	PQclear(res);

	/* Hack is here ;) */
	rc = 0;
	return rc;
}


/* Function to add a player record */
int _ggzdb_player_add(ggzdbPlayerEntry *pe)
{
	int rc;

	/* FIXME: use auto_increment SEQUENCE here */
	snprintf(query, sizeof(query), "INSERT INTO users "
		"(handle, password, name, email, lastlogin, permissions) VALUES "
		"('%s', '%s', '%s', '%s', %li, %u)",
		pe->handle, pe->password, pe->name, pe->email, pe->last_login, pe->perms);
	res = PQexec(conn, query);

	rc = !(PQresultStatus(res) == PGRES_COMMAND_OK);
	if(rc)
	{
		err_sys("Couldn't add player.");
	}
	PQclear(res);

	return rc;
}


/* Function to retrieve a player record */
int _ggzdb_player_get(ggzdbPlayerEntry *pe)
{
	int rc;

	snprintf(query, sizeof(query), "SELECT "
		"handle, password, name, email, lastlogin, permissions FROM users WHERE "
		"id = %i",
		pe->user_id);
	res = PQexec(conn, query);

	rc = !(PQresultStatus(res) == PGRES_TUPLES_OK);
	if(!rc)
	{
		if(PQntuples(res) == 1)
		{
			strncpy(pe->handle, PQgetvalue(res, 0, 0), sizeof(pe->handle));
			strncpy(pe->password, PQgetvalue(res, 0, 1), sizeof(pe->password));
			strncpy(pe->name, PQgetvalue(res, 0, 2), sizeof(pe->name));
			strncpy(pe->email, PQgetvalue(res, 0, 3), sizeof(pe->email));
			pe->last_login = atol(PQgetvalue(res, 0, 4));
			pe->perms = atol(PQgetvalue(res, 0, 5));
		}
		else
		{
			err_sys("Not exactly one entry found.");
			rc = GGZDB_ERR_NOTFOUND;
		}
	}
	else
	{
		err_sys("Couldn't lookup player.");
	}
	PQclear(res);

	return rc;
}


/* Function to update a player record */
int _ggzdb_player_update(ggzdbPlayerEntry *pe)
{
	int rc;

	snprintf(query, sizeof(query), "UPDATE users SET "
		"password = '%s', name = '%s', email = '%s', lastlogin = %li, permissions = %u, handle = '%s' WHERE "
		"id = %i",
		pe->password, pe->name, pe->email, pe->last_login, pe->perms, pe->handle, pe->user_id);
	res = PQexec(conn, query);

	rc = !(PQresultStatus(res) == PGRES_COMMAND_OK);
	if(rc)
	{
		err_sys("Couldn't update player.");
	}
	PQclear(res);

	return rc;
}


/* All functions below here are NOT THREADSAFE (at least yet) */
/* All functions below here are NOT THREADSAFE (at least yet) */
/* All functions below here are NOT THREADSAFE (at least yet) */
/* All functions below here are NOT THREADSAFE (at least yet) */

int _ggzdb_player_get_first(ggzdbPlayerEntry *pe)
{
	int rc;

	if(iterres)
	{
		PQclear(iterres);
	}

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

	itercount = 0;

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


