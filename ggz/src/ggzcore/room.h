/*
 * File: room.h
 * Author: Brent Hendricks
 * Project: GGZ Core Client Lib
 * Date: 6/5/00
 * $Id: room.h 6880 2005-01-24 08:36:55Z jdorje $
 *
 * This fils contains functions for handling rooms
 *
 * Copyright (C) 1998 Brent Hendricks.
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


#ifndef __ROOM_H__
#define __ROOM_H__

#include <ggz.h>

#include "gametype.h"
#include "ggzcore.h"
#include "hook.h"
#include "player.h"
#include "server.h"
#include "table.h"

/* Array of GGZRoom messages */
static char *_ggzcore_room_events[] = {
	"GGZ_PLAYER_LIST",
	"GGZ_TABLE_LIST",
	"GGZ_CHAT_EVENT",
	"GGZ_ROOM_ENTER",
	"GGZ_ROOM_LEAVE",
	"GGZ_TABLE_UPDATE",
	"GGZ_TABLE_LAUNCHED",
	"GGZ_TABLE_LAUNCH_FAIL",
	"GGZ_TABLE_JOINED",
	"GGZ_TABLE_JOIN_FAIL",
	"GGZ_TABLE_LEFT",
	"GGZ_TABLE_LEAVE_FAIL",
	"GGZ_PLAYER_LAG",
	"GGZ_PLAYER_STATS",
	"GGZ_PLAYER_COUNT"
};

/*
 * The GGZRoom struct manages information about a particular room
 */
struct _GGZRoom {

	/* Server which this room is on */
	struct _GGZServer *server;

	/* Monitoring flag */
	char monitor;

	/* Room ID on the server */
	unsigned int id;

	/* Name of room */
	char *name;

	/* Supported game type (ID on server) */
	unsigned int game;

	/* Room description */
	char *desc;

	/* Number of players in list room (current room only) */
	unsigned int num_players;

	/* List of players in the room (current room only) */
	GGZList *players;

	/* Number of players we suspect are in the room */
	int player_count;

	/* Number of tables (current room only) */
	unsigned int num_tables;

	/* List of tables in the room (current room only) */
	GGZList *tables;

	/* Room events */
	GGZHookList *event_hooks[sizeof(_ggzcore_room_events) /
				 sizeof(_ggzcore_room_events[0])];

};


/* Allocate space for a new room object */
struct _GGZRoom *_ggzcore_room_new(void);

/* Initialize room object */
void _ggzcore_room_init(struct _GGZRoom *room,
			const struct _GGZServer *server,
			const unsigned int id,
			const char *name,
			const unsigned int game,
			const char *desc, int player_count);

/* De-allocate room object and its children */
void _ggzcore_room_free(struct _GGZRoom *room);


/* Functions for manipulating hooks to GGZRoom events */
int _ggzcore_room_add_event_hook_full(struct _GGZRoom *room,
				      const GGZRoomEvent event,
				      const GGZHookFunc func,
				      const void *data);

int _ggzcore_room_remove_event_hook(struct _GGZRoom *room,
				    const GGZRoomEvent event,
				    const GGZHookFunc func);

int _ggzcore_room_remove_event_hook_id(struct _GGZRoom *room,
				       const GGZRoomEvent event,
				       const unsigned int hook_id);

/* Functions for changing GGZRoom data */
void _ggzcore_room_set_monitor(struct _GGZRoom *room, char monitor);

void _ggzcore_room_set_player_list(struct _GGZRoom *room,
				   unsigned int count, GGZList * list);
void _ggzcore_room_set_players(struct _GGZRoom *room, int players);
void _ggzcore_room_add_player(struct _GGZRoom *room, GGZPlayer * pdata,
			      int from_room);
void _ggzcore_room_remove_player(struct _GGZRoom *room, const char *name,
				 int to_room);

void _ggzcore_room_set_table_list(struct _GGZRoom *room,
				  unsigned int count, GGZList * list);
void _ggzcore_room_add_table(struct _GGZRoom *room,
			     struct _GGZTable *table);
void _ggzcore_room_remove_table(struct _GGZRoom *room,
				const unsigned int id);


void _ggzcore_room_player_set_table(struct _GGZRoom *room,
				    const char *name, int table);

void _ggzcore_room_table_event(struct _GGZRoom *, GGZRoomEvent,
			       void *data);



void _ggzcore_room_add_chat(struct _GGZRoom *room,
			    GGZChatType type,
			    const char *name, const char *msg);

/* Functions for notifying GGZRoom */
void _ggzcore_room_set_table_launch_status(struct _GGZRoom *room,
					   int status);
void _ggzcore_room_set_table_join(GGZRoom * room, int table_index);
void _ggzcore_room_set_table_join_status(struct _GGZRoom *room,
					 int status);
void _ggzcore_room_set_table_leave(GGZRoom * room,
				   GGZLeaveType reason,
				   const char *player);
void _ggzcore_room_set_table_leave_status(struct _GGZRoom *room,
					  int status);


/* Functions for invoking GGZRoom "actions" */

int _ggzcore_room_load_playerlist(struct _GGZRoom *room);
int _ggzcore_room_load_tablelist(struct _GGZRoom *room,
				 const int type, const char global);

int _ggzcore_room_chat(struct _GGZRoom *room,
		       const GGZChatType type,
		       const char *player, const char *msg);

int _ggzcore_room_launch_table(struct _GGZRoom *room,
			       struct _GGZTable *table);
int _ggzcore_room_join_table(struct _GGZRoom *room, const unsigned int num,
			     int spectator);
int _ggzcore_room_leave_table(struct _GGZRoom *room, int force);
int _ggzcore_room_leave_table_spectator(struct _GGZRoom *room);

int _ggzcore_room_send_game_data(struct _GGZRoom *room, char *buffer);
void _ggzcore_room_recv_game_data(struct _GGZRoom *room, char *buffer);

GGZPlayer *_ggzcore_room_get_player_by_name(GGZRoom * room,
					    const char *name);
GGZHookReturn _ggzcore_room_event(GGZRoom * room, GGZRoomEvent id,
				  const void *data);

/* Utility functions for room lists */
int _ggzcore_room_compare(void *p, void *q);
void *_ggzcore_room_copy(void *p);
void _ggzcore_room_destroy(void *p);


#endif /* __ROOM_H_ */
