/*
 * File: players.c
 * Author: Brent Hendricks
 * Project: GGZ Server
 * Date: 10/18/99
 *
 * Desc: Functions for handling players.  These functions are all
 * called by the player handler thread.  Since this thread is the only
 * one which can write to the player structure we do not need to lock
 * it when reading from it.
 *
 * Copyright (C) 1999, 2000 Brent Hendricks.
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA */


#include <config.h>

#include <sys/time.h>
#include <sys/types.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <ctype.h>

#include <easysock.h>
#include <ggzd.h>
#include <datatypes.h>
#include <players.h>
#include <table.h>
#include <protocols.h>
#include <err_func.h>
#include <seats.h>
#include <motd.h>
#include <room.h>
#include <chat.h>
#include <hash.h>
#include <transit.h>
#include <net.h>


/* Timeout for server resync */
#define GGZ_RESYNC_SEC  0
#define GGZ_RESYNC_USEC 500000


/* Server wide data structures*/
extern struct GameInfo game_types[MAX_GAME_TYPES];
extern struct GGZState state;
extern Options opt;


/* Local functions for handling players */
static void* player_new(void * sock_ptr);
static void  player_loop(GGZPlayer* player, int p_fd);
static void  player_remove(GGZPlayer* player);
static int   player_updates(GGZPlayer* player);
static int   player_msg_to_sized(GGZPlayer* player);
static int   player_transit(GGZPlayer* player, char opcode, int index);
static int   player_send_tables(int fd, int count, GGZTable* my_tables);



/* Utility functions: Should either get renamed or moved */
static int read_name(int, char[MAX_USER_NAME_LEN]);


/*
 * player_launch_handler() launches a new dedicated handler thread.
 *
 * Receives:
 * int sock : fd for communicating with the client
 */
void player_handler_launch(int sock)
{
	pthread_t thread;
	void *arg_ptr;
	int status;

	/* Temporary storage to pass fd */
	if ((arg_ptr = malloc(sizeof(int))) == NULL)
		err_sys_exit("malloc error in player_handler_launch()");
	*((int *)arg_ptr) = sock;

	status = pthread_create(&thread, NULL, player_new, arg_ptr);
	if (status != 0) {
		errno = status;
		err_sys_exit("pthread_create error");
	}
}


/*
 * player_new() is the initial function executed by a new player
 * handler thread .  It sends the initial server messages to the
 * client, and creates the new player structure.
 *
 * Receives:
 * void* arg_ptr : pointer to the client fd.
 *
 * Returns:
 * void* : NULL
 */
static void* player_new(void *arg_ptr)
{
	int sock, status;
	struct sockaddr_in addr;
	GGZPlayer *player;
	int addrlen = sizeof(addr);
	/*struct hostent *host;		Hostname lookups disabled */

	/* Get our arguments out of the arg buffer */
	sock = *((int *)arg_ptr);
	free(arg_ptr);

	/* Detach thread since no one needs to join us */
	status = pthread_detach(pthread_self());
	if (status != 0) {
		errno = status;
		err_sys_exit("pthread_detach error");
	}

	/* Get the client's IP address and store it */
	getpeername(sock, &addr, &addrlen);

	/* Send server ID */
	if (es_write_int(sock, MSG_SERVER_ID) < 0 
	    || es_va_write_string(sock, "GGZ-%s", VERSION) < 0
	    || es_write_int(sock, GGZ_CS_PROTO_VERSION) < 0
	    || es_write_int(sock, MAX_CHAT_LEN) < 0)
		pthread_exit(NULL);

	pthread_rwlock_wrlock(&state.lock);
	if (state.players == MAX_USERS) {
		pthread_rwlock_unlock(&state.lock);
		es_write_int(sock, MSG_SERVER_FULL);
		close(sock);
		pthread_exit(NULL);
	}
	state.players++;
	pthread_rwlock_unlock(&state.lock);

	/* Allocate new player structure */
	if ( (player = malloc(sizeof(GGZPlayer))) == NULL)
		err_sys_exit("malloc error in player_new()");
	
	/* Initialize player data */
	pthread_rwlock_init(&player->lock, NULL);
	player->thread = pthread_self();
	player->fd = sock;
	player->table = -1;
	player->game_fd = -1;
	player->launching = 0;
	player->transit = 0;
	player->room = -1;
	player->uid = GGZ_UID_NONE;
	strcpy(player->name, "<none>");
	/*player->addr = addr.sin_addr;*/
	inet_ntop(AF_INET, &addr.sin_addr, player->addr, sizeof(player->addr));
		  
	player->room_events = NULL;
	player->my_events_head = NULL;
	player->my_events_tail = NULL;

	dbg_msg(GGZ_DBG_CONNECTION, "New player connected from %s", 
		player->addr);
	
	player_loop(player, sock);
	player_remove(player);
	free(player);

	return (NULL);
}


