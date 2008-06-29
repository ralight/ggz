/*
 * File: ggzdb.c
 * Author: Rich Gade
 * Project: GGZ Server
 * Date: 06/11/2000
 * Desc: Front-end functions to handle database manipulation
 * $Id: ggzdb.c 10104 2008-06-29 13:35:28Z oojah $
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
#include <dlfcn.h>

#include "datatypes.h"
#include "err_func.h"
#include "ggzd.h"
#include "ggzdb.h"
#include "ggzdb_functions.h"


/* Server-wide variables */
static const char *db_hashing;
static const char *db_hashencoding;

/* Internal variables */
static char db_needs_init = 1;
static char player_needs_init = 1;
static char stats_needs_init = 1;
static void *ggzdbhandle = NULL; /* For e.g. ggzdb_mysql.so */
static void *dblibhandle = NULL; /* For e.g. libmysqlclient_r.so */

/* Internal functions */
static GGZDBResult ggzdb_player_init(void);
static GGZDBResult ggzdb_stats_init(void);
static void ggzdb_player_lowercase(ggzdbPlayerEntry *pe, char *orig);

/* Helper function to get guaranteed uniqueness over time and (local) space  */
/* For architecture-specific size constraints, see definition in ggzdb.h     */
/* FIXME: this is ugly */
static time_t serverstarttime = 0;
ggzdbStamp unique_thread_id(void)
{
	ggzdbStamp stamp;

	if(!serverstarttime)
		serverstarttime = time(NULL);

	stamp.thread = pthread_self();
	stamp.starttime = serverstarttime;
	return stamp;
}

/* Return the most suitable database backend, which must be ggz_free()'d */
char *ggzdb_get_default_backend(void)
{
	char *primarybackend = NULL;
	char *token, *nexttoken;
	char *backendlist = ggz_strdup(DATABASE_TYPES);
	char *backendlistptr = backendlist;
	bool gooddefault = false;
	token = strtok(backendlist, ",");
	while(token) {
		nexttoken = strtok(NULL, ",");
		if((!ggz_strcmp(token, "db4"))
		|| (!ggz_strcmp(token, "sqlite")))
			gooddefault = true;
		if((!nexttoken) || (gooddefault)) {
			primarybackend = ggz_strdup(token);
			break;
		}
		token = nexttoken;
	}
	ggz_free(backendlistptr);
	return primarybackend;
}

