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

#include <easysock.h>
#include <ggzd.h>
#include <datatypes.h>
#include <room.h>
#include <err_func.h>
#include <protocols.h>


/* Server wide data structures */
extern Options opt;
extern struct Users players;

/* Decl of server wide chat room structure */
RoomStruct *chat_room;

/* Internal use only */
static void room_spew_chat_room(const int);


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
/* (for now this just moves them and says "success" */
int room_join(const int p_index, const int room)
{
	int old_room;

	/* We ALWAYS lock players before chat rooms */
	pthread_rwlock_wrlock(&players.lock);
	old_room = players.info[p_index].room;

	/* The luser asked to stay in the same room! */
	if(old_room == room) {
		pthread_rwlock_unlock(&players.lock);
		return 0;
	}

	if(old_room != -1)
		pthread_rwlock_wrlock(&chat_room[old_room].lock);
	if(room != -1)
		pthread_rwlock_wrlock(&chat_room[room].lock);

	/* Yank them from this room */
	if(old_room != -1) {
		room_dequeue_chat(p_index);
		chat_room[old_room].player_count --;
		dbg_msg(GGZ_DBG_ROOM, "Room count %d = %d", old_room,
			chat_room[old_room].player_count);
	}

	/* Put them in the new room, and free up the old room */
	players.info[p_index].room = room;
	pthread_rwlock_unlock(&chat_room[old_room].lock);

	/* Adjust the new rooms statistics */
	if(room != -1) {
		chat_room[room].player_count ++;
		dbg_msg(GGZ_DBG_ROOM, "Room count %d = %d", room,
			chat_room[room].player_count);
	}

	if(players.info[p_index].chat_head != NULL) {
		err_msg("*** You triggered a memory leak! ***");
		players.info[p_index].chat_head = NULL;
	}

	/* Finally we can release the other locks */
	pthread_rwlock_unlock(&chat_room[room].lock);
	pthread_rwlock_unlock(&players.lock);

	dbg_msg(GGZ_DBG_ROOM,
		"Player %d moved from room %d to %d", p_index, old_room, room);

	return 0;
}


/* Queue up a chat emission for the room */
int room_emit(const int room, const int sender, char *msg)
{
	ChatItemStruct *new_chat;
	int i;

	/* Allocate a new chat item */
	if((new_chat = malloc(sizeof(ChatItemStruct))) == NULL)
		err_sys_exit("malloc failed in room_emit()");
	dbg_msg(GGZ_DBG_LISTS, "Allocated chat %p", new_chat);

	/* We ALWAYS lock players before chat rooms */
	pthread_rwlock_wrlock(&players.lock);
	pthread_rwlock_wrlock(&chat_room[room].lock);

	/* Nab the player's name while we've got lock */
	if((new_chat->chat_sender = malloc(strlen(players.info[sender].name)+1))
	   == NULL) {
		pthread_rwlock_unlock(&players.lock);
		pthread_rwlock_unlock(&chat_room[room].lock);
		free(msg);
		err_sys_exit("malloc failed in room_emit()");
	}
	strcpy(new_chat->chat_sender, players.info[sender].name);

	/* If players in this room don't have a chat head, put this item */
	for(i=0; i<MAX_USERS; i++)
		if((players.info[i].room == room)
		   && (players.info[i].chat_head == NULL))
			players.info[i].chat_head = new_chat;

	/* We're done with the player structures */
	pthread_rwlock_unlock(&players.lock);

	/* Now we can finish setting up the chat list item */
	new_chat->chat_msg = msg;
	new_chat->reference_count = chat_room[room].player_count;
	new_chat->next = NULL;

	/* Finally, add this to the list itself */
	if(chat_room[room].chat_tail != NULL)
		(chat_room[room].chat_tail)->next = new_chat;
	chat_room[room].chat_tail = new_chat;

#ifdef DEBUG
	if(chat_room[room].chat_head == NULL)
		chat_room[room].chat_head = new_chat;
	room_spew_chat_room(room);
#endif

	/* Phew */
	pthread_rwlock_unlock(&chat_room[room].lock);

	return 0;
}


/* Queue up a chat emission to a specific player */
int room_pemit(const int room, const int sender, char *stmt)
{
	free(stmt);

	return 0;
}


