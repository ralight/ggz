/*
 * File: transit.c
 * Author: Brent Hendricks
 * Project: GGZ Server
 * Date: 3/26/00
 * Desc: Functions for handling table transits
 * $Id: transit.c 3185 2002-01-24 10:59:56Z jdorje $
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

#include <config.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#include "ggzdmod.h"

#include <ggzd.h>
#include <datatypes.h>
#include <protocols.h>
#include <err_func.h>
#include <transit.h>
#include <seats.h>
#include <event.h>
#include <net.h>

/* Server wide data structures*/
extern struct GameInfo game_types[MAX_GAME_TYPES];


/* Local functions for handling transits */
static int transit_player_event_callback(void* target, int size, void* data);
static int transit_table_event_callback(void* target, int size, void* data);
static int transit_send_join_to_game(GGZTable* table, char* name);
static int transit_send_leave_to_game(GGZTable* table, char* name);


int transit_table_event(int room, int index, char opcode, char* name)
{
	int size, status;
	char* current;
	void* data;
	
	size = sizeof(char) + strlen(name) + 1;

	if ( (data = malloc(size)) == NULL)
		err_sys_exit("malloc failed in transit_pack");
	
	current = (char*)data;
	
	*(char*)current = opcode;
	current += sizeof(char);
	
	strcpy(current, name);
	current += (strlen(name) + 1);

	status = event_table_enqueue(room, index, transit_table_event_callback,
				     size, data);
	return status;
}

    
int transit_player_event(char* name, char opcode, int status, int index, 
			 int fd)
{
	int size;
	char* current;
	void* data;

	size = sizeof(char) + sizeof(int);
	
	/* We pass back the table index and fd if a join was successful */
	if (opcode == GGZ_TRANSIT_JOIN && status == 0)
		size += (2 * sizeof(int));

	if ( (data = malloc(size)) == NULL)
		err_sys_exit("malloc failed in transit_player_event");
	
	/* Start packing the data */
	current = (char*)data;
	
	*(char*)current = opcode;
	current += sizeof(char);
	
	*(int*)current = status;
	current += sizeof(int);

	if (opcode == GGZ_TRANSIT_JOIN && status == 0) {
		*(int*)current = index;
		current += sizeof(int);

		*(int*)current = fd;
		current += sizeof(int);
	}

	return event_player_enqueue(name, transit_player_event_callback, 
				    size, data);
}


/* Executed by table hander thread */
static int transit_table_event_callback(void* target, int size, void* data)
{
	int status;
	char opcode;
	char *current, *name;
	GGZTable* table = (GGZTable*)target;

	/* Unpack event data */
	current = (char*)data;
	opcode = *(char*)current;
	current += sizeof(char);
	name = (char*)(current);
	
	dbg_msg(GGZ_DBG_TABLE, "Table %d in room %d transit callback for %s",
		table->index, table->room, name);
	
	/* If this table is already in transit, defer event until later */
	if (table->transit) {
		dbg_msg(GGZ_DBG_TABLE, 
			"Deferring transit for table %d in room %d",
			table->index, table->room);
		return GGZ_EVENT_DEFER;
	}

	/* First check to see if table is in process of being removed */
	switch (table->state) {

	case GGZ_TABLE_ERROR:
	case GGZ_TABLE_DONE:
		/* Don't care if this fails, we aren't transiting anyway */
		transit_player_event(name, opcode, E_BAD_OPTIONS, 0, 0);
		return GGZ_EVENT_OK;

	case GGZ_TABLE_PLAYING:
		/* Don't care about joins */
		if (opcode != GGZ_TRANSIT_LEAVE)
			break;
		
		/* Only allow leave during gameplay if game type supports it */
		pthread_rwlock_rdlock(&game_types[table->type].lock);
		if (!game_types[table->type].allow_leave) {
			pthread_rwlock_unlock(&game_types[table->type].lock);
			transit_player_event(name, opcode, E_LEAVE_FORBIDDEN, 
					     0, 0);
			return GGZ_EVENT_OK;
		}
		pthread_rwlock_unlock(&game_types[table->type].lock);
		break;
	}

	if (opcode == GGZ_TRANSIT_JOIN && !seats_open(table)) {
		/* Don't care if this fails, we aren't transiting anyway */
		transit_player_event(name, opcode, E_TABLE_FULL, 0, 0);
		return GGZ_EVENT_OK;
	}
	
	switch (opcode) {
	case GGZ_TRANSIT_JOIN:
		status = transit_send_join_to_game(table, name);
		break;
	case GGZ_TRANSIT_LEAVE:
		status = transit_send_leave_to_game(table, name);
		break;
	default:
		status = -1;
	}
	
	/* If we sent it successfully, mark this table as in transit */
	if (status == 0) {
		table->transit = 1;
		status = GGZ_EVENT_OK;
	}
	/* Otherwise send an error message back to the player */
	else {
		transit_player_event(name, opcode, E_JOIN_FAIL, 0, 0);
		status = GGZ_EVENT_ERROR;
	}

	return status;
}
		

