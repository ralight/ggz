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
extern struct GameTypes game_types;

/* Decl of server wide chat room structure */
RoomStruct *chat_room;

/* Internal use only */
static void room_spew_chat_room(const int);
static void room_notify_change(const int, const int, const int);
static void room_dequeue_chat(const int p);
static int room_list_send(const int p_fd);
static int room_handle_join(const int, const int);


/* Handle opcodes from player_handle() */
int room_handle_request(const int request, const int p, const int p_fd)
{
	int status = GGZ_REQ_OK;

	switch(request) {
		case REQ_LIST_ROOMS:
			status = room_list_send(p_fd);
			break;
		case REQ_ROOM_JOIN:
			status = room_handle_join(p, p_fd);
			break;
		default:
			/* player_handle() sent us an invalid opcode	*/
			/* this is utterly impossible and so we mark it */
			/* with something utterly inexplicable, and	*/
			/* do the only honorable thing....		*/

			err_msg("Truth is false and logic lost");
			err_msg("Now the fourth dimension is crossed");
			err_msg_exit("--in room_handle_request()");

			/* Suicide is painless -- Not Reached */
			break;
	}

	return status;
}


/* Handle a REQ_LIST_ROOMS opcode */
static int room_list_send(const int p_fd)
{
	int req_game;
	char verbose;
	int i, count=0;

	/* We don't need to lock anything because CURRENTLY the room count  */
	/* and options can change ONLY before threads are in existence	    */

	/* Get the options from teh client */
	if(es_read_int(p_fd, &req_game) < 0
	   || es_read_char(p_fd, &verbose) < 0)
		return -1;

	if((verbose != 0 && verbose != 1)
	   || req_game < -1 || req_game >= game_types.count) {
		/* Invalid Options Sent */
		if(es_write_int(p_fd, RSP_LIST_ROOMS) < 0
		   || es_write_int(p_fd, E_BAD_OPTIONS) < 0)
			return -1;
	}

	/* First we have to figure out how many rooms to announce  */
	/* This is easy if a req_game filter hasn't been specified */
	if(req_game != -1) {
		for(i=0; i<opt.num_rooms; i++)
			if(req_game == chat_room[i].game_type)
				count++;
	} else
		count = opt.num_rooms;

	/* Do da opcode, and announce our count */
	if(es_write_int(p_fd, RSP_LIST_ROOMS) < 0
	   || es_write_int(p_fd, count) < 0)
		return -1;

	/* Send off all the room announcements */
	for(i=0; i<opt.num_rooms; i++)
		if(req_game == -1 || req_game == chat_room[i].game_type) {
			if(es_write_int(p_fd, i) < 0
			   || es_write_string(p_fd, chat_room[i].name) < 0
			   || es_write_int(p_fd, chat_room[i].game_type) < 0)
				return -1;
			if(verbose
			   && es_write_string(p_fd, chat_room[i].description)<0)
				return -1;
		}

	return 0;
}


/* Initialize the first room */
void room_initialize(void)
{
	dbg_msg(GGZ_DBG_ROOM, "Initializing room array");

	opt.num_rooms=1;

	/* Calloc a big enough array to hold all our first room */
	if((chat_room = calloc(opt.num_rooms, sizeof(RoomStruct))) == NULL)
		err_sys_exit("calloc failed in room_initialize_lists()");

	/* Initialize the chat_tail and lock */
	chat_room[0].chat_tail = NULL;
	pthread_rwlock_init(&chat_room[0].lock, NULL);
#ifdef DEBUG
	chat_room[0].chat_head = NULL;
#endif
}


/* Initialize an additional room */
void room_create_additional(void)
{
	/* Right now this is only used at startup, so we don't lock anything */
	dbg_msg(GGZ_DBG_ROOM, "Creating a new room");

	opt.num_rooms++;

	/* Realloc the chat_room array */
	chat_room = realloc(chat_room, opt.num_rooms * sizeof(RoomStruct));
	if(chat_room == NULL)
		err_sys_exit("realloc failed in room_create_new()");

	/* Initialize the chat_tail and lock on the new one */
	chat_room[opt.num_rooms-1].chat_tail = NULL;
	pthread_rwlock_init(&chat_room[opt.num_rooms-1].lock, NULL);
#ifdef DEBUG
	chat_room[opt.num_rooms-1].chat_head = NULL;
#endif
}