/*
 * player_loop() is the main player event loop.  It handles GGZEvents,
 * data from the client, and data for the client from the game module.
 *
 * Receives:
 * GGZPlayer* player : pointer to player structure managed by this thread
 * int p_fd : player's fd
 */
static void player_loop(GGZPlayer* player, int p_fd)
{
	int status, fd_max;
	fd_set active_fd_set, read_fd_set;
	struct timeval timer;

	/* Start off listening only to player */
	FD_ZERO(&active_fd_set);
	FD_SET(p_fd, &active_fd_set);
	
	for (;;) {
		/* Process private events */
		if ( (status = player_updates(player)) < 0)
			break;
		
		switch (status) {
		case GGZ_REQ_TABLE_JOIN:
			dbg_msg(GGZ_DBG_TABLE, "%s now at a table",
				player->name);
			FD_SET(player->game_fd, &active_fd_set);
			break;
		case GGZ_REQ_TABLE_LEAVE:
			dbg_msg(GGZ_DBG_TABLE, "%s's game-over", player->name);
			
			close(player->game_fd);
			FD_CLR(player->game_fd, &active_fd_set);

			pthread_rwlock_wrlock(&player->lock);
			player->game_fd = -1;			
			pthread_rwlock_unlock(&player->lock);
			break;
		}

		read_fd_set = active_fd_set;
		fd_max = ((p_fd > player->game_fd)? p_fd : player->game_fd)+1;
		
		/* Setup timeout for select*/
		timer.tv_sec = GGZ_RESYNC_SEC;
		timer.tv_usec = GGZ_RESYNC_USEC;
		
		status = select(fd_max, &read_fd_set, NULL, NULL, &timer);
		
		if (status <= 0) {
			if (status == 0 || errno == EINTR)
				continue;
			else
				err_sys_exit("select error");
		}

		/* Check for message from player */
		if (FD_ISSET(p_fd, &read_fd_set)) {
			if ( (status = net_read_data(player, p_fd)) < 0)
				break;
		}
		
		/* Check for data from table */
		if (player->game_fd != -1 
		    && FD_ISSET(player->game_fd, &read_fd_set)) {

			if ( (status = player_msg_to_sized(player)) < 0)
				break;
			
			/* Stop listening on error */
			if (status == GGZ_REQ_FAIL)
				FD_CLR(player->game_fd, &active_fd_set);
		}
	} /* for(;;) */

	/* Clean up if there was an error during a game */
	if (player->game_fd != -1) {
		dbg_msg(GGZ_DBG_TABLE, "%s logout during game", player->name);
		player_transit(player, GGZ_TRANSIT_LEAVE, player->table);
		close(player->game_fd);
	}
}


/*
 * player_remove() does various cleanups for logout
 *
 * Receives:
 * GGZPlayer* player : pointer to player structure
 */