static int transit_player_event_callback(void* target, int size, void* data)
{
	int status, index = -1, fd = 0;
	char opcode;
	char *current;
	GGZPlayer* player = (GGZPlayer*)target;

	/* Unpack event data */
	current = (char*)data;

	opcode = *(char*)current;
	current += sizeof(char);

	status = *(int*)(current);
	current += sizeof(int);

	if (opcode == GGZ_TRANSIT_JOIN && status == 0) {
		index = *(int*)(current);
		current += sizeof(int);

		fd = *(int*)(current);
		current += sizeof(int);
	}
	
	dbg_msg(GGZ_DBG_TABLE, "%s transit result: %d", player->name, status);
	
	switch (opcode) {
	case GGZ_TRANSIT_LEAVE:
		pthread_rwlock_wrlock(&player->lock);
		player->transit = 0;
		if (status == 0)
			player->table = -1;
		pthread_rwlock_unlock(&player->lock);
		
		if (net_send_table_leave(player->net, (char)status) < 0)
			return GGZ_EVENT_ERROR;
		break;

	case GGZ_TRANSIT_JOIN:
		pthread_rwlock_wrlock(&player->lock);
		player->transit = 0;
		if (status == 0) {
			player->game_fd = fd;
			player->table = index;
		}
		pthread_rwlock_unlock(&player->lock);		

		if (net_send_table_join(player->net, (char)status) < 0)
			return GGZ_EVENT_ERROR;
		break;
	}

	return GGZ_EVENT_OK;
}


/*
 * transit_join sends REQ_GAME_JOIN to table
 */
static int transit_send_join_to_game(GGZTable* table, char* name)
{
	GGZSeat seat;
	int seats, i, fd[2];

	dbg_msg(GGZ_DBG_TABLE, "Sending join for table %d in room %d",
		table->index, table->room);
		
	/* Find my seat or unoccupied one */
	/* FIXME: look for reserved seat */
	seats = seats_num(table);
	for (i = 0; i < seats; i++)
		if (seats_type(table, i) == GGZ_SEAT_OPEN)
			break;
	
	/* Ack! Fatal error...this should never happen */
	if (i == seats)
		return -1;
	
	/* Create socket for communication with player thread */
	if (socketpair(PF_UNIX, SOCK_STREAM, 0, fd) < 0)
		err_sys_exit("socketpair failed in transit_join");
	
	/* Save transit info so we have it when game module responds */
	table->transit_name = strdup(name);
	table->transit_seat = i;
	table->transit_fd = fd[1];

	/* Send info to table */
	seat.num = i;
	seat.name = name;
	seat.fd = fd[0];
	seat.type = GGZ_SEAT_PLAYER;
	if (ggzdmod_set_seat(table->ggzdmod, &seat) < 0)
		return -1;
	
	/* Must close remote end of socketpair */
	close(fd[0]);
	
	return 0;
}

	
/*
 * transit_join sends REQ_GAME_LEAVE to table
 */
static int transit_send_leave_to_game(GGZTable* table, char* name)
{
	GGZSeat seat;
	int seats, i;
	
	dbg_msg(GGZ_DBG_TABLE, "Sending leave for table %d in room %d", 
		table->index, table->room);
	
	/* Find seat player is in */
	seats  = seats_num(table);
	for (i = 0; i < seats; i++)
		if (table->seat_types[i] == GGZ_SEAT_PLAYER
		    && strcmp(table->seat_names[i], name) == 0)
			break;

	/* Ack! Fatal error...this should never happen */
	if (i == seats)
		return -1;

	/* Save transit info so we have it when game module responds */
	table->transit_name = strdup(name);
	table->transit_seat = i;
	
	/* Send message to table */
	seat.num = i;
	seat.name = NULL;
	seat.fd = -1;
	seat.type = GGZ_SEAT_OPEN;
	ggzdmod_set_seat(table->ggzdmod, &seat);

	return 0;
}


