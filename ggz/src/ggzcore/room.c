/*
 * File: room.c
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


#include "ggzcore.h"
#include "room.h"
#include "server.h"
#include "table.h"
#include "player.h"

#include <stdlib.h>
#include <string.h>

/* Local functions */
static int _ggzcore_room_event_is_valid(GGZRoomEvent event);
static GGZHookReturn _ggzcore_room_event(GGZRoom *room, GGZRoomEvent id, 
					 void *data);

/* Publicly exported functions */

int ggzcore_room_get_num(GGZRoom *room)
{
	if (!room)
		return -1;

	return _ggzcore_room_get_num(room);
}


char* ggzcore_room_get_name(GGZRoom *room)
{
	if (!room)
		return NULL;

	return _ggzcore_room_get_name(room);
}


char* ggzcore_room_get_desc(GGZRoom *room)
{
	if (!room)
		return NULL;

	return _ggzcore_room_get_desc(room);
}


int ggzcore_room_get_gametype(GGZRoom *room)
{
	if (!room)
		return -1;

	return _ggzcore_room_get_game(room);
}

/* Functions for attaching hooks to GGZRoom events */
int ggzcore_room_add_event_hook(GGZRoom *room,
				const GGZRoomEvent event, 
				const GGZHookFunc func) 
{
	if (!room || !_ggzcore_room_event_is_valid(event))
		return -1;

	return _ggzcore_room_add_event_hook_full(room, event, func, NULL);
}


int ggzcore_room_add_event_hook_full(GGZRoom *room,
				     const GGZRoomEvent event, 
				     const GGZHookFunc func,
				     void *data)
{
	if (!room || !_ggzcore_room_event_is_valid(event))
		return -1;

	return _ggzcore_hook_add_full(room->event_hooks[event], func, data);
}


/* Functions for removing hooks from GGZRoom events */
int ggzcore_room_remove_event_hook(GGZRoom *room,
				   const GGZRoomEvent event, 
				   const GGZHookFunc func) 
{
	if (!room || !_ggzcore_room_event_is_valid(event))
		return -1;

	return _ggzcore_hook_remove(room->event_hooks[event], func);
}


int ggzcore_room_remove_event_hook_id(GGZRoom *room,
				      const GGZRoomEvent event, 
				      const unsigned int hook_id) 
{
	if (!room || !_ggzcore_room_event_is_valid(event))
		return -1;

	return _ggzcore_hook_remove_id(room->event_hooks[event], hook_id);
}


void ggzcore_room_chat(struct _GGZRoom *room,
		       const GGZChatOp opcode,
		       const char *player,
		       const char *msg)
{
	if (!room)
		return;

	/* FIXME: check validty of args */
	
	_ggzcore_room_chat(room, opcode, player, msg);
}


/* 
 * Internal library functions (prototypes in room.h) 
 * NOTE:All of these functions assume valid inputs!
 */


/* Create a new room object for a given server*/
struct _GGZRoom* _ggzcore_room_new(const struct _GGZServer *server,
				   const unsigned int id, 
				   const char* name, 
				   const unsigned int game, 
				   const char* desc)
{
	int i;
	struct _GGZRoom *room;

	/* Allocate and zero space for GGZRoom object */
	if (!(room = calloc(1, sizeof(GGZRoom))))
		ggzcore_error_sys_exit("malloc() failed in ggzcore_room_new");

	room->server = (struct _GGZServer*)server;
	room->id = id;
	room->name = strdup(name);
	room->game = game;

	/* Descriptions are optional */
	if (desc)
		room->desc = strdup(desc);
	
	/* FIXME: create player list? */
	/* FIXME: create table list? */

	/* Setup event hook list */
	for (i = 0; i < GGZ_NUM_ROOM_EVENTS; i++)
		room->event_hooks[i] = _ggzcore_hook_list_init(i);

	return room;
}


/* Return 0 if equal and -1 otherwise */
int _ggzcore_room_compare(void* p, void* q)
{
	if (((struct _GGZRoom*)p)->id == ((struct _GGZRoom*)q)->id)
		return 0;

	return -1;
}


