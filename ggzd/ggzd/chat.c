/*
 * File: chat.c
 * Author: Brent Hendricks
 * Project: GGZ Server
 * Date: 5/10/00
 * Desc: Functions for handling/manipulating GGZ chat/messaging
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

#include <stdlib.h>
#include <easysock.h>
#include <string.h>

#include <chat.h>
#include <datatypes.h>
#include <err_func.h>
#include <event.h>
#include <room.h>
#include <ggzd.h>
#include <protocols.h>
#include <hash.h>
#include <players.h>

/* Server wide data structures */
extern Options opt;
extern struct GameInfo game_types[MAX_GAME_TYPES];


/* Local functions and callbacks */
static int chat_pack(void** data, unsigned char opcode, char* sender, 
		     char* msg);
static int chat_event_callback(void* target, int size, void* data);


/* Queue up a chat for the room */
int chat_room_enqueue(int room, unsigned char opcode, GGZPlayer* sender, 
		      char *msg)
{
	void* data = NULL;
	int size, status;

	/* Pack up chat message */
	size = chat_pack(&data, opcode, sender->name, msg);

	/* Queue chat event for whole room */
	status = event_room_enqueue(room, chat_event_callback, size, data);
	
	return status;
}


/* Queue up a chat for a player */
int chat_player_enqueue(char* receiver, unsigned char opcode, 
			GGZPlayer* sender, char *msg)

{
	int size, status=0;
	GGZPlayer* rcvr = NULL;
	void *data = NULL;

	/* Don't allow personal chat from a player at a table */
	pthread_rwlock_rdlock(&sender->lock);
	if (sender->table != -1) {
		pthread_rwlock_unlock(&sender->lock);
		return E_AT_TABLE;
	}
	pthread_rwlock_unlock(&sender->lock);	

	/* Find target player.  Returns with player write-locked */
	if ( (rcvr = hash_player_lookup(receiver)) == NULL )
		return E_USR_LOOKUP;
	
	/* Don't allow personal chat to a player at a table */
	if (rcvr->table != -1) {
		pthread_rwlock_unlock(&rcvr->lock);
		return E_AT_TABLE;
	}
	pthread_rwlock_unlock(&rcvr->lock);

	/* Pack up chat message */
	size = chat_pack(&data, opcode, sender->name, msg);
	
	/* Queue chat event for individual player */
	status = event_player_enqueue(receiver, chat_event_callback, size, 
				      data);

	return status;
}


/* Return packaged chat message in dynamically allocated memory */
static int chat_pack(void** data, unsigned char opcode, char* sender, 
		     char* msg)
{
	char* current;
	int size;

	/* Always package opcode and sender (and a null byte) */
	size = sizeof(char) + strlen(sender) + 1;
	
	/* Add on message (and null byte) if it's there */
	if (msg)
		size += strlen(msg) + 1;
	
	if ( (*data = malloc(size)) == NULL)
		err_sys_exit("malloc failed in chat_room_enqueue");
	
	/* Package data */
	current = (char*)*data;
	
	*(unsigned char*)current = opcode;
	current += sizeof(char);
	
	strcpy(current, sender);
	current += (strlen(sender) + 1);
		
	if (msg) {
		strcpy(current, msg);
		current += strlen(msg) + 1;
	}

	return size;
}


/* Event callback for delivering chat to player */
static int chat_event_callback(void* target, int size, void* data)
{
	unsigned char opcode;
	char* name;
	char* msg = NULL;
	GGZPlayer* player = (GGZPlayer*)target;
	int fd = player->fd;
	

	/* Unpack event data */
	opcode = *(unsigned char*)data;
	name = (char*)(data + sizeof(char));

	if (opcode & GGZ_CHAT_M_MESSAGE)
		msg = (char*)(data + sizeof(char) + strlen(name) + 1);
	
	dbg_msg(GGZ_DBG_CHAT, "%s chat opcode: %d, sender: %s, msg: %s",
		player->name, opcode, name, msg);

	if (es_write_int(fd, MSG_CHAT) < 0
	    || es_write_char(fd, opcode) < 0
	    || es_write_string(fd, name) < 0)
		return GGZ_EVENT_ERROR;

	if (opcode & GGZ_CHAT_M_MESSAGE
	    && es_write_string(fd, msg) < 0)
		return GGZ_EVENT_ERROR;
	
	return GGZ_EVENT_OK;
}