/* Send out all chat to a player from his current room */
int room_send_chat(const int p)
{
	ChatItemStruct *cur_chat;
	int room;
	int fd;

	pthread_rwlock_rdlock(&players.lock);
	fd = players.info[p].fd;
	room = players.info[p].room;
	cur_chat = players.info[p].chat_head;
	pthread_rwlock_unlock(&players.lock);

	/* We carefully sequence our locks so that:             */
	/* 1) Other threads can be in room_send_chat at the     */
	/*    same time as us, even in the same room            */
	/* 2) Chats can be added to the room, and the player    */
	/*    while we are dumping the chat.                    */
	/* This is safe assuming these (currently true) facts:  */
	/* 1) No one can remove a chat from our queue except us */
	/* 2) Anyone can alter our chat_head ONLY if it is NULL */
	/* 3) No one can alter or remove the chat strings while */
	/*    we still have a reference_count to it             */
	while(cur_chat) {
		if (es_write_int(fd, MSG_CHAT) < 0
		    || es_write_string(fd, cur_chat->chat_sender) < 0
		    || es_write_string(fd, cur_chat->chat_msg) < 0)
			return(-1);
		/* Now we gotta do strict locks */
		/* We ALWAYS lock players before chat rooms */
		pthread_rwlock_wrlock(&players.lock);
		pthread_rwlock_wrlock(&chat_room[room].lock);

		/* Update the player's chat_head */
		players.info[p].chat_head = cur_chat->next;
		pthread_rwlock_unlock(&players.lock);

		/* Update the chat_item, possibly removing it */
		cur_chat->reference_count --;
		if(cur_chat->reference_count == 0) {
			dbg_msg(GGZ_DBG_LISTS, "Removing chat %p", cur_chat);
			if(chat_room[room].chat_tail == cur_chat) {
				chat_room[room].chat_tail = NULL;
				free(cur_chat->chat_sender);
				free(cur_chat->chat_msg);
			}
#ifdef DEBUG
			chat_room[room].chat_head = cur_chat->next;
#endif DEBUG
			free(cur_chat);
		}
#ifdef DEBUG
		if(chat_room[room].chat_tail == NULL)
			chat_room[room].chat_head = NULL;
		room_spew_chat_room(room);
#endif
		pthread_rwlock_unlock(&chat_room[room].lock);

		/* Update cur_chat */
		pthread_rwlock_rdlock(&players.lock);
		cur_chat = players.info[p].chat_head;
		pthread_rwlock_unlock(&players.lock);
	}

	return 0;
}


/* Zap all chats to this player in the current room */
/* Note: this function MUST be called with the player structs and */
/*       the proper chat room already locked!!                    */
void room_dequeue_chat(const int p)
{
	ChatItemStruct *chat_item;
	int room = players.info[p].room;

	dbg_msg(GGZ_DBG_LISTS, "Dequeuing all room chat for %d in %d", p, room);

	while((chat_item = players.info[p].chat_head)) {
		players.info[p].chat_head = chat_item->next;
		chat_item->reference_count --;
		if(chat_item->reference_count == 0) {
			dbg_msg(GGZ_DBG_LISTS, "Removing chat %p", chat_item);
			if(chat_room[room].chat_tail == chat_item) {
				chat_room[room].chat_tail = NULL;
				free(chat_item->chat_sender);
				free(chat_item->chat_msg);
			}
#ifdef DEBUG
			chat_room[room].chat_head = chat_item->next;
#endif DEBUG
			free(chat_item);
		}
	}

#ifdef DEBUG
	if(chat_room[room].chat_tail == NULL)
		chat_room[room].chat_head = NULL;
	room_spew_chat_room(room);
#endif
}


/* Zap all personal chat to this player */
void room_dequeue_personal(const int p)
{

}


/* Spew out the entire chat list for a room */
#ifdef DEBUG
static void room_spew_chat_room(const int room)
{
	ChatItemStruct *chat_item;

	dbg_msg(GGZ_DBG_LISTS, "------ Chat List ------");
	chat_item = chat_room[room].chat_head;
	dbg_msg(GGZ_DBG_LISTS, "Chat head is %p", chat_item);
	while(chat_item != NULL) {
		dbg_msg(GGZ_DBG_LISTS, "  Chain item %p (%d)",
			chat_item, chat_item->reference_count);
		chat_item = chat_item->next;
	}
	dbg_msg(GGZ_DBG_LISTS, "Chat tail is %p", chat_room[room].chat_tail);
	dbg_msg(GGZ_DBG_LISTS, "-----------------------");
}
#endif /* DEBUG */
