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


#include <config.h>
#include <room.h>
#include <lists.h>
#include <msg.h>

#include <stdlib.h>
#include <string.h>

/* 
 * The Room structure is meant to be a node in a linked list of
 * the rooms on the server 
 */
struct _GGZRoom {
	
	/* Room ID (index) */
	unsigned int id;

	/* Name of room */
	char *name;	

	/* Supported game type*/
	unsigned int game;

	/* Room description */
	char *desc;
};


/* List of rooms on the server */
static struct _ggzcore_list *room_list;

/* Local functions for manipulating room list */
static void _ggzcore_room_list_print(void);
static void _ggzcore_room_print(struct _GGZRoom*);

/* Utility functions used by _ggzcore_list */
static int   _ggzcore_room_compare(void* p, void* q);
static void* _ggzcore_room_create(void* p);
static void  _ggzcore_room_destroy(void* p);


void _ggzcore_room_list_clear(void)
{
	if (room_list)
		_ggzcore_list_destroy(room_list);
	
	room_list = _ggzcore_list_create(_ggzcore_room_compare,
					 _ggzcore_room_create,
					 _ggzcore_room_destroy,
					 0);
}


int _ggzcore_room_list_add(const unsigned int id, const char* name, 
			   const unsigned int game, const char* desc)
{
	int status;
	struct _GGZRoom room;

	ggzcore_debug(GGZ_DBG_PLAYER, "Adding room %d to room list", id);
	
	room.id = id;
	room.name = (char*)name;
	room.game = game;
	room.desc = (char*)desc;

	status = _ggzcore_list_insert(room_list, (void*)&room);
	_ggzcore_room_list_print();

	return status;
}


int _ggzcore_room_list_remove(const unsigned int id)
{
	struct _ggzcore_list_entry *entry;
	struct _GGZRoom room;

	ggzcore_debug(GGZ_DBG_ROOM, "Removing room %d from room list", id);
	
	room.id = id;
	if (!(entry = _ggzcore_list_search(room_list, &room)))
		return -1;

	_ggzcore_list_delete_entry(room_list, entry);
	_ggzcore_room_list_print();

	return 0;
}


int _ggzcore_room_list_replace(const unsigned int id, const char* name, 
			       const unsigned int game, const char* desc)
{
	struct _ggzcore_list_entry *entry;
	struct _GGZRoom data, *room;
	
	ggzcore_debug(GGZ_DBG_ROOM, "Updating info for room %d", id);

	data.id = id;
	if (!(entry = _ggzcore_list_search(room_list, &data)))
		return -1;

	/* Update information */
	room = _ggzcore_list_get_data(entry);
	if (room->name)
		free(room->name);

	if (room->desc)
		free(room->desc);

	room->name = strdup(name);
	room->game = game;
	if (desc)
		room->desc = strdup(desc);
	else
		room->desc = NULL;

	_ggzcore_room_list_print();

	return 0;
}


static int _ggzcore_room_compare(void* p, void* q)
{
	return (((struct _GGZRoom*)p)->id == ((struct _GGZRoom*)q)->id);
}


static void* _ggzcore_room_create(void* p)
{
	struct _GGZRoom *new, *src = p;

	if (!(new = malloc(sizeof(struct _GGZRoom))))
		ggzcore_error_sys_exit("malloc failed in room_create");

	new->id = src->id;
	new->name = strdup(src->name);
	new->game = src->game;
	if (src->desc)
		new->desc = strdup(src->desc);
	else
		new->desc = NULL;

	return (void*)new;
}


static void  _ggzcore_room_destroy(void* p)
{
	struct _GGZRoom *room = p;

	if (room->name)
		free(room->name);
	if (room->desc)
		free(room->desc);
	
	free(p);
}


static void _ggzcore_room_list_print(void)
{
	struct _ggzcore_list_entry *cur;
	
	for (cur = _ggzcore_list_head(room_list); cur; cur = _ggzcore_list_next(cur))
		_ggzcore_room_print(_ggzcore_list_get_data(cur));
}


static void _ggzcore_room_print(struct _GGZRoom *room)
{
	ggzcore_debug(GGZ_DBG_ROOM, "Room %d:", room->id);
	ggzcore_debug(GGZ_DBG_ROOM, "  name: %s", room->name);
	ggzcore_debug(GGZ_DBG_ROOM, "  game: %d", room->game);
	ggzcore_debug(GGZ_DBG_ROOM, "  desc: %s", room->desc);
}
