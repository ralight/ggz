/*
 * File: players.c
 * Author: Brent Hendricks
 * Project: GGZ Server
 * Date: 10/18/99
 * Desc: Functions for handling players
 *
 * Copyright (C) 1999,2000 Brent Hendricks.
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
#include <time.h>

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


/* Timeout for server resync */
#define GGZ_RESYNC_SEC  0
#define GGZ_RESYNC_USEC 500000


/* Server wide data structures*/
extern struct GameTypes game_types;
extern struct GameTables tables;
extern struct Users players;
extern Options opt;


/* Local functions for handling players */
static void* player_new(void * sock_ptr);
static void  player_loop(int p_index, int p_fd);
static int   player_handle(int op, int p_index, int p_fd, int * t_fd);
static void  player_remove(int p_index);
static int   player_updates(int p_index, int p_fd,
			    time_t*, time_t*, time_t*, time_t*);
static int   player_msg_to_sized(int fd_in, int fd_out);
static int   player_msg_from_sized(int fd_in, int fd_out);
static int   player_chat(int p_index, int p_fd);
static int   player_login_anon(int p, int fd);
static int   player_login_new(int p_index);
static int   player_logout(int p, int fd);
static int   player_table_launch(int p_index, int p_fd, int *t_fd);
static int   player_table_join(int p_index, int p_fd, int *t_fd);
static int   player_table_leave(int p_index, int p_fd);
static int   player_list_players(int p_index, int fd);
static int   player_list_types(int p_index, int fd);
static int   player_list_tables(int p_index, int fd);
static int   player_send_error(int p_index, int fd);
static int   player_motd(int p_index, int fd);
static int player_list_tables_room(const int, const int, const int);
static int player_list_tables_global(const int, const int);

/* Utility functions: Should either get renamed or moved */
static int read_name(int, char[MAX_USER_NAME_LEN]);
static int type_match_table(int type, int num);

/*
 * launch_handler accepts the socket of a new player and launches
 * a new dedicated handler process/thread.
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
 * new_player accepts a pointer to the socket of a new player.
 * If there is an open spot take it, otherwise send a "server full"
 * message.
 */
static void* player_new(void *arg_ptr)
{
	int sock, status, i;
	char *hostname = NULL;
	struct sockaddr_in addr;
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

#if 0
	/* Lookup the hostname if enabled in ggzd.conf */
	if (opt.perform_lookups) {
		/* FIXME: Use threadsafe version */
		host = gethostbyaddr( (char*)&(addr.sin_addr),
				      sizeof(addr.sin_addr), AF_INET );
		/* FIXME:  if we're going to malloc we have to free() */
		if ((hostname = malloc(strlen(host->h_name)+1)) == NULL)
			err_sys_exit("malloc error in player_new()");
		strcpy(hostname, host->h_name);
	}
#endif

	/* Send server ID */
	if (es_write_int(sock, MSG_SERVER_ID) < 0 
	    || es_va_write_string(sock, "GGZ-%s", VERSION) < 0)
		pthread_exit(NULL);

	pthread_rwlock_wrlock(&players.lock);
	if (players.count == MAX_USERS) {
		pthread_rwlock_unlock(&players.lock);
		es_write_int(sock, MSG_SERVER_FULL);
		close(sock);
		pthread_exit(NULL);
	}

	/* Skip over occupied seats */
	for (i = 0; i < MAX_USERS; i++)
		if (players.info[i].fd < 0)
			break;
	
	/* Initialize player data */
	players.info[i].fd = sock;
	players.info[i].table_index = -1;
	players.info[i].uid = GGZ_UID_NONE;
	players.info[i].pid = pthread_self();
	strcpy(players.info[i].name, "(none)");
	inet_ntop(AF_INET, &addr.sin_addr, players.info[i].ip_addr,
		  sizeof(players.info[i].ip_addr));
	players.info[i].hostname = hostname;
	players.count++;
	players.info[i].room = -1;
	players.info[i].chat_head = NULL;
	players.info[i].personal_head = NULL;
	pthread_rwlock_unlock(&players.lock);

	dbg_msg(GGZ_DBG_CONNECTION, "New player %d connected", i);
	
	player_loop(i, sock);
	player_remove(i);

	return (NULL);
}


/*
 * io_handler accepts a player index number.  It then performs the
 * main loop for communicating with players and game tables
 * 
 */
