/*
 * File: table.h
 * Author: Brent Hendricks
 * Project: GGZ Server
 * Date: 1/9/00
 * Desc: Functions for handling tables
 * $Id: table.h 5001 2002-10-22 20:23:12Z jdorje $
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

#include <pthread.h>
#include <signal.h>

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

	/* The thread # of the handling thread */
	pthread_t thread;

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

	/* Data for communicating with game server module */
	GGZdMod *ggzdmod;

	/*
	 * Seat assignments.  We track these separately
	 * from ggzdmod because we have to worry about locking
	 * them before we change them.
	 */
#ifdef UNLIMITED_SEATS
	int num_seats;
	GGZSeatType *seat_types;
	char (*seat_names)[MAX_USER_NAME_LEN + 1];
#else
	GGZSeatType seat_types[MAX_TABLE_SIZE];
	char seat_names[MAX_TABLE_SIZE][MAX_USER_NAME_LEN + 1];
#endif

	/* Spectator assignments.  An empty name ("") means there
	   is no spectator in the seat. */
#ifdef UNLIMITED_SPECTATORS
	int max_num_spectators;
	char (*spectators)[MAX_USER_NAME_LEN + 1];
#else
	char spectators[MAX_TABLE_SPECTATORS][MAX_USER_NAME_LEN + 1];
#endif

	/* Client-provided description of this table */
	char desc[MAX_GAME_DESC_LEN + 1];

	/* Name of player who launched (owns?) the table */
	char owner[MAX_USER_NAME_LEN + 1];

	/* Linked-list of private table-specific events */
	GGZEvent *events_head;
	GGZEvent *events_tail;
	
};


/* Create a new table object */
struct GGZTable* table_new(void);

/* Destroy a table object */
void table_free(GGZTable* table);

/* Launch a table */
GGZClientReqError table_launch(struct GGZTable *table, char* name);

/* Change table description of running table */
void table_set_desc(struct GGZTable *table, const char *desc);

/* Call to have a player join the table.  Only the table thread can call
   this function. */
void table_game_join(GGZTable *table, char *name, int num);

/* Call to have a player join the table.  Only the table thread can call
   this function. */
void table_game_leave(GGZTable *table, char *name, int num);

/* Call this function to change a non-player seat to a new type.  Only
   the table thread can call this function. */
void table_game_seatchange(GGZTable *table, GGZSeatType type, int num);

/* Call to have a player join the table as spectator.  Only the table
   thread can call this function. */
void table_game_spectator_join(GGZTable *table, char *name, int num);

/* Call to have a player leave the table as a spectator.  Only the table
   thread can call this function. */
void table_game_spectator_leave(GGZTable *table, char *name, int num);

/* Kill the table */
GGZClientReqError table_kill(int room, int index, char *name);

/* Search for tables */
int table_search(char* name, int room, int type, char global, 
		 struct GGZTable*** tables);

/* Find a player at a table */
int table_find_player(int room, int index, char *name);

/* Find a spectator */
int table_find_spectator(int room, int index, char *name);


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


/* Packaging for a table launch event */
typedef struct {
	GGZClientReqError status;
	int table_index;
} GGZLaunchEventData;


#define TABLE_EVENT_SIGNAL SIGUSR2
extern pthread_key_t table_key;
RETSIGTYPE table_handle_event_signal(int signal);

#endif
