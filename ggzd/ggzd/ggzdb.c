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
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>


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
static void ggzdb_player_lowercase(ggzdbPlayerEntry *pe, char *orig);

/* Back-end functions */
extern int _ggzdb_init(char *datadir, int standalone);
extern void _ggzdb_close(void);
extern void _ggzdb_enter(void);
extern void _ggzdb_exit(void);
extern int _ggzdb_init_player(char *datadir);
extern int _ggzdb_player_add(ggzdbPlayerEntry *);
extern int _ggzdb_player_get(ggzdbPlayerEntry *);
extern int _ggzdb_player_update(ggzdbPlayerEntry *);


/* Function to initialize the database system */
int ggzdb_init(void)
{
	char *fname;
	char vid[7];	/* Space for 123.45 */
	char version_ok=0;
	FILE *vfile;
	int rc;

	/* Verify that db version is cool with us */
	if((fname = malloc(strlen(opt.data_dir)+11)) == NULL)
		err_sys_exit("malloc() failed in ggzdb_init()");
	strcpy(fname, opt.data_dir);
	strcat(fname, "/ggzdb.ver");
	if((vfile = fopen(fname, "r")) == NULL) {
		/* File not found, so we can create it */
		if((vfile = fopen(fname, "w")) == NULL)
			err_sys_exit("fopen(w) failed in ggzdb_init()");
		fprintf(vfile, "%s", GGZDB_VERSION_ID);
		version_ok = 1;
	} else {
		/* File was found, so let's check it */
		fgets(vid, 7, vfile);
		if(!strncmp(GGZDB_VERSION_ID, vid, strlen(GGZDB_VERSION_ID)))
			version_ok = 1;
	}
	fclose(vfile);
	free(fname);

	if(!version_ok)
		err_msg_exit("Bad db version id, remove or convert db files");

	/* Call backend's initialization */
	rc = _ggzdb_init(opt.data_dir, 0);
	if(rc == 0)
		db_needs_init = 0;

	return rc;
}


/* Function to TERMINATE database usage */
void ggzdb_close(void)
{
	_ggzdb_close();
}


/* Function to add a player to the database */
int ggzdb_player_add(ggzdbPlayerEntry *pe)
{
	int rc=0;
	char orig[MAX_USER_NAME_LEN + 1];
	
	/* Lowercase player's name for comparison, saving original */
	ggzdb_player_lowercase(pe, orig);

	_ggzdb_enter();
	if(player_needs_init)
		rc = ggzdb_player_init();

	if(rc == 0)
		rc = _ggzdb_player_add(pe);

	/* Restore the original name */
	strcpy(pe->handle, orig);
	
	_ggzdb_exit();
	return rc;
}


/* Function to retrieve a player from the database */
int ggzdb_player_get(ggzdbPlayerEntry *pe)
{
	int rc=0;
	char orig[MAX_USER_NAME_LEN + 1];

	/* Lowercase player's name for comparison, saving original */
	ggzdb_player_lowercase(pe, orig);

	dbg_msg(GGZ_DBG_CONNECTION, "Getting player (%s) as (%s)..", orig, pe->handle);
	
	_ggzdb_enter();
	if(player_needs_init)
		rc = ggzdb_player_init();

	if(rc == 0)
		rc = _ggzdb_player_get(pe);

	dbg_msg(GGZ_DBG_CONNECTION, "result was %d", rc);

	/* Restore the original name */
	strcpy(pe->handle, orig);
	
	_ggzdb_exit();
	return rc;
}


/* Function to update a player's entry in the database */
int ggzdb_player_update(ggzdbPlayerEntry *pe)
{
	int rc=0;
	char orig[MAX_USER_NAME_LEN + 1];

	/* Lowercase player's name for comparison, saving original */
	ggzdb_player_lowercase(pe, orig);

	_ggzdb_enter();
	if(player_needs_init)
		rc = ggzdb_player_init();

	if(rc == 0)
		rc = _ggzdb_player_update(pe);

	/* Restore the original name */
	strcpy(pe->handle, orig);
	
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


static void ggzdb_player_lowercase(ggzdbPlayerEntry *pe, char *buf)
{
	char *src, *dest;

	/* Save original name in caller provided buffer */
	strcpy(buf, pe->handle);
	
	/* Convert name to lowercase for comparisons */
	for(src=buf, dest=pe->handle; *src!='\0'; src++, dest++)
		*dest = tolower(*src);
	*dest = '\0';
}
