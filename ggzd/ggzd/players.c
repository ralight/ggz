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
#include <chat.h>
#include <seats.h>
#include <motd.h>


/* Timeout for server resync */
#define NG_RESYNC_SEC  5


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
static int   player_updates(int p_index, int p_fd, time_t*, time_t*, time_t*);
static int   player_msg_to_sized(int fd_in, int fd_out);
static int   player_msg_from_sized(int fd_in, int fd_out);
static int   player_send_chat(int p_index, int fd, char* name, char* chat);
static int   player_chat(int p_index, int p_fd);
static int   player_login_anon(int p, int fd);
static int   player_login_new(int p_index);
static int   player_logout(int p, int fd);
static int   player_table_launch(int p_index, int p_fd, int *t_fd);
static int   player_table_join(int p_index, int p_fd, int *t_fd);
static int   player_list_players(int p_index, int fd);
static int   player_list_types(int p_index, int fd);
static int   player_list_tables(int p_index, int fd);
static int   player_send_error(int p_index, int fd);

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
	struct hostent *host;

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
	players.info[i].uid = NG_UID_NONE;
	players.info[i].playing = 0;
	players.info[i].pid = pthread_self();
	strcpy(players.info[i].name, "(none)");
	/* FIXME: inet_ntoa is not reentrant */
	strcpy(players.info[i].ip_addr, inet_ntoa(addr.sin_addr));
	players.info[i].hostname = hostname;
	players.count++;
	pthread_rwlock_unlock(&players.lock);

	dbg_msg("New player %d connected", i);
	
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

	/* Start off listening only to player */
	p_fd = players.info[p_index].fd;
	FD_ZERO(&active_fd_set);
	FD_SET(p_fd, &active_fd_set);

	/* Initialize timestamps */
	player_ts = table_ts = type_ts = time(NULL);

	for (;;) {
		/* Send updated info if need be */
		if (player_updates(p_index, p_fd, &player_ts, &table_ts, 
				   &type_ts) < 0)
			break;
		
		read_fd_set = active_fd_set;
		fd_max = ((p_fd > t_fd) ? p_fd : t_fd) + 1;
		
		/* Setup timeout for select*/
		timer.tv_sec = NG_RESYNC_SEC;
		timer.tv_usec = 0;
		
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
			
			if (status < 0)
				break;
			
			switch (status) {
			case GGZ_REQ_TABLE_JOIN:
				/* Player launched or joined a game */
				dbg_msg("Player %d now in game", p_index);
				pthread_rwlock_wrlock(&players.lock);
				players.info[p_index].playing = 1;
				pthread_rwlock_unlock(&players.lock);
				FD_SET(t_fd, &active_fd_set);
				break;
			case GGZ_REQ_TABLE_LEAVE:
				dbg_msg("Player %d game-over [user]", p_index);
				game_over = 1;
				break;
			case GGZ_REQ_OK:
				break;
			}
		}
		
		/* Check for data from table */
		if (t_fd != -1  && FD_ISSET(t_fd, &read_fd_set)) {
			if ( (status = player_msg_to_sized(t_fd, p_fd)) <= 0) {
				dbg_msg("Player %d game-over [game]", p_index);
				game_over = 1;
			}
			else 
				dbg_msg("Game to User: %d bytes", status);
		}

		/* Clean up after either player or table ends game,
		   but don't try to do both */
		if (game_over && t_fd != -1) {
			dbg_msg("Cleaning up player %d's game", p_index);
			close(t_fd);
			FD_CLR(t_fd, &active_fd_set);
			t_fd = -1;
			pthread_rwlock_wrlock(&players.lock);
			players.info[p_index].playing = 0;
			players.info[p_index].table_index = -1;
			players.timestamp = time(NULL);
			pthread_rwlock_unlock(&players.lock);
			game_over = 0;
		}

	} /* for(;;) */

	/* Clean up if there was an error during a game */
	if (t_fd != -1)
		close(t_fd);
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
 *  GGZ_REQ_LOGOUT       : player is being logged out (possbily due to error)
 *  GGZ_REQ_OK           : nothing special 
 */