/* Function to initialize the database system */
int ggzdb_init(ggzdbConnection connection, bool standalone)
{
	/* Verify that db version is cool with us */
	/* FIXME: move all of that into db4.c! */
	if(!ggz_strcmp(connection.type, "db4")) {
		const char *suffix = "/ggzdb.ver";
		char fname[strlen(connection.datadir) + strlen(suffix) + 1];
		char vid[7];	/* Space for 123.45 */
		char version_ok = 0;
		FILE *vfile;

		snprintf(fname, sizeof(fname), "%s%s", connection.datadir, suffix);

		if((vfile = fopen(fname, "r")) == NULL) {
			/* File not found, so we can create it */
			if((vfile = fopen(fname, "w")) == NULL)
				ggz_error_sys_exit("fopen(w) failed in ggzdb_init()");
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
			printf("Bad GGZ database version %s, expected %s.\n"
			       "Most likely this means you must upgrade your\n"
			       "database schema.  It may be possible to automate this;\n"
			       "see http://ggzgamingzone.org.\n",
			       vid, GGZDB_VERSION_ID);
			printf("Database location: %s\n", fname);
			exit(-1);
			/* FIXME: we should be able to rely on ggz_ggz_error_sys() */
		}
	}

	/* Load the database backend plugin */
	const char *backend = connection.type;
	const char *primarybackend = NULL;
	if(!backend){
		primarybackend = ggzdb_get_default_backend();
		backend = primarybackend;
	}

	if(!strcmp(backend, "db4")){
		dblibhandle = dlopen("libdb.so", RTLD_NOW | RTLD_GLOBAL);
		if(!dblibhandle){
			ggz_error_msg_exit("Unable to open libdb.so (%s)",
				dlerror());
		}
	}else if(!strcmp(backend, "dbi")){
		dblibhandle = dlopen("libdbi.so", RTLD_NOW | RTLD_GLOBAL);
		if(!dblibhandle){
			ggz_error_msg_exit("Unable to open libdbi.so (%s)",
				dlerror());
		}
	}else if(!strcmp(backend, "mysql")){
		dblibhandle = dlopen("libmysqlclient_r.so", RTLD_NOW | RTLD_GLOBAL);
		if(!dblibhandle){
			ggz_error_msg_exit("Unable to open libmysqlclient_r.so (%s)",
				dlerror());
		}
	}else if(!strcmp(backend, "pgsql")){
		dblibhandle = dlopen("libpq.so", RTLD_NOW | RTLD_GLOBAL);
		if(!dblibhandle){
			ggz_error_msg_exit("Unable to open libpq.so (%s)",
				dlerror());
		}
	}else if(!strcmp(backend, "sqlite")){
		dblibhandle = dlopen("libsqlite3.so", RTLD_NOW | RTLD_GLOBAL);
		if(!dblibhandle){
			ggz_error_msg_exit("Unable to open libsqlite3.so (%s)",
				dlerror());
		}
	}

	char backendmodule[128];
	snprintf(backendmodule, sizeof(backendmodule),
		"%s/database/libggzdb_%s.so",
		GGZDEXECMODDIR, backend);
	ggzdbhandle = dlopen(backendmodule, RTLD_NOW);
	if(!ggzdbhandle) {
		ggz_error_msg_exit("%s (%s) is not a suitable database module (%s)",
			backend, backendmodule, dlerror());
	}

	if(primarybackend)
		ggz_free(primarybackend);

	if(((_ggzdb_init = dlsym(ggzdbhandle, "_ggzdb_init")) == NULL)
	|| ((_ggzdb_close = dlsym(ggzdbhandle, "_ggzdb_close")) == NULL)
	|| ((_ggzdb_enter = dlsym(ggzdbhandle, "_ggzdb_enter")) == NULL)
	|| ((_ggzdb_exit = dlsym(ggzdbhandle, "_ggzdb_exit")) == NULL)
	|| ((_ggzdb_init_player = dlsym(ggzdbhandle, "_ggzdb_init_player")) == NULL)
	|| ((_ggzdb_player_add = dlsym(ggzdbhandle, "_ggzdb_player_add")) == NULL)
	|| ((_ggzdb_player_get = dlsym(ggzdbhandle, "_ggzdb_player_get")) == NULL)
	|| ((_ggzdb_player_update = dlsym(ggzdbhandle, "_ggzdb_player_update")) == NULL)
	|| ((_ggzdb_player_get_first = dlsym(ggzdbhandle, "_ggzdb_player_get_first")) == NULL)
	|| ((_ggzdb_player_get_next = dlsym(ggzdbhandle, "_ggzdb_player_get_next")) == NULL)
	|| ((_ggzdb_player_get_extended = dlsym(ggzdbhandle, "_ggzdb_player_get_extended")) == NULL)
	|| ((_ggzdb_player_next_uid = dlsym(ggzdbhandle, "_ggzdb_player_next_uid")) == NULL)
	|| ((_ggzdb_player_drop_cursor = dlsym(ggzdbhandle, "_ggzdb_player_drop_cursor")) == NULL)
	|| ((_ggzdb_init_stats = dlsym(ggzdbhandle, "_ggzdb_init_stats")) == NULL)
	|| ((_ggzdb_stats_lookup = dlsym(ggzdbhandle, "_ggzdb_stats_lookup")) == NULL)
	|| ((_ggzdb_stats_update = dlsym(ggzdbhandle, "_ggzdb_stats_update")) == NULL)
	|| ((_ggzdb_stats_newmatch = dlsym(ggzdbhandle, "_ggzdb_stats_newmatch")) == NULL)
	|| ((_ggzdb_stats_savegame = dlsym(ggzdbhandle, "_ggzdb_stats_savegame")) == NULL)
	|| ((_ggzdb_stats_match = dlsym(ggzdbhandle, "_ggzdb_stats_match")) == NULL)
	|| ((_ggzdb_stats_toprankings = dlsym(ggzdbhandle, "_ggzdb_stats_toprankings")) == NULL)
	|| ((_ggzdb_stats_calcrankings = dlsym(ggzdbhandle, "_ggzdb_stats_calcrankings")) == NULL)
	|| ((_ggzdb_savegames = dlsym(ggzdbhandle, "_ggzdb_savegames")) == NULL)
	|| ((_ggzdb_savegame_owners = dlsym(ggzdbhandle, "_ggzdb_savegame_owners")) == NULL)
	|| ((_ggzdb_savegame_player = dlsym(ggzdbhandle, "_ggzdb_savegame_player")) == NULL)
	|| ((_ggzdb_rooms = dlsym(ggzdbhandle, "_ggzdb_rooms")) == NULL)
	|| ((_ggzdb_reconfiguration_fd = dlsym(ggzdbhandle, "_ggzdb_reconfiguration_fd")) == NULL)
	|| ((_ggzdb_reconfiguration_load = dlsym(ggzdbhandle, "_ggzdb_reconfiguration_load")) == NULL)
	|| ((_ggzdb_reconfiguration_room = dlsym(ggzdbhandle, "_ggzdb_reconfiguration_room")) == NULL)
	)
	{
		ggz_error_msg_exit("%s is an invalid database module (%s)",
			backend, dlerror());
	}

	if(connection.hashing)
		db_hashing = ggz_strdup(connection.hashing);
	if(connection.hashencoding)
		db_hashencoding = ggz_strdup(connection.hashencoding);

	/* Call backend's initialization */
	if (_ggzdb_init(connection, standalone) != GGZ_OK)
		return GGZ_ERROR;
	db_needs_init = 0;

	return GGZ_OK;
}


