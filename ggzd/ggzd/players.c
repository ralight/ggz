/*
 * File: players.c
 * Author: Brent Hendricks
 * Project: GGZ Server
 * Date: 10/18/99
 * Desc: Functions for handling players
 * $Id: players.c 4541 2002-09-13 05:49:33Z jdorje $
 *
 * Desc: Functions for handling players.  These functions are all
 * called by the player handler thread.  Since this thread is the only
 * one which can write to the player structure we do not need to lock
 * it when reading from it.
 *
 * Copyright (C) 1999, 2000 Brent Hendricks.
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA */


#include <config.h>

#include <sys/time.h>
#include <sys/types.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <ctype.h>
#include <ggz.h>

#include <ggzd.h>
#include <datatypes.h>
#include <players.h>
#include <table.h>
#include <protocols.h>
#include <err_func.h>
#include <seats.h>
#include <motd.h>
#include <room.h>
#include <chat.h>
#include <hash.h>
#include <transit.h>
#include <net.h>
#include <perms.h>
#include "client.h"


/* Timeout for server resync */
#define GGZ_RESYNC_SEC  0
#define GGZ_RESYNC_USEC 500000


/* Server wide data structures*/
extern struct GameInfo game_types[MAX_GAME_TYPES];
extern struct GGZState state;
extern Options opt;


/* Local functions for handling players */
static int player_transit(GGZPlayer* player, GGZTransitType opcode, int index);
static GGZPlayerHandlerStatus player_send_ping(GGZPlayer *player);
static int player_get_time_since_ping(GGZPlayer *player);


/**
 * Create and initialize new player structure.
 *
 * @param client : pointer to the client connection.
 *
 * @return pointer to new GGZPlayer object
 */
GGZPlayer* player_new(GGZClient *client)
{
	GGZPlayer *player;
	
	/* Allocate new player structure */
	player = ggz_malloc(sizeof(GGZPlayer));
	
	/* Initialize player data */
	pthread_rwlock_init(&player->lock, NULL);
	player->client = client;
	player->table = -1;
	player->game_fd = -1;
	player->launching = 0;
	player->transit = 0;
	player->room = -1;
	player->login_status = GGZ_LOGIN_NONE;
	player->perms = PERMS_DEFAULT_ANON;
	strcpy(player->name, "<none>");
	player->room_events = NULL;
	player->my_events_head = NULL;
	player->my_events_tail = NULL;
	player->lag_class = 1;			/* Assume they are low lag */
	player->next_ping = 0;			/* Don't ping until login */

	return player;
}


/*
 * player_logout() does various cleanups for logout
 *
 * Receives:
 * GGZPlayer* player : pointer to player structure
 */
void player_logout(GGZPlayer* player)
{
	long connect_time;
	int hours, mins, secs;
	int anon = 0;

	pthread_rwlock_rdlock(&player->lock);
	/* There's no need to remove them if they aren't "here" */
	if(strcmp(player->name, "<none>")) {
		connect_time = (long)time(NULL) - player->login_time;
		if(player->login_status == GGZ_LOGIN_ANON)
			anon = 1;
		pthread_rwlock_unlock(&player->lock);

		/* Take their name off the hash list */
		hash_player_delete(player->name);

		hours = connect_time / 3600;
		mins = (connect_time % 3600) / 60;
		secs = connect_time % 60;
		log_msg(GGZ_LOG_CONNECTION_INFO,
			"LOGOUT %s%sconnected for %d:%02d:%02d seconds",
			player->name, anon?" (anon) ":" ", hours, mins, secs);
		if(anon)
			log_logout_anon();
		else
			log_logout_regd();
	} else
		pthread_rwlock_unlock(&player->lock);

	dbg_msg(GGZ_DBG_CONNECTION, "Logging out %s", player->name);

	/* Remove us from room, so we get no new events */
	if (player->room != -1)
		room_join(player, -1);
	
	/* FIXME: is this the right place for this? */
	event_player_flush(player);

	/* FIXME: need to leave table if we're at one 
	player_transit(player, GGZ_TRANSIT_LEAVE, player->table);*/

	/* Close channel if it's open */
	if (player->game_fd != -1) {
		dbg_msg(GGZ_DBG_TABLE, "Closing leftover channel for %s", 
			player->name);
		close(player->game_fd);
	}
}


