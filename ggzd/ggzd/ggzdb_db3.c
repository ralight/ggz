/*
 * File: ggzdb_db3.c
 * Author: Rich Gade
 * Project: GGZ Server
 * Date: 11/10/2000
 * Desc: Back-end functions for handling the db3 sytle database
 * $Id: ggzdb_db3.c 4501 2002-09-10 06:42:12Z jdorje $
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

#ifdef DB3_IN_DIR
#include <db3/db.h>
#else
#include <db.h>
#endif

#include "err_func.h"
#include "ggzd.h"
#include "ggzdb.h"
#include "ggzdb_proto.h"

/* Internal variables */
static DB *db_p = NULL;
static DB_ENV *db_e;
static int standalone = 0;


int _ggzdb_player_add(ggzdbPlayerEntry *);


/* Function to initialize the db3 database system */
int _ggzdb_init(ggzdbConnection connection, int set_standalone)
{
	int rc;
	u_int32_t flags;

	if(set_standalone) {
		flags = DB_INIT_MPOOL | DB_INIT_LOCK;
		standalone = 1;
	} else
		flags = DB_CREATE | DB_INIT_MPOOL | DB_INIT_LOCK | DB_THREAD;
	if((rc = db_env_create(&db_e, 0)) != 0)
		err_sys("db_env_create() failed in _ggzdb_init()");
	else if((rc = db_e->open(db_e, connection.datadir, flags , 0600)) != 0)
		err_sys("db_e->open() failed in _ggzdb_init()");

	return rc;
}


/* Function to deinitialize the db3 database system */
void _ggzdb_close(void)
{
	/* FIXME: Check the return codes */
	if(db_p) {
		db_p->close(db_p, 0);
		db_p = NULL;
	}

	db_e->close(db_e, 0);
	db_e = NULL;
}


/* Function to enter the database */
void _ggzdb_enter(void)
{
	/* db3 doesn't need to set locks or anything */
}


/* Function to exit the database */
void _ggzdb_exit(void)
{
	/* db3 doesn't need to free locks or anything */
}


/* Function to initialize the player table */
int _ggzdb_init_player(char *datadir)
{
	int rc;
	u_int32_t flags;
	ggzdbPlayerEntry marker;

	if(standalone)
		flags = 0;
	else
		flags = DB_CREATE | DB_THREAD;

	/* Open the database file */
	if(db_create(&db_p, db_e, 0) != 0)
		err_sys_exit("db_create() failed in _ggzdb_init_player()");
	rc = db_p->open(db_p, "player.db", NULL, DB_BTREE, flags, 0600);

	/* Check for errors */
	if(rc != 0)
		err_sys_exit("db_p->open() failed in _ggzdb_init_player()");

	/* Add a marker entry for our next UID */
	marker.user_id = 1;
#if MAX_USER_NAME_LEN < 8
#error MAX_USER_NAME_LEN (see ggzd.h) must be at least 8 characters
#endif
	strcpy(marker.handle, "&nxtuid&");

	/* FIXME: We really should check for "impossible" failure below */
	_ggzdb_player_add(&marker);

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
		free(data.data); /* Allocated by db3? */
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


/* Function to get and update the next uid */
unsigned int _ggzdb_player_next_uid(void)
{
	ggzdbPlayerEntry marker;
	unsigned int nxt;

	/* Get the marker entry for our next UID */
	strcpy(marker.handle, "&nxtuid&");

	/* FIXME: We really should check for "impossible" failures below */
	_ggzdb_player_get(&marker);
	nxt = marker.user_id;
	marker.user_id++;
	_ggzdb_player_update(&marker);

	return nxt;
}


/* All functions below here are NOT THREADSAFE (at least yet) */
/* All functions below here are NOT THREADSAFE (at least yet) */
/* All functions below here are NOT THREADSAFE (at least yet) */
/* All functions below here are NOT THREADSAFE (at least yet) */

static DBC *db_c = NULL;
int _ggzdb_player_get_first(ggzdbPlayerEntry *pe)
{
	int rc=0;
	DBT key, data;

	if(db_c == NULL)
		if((rc = db_p->cursor(db_p, NULL, &db_c, 0)) != 0) {
			err_sys("Failed to create db2 cursor");
			return rc;
		}

	/* Build the two DBT structures */
	memset(&key, 0, sizeof(DBT));
	memset(&data, 0, sizeof(DBT));
	key.data = pe->handle;
	key.size = sizeof(pe->handle);
	data.size = sizeof(ggzdbPlayerEntry);
	data.flags = DB_DBT_MALLOC;
	if((rc = db_c->c_get(db_c, &key, &data, DB_FIRST)) != 0)
		err_sys("Failed to get DB_FIRST record");
	else {
		memcpy(pe, data.data, sizeof(ggzdbPlayerEntry));
		free(data.data); /* Allocated by db3? */
	}

	return rc;
}


int _ggzdb_player_get_next(ggzdbPlayerEntry *pe)
{
	int rc=0;
	DBT key, data;

	if(db_c == NULL)
		err_sys_exit("get_next called before get_first, dummy");

	/* Build the two DBT structures */
	memset(&key, 0, sizeof(DBT));
	memset(&data, 0, sizeof(DBT));
	key.data = pe->handle;
	key.size = sizeof(pe->handle);
	data.size = sizeof(ggzdbPlayerEntry);
	data.flags = DB_DBT_MALLOC;
	if((rc = db_c->c_get(db_c, &key, &data, DB_NEXT)) != 0) {
		if(rc != DB_NOTFOUND)
			err_sys("Failed to get DB_NEXT record");
		else
			rc = GGZDB_ERR_NOTFOUND;
	} else {
		memcpy(pe, data.data, sizeof(ggzdbPlayerEntry));
		free(data.data); /* Allocated by db3? */
	}

	return rc;
}


void _ggzdb_player_drop_cursor(void)
{
	if(db_c == NULL)
		err_sys_exit("drop_cursor called before get_first, dummy");
	db_c->c_close(db_c);
	db_c = NULL;
}