/* Function to TERMINATE database usage */
void ggzdb_close(void)
{
	if(db_hashing)
		ggz_free(db_hashing);
	if(db_hashencoding)
		ggz_free(db_hashencoding);

	_ggzdb_close();
	dlclose(ggzdbhandle);
	dlclose(dblibhandle);
}


/* Not threadsafe: return file descriptor for notifications */
int ggzdb_reconfiguration_fd(void)
{
	return _ggzdb_reconfiguration_fd();
}


/* Not threadsafe: trigger mass room addition on startup */
void ggzdb_reconfiguration_load(void)
{
	_ggzdb_reconfiguration_load();
}


/* Function to add a player to the database */
GGZDBResult ggzdb_player_add(ggzdbPlayerEntry *pe)
{
	GGZDBResult rc = GGZDB_NO_ERROR;
	char orig[MAX_USER_NAME_LEN + 1];
	hash_t hash;
	char *password_enc;
	char *origpassword = NULL;

	if(!db_hashing) return GGZDB_ERR_DB;

	/* Lowercase player's name for comparison, saving original */
	ggzdb_player_lowercase(pe, orig);

	_ggzdb_enter();
	if(player_needs_init)
		rc = ggzdb_player_init();

	if(rc == GGZDB_NO_ERROR)
	{
		if((!strcmp(db_hashing, "md5"))
		|| (!strcmp(db_hashing, "sha1"))
		|| (!strcmp(db_hashing, "ripemd160")))
		{
			hash = ggz_hash_create(db_hashing, pe->password);
			if(!strcmp(db_hashencoding, "base16")){
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

	ggz_debug(GGZ_DBG_CONNECTION, "Getting player (%s) as (%s)..", orig, pe->handle);
	
	_ggzdb_enter();
	if(player_needs_init)
		rc = ggzdb_player_init();

	if(rc == GGZDB_NO_ERROR)
		rc = _ggzdb_player_get(pe);

	ggz_debug(GGZ_DBG_CONNECTION, "result was %d", rc);

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
	GGZDBResult rc = GGZDB_NO_ERROR;
	int ret = -1;
	_ggzdb_enter();
	if(player_needs_init)
		rc = ggzdb_player_init();

	if(rc == GGZDB_NO_ERROR)
		ret = _ggzdb_player_next_uid();

	_ggzdb_exit();
	return ret;
}


GGZDBResult ggzdb_player_get_first(ggzdbPlayerEntry *pe)
{
	GGZDBResult rc = GGZDB_NO_ERROR;
	_ggzdb_enter();
	if(player_needs_init)
		rc = ggzdb_player_init();

	if(rc == GGZDB_NO_ERROR)
		rc = _ggzdb_player_get_first(pe);

	_ggzdb_exit();
	return rc;
}


GGZDBResult ggzdb_player_get_next(ggzdbPlayerEntry *pe)
{
	GGZDBResult rc = GGZDB_NO_ERROR;
	_ggzdb_enter();
	if(player_needs_init)
		rc = ggzdb_player_init();

	if(rc == GGZDB_NO_ERROR)
		rc = _ggzdb_player_get_next(pe);

	_ggzdb_exit();
	return rc;
}


void ggzdb_player_drop_cursor(void)
{
	GGZDBResult rc = GGZDB_NO_ERROR;
	_ggzdb_enter();
	if(player_needs_init)
		rc = ggzdb_player_init();

	if(rc == GGZDB_NO_ERROR)
		_ggzdb_player_drop_cursor();

	_ggzdb_exit();
}


/* Function to retrieve additional player information */
GGZDBResult ggzdb_player_get_extended(ggzdbPlayerExtendedEntry *pe)
{
	GGZDBResult rc = GGZDB_NO_ERROR;

	ggz_debug(GGZ_DBG_CONNECTION, "Getting player %s's extended info.", pe->handle);
	
	_ggzdb_enter();
	if(player_needs_init)
		rc = ggzdb_player_init();

	if(rc == GGZDB_NO_ERROR)
		rc = _ggzdb_player_get_extended(pe);

	ggz_debug(GGZ_DBG_CONNECTION, "result was %d", rc);

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


GGZDBResult ggzdb_stats_savegame(const char *game, const char *owner, const char *savegame, ggzdbStamp tableid)
{
	GGZDBResult rc = GGZDB_NO_ERROR;

	_ggzdb_enter();

	if (stats_needs_init)
		rc = ggzdb_stats_init();

	if (rc == GGZDB_NO_ERROR)
		rc = _ggzdb_stats_savegame(game, owner, savegame, tableid);

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

GGZList *ggzdb_savegames(const char *game, const char *owner)
{
	GGZList *list = NULL;
	GGZDBResult rc = GGZDB_NO_ERROR;

	_ggzdb_enter();

	if (stats_needs_init)
		rc = ggzdb_stats_init();

	if (rc == GGZDB_NO_ERROR)
		list = _ggzdb_savegames(game, owner);

	_ggzdb_exit();

	return list;
}

GGZList *ggzdb_savegame_owners(const char *game)
{
	GGZList *list = NULL;
	GGZDBResult rc = GGZDB_NO_ERROR;

	_ggzdb_enter();

	if (stats_needs_init)
		rc = ggzdb_stats_init();

	if (rc == GGZDB_NO_ERROR)
		list = _ggzdb_savegame_owners(game);

	_ggzdb_exit();

	return list;
}

GGZDBResult ggzdb_savegameplayer(ggzdbStamp tableid, int seat, const char *name, int type)
{
	GGZDBResult rc = GGZDB_NO_ERROR;

	_ggzdb_enter();

	if (stats_needs_init)
		rc = ggzdb_stats_init();

	if (rc == GGZDB_NO_ERROR)
		rc = _ggzdb_savegame_player(tableid, seat, name, type);

	_ggzdb_exit();

	return rc;
}

GGZDBResult ggzdb_rooms(RoomStruct *rooms, int num)
{
	GGZDBResult rc = GGZDB_NO_ERROR;

	_ggzdb_enter();

	if (stats_needs_init)
		rc = ggzdb_stats_init();

	if (rc == GGZDB_NO_ERROR)
		rc = _ggzdb_rooms(rooms, num);

	_ggzdb_exit();

	return rc;
}

RoomStruct* ggzdb_reconfiguration_room(void)
{
	RoomStruct *rooms = NULL;
	GGZDBResult rc = GGZDB_NO_ERROR;

	_ggzdb_enter();

	if (stats_needs_init)
		rc = ggzdb_stats_init();

	if (rc == GGZDB_NO_ERROR)
		rooms = _ggzdb_reconfiguration_room();

	_ggzdb_exit();

	return rooms;
}


/*** INTERNAL FUNCTIONS ***/

/* Function to initialize player tables if necessary */
static GGZDBResult ggzdb_player_init(void)
{
	GGZDBResult rc;

	if(db_needs_init)
		return GGZDB_ERR_INIT;

	rc = _ggzdb_init_player();

	if (rc == GGZDB_NO_ERROR)
		player_needs_init = 0;

	return rc;
}


static GGZDBResult ggzdb_stats_init(void)
{
	GGZDBResult rc;

	if (db_needs_init)
		return GGZDB_ERR_INIT;

	rc = _ggzdb_init_stats();

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

	if(!db_hashing) return -1;
	if((!input) || (!password)) return -1;

	else if(!strcmp(db_hashing, "plain"))
	{
		if(!strcmp(input, password)) return 1;
		else return 0;
	}
	else if((!strcmp(db_hashing, "md5"))
	|| (!strcmp(db_hashing, "sha1"))
	|| (!strcmp(db_hashing, "ripemd160")))
	{
		hash = ggz_hash_create(db_hashing, input);
		if(!strcmp(db_hashencoding, "base16")){
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

	ggz_error_msg_exit("Unknown hashing type '%s'", db_hashing);
	
	return -1;
}

