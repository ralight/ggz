/*
 * File: ggzdb_db2.c
 * Author: Rich Gade
 * Project: GGZ Server
 * Date: 06/11/2000
 * Desc: Back-end functions for handling the db2 sytle database
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

#ifdef DB2_IN_DIR
#include <db2/db.h>
#else
#include <db.h>
#endif

#include "ggzd.h"
#include "ggzdb.h"
#include "err_func.h"

/* Internal variables */
static DB *db_p = NULL;
static DB_ENV db_e;
static DB_INFO db_i;

/* Internal functions */


/* Function to initialize the db2 database system */
int _ggzdb_init(char *datadir)
{
	int rc;

	memset(&db_i, 0, sizeof(db_i));
	memset(&db_e, 0, sizeof(db_e));

	rc = db_appinit(datadir, NULL, &db_e,
			DB_CREATE | DB_INIT_LOCK | DB_THREAD);
	if(rc != 0)
		err_sys("db_appinit() failed in _ggzdb_init()");

	return rc;
}


/* Function to deinitialize the db2 database system */
void _ggzdb_close(void)
{
	if(db_p) {
		db_p->close(db_p, 0);
		db_p = NULL;
	}

	db_appexit(&db_e);
}


/* Function to enter the database */
void _ggzdb_enter(void)
{
	/* db2 doesn't need to set locks or anything */
}


/* Function to exit the database */
void _ggzdb_exit(void)
{
	/* db2 doesn't need to free locks or anything */
}


/* Function to initialize the player table */
int _ggzdb_init_player(char *datadir)
{
	int rc;

	/* Open the database file */
	rc = db_open("player.db", DB_BTREE, DB_CREATE | DB_THREAD, 0600,
		     &db_e, &db_i, &db_p);

	/* Check for errors */
	if(rc != 0)
		err_sys_exit("dbopen() failed in _ggzdb_init_player()");

	return 0;
}


/* Function to add a player record */
int _ggzdb_player_add(ggzdbPlayerEntry *pe)
{
	int rc;
	DBT key, data;

	/* Build the two DBT structures */
	memset(&key, 0, sizeof(DBT));
	memset(&data, 0, sizeof(DBT));
	key.data = pe->handle;
	key.size = strlen(pe->handle);
	data.data = pe;
	data.size = sizeof(ggzdbPlayerEntry);
	data.flags = DB_DBT_USERMEM;

	rc = db_p->put(db_p, NULL, &key, &data, DB_NOOVERWRITE);
	if(rc == DB_KEYEXIST)
		rc = GGZDB_ERR_DUPKEY;
	else if(rc != 0)
		err_sys("put failed in _ggzdb_player_add()");

	/* FIXME: We won't have to do this once ggzd can exit */
	db_p->sync(db_p, 0);

	return rc;
}


/* Function to retrieve a player record */
int _ggzdb_player_get(ggzdbPlayerEntry *pe)
{
	int rc;
	DBT key, data;

	/* Build the two DBT structures */
	memset(&key, 0, sizeof(DBT));
	memset(&data, 0, sizeof(DBT));
	key.data = pe->handle;
	key.size = strlen(pe->handle);
	data.size = sizeof(ggzdbPlayerEntry);
	data.flags = DB_DBT_MALLOC;

	/* Perform the get */
	rc = db_p->get(db_p, NULL, &key, &data, 0);
	if(rc == DB_NOTFOUND)
		rc = GGZDB_ERR_NOTFOUND;
	else if(rc != 0)
		err_sys("get failed in _ggzdb_player_get()");

	/* Copy it to the user data buffer */
	if(rc == 0) {
		memcpy(pe, data.data, sizeof(ggzdbPlayerEntry));
		free(data.data);
	}

	return rc;
}


/* Function to update a player record */
int _ggzdb_player_update(ggzdbPlayerEntry *pe)
{
	int rc;
	DBT key, data;

	/* Build the two DBT structures */
	memset(&key, 0, sizeof(DBT));
	memset(&data, 0, sizeof(DBT));
	key.data = pe->handle;
	key.size = strlen(pe->handle);
	data.data = pe;
	data.size = sizeof(ggzdbPlayerEntry);
	data.flags = DB_DBT_USERMEM;

	rc = db_p->put(db_p, NULL, &key, &data, 0);
	if(rc != 0)
		err_sys("put failed in _ggzdb_player_add()");

	/* FIXME: We won't have to do this once ggzd can exit */
	db_p->sync(db_p, 0);

	return rc;
}
