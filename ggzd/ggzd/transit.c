/*
 * File: transit.c
 * Author: Brent Hendricks
 * Project: GGZ Server
 * Date: 3/26/00
 * Desc: Functions for handling table transits
 * $Id: transit.c 3513 2002-03-02 18:42:05Z bmh $
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

#include "ggzd.h"
#include "datatypes.h"
#include "protocols.h"
#include "err_func.h"
#include "transit.h"
#include "seats.h"
#include "event.h"
#include "net.h"

/* Server wide data structures*/
extern struct GameInfo game_types[MAX_GAME_TYPES];


/* Packaging for seat events */
struct GGZSeatEvent {
	struct GGZTableSeat seat;
	char caller[MAX_USER_NAME_LEN + 1];
};


/* Local functions for handling transits */
static GGZEventFuncReturn transit_player_event_callback(void* target,
                                                        int size,
                                                        void* data);
static GGZEventFuncReturn transit_seat_event_callback(void* target,
						      int size,
						      void* data);

static int transit_send_seat_to_game(GGZTable* table, struct GGZSeatEvent *event);
static int transit_find_seat(GGZTable *table, char *name);

int transit_seat_event(int room, int index, struct GGZTableSeat seat, char *caller)
{
	int status;
	struct GGZSeatEvent *data;

	if ( (data = malloc(sizeof(struct GGZSeatEvent))) == NULL)
		err_sys_exit("malloc failed in transit_pack");

	data->seat = seat;
	strcpy(data->caller, caller);
	
	status = event_table_enqueue(room, index, transit_seat_event_callback,
				     sizeof(data), data);
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
static GGZEventFuncReturn transit_seat_event_callback(void* target, 
						      int size, 
						      void* data)
{
	int status;
	char allow, action;
	GGZTable *table = target;
	struct GGZSeatEvent *event = data;
	struct GGZTableSeat *seat = &(event->seat);

	dbg_msg(GGZ_DBG_TABLE, 
		"%s requested seat change on table %d: Seat %d to %s (%s)", 
		event->caller, table->index, seat->index, 
		ggz_seattype_to_string(seat->type), seat->name);

	/* Figure out what kind of event this is */
	if (seat->type == GGZ_SEAT_PLAYER)
		action = GGZ_TRANSIT_JOIN;
	else if (table->seat_types[seat->index] == GGZ_SEAT_PLAYER)
		action = GGZ_TRANSIT_LEAVE;
	else
		action = GGZ_TRANSIT_SEAT;

	
	/* If this table is already in transit, defer event until later */
	if (table->transit) {
		dbg_msg(GGZ_DBG_TABLE, 
			"Deferring transit for table %d in room %d",
			table->index, table->room);
		return GGZ_EVENT_DEFER;
	}

	/* Make sure table is in an acceptable state */
	if (table->state == GGZ_TABLE_ERROR || table->state == GGZ_TABLE_DONE) {
		/* Notify player that transit failed */
		/* Don't care if this fails, we aren't transiting anyway */
		transit_player_event(event->caller, action, E_BAD_OPTIONS, 0, 0);
		return GGZ_EVENT_OK;
	}

	/* Check if seat assignment during gameplay is allowed */
	pthread_rwlock_rdlock(&game_types[table->type].lock);
	allow = game_types[table->type].allow_leave;
	pthread_rwlock_unlock(&game_types[table->type].lock);

	/* Try to find a seat if one isn't specified */
	if (seat->type == GGZ_SEAT_PLAYER && seat->index == GGZ_SEATNUM_ANY) {
		if ( (seat->index = transit_find_seat(table, event->caller)) < 0) {
			/* Don't care if this fails, we aren't transiting anyway */
			transit_player_event(event->caller, action, E_TABLE_FULL, 0, 0);
			return GGZ_EVENT_OK;
		}
	}
		
	/* If this isn't a join, the game type must explicityly allow assignments */
	if (table->state == GGZ_TABLE_PLAYING && !(seat->type == GGZ_SEAT_PLAYER || allow)) {
		transit_player_event(event->caller, action, E_LEAVE_FORBIDDEN, 0, 0);
		return GGZ_EVENT_OK;
	}
	
	status = transit_send_seat_to_game(table, event);

	/* If we sent it successfully, mark this table as in transit */
	if (status == 0) {
		table->transit = 1;
		status = GGZ_EVENT_OK;
	}
	/* Otherwise send an error message back to the player */
	else {
		transit_player_event(event->caller, action, E_SEAT_ASSIGN_FAIL, 0, 0);
		status = GGZ_EVENT_ERROR;
	}

	return status;
}
		

static GGZEventFuncReturn transit_player_event_callback(void* target,
                                                        int size,
                                                        void* data)
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
	case GGZ_TRANSIT_SEAT:
		if (net_send_update_result(player->net, (char)status) < 0)
			return GGZ_EVENT_ERROR;
		break;
	}

	return GGZ_EVENT_OK;
}


/*
 * transit_send_seat_to_game sets the seat on ggzdmod
 *
 * Returns 0 on success, -1 on failure (in which case the
 * player should be sent an failure notice).
 */
static int transit_send_seat_to_game(GGZTable* table, struct GGZSeatEvent *event)
{
	GGZSeat seat;
	int fd[2] = {-1, -1}, status = 0;

	dbg_msg(GGZ_DBG_TABLE, "Sending seat for table %d in room %d",
		table->index, table->room);
	
	/* If this is a 'join' create socket for communication with player thread */
	if (event->seat.type == GGZ_SEAT_PLAYER)
		if (socketpair(PF_UNIX, SOCK_STREAM, 0, fd) < 0)
			err_sys_exit("socketpair failed in transit_join");
	
	/* Save transit info so we have it when game module responds */
	table->transit_name = strdup(event->caller);
	table->transit_seat = event->seat.index;
	table->transit_fd = fd[1];

	/* Assemble seat structure */
	seat.num = event->seat.index;
	seat.type = event->seat.type;
	seat.fd = fd[0];
	/* Only dup name if it's not empty */
	if (event->seat.name[0] != '\0')
		seat.name = strdup(event->seat.name);
	else
		seat.name = NULL;

	
	if (ggzdmod_set_seat(table->ggzdmod, &seat) < 0) {
		dbg_msg(GGZ_DBG_TABLE, "ERROR: transit_send_seat_to_game: failed ggzdmod_set_seat() call.");
		status = -1;
	}

	if (seat.name)
		free(seat.name);
	
	/* Must close remote end of socketpair */
	if (fd[0] != -1)
		close(fd[0]);
	
	return status;
}


static int transit_find_seat(GGZTable *table, char *name)
{
	int i, num_seats = seats_num(table);
	
	/* First look for my (reserved) seat. */
	for (i = 0; i < num_seats; i++)
		if (seats_type(table, i) == GGZ_SEAT_RESERVED
		    && !strcasecmp(table->seat_names[i], name))
			return i;
			
	/* If that failed, look for first open seat. */
	for (i = 0; i < num_seats; i++)
		if (seats_type(table, i) == GGZ_SEAT_OPEN)
			return i;

	return -1;
}
