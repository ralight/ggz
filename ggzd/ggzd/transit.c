/*
 * File: transit.c
 * Author: Brent Hendricks
 * Project: GGZ Server
 * Date: 3/26/00
 * Desc: Functions for handling table transits
 * $Id: transit.c 4537 2002-09-13 04:34:38Z jdorje $
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
#include "client.h"

/* Server wide data structures*/
extern struct GameInfo game_types[MAX_GAME_TYPES];


/* Packaging for seat events */
typedef struct{
	struct GGZTableSeat seat;
	char caller[MAX_USER_NAME_LEN + 1];
} GGZSeatEventData;
typedef struct {
	GGZTransitType opcode;
	int status;
	int table_index; /* Only for certain operations */
} GGZPlayerTransitEventData;

/* Packaging for spectator events */
struct GGZSpectatorEvent {
	struct GGZTableSpectator spectator;
	char caller[MAX_USER_NAME_LEN + 1];
};


/* Local functions for handling transits */
static GGZEventFuncReturn transit_player_event_callback(void* target,
                                                        size_t size,
                                                        void* data);
static GGZEventFuncReturn transit_seat_event_callback(void* target,
						      size_t size,
						      void* data);
static GGZEventFuncReturn transit_spectator_event_callback(void* target,
							   size_t size,
							   void* data);

static int transit_send_seat_to_game(GGZTable* table, GGZSeatEventData *event);
static int transit_find_seat(GGZTable *table, char *name);

static int transit_send_spectator_to_game(GGZTable* table, struct GGZSpectatorEvent *event);
static int transit_find_spectator(GGZTable *table, char *name);

int transit_seat_event(int room, int index, struct GGZTableSeat seat, char *caller)
{
	int status;
	GGZSeatEventData *data = ggz_malloc(sizeof(*data));

	data->seat = seat;
	strcpy(data->caller, caller);
	
	status = event_table_enqueue(room, index, transit_seat_event_callback,
				     sizeof(*data), data, NULL);
	return status;
}

int transit_spectator_event(int room, int index, struct GGZTableSpectator spectator, char *caller)
{
	int status;
	struct GGZSpectatorEvent *data;

	data = ggz_malloc(sizeof(struct GGZSpectatorEvent));

	data->spectator = spectator;
	strcpy(data->caller, caller);
	
	status = event_table_enqueue(room, index,
				     transit_spectator_event_callback,
				     sizeof(*data), data, NULL);
	return status;
}

int transit_player_event(char* name, GGZTransitType opcode,
			 int status, int index)
{
	GGZPlayerTransitEventData* data = ggz_malloc(sizeof(*data));

	data->opcode = opcode;
	data->status = status;
	if (status == 0
	    && (opcode == GGZ_TRANSIT_JOIN
		|| opcode == GGZ_TRANSIT_JOIN_SPECTATOR))
		data->table_index = index;
	else
		data->table_index = -1;
	
	return event_player_enqueue(name, transit_player_event_callback, 
				    sizeof(*data), data, NULL);
}


/* Executed by table hander thread */
static GGZEventFuncReturn transit_seat_event_callback(void* target,
						      size_t size, 
						      void* data)
{
	int status;
	GGZTransitType action;
	GGZTable *table = target;
	GGZSeatEventData *event = data;
	struct GGZTableSeat *seat = &(event->seat);

	dbg_msg(GGZ_DBG_TABLE, 
		"%s requested seat change on table %d: Seat %d to %s (%s) with fd %d", 
		event->caller, table->index, seat->index, 
		ggz_seattype_to_string(seat->type), seat->name,
		seat->fd);

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
		transit_player_event(event->caller, action, E_BAD_OPTIONS, 0);
		return GGZ_EVENT_OK;
	}


	/* Try to find a seat if one isn't specified */
	if (seat->type == GGZ_SEAT_PLAYER && seat->index == GGZ_SEATNUM_ANY) {
		if ( (seat->index = transit_find_seat(table, event->caller)) < 0) {
			/* Don't care if this fails, we aren't transiting anyway */
			transit_player_event(event->caller, action, E_TABLE_FULL, 0);
			return GGZ_EVENT_OK;
		}
	}
		
	status = transit_send_seat_to_game(table, event);

	/* If we sent it successfully, mark this table as in transit */
	if (status == 0) {
		table->transit = 1;
	}
	/* Otherwise send an error message back to the player */
	else {
		transit_player_event(event->caller, action, E_SEAT_ASSIGN_FAIL, 0);
		return GGZ_EVENT_ERROR;
	}

	return GGZ_EVENT_OK;
}