static void player_loop(int p_index, int p_fd)
{
	int op, status, fd_max, t_fd = -1;
	char game_over = 0;
	fd_set active_fd_set, read_fd_set;
	struct timeval timer;
	/* Update timestamps */
	time_t player_ts;
	time_t table_ts;
	time_t type_ts;
	time_t room_ts;

	/* Start off listening only to player */
	p_fd = players.info[p_index].fd;
	FD_ZERO(&active_fd_set);
	FD_SET(p_fd, &active_fd_set);

	/* Initialize timestamps */
	player_ts = table_ts = type_ts = room_ts = time(NULL);

	for (;;) {
		/* Send updated info if need be */
		if (player_updates(p_index, p_fd, &player_ts, &table_ts, 
				   &type_ts, &room_ts) < 0)
			break;
		
		read_fd_set = active_fd_set;
		fd_max = ((p_fd > t_fd) ? p_fd : t_fd) + 1;
		
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
			
			if (es_read_int(p_fd, (int *)&op) < 0)
				break;
			
			status = player_handle(op, p_index, p_fd, &t_fd);
			
			if (status == GGZ_REQ_DISCONNECT) 
				break;
			
			switch (status) {
			case GGZ_REQ_TABLE_JOIN:
				/* Player launched or joined a game */
				dbg_msg(GGZ_DBG_TABLE, "Player %d now in game",
					p_index);
				FD_SET(t_fd, &active_fd_set);
				break;
			case GGZ_REQ_TABLE_LEAVE:
				dbg_msg(GGZ_DBG_TABLE,
					"Player %d game-over [user]", p_index);
				/* Mark game over if we haven't already */
				if (t_fd != -1)
					game_over = 1;
				break;
			case GGZ_REQ_FAIL:
			case GGZ_REQ_OK:
				break;
			}
		}
		
		/* Check for data from table */
		if (t_fd != -1  && FD_ISSET(t_fd, &read_fd_set)) {
			if ( (status = player_msg_to_sized(t_fd, p_fd)) <= 0) {
				dbg_msg(GGZ_DBG_TABLE,
					"Player %d game-over [game]", p_index);
				game_over = 1;
			}
			else 
				dbg_msg(GGZ_DBG_GAME_MSG,
					"Game to User: %d bytes", status);
		}

		/* Clean up after either player or table ends game,
		   but don't try to do both */
		if (game_over && t_fd != -1) {
			dbg_msg(GGZ_DBG_TABLE, "Cleaning up player %d's game",
				p_index);
			table_leave(p_index, players.info[p_index].table_index);
			close(t_fd);
			FD_CLR(t_fd, &active_fd_set);
			t_fd = -1;
			pthread_rwlock_wrlock(&players.lock);
			players.info[p_index].table_index = -1;
			players.timestamp = time(NULL);
			pthread_rwlock_unlock(&players.lock);
			/* FIXME: update time stamp in room */
			game_over = 0;
		}

	} /* for(;;) */

	/* Clean up if there was an error during a game */
	if (t_fd != -1) {
		dbg_msg(GGZ_DBG_TABLE, 
			"Logout or error forcing player %d to leave table %d",
			p_index, players.info[p_index].table_index);
		table_leave(p_index, players.info[p_index].table_index);
		close(t_fd);
	}
}



/*
 * player_handle() receives the opcode of the incoming message, the
 * player's index and player's fd, and the table fd by reference.  It
 * then dispatches the correct function to read in the data and handle
 * any requests.
 *
 * returns: 
 *  GGZ_REQ_TABLE_JOIN   : player is now in a game.  fd of the game 
 *                         is returned by reference.
 *  GGZ_REQ_TABLE_LEAVE  : player has completed game
 *  GGZ_REQ_DISCONNECT   : player is being logged out (possbily due to error)
 *  GGZ_REQ_FAIL         : request failed
 *  GGZ_REQ_OK           : nothing special 
 */
int player_handle(int request, int p_index, int p_fd, int *t_fd)
{
	int status;
	UserToControl op = (UserToControl)request;

	switch (op) {

	case REQ_LOGIN_NEW:
	case REQ_LOGIN:
	case REQ_LOGIN_ANON:
		status = player_login_anon(p_index, p_fd);
		break;

	case REQ_LOGOUT:
		player_logout(p_index, p_fd);
		status = GGZ_REQ_DISCONNECT;
		break;

	case REQ_TABLE_LAUNCH:
		status = player_table_launch(p_index, p_fd, t_fd);
		if (status == GGZ_REQ_OK)
			status = GGZ_REQ_TABLE_JOIN;
		break;

	case REQ_TABLE_JOIN:
		status = player_table_join(p_index, p_fd, t_fd);
		if (status == GGZ_REQ_OK)
			status = GGZ_REQ_TABLE_JOIN;
		break;

	case REQ_TABLE_LEAVE:
		status = player_table_leave(p_index, p_fd);
		if (status == GGZ_REQ_OK)
			status = GGZ_REQ_TABLE_LEAVE;
		break;

	case REQ_LIST_PLAYERS:
		status = player_list_players(p_index, p_fd);
		break;

	case REQ_LIST_TYPES:
		status = player_list_types(p_index, p_fd);
		break;

	case REQ_LIST_TABLES:
		status = player_list_tables(p_index, p_fd);
		break;

	case REQ_LIST_ROOMS:
	case REQ_ROOM_JOIN:
		status = room_handle_request(request, p_index, p_fd);
		break;

	case REQ_GAME:
		status = player_msg_from_sized(p_fd, *t_fd); 
		if (status <= 0)
			status = GGZ_REQ_TABLE_LEAVE;
		else 
			dbg_msg(GGZ_DBG_GAME_MSG, "User to Game: %d bytes", 
				status);
		break;
			
	case REQ_CHAT:
		status = player_chat(p_index, p_fd);
		break;

	case REQ_MOTD:
		status = player_motd(p_index, p_fd);
		break;
	  
	case REQ_PREF_CHANGE:
	case REQ_REMOVE_USER:
	case REQ_TABLE_OPTIONS:
	case REQ_USER_STAT:
	default:
		dbg_msg(GGZ_DBG_PROTOCOL,
			"Player %d (uid: %d) requested unimplemented op %d",
			p_index, players.info[p_index].uid, op);
		status = player_send_error(p_index, p_fd);
	}

	return status;
}


/*
 * player_remove accepts a player index number and then proceeds to
 * remove that player from the list
 */
static void player_remove(int p_index)
{
	int fd;

	pthread_rwlock_wrlock(&players.lock);
	dbg_msg(GGZ_DBG_CONNECTION, "Removing player %d (uid: %d)", p_index, 
		players.info[p_index].uid);
	log_msg(GGZ_LOG_CONNECTION_INFO, "%s disconnected from server",
		players.info[p_index].name);
	fd = players.info[p_index].fd;
	players.info[p_index].fd = -1;
	players.count--;
	players.timestamp = time(NULL);

	pthread_rwlock_unlock(&players.lock);
	if (players.info[p_index].room != -1) {
		room_join(p_index, -1, fd);
		/*room_dequeue_personal(p_index);*/
	}

	close(fd);
}