/*
 * player_updates() handles queued-up GGZEvents (both room-wide and
 * private)
 *
 * Receives:
 * GGZPlayer* player : pointer to player structure 
 *
 * Returns: 
 * int : 0 on sucess.  -1 on error
 */
int player_updates(GGZPlayer* player)
{
	int changed = 0;

	/* Process room events */
	if (player->room != -1 && event_room_handle(player) < 0)
		return -1;
	
	/* Process personal events */
	if (player->my_events_head)
		if (event_player_handle(player) < 0)
			return -1;

	/* Lowest priority update - send a PING / check lag */
	if(player->next_ping && time(NULL) >= player->next_ping) {
		if(player_send_ping(player) < 0)
			return -1;
	} else if (player->next_ping == 0) {
		/* We're waiting for a PONG, but if we wait too long
		   we should increase the lag class without waiting */
		
		while (player->lag_class < 5
		       && player_get_time_since_ping(player) >=
		             opt.lag_class[player->lag_class - 1]) {
			player->lag_class++;
			changed = 1;
		}
		
		if (changed)
			room_notify_lag(player->name, player->room);
	}

	return 0;
}


int player_get_room(GGZPlayer *player)
{
	int room;

	if (!player) return -1;

	pthread_rwlock_rdlock(&player->lock);
	room = player->room;
	pthread_rwlock_unlock(&player->lock);

	return room;
}


GGZPlayerType player_get_type(GGZPlayer *player)
{
	int type;

	pthread_rwlock_rdlock(&player->lock);
	if(player->login_status == GGZ_LOGIN_ANON)
		type = GGZ_PLAYER_GUEST;
	else if(perms_is_admin(player))
		type = GGZ_PLAYER_ADMIN;
	else
		type = GGZ_PLAYER_NORMAL;
	pthread_rwlock_unlock(&player->lock);

	return type;
}


/*
 * player_table_launch() handles REQ_TABLE_LAUNCH request from the
 * client.  It reads in the table data and seat assigments from the
 * client, does some sanity checking, and allocates a new GGZTable
 * structure.
 *
 * Receives:
 * GGZPlayer* player : pointer to player structure 
 * int p_fd : player's fd
 * int *t_fd : pointer to fd for communication with game module
 *
 * Returns: 
 * int : one of
 *  GGZ_REQ_DISCONNECT   : i/o error
 *  GGZ_REQ_FAIL         : request failed
 *  GGZ_REQ_OK           : request succeeded.  t_fd points to new fd 
 */
