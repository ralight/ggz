/*
 * File: perms.c
 * Author: Rich Gade
 * Project: GGZ Server
 * Date: 9/23/01
 * Desc: Functions for dealing with user permissions
 * $Id: perms.c 8763 2006-12-27 10:02:33Z jdorje $
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

void perms_init_from_db(GGZPermset *perms, ggzdbPlayerEntry *pe)
{
	*perms = pe->perms;
}

/* Returns TRUE on success, FALSE on failure. */
bool perms_set_to_db(GGZPermset perms, ggzdbPlayerEntry *pe)
{
	pe->perms = perms;
	return (ggzdb_player_update(pe) != GGZDB_NO_ERROR);
}


bool perms_check(GGZPlayer *player, GGZPerm perm)
{
	bool result;

	if (perm < 0 || perm >= GGZ_PERM_COUNT) {
	  err_msg("Invalid perm %d.", perm);
	  return false;
	}

	pthread_rwlock_rdlock(&player->lock);
	result = ggz_perms_is_set(player->perms, perm);
	pthread_rwlock_unlock(&player->lock);

	return result;
}

GGZPerm perms_default_anon[] = {
  GGZ_PERM_JOIN_TABLE, GGZ_PERM_LAUNCH_TABLE, GGZ_PERM_NO_STATS
};

/* Permissions for newly-registered user. */
GGZPerm perms_default[] = {
  GGZ_PERM_JOIN_TABLE, GGZ_PERM_LAUNCH_TABLE, GGZ_PERM_ROOMS_LOGIN
};

const int num_perms_default_anon = ARRAY_SIZE(perms_default_anon);
const int num_perms_default = ARRAY_SIZE(perms_default);
