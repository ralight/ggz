/*
 * File: players.h
 * Author: Brent Hendricks
 * Project: GGZ Server
 * Date: 10/18/99
 * Desc: Functions for handling players
 * $Id: players.h 3433 2002-02-21 04:01:18Z bmh $
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
#include <ggzd.h>
#include <table.h>


/* 
 * The GGZPlayer structure contains information about a single
 * logged-in player 
 */
struct _GGZPlayer {

	/* Individual mutex lock */
	pthread_rwlock_t lock;

	/* Player name, leaving room for '\0' */
	char name[MAX_USER_NAME_LEN + 1]; 

	/* IP (or hostname) address from which player connected */
	char addr[64];

	/* Network IO object for communicating with player */
	struct _GGZNetIO *net;

	/* Thread ID of the thread handling this player's requests */
	pthread_t thread;

	/* FIXME: need to replace with a "logged-in" flag */
	int uid;

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
	void *room_events;        /* protected by room lock*/
        void *my_events_head;
        void *my_events_tail;

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

/* Special UID values.  Eventually, every registered player
   will have a unique UID, but for now they are all just
   assigned a UID of 0. */
typedef enum {
	GGZ_UID_REGISTERED = 0,
	GGZ_UID_NONE = -1,
	GGZ_UID_ANON = -2
} GGZUIDType;

typedef enum {
	GGZ_PLAYER_NONE,
	GGZ_PLAYER_NORMAL,
	GGZ_PLAYER_GUEST,
	GGZ_PLAYER_ADMIN
} GGZPlayerType;

typedef struct _GGZPlayer GGZPlayer; 

void player_handler_launch(int sock);

GGZEventFuncReturn player_launch_callback(void* target, int size, void* data);

GGZPlayerHandlerStatus player_msg_from_sized(GGZPlayer* p, int size, char *buf);
int player_chat(GGZPlayer* player, unsigned char subop, char *target, char *msg);
GGZPlayerHandlerStatus player_table_launch(GGZPlayer* player, GGZTable *table);
GGZPlayerHandlerStatus player_table_update(GGZPlayer* player, GGZTable *table);
GGZPlayerHandlerStatus player_table_join(GGZPlayer* player, int index);
int   player_table_leave(GGZPlayer* player);
GGZPlayerHandlerStatus player_list_players(GGZPlayer* player);
GGZPlayerHandlerStatus player_list_types(GGZPlayer* player, char verbose);
GGZPlayerHandlerStatus player_list_tables(GGZPlayer* player, int type,
                                          char global);
GGZPlayerHandlerStatus player_motd(GGZPlayer* player);

int player_get_room(GGZPlayer *player);
GGZPlayerType player_get_type(GGZPlayer *player);

void player_handle_pong(GGZPlayer *player);

void player_set_ip_ban_list(int count, char **list);

#endif