GGZPlayerHandlerStatus player_table_launch(GGZPlayer* player, GGZTable *table)
{
	int room;
	GGZPlayerHandlerStatus status;

	dbg_msg(GGZ_DBG_TABLE, "Handling table launch for %s", player->name);

	/* Check permissions */
	if(perms_check(player, PERMS_LAUNCH_TABLE) == PERMS_DENY) {
		dbg_msg(GGZ_DBG_TABLE, "%s insufficient perms to launch",
			player->name);
		if(net_send_table_launch(player->client->net, E_NO_PERMISSION) < 0)
			return GGZ_REQ_DISCONNECT;
		return GGZ_REQ_FAIL;
	}

	/* Check if in a room */
	if ( (room = player->room) == -1) {
		dbg_msg(GGZ_DBG_TABLE, "%s tried to launch table in room -1", 
			player->name);
		if (net_send_table_launch(player->client->net, E_NOT_IN_ROOM) < 0)
			return GGZ_REQ_DISCONNECT;
		return GGZ_REQ_FAIL;
	}
	
#ifndef UNLIMITED_SEATS
	/* Silly client. Tables are only so big*/
	if (seats_num(table) > MAX_TABLE_SIZE) {
		dbg_msg(GGZ_DBG_TABLE, 
			"%s tried to launch a table with > %d seats",
			player->name, MAX_TABLE_SIZE);
		if (net_send_table_launch(player->client->net, E_BAD_OPTIONS) < 0)
			return GGZ_REQ_DISCONNECT;
		return GGZ_REQ_FAIL;
	}
#else
	/* Shouldn't some check be done?  Note that the number of players is
	   checked against the player_allow_list later.  --JDS */
#endif
	
	/* Don't allow multiple table launches */
	if (player->table != -1 || player->launching) {
		dbg_msg(GGZ_DBG_TABLE, "%s tried to launch table while at one",
			player->name);
		if (net_send_table_launch(player->client->net, E_LAUNCH_FAIL) < 0)
			return GGZ_REQ_DISCONNECT;
		return GGZ_REQ_FAIL;
	}

	/* Make sure type index matches */
	/* FIXME: Do we need a room lock here? */
	/* RG: Eventually we will need more room locks when we have dynamic */
	/*     rooms, right now the room's game_type can't change           */
	if (table->type != rooms[room].game_type) {
		dbg_msg(GGZ_DBG_TABLE, "%s tried to launch wrong table type",
			player->name);
		if (net_send_table_launch(player->client->net, E_NOT_IN_ROOM) < 0)
			return GGZ_REQ_DISCONNECT;
		return GGZ_REQ_FAIL;
	}

	/* Do actual launch of table */
	status = table_launch(table, player->name);
	
	if (status == 0) {
		/* Mark player as launching table so we can't launch more */
		pthread_rwlock_wrlock(&player->lock);
		player->launching = 1;
		pthread_rwlock_unlock(&player->lock);
	} else {
		if (net_send_table_launch(player->client->net, (char)status) < 0)
			return GGZ_REQ_DISCONNECT;
		status = GGZ_REQ_FAIL;
	}
	
	return status;
}


GGZEventFuncReturn player_launch_callback(void* target, size_t size,
					  void* data)
{
	GGZLaunchEventData *event = data;
	GGZPlayer* player = (GGZPlayer*)target;

	/* Launch compleyed */
	pthread_rwlock_wrlock(&player->lock);
	player->launching = 0;
	pthread_rwlock_unlock(&player->lock);

	dbg_msg(GGZ_DBG_TABLE, "%s launch result: %d",
		player->name, event->status);
	
	/* Automatically join newly created table */
	if (event->status == 0)
		player_transit(player, GGZ_TRANSIT_JOIN, event->table_index);

	/* Return status to client */
	if (net_send_table_launch(player->client->net,
				  (char)event->status) < 0)
		return GGZ_EVENT_ERROR;
	
	return GGZ_EVENT_OK;
}


GGZPlayerHandlerStatus player_table_update(GGZPlayer* player, GGZTable *table)
{
	int i, num_seats;
	char owner[MAX_USER_NAME_LEN + 1];
	GGZPlayerHandlerStatus status = 0;
	GGZTable *real_table;
	struct GGZTableSeat seat;
	
	dbg_msg(GGZ_DBG_TABLE, "Handling table update for %s", player->name);
	
	/* Check permissions */
	real_table = table_lookup(table->room, table->index);
	strcpy(owner, real_table->owner);
	pthread_rwlock_unlock(&real_table->lock);
	if (strcmp(owner, player->name) != 0) {
		dbg_msg(GGZ_DBG_TABLE, "%s tried to modify table owned by %s",
			player->name, owner);
		if(net_send_update_result(player->client->net, E_NO_PERMISSION) < 0)
			return GGZ_REQ_DISCONNECT;
		return GGZ_REQ_FAIL;
	}
	
	/* Do actual table update */

	/*  Update the description if necessary */
	if (strlen(table->desc))
		table_set_desc(real_table, table->desc);
	
	/* Find the seat that was updated: it's the one that isn't NONE */
	num_seats = seats_num(table);
	for (i = 0; i < num_seats; i++) {
		if (seats_type(table, i) != GGZ_SEAT_NONE) {
			seat.index = i;
			seat.type = seats_type(table, i);
			strcpy(seat.name, table->seat_names[i]);
			seat.fd = -1;
			
			if ( (status = transit_seat_event(table->room, table->index, seat, player->name)))
				break;
		}
	}

	/* Return any immediate failures to client*/
	if (status < 0) {
		if (net_send_update_result(player->client->net, (char)status) < 0)
			return GGZ_REQ_DISCONNECT;
		status = GGZ_REQ_FAIL;
	}
	
	return status;
}


