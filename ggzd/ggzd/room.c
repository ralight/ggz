/*
 * File: room.c
 * Author: Brent Hendricks
 * Project: GGZ Server
 * Date: 3/20/00
 * Desc: Functions for interfacing with room and chat facility
 * $Id: room.c 8746 2006-12-23 21:35:25Z jdorje $
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

#include "chat.h"
#include "client.h"
#include "err_func.h"
#include "event.h"
#include "datatypes.h"
#include "ggzd.h"
#include "net.h"
#include "perms.h"
#include "protocols.h"
#include "room.h"
#include "stats.h"


typedef struct {
	GGZPlayerUpdateType opcode;
	char player[MAX_USER_NAME_LEN + 1];

	/* Other room specifies the room a player comes from or is going to
	 * in an ADD or DELETE event.  This info is sent to clients. */
	int other_room;
} GGZRoomEventData;

/* Decl of server wide chat room structure */
RoomStruct *rooms;
RoomInfo room_info;

/* Internal use only */
static void room_notify_change(char* name, const int, const int);
static GGZEventFuncReturn room_event_callback(void* target_player,
					      size_t size, void* data);


/* Handle a REQ_LIST_ROOMS opcode */
GGZPlayerHandlerStatus room_list_send(GGZPlayer* player, int req_game,
                                      char verbose)
{
	int i, max, count=0;

	/* We don't need to lock anything because CURRENTLY the room count  */
	/* and options can change ONLY before threads are in existence	    */

	/* Update: actual now that rooms can change, must be revisited */
	/* In particular, which rooms should be sent, does order matter etc? */

	/* Don't send list if they're not logged in */
	if (player->login_status == GGZ_LOGIN_NONE) {
		if (net_send_room_list_error(player->client->net, E_NOT_LOGGED_IN) < 0)
			return GGZ_REQ_DISCONNECT;
		return GGZ_REQ_FAIL;
	}

	pthread_rwlock_rdlock(&state.lock);
	max = state.types;
	pthread_rwlock_unlock(&state.lock);	

	if((verbose != 0 && verbose != 1) || req_game < -1 || req_game >= max){
		/* Invalid Options Sent */
		if (net_send_room_list_error(player->client->net, E_BAD_OPTIONS) < 0)
			return GGZ_REQ_DISCONNECT;
		return GGZ_REQ_FAIL;
	}

	/* First we have to figure out how many rooms to announce  */
	/* This is easy if a req_game filter hasn't been specified */
	if(req_game != -1) {
		for(i = 0; i < room_get_num_rooms(); i++)
			if(room_will_be_removed(i))
				continue;
			if(req_game == rooms[i].game_type)
				count++;
	} else
		count = room_get_count_rooms();

	/* Do da opcode, and announce our count */
	if (net_send_room_list_count(player->client->net, count) < 0)
		return GGZ_REQ_DISCONNECT;

	/* Send off all the room announcements */
	for(i = 0; i < room_get_num_rooms(); i++)
		if(req_game == -1 || req_game == rooms[i].game_type) {
			GGZReturn result;

			/* Skip removal-pending rooms (includes removal-done) */
			if (room_will_be_removed(i)) {
				continue;
			}

			/* We get a read-lock before sending because we
			 * send the number of players which may change.  All
			 * other data sent is unchanging and doesn't require
			 * a lock.  However this assumes that the rooms
			 * don't change once the server is started.  This
			 * may not always be true.
			 *
			 * Using a second lock rooms[i].count_lock would
			 * cut down on wait time for these locks.  Or we
			 * could just make a copy of the room inside the
			 * lock. */
			pthread_rwlock_rdlock(&rooms[i].lock);
			result = net_send_room(player->client->net, i,
					       &rooms[i], verbose);
			pthread_rwlock_unlock(&rooms[i].lock);

			if (result != 0) {
				return GGZ_REQ_DISCONNECT;
			}
		}

	/* End room list */
	if (net_send_room_list_end(player->client->net) < 0)
		return GGZ_REQ_DISCONNECT;

	/* Schedule the next room update. */
	if (opt.room_update_freq > 0) {
		player->next_room_update_time = get_current_time()
		  + opt.room_update_freq;
	}

	return GGZ_REQ_OK;
}


/* Initialize the first room */
void room_initialize(void)
{
	dbg_msg(GGZ_DBG_ROOM, "Initializing room array");

	room_info.num_rooms = 1;
	room_info.count_rooms = 1;
	pthread_rwlock_init(&room_info.lock, NULL);

	/* Allocate a big enough array to hold all our first room */
	rooms = ggz_malloc(room_info.num_rooms * sizeof(RoomStruct));

	/* Initialize the room_tail and lock */
	rooms[0].event_tail = NULL;
	pthread_rwlock_init(&rooms[0].lock, NULL);
#ifdef DEBUG
	rooms[0].event_head = NULL;
#endif
}


