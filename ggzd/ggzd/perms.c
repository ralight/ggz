/*
 * File: perms.c
 * Author: Rich Gade
 * Project: GGZ Server
 * Date: 9/23/01
 * Desc: Functions for dealing with user permissions
 * $Id: perms.c 5923 2004-02-14 21:12:29Z jdorje $
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


bool perms_check(GGZPlayer *player, unsigned int perm)
{
	bool result;

	pthread_rwlock_rdlock(&player->lock);
	result = ((player->perms & perm) == perm);
	pthread_rwlock_unlock(&player->lock);

	return result;
}


bool perms_is_admin(GGZPlayer *player)
{
	return ((player->perms & PERMS_ADMIN_MASK) != 0);
}


bool perms_is_bot(GGZPlayer *player)
{
	return ((player->perms & PERMS_CHAT_BOT) != 0);
}
