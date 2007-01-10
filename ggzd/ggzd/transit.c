/*
 * File: transit.c
 * Author: Brent Hendricks
 * Project: GGZ Server
 * Date: 3/26/00
 * Desc: Functions for handling table transits
 * $Id: transit.c 8887 2007-01-10 01:27:41Z jdorje $
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
#include "ggzdmod-ggz.h"

#include "client.h"
#include "datatypes.h"
#include "err_func.h"
#include "event.h"
#include "ggzd.h"
#include "net.h"
#include "protocols.h"
#include "seats.h"
#include "transit.h"


/* Packaging for seat and spectator events */
typedef struct{
	GGZTransitType transit;
	struct GGZTableSeat seat;
	char caller[MAX_USER_NAME_LEN + 1];
	int reason; /* only for LEAVE/JOIN transits */
} GGZSeatEventData;


typedef struct {
	GGZTransitType opcode;
	GGZClientReqError status;
	int reason; /* Only for LEAVE/JOIN transits */
	char caller[MAX_USER_NAME_LEN + 1]; /* only for some ops */
	int table_index; /* Only for certain operations */
} GGZPlayerTransitEventData;


/* Local functions for handling transits */
static GGZEventFuncReturn transit_player_event_callback(void* target,
                                                        size_t size,
                                                        void* data);
static GGZEventFuncReturn transit_seat_event_callback(void* target,
						      size_t size,
						      void* data);

static GGZReturn transit_send_seat_to_game(GGZTable* table,
					   GGZTransitType action,
					   GGZSeatEventData *event);

static int transit_table_find_player(GGZTable *table, char *name);
static int transit_table_find_spectator(GGZTable *table, char *name);
static int transit_find_required_seat(GGZTable *table, const char *name);
static int transit_find_available_seat(GGZTable *table, char *name);
static int transit_find_available_spectator(GGZTable *table, char *name);


GGZReturn transit_seat_event(int room, int index, GGZTransitType transit,
			     struct GGZTableSeat seat, const char *caller,
			     int reason)
{
	GGZSeatEventData *data = ggz_malloc(sizeof(*data));

	data->transit = transit;
	data->seat = seat;
	data->reason = reason;
	strcpy(data->caller, caller);
	
	return event_table_enqueue(room, index, transit_seat_event_callback,
				   sizeof(*data), data, NULL);
}


GGZReturn transit_player_event(const char *name, GGZTransitType opcode,
			       GGZClientReqError status,
			       const char *caller, int reason, int index)
{
	GGZPlayerTransitEventData* data = ggz_malloc(sizeof(*data));

	data->opcode = opcode;
	data->status = status;
	data->table_index = index;
	data->reason = reason;
	strcpy(data->caller, caller);
	
	return event_player_enqueue(name, transit_player_event_callback, 
				    sizeof(*data), data, NULL);
}


