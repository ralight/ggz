/*
 * File: roomlist.h
 * Author: Brent Hendricks
 * Project: GGZ Core Client Lib
 * Date: 1/24/01
 *
 * This fils contains functions for handling room lists
 *
 * Copyright (C) 2001 Brent Hendricks.
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

#ifndef __ROOMLIST_H__
#define __ROOMLIST_H__

#include "lists.h"
#include "ggzcore.h"

struct _GGZRoomList {

	/* Number of rooms */
	int num;
	
	/* List of rooms */
	struct _ggzcore_list *rooms;
};


GGZRoomList* _ggzcore_roomlist_new(void);

void _ggzcore_roomlist_free(GGZRoomList *list);

int _ggzcore_roomlist_add(GGZRoomList *list, 
			  const unsigned int id, 
			  const char* name, 
			  const unsigned int game, 
			  const char* desc);

int _ggzcore_roomlist_remove(GGZRoomList *list, 
			     const unsigned int id);

int _ggzcore_roomlist_replace(GGZRoomList *list,
			      const unsigned int id, 
			      const char* name, 
			      const unsigned int game, 
			      const char* desc);

int _ggzcore_roomlist_get_num(GGZRoomList *list);

char* _ggzcore_roomlist_get_room_name(GGZRoomList *list, 
				      const unsigned int id);

int _ggzcore_roomlist_get_room_type(GGZRoomList *list,
				    const unsigned int id);

char* _ggzcore_roomlist_get_room_desc(GGZRoomList *list,
				      const unsigned int id);

char** _ggzcore_roomlist_get_room_names(GGZRoomList *list);




#endif /* __ROOMLIST_H__ */