/* 
 * player_table_join() handles REQ_TABLE_JOIN request from the client.
 * It reads in the table number from the client, does some sanity
 * checking.
 *
 * Receives:
 * GGZPlayer* player : pointer to player structure
 * int p_fd : player's fd
 *
 * Returns:
 * int : one of
 *  GGZ_REQ_DISCONNECT   : i/o error
 *  GGZ_REQ_FAIL         : request failed
 *  GGZ_REQ_OK           : request succeeded. 
 */
GGZPlayerHandlerStatus player_table_join(GGZPlayer* player, int index)
{
	int status;

	dbg_msg(GGZ_DBG_TABLE, "Handling table join for %s", player->name);

	dbg_msg(GGZ_DBG_TABLE, "%s attempting to join table %d in room %d", 
		player->name, index, player->room);

	if (player->table != -1)
		status = E_AT_TABLE;
	else if (player->transit)
		status = E_IN_TRANSIT;
	else if (perms_check(player, PERMS_JOIN_TABLE) == PERMS_DENY)
		status = E_NO_PERMISSION;
	else /* Send a join event to the table */
		status = player_transit(player, GGZ_TRANSIT_JOIN, index);

	/* Return any immediate failures to client*/
	if (status < 0) {
		if (net_send_table_join(player->client->net, (char)status) < 0)
			return GGZ_REQ_DISCONNECT;
		status = GGZ_REQ_FAIL;
	}
	
	return status;
}

GGZPlayerHandlerStatus player_table_join_spectator(GGZPlayer* player, int index)
{
	int status;

	dbg_msg(GGZ_DBG_TABLE, "Handling table join (as spectator) for %s", player->name);

	dbg_msg(GGZ_DBG_TABLE, "%s attempting to join (as spectator) table %d in room %d", 
		player->name, index, player->room);

	if (player->table != -1)
		status = E_AT_TABLE;
	else if (player->transit)
		status = E_IN_TRANSIT;
	else if (perms_check(player, PERMS_JOIN_TABLE) == PERMS_DENY)
		status = E_NO_PERMISSION;
	else /* Send a join event to the table */
		status = player_transit(player, GGZ_TRANSIT_JOIN_SPECTATOR, index);

	/* Return any immediate failures to client*/
	if (status < 0) {
		if (net_send_table_join(player->client->net, (char)status) < 0)
			return GGZ_REQ_DISCONNECT;
		status = GGZ_REQ_FAIL;
	}
	
	return status;
}

/* 
 * player_table_leave() handles REQ_TABLE_LEAVE request from the
 * client.
 *
 * Receives:
 * GGZPlayer* player : pointer to player structure
 * int p_fd : player's fd
 *
 * Returns:
 * int : one of
 *  GGZ_REQ_DISCONNECT   : i/o error
 *  GGZ_REQ_FAIL         : request failed
 *  GGZ_REQ_OK           : request succeeded.  
 */