/* Handle the REQ_ROOM_JOIN opcode */
int room_handle_join(const int p_index, const int p_fd)
{
	int room, result;

	/* Get the user's room request */
	if(es_read_int(p_fd, &room) < 0)
		return -1;

	/* Check for silliness from the user */
	if(room > opt.num_rooms || room < 0)
		if(es_write_int(p_fd, RSP_ROOM_JOIN) < 0
		   || es_write_char(p_fd, E_BAD_OPTIONS) < 0)
			return -1;

	/* Do the actual room change, and return results */
	result = room_join(p_index, room);
	if(es_write_int(p_fd, RSP_ROOM_JOIN) < 0
	   || es_write_char(p_fd, result) < 0)
		return -1;

	return 0;
}


/* Join a player to a room, returns explanatory code on failure */
int room_join(const int p_index, const int room)
{
	int old_room;
	int i, count, last;

	/* No other thread could possibly be changing the room # */
	/* so we can read it without a lock! */
	old_room = players.info[p_index].room;

	/* Check for valid inputs */
	if(old_room == room)
		return 0;
	if(room > opt.num_rooms || room < -2)
		return E_BAD_OPTIONS;

	/* Give 'em their queued messages */
	room_send_chat(p_index);

	/* We ALWAYS lock the lower ordered room first! */
	if(old_room < room) {
		if(old_room != -1)
			pthread_rwlock_wrlock(&chat_room[old_room].lock);
		pthread_rwlock_wrlock(&chat_room[room].lock);
	} else {
		if(room != -1)
			pthread_rwlock_wrlock(&chat_room[room].lock);
		pthread_rwlock_wrlock(&chat_room[old_room].lock);
	}

	/* Check for room full condition */
	if(room != -1
	   && chat_room[room].player_count == chat_room[room].max_players) {
		pthread_rwlock_unlock(&chat_room[room].lock);
		if(old_room != -1)
			pthread_rwlock_unlock(&chat_room[old_room].lock);
		return E_ROOM_FULL;
	}

	/* Yank them from this room */
	if(old_room != -1) {
		if(players.info[p_index].chat_head != NULL)
			room_dequeue_chat(p_index);
		count = chat_room[old_room].player_count;
		last = chat_room[old_room].player_index[count-1];
		for(i=0; i<count; i++)
			if(chat_room[old_room].player_index[i] == p_index)
				chat_room[old_room].player_index[i] = last;
		chat_room[old_room].player_count --;
		dbg_msg(GGZ_DBG_ROOM, "Room count %d = %d", old_room,
			chat_room[old_room].player_count);
	}

	/* Put them in the new room, and free up the old room */
	players.info[p_index].room = room;
	if(old_room != -1)
		pthread_rwlock_unlock(&chat_room[old_room].lock);

	/* Adjust the new rooms statistics */
	if(room != -1) {
		count = ++ chat_room[room].player_count;
		chat_room[room].player_index[count-1] = p_index;
		dbg_msg(GGZ_DBG_ROOM, "Room count %d = %d", room, count);
	}

	/* Finally we can release the other chat room lock */
	if(room != -1)
		pthread_rwlock_unlock(&chat_room[room].lock);

	room_notify_change(p_index, old_room, room);

	return 0;
}


/* Notifies clients that someone has entered/left the room */
static void room_notify_change(const int p, const int old, const int new)
{
	char *part_msg, *join_msg;

	dbg_msg(GGZ_DBG_ROOM,
		"Player %d moved from room %d to %d", p, old, new);

	if(old != -1) {
		if((part_msg = malloc(8)) == NULL)
			err_sys_exit("malloc error in room_notify_change");
		strcpy(part_msg, "/SpartS");
		room_emit(old, p, part_msg);
	}

	if(new != -1) {
		if((join_msg = malloc(8)) == NULL)
			err_sys_exit("malloc error in room_notify_change");
		strcpy(join_msg, "/SjoinS");
		room_emit(new, p, join_msg);
	}
}