/* Initialize an additional room */
/* Called from control thread only */
void room_create_additional(void)
{
	int room_id;

	dbg_msg(GGZ_DBG_ROOM, "Creating a new room");

	room_id = room_info.num_rooms;

	/* Reallocate the room's array */
	/* FIXME: reallocation might create wholly new array? */
	rooms = ggz_realloc(rooms, (room_id + 1) * sizeof(RoomStruct));

	/* Initialize the room_tail and lock on the new one */
	rooms[room_id].player_count = 0;
	rooms[room_id].last_player_change = 0;
	rooms[room_id].table_count = 0;
	rooms[room_id].event_tail = NULL;
	pthread_rwlock_init(&rooms[room_id].lock, NULL);
#ifdef DEBUG
	rooms[room_id].event_head = NULL;
#endif
	rooms[room_id].removal_pending = 0;
	rooms[room_id].removal_done = 0;

	/* Finally, make it available to iterations over all rooms */
	pthread_rwlock_wrlock(&room_info.lock);
	room_info.num_rooms++;
	room_info.count_rooms++;
	pthread_rwlock_unlock(&room_info.lock);
}

/* Announce a room which was added */
/* Only called from control thread */
void room_add(int room)
{
	int roomnum = room_info.num_rooms, i;

	for (i = 0; i < roomnum; i++) {
		if (rooms[i].removal_done)
			continue;

		room_update_event("", GGZ_ROOM_UPDATE_ADD, i, room);
	}

#if 0
	/* FIXME: This needs to go into the clients! */
	chat_server_2_player(NULL, "Reconfiguration: A new room has been added!");
#endif
}


/* Mark a room to be subject of removal */
/* FIXME: together with room_create_additional, should return error status? */
/* Only called from control thread (but we lock for writing) */
void room_remove(int room)
{
	int i;

	/* actual rooms really start with index 1 (= entry room) */
	if (room <= 0 || room > room_info.num_rooms - 1)
		return;
	if (rooms[room].removal_pending)
		return;

	pthread_rwlock_wrlock(&rooms[room].lock);
	rooms[room].removal_pending = 1;
	pthread_rwlock_unlock(&rooms[room].lock);
	pthread_rwlock_wrlock(&room_info.lock);
	room_info.count_rooms--;
	pthread_rwlock_unlock(&room_info.lock);

	/* We can kill it off right away if nobody's in there */
	if (rooms[room].player_count == 0) {
		room_remove_really(room);
		return;
	}

	dbg_msg(GGZ_DBG_ROOM, "Marking room %i for removal", room);

	/* TODO: need global event enqueue function (for server-wide events) */

	for (i = 0; i < room_info.num_rooms; i++) {
		if (rooms[i].removal_done)
			continue;

		room_update_event("", GGZ_ROOM_UPDATE_CLOSE, i, room);
	}

#if 0
	/* FIXME: really send this? */
	chat_server_2_player(NULL, "A room is about to close!");
#endif
}


/* Called from control thread only */
void room_remove_really(int old_room)
{
	int i;

	if(rooms[old_room].player_count == 0
	&& rooms[old_room].removal_pending)
	{
		pthread_rwlock_wrlock(&rooms[old_room].lock);
		rooms[old_room].removal_done = 1;
		pthread_rwlock_unlock(&rooms[old_room].lock);

		dbg_msg(GGZ_DBG_ROOM, "Delete room %i entirely", old_room);

		/* For notification, see ROOM_UPDATE_CLOSE */
		for (i = 0; i < room_info.num_rooms; i++) {
			room_update_event("", GGZ_ROOM_UPDATE_DELETE, i, old_room);
		}
#if 0 /* This spam message is not useful. */
		chat_server_2_player(NULL, "A room was removed!");
#endif
	}
}