GGZPlayerHandlerStatus player_table_leave(GGZPlayer* player,
					  int spectator, int force)
{
	int status, gametype;
	char allow;
	GGZTableState state;
	GGZTable *table;
	GGZTransitType transit;

	/* Check if leave during gameplay is allowed */
	table = table_lookup(player->room, player->table);

	if (!table) {
		dbg_msg(GGZ_DBG_TABLE,
			"%s tried to leave nonexistent table %d (room %d)",
			player->name, player->table, player->room);
		if (player->room >= 0 && player->table >= 0) {
			err_msg("Couldn't find %s's table %d (room %d)!",
				player->name, player->table, player->room);
			player->table = -1; /* What else can we do? */
		}
		/* This is probably an error on the client's part.  But
		   to avoid confusion, the best bet is probably to
		   claim success. */
		if (net_send_table_leave(player->client->net, 0) < 0)
			return GGZ_REQ_DISCONNECT;
		return 0;
	}		

	if (spectator) {
		pthread_rwlock_unlock(&table->lock);

		/* FIXME: make sure the player really is a spectator!
		   Otherwise the table could break! */
		allow = 1;
		state = GGZ_TABLE_WAITING;

		transit = GGZ_TRANSIT_LEAVE_SPECTATOR;
	} else {
		gametype = table->type;
		state = table->state;
		pthread_rwlock_unlock(&table->lock);

		/* FIXME: make sure the player isn't a spectator!  Otherwise
		   we could up killing the table when a spectator leaves! */
		pthread_rwlock_rdlock(&game_types[gametype].lock);
		allow = game_types[gametype].allow_leave;
		pthread_rwlock_unlock(&game_types[gametype].lock);

		transit = GGZ_TRANSIT_LEAVE;
	}

	dbg_msg(GGZ_DBG_TABLE, "%s attempting to leave table %d%s", 
		player->name, player->table,
		spectator ? " (spectating)" : (force ? " (forced)" : ""));

	/* Error if we're already in transit */
	if (player->transit)
		status = E_IN_TRANSIT;
	else if (!spectator && state == GGZ_TABLE_PLAYING && !allow) {
		if (force)
			status = table_kill(player->room, player->table,
					    player->name);
		else 
			status = E_LEAVE_FORBIDDEN;
	} else { 
		/* All clear: send leave event to table */
		status = player_transit(player, transit, player->table);
	}

	/* Return any immediate failures to client*/
	if (status < 0) {
		if (net_send_table_leave(player->client->net,
					 (char)status) < 0)
			return GGZ_REQ_DISCONNECT;
		status = GGZ_REQ_FAIL;
	}

	return status;
}


#if 0 /* Folded into player_table_leave() */
/* 
 * player_table_leave_spectator() handles REQ_TABLE_LEAVE_SPECTATOR request from the
 * client.
 *
 * Receives:
 * GGZPlayer* player : pointer to player structure
 * int p_fd : player's fd
 *
 * Returns:
 * int : one of
 *  GGZ_REQ_DISCONNECT   : i/o error
 *  GGZ_REQ_FAIL         : request failed
 *  GGZ_REQ_OK           : request succeeded.  
 */
GGZPlayerHandlerStatus player_table_leave_spectator(GGZPlayer* player)
{
	int status, gametype;
	GGZTableState state;
	GGZTable *table;

	/* Check if leave during gameplay is allowed */
	table = table_lookup(player->room, player->table);

	if (!table) {
		dbg_msg(GGZ_DBG_TABLE,
			"%s tried to leave nonexistent table %d (room %d)",
			player->name, player->table, player->room);
		/* This is probably an error on the client's part.  But
		   to avoid confusion, the best bet is probably to
		   claim success. */
		if (net_send_table_leave(player->client->net, 0) < 0)
			return GGZ_REQ_DISCONNECT;
		return 0;
	}		

	dbg_msg(GGZ_DBG_TABLE, "%s attempting to leave table %d as spectator",
		player->name, player->table);

	gametype = table->type;
	state = table->state;
	pthread_rwlock_unlock(&table->lock);

	/* Error if we're already in transit */
	if (player->transit)
		status = E_IN_TRANSIT;
	else 
		/* All clear: send leave event to table */
		status = player_transit(player, GGZ_TRANSIT_LEAVE_SPECTATOR,
					player->table);

	/* Return any immediate failures to client*/
	if (status < 0) {
		if (net_send_table_leave(player->client->net,
					 (char)status) < 0)
			return GGZ_REQ_DISCONNECT;
		status = GGZ_REQ_FAIL;
	}

	return status;
}
#endif


