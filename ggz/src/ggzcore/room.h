/*
 * File: room.h
 * Author: Brent Hendricks
 * Project: GGZ Core Client Lib
 * Date: 6/5/00
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

#include "ggzcore.h"
#include "server.h"
#include "player.h"
#include "table.h"
#include "gametype.h"


/* Create a new room object for a given server */
struct _GGZRoom* _ggzcore_room_new(void);

void _ggzcore_room_init(struct _GGZRoom *room,
			const struct _GGZServer *server,
			const unsigned int id, 
			const char* name, 
			const unsigned int game, 
			const char* desc);

/* Utility functions for room lists */
int   _ggzcore_room_compare(void* p, void* q);
void* _ggzcore_room_copy(void* p);
void  _ggzcore_room_destroy(void* p);


/* Functions for attaching hooks to GGZRoom events */
int _ggzcore_room_add_event_hook_full(struct _GGZRoom *room,
				      const GGZRoomEvent event, 
				      const GGZHookFunc func,
				      void *data);

/* Functions for removing hooks from GGZRoom events */
int _ggzcore_room_remove_event_hook(struct _GGZRoom *room,
				    const GGZRoomEvent event, 
				    const GGZHookFunc func);

int _ggzcore_room_remove_event_hook_id(struct _GGZRoom *room,
				       const GGZRoomEvent event, 
				       const unsigned int hook_id);


/* Functions to retrieve room information */
struct _GGZServer*   _ggzcore_room_get_server(struct _GGZRoom *room);
unsigned int         _ggzcore_room_get_id(struct _GGZRoom *room);
char*                _ggzcore_room_get_name(struct _GGZRoom *room);
struct _GGZGameType* _ggzcore_room_get_game(struct _GGZRoom *room); 
char*                _ggzcore_room_get_desc(struct _GGZRoom *room);
unsigned int         _ggzcore_room_get_num_players(struct _GGZRoom *room);
struct _GGZPlayer*   _ggzcore_room_get_nth_player(struct _GGZRoom *room, 
						const unsigned int num);

unsigned int       _ggzcore_room_get_num_tables(struct _GGZRoom *room);
struct _GGZTable*  _ggzcore_room_get_nth_table(struct _GGZRoom *room, 
					       const unsigned int num);

struct _GGZTable*  _ggzcore_room_get_table_by_id(struct _GGZRoom *room, 
						 const unsigned int id);


void _ggzcore_room_set_player_list(struct _GGZRoom *room,
				   unsigned int count,
				   struct _ggzcore_list *list);

void _ggzcore_room_set_table_list(struct _GGZRoom *room,
				  unsigned int count,
				  struct _ggzcore_list *list);

void _ggzcore_room_set_monitor(struct _GGZRoom *room, char monitor);

void _ggzcore_room_add_player(struct _GGZRoom *room, char *name);

void _ggzcore_room_remove_player(struct _GGZRoom *room, char *name);

void _ggzcore_room_add_table(struct _GGZRoom *room, 
			     struct _GGZTable *table);

void _ggzcore_room_remove_table(struct _GGZRoom *room, int index);

void _ggzcore_room_add_chat(struct _GGZRoom *room, 
			    GGZChatOp op, 
			    char *name,
			    char *msg);

void _ggzcore_room_list_players(struct _GGZRoom *room);

void _ggzcore_room_list_tables(struct _GGZRoom *room, 
			       const int type, 
			       const char global);

void _ggzcore_room_chat(struct _GGZRoom *room,
			const GGZChatOp opcode,
			const char *player,
			const char *msg);


unsigned int _ggzcore_room_get_num(struct _GGZRoom *room);

void _ggzcore_room_free(struct _GGZRoom *room);


#endif /* __ROOM_H_ */
