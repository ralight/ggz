/*
 * File: room.c
 * Author: Brent Hendricks
 * Project: GGZ Server
 * Date: 3/20/00
 * Desc: Functions for interfacing with room and chat facility
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

#include <config.h>

#include <pthread.h>
#include <string.h>
#include <stdlib.h>

#include <ggzd.h>
#include <datatypes.h>
#include <room.h>
#include <err_func.h>


/* Server wide data structures */
extern Options opt;

/* Decl of server wide chat room structure */
RoomStruct *chat_room;


/* Initialize the chat lists */
void room_initialize_lists(void)
{
	int	i;

	dbg_msg(GGZ_DBG_ROOM, "Initializing %d rooms", opt.num_rooms);

	/* Calloc a big enough array to hold all our rooms */
	if((chat_room = calloc(opt.num_rooms, sizeof(RoomStruct)))
	    == NULL)
		err_sys_exit("calloc failed in chat_initialize_lists()");

	/* Initialize all vars for each room */
	for(i=0; i<opt.num_rooms; i++) {
		pthread_rwlock_init(&chat_room[i].lock, NULL);
		chat_room[i].chat_tail = NULL; /* Not strictly necessary */
	}
}


/* Join a player to a room */
/* (for now this just returns the requested room, later it should check */
/* room parameters to see if the player can enter			*/
int room_join(int room)
{
	return room;
}


/* Queue up a chat emission for the room */
int room_emit(int room, char *msg)
{
	return 0;
}


/* Queue up a chat emission to a specific player */
int room_pemit(int room, char *stmt)
{
	return 0;
}


/* Zap all chats to this player in the current room */
void room_dequeue_chat(int p)
{

}


/* Zap all personal chat to this player */
void room_dequeue_personal(int p)
{

}