/* Executed by table hander thread */
static GGZEventFuncReturn transit_seat_event_callback(void* target,
						      size_t size, 
						      void* data)
{
	GGZTable *table = target;
	GGZSeatEventData *event = data;
	struct GGZTableSeat *seat = &(event->seat);
	bool spectating = false;

	dbg_msg(GGZ_DBG_TABLE, 
		"%s requested seat change %d on table %d: "
		"Seat %d to %s with fd %d", 
		event->caller, event->transit, table->index,
		seat->index, seat->name, seat->fd);

	/* Figure out what kind of event this is */
	switch (event->transit) {
	case GGZ_TRANSIT_JOIN:
	case GGZ_TRANSIT_LEAVE:
	case GGZ_TRANSIT_SEAT:
	case GGZ_TRANSIT_MOVE:
	case GGZ_TRANSIT_SIT:
		spectating = 0;
		break;
	case GGZ_TRANSIT_JOIN_SPECTATOR:
	case GGZ_TRANSIT_LEAVE_SPECTATOR:
	case GGZ_TRANSIT_STAND:
		spectating = 1;
		break;
	}

	/* Make sure table is in an acceptable state */
	if (table->state == GGZ_TABLE_ERROR
	    || table->state == GGZ_TABLE_DONE) {
		/* Notify player that transit failed */
		/* Don't care if this fails, we aren't transiting anyway */
		transit_player_event(event->caller, event->transit,
				     E_BAD_OPTIONS, event->caller, -1, 0);
		return GGZ_EVENT_OK;
	}

	if (!spectating && seat->type == GGZ_SEAT_PLAYER) {
		int index = transit_find_required_seat(table, event->caller);

		if (index >= 0) {
			/* Force the player into this seat. */
			seat->index = index;
		}
	}

	/* Try to find a seat if one isn't specified */
	if (seat->index == GGZ_SEATNUM_ANY
	    && (spectating || seat->type == GGZ_SEAT_PLAYER)) {
		if (spectating)
			seat->index = transit_find_available_spectator(table,
						event->caller);
		else
			seat->index = transit_find_available_seat(table,
						event->caller);
		if (seat->index < 0) {
			/* Don't care if this fails,
			   we aren't transiting anyway */
			transit_player_event(event->caller, event->transit,
					     E_TABLE_FULL, event->caller,
					     -1, 0);
			return GGZ_EVENT_OK;
		}
	}

	if (event->transit == GGZ_TRANSIT_LEAVE
	    && event->reason == GGZ_LEAVE_NORMAL
	    && table->state == GGZ_TABLE_PLAYING) {
		/* The seat becomes abandoned not open. */
		strncpy(event->seat.name, table->seat_names[seat->index],
			sizeof(event->seat.name));
		event->seat.type = GGZ_SEAT_ABANDONED;
	}

	if (transit_send_seat_to_game(table, event->transit,
				      event) != GGZ_OK) {
		/* Otherwise send an error message back to the player */
		transit_player_event(event->caller, event->transit,
				     E_SEAT_ASSIGN_FAIL,
				     event->caller, -1, 0);

		/* This isn't an error really; it can happen if two joins
		   happen simultaneously.  If we returned an error here it
		   would cause the table to be killed. */
		return GGZ_EVENT_OK;
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
		player->transit = false;
		if (data->status == E_OK || data->status == E_NO_STATUS)
			player->table = -1;
		pthread_rwlock_unlock(&player->lock);
		
		if (data->status != E_NO_STATUS
		    && net_send_table_leave_result(player->client->net,
						   data->status) != GGZ_OK)
			return GGZ_EVENT_ERROR;
		if (data->status == E_NO_STATUS || data->status == E_OK) {
			char *rplayer = NULL;
			if (data->reason == GGZ_LEAVE_BOOT)
				rplayer = data->caller;
			if (net_send_table_leave(player->client->net,
						 data->reason,
						 rplayer) != GGZ_OK)
				return GGZ_EVENT_ERROR;
		}
		break;

	case GGZ_TRANSIT_JOIN:
	case GGZ_TRANSIT_JOIN_SPECTATOR:
		pthread_rwlock_wrlock(&player->lock);
		player->transit = false;
		if (data->status == E_OK || data->status == E_NO_STATUS) {
			player->table = data->table_index;
		}
		pthread_rwlock_unlock(&player->lock);

		if (data->status != E_NO_STATUS
		    && net_send_table_join_result(player->client->net,
					       data->status) != GGZ_OK)
			return GGZ_EVENT_ERROR;
		if ((data->status == E_NO_STATUS || data->status == E_OK)
		    && net_send_table_join(player->client->net,
					   data->opcode != GGZ_TRANSIT_JOIN,
					   data->table_index) != GGZ_OK)
			return GGZ_EVENT_ERROR;
		break;

	case GGZ_TRANSIT_SEAT:
		if (net_send_update_result(player->client->net,
					   data->status) != GGZ_OK)
			return GGZ_EVENT_ERROR;
		break;
	case GGZ_TRANSIT_SIT:
	case GGZ_TRANSIT_STAND:
	case GGZ_TRANSIT_MOVE:
		player->transit = false;
		if (net_send_reseat_result(player->client->net,
					   data->status) != GGZ_OK)
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
					   GGZTransitType action,
					   GGZSeatEventData *event)
{
	GGZSeat seat;
	GGZSpectator sseat;
	int result = 0;
	int old_seat = -1;

	/* Only send name if it's not empty */
	if (event->seat.name[0] == '\0')
		seat.name = sseat.name = NULL;

	/* First, send the data to the game module. */
	switch (action) {
	case GGZ_TRANSIT_JOIN:
	case GGZ_TRANSIT_LEAVE:
	case GGZ_TRANSIT_SEAT:
		dbg_msg(GGZ_DBG_TABLE, "Sending seat for table %d in room %d",
			table->index, table->room);

		seat.num = event->seat.index;
		seat.type = event->seat.type;
		seat.fd = event->seat.fd;
		seat.name = event->seat.name[0] ? event->seat.name : NULL;

		result = ggzdmod_set_seat(table->ggzdmod, &seat);
		break;
	case GGZ_TRANSIT_SIT:
		old_seat = transit_table_find_spectator(table, event->caller);
		result = ggzdmod_reseat(table->ggzdmod,
					old_seat, 1,
					event->seat.index, 0);
		break;
	case GGZ_TRANSIT_STAND:
		old_seat = transit_table_find_player(table, event->caller);
		result = ggzdmod_reseat(table->ggzdmod,
					old_seat, 0,
					event->seat.index, 1);
		break;
	case GGZ_TRANSIT_MOVE:
		/* FIXME: moves shouldn't be allowed while the game
		   is PLAYING. */
		old_seat = transit_table_find_player(table, event->caller);
		result = ggzdmod_reseat(table->ggzdmod,
					old_seat, 0,
					event->seat.index, 0);
		break;
	case GGZ_TRANSIT_JOIN_SPECTATOR:
	case GGZ_TRANSIT_LEAVE_SPECTATOR:

		sseat.num = event->seat.index;
		sseat.name = event->seat.name[0] ? event->seat.name : NULL;
		sseat.fd = event->seat.fd;

		result = ggzdmod_set_spectator(table->ggzdmod, &sseat);
		if (sseat.name) {
			/* Must close remote end of socketpair */
			close(sseat.fd);
		}
		break;
	}

	if (result < 0) {
		err_msg("transit_send_seat_to_game (transit %d): "
			"failed ggzdmod call - error %d.",
			action, result);
		return GGZ_ERROR;
	}

	/* Next, set it internally. */
	switch (action) {
	case GGZ_TRANSIT_JOIN:
		table_game_join(table, event->caller,
				event->reason, seat.num);
		break;
	case GGZ_TRANSIT_LEAVE:
		table_game_leave(table, event->caller,
				 event->reason, seat.num);
		break;
	case GGZ_TRANSIT_SEAT:
		table_game_seatchange(table, seat.type, seat.num);
		break;
	case GGZ_TRANSIT_SIT:
		table_game_reseat(table, GGZ_RESEAT_SIT,
				  event->caller,
				  old_seat, event->seat.index);
		break;
	case GGZ_TRANSIT_STAND:
		table_game_reseat(table, GGZ_RESEAT_STAND,
				  event->caller,
				  old_seat, event->seat.index);
		break;
	case GGZ_TRANSIT_MOVE:
		table_game_reseat(table, GGZ_RESEAT_MOVE,
				  event->caller,
				  old_seat, event->seat.index);
		break;
	case GGZ_TRANSIT_JOIN_SPECTATOR:
		table_game_spectator_join(table, event->caller,
					  event->reason, sseat.num);
		break;
	case GGZ_TRANSIT_LEAVE_SPECTATOR:
		table_game_spectator_leave(table, event->caller,
					   event->reason, sseat.num);
		break;
	}
	
	return GGZ_OK;
}


static int transit_table_find_player(GGZTable *table, char *name)
{
	int i, num_seats = seats_num(table);

	for (i = 0; i < num_seats; i++) {
		if (seats_type(table, i) == GGZ_SEAT_PLAYER
		    && !strcasecmp(table->seat_names[i], name))
			return i;
	}

	return -1;
}


static int transit_table_find_spectator(GGZTable *table, char *name)
{
	int i, num_seats = spectator_seats_num(table);

	for (i = 0; i < num_seats; i++) {
		if (!strcasecmp(table->spectators[i], name))
			return i;
	}

	return -1;
}


/* Call this function to find the required seat for a player.  Even if
   the player gives a specific preference it should be ignored. */
static int transit_find_required_seat(GGZTable *table, const char *name)
{
	int i, num_seats = seats_num(table);

	for (i = 0; i < num_seats; i++) {
		GGZSeatType type = seats_type(table, i);

		if (type == GGZ_SEAT_ABANDONED
		    && strcasecmp(table->seat_names[i], name) == 0)
			return i;
	}

	return -1;
}

/* Call this function to find the preferred seat for a player, only if
   the player doesn't have a preference. */
static int transit_find_available_seat(GGZTable *table, char *name)
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

static int transit_find_available_spectator(GGZTable *table, char *name)
{
	int i, allow_spectators;
	int new, old;

	/* Look for first open spectator. */
	for (i = 0; i < spectator_seats_num(table); i++)
		if (!table->spectators[i][0])
			return i;

	/* If that failed, see if we allow specators here. */
	pthread_rwlock_rdlock(&game_types[table->type].lock);
	allow_spectators = game_types[table->type].allow_spectators;
	pthread_rwlock_unlock(&game_types[table->type].lock);

	/* If spectators aren't allowed, then don't allow one. */
	if (!allow_spectators)
		return -1;

	/* Otherwise increase the size of the spectator array. */
	old = table->max_num_spectators;
	new = (old > 0) ? old * 2 : 1;
	table->spectators = ggz_realloc(table->spectators,
					new * sizeof(*table->spectators));
	for (i = old; i < new; i++) {
		table->spectators[i][0] = '\0';
	}
	table->max_num_spectators = new;

	return old;
}