static int player_transit(GGZPlayer* player, GGZTransitType opcode, int index)
{
	struct GGZTableSeat seat;
	struct GGZTableSpectator spectator;
	int status, spectating, seat_num, try;

	/* Do some quick sanity checking */
	if (player->room == -1) 
		return E_NOT_IN_ROOM;
	if (index == -1)
		return E_NO_TABLE;

	/* Implement LEAVE by setting my seat to open */
	switch (opcode) {
	case GGZ_TRANSIT_LEAVE: 
	case GGZ_TRANSIT_LEAVE_SPECTATOR:
		spectating = (opcode == GGZ_TRANSIT_LEAVE_SPECTATOR);
		/* At this point, whether we're spectating or not is
		   determined by what the client told us.  But we can't
		   trust the client, so we double-check. */
		for (try = 0, seat_num = -1; try < 2 && seat_num < 0; try++) {
			if (spectating)
				seat_num = table_find_spectator(player->room,
								index,
								player->name);
			else
				seat_num = table_find_player(player->room,
							     index,
							     player->name);
			if (seat_num < 0) spectating = !spectating;
		}

		if (seat_num < 0) {
			/* Uh oh.  A ggzd bug.  But it doesn't have
			   to be fatal. */
			err_msg("%s couldn't be found at table %d (room %d).",
				player->name, player->table, player->room);
			/* Fake success, so that the client doesn't get
			   confused. */
			(void) net_send_table_leave(player->client->net, 0);
			/* Remove the player from the table.  Perhaps we
			   should see if they're actually at a different
			   table?  Or perhaps we should send a full update? */
			player->table = -1;
			return 0;
		}

		if (spectating) {
			spectator.index = seat_num;
			spectator.name[0] = '\0';
			spectator.fd = -1;

			status = transit_spectator_event(player->room, index,
							 spectator,
							 player->name);
		} else {
			seat.index = seat_num;
			seat.type = GGZ_SEAT_OPEN;
			seat.name[0] = '\0';
			seat.fd = -1;
		
			status = transit_seat_event(player->room, index, seat,
						    player->name);
		}
		break;
	case GGZ_TRANSIT_JOIN:
		seat.index = GGZ_SEATNUM_ANY; /* Take first available seat */
		seat.type = GGZ_SEAT_PLAYER;
		seat.fd = player->game_fd;
		strcpy(seat.name, player->name);
		
		status = transit_seat_event(player->room, index, seat,
					    player->name);
		/* Now that channel fd has been sent, rest it here */
		pthread_rwlock_wrlock(&player->lock);
		player->game_fd = -1;
		pthread_rwlock_unlock(&player->lock);

		break;
	case GGZ_TRANSIT_JOIN_SPECTATOR:
		spectator.index = GGZ_SEATNUM_ANY;
		spectator.fd = player->game_fd;
		strcpy(spectator.name, player->name);

		status = transit_spectator_event(player->room, index,
						 spectator, player->name);
		/* Now that channel fd has been sent, rest it here */
		pthread_rwlock_wrlock(&player->lock);
		player->game_fd = -1;
		pthread_rwlock_unlock(&player->lock);

		break;
	default:
		/* Should never get here */
		status = -1;
		break;
	}
	
	/* If enqueue fails, it's because the table has been removed */
	if (status < 0)
		return E_NO_TABLE;

	/* Mark player as "in transit" */
	pthread_rwlock_wrlock(&player->lock);
	player->transit = 1;
	pthread_rwlock_unlock(&player->lock);

	return 0;
}