/*
 * player_updates checks any updates (player/table/type) lists
 * or chats which need to be sent to the player, and sends them
 */
static int player_updates(int p, int fd, time_t* player_ts, time_t* table_ts, 
			  time_t* type_ts, time_t* room_ts) {
	int room;
	char user_update = 0;
	char table_update = 0;
	char type_update = 0;
	char room_update = 0;

	/* Don't send updates to people who aren't logged in */
 	if (players.info[p].uid == GGZ_UID_NONE)
 		return GGZ_REQ_FAIL;

	if ( (room = players.info[p].room) != -1) {
		/* Check for player list updates in our room */
		pthread_rwlock_rdlock(&chat_room[room].lock);
		if (difftime(chat_room[room].player_timestamp, *player_ts) !=0){
			*player_ts = chat_room[room].player_timestamp;
			user_update = 1;
			dbg_msg(GGZ_DBG_UPDATE,
				"Player %d needs player update", p);
		}

		/* Check for table list updates in our room */
		if (difftime(chat_room[room].table_timestamp, *table_ts) != 0) {
			*table_ts = chat_room[room].table_timestamp;
			table_update = 1;
			dbg_msg(GGZ_DBG_UPDATE,
				"Player %d needs table update", p);
		}
		pthread_rwlock_unlock(&chat_room[room].lock);
	}

	/* Check for room list update */
	/* pthread_rwlock_rdlock(&room_info.lock); */
	if(difftime(room_info.timestamp, *room_ts) != 0) {
		*room_ts = room_info.timestamp;
		room_update = 1;
		dbg_msg(GGZ_DBG_UPDATE, "Player %d needs room update", p);
	}
	/* pthread_rwlock_unlock(&room_info.lock); */

	/* Check for game type list updates*/
	pthread_rwlock_rdlock(&game_types.lock);
	if (difftime(game_types.timestamp, *type_ts) != 0 ) {
		*type_ts = game_types.timestamp;
		type_update = 1;
		dbg_msg(GGZ_DBG_UPDATE, "Player %d needs type update", p);
	}
	pthread_rwlock_unlock(&game_types.lock);

	/* Send out proper update messages */
	if ((user_update && es_write_int(fd, MSG_UPDATE_PLAYERS) < 0)
	    || (type_update && es_write_int(fd, MSG_UPDATE_TYPES) < 0)
	    || (table_update && es_write_int(fd, MSG_UPDATE_TABLES) < 0)
	    || (room_update && es_write_int(fd, MSG_UPDATE_ROOMS) < 0))
		return GGZ_REQ_DISCONNECT;
	
	/* Send any unread chats */
	if (players.info[p].room != -1
	    && (room_send_chat(p, fd) < 0))
		return GGZ_REQ_DISCONNECT;
	
	return GGZ_REQ_OK;
}


/*
 * player_login_new implements the following exchange:
 * 
 * REQ_NEW_LOGIN
 *  str: login name
 * RSP_NEW_LOGIN
 *  chr: success flag (0 for success, -1 invalid name, -2 duplicate name)
 *  str: initial password (if success)
 *  int: game type checksum (if success)
 */
static int player_login_new(int p)
{
	/*FIXME: needs rewitten in style of player_table_launch */
	char name[MAX_USER_NAME_LEN + 1];

	dbg_msg(GGZ_DBG_CONNECTION, "Creating new login for player %d", p);

	if (read_name(players.info[p].fd, name) < 0)
		return GGZ_REQ_DISCONNECT;

	/* FIXME: assign unique uid and create new entry in database */
	pthread_rwlock_wrlock(&players.lock);
	players.info[p].uid = p;
	strncpy(players.info[p].name, name, MAX_USER_NAME_LEN + 1);
	pthread_rwlock_unlock(&players.lock);

	/* FIXME: good password generation */
	/* FIXME: calculate game type checksum (should be done earlier?) */

	if (es_write_int(players.info[p].fd, RSP_LOGIN_NEW) < 0 
	    || es_write_char(players.info[p].fd, 0) < 0 
	    || es_write_string(players.info[p].fd, "password") < 0 
	    || es_write_int(players.info[p].fd, 265) < 0)
		return GGZ_REQ_DISCONNECT;

	/* Send off the Message Of The Day */
	if (motd_info.use_motd
	    && (es_write_int(players.info[p].fd, MSG_MOTD) < 0
		|| motd_send_motd(players.info[p].fd) < 0)) {
		player_remove(p);
		return GGZ_REQ_DISCONNECT;
	}

	dbg_msg(GGZ_DBG_CONNECTION, "Successful new login of %s: UID = %d",
		players.info[p].name, players.info[p].uid);

	return GGZ_REQ_OK;
}


/*
 * player_login_anon implements the following exchange:
 * 
 * REQ_LOGIN_ANON
 *  str: login name
 * RSP_LOGIN_ANON
 *  chr: success flag (0 for success, -1 invalid name, -2 duplicate name)
 *  int: game type checksum (if success)
 */