static void player_remove(GGZPlayer* player)
{
	int fd = player->fd;
	char lc_name[MAX_USER_NAME_LEN + 1];
	char *src, *dest;

	/* Take their name off the hash list */
	/* Convert name to lowercase for comparisons */
	pthread_rwlock_rdlock(&player->lock);
	for(src=player->name,dest=lc_name; *src!='\0'; src++,dest++)
		*dest = tolower(*src);
	*dest = '\0';
	pthread_rwlock_unlock(&player->lock);

	hash_player_delete(lc_name);

	dbg_msg(GGZ_DBG_CONNECTION, "Removing %s", player->name);
	log_msg(GGZ_LOG_CONNECTION_INFO, "%s disconnected from server",
		player->name);

	/* Remove us from room, so we get no new events */
	if (player->room != -1)
		room_join(player, -1, fd);
	
	/* FIXME: is this the right place for this? */
	event_player_flush(player);
	
	pthread_rwlock_wrlock(&player->lock);
	player->fd = -1;
	pthread_rwlock_unlock(&player->lock);	

	pthread_rwlock_wrlock(&state.lock);
	state.players--;
	pthread_rwlock_unlock(&state.lock);

	close(fd);
}


/*
 * player_updates() handles queued-up GGZEvents (both room-wide and
 * private)
 *
 * Receives:
 * GGZPlayer* player : pointer to player structure 
 *
 * Returns: 
 * int : one of
 *  GGZ_REQ_TABLE_JOIN   : player is now in a game. 
 *  GGZ_REQ_TABLE_LEAVE  : player has completed game
 *  GGZ_REQ_DISCONNECT   : player is being logged out (possbily due to error)
 *  GGZ_REQ_FAIL         : request failed
 *  GGZ_REQ_OK           : nothing special 
 */
static int player_updates(GGZPlayer* player)
{
	int status = 0;
	int old_table = player->table;
	
	/* Don't send updates to people who aren't logged in */
 	if (player->uid == GGZ_UID_NONE)
 		return GGZ_REQ_FAIL;

	/* Process events from queue */
	if (player->room != -1 && event_room_handle(player) < 0)
		return GGZ_REQ_DISCONNECT;
	
	if (player->my_events_head)
		status = event_player_handle(player);

	if (status < 0)
		return GGZ_REQ_DISCONNECT;

	if (old_table == -1 && player->table != -1)
		return GGZ_REQ_TABLE_JOIN;
	
	if (player->table == -1 && old_table != -1)
		return GGZ_REQ_TABLE_LEAVE;	

	return GGZ_REQ_OK;
}


/*
 * player_table_launch() handles REQ_TABLE_LAUNCH request from the
 * client.  It reads in the table data and seat assigments from the
 * client, does some sanity checking, and allocates a new GGZTable
 * structure.
 *
 * Receives:
 * GGZPlayer* player : pointer to player structure 
 * int p_fd : player's fd
 * int *t_fd : pointer to fd for communication with game module
 *
 * Returns: 
 * int : one of
 *  GGZ_REQ_DISCONNECT   : i/o error
 *  GGZ_REQ_FAIL         : request failed
 *  GGZ_REQ_OK           : request succeeded.  t_fd points to new fd 
 */
