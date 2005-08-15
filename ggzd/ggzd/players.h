/*
 * File: players.h
 * Author: Brent Hendricks
 * Project: GGZ Server
 * Date: 10/18/99
 * Desc: Functions for handling players
 * $Id: players.h 7424 2005-08-15 09:00:27Z josef $
 *
 * Copyright (C) 1999,2000 Brent Hendricks.
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


#ifndef _GGZ_PLAYER_H
#define _GGZ_PLAYER_H

#include <pthread.h>
#include <signal.h>
#include <sys/time.h>

#include <ggz_common.h>

#include "ggzd.h"
#include "protocols.h"
#include "seats.h"
#include "util.h"


/* 
 * The GGZPlayer structure contains information about a single
 * logged-in player 
 */
struct _GGZPlayer {

	/* Individual mutex lock.  The player structure (except as noted
	   below) will only be *written* to by its own player thread.
	   It may be read by any thread.  Thus a rdlock is necessary
	   for those other threads to read it, and a rwlock is necessary
	   when the player thread writes. */
	pthread_rwlock_t lock;

	/* Player name, leaving room for '\0' */
	char name[MAX_USER_NAME_LEN + 1]; 

	GGZClient *client;

	/* Player's login status/type */
	GGZLoginStatus login_status;

	/* Players User ID number*/
	unsigned int uid;

	/* "Room" in which player currently resides (if any) */
	int room;

	/* Table at while player is "sitting" (if any) */
	int table; 
	
	/* fd for communicating with game module */
	int game_fd;
	   
	/* Launching flag */
	bool launching;

	/* Transit flag */
	bool transit;

	/* Linked lists of events */
	GGZEvent *room_events;        /* protected by room lock*/
	GGZEvent *my_events_head;
	GGZEvent *my_events_tail;

	/* Lag tracking */
	bool is_ping_sent;
	ggztime_t next_ping_time;
	ggztime_t sent_ping_time;
	int lag_class;

	/* Room tracking.  If non-zero this indicates the time of the 
	 * next room update.  See opt.room_update_freq. */
	bool is_room_update_needed;
	ggztime_t next_room_update_time;

	/* Stats tracking.  This holds stats information for the player in
	   the current room.  Unlike the rest of the player structure, this
	   data may be written to by any thread.  Thus it needs a separate
	   lock, which everyone needs to use to read and write to it. */
	pthread_rwlock_t stats_lock;
	bool have_record;
	int wins, losses, ties, forfeits;
	int have_rating;
	int rating;
#if 0
	bool have_ranking;
	unsigned int ranking;
#endif
	int have_highscore;
	int highscore;

	/* Connection info */
	long login_time;

	/* Player permissions settings */
	unsigned int perms;
	/*list_t *op_rooms;	Not yet */
};


GGZPlayer* player_new(GGZClient *client);
void player_free(GGZPlayer *player);
GGZReturn player_updates(GGZPlayer* player);
void player_logout(GGZPlayer* player);

GGZEventFuncReturn player_launch_callback(void* target, size_t size,
					  void* data);

GGZPlayerHandlerStatus player_chat(GGZPlayer* player, GGZChatType type,
				   const char *target, const char *msg);
GGZPlayerHandlerStatus player_table_launch(GGZPlayer* player, GGZTable *table);
GGZPlayerHandlerStatus player_table_join(GGZPlayer* player,
					 int table_index,
					 int seat_num);
GGZPlayerHandlerStatus player_table_join_spectator(GGZPlayer* player, int index);
GGZPlayerHandlerStatus player_table_leave(GGZPlayer* player,
					  int spectator, int force);
GGZPlayerHandlerStatus player_table_reseat(GGZPlayer *player,
					   GGZReseatType opcode,
					   int seat_num);

GGZPlayerHandlerStatus player_table_desc_update(GGZPlayer* player,
						int room_id, int table_id,
						const char *desc);
GGZPlayerHandlerStatus player_table_seat_update(GGZPlayer *player,
						int room_id, int table_id,
						GGZTableSeat *seat);
GGZPlayerHandlerStatus player_table_boot_update(GGZPlayer *player,
						int room_id, int table_id,
						const char *name);

GGZPlayerHandlerStatus player_table_info(GGZPlayer *player, int seat_num);

GGZPlayerHandlerStatus player_list_players(GGZPlayer* player);
GGZPlayerHandlerStatus player_list_types(GGZPlayer* player, char verbose);
GGZPlayerHandlerStatus player_list_tables(GGZPlayer* player, int type,
                                          char global);
GGZPlayerHandlerStatus player_motd(GGZPlayer* player);

int player_get_room(GGZPlayer *player);

/* Returns the player type of the player.  The caller should have
   acquired any necessary locks already. */
GGZPlayerType player_get_type(GGZPlayer *player);

void player_handle_pong(GGZPlayer *player);

#define PLAYER_EVENT_SIGNAL SIGUSR1
extern pthread_key_t player_key;
RETSIGTYPE player_handle_event_signal(int signal);


#endif