/* Executed by table hander thread */
static GGZEventFuncReturn transit_spectator_event_callback(void* target,
							   size_t size,
							   void* data)
{
	int status;
	GGZTransitType action;
	GGZTable *table = target;
	struct GGZSpectatorEvent *event = data;
	struct GGZTableSpectator *spectator = &(event->spectator);

	dbg_msg(GGZ_DBG_TABLE, 
		"%s requested spectator change on table %d: Spectator %d to 'spectator' (%s) with fd %d", 
		event->caller, table->index, spectator->index, 
		spectator->name, spectator->fd);

	action = GGZ_TRANSIT_JOIN_SPECTATOR;
	if (spectator->index >= 0
	    && spectator->index < spectator_seats_num(table)
	    && table->spectators[spectator->index][0])
		action = GGZ_TRANSIT_LEAVE_SPECTATOR;


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
		transit_player_event(event->caller, action, E_BAD_OPTIONS, 0);
		return GGZ_EVENT_OK;
	}


	/* Try to find a seat if one isn't specified */
	if (action == GGZ_TRANSIT_JOIN_SPECTATOR && spectator->index == GGZ_SEATNUM_ANY) {
		if ( (spectator->index = transit_find_spectator(table, event->caller)) < 0) {
			/* Don't care if this fails, we aren't transiting
			   anyway */
			/* FIXME: this shouldn't be E_TABLE_FULL, since the
			   likely error is that the game doesn't support
			   spectators at all. */
			transit_player_event(event->caller, action,
					     E_TABLE_FULL, 0);
			return GGZ_EVENT_OK;
		}
	}
		
	status = transit_send_spectator_to_game(table, event);

	/* If we sent it successfully, mark this table as in transit */
	if (status == 0) {
		table->transit = 1;
	}
	/* Otherwise send an error message back to the player */
	else {
		transit_player_event(event->caller, action, E_SEAT_ASSIGN_FAIL, 0);
		return GGZ_EVENT_ERROR;
	}

	return GGZ_EVENT_OK;
}

