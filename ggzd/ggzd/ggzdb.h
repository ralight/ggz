/*
 * File: ggzdb.h
 * Author: Rich Gade
 * Project: GGZ Server
 * Date: 06/11/2000
 * Desc: Front-end functions for handling database manipulation
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

#include "ggzd.h"
#include <time.h>

/* Can't include twice */
#ifndef GGZDB_VERSION_ID

/* Structures to use to pass data to front end functions */

/* Anytime structures change, bump the version id */
#define GGZDB_VERSION_ID	"0.4"

/* For ggzdb_player_XXX */
typedef struct ggzdbPlayerEntry {
	unsigned int user_id;			/* Numeric UID */
	char handle[MAX_USER_NAME_LEN+1];	/* Players nickname/handle */
	char password[17];			/* Players password */
	char name[33];				/* Players real name */
	char email[33];				/* Players email address */
	time_t last_login;			/* Last login */
	unsigned int perms;			/* Permission settings */
} ggzdbPlayerEntry;


/* Error codes */
typedef enum {
	GGZDB_NO_ERROR,		/* All's well */
	GGZDB_ERR_INIT,		/* Engine not initialized */
	GGZDB_ERR_DUPKEY,	/* Tried to overwrite on add */
	GGZDB_ERR_NOTFOUND	/* Couldn't find a record */
} GGZDBResult;


/* Exported functions */
extern int ggzdb_init(void);
extern void ggzdb_close(void);
extern int ggzdb_player_add(ggzdbPlayerEntry *);
extern int ggzdb_player_update(ggzdbPlayerEntry *);
extern int ggzdb_player_get(ggzdbPlayerEntry *);
extern int ggzdb_player_delete(char *handle);
extern unsigned int ggzdb_player_next_uid(void);


#ifdef DB_STANDALONE
/* For standalone programs */
extern int _ggzdb_init(char *datadir, int standalone);
extern void _ggzdb_close(void);
extern void _ggzdb_enter(void);
extern void _ggzdb_exit(void);
extern int _ggzdb_init_player(char *datadir);
extern int _ggzdb_player_add(ggzdbPlayerEntry *);
extern int _ggzdb_player_get(ggzdbPlayerEntry *);
extern int _ggzdb_player_update(ggzdbPlayerEntry *);
extern int _ggzdb_player_get_first(ggzdbPlayerEntry *);
extern int _ggzdb_player_get_next(ggzdbPlayerEntry *);
extern void _ggzdb_player_drop_cursor(void);
#endif

#endif
