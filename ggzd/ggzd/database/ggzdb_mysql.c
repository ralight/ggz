/*
 * File: ggzdb_mysql.c
 * Author: Josef Spillner
 * Project: GGZ Server
 * Date: 03.05.2002
 * Desc: Back-end functions for handling the postgresql style database
 * $Id: ggzdb_mysql.c 7067 2005-03-28 19:30:35Z josef $
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

#include <mysql/mysql.h>

#include "err_func.h"
#include "ggzd.h"
#include "ggzdb.h"
#include "ggzdb_proto.h"


/* Internal variables */
static MYSQL *conn = NULL;
static MYSQL_RES *iterres = NULL;
static int itercount;
static pthread_mutex_t mutex;

/* Internal functions */


/* Function to initialize the mysql database system */
GGZReturn _ggzdb_init(ggzdbConnection connection, int set_standalone)
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
	return GGZ_OK;
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
GGZDBResult _ggzdb_init_player(const char *datadir)
{
	return GGZDB_NO_ERROR;
}


/* Function to add a player record */
GGZDBResult _ggzdb_player_add(ggzdbPlayerEntry *pe)
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

	if (rc != 0) {
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
	MYSQL_RES *res;
	MYSQL_ROW row;
	char query[4096];

	snprintf(query, sizeof(query), "SELECT "
		"password, name, email, lastlogin, permissions FROM users WHERE "
		"handle = '%s'",
		pe->handle);

	pthread_mutex_lock(&mutex);
	rc = mysql_query(conn, query);

	if (rc != 0) {
		res = mysql_store_result(conn);
		pthread_mutex_unlock(&mutex);

		if (mysql_num_rows(res) == 1) {
			row = mysql_fetch_row(res);
			strncpy(pe->password, row[0], sizeof(pe->password));
			strncpy(pe->name, row[1], sizeof(pe->name));
			strncpy(pe->email, row[2], sizeof(pe->email));
			pe->last_login = atol(row[3]);
			pe->perms = atol(row[4]);
			mysql_free_result(res);
			return GGZDB_NO_ERROR;
		} else {
			/* This is supposed to happen if we look up a
			   nonexistent player. */
			mysql_free_result(res);
			return GGZDB_ERR_NOTFOUND;
		}
	} else {
		pthread_mutex_unlock(&mutex);
		err_sys("Couldn't lookup player.");
		return GGZDB_ERR_DB;
	}
}


/* Function to update a player record */
GGZDBResult _ggzdb_player_update(ggzdbPlayerEntry *pe)
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

	if(rc) {
		err_sys("Couldn't update player.");
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
	MYSQL_ROW row;
	char query[4096];

	if (iterres) {
		mysql_free_result(iterres);
	}

	snprintf(query, sizeof(query), "SELECT "
		"id, handle, password, name, email, lastlogin, permissions FROM users");
	result = mysql_query(conn, query);

	if (result == 0) {
		iterres = mysql_store_result(conn);
		if (mysql_num_rows(iterres) > 0) {
			row = mysql_fetch_row(iterres);
			pe->user_id = atoi(row[0]);
			strncpy(pe->handle, row[1], sizeof(pe->handle));
			strncpy(pe->password, row[2], sizeof(pe->password));
			strncpy(pe->name, row[3], sizeof(pe->name));
			strncpy(pe->email, row[4], sizeof(pe->email));
			pe->last_login = atol(row[5]);
			pe->perms = atol(row[6]);
			itercount = 0;
			return GGZDB_NO_ERROR;
		} else {
			err_sys("No entries found.");
			mysql_free_result(iterres);
			iterres = NULL;
			return GGZDB_NO_ERROR;
		}
	} else {
		err_sys("Couldn't lookup player.");
		iterres = NULL;
		return GGZDB_ERR_DB;
	}
}


GGZDBResult _ggzdb_player_get_next(ggzdbPlayerEntry *pe)
{
	MYSQL_ROW row;

	if (!iterres) {
		err_sys_exit("get_next called before get_first, dummy");
	}

	if(itercount < mysql_num_rows(iterres) - 1) {
		itercount++;
		row = mysql_fetch_row(iterres);
		pe->user_id = atoi(row[0]);
		strncpy(pe->handle, row[1], sizeof(pe->handle));
		strncpy(pe->password, row[2], sizeof(pe->password));
		strncpy(pe->name, row[3], sizeof(pe->name));
		strncpy(pe->email, row[4], sizeof(pe->email));
		pe->last_login = atol(row[5]);
		pe->perms = atol(row[6]);
		return GGZDB_NO_ERROR;
	} else {
		mysql_free_result(iterres);
		iterres = NULL;
		return GGZDB_ERR_NOTFOUND;
	}
}


void _ggzdb_player_drop_cursor(void)
{
	if(!iterres) {
		/* This isn't an error; since we clear the cursor at the end
		   of _ggzdb_player_get_next we should expect to end up
		   here.  --JDS */
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

GGZDBResult _ggzdb_init_stats(ggzdbConnection connection)
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

GGZDBResult _ggzdb_stats_savegame(const char *game, const char *owner, const char *savegame)
{
	/* Not implemented, but do not return error */
	return GGZDB_NO_ERROR;
}