int player_table_launch(GGZPlayer* player, int p_fd)
{
	char desc[MAX_GAME_DESC_LEN + 1];
	char names[MAX_TABLE_SIZE][MAX_USER_NAME_LEN + 1];
	int seats[MAX_TABLE_SIZE], i, room, count, type, status;

	dbg_msg(GGZ_DBG_TABLE, "Handling table launch for %s", player->name);

	if (es_read_int(p_fd, &type) < 0
	    || es_read_string(p_fd, desc, (MAX_GAME_DESC_LEN + 1)) < 0
	    || es_read_int(p_fd, &count) < 0)
		return GGZ_REQ_DISCONNECT;

	/* Silly client. Tables are only so big*/
	if (count > MAX_TABLE_SIZE) {
		dbg_msg(GGZ_DBG_TABLE, 
			"%s tried to launch a table with > %d seats",
			player->name, MAX_TABLE_SIZE);

		if (es_write_int(p_fd, RSP_TABLE_LAUNCH) == 0)
			es_write_char(p_fd, E_BAD_OPTIONS);
		
		return GGZ_REQ_DISCONNECT;
	}
		
	/* Read in seat assignments */
	for (i = 0; i < count; i++) {
		if (es_read_int(p_fd, &seats[i]) < 0)
			return GGZ_REQ_DISCONNECT;
		if (seats[i] == GGZ_SEAT_RESV 
		    && read_name(p_fd, names[i]) < 0)
			return GGZ_REQ_DISCONNECT;
	}

	/* Blank out the other seats */
	for (i = count; i < MAX_TABLE_SIZE; i++)
		seats[i] = GGZ_SEAT_NONE;
		
	/* Now that we've cleared the channel, check if in a room */
	if ( (room = player->room) == -1) {
		dbg_msg(GGZ_DBG_TABLE, "%s tried to launch table in room -1", 
			player->name);
		if (es_write_int(p_fd, RSP_TABLE_LAUNCH) < 0
		    || es_write_char(p_fd, E_NOT_IN_ROOM) < 0)
			return GGZ_REQ_DISCONNECT;
		return GGZ_REQ_FAIL;
	}

	/* Don't allow multiple table launches */
	if (player->table != -1 || player->launching) {
		dbg_msg(GGZ_DBG_TABLE, "%s tried to launch table while at one",
			player->name);
		if (es_write_int(p_fd, RSP_TABLE_LAUNCH) < 0
		    || es_write_char(p_fd, E_LAUNCH_FAIL) < 0)
			return GGZ_REQ_DISCONNECT;
		return GGZ_REQ_FAIL;
	}

	/* Make sure type index matches */
	/* FIXME: Do we need a room lock here? */
	/* RG: Eventually we will need more room locks when we have dynamic */
	/*     rooms, right now the room's game_type can't change           */
	if (type != rooms[room].game_type) {
		dbg_msg(GGZ_DBG_TABLE, "%s tried to launch wrong table type",
			player->name);
		if (es_write_int(p_fd, RSP_TABLE_LAUNCH) < 0
		    || es_write_char(p_fd, E_NOT_IN_ROOM) < 0)
			return GGZ_REQ_DISCONNECT;
		return GGZ_REQ_FAIL;
	}

	/* Do actual launch of table */
	status = table_launch(player->name, type, room, desc, seats, names);
	
	if (status == 0) {
		/* Mark player as launching table so we can't launch more */
		pthread_rwlock_wrlock(&player->lock);
		player->launching = 1;
		pthread_rwlock_unlock(&player->lock);
	} else {
		if (es_write_int(player->fd, RSP_TABLE_LAUNCH) < 0
		    || es_write_char(player->fd, (char)status) < 0)
			return GGZ_REQ_DISCONNECT;
		status = GGZ_REQ_FAIL;
	}
	
	return status;
}


int player_launch_callback(void* target, int size, void* data)
{
	int status, index = -1;
	char *current;
	GGZPlayer* player = (GGZPlayer*)target;

	/* Unpack event data */
	current = (char*)data;

	status = *(int*)(current);
	current += sizeof(int);

	if (status == 0) {
		index = *(int*)(current);
		current += sizeof(int);
	}

	/* Launch compleyed */
	pthread_rwlock_wrlock(&player->lock);
	player->launching = 0;
	pthread_rwlock_unlock(&player->lock);

	dbg_msg(GGZ_DBG_TABLE, "%s launch result: %d", player->name, status);
	
	/* Automatically join newly created table */
	if (status == 0)
		player_transit(player, GGZ_TRANSIT_JOIN, index);

	/* Return status to client */
	if (es_write_int(player->fd, RSP_TABLE_LAUNCH) < 0
	    || es_write_char(player->fd, (char)status) < 0) {
		return GGZ_EVENT_ERROR;
	}
	
	return GGZ_EVENT_OK;
}


/* 
 * player_table_join() handles REQ_TABLE_JOIN request from the client.
 * It reads in the table number from the client, does some sanity
 * checking.
 *
 * Receives:
 * GGZPlayer* player : pointer to player structure
 * int p_fd : player's fd
 *
 * Returns:
 * int : one of
 *  GGZ_REQ_DISCONNECT   : i/o error
 *  GGZ_REQ_FAIL         : request failed
 *  GGZ_REQ_OK           : request succeeded. 
 */