static int player_login_anon(int p, int fd)
{
	char name[MAX_USER_NAME_LEN + 1];
	char *ip_addr, *hostname;
	int i;

	/* Read this first to get it out of the socket */
	if (read_name(fd, name) < 0)
		return GGZ_REQ_DISCONNECT;

	/* Can't login twice */
	if (players.info[p].uid != GGZ_UID_NONE) {
		dbg_msg(GGZ_DBG_CONNECTION, 
			"Player %d attempted to log in again", p);
		if (es_write_int(fd, RSP_LOGIN_ANON) < 0
		    || es_write_char(fd, E_ALREADY_LOGGED_IN) < 0)
			return GGZ_REQ_DISCONNECT;
		return GGZ_REQ_FAIL;
	}
	
	dbg_msg(GGZ_DBG_CONNECTION, "Creating guest login for player %d", p);
		
	/* Check name for uniqueness */
	pthread_rwlock_rdlock(&players.lock);
	for (i = 0; i < MAX_USERS; i++)
		if (players.info[i].fd != -1 
		    && !(strcmp(name, players.info[i].name)))
			break;
	pthread_rwlock_unlock(&players.lock);

	/* FIXME: need to check vs. database too */
	if (i != MAX_USERS) {
		dbg_msg(GGZ_DBG_CONNECTION,
			"Unsuccessful anonymous login of %s", name);
		if (es_write_int(fd, RSP_LOGIN_ANON) < 0
		    || es_write_char(fd, -1) < 0)
			return GGZ_REQ_DISCONNECT;
		return GGZ_REQ_FAIL;
	}

	pthread_rwlock_wrlock(&players.lock);
	players.info[p].uid = GGZ_UID_ANON;
	strncpy(players.info[p].name, name, MAX_USER_NAME_LEN + 1);
	players.timestamp = time(NULL);
	ip_addr = players.info[p].ip_addr;
	hostname = players.info[p].hostname;
	pthread_rwlock_unlock(&players.lock);

	if (es_write_int(fd, RSP_LOGIN_ANON) < 0
	    || es_write_char(fd, 0) < 0
	    || es_write_int(fd, 265) < 0)
		return GGZ_REQ_DISCONNECT;

	/* Send off the Message Of The Day */
	if (motd_info.use_motd
	    && (es_write_int(fd, MSG_MOTD) < 0
		|| motd_send_motd(fd) < 0)) {
		player_remove(p);
		return GGZ_REQ_DISCONNECT;
	}
	
	dbg_msg(GGZ_DBG_CONNECTION, "Successful anonymous login of %s", name);

	if(hostname)
		log_msg(GGZ_LOG_CONNECTION_INFO,
			"Anonymous player %s logged in from %s (%s)",
			name, hostname, ip_addr);
	else
		log_msg(GGZ_LOG_CONNECTION_INFO,
			"Anonymous player %s logged in from %s", name, ip_addr);

	return 0;
}


/*
 * player_logout implements the following exchange:
 * 
 * REQ_LOGOUT
 * RSP_LOGOUT
 *  chr: success flag (0 for success, -1 for error )
 */
static int player_logout(int p, int fd)
{
	dbg_msg(GGZ_DBG_CONNECTION, "Handling logout for player %d", p);
	
	/* FIXME: Saving of stats and other things */
	if (es_write_int(fd, RSP_LOGOUT) < 0 
	    || es_write_char(fd, 0) < 0)
		return GGZ_REQ_DISCONNECT;
	
	return GGZ_REQ_OK;
}


/*
 * player_table_launch implements the following exchange:
 *
 * REQ_TABLE_LAUNCH
 *  int: game type index
 *  str: table description
 *  int: number of seats
 *  sequence of
 *    int: seat assignment
 *    str: login name for reservation
 *  int: size of options struct in bytes
 *  (option data)
 * RSP_TABLE_LAUNCH
 *  chr: success flag (0 for success, negative values for various failures)
 * 
 * returns 0 if successful, -1 on error.  If successful, returns table
 * fd by reference
 */
