/*
 * File: ggzdb_mysql.c
 * Author: Josef Spillner
 * Project: GGZ Server
 * Date: 03.05.2002
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

#include <mysql/mysql.h>
#include <pthread.h>

#include "ggzd.h"
#include "ggzdb.h"
#include "err_func.h"

/* Internal variables */
static MYSQL *conn = NULL;
static MYSQL_RES *iterres = NULL;
static int itercount;
static pthread_mutex_t mutex;

/* Internal functions */


/* Function to initialize the mysql database system */
int _ggzdb_init(ggzdbConnection connection, int set_standalone)
{
	int rc;
	char query[4096];

	if(conn) return 0;

	conn = mysql_init(conn);
	conn = mysql_real_connect(conn, connection.host, connection.username,
		connection.password, connection.database, 0, NULL, 0);

	if(!conn)
	{
		err_sys("Couldn't initialize database.");
		return 1;
	}

	snprintf(query, sizeof(query), "CREATE TABLE users "
		"(id int4 AUTO_INCREMENT PRIMARY KEY, handle varchar(255), password varchar(255), "
		"name varchar(255), email varchar(255), lastlogin int8, permissions int8)");

	rc = mysql_query(conn, query);
	/* Hack. */
	rc = 0;

	return rc;
}


/* Function to deinitialize the mysql database system */
void _ggzdb_close(void)
{
	/*mysql_close(conn);
	conn = NULL;*/
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
int _ggzdb_init_player(char *datadir)
{
	return 0;
}


/* Function to add a player record */
int _ggzdb_player_add(ggzdbPlayerEntry *pe)
{
	int rc;
	char query[4096];

	snprintf(query, sizeof(query), "INSERT INTO users "
		"(handle, password, name, email, lastlogin, permissions) VALUES "
		"('%s', '%s', '%s', '%s', %li, %u)",
		pe->handle, pe->password, pe->name, pe->email, pe->last_login, pe->perms);

	pthread_mutex_lock(&mutex);
	rc = mysql_query(conn, query);
	pthread_mutex_unlock(&mutex);

	if(rc)
	{
		err_sys("Couldn't add player.");
	}

	return rc;
}


/* Function to retrieve a player record */
int _ggzdb_player_get(ggzdbPlayerEntry *pe)
{
	int rc;
	MYSQL_RES *res;
	MYSQL_ROW row;
	char query[4096];

	snprintf(query, sizeof(query), "SELECT "
		"password, name, email, lastlogin, permissions FROM users WHERE "
		"handle = '%s'",
		pe->handle);

	pthread_mutex_lock(&mutex);
	rc = mysql_query(conn, query);

	if(!rc)
	{
		res = mysql_store_result(conn);
		pthread_mutex_unlock(&mutex);
		if(mysql_num_rows(res) == 1)
		{
			row = mysql_fetch_row(res);
			strncpy(pe->password, row[0], sizeof(pe->password));
			strncpy(pe->name, row[1], sizeof(pe->name));
			strncpy(pe->email, row[2], sizeof(pe->email));
			pe->last_login = atol(row[3]);
			pe->perms = atol(row[4]);
		}
		else
		{
			err_sys("Not exactly one entry found.");
			rc = GGZDB_ERR_NOTFOUND;
		}
		mysql_free_result(res);
	}
	else
	{
		pthread_mutex_unlock(&mutex);
		err_sys("Couldn't lookup player.");
	}

	return rc;
}


/* Function to update a player record */
int _ggzdb_player_update(ggzdbPlayerEntry *pe)
{
	int rc;
	char query[4096];

	snprintf(query, sizeof(query), "UPDATE users SET "
		"password = '%s', name = '%s', email = '%s', lastlogin = %li, permissions = %u WHERE "
		"handle = '%s'",
		pe->password, pe->name, pe->email, pe->last_login, pe->perms, pe->handle);

	pthread_mutex_lock(&mutex);
	rc = mysql_query(conn, query);
	pthread_mutex_unlock(&mutex);

	if(rc)
	{
		err_sys("Couldn't update player.");
	}

	return rc;
}


/* All functions below here are NOT THREADSAFE (at least yet) */
/* All functions below here are NOT THREADSAFE (at least yet) */
/* All functions below here are NOT THREADSAFE (at least yet) */
/* All functions below here are NOT THREADSAFE (at least yet) */

int _ggzdb_player_get_first(ggzdbPlayerEntry *pe)
{
	int rc;
	MYSQL_ROW row;
	char query[4096];

	if(iterres)
	{
		mysql_free_result(iterres);
	}

	snprintf(query, sizeof(query), "SELECT "
		"id, handle, password, name, email, lastlogin, permissions FROM users");
	rc = mysql_query(conn, query);

	if(!rc)
	{
		iterres = mysql_store_result(conn);
		if(mysql_num_rows(iterres) > 0)
		{
			row = mysql_fetch_row(iterres);
			pe->user_id = atoi(row[0]);
			strncpy(pe->handle, row[1], sizeof(pe->handle));
			strncpy(pe->password, row[2], sizeof(pe->password));
			strncpy(pe->name, row[3], sizeof(pe->name));
			strncpy(pe->email, row[4], sizeof(pe->email));
			pe->last_login = atol(row[5]);
			pe->perms = atol(row[6]);
		}
		else
		{
			err_sys("No entries found.");
			rc = GGZDB_ERR_NOTFOUND;
			mysql_free_result(iterres);
			iterres = NULL;
		}
	}
	else
	{
		err_sys("Couldn't lookup player.");
		iterres = NULL;
	}

	itercount = 0;

	return rc;
}


int _ggzdb_player_get_next(ggzdbPlayerEntry *pe)
{
	int rc;
	MYSQL_ROW row;

	if(!iterres)
	{
		err_sys_exit("get_next called before get_first, dummy");
	}

	rc = 0;
	if(itercount < mysql_num_rows(iterres) - 1)
	{
		itercount++;
		row = mysql_fetch_row(iterres);
		pe->user_id = atoi(row[0]);
		strncpy(pe->handle, row[1], sizeof(pe->handle));
		strncpy(pe->password, row[2], sizeof(pe->password));
		strncpy(pe->name, row[3], sizeof(pe->name));
		strncpy(pe->email, row[4], sizeof(pe->email));
		pe->last_login = atol(row[5]);
		pe->perms = atol(row[6]);
	}
	else
	{
		rc = GGZDB_ERR_NOTFOUND;
		mysql_free_result(iterres);
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

	mysql_free_result(iterres);
	iterres = 0;
}

unsigned int _ggzdb_player_next_uid(void)
{
	/* SQL handles id's automatically */
	return 0;
}

