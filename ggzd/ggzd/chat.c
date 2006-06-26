/*
 * File: chat.c
 * Author: Brent Hendricks
 * Project: GGZ Server
 * Date: 5/10/00
 * Desc: Functions for handling/manipulating GGZ chat/messaging
 * $Id: chat.c 8273 2006-06-26 10:29:01Z oojah $
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

#ifdef HAVE_CONFIG_H
# include <config.h>		/* Site specific config */
#endif

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "chat.h"
#include "datatypes.h"
#include "err_func.h"
#include "event.h"
#include "ggzd.h"
#include "hash.h"
#include "net.h"
#include "perms.h"
#include "players.h"
#include "protocols.h"
#include "room.h"
#include "table.h"


/* Local data type */
typedef struct {
	GGZChatType type;
	const char *sender;
	const char *message;
} GGZChatEventData;

/* Local functions and callbacks */
static GGZChatEventData *chat_pack(GGZChatType type,
				   const char *sender, const char *message);
static void chat_free(void *event_data);
static GGZEventFuncReturn chat_event_callback(void* target, size_t size,
                                              void* event_data);
static GGZEventFuncReturn chat_table_event_callback(void *target, size_t size,
						    void *event_data);

/* Queue up a chat for the room */
GGZClientReqError chat_room_enqueue(int room, GGZChatType type,
				    GGZPlayer *sender, const char *msg)
{
	GGZChatEventData *data;
	int i, roomnum;

	/* A message to room -1 announces to all rooms */
	if(room == -1) {
		GGZClientReqError status = E_OK;
		if(perms_check(sender, PERMS_CHAT_ANNOUNCE) != PERMS_ALLOW)
			return E_NO_PERMISSION;
		roomnum = room_get_num_rooms();
		for(i=0; i<roomnum; i++) {
			GGZClientReqError result;
			if(rooms[i].removal_pending)
				continue;
			result = chat_room_enqueue(i, type, sender, msg);
			if (result != E_OK)
				status = result;
		}
		return status;
	}

	/* Pack up chat message */
	data = chat_pack(type, sender->name, msg);

	if (event_room_enqueue(room, chat_event_callback,
			       sizeof(*data), data, chat_free) != GGZ_OK)
		return E_BAD_XML; /* internal server error? */
	else
		return E_OK;
}


/* Queue up a chat for a table */
GGZClientReqError chat_table_enqueue(int room, int table, GGZChatType type,
				     GGZPlayer *sender, const char *msg)
{
	GGZChatEventData *data;

	if (room == -1 || table == -1)
		return E_NO_TABLE;

	data = chat_pack(type, sender->name, msg);

	/* We can't just loop over the players at the table to do an
	   event_player_enqueue on them - it's possible players might be
	   joining or leaving the table in other threads, and that would
	   be Bad.  So we must queue up a table event.  Unfortunately,
	   the table event is done once for the entire table - not once
	   per player - so in that event (in chat_table_event_callback)
	   we then have to loop over the players at the table and
	   enqueue the chat for them.  This extra step could be avoided
	   if there were an event_tableplayer_enqueue that performed
	   similarly to event_room_enqueue (but for tables).  --JDS */
	if (event_table_enqueue(room, table, chat_table_event_callback,
				sizeof(*data), data, chat_free) != GGZ_OK)
		return E_BAD_XML; /* internal server error? */

	return E_OK;
}


/* Queue up a chat for a player.  The sender's thread is the caller. */
GGZClientReqError chat_player_enqueue(const char* receiver, GGZChatType type, 
				      GGZPlayer* sender, const char *msg)