int player_table_join(GGZPlayer* player, int p_fd)
{
	int index, status;

	dbg_msg(GGZ_DBG_TABLE, "Handling table join for %s", player->name);

	if (es_read_int(p_fd, &index) < 0)
		return GGZ_REQ_DISCONNECT;
	
	dbg_msg(GGZ_DBG_TABLE, "%s attempting to join table %d in room %d", 
		player->name, index, player->room);

	if (player->table != -1)
		status = E_AT_TABLE;
	else if (player->transit)
		status = E_IN_TRANSIT;
	else /* Send a join event to the table */
		status = player_transit(player, GGZ_TRANSIT_JOIN, index);

	/* Return any immediate failures to client*/
	if (status < 0) {
		if (es_write_int(player->fd, RSP_TABLE_JOIN) < 0
		    || es_write_char(player->fd, (char)status) < 0)
			return GGZ_REQ_DISCONNECT;
		status = GGZ_REQ_FAIL;
	}
	
	return status;
}


/* 
 * player_table_leave() handles REQ_TABLE_LEAVE request from the
 * client.
 *
 * Receives:
 * GGZPlayer* player : pointer to player structure
 * int p_fd : player's fd
 *
 * Returns:
 * int : one of
 *  GGZ_REQ_DISCONNECT   : i/o error
 *  GGZ_REQ_FAIL         : request failed
 *  GGZ_REQ_OK           : request succeeded.  
 */
int player_table_leave(GGZPlayer* player, int p_fd)
{
	int status;

	dbg_msg(GGZ_DBG_TABLE, "Handling table leave for %s", player->name);

	dbg_msg(GGZ_DBG_TABLE, "%s attempting to leave table %d", player->name,
		player->table);
	
	if (player->transit)
		status = E_IN_TRANSIT;
	else /* Send leave event to table */
		status = player_transit(player, GGZ_TRANSIT_LEAVE, 
					player->table);
	
	/* Return any immediate failures to client*/
	if (status < 0) {
		if (es_write_int(player->fd, RSP_TABLE_LEAVE) < 0
		    || es_write_char(player->fd, (char)status) < 0)
			return GGZ_REQ_DISCONNECT;
		status = GGZ_REQ_FAIL;
	}

	return status;
}


static int player_transit(GGZPlayer* player, char opcode, int index)
{
	int status;

	/* Do some quick sanity checking */
	if (player->room == -1) 
		return E_NOT_IN_ROOM;
	if (index == -1)
		return E_NO_TABLE;
	
	status = transit_table_event(player->room, index, opcode, 
				     player->name);
					     
	/* If enqueue fails, it's because the table has been removed */
	if (status < 0)
		return E_NO_TABLE;

	/* Mark player as "in transit" */
	pthread_rwlock_wrlock(&player->lock);
	player->transit = 1;
	pthread_rwlock_unlock(&player->lock);

	return 0;
}


int player_list_players(GGZPlayer* player, int fd)
{
	int i, count, room;
	GGZPlayer* p;
	GGZPlayer* data;


	dbg_msg(GGZ_DBG_UPDATE, "Handling player list request for %s", 
		player->name);
		
	/* Don't send list if they're not in a room */
	if (player->room == -1) {
		dbg_msg(GGZ_DBG_UPDATE, "%s requested player list in room -1",
			player->name);
		if (es_write_int(fd, RSP_LIST_PLAYERS) < 0
		    || es_write_int(fd, E_NOT_IN_ROOM) < 0)
			return GGZ_REQ_DISCONNECT;
		return GGZ_REQ_FAIL;
	}

	/* Grab room number */
	pthread_rwlock_rdlock(&player->lock);
	room = player->room;
	pthread_rwlock_unlock(&player->lock);

	pthread_rwlock_rdlock(&rooms[room].lock);
	count = rooms[room].player_count;
	if ( (data = calloc(count, sizeof(GGZPlayer))) == NULL) {
		pthread_rwlock_unlock(&rooms[room].lock);
		err_sys_exit("calloc error in player_list_players()");
	}
	for (i = 0; i < count; i++) {
		p = rooms[room].players[i];
		pthread_rwlock_rdlock(&p->lock);
		data[i] = *p;
		pthread_rwlock_unlock(&p->lock);
	}
	pthread_rwlock_unlock(&rooms[room].lock);

	if (es_write_int(fd, RSP_LIST_PLAYERS) < 0
	    || es_write_int(fd, count) < 0) {
		free(data);
		return GGZ_REQ_DISCONNECT;
	}

	for (i = 0; i < count; i++) {
		if (es_write_string(fd, data[i].name) < 0
		    || es_write_int(fd, data[i].table) < 0) {
			free(data);
			return GGZ_REQ_DISCONNECT;
		}
	}

	free(data);
	return GGZ_REQ_OK;
}


