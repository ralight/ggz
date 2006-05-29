/*
 * File: room.h
 * Author: Brent Hendricks
 * Project: GGZ Server
 * Date: 3/20/00
 * Desc: Functions for interfacing with the room and chat facility
 * $Id: room.h 8071 2006-05-29 07:34:31Z josef $
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

#ifndef _GGZ_ROOM_H
#define _GGZ_ROOM_H

#include <pthread.h>

#include "event.h"
#include "players.h"

/* 
 * The RoomStruct structure is meant to be a node in a linked list
 * of rooms.
 */
/* A Room Structure */
typedef struct {

	/* Individual mutex lock */
	pthread_rwlock_t lock;

	/* Room name */
	char *name;			/* cleanup() */

	/* Room file name (internal) */
	char *room;			/* cleanup() */

	/* Room description */
	char *description;		/* cleanup() */

	/* Number of players curently in room */
	int player_count;

	/* Maximum number of allowed players */
	int max_players;

	/* Number of tables curently in room */
	int table_count;

	/* Maximum number of allowed tables */
	int max_tables;

	/* Type of game played in this room */
	int game_type;

	/* Room entry restrictions */
	unsigned int perms;	/* Set bits to equal perms in perms.h */

	/* Array of pointers to players in this room (dynamcially allocated) */
	GGZPlayer **players;		/* cleanup() */

	/* The time when the last player entered/exited the room. */
	time_t last_player_change;

	/* Array of pointers to tables in this room (dynamcially allocated) */
	GGZTable **tables;		/* cleanup() */

	/* Linked lists of events */
	GGZEvent *event_tail;
#ifdef DEBUG
	GGZEvent *event_head;
#endif

	/* If set, room will be nuked when all players left (and then remains set!) */
	int removal_pending;

	/* If set, room is nuked (since we cannot really modify the rooms array) */
	int removal_done;
} RoomStruct;


/* Information about all rooms */
typedef struct {
#if 0
	pthread_rwlock_t lock;	/* Not inititalized or used yet */
#endif
	/* All rooms including removal-pending and removal-done */
	int num_rooms;
	/* Number of active rooms */
	int count_rooms;
} RoomInfo;


extern RoomStruct *rooms;		/* cleanup() */
extern RoomInfo room_info;

void room_initialize(void);
void room_create_additional(void);
void room_add(int room);
void room_remove(int room);
GGZClientReqError room_join(GGZPlayer* player, const int);
GGZPlayerHandlerStatus room_list_send(GGZPlayer* player, int game,
                                             char verbose);
GGZPlayerHandlerStatus room_handle_join(GGZPlayer* player, int room);
int room_get_num_rooms(void);

GGZReturn room_update_event(const char *player,
			    GGZPlayerUpdateType update,
			    int room, int other_room);

#endif
