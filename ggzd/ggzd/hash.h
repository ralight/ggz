/*
 * File: hash.h
 * Author: Rich Gade
 * Project: GGZ Server
 * Date: 06/08/2000
 * Desc: Functions for handling the player name hash tables
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

/* Exported functions */
extern int hash_player_add(char *name, int p_index);
extern int hash_player_lookup(char *name);
extern void hash_player_delete(char *name);

/* Tuning parameter - This should be a prime */
#define HASH_NUM_LISTS	31

/* How often to print hash debugging stats */
/* Requires DEBUG and GGZ_DBG_LISTS */
#define HASH_DEBUG_FREQ	1
