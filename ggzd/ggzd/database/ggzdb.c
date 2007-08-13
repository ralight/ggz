/*
 * File: ggzdb.c
 * Author: Rich Gade
 * Project: GGZ Server
 * Date: 06/11/2000
 * Desc: Front-end functions to handle database manipulation
 * $Id: ggzdb.c 9245 2007-08-13 07:01:38Z josef $
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

#include <ctype.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "datatypes.h"
#include "err_func.h"
#include "ggzd.h"
#include "ggzdb.h"
#include "ggzdb_proto.h"


/* Server-wide variables */
extern Options opt;

/* Internal variables */
static char db_needs_init = 1;
static char player_needs_init = 1;
static char stats_needs_init = 1;

/* Internal functions */
static GGZDBResult ggzdb_player_init(void);
static GGZDBResult ggzdb_stats_init(void);
static void ggzdb_player_lowercase(ggzdbPlayerEntry *pe, char *orig);

/* Function to initialize the database system */
int ggzdb_init(void)
{
	const char *suffix = "/ggzdb.ver";
	char fname[strlen(opt.data_dir) + strlen(suffix) + 1];
	char vid[7];	/* Space for 123.45 */
	char version_ok=0;
	FILE *vfile;
	ggzdbConnection connection;

	/* Verify that db version is cool with us */
	connection.database = 0;
	if(!connection.database) {
		snprintf(fname, sizeof(fname), "%s%s", opt.data_dir, suffix);

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

		if (!version_ok) {
			printf("Bad db version id, remove or convert db files.\n"
			       "Most likely this means you must upgrade your\n"
		    	   "database.  It may be possible to automate this;\n"
			       "see http://ggzgamingzone.org.\n");
			exit(-1);
		}
	}

	/* Call backend's initialization */
	connection.type = opt.dbtype;
	connection.datadir = opt.data_dir;
	connection.host = opt.dbhost;
	connection.database = opt.dbname;
	connection.username = opt.dbusername;
	connection.password = opt.dbpassword;
	connection.hashing = opt.dbhashing;
	if (_ggzdb_init(connection, 0) != GGZ_OK)
		return GGZ_ERROR;
	db_needs_init = 0;

	return GGZ_OK;
}


/* Function to TERMINATE database usage */
void ggzdb_close(void)
{
	_ggzdb_close();
}


/* Function to add a player to the database */
GGZDBResult ggzdb_player_add(ggzdbPlayerEntry *pe)
{
	GGZDBResult rc = GGZDB_NO_ERROR;
	char orig[MAX_USER_NAME_LEN + 1];
	hash_t hash;
	char *password_enc;
	char *origpassword = NULL;

	if(!opt.dbhashing) return GGZDB_ERR_DB;

	/* Lowercase player's name for comparison, saving original */
	ggzdb_player_lowercase(pe, orig);

	_ggzdb_enter();
	if(player_needs_init)
		rc = ggzdb_player_init();

	if(rc == GGZDB_NO_ERROR)
	{
		if((!strcmp(opt.dbhashing, "md5"))
		|| (!strcmp(opt.dbhashing, "sha1"))
		|| (!strcmp(opt.dbhashing, "ripemd160")))
		{
			hash = ggz_hash_create(opt.dbhashing, pe->password);
			if(!strcmp(opt.dbhashencoding, "base16")){
				password_enc = ggz_base16_encode(hash.hash, hash.hashlen);
			}else{
				password_enc = ggz_base64_encode(hash.hash, hash.hashlen);
			}
			if(hash.hash)
				ggz_free(hash.hash);

			if(password_enc)
			{
				origpassword = ggz_strdup(pe->password);
				snprintf(pe->password, sizeof(pe->password), "%s", password_enc);
				ggz_free(password_enc);
			}
			// FIXME - shouldn't failing to encrypt the password be an error?
		}
		rc = _ggzdb_player_add(pe);
	}

	/* Restore the original name */
	strcpy(pe->handle, orig);
	if(origpassword)
	{
		snprintf(pe->password, sizeof(pe->password), "%s", origpassword);
		ggz_free(origpassword);
	}
	
	_ggzdb_exit();
	return rc;
}


