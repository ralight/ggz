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
#include "err_func.h"


/* Server-wide variables */
extern Options opt;

/* Internal variables */
static char db_needs_init = 1;
static char player_needs_init = 1;

/* Internal functions */
static int ggzdb_player_init(void);

/* Back-end functions */
extern int _ggzdb_init(char *datadir);
extern void _ggzdb_enter(void);
extern void _ggzdb_exit(void);
extern int _ggzdb_init_player(char *datadir);
extern int _ggzdb_player_add(ggzdbPlayerEntry *);
extern int _ggzdb_player_get(ggzdbPlayerEntry *);


/* Function to initialize the database system */
int ggzdb_init(void)
{
	int rc;

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

	_ggzdb_enter();
	if(player_needs_init)
		rc = ggzdb_player_init();

	if(rc == 0)
		rc = _ggzdb_player_add(pe);

	_ggzdb_exit();
	return rc;
}


/* Function to retrieve a player from the database */
int ggzdb_player_get(ggzdbPlayerEntry *pe)
{
	int rc=0;

	_ggzdb_enter();
	if(player_needs_init)
		rc = ggzdb_player_init();

	if(rc == 0)
		rc = _ggzdb_player_get(pe);

	_ggzdb_exit();
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
