/*
 * File: perms.h
 * Author: Rich Gade
 * Project: GGZ Server
 * Date: 9/23/01
 * Desc: Functions for dealing with user permissions
 *
 * Copyright (C) 2001 Brent Hendricks.
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

#ifndef _GGZ_PERMS_H
#define _GGZ_PERMS_H

#include "ggzdb.h"
#include "players.h"

/* If adding/editing perms, be sure to update perm_names down below. */
typedef enum {
	PERMS_JOIN_TABLE,	/* Can join a table */
	PERMS_LAUNCH_TABLE,	/* Can launch a new table */
	PERMS_ROOMS_LOGIN,	/* Can enter login-only rooms */
	PERMS_ROOMS_ADMIN,	/* Can enter admin-only rooms */
	PERMS_CHAT_ANNOUNCE,	/* Can make announcements */
	PERMS_CHAT_BOT,		/* Player is a known bot */
	PERMS_NO_STATS,		/* No stats for this player. */
	PERMS_EDIT_TABLES,	/* Can edit tables. */
	PERMS_TABLE_PRIVMSG,	/* Can send private messages at a table */
	PERMS_COUNT		/* placeholder */
} Perm;

typedef unsigned int Permset;

#define perms_is_host(ps) (perms_is_set((ps), PERMS_EDIT_TABLES)	\
			   || perms_is_set((ps), PERMS_TABLE_PRIVMSG))
#define perms_is_bot(ps) (perms_is_set((ps), PERMS_CHAT_BOT))
#define perms_is_admin(ps) (perms_is_set((ps), PERMS_ROOMS_ADMIN)	\
			    && perms_is_set((ps), PERMS_CHAT_ANNOUNCE)	\
			    && perms_is_set((ps), PERMS_EDIT_TABLES)	\
			    && perms_is_set((ps), PERMS_TABLE_PRIVMSG))

/* Permissions for newly-registered and anonymous users. */
extern Perm perms_default_anon[], perms_default[];
extern const int num_perms_default_anon, num_perms_default;

/* These functions must already have a player read-lock. */
void perms_init_from_db(Permset *perms, ggzdbPlayerEntry *db_pe);
void perms_init_from_list(Permset *perms, Perm *list, size_t listsz);
bool perms_is_set(Permset perms, Perm perm);

/* Acquires a read-lock and checks for the permission. */
bool perms_check(GGZPlayer *player, Perm perm);

extern char *perm_names[PERMS_COUNT];

#endif