int player_list_types(GGZPlayer* player, int fd)
{
	char verbose;
	int i, max, count = 0;
	GameInfo info[MAX_GAME_TYPES];

	dbg_msg(GGZ_DBG_UPDATE, "Handling type list request for %s", 
		player->name);
		
	
	if (es_read_char(fd, &verbose) < 0)
		return GGZ_REQ_DISCONNECT;

	/* Don't send list if they're not logged in */
 	if (player->uid == GGZ_UID_NONE) {
		dbg_msg(GGZ_DBG_UPDATE, "%s requested type list before login",
			player->name);
 		if (es_write_int(fd, RSP_LIST_TYPES) < 0
 		    || es_write_int(fd, E_NOT_LOGGED_IN) < 0)
 			return GGZ_REQ_DISCONNECT;
 		return GGZ_REQ_FAIL;
 	}

	pthread_rwlock_rdlock(&state.lock);
	max = state.types;
	pthread_rwlock_unlock(&state.lock);
	
	for (i = 0; (i < MAX_GAME_TYPES && count < max); i++) {
		pthread_rwlock_rdlock(&game_types[i].lock);
		if (game_types[i].enabled)
			info[count++] = game_types[i];
		pthread_rwlock_unlock(&game_types[i].lock);
	}
		
	if (es_write_int(fd, RSP_LIST_TYPES) < 0
	    || es_write_int(fd, count) < 0)
		return GGZ_REQ_DISCONNECT;

	for (i = 0; i < count; i++) {
		if (es_write_int(fd, i) < 0
		    || es_write_string(fd, info[i].name) < 0
		    || es_write_string(fd, info[i].version) < 0
		    || es_write_string(fd, info[i].p_engine) < 0
		    || es_write_string(fd, info[i].p_version) < 0
		    || es_write_char(fd, info[i].player_allow_mask) < 0
		    || es_write_char(fd, info[i].bot_allow_mask) < 0)
			return GGZ_REQ_DISCONNECT;
		if (!verbose)
			continue;
		if (es_write_string(fd, info[i].desc) < 0
		    || es_write_string(fd, info[i].author) < 0
		    || es_write_string(fd, info[i].homepage) < 0)
			return GGZ_REQ_DISCONNECT;
	}

	return GGZ_REQ_OK;
}


int player_list_tables(GGZPlayer* player, int fd)
{
	GGZTable *my_tables;
	int count, type, status;
	char global;
	
	dbg_msg(GGZ_DBG_UPDATE, "Handling table list request for %s", 
		player->name);
	
	if (es_read_int(fd, &type) < 0
	    || es_read_char(fd, &global) < 0)
		return GGZ_REQ_DISCONNECT;
	
	/* Don`t send list if they`re not logged in */
	if (player->uid == GGZ_UID_NONE) {
		dbg_msg(GGZ_DBG_UPDATE, "%s requested table list before login",
			player->name);
		if (es_write_int(fd, RSP_LIST_TABLES) < 0
		    || es_write_int(fd, E_NOT_LOGGED_IN) < 0)
			return GGZ_REQ_DISCONNECT;
		return GGZ_REQ_FAIL;
	}
	
	/* Don't send list if they're not in a room */
	if (player->room == -1) {
		if (es_write_int(fd, RSP_LIST_TABLES) < 0
		    || es_write_int(fd, E_NOT_IN_ROOM) < 0)
			return GGZ_REQ_DISCONNECT;
		return GGZ_REQ_FAIL;
	}

	count = table_search(player->name, player->room, type, global, 
			     &my_tables);
	
	if (es_write_int(fd, RSP_LIST_TABLES) < 0
	    || es_write_int(fd, count) < 0)
		return GGZ_REQ_DISCONNECT;
	
	/* Don`t proceed if there was an error, or no tables found*/
	if (count <= 0)
		return GGZ_REQ_FAIL;
	
	status = player_send_tables(fd, count, my_tables);
	
	free(my_tables);
	
	return status;
}