static int player_table_launch(int p_index, int p_fd, int *t_fd)
{
	TableInfo table;
	int i, t_index = -1;
	unsigned int size;
	int status = 0;
	int seats;
	int room;
	void *options = NULL;
	char name[MAX_USER_NAME_LEN + 1];
	int count;

	dbg_msg(GGZ_DBG_TABLE, "Handling table launch for player %d", p_index);

	if (es_read_int(p_fd, &table.type_index) < 0
	    || es_read_string(p_fd, table.desc, (MAX_GAME_DESC_LEN + 1)) < 0
	    || es_read_int(p_fd, &seats) < 0)
		return GGZ_REQ_DISCONNECT;

	/* Silly client. Tables are only so big*/
	if (seats > MAX_TABLE_SIZE) {
		dbg_msg(GGZ_DBG_TABLE, 
			"Player %d tried to launch a table with > %d seats",
			p_index, MAX_TABLE_SIZE);
		if (es_write_int(p_fd, RSP_TABLE_LAUNCH) < 0
		    || es_write_char(p_fd, E_BAD_OPTIONS) < 0)
			return GGZ_REQ_DISCONNECT;
		return GGZ_REQ_DISCONNECT;
	}
		
	/* Read in seat assignments */
	for (i = 0; i < seats; i++) {
		if (es_read_int(p_fd, &table.seats[i]) < 0)
			return GGZ_REQ_DISCONNECT;
		if (table.seats[i] != GGZ_SEAT_RESV)
			continue;
		if (read_name(p_fd, name) < 0)
			return GGZ_REQ_DISCONNECT;
		/*
		 * FIXME: lookup uid of name and asign to table.reserve[i]
		 * upon error, set status = E_USR_LOOKUP
		 */
	}

	/* Read in game specific options */
	if (es_read_int(p_fd, &size) < 0)
		return GGZ_REQ_DISCONNECT;
	if (size > 0 && (options = malloc(size)) == NULL)
		err_sys_exit("malloc error");
	if (size > 0 && es_readn(p_fd, options, size) < size) {
		free(options);
		return GGZ_REQ_DISCONNECT;
	}

	/* Now that we've cleared the socket, check if in a room */
	if ( (room = players.info[p_index].room) == -1) {
		dbg_msg(GGZ_DBG_TABLE, 
			"Player %d tried to launch a table from room -1",
			p_index);
		if (options)
			free(options);
		if (es_write_int(p_fd, RSP_TABLE_LAUNCH) < 0
		    || es_write_char(p_fd, E_NOT_IN_ROOM) < 0)
			return GGZ_REQ_DISCONNECT;
		return GGZ_REQ_FAIL;
	}

	/* Don't allow multiple table launches */
	if (players.info[p_index].table_index != -1) {
		dbg_msg(GGZ_DBG_TABLE, 
			"Player %d tried to launch a table while at one",
			p_index);
		if (options)
			free(options);
		if (es_write_int(p_fd, RSP_TABLE_LAUNCH) < 0
		    || es_write_char(p_fd, E_LAUNCH_FAIL) < 0)
			return GGZ_REQ_DISCONNECT;
		return GGZ_REQ_FAIL;
	}

	/* Make sure type index matches */
	/* FIXME: Do we need a room lock here? */
	/* RG: Eventually we will need more room locks when we have dynamic */
	/*     rooms, right now the room's game_type can't change           */
	if (table.type_index != chat_room[room].game_type) {
		dbg_msg(GGZ_DBG_TABLE, 
			"Player %d tried to launch a table from wrong room",
			p_index);
		if (options)
			free(options);
		if (es_write_int(p_fd, RSP_TABLE_LAUNCH) < 0
		    || es_write_char(p_fd, E_NOT_IN_ROOM) < 0)
			return GGZ_REQ_DISCONNECT;
		return GGZ_REQ_FAIL;
	}

	/* Fill remaining parameters */
	table.room = players.info[p_index].room;
	table.state = GGZ_TABLE_CREATED;
	table.transit_flag = GGZ_TRANSIT_CLR;
	table.pid = -1;
	table.fd_to_game = -1;
	for (i = seats; i < MAX_TABLE_SIZE; i++)
		table.seats[i] = GGZ_SEAT_NONE;
	table.options = options;
	pthread_mutex_init(&table.transit_lock, NULL);
	pthread_mutex_init(&table.state_lock, NULL);
	pthread_cond_init(&table.transit_cond, NULL);
	pthread_cond_init(&table.state_cond, NULL);

	/* Do actual launch of table */
	status = table_launch(p_index, table, &t_index);
	
	if (status != 0) 
		dbg_msg(GGZ_DBG_TABLE, 
			"Player %d's table launch failed with err %d", p_index,
			status);
	else {
		dbg_msg(GGZ_DBG_TABLE, "Player %d's table launch successful", 
			p_index);
		/* Setup an entry in the chat_room */
		pthread_rwlock_wrlock(&chat_room[table.room].lock);
		count = ++ chat_room[table.room].table_count;
		chat_room[table.room].table_index[count-1] = t_index;
		dbg_msg(GGZ_DBG_ROOM,
			"Room %d table count = %d", table.room, count);
		chat_room[table.room].table_timestamp = time(NULL);
		chat_room[table.room].player_timestamp = time(NULL);
		pthread_rwlock_unlock(&chat_room[table.room].lock);
	}

	/* Join newly created table */
	status = table_join(p_index, t_index, t_fd);

	if (status != 0) 
		dbg_msg(GGZ_DBG_TABLE, 
			"Player %d's table join failed with err %d", p_index, 
			status);
	else
		dbg_msg(GGZ_DBG_TABLE, "Player %d's table join successful", 
			p_index);


	/* Return status to client */
	if (es_write_int(p_fd, RSP_TABLE_LAUNCH) < 0
	    || es_write_char(p_fd, (char)status) < 0) {
		if (status == 0)
			table_leave(p_index, t_index);
		return GGZ_REQ_DISCONNECT;
	}

	if (status != 0)
		return GGZ_REQ_FAIL;
	
	pthread_rwlock_wrlock(&players.lock);
	players.info[p_index].table_index = t_index;
	players.timestamp = time(NULL);
	pthread_rwlock_unlock(&players.lock);

	return GGZ_REQ_OK;
}


/*
 * player_table_join implements the following exchange:
 *
 * REQ_TABLE_JOIN
 *  int: game table index
 * RSP_TABLE_JOIN
 *  chr: success flag (0 for success, negative values for various failures)
 * 
 * returns 0 if successful, -1 on error.  If successful, returns table
 * fd by reference
 */
static int player_table_join(int p_index, int p_fd, int *t_fd)
{
	int t_index;
	int status = 0;
	int room;

	dbg_msg(GGZ_DBG_TABLE, "Handling table join for player %d", p_index);
	if (es_read_int(p_fd, &t_index) < 0)
		return GGZ_REQ_DISCONNECT;
	
	dbg_msg(GGZ_DBG_TABLE,
		"Player %d attempting to join table %d", p_index, t_index);

	/* Now that we've cleared the socket, check if in a room */
	if (players.info[p_index].room == -1) {
		dbg_msg(GGZ_DBG_TABLE, 
			"Player %d tried to join a table from room -1",
			p_index);
		if (es_write_int(p_fd, RSP_TABLE_JOIN) < 0
		    || es_write_char(p_fd, E_NOT_IN_ROOM) < 0)
			return GGZ_REQ_DISCONNECT;
		return GGZ_REQ_FAIL;
	}
	
	status = table_join(p_index, t_index, t_fd);

	if (status != 0) 
		dbg_msg(GGZ_DBG_TABLE, 
			"Player %d's table join failed with err %d", p_index, 
			status);
	else
		dbg_msg(GGZ_DBG_TABLE, "Player %d's table join successful", 
			p_index);

	/* Return status to client*/
	if (es_write_int(p_fd, RSP_TABLE_JOIN) < 0
	    || es_write_char(p_fd, (char)status) < 0) {
		if (status == 0)
			table_leave(p_index, t_index);
		return GGZ_REQ_DISCONNECT;
	}
	
	if (status != 0)
		return GGZ_REQ_FAIL;
	
	pthread_rwlock_wrlock(&players.lock);
	players.info[p_index].table_index = t_index;
	players.timestamp = time(NULL);
	pthread_rwlock_unlock(&players.lock);

	/* Update notifications */
	pthread_rwlock_rdlock(&tables.lock);
	room = tables.info[t_index].room;
	pthread_rwlock_unlock(&tables.lock);
	pthread_rwlock_wrlock(&chat_room[room].lock);
	chat_room[room].table_timestamp = time(NULL);
	chat_room[room].player_timestamp = time(NULL);
	pthread_rwlock_unlock(&chat_room[room].lock);

	return GGZ_REQ_OK;
}