{
	GGZPlayer* rcvr;
	GGZChatEventData *data;
	bool at_table;

	if (receiver == NULL) {
		return E_BAD_OPTIONS;
	}

	pthread_rwlock_rdlock(&sender->lock);
	at_table = (sender->table != -1);
	pthread_rwlock_unlock(&sender->lock);	

	/* Don't allow personal chat from a player at a table */
	/* Players with the PERMS_TABLE_PRIVMSG permission can send 
	 * personal chat whilst at a table */
	if (at_table && !perms_check(sender, PERMS_TABLE_PRIVMSG)){
		return E_AT_TABLE;
	}

	/* Find target player.  Returns with player write-locked */
	rcvr = hash_player_lookup(receiver);

	if (rcvr == NULL) {
		return E_USR_LOOKUP;
	}

	/* Don't allow personal chat to a player at a table */
	at_table = (rcvr->table != -1);

	/* FIXME: we release the lock only to acquire it again in
	   event_player_enqueue.  This is inefficient. */
	pthread_rwlock_unlock(&rcvr->lock);

	/* Players with the PERMS_TABLE_PRIVMSG permission can receive
	 * personal chat whilst at a table */
	if (at_table && !perms_check(rcvr, PERMS_TABLE_PRIVMSG)) {
		return E_AT_TABLE;
	}

	/* Pack up chat message.  Sender->name won't change so no lock
	 * is needed. */
	data = chat_pack(type, sender->name, msg);
	
	/* Queue chat event for individual player */
	if (event_player_enqueue(receiver, chat_event_callback,
				 sizeof(*data), data, chat_free) != GGZ_OK) {
		return E_USR_LOOKUP;
	}

	return E_OK;
}


/* Return packaged chat message in dynamically allocated memory */
static GGZChatEventData *chat_pack(GGZChatType type,
				   const char* sender, const char* msg)
{
	GGZChatEventData *data;

	data = ggz_malloc(sizeof(*data));
	data->type = type;
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


/* Event callback for delivering chat to player.  This is called by the
   target player's thread when it wakes up to receive events. */
static GGZEventFuncReturn chat_event_callback(void* target, size_t size,
					      void* event_data)
{
	GGZPlayer* player = (GGZPlayer*)target;
	GGZChatEventData *data = event_data;

	dbg_msg(GGZ_DBG_CHAT, "%s chat opcode: %s, sender: %s, msg: %s",
		player->name, ggz_chattype_to_string(data->type),
		data->sender, data->message);

	if (net_send_chat(player->client->net, data->type,
			  data->sender, data->message) < 0)
		return GGZ_EVENT_ERROR;
		
	return GGZ_EVENT_OK;
}


static GGZEventFuncReturn chat_table_event_callback(void *target, size_t size,
						    void *event_data)
{
	GGZTable *table = target;
	GGZChatEventData *data = event_data;
	int i;
	GGZEventFuncReturn status = GGZ_EVENT_OK;

	/* We loop over all players at the table, and for each of them
	   enqueue a player event to handle the chat.  Note that we have
	   to repack the chat event data for each player, since it will
	   be freed after the *table* event has been handled.  This is
	   just another argument in favor of a event_tableplayer_enqueue()
	   function.  --JDS */

	for (i = 0; i < table->num_seats; i++) {
		GGZChatEventData *ndata;
		char *receiver = table->seat_names[i];

		if (table->seat_types[i] != GGZ_SEAT_PLAYER)
			continue;

		ndata = chat_pack(data->type, data->sender, data->message);
		if (event_player_enqueue(receiver, chat_event_callback,
					 sizeof(*ndata), ndata,
					 chat_free) != GGZ_OK)
			status = GGZ_EVENT_ERROR;
	}

	for (i = 0; i < table->max_num_spectators; i++) {
		GGZChatEventData *ndata;
		char *receiver = table->spectators[i];

		if (receiver[0] == '\0')
			continue;

		ndata = chat_pack(data->type, data->sender, data->message);
		if (event_player_enqueue(receiver, chat_event_callback,
					 sizeof(*ndata), ndata,
					 chat_free) != GGZ_OK)
			status = GGZ_EVENT_ERROR;
	}

	return status;
}


/* This is more or less a temporary hack for show_server_info() - room.c */
/* Although this could be a useful function for other uses. */
GGZReturn chat_server_2_player(const char *name, const char *msg)
{
	if (name) {
		/* Queue chat event for individual player */
		GGZChatEventData *data;

		/* Pack up chat message */
		data = chat_pack(GGZ_CHAT_PERSONAL, "[Server]", msg);

		return event_player_enqueue(name, chat_event_callback,
					    sizeof(*data), data, chat_free);
	} else {
		int roomnum = room_get_num_rooms(), i;
		GGZReturn status = GGZ_OK;

		for (i = 0; i < roomnum; i++) {
			GGZChatEventData *data;

			/*if(rooms[i].removal_pending)
				continue;*/

			data = chat_pack(GGZ_CHAT_ANNOUNCE, "[Server]", msg);

			if (event_room_enqueue(i, chat_event_callback,
					       sizeof(*data), data,
					       chat_free) != GGZ_OK)
				status = GGZ_ERROR;
		}

		return status;
	}
}