/* Create a copy of a room object */
void* _ggzcore_room_copy(void* p)
{
	struct _GGZRoom *new, *src = p;

	new = _ggzcore_room_new(src->server, src->id, src->name, src->game,
				src->desc);
	
	return (void*)new;
}


void  _ggzcore_room_destroy(void* p)
{
	_ggzcore_room_free(p);
}



/* Functions to retrieve room information */
struct _GGZServer* _ggzcore_room_get_server(struct _GGZRoom *room)
{
	return room->server;
}


unsigned int _ggzcore_room_get_num(struct _GGZRoom *room)
{
	return room->id;
}


char* _ggzcore_room_get_name(struct _GGZRoom *room)
{
	return room->name;
}


unsigned int _ggzcore_room_get_game(struct _GGZRoom *room)
{
	return room->game;
}


char* _ggzcore_room_get_desc(struct _GGZRoom *room)
{
	return room->desc;
}


void _ggzcore_room_add_player(struct _GGZRoom *room, 
			      struct _GGZPlayer *player) {}

void _ggzcore_room_remove_player(struct _GGZRoom *room, char *name) {}

void _ggzcore_room_add_table(struct _GGZRoom *room, 
			     struct _GGZTable *table) {}

void _ggzcore_room_remove_table(struct _GGZRoom *room, int index) {}


void _ggzcore_room_add_chat(struct _GGZRoom *room, GGZChatOp op, char *name,
			    char *msg)
{
	char *data[2];

	data[0] = name;
	data[1] = msg;

	ggzcore_debug(GGZ_DBG_ROOM, "op = %d", op);

	switch(op) {
	case GGZ_CHAT_NORMAL:
		_ggzcore_room_event(room, GGZ_CHAT, data);
		break;
	case GGZ_CHAT_ANNOUNCE:
		_ggzcore_room_event(room, GGZ_ANNOUNCE, data);		
		break;
	case GGZ_CHAT_PERSONAL:
		_ggzcore_room_event(room, GGZ_PRVMSG, data);
		break;
	case GGZ_CHAT_BEEP:
		_ggzcore_room_event(room, GGZ_BEEP, data);
		break;
	}
}


void _ggzcore_room_chat(struct _GGZRoom *room,
			const GGZChatOp opcode,
			const char *player,
			const char *msg)
{
	_ggzcore_server_chat(room->server, opcode, player, msg);
}


/* Functions for attaching hooks to GGZRoom events */
int _ggzcore_room_add_event_hook_full(GGZRoom *room,
				      const GGZRoomEvent event, 
				      const GGZHookFunc func,
				      void *data)
{
	return _ggzcore_hook_add_full(room->event_hooks[event], func, data);
}


/* Functions for removing hooks from GGZRoom events */
int _ggzcore_room_remove_event_hook(GGZRoom *room,
				    const GGZRoomEvent event, 
				    const GGZHookFunc func) 
{
	return _ggzcore_hook_remove(room->event_hooks[event], func);
}


int _ggzcore_room_remove_event_hook_id(GGZRoom *room,
				       const GGZRoomEvent event, 
				       const unsigned int hook_id) 
{
	return _ggzcore_hook_remove_id(room->event_hooks[event], hook_id);
}


void _ggzcore_room_free(struct _GGZRoom *room)
{
	int i;
	
	if (room->name)
		free(room->name);

	if (room->desc)
		free(room->desc);

	if (room->players)
		_ggzcore_list_destroy(room->players);

	if (room->tables)
		_ggzcore_list_destroy(room->tables);

	for (i = 0; i < GGZ_NUM_ROOM_EVENTS; i++)
		_ggzcore_hook_list_destroy(room->event_hooks[i]);
	
	free(room);
}

/* Static functions internal to this file */

static int _ggzcore_room_event_is_valid(GGZRoomEvent event)
{
	return (event >= 0 && event < GGZ_NUM_ROOM_EVENTS);
}


static GGZHookReturn _ggzcore_room_event(GGZRoom *room, GGZRoomEvent id, 
					 void *data)
{
	return _ggzcore_hook_list_invoke(room->event_hooks[id], data);
}