GGZPlayerHandlerStatus player_list_players(GGZPlayer* player)
{
	int i, count, room;
	GGZPlayer* p;
	GGZPlayer* data;


	dbg_msg(GGZ_DBG_UPDATE, "Handling player list request for %s", 
		player->name);
		
	/* Don't send list if they're not in a room */
	if (player->room == -1) {
		dbg_msg(GGZ_DBG_UPDATE, "%s requested player list in room -1",
			player->name);
		if (net_send_player_list_error(player->client->net, E_NOT_IN_ROOM) < 0)
			return GGZ_REQ_DISCONNECT;
		return GGZ_REQ_FAIL;
	}

	/* Grab room number */
	pthread_rwlock_rdlock(&player->lock);
	room = player->room;
	pthread_rwlock_unlock(&player->lock);

	pthread_rwlock_rdlock(&rooms[room].lock);
	count = rooms[room].player_count;
	/* FIXME: data should be allocated on the stack, not in the heap. */
	data = ggz_malloc(count * sizeof(GGZPlayer));
	for (i = 0; i < count; i++) {
		p = rooms[room].players[i];
		pthread_rwlock_rdlock(&p->lock);
		/* Does this copy all applicable player data? */
		data[i] = *p;
		pthread_rwlock_unlock(&p->lock);
	}
	pthread_rwlock_unlock(&rooms[room].lock);

	if (net_send_player_list_count(player->client->net, count) < 0) {
		ggz_free(data);
		return GGZ_REQ_DISCONNECT;
	}

	for (i = 0; i < count; i++)
		if (net_send_player(player->client->net, &data[i]) < 0) {
			ggz_free(data);
			return GGZ_REQ_DISCONNECT;
		}

	if (net_send_player_list_end(player->client->net) < 0) {
		ggz_free(data);
		return GGZ_REQ_DISCONNECT;
	}

	ggz_free(data);
	return GGZ_REQ_OK;
}


GGZPlayerHandlerStatus player_list_types(GGZPlayer* player, char verbose)
{
	int i, max, count = 0;
	GameInfo info[MAX_GAME_TYPES];

	dbg_msg(GGZ_DBG_UPDATE, "Handling type list request for %s", 
		player->name);
		
	/* Don't send list if they're not logged in */
 	if (player->login_status == GGZ_LOGIN_NONE) {
		dbg_msg(GGZ_DBG_UPDATE, "%s requested type list before login",
			player->name);
		if (net_send_type_list_error(player->client->net, E_NOT_LOGGED_IN) < 0)
 			return GGZ_REQ_DISCONNECT;
 		return GGZ_REQ_FAIL;
 	}

	pthread_rwlock_rdlock(&state.lock);
	max = state.types;
	pthread_rwlock_unlock(&state.lock);
	
	for (i = 0; (i < MAX_GAME_TYPES && count < max); i++) {
		pthread_rwlock_rdlock(&game_types[i].lock);
		info[count++] = game_types[i];
		pthread_rwlock_unlock(&game_types[i].lock);
	}
		
	/* Send game type count */
	if (net_send_type_list_count(player->client->net, count) < 0)
		return GGZ_REQ_DISCONNECT;

	for (i = 0; i < count; i++)
		if (net_send_type(player->client->net, i, &info[i], verbose) < 0)
			return GGZ_REQ_DISCONNECT;
	
	if (net_send_type_list_end(player->client->net) < 0)
		return GGZ_REQ_DISCONNECT;


	return GGZ_REQ_OK;
}


GGZPlayerHandlerStatus player_list_tables(GGZPlayer* player, int type,
                                          char global)
{
	GGZTable *my_tables;
	int count, i;
	
	dbg_msg(GGZ_DBG_UPDATE, "Handling table list request for %s", 
		player->name);
	
	
	/* Don`t send list if they`re not logged in */
	if (player->login_status == GGZ_LOGIN_NONE) {
		dbg_msg(GGZ_DBG_UPDATE, "%s requested table list before login",
			player->name);
		if (net_send_table_list_error(player->client->net, E_NOT_LOGGED_IN) < 0)
			return GGZ_REQ_DISCONNECT;
		return GGZ_REQ_FAIL;
	}
	
	/* Don't send list if they're not in a room */
	if (player->room == -1) {
		if (net_send_table_list_error(player->client->net, E_NOT_IN_ROOM) < 0)
			return GGZ_REQ_DISCONNECT;
		return GGZ_REQ_FAIL;
	}

	count = table_search(player->name, player->room, type, global, 
			     &my_tables);
	
	if (net_send_table_list_count(player->client->net, count) < 0)
		return GGZ_REQ_DISCONNECT;
	
	/* Don`t proceed if there was an error, or no tables found*/
	if (count < 0)
		return GGZ_REQ_FAIL;
	
	for (i = 0; i < count; i++)
		if (net_send_table(player->client->net, &my_tables[i]) < 0)
			return GGZ_REQ_DISCONNECT;


	if (net_send_table_list_end(player->client->net) < 0)
		return GGZ_REQ_DISCONNECT;

	if (count > 0)
		ggz_free(my_tables);
	
	return GGZ_REQ_OK;
}


