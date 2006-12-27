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

#include <ggz_common.h>

#include "ggzdb.h"
#include "players.h"

/* Permissions for newly-registered and anonymous users. */
extern GGZPerm perms_default_anon[], perms_default[];
extern const int num_perms_default_anon, num_perms_default;

void perms_init_from_db(GGZPermset *perms, ggzdbPlayerEntry *db_pe);
bool perms_set_to_db(GGZPermset perms, ggzdbPlayerEntry *pe);

/* Acquires a read-lock and checks for the permission. */
bool perms_check(GGZPlayer *player, GGZPerm perm);

#endif
