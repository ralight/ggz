/*
 * File: table.c
 * Author: Justin Zaun
 * Project: GGZ Core Client Lib
 * Date: 6/5/00
 *
 * This fils contains functions for handling tables
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


#include "config.h"
#include "table.h"
#include "ggzcore.h"
#include "room.h"
#include "net.h"

#include <ggz.h>
#include <ggz_common.h>
#include <stdlib.h>
#include <string.h>

/* 
 * The Table structure is meant to be a node in a linked list of
 * the tables in the current room .
 */


/* Publicly exported functions */

GGZTable* ggzcore_table_new(void)
{
	return _ggzcore_table_new();
}


int ggzcore_table_init(GGZTable *table,
		       GGZGameType *gametype,
		       char *desc,
		       const unsigned int num_seats)
{
	if (table && gametype) {
		_ggzcore_table_init(table, gametype, desc, num_seats, 
				    GGZ_TABLE_CREATED, -1);
		return 0;
	}
	else
		return -1;
}


void ggzcore_table_free(GGZTable *table)
{
	if (table)
		_ggzcore_table_free(table);
}


int ggzcore_table_set_seat(GGZTable *table,
			   const unsigned int index,
			   GGZSeatType type,
			   char *name)
{
	GGZRoom *room;
	GGZServer *server;
	GGZNet *net;
	struct _GGZSeat seat = {index, type, name};

	ggz_debug("GGZCORE:TABLE", "User changing seats... on %p", table);

	/* Check table and seat number. */
	if (!table || index >= table->num_seats)
		return -1;
		
	/* GGZ clients should only ever set seats to OPEN, BOT, or RESERVED. */
	if (type != GGZ_SEAT_OPEN
	    && type != GGZ_SEAT_BOT
	    && type != GGZ_SEAT_RESERVED)
		return -1;
		
	/* If we set a seat to RESERVED, we need a reservation name. */
	if (type == GGZ_SEAT_RESERVED && !name)
		return -1;

	/* If the table is newly created and not involved in a game
           yet, users may change seats all they want */
	if (table->state == GGZ_TABLE_CREATED)
		_ggzcore_table_set_seat(table, &seat);
	else {
		/* Otherwise we have to do it through the server */
		if (!(room = _ggzcore_table_get_room(table)))
			return -1;

		if (!(server = _ggzcore_room_get_server(room)))
			return -1;

		if (!(net = _ggzcore_server_get_net(server)))
			return -1;
		
		return _ggzcore_net_send_table_seat_update(net, table, &seat);
	}


	return 0;
}


int ggzcore_table_set_desc(GGZTable *table, const char *desc)
{
	GGZRoom *room;
	GGZServer *server;
	GGZNet *net;

	ggz_debug("GGZCORE:TABLE", "User changing desc... on %p", table);

	/* Check table and seat number. */
	if (!table)
		return -1;
		
	/* If the table is newly created and not involved in a game
           yet, users may change desc all they want */
	if (table->state == GGZ_TABLE_CREATED)
		_ggzcore_table_set_desc(table, desc);
	else {
		/* Otherwise we have to do it through the server */
		if (!(room = _ggzcore_table_get_room(table)))
			return -1;

		if (!(server = _ggzcore_room_get_server(room)))
			return -1;

		if (!(net = _ggzcore_server_get_net(server)))
			return -1;
		
		return _ggzcore_net_send_table_desc_update(net, table, desc);
	}


	return 0;
}


int ggzcore_table_remove_player(GGZTable *table, char *name)
{
	int i, status = -1;
	struct _GGZSeat seat;

	if (table && name) {
		for (i = 0; i < table->num_seats; i++)
			if (table->seats[i].name != NULL 
			    && strcmp(table->seats[i].name, name) == 0) {
				seat.index = i;
				seat.type = GGZ_SEAT_OPEN;
				seat.name = NULL;
				_ggzcore_table_set_seat(table, &seat);
				status = 0;
			}
	}
	
	return status;
}


