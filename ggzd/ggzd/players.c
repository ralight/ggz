/*
 * File: players.c
 * Author: Brent Hendricks
 * Project: GGZ Server
 * Date: 10/18/99
 * Desc: Functions for handling players
 * $Id: players.c 10780 2009-01-02 13:02:26Z oojah $
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */

#ifdef HAVE_CONFIG_H
# include <config.h>		/* Site specific config */
#endif

#include <arpa/inet.h>
#include <ctype.h>
#include <errno.h>
#include <netdb.h>
#include <ggz_threads.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>

#include <ggz.h>

#include "chat.h"
#include "client.h"
#include "datatypes.h"
#include "err_func.h"
#include "ggzd.h"
#include "hash.h"
#include "motd.h"
#include "net.h"
#include "perms.h"
#include "players.h"
#include "protocols.h"
#include "room.h"
#include "seats.h"
#include "table.h"
#include "transit.h"
#include "stats.h"


pthread_key_t player_key;


/* Local functions for handling players */
static GGZClientReqError player_transit(GGZPlayer* player,
					GGZTransitType opcode,
					char *caller,
					const int table_index,
					const int seat_index,
					int reason);
static GGZPlayerHandlerStatus player_send_room_update(GGZPlayer *player);
static GGZPlayerHandlerStatus player_send_ping(GGZPlayer *player);
static ggztime_t player_get_time_since_ping(GGZPlayer *player);


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
	player->launching = false;
	player->transit = false;
	player->room = -1;
	player->login_status = GGZ_LOGIN_NONE;
	ggz_perms_init_from_list(&player->perms,
			     perms_default_anon, num_perms_default_anon);
	strcpy(player->name, "<none>");
	player->room_events = NULL;
	player->my_events_head = NULL;
	player->my_events_tail = NULL;
	player->lag_class = 1;			/* Assume they are low lag */

	/* Don't ping until login */
	player->is_ping_sent = false;
	player->next_ping_time = 0.0;
	player->sent_ping_time = 0.0;

	/* Don't update until login. */
	player->is_room_update_needed = false;
	player->next_room_update_time = 0.0;

	pthread_rwlock_init(&player->stats_lock, NULL);
	player->wins = -1;
	player->losses = -1;
	player->ties = -1;
	player->rating = -1;
	player->highscore = -1;

	player->gagged = false;

	return player;
}

/**
 * Free a player structure.  The player should already have been logged out.
 *
 * @param player A pointer to the player structure.
 */
