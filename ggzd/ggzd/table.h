/*
 * File: table.h
 * Author: Brent Hendricks
 * Project: GGZ Server
 * Date: 1/9/00
 * Desc: Functions for handling tables
 * $Id: table.h 4161 2002-05-05 18:43:52Z bmh $
 *
 * Copyright (C) 1999 Brent Hendricks.
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

#ifndef _GGZ_TABLE_H
#define _GGZ_TABLE_H

#include <config.h>

#include <pthread.h>

#include "ggzdmod.h"
#include "ggzd.h"

/*
 * The GGZTable structure contains information about a single game
 * table 
 *
 * Note: Most of its members are arrays rather than pointers.  This is
 * so that when we copy the struct it copies the arrays too.  Then we
 * can use the copy in another thread without worrying about the
 * original thread freeing the memory
 */
struct GGZTable {

	/* Individual mutex lock */
	pthread_rwlock_t lock;

	/* Type of game being played on table */
	int type;

	/* Room in which game exists */
	int room;

	/* Index of table in room */
	int index;

	/* State of game: One of GGZ_TABLE_XXXX */
	GGZTableState state;

	/* Transit status flag */
	char transit;

	/* 
	 * Variables to hold player name and data during transit to
	 * and from table 
	 */
	char* transit_name;
	int transit_seat;

	/* Data for communicating with game server module */
	GGZdMod *ggzdmod;

	/*
	 * Seat assignments.  We track these separately
	 * from ggzdmod because we have to worry about locking
	 * them before we change them.
	 */
	GGZSeatType seat_types[MAX_TABLE_SIZE];
	char seat_names[MAX_TABLE_SIZE][MAX_USER_NAME_LEN + 1];

	/* Client-provided description of this table */
	char desc[MAX_GAME_DESC_LEN + 1];

	/* Name of player who launched (owns?) the table */
	char owner[MAX_USER_NAME_LEN + 1];

	/* Linked-list of private table-specific events */
	void* events_head;
	void* events_tail;
	
};


/* Create a new table object */
struct GGZTable* table_new(void);

/* Launch a table */
int table_launch(struct GGZTable *table, char* name);

/* Change table description of running table */
void table_set_desc(struct GGZTable *table, char *desc);

/* Kill the table */
int table_kill(int room, int index, char *name);

/* Search for tables */
int table_search(char* name, int room, int type, char global, 
		 struct GGZTable** tables);

/* Find a player at a table */
int table_find_player(int room, int index, char *name);


/*
 * table_lookup() looks up a table inside a room
 *
 * Receives:
 * int room        : index of room in which table resides
 * int index       : index of table in room to lookup
 *
 * Returns:
 * GGZTable *table : pointer to desired table
 *
 * Note: table is returned with write lock acquired
 */
struct GGZTable* table_lookup(int room, int index);

#endif
