/*
 * File: chat.c
 * Author: Brent Hendricks
 * Project: GGZ Server
 * Date: 5/10/00
 * Desc: Functions for handling/manipulating GGZ chat/messaging
 * $Id: chat.c 4534 2002-09-13 02:20:58Z jdorje $
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
#include <string.h>
#include <ctype.h>

#include <chat.h>
#include <datatypes.h>
#include <err_func.h>
#include <event.h>
#include <room.h>
#include <ggzd.h>
#include <protocols.h>
#include <hash.h>
#include <net.h>
#include <players.h>
#include <perms.h>
#include "client.h"


/* Server wide data structures */
extern Options opt;
extern struct GameInfo game_types[MAX_GAME_TYPES];


/* Local functions and callbacks */
static int chat_pack(void** data, unsigned char opcode, char* sender, 
		     char* msg);
static GGZEventFuncReturn chat_event_callback(void* target, size_t size,
                                              void* data);


/* Queue up a chat for the room */
int chat_room_enqueue(int room, unsigned char opcode, GGZPlayer* sender, 
		      char *msg)
{
	void* data = NULL;
	int size, status=0;
	int i, rooms;

	/* A message to room -1 announces to all rooms */
	if(room == -1) {
		if(perms_check(sender, PERMS_CHAT_ANNOUNCE) != PERMS_ALLOW)
			return E_NO_PERMISSION;
		rooms = room_get_num_rooms();
		for(i=0; i<rooms; i++)
			status = chat_room_enqueue(i, opcode, sender, msg);
		return status;
	}

	/* Pack up chat message */
	size = chat_pack(&data, opcode, sender->name, msg);

	status = event_room_enqueue(room, chat_event_callback,
				    size, data, NULL);
	
	return status;
}


/* Queue up a chat for a player */
int chat_player_enqueue(char* receiver, unsigned char opcode, 
			GGZPlayer* sender, char *msg)

{
	int size, status=0;
	GGZPlayer* rcvr = NULL;
	void *data = NULL;

	if(receiver == NULL)
		return E_BAD_OPTIONS;

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
	status = event_player_enqueue(receiver, chat_event_callback,
				      size, data, NULL);

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
	
	*data = ggz_malloc(size);
	
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
static GGZEventFuncReturn chat_event_callback(void* target, size_t size,
					      void* data)
{
	unsigned char opcode;
	char* name;
	char* msg = NULL;
	GGZPlayer* player = (GGZPlayer*)target;
	char* current = data;

	/* Unpack event data */
	opcode = *(unsigned char*)data;
	current += 1;

	name = current;
	current += strlen(name) + 1;

	if (opcode & GGZ_CHAT_M_MESSAGE)
		msg = current;
	
	dbg_msg(GGZ_DBG_CHAT, "%s chat opcode: %d, sender: %s, msg: %s",
		player->name, opcode, name, msg);

	if (net_send_chat(player->client->net, opcode, name, msg) < 0)
		return GGZ_EVENT_ERROR;
		
	return GGZ_EVENT_OK;
}


/* This is more or less a temporary hack for show_server_info() - room.c */
/* Although this could be a useful function for other uses. */
int chat_server_2_player(char *name, char *msg)
{
	void *data = NULL;
	int size, status;

	/* Pack up chat message */
	size = chat_pack(&data, GGZ_CHAT_PERSONAL, "[Server]", msg);
	
	/* Queue chat event for individual player */
	status = event_player_enqueue(name, chat_event_callback,
				      size, data, NULL);

	return status;
}