GGZGameType* ggzcore_table_get_type(GGZTable *table)
{
	if (table)
		return _ggzcore_table_get_type(table);
	else
		return NULL;
}


int ggzcore_table_get_id(GGZTable *table)
{
	if (table)
		return _ggzcore_table_get_id(table);
	else
		return -1;
}

     
char* ggzcore_table_get_desc(GGZTable *table)
{
	if (table)
		return _ggzcore_table_get_desc(table);
	else
		return NULL;
}


GGZTableState ggzcore_table_get_state(GGZTable *table)
{
	if (table)
		return _ggzcore_table_get_state(table);
	else
		return -1;
}


int ggzcore_table_get_num_seats(GGZTable *table)
{
	if (table)
		return _ggzcore_table_get_num_seats(table);
	else
		return -1;
}


int ggzcore_table_get_seat_count(GGZTable *table, GGZSeatType type)
{
	if (table)
		return _ggzcore_table_get_seat_count(table, type);
	else 
		return -1;
}

char* ggzcore_table_get_nth_player_name(GGZTable *table, const unsigned int num)
{
	if (table && num < table->num_seats)
		return _ggzcore_table_get_nth_player_name(table, num);
	else
		return NULL;
}


GGZSeatType ggzcore_table_get_nth_player_type(GGZTable *table, const unsigned int num)
{
	if (table && num < table->num_seats)
		return _ggzcore_table_get_nth_player_type(table, num);
	else
		return 0;
}
					           

/* 
 * Internal library functions (prototypes in table.h) 
 * NOTE:All of these functions assume valid inputs!
 */

struct _GGZTable* _ggzcore_table_new(void)
{
	struct _GGZTable *table;

	table = ggz_malloc(sizeof(struct _GGZTable));

	/* FIXME: anything we should mark invalid? */
	return table;
}


void _ggzcore_table_init(struct _GGZTable *table, 
			 struct _GGZGameType *gametype,
			 const char *desc,
			 const unsigned int num_seats,
			 const GGZTableState state,
			 const int id)
{
	int i;
	
	table->room = NULL;
	table->gametype = gametype;
	table->id = id;
	table->state = state;
	table->desc = ggz_strdup(desc);

	table->num_seats = num_seats;
	ggz_debug("GGZCORE:TABLE", "Allocating %d seats", num_seats);
	if (num_seats)
		table->seats = ggz_malloc(num_seats * sizeof(struct _GGZSeat));

	for (i = 0; i < num_seats; i++) {
		table->seats[i].index = i;
		table->seats[i].type = GGZ_SEAT_OPEN;
		table->seats[i].name =  NULL;
	}
}


void _ggzcore_table_free(struct _GGZTable *table)
{
	int i;

	if (table->desc)
		ggz_free(table->desc);

	if (table->seats) {
		for (i = 0; i < table->num_seats; i++)
			if (table->seats[i].name)
				ggz_free(table->seats[i].name);
		ggz_free(table->seats);
	}
	
	ggz_free(table);

}

void _ggzcore_table_set_room(struct _GGZTable *table, struct _GGZRoom *room)
{
	table->room = room;
}


void _ggzcore_table_set_id(struct _GGZTable *table, const int id)
{
	table->id = id;
}


void _ggzcore_table_set_state(struct _GGZTable *table, const GGZTableState state)
{
	ggz_debug("GGZCORE:TABLE", "Table %d new state %d", table->id, state);
	table->state = state;
	
	/* If we're in a room, notify it of a table event */
	if (table->room)
		_ggzcore_room_table_event(table->room, GGZ_TABLE_UPDATE, NULL);
}


void _ggzcore_table_set_desc(struct _GGZTable *table, const char *desc)
{
	ggz_debug("GGZCORE:TABLE", "Table %d new desc %s", table->id, desc);

	/* Free previous description */
	if (table->desc)
		ggz_free(table->desc);
	table->desc = ggz_strdup(desc);

	/* If we're in a room, notify it of a table event */
	if (table->room) 
		_ggzcore_room_table_event(table->room, GGZ_TABLE_UPDATE, NULL);
}


