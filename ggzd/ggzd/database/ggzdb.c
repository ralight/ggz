/*
 * File: ggzdb.c
 * Author: Rich Gade
 * Project: GGZ Server
 * Date: 06/11/2000
 * Desc: Front-end functions to handle database manipulation
 * $Id: ggzdb.c 10816 2009-01-04 17:53:13Z oojah $
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
static void *ggzdbhandle = NULL; /* For e.g. ggzdb_mysql.so */

/* Internal functions */
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

/* Return the most suitable database backend, which is a constant #define and
 * must not be freed */
const char *ggzdb_get_default_backend(void)
{
	return DEFAULT_DB;
}

/* Function to initialize the database system */
int ggzdb_init(ggzdbConnection connection, bool standalone)
{
	/* Load the database backend plugin */
	const char *backend = connection.type;
	const char *primarybackend = NULL;
	char *error;

	if(!backend){
		primarybackend = ggzdb_get_default_backend();
		backend = primarybackend;
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

#define DL_LOAD(x) (x) = dlsym(ggzdbhandle, #x)

	/* Required backend functions. */
	DL_LOAD(_ggzdb_init);
	DL_LOAD(_ggzdb_close);
	DL_LOAD(_ggzdb_enter);
	DL_LOAD(_ggzdb_exit);
	DL_LOAD(_ggzdb_player_add);
	DL_LOAD(_ggzdb_player_get);
	DL_LOAD(_ggzdb_player_update);
	DL_LOAD(_ggzdb_player_get_first);
	DL_LOAD(_ggzdb_player_get_next);
	DL_LOAD(_ggzdb_player_get_extended);
	DL_LOAD(_ggzdb_player_next_uid);
	DL_LOAD(_ggzdb_player_drop_cursor);
	DL_LOAD(_ggzdb_stats_lookup);
	DL_LOAD(_ggzdb_stats_update);
	DL_LOAD(_ggzdb_stats_newmatch);
	DL_LOAD(_ggzdb_stats_savegame);
	DL_LOAD(_ggzdb_stats_match);
	DL_LOAD(_ggzdb_stats_toprankings);
	DL_LOAD(_ggzdb_stats_calcrankings);
	DL_LOAD(_ggzdb_savegames);
	DL_LOAD(_ggzdb_savegame_owners);
	DL_LOAD(_ggzdb_savegame_player);
	DL_LOAD(_ggzdb_rooms);
	DL_LOAD(_ggzdb_reconfiguration_fd);
	DL_LOAD(_ggzdb_reconfiguration_load);
	DL_LOAD(_ggzdb_reconfiguration_room);

	error = dlerror();
	if (error) {
		ggz_error_msg_exit("Error loading database module %s (%s)",
				   backend, error);
	}

	/* These two functions are optional so we don't need to worry if they aren't provided. */
	DL_LOAD(_ggzdb_escape);
	DL_LOAD(_ggzdb_unescape);

#undef DL_LOAD

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
	ggzdbhandle = NULL;
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
	ret = _ggzdb_player_next_uid();
	_ggzdb_exit();

	return ret;
}


GGZDBResult ggzdb_player_get_first(ggzdbPlayerEntry *pe)
{
	GGZDBResult rc = GGZDB_NO_ERROR;

	_ggzdb_enter();
	rc = _ggzdb_player_get_first(pe);
	_ggzdb_exit();

	return rc;
}


GGZDBResult ggzdb_player_get_next(ggzdbPlayerEntry *pe)
{
	GGZDBResult rc = GGZDB_NO_ERROR;

	_ggzdb_enter();
	rc = _ggzdb_player_get_next(pe);
	_ggzdb_exit();

	return rc;
}


void ggzdb_player_drop_cursor(void)
{
	GGZDBResult rc = GGZDB_NO_ERROR;

	_ggzdb_enter();
	_ggzdb_player_drop_cursor();
	_ggzdb_exit();
}


/* Function to retrieve additional player information */
GGZDBResult ggzdb_player_get_extended(ggzdbPlayerExtendedEntry *pe)
{
	GGZDBResult rc = GGZDB_NO_ERROR;

	ggz_debug(GGZ_DBG_CONNECTION, "Getting player %s's extended info.", pe->handle);
	
	_ggzdb_enter();
	rc = _ggzdb_player_get_extended(pe);
	ggz_debug(GGZ_DBG_CONNECTION, "result was %d", rc);
	_ggzdb_exit();

	return rc;
}


GGZDBResult ggzdb_stats_lookup(ggzdbPlayerGameStats *stats)
{
	GGZDBResult rc = GGZDB_NO_ERROR;

	_ggzdb_enter();
	rc = _ggzdb_stats_lookup(stats);
	_ggzdb_exit();

	return rc;
}


GGZDBResult ggzdb_stats_newmatch(const char *game, const char *winner, const char *savegame)
{
	GGZDBResult rc = GGZDB_NO_ERROR;

	_ggzdb_enter();
	rc = _ggzdb_stats_newmatch(game, winner, savegame);
	_ggzdb_exit();

	return rc;
}


GGZDBResult ggzdb_stats_update(ggzdbPlayerGameStats *stats)
{
	GGZDBResult rc = GGZDB_NO_ERROR;

	_ggzdb_enter();
	rc = _ggzdb_stats_update(stats);
	_ggzdb_exit();

	return rc;
}


GGZDBResult ggzdb_stats_savegame(const char *game, const char *owner, const char *savegame, ggzdbStamp tableid)
{
	GGZDBResult rc = GGZDB_NO_ERROR;

	if (!game || !owner || !savegame) {
		ggz_error_msg("NULL strings in ggzdb_stats_savegame: %s %s %s",
			      game, owner, savegame);
		return GGZDB_NO_ERROR;
	}

	_ggzdb_enter();
	rc = _ggzdb_stats_savegame(game, owner, savegame, tableid);
	_ggzdb_exit();

	return rc;
}


GGZDBResult ggzdb_stats_toprankings(const char *game, int number, ggzdbPlayerGameStats **rankings)
{
	GGZDBResult rc = GGZDB_NO_ERROR;

	_ggzdb_enter();
	rc = _ggzdb_stats_toprankings(game, number, rankings);
	_ggzdb_exit();

	return rc;
}


GGZDBResult ggzdb_stats_calcrankings(const char *game)
{
	GGZDBResult rc = GGZDB_NO_ERROR;

	_ggzdb_enter();
	rc = _ggzdb_stats_calcrankings(game);
	_ggzdb_exit();

	return rc;
}

GGZList *ggzdb_savegames(const char *game, const char *owner)
{
	GGZList *list = NULL;
	GGZDBResult rc = GGZDB_NO_ERROR;

	_ggzdb_enter();
	list = _ggzdb_savegames(game, owner);
	_ggzdb_exit();

	return list;
}

GGZList *ggzdb_savegame_owners(const char *game)
{
	GGZList *list = NULL;
	GGZDBResult rc = GGZDB_NO_ERROR;

	_ggzdb_enter();
	list = _ggzdb_savegame_owners(game);
	_ggzdb_exit();

	return list;
}

GGZDBResult ggzdb_savegameplayer(ggzdbStamp tableid, int seat, const char *name, int type)
{
	GGZDBResult rc = GGZDB_NO_ERROR;

	_ggzdb_enter();
	rc = _ggzdb_savegame_player(tableid, seat, name, type);
	_ggzdb_exit();

	return rc;
}

GGZDBResult ggzdb_rooms(RoomStruct *rooms, int num)
{
	GGZDBResult rc = GGZDB_NO_ERROR;

	_ggzdb_enter();
	rc = _ggzdb_rooms(rooms, num);
	_ggzdb_exit();

	return rc;
}

RoomStruct* ggzdb_reconfiguration_room(void)
{
	RoomStruct *rooms = NULL;
	GGZDBResult rc = GGZDB_NO_ERROR;

	_ggzdb_enter();
	rooms = _ggzdb_reconfiguration_room();
	_ggzdb_exit();

	return rooms;
}


/*** INTERNAL FUNCTIONS ***/

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

