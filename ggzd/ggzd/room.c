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
#include <event.h>
#include <chat.h>


/* Server wide data structures */
extern Options opt;
extern struct GGZState state;
extern struct GameInfo game_types[MAX_GAME_TYPES];

/* Decl of server wide chat room structure */
RoomStruct *rooms;
RoomInfo room_info;

/* Internal use only */
static void room_notify_change(char* name, const int, const int);
static int room_list_send(GGZPlayer* player, const int p_fd);
static int room_handle_join(GGZPlayer* player, const int);
static int room_event_callback(GGZPlayer* player, int size, void* data);
static int show_server_info(GGZPlayer *player, const int p_fd);

/* Handle opcodes from player_handle() */
int room_handle_request(const int request, GGZPlayer* player, const int p_fd)
{
	int status = GGZ_REQ_OK;

	switch(request) {
		case REQ_LIST_ROOMS:
			status = room_list_send(player, p_fd);
			break;
		case REQ_ROOM_JOIN:
			status = room_handle_join(player, p_fd);
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
static int room_list_send(GGZPlayer* player, const int p_fd)
{
	int req_game;
	char verbose;
	int i, max, count=0;

	/* We don't need to lock anything because CURRENTLY the room count  */
	/* and options can change ONLY before threads are in existence	    */
		
	/* Get the options from teh client */
	if(es_read_int(p_fd, &req_game) < 0
	   || es_read_char(p_fd, &verbose) < 0)
		return GGZ_REQ_DISCONNECT;

	/* Don't send list if they're not logged in */
	if (player->uid == GGZ_UID_NONE) {
		if (es_write_int(p_fd, RSP_LIST_ROOMS) < 0
		    || es_write_int(p_fd, E_NOT_LOGGED_IN) < 0)
			return GGZ_REQ_DISCONNECT;
		return GGZ_REQ_FAIL;
	}
	
	pthread_rwlock_rdlock(&state.lock);
	max = state.types;
	pthread_rwlock_unlock(&state.lock);	

	if((verbose != 0 && verbose != 1) || req_game < -1 || req_game >= max){
		/* Invalid Options Sent */
		if(es_write_int(p_fd, RSP_LIST_ROOMS) < 0
		   || es_write_int(p_fd, E_BAD_OPTIONS) < 0)
			return GGZ_REQ_DISCONNECT;
		return GGZ_REQ_FAIL;
	}

	/* First we have to figure out how many rooms to announce  */
	/* This is easy if a req_game filter hasn't been specified */
	if(req_game != -1) {
		for(i=0; i<room_info.num_rooms; i++)
			if(req_game == rooms[i].game_type)
				count++;
	} else
		count = room_info.num_rooms;

	/* Do da opcode, and announce our count */
	if(es_write_int(p_fd, RSP_LIST_ROOMS) < 0
	   || es_write_int(p_fd, count) < 0)
		return GGZ_REQ_DISCONNECT;

	/* Send off all the room announcements */
	for(i=0; i<room_info.num_rooms; i++)
		if(req_game == -1 || req_game == rooms[i].game_type) {
			if(es_write_int(p_fd, i) < 0
			   || es_write_string(p_fd, rooms[i].name) < 0
			   || es_write_int(p_fd, rooms[i].game_type) < 0)
				return GGZ_REQ_DISCONNECT;
			if(verbose
			   && es_write_string(p_fd, rooms[i].description)<0)
				return GGZ_REQ_DISCONNECT;
		}

	return GGZ_REQ_OK;
}


/* Initialize the first room */
void room_initialize(void)
{
	dbg_msg(GGZ_DBG_ROOM, "Initializing room array");

	room_info.num_rooms=1;

	/* Calloc a big enough array to hold all our first room */
	if((rooms = calloc(room_info.num_rooms, sizeof(RoomStruct))) == NULL)
		err_sys_exit("calloc failed in room_initialize_lists()");

	/* Initialize the chat_tail and lock */
	rooms[0].event_tail = NULL;
	pthread_rwlock_init(&rooms[0].lock, NULL);
#ifdef DEBUG
	rooms[0].event_head = NULL;
#endif
}


/* Initialize an additional room */
void room_create_additional(void)
{
	/* Right now this is only used at startup, so we don't lock anything */
	dbg_msg(GGZ_DBG_ROOM, "Creating a new room");

	room_info.num_rooms++;

	/* Realloc the rooms array */
	rooms = realloc(rooms, room_info.num_rooms * sizeof(RoomStruct));
	if(rooms == NULL)
		err_sys_exit("realloc failed in room_create_new()");

	/* Initialize the chat_tail and lock on the new one */
	rooms[room_info.num_rooms-1].player_count = 0;
	rooms[room_info.num_rooms-1].table_count = 0;
	rooms[room_info.num_rooms-1].event_tail = NULL;
	pthread_rwlock_init(&rooms[room_info.num_rooms-1].lock, NULL);
#ifdef DEBUG
	rooms[room_info.num_rooms-1].event_head = NULL;
#endif
}


/* Handle the REQ_ROOM_JOIN opcode */
int room_handle_join(GGZPlayer* player, const int p_fd)
{
	int room, result;
	
	/* Get the user's room request */
	if(es_read_int(p_fd, &room) < 0)
		return GGZ_REQ_DISCONNECT;

	/* Check for silliness from the user */
	if (player->table != -1 || player->launching) {
		if(es_write_int(p_fd, RSP_ROOM_JOIN) < 0
		   || es_write_char(p_fd, E_AT_TABLE) < 0)
			return GGZ_REQ_DISCONNECT;
		return GGZ_REQ_FAIL;
	}

	if (player->transit) {
		if(es_write_int(p_fd, RSP_ROOM_JOIN) < 0
		   || es_write_char(p_fd, E_IN_TRANSIT) < 0)
			return GGZ_REQ_DISCONNECT;
		return GGZ_REQ_FAIL;
	}
	
	if(room > room_info.num_rooms || room < 0) {
		if(es_write_int(p_fd, RSP_ROOM_JOIN) < 0
		   || es_write_char(p_fd, E_BAD_OPTIONS) < 0)
			return GGZ_REQ_DISCONNECT;
		
		return GGZ_REQ_FAIL;
	}

	/* Do the actual room change, and return results */
	if((result = room_join(player, room, p_fd)) == GGZ_REQ_DISCONNECT)
		return result;
	if(es_write_int(p_fd, RSP_ROOM_JOIN) < 0
	   || es_write_char(p_fd, result) < 0)
		return GGZ_REQ_DISCONNECT;

	if(room == 0 && show_server_info(player, p_fd) < 0)
		return GGZ_REQ_DISCONNECT;

	return GGZ_REQ_OK;
}


/* Join a player to a room, returns explanatory code on failure */
int room_join(GGZPlayer* player, const int room, const int fd)
{
	int old_room;
	int i, count;
	GGZPlayer* last;
	char name[MAX_USER_NAME_LEN+1];

	/* No other thread could possibly be changing the room # */
	/* so we can read it without a lock! */
	old_room = player->room;

	/* Check for valid inputs */
	if(old_room == room)
		return GGZ_REQ_OK;
	if(room > room_info.num_rooms || room < -2)
		return E_BAD_OPTIONS;

	/* Process queued messages unless they are connecting/disconnecting */
	if (old_room != -1 && room != -1)
		if(event_room_handle(player) < 0)
			return GGZ_REQ_DISCONNECT;

	/* We ALWAYS lock the lower ordered room first! */
	if(old_room < room) {
		if(old_room != -1)
			pthread_rwlock_wrlock(&rooms[old_room].lock);
		pthread_rwlock_wrlock(&rooms[room].lock);
	} else {
		if(room != -1)
			pthread_rwlock_wrlock(&rooms[room].lock);
		pthread_rwlock_wrlock(&rooms[old_room].lock);
	}

	/* Check for room full condition */
	if(room != -1
	   && rooms[room].player_count == rooms[room].max_players) {
		pthread_rwlock_unlock(&rooms[room].lock);
		if(old_room != -1)
			pthread_rwlock_unlock(&rooms[old_room].lock);
		return E_ROOM_FULL;
	}

	/* Yank them from this room */
	if(old_room != -1) {
		if (player->room_events)
			event_room_flush(player);
		count = -- rooms[old_room].player_count;
		last = rooms[old_room].players[count];
		for(i=0; i<=count; i++)
			if(rooms[old_room].players[i] == player) {
				rooms[old_room].players[i] = last;
				break;
			}
		dbg_msg(GGZ_DBG_ROOM,
			"Room %d player count = %d", old_room, count);
	}

	/* Put them in the new room, and free up the old room */
	player->room = room;
	if(old_room != -1)
		pthread_rwlock_unlock(&rooms[old_room].lock);

	/* Adjust the new rooms statistics */
	if(room != -1) {
		count = ++ rooms[room].player_count;
		rooms[room].players[count-1] = player;
		dbg_msg(GGZ_DBG_ROOM, "Room %d player count = %d", room, count);
	}

	/* Finally we can release the other chat room lock */
	if(room != -1)
		pthread_rwlock_unlock(&rooms[room].lock);

	pthread_rwlock_rdlock(&player->lock);
	strcpy(name, player->name);
	pthread_rwlock_unlock(&player->lock);

	room_notify_change(name, old_room, room);

	return GGZ_REQ_OK;
}


/* Notify clients that someone has entered/left the room */
static void room_notify_change(char* name, const int old, const int new)
{
	void* data = NULL;
	char* current;
	int size;

	dbg_msg(GGZ_DBG_ROOM, "%s moved from room %d to %d", name, old, new);
		
	size = sizeof(char) + strlen(name) + 1;
	
	if (old != -1) {
	/* Send DELETE update to old room */
		if ( (data = malloc(size)) == NULL)
			err_sys_exit("malloc failed in room_notify_change");

		current = (char*)data;

		*(char*)current = GGZ_UPDATE_DELETE; 
		current += sizeof(char);

		strcpy(current, name);
		current += (strlen(name) + 1);
		
		event_room_enqueue(old, (GGZEventFunc)room_event_callback, 
				   size, data);
	}

	/* Send ADD update to new room */
	if (new != -1) {
		if ( (data = malloc(size)) == NULL)
			err_sys_exit("malloc failed in room_notify_change");

		current = (char*)data;

		*(char*)current = GGZ_UPDATE_ADD;
		current += sizeof(char);

		strcpy(current, name);
		current += (strlen(name) + 1);
		
		event_room_enqueue(new, (GGZEventFunc)room_event_callback, 
				   size, data);
	}
}


/* Event callback for delivering player list update to player */
static int room_event_callback(GGZPlayer* player, int size, void* data)
{
	unsigned char opcode;
	char* name;
	int fd, room;

	room = player->room;
	fd = player->fd;

	/* Unpack event data */
	opcode = *(unsigned char*)data;
	name = (char*)(data + sizeof(char));

	/* Don't deliver updates about ourself! */
	if (strcmp(name, player->name) == 0)
		return 0;
	
	if (es_write_int(fd, MSG_UPDATE_PLAYERS) < 0
	    || es_write_char(fd, opcode) < 0
	    || es_write_string(fd, name) < 0)
		return -1;
	
	return 0;
}


/* This is more or less a temporary hack to get some server info on login */
static int show_server_info(GGZPlayer *player, const int p_fd)
{
	int players;
	int status;
	char msg[128];

	pthread_rwlock_rdlock(&state.lock);
	players = state.players;
	pthread_rwlock_unlock(&state.lock);

	if(players == 1) {
		status = chat_server_2_player(player->name,
			    "You are the only player currently logged in.");
	} else if(players == 2) {
		status = chat_server_2_player(player->name,
			    "There is one other player currently logged in.");
	} else {
		snprintf(msg, 128,
			"There are %d other players currently logged in.",
		 	players-1);
		status = chat_server_2_player(player->name, msg);
	}

	return status;
}
