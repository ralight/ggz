/*
 * File: perms.c
 * Author: Rich Gade
 * Project: GGZ Server
 * Date: 9/23/01
 * Desc: Functions for dealing with user permissions
 * $Id: perms.c 4965 2002-10-20 09:05:32Z jdorje $
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

#include "ggzd.h"
#include "ggzdb.h"
#include "perms.h"
#include "players.h"

void perms_init(GGZPlayer *player, ggzdbPlayerEntry *pe)
{
	player->perms = pe->perms;
}


int perms_check(GGZPlayer *player, unsigned int perm)
{
	int rc=0;

	pthread_rwlock_rdlock(&player->lock);
	if((player->perms & perm) == perm)
		rc = 1;
	pthread_rwlock_unlock(&player->lock);

	return rc;
}


int perms_is_admin(GGZPlayer *player)
{
	if((player->perms & PERMS_ADMIN_MASK) != 0)
		return 1;
	return 0;
}