/* Function to retrieve a player from the database */
GGZDBResult ggzdb_player_get(ggzdbPlayerEntry *pe)
{
	GGZDBResult rc = GGZDB_NO_ERROR;
	char orig[MAX_USER_NAME_LEN + 1];

	/* Lowercase player's name for comparison, saving original */
	ggzdb_player_lowercase(pe, orig);

	dbg_msg(GGZ_DBG_CONNECTION, "Getting player (%s) as (%s)..", orig, pe->handle);
	
	_ggzdb_enter();
	if(player_needs_init)
		rc = ggzdb_player_init();

	if(rc == GGZDB_NO_ERROR)
		rc = _ggzdb_player_get(pe);

	dbg_msg(GGZ_DBG_CONNECTION, "result was %d", rc);

	/* Restore the original name */
	strcpy(pe->handle, orig);
	
	_ggzdb_exit();
	return rc;
}


/* Function to update a player's entry in the database */
GGZDBResult ggzdb_player_update(ggzdbPlayerEntry *pe)
{
	GGZDBResult rc = GGZDB_NO_ERROR;
	char orig[MAX_USER_NAME_LEN + 1];

	/* Lowercase player's name for comparison, saving original */
	ggzdb_player_lowercase(pe, orig);

	_ggzdb_enter();
	if(player_needs_init)
		rc = ggzdb_player_init();

	if(rc == GGZDB_NO_ERROR)
		rc = _ggzdb_player_update(pe);

	/* Restore the original name */
	strcpy(pe->handle, orig);
	
	_ggzdb_exit();
	return rc;
}


/* Function to get the next valid user id to assign */
unsigned int ggzdb_player_next_uid(void)
{
	/* Just link to the db specific code */
	return _ggzdb_player_next_uid();
}


/* Function to retrieve additional player information */
GGZDBResult ggzdb_player_get_extended(ggzdbPlayerExtendedEntry *pe)
{
	GGZDBResult rc = GGZDB_NO_ERROR;

	dbg_msg(GGZ_DBG_CONNECTION, "Getting player %s's extended info.", pe->handle);
	
	_ggzdb_enter();
	if(player_needs_init)
		rc = ggzdb_player_init();

	if(rc == GGZDB_NO_ERROR)
		rc = _ggzdb_player_get_extended(pe);

	dbg_msg(GGZ_DBG_CONNECTION, "result was %d", rc);

	_ggzdb_exit();
	return rc;
}


GGZDBResult ggzdb_stats_lookup(ggzdbPlayerGameStats *stats)
{
	GGZDBResult rc = GGZDB_NO_ERROR;

	_ggzdb_enter();

	if (stats_needs_init)
		rc = ggzdb_stats_init();

	if (rc == GGZDB_NO_ERROR)
		rc = _ggzdb_stats_lookup(stats);

	_ggzdb_exit();

	return rc;
}


GGZDBResult ggzdb_stats_newmatch(const char *game, const char *winner, const char *savegame)
{
	GGZDBResult rc = GGZDB_NO_ERROR;

	_ggzdb_enter();

	if (stats_needs_init)
		rc = ggzdb_stats_init();

	if (rc == GGZDB_NO_ERROR)
		rc = _ggzdb_stats_newmatch(game, winner, savegame);

	_ggzdb_exit();

	return rc;
}


GGZDBResult ggzdb_stats_update(ggzdbPlayerGameStats *stats)
{
	GGZDBResult rc = GGZDB_NO_ERROR;

	_ggzdb_enter();

	if (stats_needs_init)
		rc = ggzdb_stats_init();

	if (rc == GGZDB_NO_ERROR)
		rc = _ggzdb_stats_update(stats);

	_ggzdb_exit();

	return rc;
}


GGZDBResult ggzdb_stats_savegame(const char *game, const char *owner, const char *savegame)
{
	GGZDBResult rc = GGZDB_NO_ERROR;

	_ggzdb_enter();

	if (stats_needs_init)
		rc = ggzdb_stats_init();

	if (rc == GGZDB_NO_ERROR)
		rc = _ggzdb_stats_savegame(game, owner, savegame);

	_ggzdb_exit();

	return rc;
}


GGZDBResult ggzdb_stats_toprankings(const char *game, int number, ggzdbPlayerGameStats **rankings)
{
	GGZDBResult rc = GGZDB_NO_ERROR;

	_ggzdb_enter();

	if (stats_needs_init)
		rc = ggzdb_stats_init();

	if (rc == GGZDB_NO_ERROR)
		rc = _ggzdb_stats_toprankings(game, number, rankings);

	_ggzdb_exit();

	return rc;
}