static int player_table_leave(int p_index, int p_fd)
{
	int t_index;
	int status = 0;
	int room;

	dbg_msg(GGZ_DBG_TABLE, "Handling table leave for player %d", p_index);

	pthread_rwlock_rdlock(&players.lock);
	t_index = players.info[p_index].table_index;
	pthread_rwlock_unlock(&players.lock);

	/* Now that we've cleared the socket, check if at table */
	if (t_index < 0) {
		dbg_msg(GGZ_DBG_TABLE, 
			"Player %d tried to leave a table, but not at one",
			p_index);
		if (es_write_int(p_fd, RSP_TABLE_LEAVE) < 0
		    || es_write_char(p_fd, E_NO_TABLE) < 0)
			return GGZ_REQ_DISCONNECT;
		return GGZ_REQ_FAIL;
	}

	dbg_msg(GGZ_DBG_TABLE,
		"Player %d attempting to leave table %d", p_index, t_index);

	status = table_leave(p_index, t_index);

	if (status != 0) 
		dbg_msg(GGZ_DBG_TABLE, 
			"Player %d's table leave failed with err %d", p_index, 
			status);
	else
		dbg_msg(GGZ_DBG_TABLE, "Player %d's table leave successful", 
			p_index);

	/* Return status to client*/
	if (es_write_int(p_fd, RSP_TABLE_LEAVE) < 0
	    || es_write_char(p_fd, (char)status) < 0)
		return GGZ_REQ_DISCONNECT;
	
	if (status != 0)
		return GGZ_REQ_FAIL;
	
	/* Update notifications */
	pthread_rwlock_rdlock(&tables.lock);
	room = tables.info[t_index].room;
	pthread_rwlock_unlock(&tables.lock);
	pthread_rwlock_wrlock(&chat_room[room].lock);
	chat_room[room].table_timestamp = time(NULL);
	chat_room[room].player_timestamp = time(NULL);
	pthread_rwlock_unlock(&chat_room[room].lock);

	pthread_rwlock_wrlock(&players.lock);
	players.info[p_index].table_index = -1;
	players.timestamp = time(NULL);
	pthread_rwlock_unlock(&players.lock);

	return GGZ_REQ_OK;
}


static int player_list_players(int p_index, int fd)
{
	int i, count, room;
	UserInfo info[MAX_USERS];
	int *p_list;

	dbg_msg(GGZ_DBG_UPDATE,
		"Handling player list request for player %d", p_index);

 	/* Don't send list if they're not in a room */
 	if (players.info[p_index].room == -1) {
		dbg_msg(GGZ_DBG_UPDATE, 
			"Player %d requested player list from room -1",
			p_index);
 		if (es_write_int(fd, RSP_LIST_PLAYERS) < 0
 		    || es_write_int(fd, E_NOT_IN_ROOM) < 0)
 			return GGZ_REQ_DISCONNECT;
 		return GGZ_REQ_FAIL;
 	}

	pthread_rwlock_rdlock(&players.lock);
	memcpy(info, players.info, sizeof(info));
	room = players.info[p_index].room;
	pthread_rwlock_unlock(&players.lock);
	
	pthread_rwlock_rdlock(&chat_room[room].lock);
	count = chat_room[room].player_count;
	if ( (p_list = calloc(count, sizeof(int))) == NULL) {
		pthread_rwlock_unlock(&chat_room[room].lock);
		err_sys_exit("calloc error in player_list_players()");
	}
	memcpy(p_list, chat_room[room].player_index, count*sizeof(int));
	pthread_rwlock_unlock(&chat_room[room].lock);

	if (es_write_int(fd, RSP_LIST_PLAYERS) < 0
	    || es_write_int(fd, count) < 0) {
		free(p_list);
		return GGZ_REQ_DISCONNECT;
	}

	for (i = 0; i < count; i++) {
		if (es_write_string(fd, info[p_list[i]].name) < 0
		    || es_write_int(fd, info[p_list[i]].table_index) < 0) {
			free(p_list);
			return GGZ_REQ_DISCONNECT;
		}
	}

	free(p_list);
	return GGZ_REQ_OK;
}


