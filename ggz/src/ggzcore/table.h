/*
 * File: player.h
 * Author: Brent Hendricks
 * Project: GGZ Core Client Lib
 * Date: 6/5/00
 *
 * This fils contains functions for handling players
 *
 * Copyright (C) 1998 Brent Hendricks.
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


#ifndef __TABLE_H_
#define __TABLE_H_

#include <ggz_common.h>

#include "gametype.h"
#include "ggzcore.h"

struct _GGZSeat {
	/* Seat index */
	int index;

	/* Type of player in seat.  Used for regular seats only;
	   spectator seats just ignore it.*/
	GGZSeatType type;

	/* Player's name; or NULL if none.  An empty spectator seat
	   will have no name. */
	const char *name;
};


/* Table Information */
struct _GGZTable {
 
	/* Pointer to room this table resides in */
	struct _GGZRoom *room;

        /* Server ID of table */
        int id;
 
        /* Game Type */
	struct _GGZGameType *gametype;

	/* Table description */
	const char * desc;        

        /* Table state */
        GGZTableState state;

        /* Total seats */
        unsigned int num_seats;

	/* Seats */
	struct _GGZSeat *seats;

	/* Total spectator seats */
	unsigned int num_spectator_seats;

	/* Spectator seats - "type" is unused; player name is
	   NULL for empty seat. */
	struct _GGZSeat *spectator_seats;
};


struct _GGZTable* _ggzcore_table_new(void);

void _ggzcore_table_init(struct _GGZTable *table, 
			 struct _GGZGameType *gametype,
			 const char *desc,
			 const unsigned int num_seats,
			 const GGZTableState state,
			 const int id);

void _ggzcore_table_free(struct _GGZTable *table);

void _ggzcore_table_set_room(struct _GGZTable *table, struct _GGZRoom *room);
void _ggzcore_table_set_id(struct _GGZTable *table, const int id);
void _ggzcore_table_set_state(struct _GGZTable *table, const GGZTableState state);
void _ggzcore_table_set_desc(struct _GGZTable *table, const char *desc);

/** @brief Change a seat value.
 *
 *  This changes the seat status for any seat at the table.  It is
 *  called by both front-end and back-end functions to do the
 *  actual work of changing the seat.
 */
void _ggzcore_table_set_seat(struct _GGZTable *table, struct _GGZSeat *seat);


/** @brief Change a spectator seat value.
 *
 *  This changes the seat status for any spectator seat at the table.
 */
void _ggzcore_table_set_spectator_seat(struct _GGZTable *table,
				       struct _GGZSeat *seat);

struct _GGZRoom*      _ggzcore_table_get_room(struct _GGZTable *table);
int                   _ggzcore_table_get_id(struct _GGZTable *table);
struct _GGZGameType*  _ggzcore_table_get_type(struct _GGZTable *table);
const char * _ggzcore_table_get_desc(struct _GGZTable *table);
GGZTableState         _ggzcore_table_get_state(struct _GGZTable *table);
int _ggzcore_table_get_num_seats(GGZTable *table);
int _ggzcore_table_get_num_spectator_seats(GGZTable *table);
int                   _ggzcore_table_get_seat_count(struct _GGZTable *table, GGZSeatType type);

struct _GGZSeat* _ggzcore_table_get_nth_seat(GGZTable *table,
					     const unsigned int num);
struct _GGZSeat* _ggzcore_table_get_nth_spectator_seat(GGZTable *table,
						       const unsigned int num);
const char * _ggzcore_table_get_nth_player_name(struct _GGZTable *table,
						const unsigned int num);
GGZSeatType _ggzcore_table_get_nth_player_type(struct _GGZTable *table, const unsigned int num);
const char *_ggzcore_table_get_nth_spectator_name(GGZTable *table,
						  const unsigned int num);


/* Utility functions used by _ggzcore_list */
int   _ggzcore_table_compare(void* p, void* q);
void* _ggzcore_table_create(void* p);
void  _ggzcore_table_destroy(void* p);

#endif /* __TABLE_H_ */