static int player_send_tables(int fd, int count, GGZTable* my_tables)
{
	int i, j, seat;
	char* name = NULL;
	
	for (i = 0; i < count; i++) {
		if (es_write_int(fd, my_tables[i].index) < 0
		    || es_write_int(fd, my_tables[i].room) < 0
		    || es_write_int(fd, my_tables[i].type) < 0
		    || es_write_string(fd, my_tables[i].desc) < 0
		    || es_write_char(fd, my_tables[i].state) < 0
		    || es_write_int(fd, seats_num(&my_tables[i])) < 0)
			return GGZ_REQ_DISCONNECT;
		
		for (j = 0; j < seats_num(&my_tables[i]); j++) {
			seat = seats_type(&my_tables[i], j);
			if (es_write_int(fd, seat) < 0)
				return GGZ_REQ_DISCONNECT;
			
			switch(seat) {
			case GGZ_SEAT_OPEN:
			case GGZ_SEAT_BOT:
				continue;  /* no name for these */
			case GGZ_SEAT_RESV:
				name = my_tables[i].reserve[j];
				break;
			case GGZ_SEAT_PLAYER:
				name = my_tables[i].seats[j];
				break;
			}  
			
			if (es_write_string(fd, name) < 0)
				return GGZ_REQ_DISCONNECT;
		}
	}

	return GGZ_REQ_OK;
}


static int player_msg_to_sized(GGZPlayer* p) 
{
	char buf[4096];
	int size;
	
	if ( (size = read(p->game_fd, buf, 4096)) < 0) {
		dbg_msg(GGZ_DBG_CONNECTION, "Error reading from %s's game ", 
			p->name);
		player_transit(p, GGZ_TRANSIT_LEAVE, p->table);
		return GGZ_REQ_FAIL;
	}
				     
	if (size == 0) {
		dbg_msg(GGZ_DBG_CONNECTION, "Empty msg from %s's game ", 
			p->name);
		/*player_transit(p, GGZ_TRANSIT_LEAVE, p->table);*/
		return GGZ_REQ_FAIL;
	}
	
	if (es_write_int(p->fd, RSP_GAME) < 0
	    || es_write_int(p->fd, size) < 0
	    || es_writen(p->fd, buf, size) < 0)
		return GGZ_REQ_DISCONNECT;
	
	dbg_msg(GGZ_DBG_GAME_MSG, "Game to User: %d bytes", size);
		
	return GGZ_REQ_OK;
}


int player_msg_from_sized(GGZPlayer* p) 
{
	char buf[4096];
	int size;
	
	if (es_read_int(p->fd, &size) < 0
	    || es_readn(p->fd, buf, size) < 0) {
		dbg_msg(GGZ_DBG_CONNECTION, "Game msg read error from %s",
			p->name);
		return GGZ_REQ_DISCONNECT;
	}
	
	if (size == 0) {
		dbg_msg(GGZ_DBG_CONNECTION, "Empty game msg from %s", p->name);
		/*player_transit(p, GGZ_TRANSIT_LEAVE, p->table);*/
		return GGZ_REQ_FAIL;
	}
	
	if (es_writen(p->game_fd, buf, size) < 0) {
		dbg_msg(GGZ_DBG_CONNECTION, "Error writing to %s's game ", 
			p->name);
		player_transit(p, GGZ_TRANSIT_LEAVE, p->table);
		return GGZ_REQ_FAIL;
	}

	dbg_msg(GGZ_DBG_GAME_MSG, "User to Game: %d bytes", size);	

	return GGZ_REQ_OK;
}