/* Handle the REQ_ROOM_JOIN opcode */
GGZPlayerHandlerStatus room_handle_join(GGZPlayer* player, int room)
{
	int result;
	char *rname;

	/* Check for silliness from the user */
	if (player->table != -1 || player->launching) {
		if (net_send_room_join(player->client->net, E_AT_TABLE) < 0)
			return GGZ_REQ_DISCONNECT;
		return GGZ_REQ_FAIL;
	}

	if (player->transit) {
		if (net_send_room_join(player->client->net, E_IN_TRANSIT) < 0)
			return GGZ_REQ_DISCONNECT;
		return GGZ_REQ_FAIL;
	}

	if(room > room_get_num_rooms() || room < 0) {
		if (net_send_room_join(player->client->net, E_BAD_OPTIONS) < 0)
			return GGZ_REQ_DISCONNECT;
		return GGZ_REQ_FAIL;
	}

	if(room_will_be_removed(room)) {
		if (net_send_room_join(player->client->net, E_BAD_OPTIONS) < 0)
			return GGZ_REQ_DISCONNECT;
		return GGZ_REQ_FAIL;
	}

	/* Do the actual room change, and return results */
	result = room_join(player, room);

	/* Generate a log entry if room was full */
	if(result == E_ROOM_FULL) {
		pthread_rwlock_rdlock(&rooms[room].lock);
		rname = ggz_strdup(rooms[room].name);
		pthread_rwlock_unlock(&rooms[room].lock);
		log_msg(GGZ_LOG_NOTICE,
			"ROOM_FULL - %s rejected entry to %s",
			player->name, rname);
		ggz_free(rname);
	}

	if (net_send_room_join(player->client->net, result) < 0)
		return GGZ_REQ_DISCONNECT;

	return GGZ_REQ_OK;
}


/* Read the player's stats for this room. */
static void update_room_stats(GGZPlayer *player, int game_type)
{
	ggzdbPlayerGameStats stats;
	int records, ratings, highscores;

	/* Don't show stats for guest players at all.  Later it may be
	   that some games will want to let guest players have stats. */
	if (!perms_check(player, GGZ_PERM_NO_STATS)
	    && game_type >= 0) {
		pthread_rwlock_rdlock(&game_types[game_type].lock);
		strcpy(stats.game, game_types[game_type].name);
		records = game_types[game_type].stats_records;
		ratings = game_types[game_type].stats_ratings;
		highscores = game_types[game_type].stats_highscores;
		pthread_rwlock_unlock(&game_types[game_type].lock);
	} else {
		records = ratings = highscores = 0;
	}

	if (records || ratings || highscores) {
		strcpy(stats.player, player->name);
		/* FIXME: doing a DB lookup while we have a write-lock
		   held on the room is bad! */
		if (stats_lookup(&stats) != GGZ_OK) {
			records = ratings = 0;
		}
	}

	pthread_rwlock_wrlock(&player->stats_lock);
	player->have_record = records;
	if (records) {
		player->wins = stats.wins;
		player->losses = stats.losses;
		player->ties = stats.ties;
		player->forfeits = stats.forfeits;
	}
	player->have_rating = ratings;
	if (ratings) 
		player->rating = (int)(stats.rating + 0.5);
	player->have_highscore = highscores;
	if (highscores) {
		player->highscore = stats.highest_score;
	}
	pthread_rwlock_unlock(&player->stats_lock);
}


GGZClientReqError room_join(GGZPlayer* player, const int room)
{
	int old_room;
	int i, count;
	GGZPlayer* last;
	char name[MAX_USER_NAME_LEN+1];

	/* No other thread could possibly be changing the room # */
	/* so we can read it without a lock! */
	old_room = player->room;

	/* Check for valid inputs */
	if(old_room == room)
		return E_OK;
	if(room > room_get_num_rooms() || room < -2)
		return E_BAD_OPTIONS;

	/* Process queued messages unless they are connecting/disconnecting */
	if (old_room != -1 && room != -1)
		if(event_room_handle(player) < 0)
			return E_BAD_XML; /* ??? */

	/* We ALWAYS lock the lower ordered room first! */
	if(old_room < room) {
		if(old_room != -1)
			pthread_rwlock_wrlock(&rooms[old_room].lock);
		pthread_rwlock_wrlock(&rooms[room].lock);
	} else {
		if(room != -1)
			pthread_rwlock_wrlock(&rooms[room].lock);
		pthread_rwlock_wrlock(&rooms[old_room].lock);
	}

	/* Check permissions to enter new room */
	if (room != -1 && rooms[room].perms != 0
	    && !perms_check(player, rooms[room].perms)) {
		pthread_rwlock_unlock(&rooms[room].lock);
		if(old_room != -1)
			pthread_rwlock_unlock(&rooms[old_room].lock);
		return E_NO_PERMISSION;
	}

	/* Check for room full condition */
	if(room != -1
	   && rooms[room].player_count == rooms[room].max_players) {
		pthread_rwlock_unlock(&rooms[room].lock);
		if(old_room != -1)
			pthread_rwlock_unlock(&rooms[old_room].lock);
		return E_ROOM_FULL;
	}

	/* Yank them from this room */
	if(old_room != -1) {
		if (player->room_events)
			event_room_flush(player);
		count = -- rooms[old_room].player_count;
		rooms[old_room].last_player_change = time(NULL);
		last = rooms[old_room].players[count];
		for(i=0; i<=count; i++)
			if(rooms[old_room].players[i] == player) {
				rooms[old_room].players[i] = last;
				break;
			}
		dbg_msg(GGZ_DBG_ROOM,
			"Room %d player count = %d", old_room, count);
	}

	/* Put them in the new room, and free up the old room */
	player->room = room;
	if(old_room != -1)
		pthread_rwlock_unlock(&rooms[old_room].lock);

	/* Update their game stats for this room.  Note, this is a major
	   bottleneck since it means we're doing a DB access while we have
	   a write-lock on the room!  But doing it differently is tricky. */
	if (room != -1)
		update_room_stats(player, rooms[room].game_type);

	/* Adjust the new rooms statistics */
	if(room != -1) {
		count = ++ rooms[room].player_count;
		rooms[room].last_player_change = time(NULL);
		rooms[room].players[count-1] = player;
		dbg_msg(GGZ_DBG_ROOM, "Room %d player count = %d", room, count);
	}

	/* Finally we can release the other chat room lock */
	if(room != -1)
		pthread_rwlock_unlock(&rooms[room].lock);

	pthread_rwlock_rdlock(&player->lock);
	strcpy(name, player->name);
	pthread_rwlock_unlock(&player->lock);

	room_notify_change(name, old_room, room);

	/* Reconfiguration: last man leaving might kill room */
	if(old_room != -1)
		room_remove_really(old_room);

	return E_OK;
}