void player_free(GGZPlayer *player)
{
	ggz_free(player);
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
	bool anon = false;

	pthread_rwlock_rdlock(&player->lock);
	/* There's no need to remove them if they aren't "here" */
	if (strcasecmp(player->name, "<none>") != 0) {
		connect_time = (long)time(NULL) - player->login_time;
		if(player->login_status == GGZ_LOGIN_ANON)
			anon = true;
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

	ggz_debug(GGZ_DBG_CONNECTION, "Logging out %s", player->name);

	/* Need to leave table if we're at one */
	/* FIXME: is this the correct way to do it? */
	/* Note: GGZ_TRANSIT_LEAVE should work for spectators too. */
	if (player->room >= 0 && player->table >= 0)
		player_transit(player, GGZ_TRANSIT_LEAVE, player->name,
			       player->table, -1, GGZ_LEAVE_NORMAL);

	/* Remove us from room, so we get no new events */
	if (player->room != -1)
		room_join(player, -1);
	
	/* FIXME: is this the right place for this? */
	event_player_flush(player);

	/* Close channel if it's open */
	if (player->game_fd != -1) {
		ggz_debug(GGZ_DBG_TABLE, "Closing leftover channel for %s", 
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
GGZReturn player_updates(GGZPlayer* player)
{
	bool changed = false;

	/* There's a bit of a problem here: because the room events will all
	   be in order, and the player events will all be in order, but
	   there's no way to make sure they're collectively in order.  Given
	   that, it's safest to do the room events first (so that, say, a
	   player doesn't join a table that they don't even know has been
	   launched yet).  --JDS */

	/* Process room events. */
	if (player->room != -1 && event_room_handle(player) < 0)
		return GGZ_ERROR;
	
	/* Process personal events */
	if (player->my_events_head)
		if (event_player_handle(player) < 0)
			return GGZ_ERROR;

	/* Lowest priority update - send a PING / check lag */
	if (!player->is_ping_sent
	    && get_current_time() >= player->next_ping_time) {
		if(player_send_ping(player) < 0)
			return GGZ_ERROR;
	} else if (player->is_ping_sent) {
		/* We're waiting for a PONG, but if we wait too long
		   we should increase the lag class without waiting */
		while (player->lag_class < 5
		       && (player_get_time_since_ping(player)
			   >= opt.lag_class_time[player->lag_class - 1])) {
			player->lag_class++;
			changed = true;
		}
		
		if (changed)
			room_update_event(player->name, GGZ_PLAYER_UPDATE_LAG,
					  player->room, -1);
	}

	if (player->is_room_update_needed
	    && get_current_time() >= player->next_room_update_time) {
		if (player_send_room_update(player) < 0) {
			return GGZ_ERROR;
		}
		player->next_room_update_time += opt.room_update_freq;
	}

	return GGZ_OK;
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
	GGZPlayerType type;

	/* The caller should have already acquired a read lock.  Acquiring
	   one now wouldn't do much good anyway, since the read lock is
	   needed to protect the pointer itself... */

	if (player->login_status == GGZ_LOGIN_ANON) {
		type = GGZ_PLAYER_GUEST;
	} else if (ggz_perms_is_bot(player->perms)) {
		/* Check for a bot first, because bots may have admin
		 * capabilities as well */
		type = GGZ_PLAYER_BOT;
	} else if (ggz_perms_is_admin(player->perms)) {
		type = GGZ_PLAYER_ADMIN;
	} else if (ggz_perms_is_host(player->perms)) {
		type = GGZ_PLAYER_HOST;
	} else {
		type = GGZ_PLAYER_NORMAL;
	}

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
	GGZClientReqError status;

	ggz_debug(GGZ_DBG_TABLE, "Handling table launch for %s", player->name);

	/* Check permissions */
	if (!perms_check(player, GGZ_PERM_LAUNCH_TABLE)) {
		ggz_debug(GGZ_DBG_TABLE, "%s insufficient perms to launch",
			player->name);
		if(net_send_table_launch(player->client->net,
					 E_NO_PERMISSION) < 0)
			return GGZ_REQ_DISCONNECT;
		return GGZ_REQ_FAIL;
	}

	/* Check if in a room */
	if ( (room = player->room) == -1) {
		ggz_debug(GGZ_DBG_TABLE, "%s tried to launch table in room -1", 
			player->name);
		if (net_send_table_launch(player->client->net, E_NOT_IN_ROOM) < 0)
			return GGZ_REQ_DISCONNECT;
		return GGZ_REQ_FAIL;
	}

	/* We used to check for _max_ table size here. Now there is no
	 * max table size. */
	/* Shouldn't some check be done?  Note that the number of players is
	   checked against the player_allow_list later.  --JDS */
	
	/* Don't allow multiple table launches */
	if (player->table != -1 || player->launching) {
		ggz_debug(GGZ_DBG_TABLE, "%s tried to launch table while at one",
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
		ggz_debug(GGZ_DBG_TABLE, "%s tried to launch wrong table type",
			player->name);
		if (net_send_table_launch(player->client->net, E_NOT_IN_ROOM) < 0)
			return GGZ_REQ_DISCONNECT;
		return GGZ_REQ_FAIL;
	}

	/* Currently a <LAUNCH> implies an immediate <JOIN> as well.  So you
	   need to have a connection already established. */
	if (player->game_fd < 0) {
		ggz_debug(GGZ_DBG_TABLE,
			"%s tries to launch table without connection",
			player->name);
		if (!perms_check(player, GGZ_PERM_ROOMS_ADMIN)) {
			if (net_send_table_launch(player->client->net,
						  E_NO_CHANNEL) < 0)
				return GGZ_REQ_DISCONNECT;
			return GGZ_REQ_FAIL;
		}
	}

	/* Do actual launch of table */
	status = table_launch(table, player->name);
	
	if (status == E_OK) {
		/* Mark player as launching table so we can't launch more */
		pthread_rwlock_wrlock(&player->lock);
		player->launching = true;
		pthread_rwlock_unlock(&player->lock);
	} else {
		if (net_send_table_launch(player->client->net, status) < 0)
			return GGZ_REQ_DISCONNECT;
		return GGZ_REQ_FAIL;
	}
	
	return GGZ_REQ_OK;
}


GGZEventFuncReturn player_launch_callback(void* target, size_t size,
					  void* data)
{
	GGZLaunchEventData *event = data;
	GGZPlayer* player = (GGZPlayer*)target;

	/* Launch completed */
	pthread_rwlock_wrlock(&player->lock);
	player->launching = false;
	pthread_rwlock_unlock(&player->lock);

	ggz_debug(GGZ_DBG_TABLE, "%s launch result: %d",
		player->name, event->status);
	
	/* Automatically join newly created table */
	if (event->status == 0)
		player_transit(player, GGZ_TRANSIT_JOIN, player->name,
			       event->table_index, -1, GGZ_JOIN_LAUNCH);

	/* Return status to client */
	if (net_send_table_launch(player->client->net, event->status) < 0)
		return GGZ_EVENT_ERROR;
	
	return GGZ_EVENT_OK;
}

/* Check if the player has permissions to modify the table.  This will need
   to be more extensive later.  Return the table (with write-lock) if they
   have permissions, otherwise NULL. */
static GGZTable *check_table_perms(GGZPlayer *player,
				   int room_id, int table_id)
{
	GGZTable *table;

	/* Check permissions */
	table = table_lookup(room_id, table_id);
	if (!table)
		return NULL;

	/* perms_check does a read-lock on the player.  This will succeed
	 * since we are the player thread. */
	if (perms_check(player, GGZ_PERM_EDIT_TABLES)) {
		return table;
	}

	if (strcasecmp(table->owner, player->name) != 0) {
		pthread_rwlock_unlock(&table->lock);
		ggz_debug(GGZ_DBG_TABLE, "%s tried to modify bad table",
			player->name);
		return NULL;
	}

	return table;
}


GGZPlayerHandlerStatus player_table_desc_update(GGZPlayer* player,
						int room_id, int table_id,
						const char *desc)
{
	GGZTable *table = check_table_perms(player, room_id, table_id);
	if (!table) {
		if(net_send_update_result(player->client->net,
					  E_NO_PERMISSION) < 0)
			return GGZ_REQ_DISCONNECT;
		return GGZ_REQ_FAIL;
	}
	pthread_rwlock_unlock(&table->lock);

	/* FIXME: this is buggy for two reasons.  (1) Since we don't have
	   a lock on the table at this point, it is possible for the
	   table to be deallocated by the table thread, causing a server
	   segfault (at best).  (2) We (the player thread) should not be
	   accessing and using a write lock on the table's description -
	   the new desc should instead be sent to the table thread to
	   do this. */
	if (desc)
		table_set_desc(table, desc);

	if(net_send_update_result(player->client->net, E_OK) < 0)
		return GGZ_REQ_DISCONNECT;
	return GGZ_REQ_OK;
}

/* Called by the network code when the client sends an <UPDATE> for the
   table seat.  This can be used for instance to toggle bot and open
   seats. */
GGZPlayerHandlerStatus player_table_seat_update(GGZPlayer *player,
						int room_id, int table_id,
						GGZTableSeat *seat)
{
	bool allow;
	GGZTable *table = check_table_perms(player, room_id, table_id);
	GGZNumberList bots_allowed;

	if (!table) {
		if(net_send_update_result(player->client->net,
					  E_NO_PERMISSION) < 0)
			return GGZ_REQ_DISCONNECT;
		return GGZ_REQ_FAIL;
	}

	pthread_rwlock_rdlock(&game_types[table->type].lock);
	allow = game_types[table->type].allow_leave;
	bots_allowed = game_types[table->type].bot_allow_list;
	pthread_rwlock_unlock(&game_types[table->type].lock);
	pthread_rwlock_unlock(&table->lock);

	/* Check if adding a bot player is allowed. */
	if (allow && seat->type == GGZ_SEAT_BOT) {
		const int bots = seats_count(table, GGZ_SEAT_BOT);

		if (!ggz_numberlist_isset(&bots_allowed, bots + 1)) {
			allow = false;
		}
	}

	/* FIXME: I think this may be a significiant security hole, as we do
	   not do full checking of the seat change here and it is not
	   checked at all later that I can see. --jdorje */

	if (!allow) {
		if(net_send_update_result(player->client->net,
					  E_LEAVE_FORBIDDEN) < 0)
			return GGZ_REQ_DISCONNECT;
		return GGZ_REQ_FAIL;
	}

	ggz_debug(GGZ_DBG_TABLE,
		"%s requested seat %d on table %d/%d become %s",
		player->name, seat->index, table->room, table->index,
		ggz_seattype_to_string(seat->type));

	/* This function is buggy, just like player_table_desc_update */

	if (transit_seat_event(table->room, table->index,
			       GGZ_TRANSIT_SEAT,
			       *seat, player->name, -1) != GGZ_OK) {
		if(net_send_update_result(player->client->net,
					  E_NO_TABLE) < 0)
			return GGZ_REQ_DISCONNECT;
		return GGZ_REQ_FAIL;
	}

	if(net_send_update_result(player->client->net, E_OK) < 0)
		return GGZ_REQ_DISCONNECT;
	return GGZ_REQ_OK;
}

GGZPlayerHandlerStatus player_table_boot_update(GGZPlayer *player,
						int room_id, int table_id,
						const char *name)
{
	GGZPlayer *them;
	GGZClientReqError status;
	bool allow;

	GGZTable *table = check_table_perms(player, room_id, table_id);
	if (!table) {
		if(net_send_update_result(player->client->net,
					  E_NO_PERMISSION) < 0)
			return GGZ_REQ_DISCONNECT;
		return GGZ_REQ_FAIL;
	}

	/* This function is buggy, just like player_table_desc_update */

	pthread_rwlock_rdlock(&game_types[table->type].lock);
	allow = game_types[table->type].allow_leave;
	pthread_rwlock_unlock(&game_types[table->type].lock);
	pthread_rwlock_unlock(&table->lock);

	ggz_debug(GGZ_DBG_TABLE,
		"%s tried to boot %s from table %d in room %d",
		player->name, name, table_id, room_id);

	if (!allow) {
		if(net_send_update_result(player->client->net,
					  E_LEAVE_FORBIDDEN) < 0)
			return GGZ_REQ_DISCONNECT;
		return GGZ_REQ_FAIL;
	}

	them = hash_player_lookup(name);
	if (!them || them->room != room_id || them->table != table_id) {
		if (them)
			pthread_rwlock_unlock(&them->lock);
		if (net_send_update_result(player->client->net,
					   E_BAD_OPTIONS) < 0)
			return GGZ_REQ_DISCONNECT;
		return GGZ_REQ_FAIL;
	}
	pthread_rwlock_unlock(&them->lock);
	/* FIXME: player lock is lost... */
	status = player_transit(them, GGZ_TRANSIT_LEAVE,
				player->name,
				table_id, -1, GGZ_LEAVE_BOOT);

	if (status != E_OK) {
		if (net_send_update_result(player->client->net, status) < 0)
			return GGZ_REQ_DISCONNECT;
		return GGZ_REQ_FAIL;
	}

	if(net_send_update_result(player->client->net, E_OK) < 0)
		return GGZ_REQ_DISCONNECT;
	return GGZ_REQ_OK;
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
GGZPlayerHandlerStatus player_table_join(GGZPlayer* player,
					 int table_index,
					 int seat_num)
{
	GGZClientReqError status;

	ggz_debug(GGZ_DBG_TABLE, "Handling table join for %s", player->name);

	ggz_debug(GGZ_DBG_TABLE, "%s attempting to join table %d in room %d", 
		player->name, table_index, player->room);

	if (player->table != -1)
		status = E_AT_TABLE;
	else if (player->transit)
		status = E_IN_TRANSIT;
	else if (!perms_check(player, GGZ_PERM_JOIN_TABLE))
		status = E_NO_PERMISSION;
	else if (player->game_fd < 0) {
		/* You have to establish the channel (direct connection)
		   BEFORE sending the <JOIN> request. */
		status = E_NO_CHANNEL;
	} else /* Send a join event to the table */
		status = player_transit(player, GGZ_TRANSIT_JOIN,
					player->name,
					table_index, seat_num,
					GGZ_JOIN_REQUEST);

	/* Return any immediate failures to client*/
	if (status != E_OK) {
		if (net_send_table_join_result(player->client->net,
					       status) != GGZ_OK)
			return GGZ_REQ_DISCONNECT;
		return GGZ_REQ_FAIL;
	}
	
	return GGZ_REQ_OK;
}

GGZPlayerHandlerStatus player_table_join_spectator(GGZPlayer* player, int table_id)
{
	GGZClientReqError status;

	ggz_debug(GGZ_DBG_TABLE, "Handling table join (as spectator) for %s", player->name);

	ggz_debug(GGZ_DBG_TABLE, "%s attempting to join (as spectator) table %d in room %d", 
		  player->name, table_id, player->room);

	if (player->table != -1)
		status = E_AT_TABLE;
	else if (player->transit)
		status = E_IN_TRANSIT;
	else if (!perms_check(player, GGZ_PERM_JOIN_TABLE))
		status = E_NO_PERMISSION;
	else /* Send a join event to the table */
		status = player_transit(player, GGZ_TRANSIT_JOIN_SPECTATOR,
					player->name,
					table_id, -1, GGZ_JOIN_REQUEST);

	/* Return any immediate failures to client*/
	if (status != E_OK) {
		if (net_send_table_join_result(player->client->net,
					       status) != GGZ_OK)
			return GGZ_REQ_DISCONNECT;
		return GGZ_REQ_FAIL;
	}
	
	return GGZ_REQ_OK;
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
	int gametype;
	GGZClientReqError status;
	bool allow;
	GGZTableState tablestate;
	GGZTable *table;
	GGZTransitType transit;

	/* Check if leave during gameplay is allowed */
	table = table_lookup(player->room, player->table);

	if (!table) {
		ggz_debug(GGZ_DBG_TABLE,
			"%s tried to leave nonexistent table %d (room %d)",
			player->name, player->table, player->room);
		if (player->room >= 0 && player->table >= 0) {
			ggz_error_msg("Couldn't find %s's table %d (room %d)!",
				player->name, player->table, player->room);
			player->table = -1; /* What else can we do? */
		}
		/* This is probably an error on the client's part.  But
		   to avoid confusion, the best bet is probably to
		   claim success. */
		if (net_send_table_leave_result(player->client->net,
						E_OK) != GGZ_OK)
			return GGZ_REQ_DISCONNECT;
		if (net_send_table_leave(player->client->net,
					 GGZ_LEAVE_NORMAL, NULL) != GGZ_OK)
			return GGZ_REQ_DISCONNECT;
		return GGZ_REQ_OK;
	}		

	if (spectator) {
		pthread_rwlock_unlock(&table->lock);

		/* FIXME: make sure the player really is a spectator!
		   Otherwise the table could break! */
		allow = true;
		tablestate = GGZ_TABLE_WAITING;

		transit = GGZ_TRANSIT_LEAVE_SPECTATOR;
	} else {
		gametype = table->type;
		tablestate = table->state;
		pthread_rwlock_unlock(&table->lock);

		/* FIXME: make sure the player isn't a spectator!  Otherwise
		   we could up killing the table when a spectator leaves! */
		pthread_rwlock_rdlock(&game_types[gametype].lock);
		allow = game_types[gametype].allow_leave;
		pthread_rwlock_unlock(&game_types[gametype].lock);

		transit = GGZ_TRANSIT_LEAVE;
	}

	ggz_debug(GGZ_DBG_TABLE, "%s attempting to leave table %d%s", 
		player->name, player->table,
		spectator ? " (spectating)" : (force ? " (forced)" : ""));

	/* Error if we're already in transit */
	if (player->transit)
		status = E_IN_TRANSIT;
	else if (!spectator && tablestate == GGZ_TABLE_PLAYING && !allow) {
		if (force)
			status = table_kill(player->room, player->table,
					    player->name);
		else 
			status = E_LEAVE_FORBIDDEN;
	} else { 
		/* All clear: send leave event to table */
		status = player_transit(player, transit, player->name,
					player->table, -1, GGZ_LEAVE_NORMAL);
	}

	/* Return any immediate failures to client*/
	if (status != E_OK) {
		if (net_send_table_leave_result(player->client->net,
					 status) != GGZ_OK)
			return GGZ_REQ_DISCONNECT;
		return GGZ_REQ_FAIL;
	}

	return GGZ_REQ_OK;
}


GGZPlayerHandlerStatus player_table_reseat(GGZPlayer *player,
					   GGZReseatType opcode,
					   int seat_num)
{
	GGZTable *table;
	int gametype;
	bool allow;
	GGZClientReqError status;
	GGZTransitType action;

	switch (opcode) {
	case GGZ_RESEAT_SIT:
		action = GGZ_TRANSIT_SIT;
		break;
	case GGZ_RESEAT_STAND:
		action = GGZ_TRANSIT_STAND;
		break;
	case GGZ_RESEAT_MOVE:
		action = GGZ_TRANSIT_MOVE;
		break;
	default:
		ggz_error_msg("player_table_reseat: bad opcode %d.", opcode);
		return GGZ_REQ_OK;
	}

	table = table_lookup(player->room, player->table);
	if (!table) {
		ggz_debug(GGZ_DBG_TABLE,
			"%s tried to reseat at nonexistent table %d:%d.",
			player->name, player->room, player->table);
		if (player->room >= 0 && player->table >= 0) {
			ggz_error_msg("Couldn't find %s's table %d (room %d)!",
				player->name, player->table, player->room);
			player->table = -1; /* What else can we do? */
		}
		/* This is probably an error on the client's part.  But
		   to avoid confusion, the best bet is probably to
		   claim success. */
		if (net_send_table_leave(player->client->net,
					 GGZ_LEAVE_GAMEERROR, NULL) != GGZ_OK)
			return GGZ_REQ_DISCONNECT;
		return GGZ_REQ_OK;
	}

	gametype = table->type;
	pthread_rwlock_unlock(&table->lock);

	pthread_rwlock_rdlock(&game_types[gametype].lock);
	allow = game_types[gametype].allow_leave;
	pthread_rwlock_unlock(&game_types[gametype].lock);

	if (player->transit)
		status = E_IN_TRANSIT;
	else if (!allow)
		status = E_LEAVE_FORBIDDEN;
	else
		status = player_transit(player, action, player->name,
					player->table, seat_num, -1);

	/* Return any immediate failures to client*/
	if (status != E_OK) {
		if (net_send_reseat_result(player->client->net, status) < 0)
			return GGZ_REQ_DISCONNECT;
		return GGZ_REQ_FAIL;
	}

	return GGZ_REQ_OK;
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
	GGZTableState tablestate;
	GGZTable *table;

	/* Check if leave during gameplay is allowed */
	table = table_lookup(player->room, player->table);

	if (!table) {
		ggz_debug(GGZ_DBG_TABLE,
			"%s tried to leave nonexistent table %d (room %d)",
			player->name, player->table, player->room);
		/* This is probably an error on the client's part.  But
		   to avoid confusion, the best bet is probably to
		   claim success. */
		if (net_send_table_leave_result(player->client->net,
						E_OK) != GGZ_OK)
			return GGZ_REQ_DISCONNECT;
		if (net_send_table_leave(player->client->net,
					 GGZ_LEAVE_NORMAL, NULL) != GGZ_OK)
			return GGZ_REQ_DISCONNECT
		return GGZ_REQ_OK;
	}		

	ggz_debug(GGZ_DBG_TABLE, "%s attempting to leave table %d as spectator",
		player->name, player->table);

	gametype = table->type;
	tablestate = table->state;
	pthread_rwlock_unlock(&table->lock);

	/* Error if we're already in transit */
	if (player->transit)
		status = E_IN_TRANSIT;
	else 
		/* All clear: send leave event to table */
		status = player_transit(player, GGZ_TRANSIT_LEAVE_SPECTATOR,
					player->name,
					player->table, GGZ_LEAVE_NORMAL);

	/* Return any immediate failures to client*/
	if (status != E_OK) {
		if (net_send_table_leave(player->client->net,
					 status) != GGZ_OK)
			return GGZ_REQ_DISCONNECT;
		return GGZ_REQ_FAIL;
	}

	return GGZ_REQ_OK;
}
#endif


GGZPlayerHandlerStatus player_table_info(GGZPlayer *player, int seat_num)
{
	GGZTable *table;
	GameInfo gametype;
	int i, do_send;
	int first, last;
	const char *realname, *photo, *host;
	int allow_hostname;
	ggzdbPlayerEntry entry;
	ggzdbPlayerExtendedEntry extentry;
	GGZDBResult status;

	table = table_lookup(player->room, player->table);
	if (!table) {
		ggz_debug(GGZ_DBG_TABLE,
			"%s tried to get player info about non-existing table %d:%d.",
			player->name, player->room, player->table);
		return GGZ_REQ_FAIL;
	}

	first = 0;
	last = table->num_seats;
	if (seat_num > -1) {
		first = seat_num;
		last = seat_num + 1;
	}

	net_send_info_list_begin(player->client->net);

	for (i = first; i < last; i++) {
		if (table->seat_types[i] == GGZ_SEAT_PLAYER) {
			ggz_strncpy(entry.handle, player->name, sizeof(entry.handle));
			status = ggzdb_player_get(&entry);

			do_send = 1;
			realname = NULL;
			photo = NULL;
			host = NULL;

			if (status == GGZDB_NO_ERROR) {
				realname = ggz_strdup(entry.name);

				ggz_strncpy(extentry.handle, player->name, sizeof(extentry.handle));
				status = ggzdb_player_get_extended(&extentry);
				if (status == GGZDB_NO_ERROR) {
					photo = ggz_strdup(extentry.photo);
				}
			} else {
				do_send = 0;
			}

			allow_hostname = 0;

			pthread_rwlock_rdlock(&game_types[i].lock);
			gametype = game_types[table->type];
			allow_hostname = gametype.allow_peers;
			pthread_rwlock_unlock(&game_types[i].lock);

			if (allow_hostname) {
				host = ggz_getpeername(net_get_fd(player->client->net), 0);
				do_send = 1;
			}

			if (do_send) {
				net_send_info(player->client->net, i,
					realname, photo, host);

			}

			if(realname)
				ggz_free(realname);
			if(photo)
				ggz_free(photo);
			if(host)
				ggz_free(host);
		}
	}
	
	net_send_info_list_end(player->client->net);

	pthread_rwlock_unlock(&table->lock);

	return GGZ_REQ_OK;
}


GGZPlayerHandlerStatus player_table_rankings(GGZPlayer *player)
{
	GGZTable *table;
	GGZListEntry *entry;

	table = table_lookup(player->room, player->table);
	if (!table) {
		ggz_debug(GGZ_DBG_TABLE,
			"%s tried to get player rankings on non-existing table %d:%d.",
			player->name, player->room, player->table);
		return GGZ_REQ_FAIL;
	}

	net_send_rankings_list_begin(player->client->net);

	GGZList *rankings = toprankings(table->type);
        for (entry = ggz_list_head(rankings); entry; entry = ggz_list_next(entry)) {
		GGZRanking *ranking = ggz_list_get_data(entry);
		net_send_rankings(player->client->net, ranking->position, ranking->name, ranking->score);
		ggz_free(ranking->name);
	}
	ggz_list_free(rankings);

	net_send_rankings_list_end(player->client->net);

	pthread_rwlock_unlock(&table->lock);

	return GGZ_REQ_OK;
}


static void find_player_at_table(GGZPlayer *player,
				 int table_index,
				 int *spectating,
				 int *seat_num)
{
	int num;

	num = table_find_player(player->room, table_index, player->name);
	if (num >= 0) {
		*spectating = 0;
		*seat_num = num;
		return;
	}

	num = table_find_spectator(player->room, table_index, player->name);
	if (num >= 0) {
		*spectating = 1;
		*seat_num = num;
		return;
	}

	*seat_num = -1;
}


static GGZClientReqError player_transit(GGZPlayer* player,
					GGZTransitType opcode,
					char *caller,
					const int table_index,
					const int seat_index,
					int reason)
{
	struct GGZTableSeat seat;
	GGZReturn status = GGZ_ERROR;
	int spectating, seat_num;

	/* Do some quick sanity checking */
	if (player->room == -1) 
		return E_NOT_IN_ROOM;
	if (table_index == -1)
		return E_NO_TABLE;

	ggz_debug(GGZ_DBG_UPDATE, "player_transit(%s, %d, %d)",
		player->name, opcode, table_index);

	/* Implement LEAVE by setting my seat to open */
	switch (opcode) {
	case GGZ_TRANSIT_LEAVE: 
	case GGZ_TRANSIT_LEAVE_SPECTATOR:
		/* At this point, whether we're spectating or not is
		   determined by what the client told us.  But we can't
		   trust the client, so we double-check. */
		find_player_at_table(player, table_index,
				     &spectating, &seat_num);

		if (seat_num < 0) {
			/* Uh oh.  A ggzd bug.  But it doesn't have
			   to be fatal. */
			ggz_error_msg("%s couldn't be found at table %d (room %d).",
				player->name, player->table, player->room);
			/* Fake success, so that the client doesn't get
			   confused. */
			net_send_table_leave_result(player->client->net, E_OK);
			net_send_table_leave(player->client->net,
					     GGZ_LEAVE_NORMAL, NULL);
			/* Remove the player from the table.  Perhaps we
			   should see if they're actually at a different
			   table?  Or perhaps we should send a full update? */
			player->table = -1;
			return E_OK;
		}

		if (spectating)
			opcode = GGZ_TRANSIT_LEAVE_SPECTATOR;
		else
			opcode = GGZ_TRANSIT_LEAVE;

		seat.index = seat_num;
		seat.type = GGZ_SEAT_OPEN;
		seat.name[0] = '\0';
		seat.fd = -1;

		status = transit_seat_event(player->room, table_index,
					    opcode, seat, caller,
					    reason);
		break;
	case GGZ_TRANSIT_JOIN:
		seat.index = seat_index;
		if (seat.index < 0) {
			/* Take first available seat */
			seat.index = GGZ_SEATNUM_ANY;
		}
		seat.type = GGZ_SEAT_PLAYER;
		seat.fd = player->game_fd;
		strcpy(seat.name, player->name);
		
		status = transit_seat_event(player->room, table_index,
					    opcode, seat,
					    caller,
					    reason);
		/* Now that channel fd has been sent, rest it here */
		pthread_rwlock_wrlock(&player->lock);
		player->game_fd = -1;
		pthread_rwlock_unlock(&player->lock);

		break;
	case GGZ_TRANSIT_JOIN_SPECTATOR:
		seat.index = GGZ_SEATNUM_ANY;
		seat.fd = player->game_fd;
		strcpy(seat.name, player->name);

		status = transit_seat_event(player->room, table_index,
					    opcode, seat, caller,
					    reason);
		/* Now that channel fd has been sent, rest it here */
		pthread_rwlock_wrlock(&player->lock);
		player->game_fd = -1;
		pthread_rwlock_unlock(&player->lock);

		break;
	case GGZ_TRANSIT_STAND:
	case GGZ_TRANSIT_SIT:
	case GGZ_TRANSIT_MOVE:
		find_player_at_table(player, table_index,
				     &spectating, &seat_num);

		if (seat_num < 0) {
			/* Uh oh.  A ggzd bug.  But it doesn't have
			   to be fatal. */
			ggz_error_msg("%s couldn't be found at table %d (room %d).",
				player->name, player->table, player->room);
			/* Fake success, so that the client doesn't get
			   confused. */
			net_send_table_leave(player->client->net,
					     GGZ_LEAVE_GAMEERROR, NULL);
			/* Remove the player from the table.  Perhaps we
			   should see if they're actually at a different
			   table?  Or perhaps we should send a full update? */
			player->table = -1;
			return E_OK;
		}

		if (spectating && opcode != GGZ_TRANSIT_SIT)
			return E_BAD_OPTIONS;
		if (!spectating && opcode == GGZ_TRANSIT_SIT)
			return E_BAD_OPTIONS;
		if (opcode == GGZ_TRANSIT_MOVE && seat_index < 0)
			return E_BAD_OPTIONS;
		if (opcode == GGZ_TRANSIT_STAND && seat_index >= 0)
			return E_BAD_OPTIONS;

		seat.index = seat_index;
		if (seat.index < 0) {
			/* Take first available seat */
			seat.index = GGZ_SEATNUM_ANY;
		}
		seat.type = GGZ_SEAT_PLAYER;
		seat.fd = -1;
		strcpy(seat.name, player->name);

		status = transit_seat_event(player->room, table_index,
					    opcode, seat, caller, -1);
		break;
	case GGZ_TRANSIT_SEAT:
		ggz_error_msg("player_transit: shouldn't have TRANSIT_SEAT.");
		break;
	}

	/* If enqueue fails, it's because the table has been removed */
	if (status != GGZ_OK)
		return E_NO_TABLE;

	/* Mark player as "in transit" */
	pthread_rwlock_wrlock(&player->lock);
	player->transit = true;
	pthread_rwlock_unlock(&player->lock);

	return E_OK;
}


GGZPlayerHandlerStatus player_list_players(GGZPlayer* player)
{
	int i, count, room;
	GGZPlayer* p;
	GGZPlayer* data;


	ggz_debug(GGZ_DBG_UPDATE, "Handling player list request for %s", 
		player->name);
		
	/* Don't send list if they're not in a room */
	if (player->room == -1) {
		ggz_debug(GGZ_DBG_UPDATE, "%s requested player list in room -1",
			player->name);
		if (net_send_player_list_error(player->client->net,
					       E_NOT_IN_ROOM) < 0)
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
		pthread_rwlock_rdlock(&p->stats_lock);
		/* Does this copy all applicable player data? */
		data[i] = *p;
		pthread_rwlock_unlock(&p->stats_lock);
		pthread_rwlock_unlock(&p->lock);
	}
	pthread_rwlock_unlock(&rooms[room].lock);

	if (net_send_player_list_count(player->client->net,
				       player_get_room(player), count) < 0) {
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

	ggz_debug(GGZ_DBG_UPDATE, "Handling type list request for %s", 
		player->name);
		
	/* Don't send list if they're not logged in */
 	if (player->login_status == GGZ_LOGIN_NONE) {
		ggz_debug(GGZ_DBG_UPDATE, "%s requested type list before login",
			player->name);
		if (net_send_type_list_error(player->client->net,
					     E_NOT_LOGGED_IN) < 0)
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
	GGZTable **my_tables;
	int count, i;
	
	ggz_debug(GGZ_DBG_UPDATE, "Handling table list request for %s", 
		player->name);
	
	
	/* Don`t send list if they`re not logged in */
	if (player->login_status == GGZ_LOGIN_NONE) {
		ggz_debug(GGZ_DBG_UPDATE, "%s requested table list before login",
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
	
	if (net_send_table_list_count(player->client->net,
				      player_get_room(player), count) < 0)
		return GGZ_REQ_DISCONNECT;
	
	/* Don`t proceed if there was an error, or no tables found*/
	if (count < 0)
		return GGZ_REQ_FAIL;
	
	for (i = 0; i < count; i++)
		if (net_send_table(player->client->net, my_tables[i]) < 0)
			return GGZ_REQ_DISCONNECT;


	if (net_send_table_list_end(player->client->net) < 0)
		return GGZ_REQ_DISCONNECT;

	if (count > 0) {
		for (i = 0; i < count; i++) table_free(my_tables[i]);
		ggz_free(my_tables);
	}
	
	return GGZ_REQ_OK;
}


GGZPlayerHandlerStatus player_chat(GGZPlayer* player, GGZChatType type,
				   const char *target, const char *msg)
{
	int target_room=-1;	/* FIXME - this should come from net.c if we */
				/* are going to support per-room announce... */
	GGZClientReqError status = E_BAD_OPTIONS;

	ggz_debug(GGZ_DBG_CHAT, "Handling chat for %s", player->name);
	
	/* Verify that we are in a regular room */
	/* No lock needed: no one can change our room but us */
	if (player->room == -1) {
		ggz_debug(GGZ_DBG_CHAT, "%s tried to chat from room -1", 
			player->name);
		if (net_send_chat_result(player->client->net, E_NOT_IN_ROOM) < 0)
			return GGZ_REQ_DISCONNECT;
		return GGZ_REQ_OK;
	}
	
	/* Parse type */
	switch (type) {
	case GGZ_CHAT_UNKNOWN:
		/* An unrecognized chat op will be here.  This is probably
		   a future chat type - not implemented yet.  We should
		   handle it gracefully if possible.  So fall through
		   to the "normal" case... */
		/* FIXME: perhaps if there is a target we should treat it
		   as a personal message instead? */
	case GGZ_CHAT_NORMAL:
		ggz_debug(GGZ_DBG_CHAT, "%s sends %s", player->name, msg);
		status = chat_room_enqueue(player->room, type, player, msg);
		break;
	case GGZ_CHAT_TABLE:
		ggz_debug(GGZ_DBG_CHAT, "%s sends table %s", player->name, msg);
		status = chat_table_enqueue(player->room, player->table,
					    type, player, msg);
		break;
	case GGZ_CHAT_BEEP:
	case GGZ_CHAT_PERSONAL:
		status = chat_player_enqueue(target, type, player, msg);
		break;
	case GGZ_CHAT_ANNOUNCE:
		ggz_debug(GGZ_DBG_CHAT, "%s announces %s", player->name, msg);
		status = chat_room_enqueue(target_room, type, player, msg);
		break;
	}

	ggz_debug(GGZ_DBG_CHAT, "%s's chat result: %d", player->name, status);
	if (net_send_chat_result(player->client->net, status) < 0)
		return GGZ_REQ_DISCONNECT;
	
	/* Don't return the chat error code */
	return GGZ_REQ_OK;
}


GGZPlayerHandlerStatus player_room_admin(GGZPlayer* player, GGZAdminType type,
					 const char *target,
					 const char *reason)
{
	GGZClientReqError status = E_BAD_OPTIONS;
	GGZPlayer* rcvr;

	ggz_debug(GGZ_DBG_CHAT, "Handling admin action for %s", player->name);

	/* Verify that we are in a regular room */
	/* No lock needed: no one can change our room but us */
	if (player->room == -1) {
		ggz_debug(GGZ_DBG_CHAT, "%s is not in a room to perform admin action", 
			player->name);
		if (net_send_admin_result(player->client->net, E_NOT_IN_ROOM) < 0)
			return GGZ_REQ_DISCONNECT;
		return GGZ_REQ_OK;
	}

	/* Only hosts and admins can do admin actions */
	if (!ggz_perms_is_host(player->perms)) {
		ggz_debug(GGZ_DBG_CHAT, "%s is no host!", player->name);
		if (net_send_admin_result(player->client->net, E_NO_PERMISSION) < 0)
			return GGZ_REQ_DISCONNECT;
		return GGZ_REQ_OK;
	}

	/* Find out target player. Returns with write-lock on. */
	rcvr = hash_player_lookup(target);

	/* Parse type */
	switch (type) {
	case GGZ_ADMIN_UNKNOWN:
		/* Do nothing. Better safe than sorry. */
		break;
	case GGZ_ADMIN_GAG:
		log_msg(GGZ_LOG_ADMIN, "ADMIN %s gags %s", player->name, target);
		if (rcvr == NULL || rcvr == player) {
			status = E_USR_LOOKUP;
		} else {
			rcvr->gagged = true;
			status = E_OK;
		}
		break;
	case GGZ_ADMIN_UNGAG:
		log_msg(GGZ_LOG_ADMIN, "ADMIN %s ungags %s again", player->name, target);
		if (rcvr == NULL || rcvr == player) {
			status = E_USR_LOOKUP;
		} else {
			rcvr->gagged = false;
			status = E_OK;
		}
		break;
	case GGZ_ADMIN_KICK:
		log_msg(GGZ_LOG_ADMIN, "ADMIN %s kicks %s, reason: %s", player->name, target, reason);
		if (rcvr == NULL || rcvr == player) {
			status = E_USR_LOOKUP;
		} else if (reason == NULL) {
			status = E_BAD_OPTIONS;
		} else {
			/* FIXME 1: session_over should be bool, not char */
			/* FIXME: We temporarily take away the wr-lock since player_enqueue needs it */
			pthread_rwlock_unlock(&rcvr->lock);	
			chat_player_enqueue(target, GGZ_CHAT_PERSONAL, player, reason);

			/* Recover the rcvr player - a new lookup is needed */
			rcvr = hash_player_lookup(target);
			if (rcvr) rcvr->client->session_over = 1;
			status = E_OK;
		}
		break;
	case GGZ_ADMIN_BAN:
		log_msg(GGZ_LOG_ADMIN, "ADMIN %s bans %s", player->name, target);
		/* This is not implemented/used yet. */
		break;
	}

	/* Now we must return write-lock! */
	if (rcvr) {
		pthread_rwlock_unlock(&rcvr->lock);
	}

	ggz_debug(GGZ_DBG_CHAT, "%s's admin action result: %d", player->name, status);
	if (net_send_admin_result(player->client->net, status) < 0)
		return GGZ_REQ_DISCONNECT;

	/* Don't return the admin error code */
	return GGZ_REQ_OK;
}


GGZPlayerHandlerStatus player_perms_admin(GGZPlayer *player,
					  const char *target,
					  GGZPerm perm, bool set)
{
	GGZPlayer* rcvr;

	ggz_debug(GGZ_DBG_CHAT, "Handling admin perm action for %s",
		player->name);

	/* Only admins can do permission actions */
	if (!ggz_perms_is_admin(player->perms)) {
		ggz_debug(GGZ_DBG_CHAT, "%s is no admin!", player->name);
		if (net_send_admin_result(player->client->net,
					  E_NO_PERMISSION) < 0)
			return GGZ_REQ_DISCONNECT;
		return GGZ_REQ_OK;
	}

	/* Find out target player. Returns with write-lock on. */
	rcvr = hash_player_lookup(target);
	if (rcvr == NULL) {
		if (net_send_admin_result(player->client->net,
					  E_USR_LOOKUP) < 0)
			return GGZ_REQ_DISCONNECT;
		return GGZ_REQ_OK;
	}
	if (rcvr == player || rcvr->login_status == GGZ_LOGIN_ANON) {
		pthread_rwlock_unlock(&rcvr->lock);
		if (net_send_admin_result(player->client->net,
					  E_BAD_OPTIONS) < 0)
			return GGZ_REQ_DISCONNECT;
		return GGZ_REQ_OK;
	}

	if (ggz_perms_is_set(rcvr->perms, perm) != set) {
		ggzdbPlayerEntry entry;

		ggz_strncpy(entry.handle, rcvr->name, sizeof(entry.handle));
		if (ggzdb_player_get(&entry) != GGZDB_NO_ERROR) {
			pthread_rwlock_unlock(&rcvr->lock);
			if (net_send_admin_result(player->client->net,
						  E_USR_LOOKUP) < 0)
				return GGZ_REQ_DISCONNECT;
			return GGZ_REQ_OK;
		}

		ggz_perms_set(&rcvr->perms, perm, set);
		perms_set_to_db(rcvr->perms, &entry);

		room_update_event(rcvr->name, GGZ_PLAYER_UPDATE_PERMS,
				  rcvr->room, -1);
	}

	pthread_rwlock_unlock(&rcvr->lock);
	if (net_send_admin_result(player->client->net, E_OK) < 0)
		return GGZ_REQ_DISCONNECT;

	/* Don't return the admin error code */
	return GGZ_REQ_OK;
}


GGZPlayerHandlerStatus player_motd(GGZPlayer* player)
{
	ggz_debug(GGZ_DBG_CHAT, "Handling motd request for %s", player->name);

  	if (!motd_is_defined())
		return GGZ_REQ_OK;

 	/* Don't send motd if they're not logged in */
 	if (player->login_status == GGZ_LOGIN_NONE) {
		ggz_debug(GGZ_DBG_CHAT, "%s requested motd before logging in",
			player->name);
		if (net_send_motd_error(player->client->net,
					E_NOT_LOGGED_IN) < 0)
 			return GGZ_REQ_DISCONNECT;
 		return GGZ_REQ_FAIL;
 	}

	if (net_send_motd(player->client->net) < 0)
		return GGZ_REQ_DISCONNECT;
	
	return GGZ_REQ_OK;
}


GGZPlayerHandlerStatus player_send_room_update(GGZPlayer *player)
{
	int i;
	time_t curr_time = time(NULL);

	if (opt.room_update_freq <= 0) {
		/*ggz_error_msg("Doing a room update even though they're disabled.");*/
		return GGZ_REQ_OK;
	}

	for (i = 0; i < room_get_num_rooms(); i++) {
		int player_count;
		time_t last_player_change;

		if (room_is_removed(i)) {
			continue;
		}

		pthread_rwlock_rdlock(&rooms[i].lock);
		player_count = rooms[i].player_count;
		last_player_change = rooms[i].last_player_change;
		pthread_rwlock_unlock(&rooms[i].lock);

		if (curr_time - last_player_change <= opt.room_update_freq) {
			net_send_room_update(player->client->net,
					     GGZ_ROOM_UPDATE_PLAYER_COUNT, i,
					     player_count, NULL);
		}
	}

	return GGZ_REQ_OK;
}


GGZPlayerHandlerStatus player_send_ping(GGZPlayer *player)
{
	/* Do not send ping if frequency is set to 0 */
	if (opt.ping_freq <= 0)
		return GGZ_REQ_OK;

	/* Send a ping and mark send time */
	if (net_send_ping(player->client->net) < 0)
		return GGZ_REQ_DISCONNECT;

	player->sent_ping_time = get_current_time();
	player->is_ping_sent = true;
	player->next_ping_time = 0.0;

	return GGZ_REQ_OK;
}


static ggztime_t player_get_time_since_ping(GGZPlayer *player)
{
	return get_current_time() - player->sent_ping_time;
}


void player_handle_pong(GGZPlayer *player)
{
	ggztime_t waited = player_get_time_since_ping(player);
	int lag_class, i;

	lag_class = 1;
	for(i=0; i<4; i++) {
		if (waited >= opt.lag_class_time[i])
			lag_class++;
		else
			break;
	}

	if(lag_class != player->lag_class) {
		player->lag_class = lag_class;
		if(player->room >= 0)
			room_update_event(player->name, GGZ_PLAYER_UPDATE_LAG,
					  player->room, -1);
	}

	/* Queue our next ping */
	player->is_ping_sent = false;
	player->sent_ping_time = 0.0;
	player->next_ping_time = get_current_time() + opt.ping_freq;
}


RETSIGTYPE player_handle_event_signal(int sig)
{
	GGZPlayer *player = pthread_getspecific(player_key);

	if (player_updates(player) != GGZ_OK)
		player->client->session_over = 1;
}
