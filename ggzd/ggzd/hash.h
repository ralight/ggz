/*
 * File: hash.h
 * Author: Rich Gade
 * Project: GGZ Server
 * Date: 06/08/2000
 * Desc: Functions for handling the player name hash tables
 * $Id: hash.h 5928 2004-02-15 02:43:16Z jdorje $
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


#include "players.h"

/* Exported functions */
void hash_initialize(void);
int hash_player_add(const char *name, GGZPlayer* player);

/* Return a pointer to the player with name "name".  Note: Returns
   with the player's write-lock acquired */
GGZPlayer* hash_player_lookup(const char *name);
void hash_player_delete(const char *name);

/* Tuning parameter - This should be a prime */
#define HASH_NUM_LISTS	31

/* How often to print hash debugging stats */
/* Requires DEBUG and GGZ_DBG_LISTS */
#define HASH_DEBUG_FREQ	1
