/*
 * File: players.h
 * Author: Brent Hendricks
 * Project: GGZ Server
 * Date: 10/18/99
 * Desc: Functions for handling players
 * $Id: players.h 4822 2002-10-09 06:17:54Z jdorje $
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

#include <config.h>

#include <pthread.h>
#include <sys/time.h>

#include <ggz_common.h>

#include "ggzd.h"


/* 
 * The GGZPlayer structure contains information about a single
 * logged-in player 
 */
struct _GGZPlayer {

	/* Individual mutex lock */
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
	char launching;

	/* Transit flag */
	char transit;

	/* Linked lists of events */
	GGZEvent *room_events;        /* protected by room lock*/
	GGZEvent *my_events_head;
	GGZEvent *my_events_tail;

	/* Lag tracking */
	time_t next_ping;
	struct timeval sent_ping;
	int lag_class;
	
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
				   char *target, char *msg);
GGZPlayerHandlerStatus player_table_launch(GGZPlayer* player, GGZTable *table);
GGZPlayerHandlerStatus player_table_update(GGZPlayer* player, GGZTable *table);
GGZPlayerHandlerStatus player_table_join(GGZPlayer* player, int index);
GGZPlayerHandlerStatus player_table_join_spectator(GGZPlayer* player, int index);
GGZPlayerHandlerStatus player_table_leave(GGZPlayer* player,
					  int spectator, int force);
GGZPlayerHandlerStatus player_list_players(GGZPlayer* player);
GGZPlayerHandlerStatus player_list_types(GGZPlayer* player, char verbose);
GGZPlayerHandlerStatus player_list_tables(GGZPlayer* player, int type,
                                          char global);
GGZPlayerHandlerStatus player_motd(GGZPlayer* player);

int player_get_room(GGZPlayer *player);
GGZPlayerType player_get_type(GGZPlayer *player);

void player_handle_pong(GGZPlayer *player);


#endif
