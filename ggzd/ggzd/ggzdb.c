/*
 * File: ggzdb.c
 * Author: Rich Gade
 * Project: GGZ Server
 * Date: 06/11/2000
 * Desc: Front-end functions to handle database manipulation
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

#include <pthread.h>

#include "ggzd.h"
#include "datatypes.h"
#include "ggzdb.h"
#include "ggzdb_db2.h"
#include "err_func.h"


/* Server-wide variables */
extern Options opt;

/* Internal variables */
static pthread_rwlock_t db_thread_lock;
static char db_needs_init = 1;
static char player_needs_init = 1;

/* Internal functions */
static int ggzdb_player_init(void);


/* Function to initialize the database system */
int ggzdb_init(void)
{
	int rc;

	/* Initialize the thread lock */
	pthread_rwlock_init(&db_thread_lock, NULL);

	/* Call backend's initialization */
	rc = _ggzdb_init(opt.data_dir);
	if(rc == 0)
		db_needs_init = 0;

	return rc;
}


/* Function to add a player to the database */
int ggzdb_player_add(ggzdbPlayerEntry *pe)
{
	int rc=0;

	DB_ENTER;
	if(player_needs_init)
		rc = ggzdb_player_init();

	if(rc == 0)
		rc = _ggzdb_player_add(pe);

	DB_EXIT;
	return rc;
}


/* Function to retrieve a player from the database */
int ggzdb_player_get(ggzdbPlayerEntry *pe)
{
	int rc=0;

	DB_ENTER;
	if(player_needs_init)
		rc = ggzdb_player_init();

	if(rc == 0)
		rc = _ggzdb_player_get(pe);

	DB_EXIT;
	return rc;
}


/*** INTERNAL FUNCTIONS ***/

/* Function to initialize player tables if necessary */
static int ggzdb_player_init(void)
{
	int rc=0;

	if(db_needs_init)
		rc = GGZDB_ERR_INIT;

	if(rc == 0)
		rc = _ggzdb_init_player(opt.data_dir);

	if(rc == 0)
		player_needs_init = 0;

	return rc;
}