static int player_list_types(int p_index, int fd)
{
	char verbose;
	int i, count = 0;
	GameInfo info[MAX_GAME_TYPES];

	dbg_msg(GGZ_DBG_UPDATE,
		"Handling type list request for player %d", p_index);
	
	if (es_read_char(fd, &verbose) < 0)
		return GGZ_REQ_DISCONNECT;

	/* Don't send list if they're not logged in */
 	if (players.info[p_index].uid == GGZ_UID_NONE) {
		dbg_msg(GGZ_DBG_UPDATE, 
			"Player %d requested type list before logging in",
			p_index);
 		if (es_write_int(fd, RSP_LIST_TYPES) < 0
 		    || es_write_int(fd, E_NOT_LOGGED_IN) < 0)
 			return GGZ_REQ_DISCONNECT;
 		return GGZ_REQ_FAIL;
 	}

	pthread_rwlock_rdlock(&game_types.lock);
	for (i = 0; (i < MAX_GAME_TYPES && count < game_types.count); i++)
		if (game_types.info[i].enabled)
			info[count++] = game_types.info[i];
	pthread_rwlock_unlock(&game_types.lock);

	if (es_write_int(fd, RSP_LIST_TYPES) < 0
	    || es_write_int(fd, count) < 0)
		return GGZ_REQ_DISCONNECT;

	for (i = 0; i < count; i++) {
		if (es_write_int(fd, i) < 0
		    || es_write_string(fd, info[i].name) < 0
		    || es_write_string(fd, info[i].version) < 0
		    || es_write_char(fd, info[i].player_allow_mask) < 0)
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


static int player_list_tables(int p_index, int fd)
{
	int status, type, room;
	char global;

	dbg_msg(GGZ_DBG_UPDATE,
		"Handling table list request for player %d", p_index);	

	if (es_read_int(fd, &type) < 0
	    || es_read_char(fd, &global) < 0)
		return GGZ_REQ_DISCONNECT;

 	/* Don't send list if they're not logged in */
 	if (players.info[p_index].uid == GGZ_UID_NONE) {
		dbg_msg(GGZ_DBG_UPDATE, 
			"Player %d requested table list before logging in",
			p_index);
 		if (es_write_int(fd, RSP_LIST_TABLES) < 0
 		    || es_write_int(fd, E_NOT_LOGGED_IN) < 0)
 			return GGZ_REQ_DISCONNECT;
 		return GGZ_REQ_FAIL;
 	}

	if(global)
		status = player_list_tables_global(fd, type);
	else {
		room = players.info[p_index].room;
		status = player_list_tables_room(fd, room, type);
	}

	return status;
}


static int player_list_tables_room(const int fd, const int room, const int type)
{
	int *t_list, *indices;
	TableInfo *my_tables;
	int count=0, t_count, i, j, status, p_index, uid;
	char *name;

	/* Don't send list if they're not in a room */
	if (room == -1) {
		if (es_write_int(fd, RSP_LIST_TABLES) < 0
		    || es_write_int(fd, E_NOT_IN_ROOM) < 0)
			return GGZ_REQ_DISCONNECT;
		return GGZ_REQ_FAIL;
	}

	/* Copy a list of tables we are interested in */
	pthread_rwlock_rdlock(&chat_room[room].lock);
	t_count = chat_room[room].table_count;

	/* Take care of degenerate case, no tables */
	if(t_count == 0) {
		pthread_rwlock_unlock(&chat_room[room].lock);
		if(es_write_int(fd, RSP_LIST_TABLES) < 0
		   || es_write_int(fd, 0) < 0)
			return GGZ_REQ_DISCONNECT;
		return 0;
	}

	/* Continue copying tables */
	if ( (t_list = calloc(t_count, sizeof(int))) == NULL) {
		pthread_rwlock_unlock(&chat_room[room].lock);
		err_sys_exit("calloc error in player_list_tables()");
	}
	memcpy(t_list, chat_room[room].table_index, t_count*sizeof(int));
	pthread_rwlock_unlock(&chat_room[room].lock);

	if((my_tables = calloc(t_count, sizeof(TableInfo))) == NULL) {
		free(t_list);
		err_sys_exit("calloc error in player_list_tables()");
	}

	/* Copy the tables we want */
	pthread_rwlock_rdlock(&tables.lock);
	if (type == -1) {
		for(i=0; i<t_count; i++)
			my_tables[i] = tables.info[t_list[i]];
		count = t_count;
		indices = t_list;
	} else {
		if((indices = calloc(t_count, sizeof(int))) == NULL) {
			pthread_rwlock_unlock(&tables.lock);
			free(t_list);
			free(my_tables);
			err_sys_exit("calloc error in player_list_tables()");
		}
		for(i=0; i<t_count; i++)
	    		if(type_match_table(type, t_list[i])) {
				my_tables[t_count] = tables.info[t_list[i]];
				indices[count++] = i;
			}
	}
	pthread_rwlock_unlock(&tables.lock);

	if (es_write_int(fd, RSP_LIST_TABLES) < 0
	    || es_write_int(fd, count) < 0) {
		return GGZ_REQ_DISCONNECT;
	}

	status = 0;
	for (i = 0; i < count; i++) {
		if (es_write_int(fd, my_tables[i].room) < 0
		    || es_write_int(fd, indices[i]) < 0
		    || es_write_int(fd, my_tables[i].type_index) < 0
		    || es_write_string(fd, my_tables[i].desc) < 0
		    || es_write_char(fd, my_tables[i].state) < 0
		    || es_write_int(fd, seats_num(my_tables[i])) < 0) {
			status = GGZ_REQ_DISCONNECT;
			goto pltr_common_exit;
		}

		for (j = 0; j < seats_num(my_tables[i]); j++) {
			if (es_write_int(fd, my_tables[i].seats[j]) < 0) {
				status = GGZ_REQ_DISCONNECT;
				goto pltr_common_exit;
			}

			switch(my_tables[i].seats[j]) {
				case GGZ_SEAT_OPEN:
				case GGZ_SEAT_BOT:
					continue;  /* no name for these */
				case GGZ_SEAT_RESV:
					uid = my_tables[i].reserve[j];
					/* Look up player name by uid */
					name = strdup("reserved");
					break;
				default: /* must be a player index */
					p_index = my_tables[i].seats[j];
					/* FIXME: Race condition */
					pthread_rwlock_rdlock(&players.lock);
					name=strdup(players.info[p_index].name);
					pthread_rwlock_unlock(&players.lock);
			}

			if (es_write_string(fd, name) < 0) {
				free(name);
				status = GGZ_REQ_DISCONNECT;
				goto pltr_common_exit;
			}

			free(name);
		}
	}

pltr_common_exit:
	free(t_list);
	free(my_tables);
	if(indices != t_list)
		free(indices);
	return status;
}


static int player_list_tables_global(const int fd, const int type)
{
	int i, j, index, uid, count = 0;
	char name[MAX_USER_NAME_LEN + 1];
	TableInfo my_tables[MAX_TABLES];
	int indices[MAX_TABLES];

	/* Copy all tables of interest to local list */
	pthread_rwlock_rdlock(&tables.lock);
	for (i = 0; (i < MAX_TABLES && count < tables.count); i++) {
		if (tables.info[i].type_index != -1 
	    	&& type_match_table(type, i) ) {
			my_tables[count] = tables.info[i];
			indices[count++] = i;
		}
	}
	pthread_rwlock_unlock(&tables.lock);

	if (es_write_int(fd, RSP_LIST_TABLES) < 0
	    || es_write_int(fd, count) < 0)
		return GGZ_REQ_DISCONNECT;

	for (i = 0; i < count; i++) {
		if (es_write_int(fd, my_tables[i].room) < 0
		    || es_write_int(fd, indices[i]) < 0
		    || es_write_int(fd, my_tables[i].type_index) < 0
		    || es_write_string(fd, my_tables[i].desc) < 0
		    || es_write_char(fd, my_tables[i].state) < 0
		    || es_write_int(fd, seats_num(my_tables[i])) < 0)
			return GGZ_REQ_DISCONNECT;

		for (j = 0; j < seats_num(my_tables[i]); j++) {
			if (es_write_int(fd, my_tables[i].seats[j]) < 0)
				return GGZ_REQ_DISCONNECT;

			switch(my_tables[i].seats[j]) {
				case GGZ_SEAT_OPEN:
				case GGZ_SEAT_BOT:
					continue;  /* no name for these */
				case GGZ_SEAT_RESV:
					uid = my_tables[i].reserve[j];
					/* Look up player name by uid */
					strcpy(name,"reserved");
					break;
				default: /* must be a player index */
					index = my_tables[i].seats[j];
					/* FIXME: Race condition */
					pthread_rwlock_rdlock(&players.lock);
					strcpy(name, players.info[index].name);
					pthread_rwlock_unlock(&players.lock);
			}

			if (es_write_string(fd, name) < 0)
				return GGZ_REQ_DISCONNECT;
		}
	}

	return 0;
}


static int player_msg_to_sized(int in, int out) 
{
	char buf[4096];
	int size;
	
	if ( (size = read(in, buf, 4096)) <= 0)
		return(-1);
	
	if (es_write_int(out, RSP_GAME) < 0
	    || es_write_int(out, size) < 0
	    || es_writen(out, buf, size) < 0)
		return(-1);

	return size;
}


static int player_msg_from_sized(int in, int out) 
{
	char buf[4096];
	int size;
	
	if (es_read_int(in, &size) < 0
	    || es_readn(in, buf, size) < 0)
		return (-1);
	
	return (es_writen(out, buf, size));
}


static int player_chat(int p_index, int p_fd) 
{
	char* msg;
	int status;

	dbg_msg(GGZ_DBG_CHAT, "Handling chat for player %d", p_index);

	if((msg = malloc(MAX_CHAT_LEN)+1) == NULL)
		err_sys_exit("malloc error in player_chat()");
	if (es_read_string(p_fd, msg, MAX_CHAT_LEN+1) < 0) {
		free(msg);
		return GGZ_REQ_DISCONNECT;
	}

	/* No lock needed, no one can change our room but us */
	if (players.info[p_index].room == -1) {
		dbg_msg(GGZ_DBG_CHAT, 
			"Player %d tried to chat from room -1",
			p_index);
		free(msg);
		if (es_write_int(p_fd, RSP_CHAT) < 0
		    || es_write_char(p_fd, E_NOT_IN_ROOM) < 0)
			return GGZ_REQ_DISCONNECT;
		return GGZ_REQ_FAIL;
	}
		
	dbg_msg(GGZ_DBG_CHAT, "Player %d sends %s", p_index, msg);
	
	status = room_emit(players.info[p_index].room, p_index, msg);

	if (es_write_int(p_fd, RSP_CHAT) < 0
	    || es_write_char(p_fd, status) < 0)
		return GGZ_REQ_DISCONNECT;

	return status;
}


static int player_send_error(int p_index, int fd)
{
	return (es_write_int(fd, MSG_ERROR));
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
	
	return 0;    ;
}


/* FIXME: move to type.c */
int type_match_table(int type, int num)
{
	/* FIXME: Do reservation checking properly */
	return ( type == GGZ_TYPE_ALL
		 || (type >= 0 && type == tables.info[num].type_index)
		 || (type == GGZ_TYPE_OPEN && seats_open(tables.info[num]))
		 || type == GGZ_TYPE_RES);
}
		

int player_motd(int p_index, int fd)
{
	dbg_msg(GGZ_DBG_CHAT, "Handling motd request for player %d", p_index);
  	if (!motd_info.use_motd)
		return GGZ_REQ_OK;

 	/* Don't send motd if they're not logged in */
 	if (players.info[p_index].uid == GGZ_UID_NONE) {
		dbg_msg(GGZ_DBG_CHAT, 
			"Player %d requested motd before loggin in",
			p_index);
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