/* Queue up a chat emission for the room */
/* Messages passed to room_emit() MUST be dynamically allocated via malloc */
/* the caller MUST NOT free the msg as we need to keep it!                 */
int room_emit(const int room, const int sender, char *msg)
{
	ChatItemStruct *new_chat;
	int i, t_p;

	/* Allocate a new chat item */
	if((new_chat = malloc(sizeof(ChatItemStruct))) == NULL)
		err_sys_exit("malloc failed in room_emit()");
	dbg_msg(GGZ_DBG_LISTS, "Allocated chat %p", new_chat);

	pthread_rwlock_wrlock(&chat_room[room].lock);

	/*Let's not assume anyone is here (might be player exiting empty room)*/
	if(chat_room[room].player_count == 0) {
		pthread_rwlock_unlock(&chat_room[room].lock);
		free(new_chat);
		free(msg);
		dbg_msg(GGZ_DBG_LISTS,
			"Deallocated chat %p (empty room)", new_chat);
		return 0;
	}

	/* Since we are the sender, we don't need lock to get our own name */
	if((new_chat->chat_sender = malloc(strlen(players.info[sender].name)+1))
	   == NULL) {
		pthread_rwlock_unlock(&chat_room[room].lock);
		free(new_chat);
		free(msg);
		err_sys_exit("malloc failed in room_emit()");
	}
	strcpy(new_chat->chat_sender, players.info[sender].name);

	/* If players in this room don't have a chat head, put this item */
	for(i=0; i<chat_room[room].player_count; i++) {
		t_p = chat_room[room].player_index[i];
		if(players.info[t_p].chat_head == NULL)
			players.info[t_p].chat_head = new_chat;
	}

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
		pthread_rwlock_rdlock(&chat_room[room].lock);
		if (es_write_int(fd, MSG_CHAT) < 0
		    || es_write_string(fd, cur_chat->chat_sender) < 0
		    || es_write_string(fd, cur_chat->chat_msg) < 0)
			return(-1);
		pthread_rwlock_unlock(&chat_room[room].lock);

		/* Need write lock to update player chat head and ref count */
		pthread_rwlock_wrlock(&chat_room[room].lock);
		players.info[p].chat_head = cur_chat->next;
		cur_chat->reference_count --;
		pthread_rwlock_unlock(&chat_room[room].lock);

		pthread_rwlock_rdlock(&chat_room[room].lock);
		if(cur_chat->reference_count == 0) {
			dbg_msg(GGZ_DBG_LISTS, "Removing chat %p", cur_chat);
			pthread_rwlock_unlock(&chat_room[room].lock);
			pthread_rwlock_wrlock(&chat_room[room].lock);
			if(chat_room[room].chat_tail == cur_chat) {
				chat_room[room].chat_tail = NULL;
			}
#ifdef DEBUG
			chat_room[room].chat_head = cur_chat->next;
#endif DEBUG
			pthread_rwlock_unlock(&chat_room[room].lock);
			free(cur_chat->chat_sender);
			free(cur_chat->chat_msg);
			free(cur_chat);
			pthread_rwlock_rdlock(&chat_room[room].lock);
		}
#ifdef DEBUG
		if(chat_room[room].chat_tail == NULL)
			chat_room[room].chat_head = NULL;
		room_spew_chat_room(room);
#endif

		/* Update cur_chat */
		cur_chat = players.info[p].chat_head;
		pthread_rwlock_unlock(&chat_room[room].lock);
	}

	return 0;
}


/* Zap all chats to this player in the current room */
/* Note: this function MUST be called with the chat room already locked! */
static void room_dequeue_chat(const int p)
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
			}
#ifdef DEBUG
			chat_room[room].chat_head = chat_item->next;
#endif DEBUG
			free(chat_item->chat_sender);
			free(chat_item->chat_msg);
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
/* To make a consistent view, read lock should be held when calling this dbg */
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
