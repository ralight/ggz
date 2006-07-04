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

#define PERMS_ALLOW	1
#define PERMS_DENY	0

/* If adding/editing perms, be sure to update STR_TABLE down below!!! */
#define PERMS_JOIN_TABLE	0x00000001	/* Can join a table */
#define PERMS_LAUNCH_TABLE	0x00000002	/* Can launch a new table */
#define PERMS_ROOMS_LOGIN	0x00000004	/* Can enter login-only rooms */
#define PERMS_ROOMS_ADMIN	0x00000008	/* Can enter admin-only rooms */
#define PERMS_CHAT_ANNOUNCE	0x00000010	/* Can make announcements */
#define PERMS_CHAT_BOT      0x00000020	/* Player is a known bot */
#define PERMS_NO_STATS      0x00000040	/* No stats for this player. */
#define PERMS_EDIT_TABLES   0x00000080	/* Can edit tables. */
#define PERMS_TABLE_PRIVMSG	0x00000100	/* Can send private messages whilst at a table */

/* Set all admin type bits */
#define PERMS_ADMIN_MASK ( \
	PERMS_ROOMS_ADMIN | \
	PERMS_CHAT_ANNOUNCE | \
	PERMS_EDIT_TABLES | \
	PERMS_TABLE_PRIVMSG)

/* Host type bits */
#define PERMS_HOST_MASK ( \
	PERMS_EDIT_TABLES | \
	PERMS_TABLE_PRIVMSG)

/* Default permissions for guest users. */
#define PERMS_DEFAULT_ANON ( \
	PERMS_JOIN_TABLE | \
	PERMS_LAUNCH_TABLE | \
	PERMS_NO_STATS)

/* Permissions for newly-registered user. */
#define PERMS_DEFAULT_SETTINGS ( \
	PERMS_JOIN_TABLE | \
	PERMS_LAUNCH_TABLE | \
	PERMS_ROOMS_LOGIN)

/* This function will acquire a read-lock on the player. */
bool perms_check(GGZPlayer *player, unsigned int perm);

/* These functions must already have a player read-lock. */
void perms_init(GGZPlayer *player, ggzdbPlayerEntry *db_pe);
bool perms_is_admin(GGZPlayer *player);
bool perms_is_host(GGZPlayer *player);
bool perms_is_bot(GGZPlayer *player);

#ifdef PERMS_DEFINE_STR_TABLE
char perms_str_table[32][21] = {
	"PERMS_JOIN_TABLE    ",		/* 00000001 */
	"PERMS_LAUNCH_TABLE  ",
	"PERMS_ROOMS_LOGIN   ",
	"PERMS_ROOMS_ADMIN   ",
	"PERMS_CHAT_ANNOUNCE ",		/* 00000010 */
	"PERMS_CHAT_BOT      ",
	"PERMS_NO_STATS      ",
	"PERMS_EDIT_TABLES   ",
	"PERMS_TABLE_PRIVMSG ",		/* 00000100 */
	"undefined 0x00000200",
	"undefined 0x00000400",
	"undefined 0x00000800",
	"undefined 0x00001000",		/* 00001000 */
	"undefined 0x00002000",
	"undefined 0x00004000",
	"undefined 0x00008000",
	"undefined 0x00010000",		/* 00010000 */
	"undefined 0x00020000",
	"undefined 0x00040000",
	"undefined 0x00080000",
	"undefined 0x00100000",		/* 00100000 */
	"undefined 0x00200000",
	"undefined 0x00400000",
	"undefined 0x00800000",
	"undefined 0x01000000",		/* 01000000 */
	"undefined 0x02000000",
	"undefined 0x04000000",
	"undefined 0x08000000",
	"undefined 0x10000000",		/* 10000000 */
	"undefined 0x20000000",
	"undefined 0x40000000",
	"undefined 0x80000000"
};
#endif

#endif