void _ggzcore_table_set_seat(struct _GGZTable *table, struct _GGZSeat *seat)
{
	/* Set up the new seat. */
	struct _GGZSeat oldseat;

	/* Sanity check */
	if (seat->index >= table->num_seats) {
		ggz_debug("GGZCORE:TABLE", "Attempt to set seat %d on table with only %d seats", seat->index, table->num_seats);
	}

	oldseat = table->seats[seat->index];
	table->seats[seat->index].index = seat->index;
	table->seats[seat->index].type = seat->type;
	table->seats[seat->index].name = ggz_strdup(seat->name);
	
	/* Check for specific seat changes */
	if (seat->type == GGZ_SEAT_PLAYER) {
		ggz_debug("GGZCORE:TABLE", "%s joining seat %d at table %d",
			      seat->name, seat->index, table->id);
		if (table->room)
			_ggzcore_room_player_set_table(table->room, seat->name, table->id);
	}
	else if (oldseat.type == GGZ_SEAT_PLAYER) {
		ggz_debug("GGZCORE:TABLE", "%s leaving seat %d at table %d",
			      oldseat.name, oldseat.index, table->id);
		if (table->room)
			_ggzcore_room_player_set_table(table->room, oldseat.name, -1);
	}
	else {
		if (table->room) 
			_ggzcore_room_table_event(table->room, GGZ_TABLE_UPDATE, NULL);
	}
	
	/* Get rid of the old seat. */
	if (oldseat.name)
		ggz_free(oldseat.name);
}


struct _GGZRoom* _ggzcore_table_get_room(struct _GGZTable *table)
{
	return table->room;
}


int _ggzcore_table_get_id(struct _GGZTable *table)
{
	return table->id;
}


struct _GGZGameType* _ggzcore_table_get_type(struct _GGZTable *table)
{
	return table->gametype;
}


char* _ggzcore_table_get_desc(struct _GGZTable *table)
{
	return table->desc;
}


GGZTableState _ggzcore_table_get_state(struct _GGZTable *table)
{
	return table->state;
}


int _ggzcore_table_get_num_seats(struct _GGZTable *table)
{
	return table->num_seats;
}


int _ggzcore_table_get_seat_count(GGZTable *table, GGZSeatType type)
{
	int i, count = 0;

	for (i = 0; i < table->num_seats; i++)
		if (table->seats[i].type == type)
			count++;
	
	return count;
}


struct _GGZSeat* _ggzcore_table_get_nth_seat(struct _GGZTable *table, const unsigned int num)
{
	return &table->seats[num];
}


char* _ggzcore_table_get_nth_player_name(struct _GGZTable *table, const unsigned int num)
{
	return table->seats[num].name;
}


GGZSeatType _ggzcore_table_get_nth_player_type(struct _GGZTable *table, const unsigned int num)
{
	return table->seats[num].type;
}


int _ggzcore_table_compare(void* p, void* q)
{
	if(((struct _GGZTable*)p)->id == ((struct _GGZTable*)q)->id)
		return 0;
	if(((struct _GGZTable*)p)->id > ((struct _GGZTable*)q)->id)
		return 1;
	if(((struct _GGZTable*)p)->id < ((struct _GGZTable*)q)->id)
		return -1;

	return 0;
}


void* _ggzcore_table_create(void* p)
{
	struct _GGZTable *new, *src = p;

	new = _ggzcore_table_new();
	_ggzcore_table_init(new, src->gametype, src->desc, src->num_seats,
			    src->state, src->id);

	/* FIXME: copy players as well */
	
	return (void*)new;
}


void  _ggzcore_table_destroy(void* p)
{
	_ggzcore_table_free(p);
}



/* Static functions internal to this file */