/* Notify clients that someone has entered/left the room */
static void room_notify_change(char* name, const int old, const int new)
{
	dbg_msg(GGZ_DBG_ROOM, "%s moved from room %d to %d", name, old, new);

	if (old != -1) {
		/* Send DELETE update to old room */
		room_update_event(name, GGZ_PLAYER_UPDATE_DELETE, old, new);
	}

	if (new != -1) {
		/* Send ADD update to new room */
		room_update_event(name, GGZ_PLAYER_UPDATE_ADD, new, old);
	}
}



GGZReturn room_update_event(const char *player,
			    GGZPlayerUpdateType update,
			    int room, int other_room)
{
	GGZRoomEventData *data = ggz_malloc(sizeof(*data));
	data->opcode = update;
	data->other_room = other_room;
	strcpy(data->player, player);

	return event_room_enqueue(room, room_event_callback,
				  sizeof(*data), data, NULL);
}

/* Event callback for delivering player list update to player */
static GGZEventFuncReturn room_event_callback(void* target_player,
					      size_t size, void* data)
{
	GGZPlayer *player = target_player;
	GGZRoomEventData *event = data;
	RoomStruct room;

	if (event->opcode == GGZ_ROOM_UPDATE_CLOSE
	|| event->opcode == GGZ_ROOM_UPDATE_DELETE) {
		net_send_room_update(player->client->net, event->opcode, event->other_room, 0, NULL);
		return GGZ_EVENT_OK;
	}

	if (event->opcode == GGZ_ROOM_UPDATE_ADD) {
		room = rooms[event->other_room];
		net_send_room_update(player->client->net, event->opcode, event->other_room, 0, &room);
		return GGZ_EVENT_OK;
	}

	/* Don't deliver updates about ourself (except lag/stats) */
	if (event->opcode != GGZ_PLAYER_UPDATE_LAG
	    && event->opcode != GGZ_PLAYER_UPDATE_STATS
	    && strcasecmp(event->player, player->name) == 0)
		return GGZ_EVENT_OK;

	if (net_send_player_update(player->client->net, event->opcode,
				   event->player, player_get_room(player),
				   event->other_room) < 0)
		return GGZ_EVENT_ERROR;

	return GGZ_EVENT_OK;
}


/* Only use in non-control thread since locking is expensive */
int room_get_num_rooms(void)
{
	int number;

	pthread_rwlock_rdlock(&room_info.lock);
	number = room_info.num_rooms;
	pthread_rwlock_unlock(&room_info.lock);

	return number;
}

/* Only use in non-control thread since locking is expensive */
int room_get_count_rooms(void)
{
	int number;

	pthread_rwlock_rdlock(&room_info.lock);
	number = room_info.count_rooms;
	pthread_rwlock_unlock(&room_info.lock);

	return number;
}

/* This is always called from within loops over all rooms */
/* Only use in non-control thread since locking is expensive */
int room_is_removed(int room)
{
	int removed = 0;

	pthread_rwlock_rdlock(&rooms[room].lock);
	if (rooms[room].removal_done) {
		removed = 1;
	}
	pthread_rwlock_unlock(&rooms[room].lock);

	return removed;
}

/* This is always called from within loops over all rooms */
/* Only use in non-control thread since locking is expensive */
int room_will_be_removed(int room)
{
	int removed = 0;

	pthread_rwlock_rdlock(&rooms[room].lock);
	if (rooms[room].removal_pending) {
		removed = 1;
	}
	pthread_rwlock_unlock(&rooms[room].lock);

	return removed;
}