int player_chat(GGZPlayer* player, unsigned char subop, char *target, char *msg)
{
	int target_room=-1;	/* FIXME - this should come from net.c if we */
				/* are going to support per-room announce... */
	int status;

	dbg_msg(GGZ_DBG_CHAT, "Handling chat for %s", player->name);
	
	/* Verify that we are in a regular room */
	/* No lock needed: no one can change our room but us */
	if (player->room == -1) {
		dbg_msg(GGZ_DBG_CHAT, "%s tried to chat from room -1", 
			player->name);
		if (net_send_chat_result(player->client->net, E_NOT_IN_ROOM) < 0)
			return GGZ_REQ_DISCONNECT;
	}
	
	/* Parse subop */
	switch (subop) {
	case GGZ_CHAT_NORMAL:
		dbg_msg(GGZ_DBG_CHAT, "%s sends %s", player->name, msg);
		status = chat_room_enqueue(player->room, subop, player, msg);
		break;
	case GGZ_CHAT_BEEP:
	case GGZ_CHAT_PERSONAL:
		status = chat_player_enqueue(target, subop, player, msg);
		break;
	case GGZ_CHAT_ANNOUNCE:
		dbg_msg(GGZ_DBG_CHAT, "%s announces %s", player->name, msg);
		status = chat_room_enqueue(target_room, subop, player, msg);
		break;
	default:
		dbg_msg(GGZ_DBG_PROTOCOL, "%s sent invalid chat subop %d", 
			player->name, subop);
		status = E_BAD_OPTIONS;
	}

	dbg_msg(GGZ_DBG_CHAT, "%s's chat result: %d", player->name, status);
	if (net_send_chat_result(player->client->net, status) < 0)
		return GGZ_REQ_DISCONNECT;
	
	/* Don't return the chat error code */
	return 0;
}


GGZPlayerHandlerStatus player_motd(GGZPlayer* player)
{
	dbg_msg(GGZ_DBG_CHAT, "Handling motd request for %s", player->name);

  	if (!motd_is_defined())
		return GGZ_REQ_OK;

 	/* Don't send motd if they're not logged in */
 	if (player->login_status == GGZ_LOGIN_NONE) {
		dbg_msg(GGZ_DBG_CHAT, "%s requested motd before logging in",
			player->name);
		if (net_send_motd_error(player->client->net, E_NOT_LOGGED_IN) < 0)
 			return GGZ_REQ_DISCONNECT;
 		return GGZ_REQ_FAIL;
 	}

	if (net_send_motd(player->client->net) < 0)
		return GGZ_REQ_DISCONNECT;
	
	return GGZ_REQ_OK;
}


GGZPlayerHandlerStatus player_send_ping(GGZPlayer *player)
{
	/* Send a ping and mark send time */
	if(net_send_ping(player->client->net) < 0)
		return GGZ_REQ_DISCONNECT;
	gettimeofday(&player->sent_ping, NULL);

	/* Set next_ping to zero so we won't ping until we've got a reply */
	player->next_ping = 0;

	return GGZ_REQ_OK;
}


/* Determine time from sent_ping to now, in msec */
static int player_get_time_since_ping(GGZPlayer *player)
{
	struct timeval tv;

	gettimeofday(&tv, NULL);
	return ((tv.tv_sec - player->sent_ping.tv_sec) * 1000)
	       + ((tv.tv_usec - player->sent_ping.tv_usec) / 1000);
}


void player_handle_pong(GGZPlayer *player)
{
	int msec, lag_class;
	int i;
	
	msec = player_get_time_since_ping(player);

	lag_class = 1;
	for(i=0; i<4; i++) {
		if(msec >= opt.lag_class[i])
			lag_class++;
		else
			break;
	}

	if(lag_class != player->lag_class) {
		player->lag_class = lag_class;
		if(player->room >= 0)
			room_notify_lag(player->name, player->room);
	}

	/* Queue our next ping */
	player->next_ping = time(NULL) + opt.ping_freq;
}