GGZDBResult ggzdb_stats_calcrankings(const char *game)
{
	GGZDBResult rc = GGZDB_NO_ERROR;

	_ggzdb_enter();

	if (stats_needs_init)
		rc = ggzdb_stats_init();

	if (rc == GGZDB_NO_ERROR)
		rc = _ggzdb_stats_calcrankings(game);

	_ggzdb_exit();

	return rc;
}


/*** INTERNAL FUNCTIONS ***/

/* Function to initialize player tables if necessary */
static GGZDBResult ggzdb_player_init(void)
{
	GGZDBResult rc;

	if(db_needs_init)
		return GGZDB_ERR_INIT;

	rc = _ggzdb_init_player(opt.data_dir);
	if (rc == GGZDB_NO_ERROR)
		player_needs_init = 0;

	return rc;
}


static GGZDBResult ggzdb_stats_init(void)
{
	GGZDBResult rc;
	ggzdbConnection connection;

	if (db_needs_init)
		return GGZDB_ERR_INIT;

	connection.datadir = opt.data_dir;
	connection.host = opt.dbhost;
	connection.database = opt.dbname;
	connection.username = opt.dbusername;
	connection.password = opt.dbpassword;
	connection.hashing = opt.dbhashing;

	rc = _ggzdb_init_stats(connection);
	if (rc == GGZDB_NO_ERROR)
		stats_needs_init = 0;

	return rc;
}


static void ggzdb_player_lowercase(ggzdbPlayerEntry *pe, char *buf)
{
	char *src, *dest;

	/* Save original name in caller provided buffer */
	strcpy(buf, pe->handle);

	/* Convert name to lowercase for comparisons */
	/* FIXME: tolower() is evil. Must use unicode canonicalization. */
	for(src=buf, dest=pe->handle; *src!='\0'; src++, dest++)
		*dest = tolower(*src);
	*dest = '\0';
}


int ggzdb_compare_password(const char *input, const char *password)
{
	hash_t hash;
	char *password_enc;
	int ret = 0;

	if(!opt.dbhashing) return -1;
	if((!input) || (!password)) return -1;

	else if(!strcmp(opt.dbhashing, "plain"))
	{
		if(!strcmp(input, password)) return 1;
		else return 0;
	}
	else if((!strcmp(opt.dbhashing, "md5"))
	|| (!strcmp(opt.dbhashing, "sha1"))
	|| (!strcmp(opt.dbhashing, "ripemd160")))
	{
		hash = ggz_hash_create(opt.dbhashing, input);
		if(!strcmp(opt.dbhashencoding, "base16")){
			password_enc = ggz_base16_encode(hash.hash, hash.hashlen);
		}else{
			password_enc = ggz_base64_encode(hash.hash, hash.hashlen);
		}
		if(hash.hash)
			ggz_free(hash.hash);

		if(!password_enc) return -1;
		if(!strcmp(password_enc, password)) ret = 1;
		ggz_free(password_enc);
		return ret;
	}

	err_msg_exit("Unknown hashing type '%s'", opt.dbhashing);
	
	return -1;
}

/* Helper function, might go into libggz*/
char *_ggz_sql_escape(const char *str)
{
	char *newstr, *q;
	const char *p;
	size_t len = 0;

	if(str == NULL)
		return NULL;

	len = strlen(str);

	for(p = str; *p != '\0'; p++) {
		if(*p == '\'') {
			len += 1;
		}
	}

	if(len == strlen(str))
		return ggz_strdup(str);

	newstr = ggz_malloc(len + 1);
	q = newstr;

	for(p = str; *p != '\0'; p++) {
		if(*p == '\'') {
			*q++ = '\\';
			*q = *p;
		} else {
			*q = *p;
		}
		q++;
	}
	*q = '\0';

	return newstr;
}

/* Helper function, might go into libggz*/
/*char *_ggz_sql_unescape(const char *str)
{
	char *new, *q;
	const char *p;
	size_t len = 0;

	if(str == NULL)
		return NULL;

	len = strlen(str);

	for(p = str; *p != '\0'; p++) {
		if(!strncmp(p, "\\'", 2)) {
			p += 1;
		}
		len++;
	}

	if(len == strlen(str))
		return ggz_strdup(str);

	q = new = ggz_malloc(len + 1);
	for(p = str; *p != '\0'; p++) {
		if(!strncmp(p, "\\'", 2)) {
			*q = '\'';
			q++;
			p += 1;
		} else {
			*q = *p;
			q++;
		}
	}
	*q = '\0';

	return new;
}*/

