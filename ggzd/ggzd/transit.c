/*
 * File: transit.c
 * Author: Brent Hendricks
 * Project: GGZ Server
 * Date: 3/26/00
 * Desc: Functions for handling table transits
 * $Id: transit.c 4965 2002-10-20 09:05:32Z jdorje $
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

#ifdef HAVE_CONFIG_H
# include <config.h>		/* Site specific config */
#endif

#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "ggzdmod.h"

#include "client.h"
#include "datatypes.h"
#include "err_func.h"
#include "event.h"
#include "ggzd.h"
#include "net.h"
#include "protocols.h"
#include "seats.h"
#include "transit.h"


/* Server wide data structures*/
extern struct GameInfo game_types[MAX_GAME_TYPES];


/* Packaging for seat events */
typedef struct{
	struct GGZTableSeat seat;
	char caller[MAX_USER_NAME_LEN + 1];
} GGZSeatEventData;

/* Packaging for spectator seat events */
typedef struct{
	struct GGZTableSpectator spectator;
	char caller[MAX_USER_NAME_LEN + 1];
} GGZSpectatorSeatEventData;


typedef struct {
	GGZTransitType opcode;
	GGZClientReqError status;
	int table_index; /* Only for certain operations */
} GGZPlayerTransitEventData;


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

static GGZReturn transit_send_seat_to_game(GGZTable* table,
					   GGZSeatEventData *event);
static int transit_find_seat(GGZTable *table, char *name);

static GGZReturn transit_send_spectator_to_game(GGZTable* table,
					GGZSpectatorSeatEventData *event);
static int transit_find_spectator(GGZTable *table, char *name);


GGZReturn transit_seat_event(int room, int index,
				     struct GGZTableSeat seat, char *caller)
{
	GGZSeatEventData *data = ggz_malloc(sizeof(*data));

	data->seat = seat;
	strcpy(data->caller, caller);
	
	return event_table_enqueue(room, index, transit_seat_event_callback,
				   sizeof(*data), data, NULL);
}

GGZReturn transit_spectator_event(int room, int index,
				  struct GGZTableSpectator spectator,
				  char *caller)
{
	GGZSpectatorSeatEventData *data;

	data = ggz_malloc(sizeof(*data));

	data->spectator = spectator;
	strcpy(data->caller, caller);
	
	return event_table_enqueue(room, index,
				   transit_spectator_event_callback,
				   sizeof(*data), data, NULL);
}

GGZReturn transit_player_event(char* name, GGZTransitType opcode,
			       GGZClientReqError status, int index)
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

	/* Make sure table is in an acceptable state */
	if (table->state == GGZ_TABLE_ERROR
	    || table->state == GGZ_TABLE_DONE) {
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

	if (transit_send_seat_to_game(table, event) != GGZ_OK) {
		/* Otherwise send an error message back to the player */
		transit_player_event(event->caller, action,
				     E_SEAT_ASSIGN_FAIL, 0);
		return GGZ_EVENT_ERROR;
	}

	return GGZ_EVENT_OK;
}

/* Executed by table hander thread */
static GGZEventFuncReturn transit_spectator_event_callback(void* target,
							   size_t size,
							   void* data)
{
	GGZTransitType action;
	GGZTable *table = target;
	GGZSpectatorSeatEventData *event = data;
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

	if (transit_send_spectator_to_game(table, event) != GGZ_OK) {
		/* Otherwise send an error message back to the player */
		transit_player_event(event->caller, action,
				     E_SEAT_ASSIGN_FAIL, 0);
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
					 data->status) < 0)
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
					data->status) < 0)
			return GGZ_EVENT_ERROR;
		break;

	case GGZ_TRANSIT_SEAT:
		if (net_send_update_result(player->client->net,
					   data->status) < 0)
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
static GGZReturn transit_send_seat_to_game(GGZTable* table,
					   GGZSeatEventData *event)
{
	GGZSeat seat = {num: event->seat.index,
			type: event->seat.type,
			fd: event->seat.fd,
			name: event->seat.name};
	int result;

	dbg_msg(GGZ_DBG_TABLE, "Sending seat for table %d in room %d",
		table->index, table->room);
	
	/* Only send name if it's not empty */
	if (seat.name[0] == '\0')
		seat.name = NULL;

	result = ggzdmod_set_seat(table->ggzdmod, &seat);
	if (event->seat.type == GGZ_SEAT_PLAYER) {
		/* Must close remote end of socketpair */
		close(seat.fd);
	}
	if (result < 0) {
		err_msg("transit_send_seat_to_game: "
			"failed ggzdmod_set_seat() call - error %d.",
			result);
		return GGZ_ERROR;
	}

	if (seat.type == GGZ_SEAT_PLAYER) {
		table_game_join(table, event->caller, seat.num);
	} else if (seat.type == GGZ_SEAT_OPEN) {
		table_game_leave(table, event->caller, seat.num);
	} else {
		err_msg("transit_send_seat_to_game: invalid seat change.");
	}
	
	return GGZ_OK;
}

/*
 * transit_send_spectator_to_game sets the spectator on ggzdmod
 *
 * Returns 0 on success, -1 on failure (in which case the
 * player should be sent an failure notice).
 */
static GGZReturn transit_send_spectator_to_game(GGZTable* table,
					GGZSpectatorSeatEventData *event)
{
	GGZSpectator seat = {num: event->spectator.index,
			     fd: event->spectator.fd,
			     name: event->spectator.name};
	int result;

	dbg_msg(GGZ_DBG_TABLE,
		"Sending spectator for table %d in room %d, index %d",
		table->index, table->room, event->spectator.index);

	/* Only send name if it's not empty */
	if (seat.name[0] == '\0')
		seat.name = NULL;

	result = ggzdmod_set_spectator(table->ggzdmod, &seat);
	if (seat.name) {
		/* Must close remote end of socketpair */
		close(seat.fd);
	}
	if (result < 0) {
		err_msg("transit_send_spectator_to_game: "
			"failed ggzdmod_set_spectator() call - error %d",
			result);
		return GGZ_ERROR;
	}

	if (seat.name) {
	  table_game_spectator_join(table, event->caller, seat.num);
	} else {
	  table_game_spectator_leave(table, event->caller, seat.num);
	}

	return GGZ_OK;
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
