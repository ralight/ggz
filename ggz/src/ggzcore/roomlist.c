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


#include "config.h"
#include "room.h"
#include "lists.h"
#include "msg.h"
#include "roomlist.h"

#include <stdlib.h>
#include <string.h>

/* 
 * The GGZRoomInfo stores information about a particular room (before
 * a player enters it)
 */
struct _GGZRoomInfo {
	
	/* Room ID (index) */
	unsigned int id;

	/* Name of room */
	char *name;	

	/* Supported game type*/
	unsigned int game;

	/* Room description */
	char *desc;
};


/* Local functions for manipulating room list */
static void _ggzcore_roomlist_print_all(GGZRoomList *list);
static void _ggzcore_roomlist_print_room(struct _GGZRoomInfo*);

/* Utility functions used by _ggzcore_list */
static int   _ggzcore_room_compare(void* p, void* q);
static void* _ggzcore_room_create(void* p);
static void  _ggzcore_room_destroy(void* p);


GGZRoomList* _ggzcore_roomlist_new(void)
{
	GGZRoomList *list;

	if ( (list = malloc(sizeof(GGZRoomList))) == NULL)
		ggzcore_error_sys_exit("malloc failed in roomlist_init");

	list->num = 0;
	list->rooms = _ggzcore_list_create(_ggzcore_room_compare,
					   _ggzcore_room_create,
					   _ggzcore_room_destroy,
					   0);

	return list;
}


void _ggzcore_roomlist_free(GGZRoomList *list)
{
	if (list) {
		list->num = -1;
		_ggzcore_list_destroy(list->rooms);
	}
}


int _ggzcore_roomlist_add(GGZRoomList *list,
			  const unsigned int id, 
			  const char* name, 
			  const unsigned int game, 
			  const char* desc)
{
	int status;
	struct _GGZRoomInfo room;

	if (!list)
		return -1;

	ggzcore_debug(GGZ_DBG_ROOM, "Adding room %d to room list", id);

	room.id = id;
	room.name = (char*)name;
	room.game = game;
	room.desc = (char*)desc;

	if ( (status = _ggzcore_list_insert(list->rooms, (void*)&room) == 0)) {
		_ggzcore_roomlist_print_all(list);
		list->num++;
	}
	
	return status;
}


int _ggzcore_roomlist_remove(GGZRoomList *list, const unsigned int id)
{
	struct _ggzcore_list_entry *entry;
	struct _GGZRoomInfo room;

	if (!list)
		return -1;

	ggzcore_debug(GGZ_DBG_ROOM, "Removing room %d from room list", id);
	
	room.id = id;
	if (!(entry = _ggzcore_list_search(list->rooms, &room)))
		return -1;
	
	_ggzcore_list_delete_entry(list->rooms, entry);
	_ggzcore_roomlist_print_all(list);
	list->num--;

	return 0;
}


int _ggzcore_roomlist_replace(GGZRoomList *list,
			      const unsigned int id, 
			      const char* name, 
			      const unsigned int game, 
			      const char* desc)
{
	struct _ggzcore_list_entry *entry;
	struct _GGZRoomInfo data, *room;
	
	if (!list)
		return -1;

	ggzcore_debug(GGZ_DBG_ROOM, "Updating info for room %d", id);

	data.id = id;
	if (!(entry = _ggzcore_list_search(list->rooms, &data)))
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

	_ggzcore_roomlist_print_all(list);

	return 0;
}


int _ggzcore_roomlist_get_num(GGZRoomList *list)
{
	if (!list)
		return -1;

	return list->num;
}


char* _ggzcore_roomlist_get_room_name(GGZRoomList *list, const unsigned int id)
{
	struct _ggzcore_list_entry *entry;
	struct _GGZRoomInfo data, *room;
	
	if (!list)
		return NULL;

	data.id = id;
	if (!(entry = _ggzcore_list_search(list->rooms, &data)))
		return NULL;
	
	room = _ggzcore_list_get_data(entry);
	
	return room->name;
}


int _ggzcore_roomlist_get_room_type(GGZRoomList *list, const unsigned int id)
{
	struct _ggzcore_list_entry *entry;
	struct _GGZRoomInfo data, *room;

	if (!list)
		return -1;

	data.id = id;
	if (!(entry = _ggzcore_list_search(list->rooms, &data)))
		return -1;

	room = _ggzcore_list_get_data(entry);
	
	return room->game;
}


char* _ggzcore_roomlist_get_room_desc(GGZRoomList *list, const unsigned int id)
{
	struct _ggzcore_list_entry *entry;
	struct _GGZRoomInfo data, *room;
	
	if (!list)
		return NULL;

	data.id = id;
	if (!(entry = _ggzcore_list_search(list->rooms, &data)))
		return NULL;

	room = _ggzcore_list_get_data(entry);
	
	return room->desc;
}


char** _ggzcore_roomlist_get_room_names(GGZRoomList *list)
{
	int i = 0;
	char **names = NULL;
	struct _ggzcore_list_entry *cur;
	struct _GGZRoomInfo *room;
	
	if (!list)
		return NULL;

	if (!(names = calloc((list->num + 1), sizeof(char*))))
		ggzcore_error_sys_exit("calloc() failed in room_get_names");
	cur = _ggzcore_list_head(list->rooms);
	while (cur) {
		room = _ggzcore_list_get_data(cur);
		names[i++] = room->name;
		cur = _ggzcore_list_next(cur);
	}
	
	return names;
}


/* Return 0 if equal and -1 otherwise */
static int _ggzcore_room_compare(void* p, void* q)
{
	if (((struct _GGZRoomInfo*)p)->id == ((struct _GGZRoomInfo*)q)->id)
		return 0;

	return -1;
}


static void* _ggzcore_room_create(void* p)
{
	struct _GGZRoomInfo *new, *src = p;

	if (!(new = malloc(sizeof(struct _GGZRoomInfo))))
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
	struct _GGZRoomInfo *room = p;

	if (room->name)
		free(room->name);
	if (room->desc)
		free(room->desc);
	
	free(p);
}


static void _ggzcore_roomlist_print_all(GGZRoomList *list)
{
	struct _ggzcore_list_entry *cur;
	
	for (cur = _ggzcore_list_head(list->rooms); cur; cur = _ggzcore_list_next(cur))
		_ggzcore_roomlist_print_room(_ggzcore_list_get_data(cur));
}


static void _ggzcore_roomlist_print_room(struct _GGZRoomInfo *room)
{
	ggzcore_debug(GGZ_DBG_ROOM, "Room %d:", room->id);
	ggzcore_debug(GGZ_DBG_ROOM, "  name: %s", room->name);
	ggzcore_debug(GGZ_DBG_ROOM, "  game: %d", room->game);
	ggzcore_debug(GGZ_DBG_ROOM, "  desc: %s", room->desc);
}

