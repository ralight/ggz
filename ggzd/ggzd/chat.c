/*
 * File: chat.c
 * Author: Brent Hendricks
 * Project: GGZ Server
 * Date: 5/10/00
 * Desc: Functions for handling/manipulating GGZ chat/messaging
 * $Id: chat.c 4555 2002-09-13 17:55:07Z jdorje $
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

/* Local data type */
typedef struct {
	unsigned char opcode;
	char *sender;
	char *message;
} GGZChatEventData;

/* Local functions and callbacks */
static GGZChatEventData *chat_pack(unsigned char opcode,
				   char *sender, char *message);
static void chat_free(void *event_data);
static GGZEventFuncReturn chat_event_callback(void* target, size_t size,
                                              void* event_data);

/* Queue up a chat for the room */
GGZReturn chat_room_enqueue(int room, unsigned char opcode,
			    GGZPlayer* sender, char *msg)
{
	GGZChatEventData *data;
	int i, rooms;

	/* A message to room -1 announces to all rooms */
	if(room == -1) {
		int status = GGZ_OK;
		if(perms_check(sender, PERMS_CHAT_ANNOUNCE) != PERMS_ALLOW)
			return E_NO_PERMISSION;
		rooms = room_get_num_rooms();
		for(i=0; i<rooms; i++)
			if (chat_room_enqueue(i, opcode,
					      sender, msg) != GGZ_OK)
				status = GGZ_ERROR;
		return status;
	}

	/* Pack up chat message */
	data = chat_pack(opcode, sender->name, msg);

	return event_room_enqueue(room, chat_event_callback,
				  sizeof(*data), data, chat_free);
}


/* Queue up a chat for a player */
GGZClientReqError chat_player_enqueue(char* receiver, unsigned char opcode, 
				      GGZPlayer* sender, char *msg)

{
	GGZPlayer* rcvr = NULL;
	GGZChatEventData *data;

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
	data = chat_pack(opcode, sender->name, msg);
	
	/* Queue chat event for individual player */
	if (event_player_enqueue(receiver, chat_event_callback,
				 sizeof(*data), data, chat_free) != GGZ_OK)
		return E_USR_LOOKUP; /* FIXME: is this correct? */

	return E_OK;
}


/* Return packaged chat message in dynamically allocated memory */
static GGZChatEventData *chat_pack(unsigned char opcode,
				   char* sender, char* msg)
{
	GGZChatEventData *data;

	data = ggz_malloc(sizeof(*data));
	data->opcode = opcode;
	data->sender = ggz_strdup(sender);
	data->message = ggz_strdup(msg);
	return data;
}


/* A callback function called to delete the GGZChatEvent data that is
   created in chat_pack() */
static void chat_free(void* event_data)
{
	GGZChatEventData *data = event_data;
	ggz_free(data->sender);
	if (data->message)
		ggz_free(data->message);
	ggz_free(data);
}


/* Event callback for delivering chat to player */
static GGZEventFuncReturn chat_event_callback(void* target, size_t size,
					      void* event_data)
{
	GGZPlayer* player = (GGZPlayer*)target;
	GGZChatEventData *data = event_data;

	dbg_msg(GGZ_DBG_CHAT, "%s chat opcode: %d, sender: %s, msg: %s",
		player->name, data->opcode, data->sender, data->message);

	if (net_send_chat(player->client->net, data->opcode,
			  data->sender, data->message) < 0)
		return GGZ_EVENT_ERROR;
		
	return GGZ_EVENT_OK;
}


/* This is more or less a temporary hack for show_server_info() - room.c */
/* Although this could be a useful function for other uses. */
GGZReturn chat_server_2_player(char *name, char *msg)
{
	GGZChatEventData *data;

	/* Pack up chat message */
	data = chat_pack(GGZ_CHAT_PERSONAL, "[Server]", msg);
	
	/* Queue chat event for individual player */
	return event_player_enqueue(name, chat_event_callback,
				      sizeof(*data), data, chat_free);
}