static GGZEventFuncReturn transit_player_event_callback(void* target,
                                                        size_t size,
                                                        void* event_data)
{
	GGZPlayer* player = target;
	GGZPlayerTransitEventData *data = event_data;
	
	dbg_msg(GGZ_DBG_TABLE, "%s transit result: %d",
		player->name, data->status);
	
	switch (data->opcode) {
	case GGZ_TRANSIT_LEAVE:
	case GGZ_TRANSIT_LEAVE_SPECTATOR:
		pthread_rwlock_wrlock(&player->lock);
		player->transit = 0;
		if (data->status == 0)
			player->table = -1;
		pthread_rwlock_unlock(&player->lock);
		
		if (net_send_table_leave(player->client->net,
					 (char)data->status) < 0)
			return GGZ_EVENT_ERROR;
		break;

	case GGZ_TRANSIT_JOIN:
	case GGZ_TRANSIT_JOIN_SPECTATOR:
		pthread_rwlock_wrlock(&player->lock);
		player->transit = 0;
		if (data->status == 0) {
			player->table = data->table_index;
		}
		pthread_rwlock_unlock(&player->lock);

		if (net_send_table_join(player->client->net,
					(char)data->status) < 0)
			return GGZ_EVENT_ERROR;
		break;

	case GGZ_TRANSIT_SEAT:
		if (net_send_update_result(player->client->net,
					   (char)data->status) < 0)
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
static int transit_send_seat_to_game(GGZTable* table, GGZSeatEventData *event)
{
	GGZSeat seat;
	int status = 0;

	dbg_msg(GGZ_DBG_TABLE, "Sending seat for table %d in room %d",
		table->index, table->room);
	
	/* Save transit info so we have it when game module responds */
	table->transit_name = ggz_strdup(event->caller);
	table->transit_seat = event->seat.index;

	/* Assemble seat structure */
	seat.num = event->seat.index;
	seat.type = event->seat.type;
	seat.fd = event->seat.fd;

	/* Only send name if it's not empty */
	if (event->seat.name[0] != '\0')
		seat.name = event->seat.name;
	else
		seat.name = NULL;

	
	if (ggzdmod_set_seat(table->ggzdmod, &seat) < 0) {
		dbg_msg(GGZ_DBG_TABLE, "ERROR: transit_send_seat_to_game: failed ggzdmod_set_seat() call.");
		status = -1;
	}

	/* Must close remote end of socketpair */
	if (event->seat.type == GGZ_SEAT_PLAYER)
		close(seat.fd);
	
	return status;
}

/*
 * transit_send_spectator_to_game sets the spectator on ggzdmod
 *
 * Returns 0 on success, -1 on failure (in which case the
 * player should be sent an failure notice).
 */
static int transit_send_spectator_to_game(GGZTable* table, struct GGZSpectatorEvent *event)
{
	GGZSpectator spectator;
	int status = 0, result;

	dbg_msg(GGZ_DBG_TABLE,
		"Sending spectator for table %d in room %d, index %d",
		table->index, table->room, event->spectator.index);

	/* Save transit info so we have it when game module responds */
	table->transit_name = ggz_strdup(event->caller);
	table->transit_seat = event->spectator.index;

	/* Assemble seat structure */
	spectator.num = event->spectator.index;
	spectator.fd = event->spectator.fd;

	/* Only send name if it's not empty */
	if (event->spectator.name[0] != '\0')
		spectator.name = event->spectator.name;
	else
		spectator.name = NULL;

	result = ggzdmod_set_spectator(table->ggzdmod, &spectator);
	if (result < 0) {
		dbg_msg(GGZ_DBG_TABLE,
			"ERROR: transit_send_spectator_to_game: "
			"failed ggzdmod_set_spectator() call - error %d",
			result);
		status = -1;
	}

	/* Must close remote end of socketpair */
	close(spectator.fd);
	
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

static int transit_find_spectator(GGZTable *table, char *name)
{
	int i, allow_spectators;
#ifdef UNLIMITED_SPECTATORS
	int new, old;
#else
	int old = -1;

	/* See if we allow specators here. */
	pthread_rwlock_rdlock(&game_types[table->type].lock);
	allow_spectators = game_types[table->type].allow_spectators;
	pthread_rwlock_unlock(&game_types[table->type].lock);

	if (!allow_spectators) return -1;
#endif

	/* Look for first open spectator. */
	for (i = 0; i < spectator_seats_num(table); i++)
		if (!table->spectators[i][0])
			return i;

#ifdef UNLIMITED_SPECTATORS
	/* If that failed, see if we allow specators here. */
	pthread_rwlock_rdlock(&game_types[table->type].lock);
	allow_spectators = game_types[table->type].allow_spectators;
	pthread_rwlock_unlock(&game_types[table->type].lock);

	/* If spectators aren't allowed, then don't allow one. */
	if (!allow_spectators)
		return -1;

	/* Otherwise increase the size of the spectator array. */
	old = table->max_num_spectators;
	new = old ? old * 2 : 1;
	table->spectators = ggz_realloc(table->spectators,
					new * sizeof(*table->spectators));
	for (i = old; i < new; i++)
		table->spectators[i][0] = '\0';
	table->max_num_spectators = new;
#endif

	return old;
}

