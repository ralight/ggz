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

#include "ggzdb.h"

#define PERMS_ALLOW	1
#define PERMS_DENY	0

#define PERMS_JOIN_TABLE	0x00000001	/* Can join a table */
#define PERMS_LAUNCH_TABLE	0x00000002	/* Can launch a new table */
#define PERMS_ROOMS_LOGIN	0x00000004	/* Can enter login-only rooms */
#define PERMS_ROOMS_ADMIN	0x00000008	/* Can enter admin-only rooms */

#define PERMS_ADMIN_MASK	0x00000008	/* Set all admin type bits */

#define PERMS_DEFAULT_ANON ( PERMS_JOIN_TABLE   |  \
			     PERMS_LAUNCH_TABLE )

#define PERMS_DEFAULT_SETTINGS ( PERMS_JOIN_TABLE   |  \
				 PERMS_LAUNCH_TABLE |  \
				 PERMS_ROOMS_LOGIN  )

extern int perms_check(GGZPlayer *player, unsigned int perm);
extern void perms_init(GGZPlayer *player, ggzdbPlayerEntry *db_pe);
extern int perms_is_admin(GGZPlayer *player);
