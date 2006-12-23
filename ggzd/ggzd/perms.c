/*
 * File: perms.c
 * Author: Rich Gade
 * Project: GGZ Server
 * Date: 9/23/01
 * Desc: Functions for dealing with user permissions
 * $Id: perms.c 8744 2006-12-23 06:27:16Z jdorje $
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

#ifdef HAVE_CONFIG_H
# include <config.h>		/* Site specific config */
#endif

#include "err_func.h"
#include "ggzd.h"
#include "ggzdb.h"
#include "perms.h"
#include "players.h"

void perms_init_from_db(Permset *perms, ggzdbPlayerEntry *pe)
{
	*perms = pe->perms;
}


void perms_init_from_list(Permset *perms, Perm *list, size_t listsz)
{
	int i;

	*perms = 0;
	for (i = 0; i < listsz; i++) {
		*perms |= (1 << list[i]);
	}
}


bool perms_is_set(Permset perms, Perm perm)
{
	return (perms & (1 << perm)) != 0;
}


bool perms_check(GGZPlayer *player, Perm perm)
{
	bool result;

	if (perm < 0 || perm >= PERMS_COUNT) {
	  err_msg("Invalid perm %d.", perm);
	  return false;
	}

	pthread_rwlock_rdlock(&player->lock);
	result = perms_is_set(player->perms, perm);
	pthread_rwlock_unlock(&player->lock);

	return result;
}

char *perm_names[PERMS_COUNT] = {
	"PERMS_JOIN_TABLE",		/* 00000001 */
	"PERMS_LAUNCH_TABLE",
	"PERMS_ROOMS_LOGIN",
	"PERMS_ROOMS_ADMIN",
	"PERMS_CHAT_ANNOUNCE",		/* 00000010 */
	"PERMS_CHAT_BOT",
	"PERMS_NO_STATS",
	"PERMS_EDIT_TABLES",
	"PERMS_TABLE_PRIVMSG"		/* 00000100 */
};


Perm perms_default_anon[] = {
  PERMS_JOIN_TABLE, PERMS_LAUNCH_TABLE, PERMS_NO_STATS
};

/* Permissions for newly-registered user. */
Perm perms_default[] = {
  PERMS_JOIN_TABLE, PERMS_LAUNCH_TABLE, PERMS_ROOMS_LOGIN
};

const int num_perms_default_anon = ARRAY_SIZE(perms_default_anon);
const int num_perms_default = ARRAY_SIZE(perms_default);