int player_chat(GGZPlayer* player, int p_fd) 
{
	unsigned char subop;
	char *msg = NULL;
	char t_player[MAX_USER_NAME_LEN+1];
	int status;

	if (es_read_char(p_fd, &subop) < 0)
		return GGZ_REQ_DISCONNECT;

	dbg_msg(GGZ_DBG_CHAT, "Handling chat for %s", player->name);

	/* Get arguments if they are used for this subop */
	if (subop & GGZ_CHAT_M_PLAYER) {
		if (es_read_string(p_fd, t_player, MAX_USER_NAME_LEN+1) < 0)
			return GGZ_REQ_DISCONNECT;
	}

	if (subop & GGZ_CHAT_M_MESSAGE) {
		if ( (msg = malloc(MAX_CHAT_LEN+1)) == NULL)
			err_sys_exit("malloc error in player_chat()");
		/* FIXME: use es_r_s_a() once it supports max length*/
		if (es_read_string(p_fd, msg, MAX_CHAT_LEN+1) < 0) {
			free(msg);
			return GGZ_REQ_DISCONNECT;
		}
	}

	/* Verify that we are in a regular room */
	/* No lock needed, no one can change our room but us */
	if (player->room == -1) {
		dbg_msg(GGZ_DBG_CHAT, "%s tried to chat from room -1", 
			player->name);
		if (msg)
			free(msg);
		
		if (es_write_int(p_fd, RSP_CHAT) < 0
		    || es_write_char(p_fd, E_NOT_IN_ROOM) < 0)
			return GGZ_REQ_DISCONNECT;
		return GGZ_REQ_FAIL;
	}
	

	/* Parse subop */
	switch (subop) {
	case GGZ_CHAT_NORMAL:
		dbg_msg(GGZ_DBG_CHAT, "%s sends %s", player->name, msg);
		status = chat_room_enqueue(player->room, subop, player, msg);
		break;
	case GGZ_CHAT_BEEP:
	case GGZ_CHAT_PERSONAL:
		status = chat_player_enqueue(t_player, subop, player, msg);
		break;
	default:
		dbg_msg(GGZ_DBG_PROTOCOL, "%s sent invalid chat subop %d", 
			player->name, subop);
		status = E_BAD_OPTIONS;
	}

	/* Free message now it's been copied to chat queue */
	if (msg)
		free(msg);
	
	if (es_write_int(p_fd, RSP_CHAT) < 0
	    || es_write_char(p_fd, status) < 0)
		return GGZ_REQ_DISCONNECT;
	
	/* Don't return the chat error code */
	return 0;
}


/*
 * Utility function for reading in names
 */
static int read_name(int sock, char name[MAX_USER_NAME_LEN + 1])
{
	char *tmp;

	if (es_read_string_alloc(sock, &tmp) < 0)
		return -1;

	strncpy(name, tmp, MAX_USER_NAME_LEN);
	free(tmp);

	/* Make sure names are null-terminated */
	name[MAX_USER_NAME_LEN] = '\0';
	
	return 0;  
}


int player_motd(GGZPlayer* player, int fd)
{
	dbg_msg(GGZ_DBG_CHAT, "Handling motd request for %s", player->name);
  	if (!motd_info.use_motd)
		return GGZ_REQ_OK;

 	/* Don't send motd if they're not logged in */
 	if (player->uid == GGZ_UID_NONE) {
		dbg_msg(GGZ_DBG_CHAT, "%s requested motd before logging in",
			player->name);
 		if (es_write_int(fd, RSP_MOTD) < 0
 		    || es_write_int(fd, E_NOT_LOGGED_IN) < 0)
 			return GGZ_REQ_DISCONNECT;
 		return GGZ_REQ_FAIL;
 	}
	
	if (es_write_int(fd, RSP_MOTD) < 0
	    || motd_send_motd(fd) < 0)
		return GGZ_REQ_DISCONNECT;

	return GGZ_REQ_OK;
}


