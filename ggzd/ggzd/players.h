/*
 * File: players.h
 * Author: Brent Hendricks
 * Project: GGZ Server
 * Date: 10/18/99
 * Desc: Functions for handling players
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

	/* IP address from which player connected */
	char addr[16];

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
	
	/* Connection info */
	long login_time;

	/* Player permissions settings */
	unsigned int perms;
	/*list_t *op_rooms;	Not yet */
};

/* Special UID values */
#define GGZ_UID_NONE -1
#define GGZ_UID_ANON -2

typedef enum {
	GGZ_PLAYER_NONE,
	GGZ_PLAYER_NORMAL,
	GGZ_PLAYER_GUEST,
	GGZ_PLAYER_ADMIN
} GGZPlayerType;

typedef struct _GGZPlayer GGZPlayer; 

void player_handler_launch(int sock);

int player_launch_callback(void* target, int size, void* data);

int player_msg_from_sized(GGZPlayer* p, int size, char *buf);
int player_chat(GGZPlayer* player, unsigned char subop, char *target, char *msg);
int   player_table_launch(GGZPlayer* player, GGZTable *table);
int   player_table_join(GGZPlayer* player, int index);
int   player_table_leave(GGZPlayer* player);
int   player_list_players(GGZPlayer* player);
int   player_list_types(GGZPlayer* player, char verbose);
int   player_list_tables(GGZPlayer* player, int type, char global);
int   player_motd(GGZPlayer* player);

int player_get_room(GGZPlayer *player);
GGZPlayerType player_get_type(GGZPlayer *player);

#endif