int player_handle(int request, int p_index, int p_fd, int *t_fd)
{
	int status;
	UserToControl op = (UserToControl)request;

	switch (op) {
	case REQ_LOGIN_ANON:
		status = player_login_anon(p_index, p_fd);
		break;

	case REQ_LOGOUT:
		player_logout(p_index, p_fd);
		status = GGZ_REQ_LOGOUT;
		break;

	case REQ_TABLE_LAUNCH:
		status = player_table_launch(p_index, p_fd, t_fd);
		if (status == 0)
			status = GGZ_REQ_TABLE_JOIN;
		break;

	case REQ_TABLE_JOIN:
		status = player_table_join(p_index, p_fd, t_fd);
		if (status == 0)
			status = GGZ_REQ_TABLE_JOIN;
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

	case REQ_GAME:
		status = player_msg_from_sized(p_fd, *t_fd); 
		if (status <= 0)
			status = GGZ_REQ_TABLE_LEAVE;
		else 
			dbg_msg("User to Game: %d bytes", status);
		break;
			
	case REQ_CHAT:
		status = player_chat(p_index, p_fd);
		break;

	case REQ_LOGIN_NEW:
	case REQ_LOGIN:
	case REQ_PREF_CHANGE:
	case REQ_REMOVE_USER:
	case REQ_TABLE_OPTIONS:
	case REQ_USER_STAT:
	case REQ_TABLE_LEAVE:
	default:
		dbg_msg("Player %d (uid: %d) requested unimplemented op %d",
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
	
	dbg_msg("Removing player %d (uid: %d)", p_index, 
		players.info[p_index].uid);
	pthread_rwlock_wrlock(&players.lock);
	fd = players.info[p_index].fd;
	players.info[p_index].fd = -1;
	players.count--;
	players.timestamp = time(NULL);
	chat_mark_all_read(p_index);
	pthread_rwlock_unlock(&players.lock);

	close(fd);
}


/*
 * player_updates checks any updates (player/table/type) lists
 * or chats which need to be sent to the player, and sends them
 */
static int player_updates(int p, int fd, time_t* player_ts, time_t* table_ts, 
			  time_t* type_ts) {

	int i, count;
	char player[MAX_USER_NAME_LEN + 1];
	char chat[MAX_CHAT_LEN + 1];
	char user_update = 0;
	char table_update = 0;
	char type_update = 0;

	/* Check for player list updates */
	pthread_rwlock_rdlock(&players.lock);
	if (difftime(players.timestamp, *player_ts) != 0 ) {
		*player_ts = players.timestamp;
		user_update = 1;
		dbg_msg("Player %d needs player list update", p);
	}
	pthread_rwlock_unlock(&players.lock);

	/* Check for table list updates*/
	pthread_rwlock_rdlock(&tables.lock);
	if (difftime(tables.timestamp, *table_ts) != 0 ) {
		*table_ts = tables.timestamp;
		table_update = 1;
		dbg_msg("Player %d needs table list update", p);
	}
	pthread_rwlock_unlock(&tables.lock);

	/* Check for game type list updates*/
	pthread_rwlock_rdlock(&game_types.lock);
	if (difftime(game_types.timestamp, *type_ts) != 0 ) {
		*type_ts = game_types.timestamp;
		type_update = 1;
		dbg_msg("Player %d needs type list update", p);
	}
	pthread_rwlock_unlock(&game_types.lock);

	/* Send out proper update messages */
	if ((user_update && es_write_int(fd, MSG_UPDATE_PLAYERS) < 0)
	    || (type_update && es_write_int(fd, MSG_UPDATE_TYPES) < 0)
	    || (table_update && es_write_int(fd, MSG_UPDATE_TABLES) < 0))
		return(-1);
	
	/* Send any unread chats */
	count = chat_check_num_unread(p);
	for (i = 0; (i < MAX_CHAT_BUFFER && count > 0) ; i++)
		if (chat_check_unread(p, i)) {
			chat_get(i, player, chat);
			chat_mark_read(p, i);
			if (player_send_chat(p, fd, player, chat) < 0)
				return(-1);
			count--;
		}
	
	return 0;
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

	dbg_msg("Creating new login for player %d", p);

	if (read_name(players.info[p].fd, name) < 0)
		return (-1);

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
		return (-1);

	/* Send off the Message Of The Day */
	if (motd_info.use_motd && (motd_send_motd(players.info[p].fd) < 0)) {
		player_remove(p);
		return -1;
	}

	dbg_msg("Successful new login of %s: UID = %d",
		players.info[p].name, players.info[p].uid);

	return 0;

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

	dbg_msg("Creating anonymous login for player %d", p);
	
	if (read_name(fd, name) < 0)
		return (-1);

	/* Check name for uniqueness */
	pthread_rwlock_rdlock(&players.lock);
	for (i = 0; i < MAX_USERS; i++)
		if (players.info[i].fd != -1 
		    && !(strcmp(name, players.info[i].name)))
			break;
	pthread_rwlock_unlock(&players.lock);

	/* FIXME: need to check vs. database too */
	if (i != MAX_USERS) {
		dbg_msg("Unsuccessful anonymous login of %s", name);
		if (es_write_int(fd, RSP_LOGIN_ANON) < 0
		    || es_write_char(fd, -1) < 0)
			return -1;
		return 0;
	}

	pthread_rwlock_wrlock(&players.lock);
	players.info[p].uid = NG_UID_ANON;
	strncpy(players.info[p].name, name, MAX_USER_NAME_LEN + 1);
	players.timestamp = time(NULL);
	ip_addr = players.info[p].ip_addr;
	hostname = players.info[p].hostname;
	pthread_rwlock_unlock(&players.lock);

	if (es_write_int(fd, RSP_LOGIN_ANON) < 0
	    || es_write_char(fd, 0) < 0
	    || es_write_int(fd, 265) < 0)
		return -1;

	/* Send off the Message Of The Day */
	if (motd_info.use_motd && (motd_send_motd(players.info[p].fd) < 0)) {
		player_remove(p);
		return -1;
	}

	dbg_msg("Successful anonymous login of %s", name);

	if(hostname)
		log_msg(CONNECTION_INFO,
			"Anonymous player %s logged in from %s (%s)",
			name, hostname, ip_addr);
	else
		log_msg(CONNECTION_INFO,
			"Anonymous player %s logged in from %s", name, ip_addr);
	
	return 0;
}


/*
 * player_logout implements the following exchange:
 * 
 * REQ_LOGOUT
 *  str: login name
 * RSP_LOGOUT
 *  chr: success flag (0 for success, -1 for error )
 */
static int player_logout(int p, int fd)
{
	dbg_msg("Handling logout for player %d", p);
	
	/* FIXME: Saving of stats and other things */
	if (es_write_int(fd, RSP_LOGOUT) < 0 
	    || es_write_char(fd, 0) < 0)
		return -1;
	
	return 0;
}


/*
 * player_table_launch implements the following exchange:
 *
 * REQ_TABLE_LAUNCH
 *  int: game type index
 *  int: number of players
 *  chr: computer players (2^num)
 *  int: number of reservations (possibly 0)
 *  sequence of
 *    str: login name for reservation
 *  int: size of options struct
 *  struct: game options
 * RSP_TABLE_LAUNCH
 *  chr: success flag (0 for success, negative values for various failures)
 * 
 * returns 0 if successful, -1 on error.  If successful, returns table
 * fd by reference
 */

/*
 * FIXME: Much of this function should be put into table_launch().
 * Then upon return from table_launch() we send out RSP_TABLE_LAUNCH
 */
static int player_table_launch(int p_index, int p_fd, int *t_fd)
{
	TableInfo table;
	int i, size, t_index = -1;
	int status = 0;
	int fds[2];
	int seats;
	void *options;
	char name[MAX_USER_NAME_LEN + 1];

	dbg_msg("Handling table launch for player %d", p_index);

	if (es_read_int(p_fd, &table.type_index) < 0
	    || es_read_string(p_fd, table.desc) < 0
	    || es_read_int(p_fd, &seats) < 0)
		return -1;

	/* FIXME: somehow check for seats > MAX_TABLE_SIZE */
	/* Read in seat assignments */
	for (i = 1; i < seats; i++) {
		if (es_read_int(p_fd, &table.seats[i]) < 0)
			return -1;
		if (table.seats[i] == GGZ_SEAT_RESV
		    && read_name(p_fd, name) < 0)
			return -1;

		/*
		 * FIXME: lookup uid of name and asign to table.reserve[i]
		 * upon error, set status = E_USR_LOOKUP
		 */
	}

	/* Read in game specific options */
	if (es_read_int(p_fd, &size) < 0)
		return -1;
	if ((options = malloc(size)) < 0)
		err_sys_exit("malloc error");
	if (es_readn(p_fd, options, size) <= 0) {
		free(options);
		return -1;
	}

	/* Create socketpair for communication */
	socketpair(PF_UNIX, SOCK_STREAM, 0, fds);
	*t_fd = fds[1];

	/* Fill remaining parameters */
	if (status == 0) {
		table.playing = 0;
		table.pid = -1;
		table.fd_to_game = -1;
		table.player_fd[0] = fds[0];
		table.seats[0] = p_index;
		for (i = seats; i < MAX_TABLE_SIZE; i++)
			table.seats[i] = GGZ_SEAT_NONE;
		table.options = options;
		pthread_mutex_init(&table.seats_lock, NULL);
		pthread_cond_init(&table.seats_cond, NULL);
	}

	/* Check validity of table info (not options) */
	if (status == 0 && (table_check(p_index, table) < 0))
		status = E_BAD_OPTIONS;

	/* Find open table */
	if (status == 0) {
		pthread_rwlock_wrlock(&tables.lock);
		if (tables.count == MAX_TABLES) {
			pthread_rwlock_unlock(&tables.lock);
			status = E_ROOM_FULL;
		} else {
			for (i = 0; i < MAX_TABLES; i++)
				if (tables.info[i].type_index < 0)
					break;

			t_index = i;
			tables.info[t_index] = table;
			tables.count++;
			tables.timestamp = time(NULL);
			pthread_rwlock_unlock(&tables.lock);
		}
	}

	/* Attempt to do launch of table-controller */
	if (status == 0 && (table_handler_launch(t_index) < 0))
		status = E_LAUNCH_FAIL;

	/* Return status */
	if (es_write_int(p_fd, RSP_TABLE_LAUNCH) < 0
	    || es_write_char(p_fd, (char) status) < 0)
		status = E_RESPOND_FAIL;

	/* Cleanup if something failed */
	switch (status) {
	case E_RESPOND_FAIL:
		/* 
		 *  FIXME: lots to do here since table thread was launched 
		 *  perhaps just remove user from table and let table die 
		 * on it's own.
		 */
		break;
	case E_LAUNCH_FAIL:
		pthread_rwlock_wrlock(&tables.lock);
		tables.info[t_index].pid = -1;
		tables.count--;
		pthread_rwlock_unlock(&tables.lock);
	case E_TABLE_FULL:
	case E_BAD_OPTIONS:
		free(options);
		dbg_msg("Player %d's table launch failed with err %d",
			p_index, status);
		break;
	case 0:		/* Everything OK */
		pthread_rwlock_wrlock(&players.lock);
		players.info[p_index].table_index = t_index;
		players.timestamp = time(NULL);
		pthread_rwlock_unlock(&players.lock);
		break;
	default:
		break;
	}

	return (status);
}


/*
 * player_table_join implements the following exchange:
 *
 * REQ_TABLE_JOIN
 *  int: game type index
 *  int: number of players
 *  chr: computer players (2^num)
 *  int: number of reservations (possibly 0)
 *  sequence of
 *    str: login name for reservation
 *  int: size of options struct
 *  struct: game options
 * RSP_TABLE_JOIN
 *  chr: success flag (0 for success, negative values for various failures)
 * 
 * returns 0 if successful, -1 on error.  If successful, returns table
 * fd by reference
 */
/* FIXME: Some day we'll worry about reservations. Not today*/
static int player_table_join(int p_index, int p_fd, int *t_fd)
{
	int i, t_index, fds[2];
	int status = 0;

	dbg_msg("Handling table join for player %d", p_index);
	if (es_read_int(p_fd, &t_index) < 0)
		return -1;
	
	dbg_msg("Player %d attempting to join table %d", p_index, t_index);

	pthread_rwlock_wrlock(&tables.lock);
	if (tables.info[t_index].type_index == -1) {
		pthread_rwlock_unlock(&tables.lock);
		status = E_TABLE_EMPTY;
	}
	else if (seats_open(tables.info[t_index]) == 0) {
		pthread_rwlock_unlock(&tables.lock);
		status = E_TABLE_FULL;
	} else {
		/* Take my seat */
		for (i = 0; i < seats_num(tables.info[t_index]); i++)
			if (tables.info[t_index].seats[i] == GGZ_SEAT_OPEN) {
				tables.info[t_index].seats[i] = p_index;
				/* Create socketpair for communication */
				socketpair(PF_UNIX, SOCK_STREAM, 0, fds);
				tables.info[t_index].player_fd[i] = fds[0];
				*t_fd = fds[1];
				dbg_msg("Player %d in seat %d", p_index, i);
				break;
			}

		tables.timestamp = time(NULL);
	        pthread_rwlock_unlock(&tables.lock);

		pthread_mutex_lock(&tables.info[t_index].seats_lock);
		pthread_cond_signal(&tables.info[t_index].seats_cond);
		pthread_mutex_unlock(&tables.info[t_index].seats_lock);
	}


	/* Return status */
	if (es_write_int(p_fd, RSP_TABLE_JOIN) < 0
	    || es_write_char(p_fd, (char)status) < 0)
		status = E_RESPOND_FAIL;

	switch (status) {
	case E_RESPOND_FAIL:
		/* FIXME: must do major cleanup to remove player from game */
	case E_TABLE_EMPTY:
	case E_TABLE_FULL:
	case E_BAD_OPTIONS:
		dbg_msg("Player %d's table join failed with err %d",
			p_index, status);
		break;
	case 0:		/* Everything OK */
		pthread_rwlock_wrlock(&players.lock);
		players.info[p_index].table_index = t_index;
		players.timestamp = time(NULL);
		pthread_rwlock_unlock(&players.lock);
		break;
	default:
		break;
	}

	return status;

}


static int player_list_players(int p_index, int fd)
{
	int i, count = 0;
	UserInfo info[MAX_USERS];

	dbg_msg("Handling player list request for player %d", p_index);

	pthread_rwlock_rdlock(&players.lock);
	for (i = 0; (i < MAX_USERS && count < players.count); i++)
		if (players.info[i].fd != -1)
			info[count++] = players.info[i];
	pthread_rwlock_unlock(&players.lock);
	
	if (es_write_int(fd, RSP_LIST_PLAYERS) < 0
	    || es_write_int(fd, count) < 0)
		return (-1);

	for (i = 0; i < count; i++) {
		if (es_write_string(fd, info[i].name) < 0
		    || es_write_int(fd, info[i].table_index) < 0)
			return (-1);
	}

	return (0);
}


static int player_list_types(int p_index, int fd)
{
	char verbose;
	int i, count = 0;

	GameInfo info[MAX_GAME_TYPES];

	if (es_read_char(fd, &verbose) < 0)
		return (-1);

	pthread_rwlock_rdlock(&game_types.lock);
	for (i = 0; (i < MAX_GAME_TYPES && count < game_types.count); i++)
		if (game_types.info[i].enabled)
			info[count++] = game_types.info[i];
	pthread_rwlock_unlock(&game_types.lock);

	if (es_write_int(fd, RSP_LIST_TYPES) < 0
	    || es_write_int(fd, count) < 0)
		return (-1);

	for (i = 0; i < count; i++) {
		if (es_write_int(fd, i) < 0
		    || es_write_string(fd, info[i].name) < 0
		    || es_write_string(fd, info[i].version) < 0
		    || es_write_char(fd, info[i].num_play_allow) < 0)
			return (-1);
		if (!verbose)
			continue;
		if (es_write_string(fd, info[i].desc) < 0
		    || es_write_string(fd, info[i].author) < 0
		    || es_write_string(fd, info[i].homepage) < 0)
			return (-1);
	}

	return (0);
}


static int player_list_tables(int p_index, int fd)
{

	int i, j, type, index, uid;
	int count = 0;
	char name[MAX_USER_NAME_LEN + 1];

	TableInfo my_tables[MAX_TABLES];
	int indices[MAX_TABLES];
	
	if (es_read_int(fd, &type) < 0)
		return (-1);

	/* Copy tables of interest to local list */
	pthread_rwlock_rdlock(&tables.lock);
	for (i = 0; (i < MAX_TABLES && count < tables.count); i++) {
		if (tables.info[i].type_index != -1 
		    && type_match_table(type, i)) {
			my_tables[count] = tables.info[i];
			indices[count++] = i;
		}
	}
	pthread_rwlock_unlock(&tables.lock);

	if (es_write_int(fd, RSP_LIST_TABLES) < 0
	    || es_write_int(fd, count) < 0)
		return (-1);

	for (i = 0; i < count; i++) {
		if (es_write_int(fd, indices[i]) < 0
		    || es_write_int(fd, my_tables[i].type_index) < 0
		    || es_write_char(fd, my_tables[i].playing) <0
		    || es_write_int(fd, seats_num(my_tables[i])) < 0)
			return (-1);

		for (j = 0; j < seats_num(my_tables[i]); j++) {

			if (es_write_int(fd, my_tables[i].seats[j]) < 0)
				return -1;

			switch(my_tables[i].seats[j]) {

			case GGZ_SEAT_OPEN:
			case GGZ_SEAT_COMP:
				continue;  /* no name for these */
			case GGZ_SEAT_RESV:
				uid = my_tables[i].reserve[j];
				/* Look up player name by uid */
				strcpy(name,"reserved");
				break;
			default: /* must be a player index */
				index = my_tables[i].seats[j];
				pthread_rwlock_rdlock(&players.lock);
				strcpy(name, players.info[index].name);
				pthread_rwlock_unlock(&players.lock);
			}

			if (es_write_string(fd, name) < 0)
				return (-1);
		}
		if (es_write_string(fd, my_tables[i].desc) < 0)
			return (-1);
	}

	return (0);
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


static int player_send_chat(int p_index, int fd, char* name, char* msg) 
{
	if (es_write_int(fd, MSG_CHAT) < 0
	    || es_write_string(fd, name) < 0
	    || es_write_string(fd, msg) < 0)
		return(-1);
	
	return 0;
}


static int player_chat(int p_index, int p_fd) 
{
	char* tmp;
	char msg[MAX_CHAT_LEN + 1];
	int status;

	dbg_msg("Handling chat for player %d", p_index);

	if (es_read_string_alloc(p_fd, &tmp) < 0)
		return(-1);
	
	strncpy(msg, tmp, MAX_CHAT_LEN);
	free(tmp);
	msg[MAX_CHAT_LEN] = '\0';  /* Make sure strings are null-terminated */
	status = chat_add(p_index, players.info[p_index].name, msg);

	if (es_write_int(p_fd, RSP_CHAT) < 0
	    || es_write_char(p_fd, status) < 0)
		return (-1);

	return 0;
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
	name[MAX_USER_NAME_LEN + 1] = '\0';
	
	return 0;    ;
}


/* Count computer players based on mask */
int num_comp_play(unsigned char mask)
{

	int i, count = 0;

	for (i = 0; i < 8; i++) {
		count += (mask / 128);
		mask = (mask << 1);
	}

	return count;

}


/* FIXME: move to type.c */
int type_match_table(int type, int num)
{
	/* FIXME: Do reservation checking properly */
	return ( type == NG_TYPE_ALL
		 || (type >= 0 && type == tables.info[num].type_index)
		 || (type == NG_TYPE_OPEN && seats_open(tables.info[num]))
		 || type == NG_TYPE_RES);
}
		

