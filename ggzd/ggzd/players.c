/*
 * File: players.c
 * Author: Brent Hendricks
 * Project: NetGames
 * Date: 10/18/99
 * Desc: Functions for handling players
 *
 * Copyright (C) 1999 Brent Hendricks.
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
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>

#include <easysock.h>
#include <datatypes.h>
#include <players.h>
#include <table.h>
#include <protocols.h>
#include <err_func.h>
#include <chat.h>


/* Timeout for server resync */
#define NG_RESYNC_SEC  5


/* Server wide data structures*/
extern struct GameTypes game_types;
extern struct GameTables game_tables;
extern struct Users players;


/* Local functions for handling players */
static void* player_new(void * sock_ptr);
static void  player_loop(int p_index, int p_fd);
static int   player_handle(int op, int p_index, int p_fd, int * t_fd);
static void  player_remove(int p_index);
static int   player_updates(int p_index);
static int   player_msg_to_sized(int fd_in, int fd_out);
static int   player_msg_from_sized(int fd_in, int fd_out);
static int   player_send_chat(int p_index, char* name, char* chat);
static int   player_chat(int p_index, int p_fd);

static int new_login(int p_index);
static int anon_login(int index);
static int login(int p_index);
static int logout(int p_index);
static int read_table_info(int p_index, int p_fd, int *t_fd);
static int join_table(int p_index, int p_fd, int *t_fd);
static int user_list(int p_index);
static int types_list(int p_index);
static int table_list(int p_index);
static int read_name(int, char[MAX_USER_NAME_LEN]);
static int type_match_table(int type, int num);

/*
 * launch_handler accepts the socket of a new player and launches
 * a new dedicated handler process/thread.
 */
void player_handler_launch(int sock)
{

	pthread_t thread;
	int *sock_ptr;
	int status;

	/* Temporary storage to pass fd */
	if (FAIL(sock_ptr = malloc(sizeof(int))))
		err_sys_exit("malloc error");
	*sock_ptr = sock;
	status = pthread_create(&thread, NULL, player_new, sock_ptr);
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
static void* player_new(void *sock_ptr)
{
	int sock, status, i;

	sock = *((int *) sock_ptr);
	free(sock_ptr);

	/* Detach thread since no one needs to join us */
	status = pthread_detach(pthread_self());
	if (status != 0) {
		errno = status;
		err_sys_exit("pthread_detach error");
	}

	/* Send server ID */
	if (FAIL(es_write_int(sock, MSG_SERVER_ID)) ||
	    FAIL(es_va_write_string(sock, "NetGames-%s", VERSION)))
		pthread_exit(NULL);

	pthread_rwlock_wrlock(&players.lock);
	if (players.count == MAX_USERS) {
		pthread_rwlock_unlock(&players.lock);
		es_write_char(sock, MSG_SERVER_FULL);
		pthread_exit(NULL);
	}

	/* Skip over occupied seats */
	for (i = 0; i < MAX_USERS; i++)
		if (players.info[i].fd < 0) {
			players.info[i].fd = sock;
			break;
		}
	
	players.info[i].pid = pthread_self();
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

	/* Start off listening only to player */
	p_fd = players.info[p_index].fd;
	FD_ZERO(&active_fd_set);
	FD_SET(p_fd, &active_fd_set);

	for (;;) {

		/* Send updated info if need be */
		if (FAIL(player_updates(p_index)))
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
			
			if (FAIL(es_read_int(p_fd, (int *)&op)))
				break;
			
			status = player_handle(op, p_index, p_fd, &t_fd);
			
			if (status == NG_HANDLE_LOGOUT)
				break;
			
			switch (status) {
			case NG_HANDLE_GAME_START:
				/* Player launched or joined a game */
				dbg_msg("Player %d now in game", p_index);
				players.info[p_index].playing = 1;
				FD_SET(t_fd, &active_fd_set);
				break;
			case NG_HANDLE_GAME_OVER:
				dbg_msg("Player %d game-over [user]", p_index);
				game_over = 1;
				break;
			case NG_HANDLE_OK:
				break;
			}
		}
		
		/* Check for data from table */
		if (t_fd != -1  && FD_ISSET(t_fd, &read_fd_set)) {
			if (FAIL(es_write_int(p_fd, RSP_GAME)) ||
			    (status = player_msg_to_sized(t_fd, p_fd)) <= 0) {
				dbg_msg("Player %d game-over [game]", p_index);
				game_over = 1;
			}
			else 
				dbg_msg("Game to User: %d bytes", status);
		}

		/* Clean up after either player or table ends game */
		if (game_over) {
			dbg_msg("Cleaning up player %d's game", p_index);
			players.info[p_index].playing = 0;
			close(t_fd);
			FD_CLR(t_fd, &active_fd_set);
			t_fd = -1;
			pthread_rwlock_wrlock(&players.lock);
			players.info[p_index].table_index = -1;
			pthread_rwlock_unlock(&players.lock);
			game_over = 0;
		}

	} /* for(;;) */

	/* Clean up if there was an error during a game */
	if (t_fd != -1)
		close(t_fd);
}



/*
 * handle_player() receives the opcode of the incoming message, the
 * player's index and player's fd, and the table fd by reference.  It
 * then dispatches the correct function to read in the data and handle
 * any requests.
 *
 * returns: 
 *  NG_HANDLE_GAME_START : player is now in a game.  fd of the game 
 *                         is returned by reference.
 *  NG_HANDLE_GAME_OVER  : player has completed game
 *  NG_HANDLE_LOGOUT     : player is being logged out (possbily due to  error)
 *  NG_HANDLE_OK : nothing special */
int player_handle(int request, int p_index, int p_fd, int *t_fd)
{

	int status;
	UserToControl op = (UserToControl) request;

	switch (op) {
	case REQ_ANON_LOGIN:
		status = anon_login(p_index);
		break;

	case REQ_LOGOUT:
		logout(p_index);
		status = NG_HANDLE_LOGOUT;
		break;

	case REQ_LAUNCH_GAME:
		status = read_table_info(p_index, p_fd, t_fd);
		if (status == 0)
			status = NG_HANDLE_GAME_START;
		break;

	case REQ_JOIN_GAME:
		status = join_table(p_index, p_fd, t_fd);
		if (status == 0)
			status = NG_HANDLE_GAME_START;
		break;

	case REQ_USER_LIST:
		status = user_list(p_index);
		break;

	case REQ_GAME_TYPES:
		status = types_list(p_index);
		break;

	case REQ_TABLE_LIST:
		status = table_list(p_index);
		break;

	case REQ_GAME:
		status = player_msg_from_sized(p_fd, *t_fd); 
		if (status <= 0)
			status = NG_HANDLE_GAME_OVER;
		else 
			dbg_msg("User to Game: %d bytes", status);
		break;
			
	case REQ_CHAT:
		status = player_chat(p_index, p_fd);
		break;

	case REQ_NEW_LOGIN:
	case REQ_LOGIN:
	case REQ_MOTD:
	case REQ_PREF_CHANGE:
	case REQ_REMOVE_USER:
	case REQ_TABLE_OPTIONS:
	case REQ_USER_STAT:
	default:
		dbg_msg("Player %d (uid: %d) requested unimplemented op %d",
			p_index, players.info[p_index].uid, op);
		status = NG_HANDLE_LOGOUT;
	}

	return status;
}


/*
 * remove_player accepts a player index number and then proceeds to
 * remove that player from the list
 */
static void player_remove(int p_index)
{

	int fd;
	
	dbg_msg("Removing player %d (uid: %d)", p_index,
		players.info[p_index].uid);
	pthread_rwlock_wrlock(&players.lock);
	players.info[p_index].uid = NG_UID_NONE;
	players.info[p_index].name[0] = '\0';
	players.info[p_index].table_index = -1;
	players.info[p_index].playing = 0;
	fd = players.info[p_index].fd;
	players.info[p_index].fd = -1;
	players.count--;
	pthread_rwlock_unlock(&players.lock);

	/* FIXME: mark all my chats as read */
	close(fd);
}


/*
 * player_updates checks any updates (player/table/type) lists
 * or chats which need to be sent to the player, and sends them
 */
static int player_updates(int p) {

	int i, count;
	char player[MAX_USER_NAME_LEN + 1];
	char chat[MAX_CHAT_LEN + 1];

	/*FIXME: Add user_list updates */
	/*FIXME: Add game_type updates */
	/*FIXME: Add game_table updates */
	
	/* Send any unread chats */
	count = chat_check_num_unread(p);
	for (i = 0; (i < MAX_CHAT_BUFFER && count > 0) ; i++)
		if (chat_check_unread(p, i)) {
			chat_get(i, player, chat);
			chat_mark_read(p, i);
			if (FAIL(player_send_chat(p, player, chat)))
				return(-1);
			count--;
		}
	
	return 0;
}


/*
 * new_login implements the following exchange:
 * 
 * REQ_NEW_LOGIN
 *  str: login name
 * RSP_NEW_LOGIN
 *  chr: success flag (0 for success, -1 invalid name, -2 duplicate name)
 *  str: initial password (if success)
 *  int: game type checksum (if success)
 */
static int new_login(int p)
{
	/*FIXME: needs rewitten in style of read_table_info */
	char name[MAX_USER_NAME_LEN + 1];

	dbg_msg("Creating new login for player %d", p);

	if (FAIL(read_name(players.info[p].fd, name)))
		return (-1);

	/* FIXME: assign unique uid and create new entry in database */
	pthread_rwlock_wrlock(&players.lock);
	players.info[p].uid = p;
	strncpy(players.info[p].name, name, MAX_USER_NAME_LEN + 1);
	pthread_rwlock_unlock(&players.lock);

	/* FIXME: good password generation */
	/* FIXME: calculate game type checksum (should be done earlier?) */

	if (FAIL(es_write_int(players.info[p].fd, RSP_NEW_LOGIN)) ||
	    FAIL(es_write_char(players.info[p].fd, 0)) ||
	    FAIL(es_write_string(players.info[p].fd, "password")) ||
	    FAIL(es_write_int(players.info[p].fd, 265)))
		return (-1);

	dbg_msg("Successful new login of %s: UID = %d",
		players.info[p].name, players.info[p].uid);

	return 0;

}


static int anon_login(int p)
{

	char name[MAX_USER_NAME_LEN + 1];

	dbg_msg("Creating anonymous login for player %d", p);

	if (FAIL(read_name(players.info[p].fd, name)))
		return (-1);

	/* FIXME: Check validity/uniqueness of name */
	pthread_rwlock_wrlock(&players.lock);
	players.info[p].uid = NG_UID_ANON;
	strncpy(players.info[p].name, name, MAX_USER_NAME_LEN + 1);
	pthread_rwlock_unlock(&players.lock);

	if (FAIL(es_write_int(players.info[p].fd, RSP_ANON_LOGIN)) ||
	    FAIL(es_write_char(players.info[p].fd, 0)) ||
	    FAIL(es_write_int(players.info[p].fd, 265)))
		return (-1);

	dbg_msg("Successful anonymous login of %s", players.info[p].name);

	return 0;
}


static int login(int p)
{
	
	dbg_msg("Handling login for player %d (not implemented)", p);
	return -1;

}


static int logout(int p)
{

	dbg_msg("Handling logout for player %d", p);

	/* FIXME: Saving of stats and other things */
	if (FAIL(es_write_int(players.info[p].fd, RSP_LOGOUT)) ||
	    FAIL(es_write_char(players.info[p].fd, 0)))
		return -1;

	return 0;
}


/*
 * read_table_info implements the following exchange:
 *
 * REQ_LAUNCH_GAME
 *  int: game type index
 *  int: number of players
 *  chr: computer players (2^num)
 *  int: number of reservations (possibly 0)
 *  sequence of
 *    str: login name for reservation
 *  int: size of options struct
 *  struct: game options
 * RSP_LAUNCH_GAME
 *  chr: success flag (0 for success, negative values for various failures)
 * 
 * returns 0 if successful, -1 on error.  If successful, returns table
 * fd by reference
 */
static int read_table_info(int p_index, int p_fd, int *t_fd)
{

	TableInfo table;
	int i, size, t_index = -1;
	int status = 0;
	int fds[2];
	void *options;
	char name[MAX_USER_NAME_LEN + 1];

	dbg_msg("Handling table launch for player %d", p_index);

	if (FAIL(es_read_int(p_fd, &(table.type_index))) ||
	    FAIL(es_read_int(p_fd, &(table.num_seats))) ||
	    FAIL(es_read_char(p_fd, &(table.comp_players))) ||
	    FAIL(es_read_int(p_fd, &table.num_reserves)))
		return -1;

	/* Read in reservations */
	for (i = 0; i < table.num_reserves; i++) {
		if (FAIL(read_name(p_fd, name)))
			return -1;
#if 0
		if ((table.reserve[i] = uid_lookup(name)) < 0)
			status = E_USR_LOOKUP;
#endif
		table.reserve[i] = i;
	}

	/* Read in game specific options */
	if (FAIL(es_read_int(p_fd, &(size))))
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
		table.num_humans =
			table.num_seats - num_comp_play(table.comp_players);
		table.open_seats = table.num_humans - 1;
		table.playing = 0;
		table.pid = -1;
		table.fd_to_game = -1;
		table.players[0] = p_index;
		table.player_fd[0] = fds[0];
		for (i = 1; i < 8; i++) {
			table.players[i] = -1;
		}
		table.options = options;
		pthread_mutex_init(&table.seats_lock, NULL);
		pthread_cond_init(&table.seats_cond, NULL);
	}

	/* Check validity of table info (not options) */
	if (status == 0 && (table_check(p_index, table) < 0))
		status = E_BAD_OPTIONS;

	/* Find open table */
	if (status == 0) {
		pthread_rwlock_wrlock(&game_tables.lock);
		if (game_tables.count == MAX_TABLES) {
			pthread_rwlock_unlock(&game_tables.lock);
			status = E_ROOM_FULL;
		} else {
			for (i = 0; i < MAX_TABLES; i++)
				if (game_tables.info[i].type_index < 0) {
					game_tables.info[i] = table;
					break;
				}
			t_index = i;
			game_tables.count++;
			pthread_rwlock_unlock(&game_tables.lock);
		}
	}

	/* Attempt to do launch of table-controller */
	if (status == 0 && (table_handler_launch(t_index) < 0))
		status = E_LAUNCH_FAIL;

	/* Return status */
	if (FAIL(es_write_int(p_fd, RSP_LAUNCH_GAME)) ||
	    FAIL(es_write_char(p_fd, (char) status)))
		status = E_RESPOND_FAIL;

	/* Cleanup if something failed */
	switch (status) {
	case E_LAUNCH_FAIL:
		pthread_rwlock_wrlock(&game_tables.lock);
		game_tables.info[t_index].pid = -1;
		game_tables.count--;
		pthread_rwlock_unlock(&game_tables.lock);
	case E_TABLE_FULL:
	case E_BAD_OPTIONS:
		free(options);
		dbg_msg("Player %d's table launch failed with err %d",
			p_index, status);
		break;
	case 0:		/* Everything OK */
		pthread_rwlock_wrlock(&players.lock);
		players.info[p_index].table_index = t_index;
		pthread_rwlock_unlock(&players.lock);
		break;
	default:
		break;
	}

	return (status);
}


/* FIXME: Some day we'll worry about reservations. Not today*/
static int join_table(int p_index, int p_fd, int *t_fd)
{
	int i, t_index, fds[2];
	int status = 0;

	dbg_msg("Handling table join for player %d", p_index);
	
	if (FAIL(es_read_int(p_fd, &(t_index))))
		return -1;

	dbg_msg("Player %d attempting to join table %d", p_index, t_index);
	
	pthread_rwlock_wrlock(&game_tables.lock);
	if (game_tables.info[t_index].type_index == -1) {
		pthread_rwlock_unlock(&game_tables.lock);
		status = E_TABLE_EMPTY;
	}
	else if (game_tables.info[t_index].open_seats == 0) {
		pthread_rwlock_unlock(&game_tables.lock);
		status = E_TABLE_FULL;
	} else {
		/* Take my seat */
		for (i = 0; i < game_tables.info[t_index].num_seats; i++)
			if (game_tables.info[t_index].players[i] < 0) {
				game_tables.info[t_index].players[i] =
					p_index;
				/* Create socketpair for communication */
				socketpair(PF_UNIX, SOCK_STREAM, 0, fds);
				game_tables.info[t_index].player_fd[i] =
					fds[0];
				*t_fd = fds[1];
				dbg_msg("Player %d sitting in seat %d",
					p_index, i);
				break;
			}

		game_tables.info[t_index].open_seats--;
		pthread_rwlock_unlock(&game_tables.lock);

		pthread_mutex_lock(&game_tables.info[t_index].seats_lock);
		pthread_cond_signal(&game_tables.info[t_index].seats_cond);
		pthread_mutex_unlock(&game_tables.info[t_index].
				     seats_lock);
	}


	/* Return status */
	if (FAIL(es_write_int(p_fd, RSP_JOIN_GAME)) ||
	    FAIL(es_write_char(p_fd, (char) status)))
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
		pthread_rwlock_unlock(&players.lock);
		break;
	default:
		break;
	}

	return status;

}


static int user_list(int p_index)
{

	int i, fd, count;
	UserInfo info[MAX_USERS];


	dbg_msg("Handling user list request for player %d", p_index);
	fd = players.info[p_index].fd;

	pthread_rwlock_rdlock(&players.lock);
	count = players.count;
	for (i = 0; i < count; i++)
		info[i] = players.info[i];
	pthread_rwlock_unlock(&players.lock);

	if (FAIL(es_write_int(fd, RSP_USER_LIST)) ||
	    FAIL(es_write_int(fd, count))) return (-1);

	for (i = 0; i < count; i++) {
		if (FAIL(es_write_string(fd, info[i].name)) ||
		    FAIL(es_write_int(fd, info[i].table_index)))
			return (-1);
	}

	return (0);
}


static int types_list(int p_index)
{

	char verbose;
	int i, fd, count;

	GameInfo info[MAX_GAME_TYPES];

	fd = players.info[p_index].fd;

	if (FAIL(es_read_char(fd, &verbose)))
		return (-1);

	pthread_rwlock_rdlock(&game_types.lock);
	count = game_types.count;
	for (i = 0; i < count; i++)
		info[i] = game_types.info[i];
	pthread_rwlock_unlock(&game_types.lock);

	if (FAIL(es_write_int(fd, RSP_GAME_TYPES)) ||
	    FAIL(es_write_int(fd, count))) return (-1);

	for (i = 0; i < count; i++) {
		if (FAIL(es_write_int(fd, i)) ||
		    FAIL(es_write_string(fd, info[i].name)) ||
		    FAIL(es_write_string(fd, info[i].version)))
			return (-1);
		if (!verbose)
			continue;
		if (FAIL(es_write_string(fd, info[i].desc)) ||
		    FAIL(es_write_string(fd, info[i].author)) ||
		    FAIL(es_write_string(fd, info[i].homepage)))
			return (-1);
	}

	return (0);
}


static int table_list(int p_index)
{

	int i, j, fd, type, player_num, seated_humans;
	int count = 0;
	char name[MAX_USER_NAME_LEN + 1];

	TableInfo tables[MAX_TABLES];
	int indices[MAX_TABLES];
	
	fd = players.info[p_index].fd;

	if (FAIL(es_read_int(fd, &type)))
		return (-1);

	/* Copy tables of interest to local list */
	pthread_rwlock_rdlock(&game_tables.lock);
	for (i = 0; i < MAX_TABLES; i++) {
		if (game_tables.info[i].type_index != -1
		    && type_match_table(type, i)) {
			tables[count] = game_tables.info[i];
			indices[count++] = i;
		}
	}
	pthread_rwlock_unlock(&game_tables.lock);

	if (FAIL(es_write_int(fd, RSP_TABLE_LIST)) ||
	    FAIL(es_write_int(fd, count))) 
		return (-1);

	for (i = 0; i < count; i++) {
		seated_humans = tables[i].num_humans - tables[i].open_seats;
		if (FAIL(es_write_int(fd, indices[i]))
		    || FAIL(es_write_int(fd, tables[i].type_index))
		    || FAIL(es_write_char(fd, tables[i].playing))
		    || FAIL(es_write_int(fd, tables[i].num_seats))
		    || FAIL(es_write_int(fd, tables[i].open_seats))
		    || FAIL(es_write_int(fd, seated_humans)))
			return (-1);

		for (j = 0; j < seated_humans; j++) {
			player_num = tables[i].players[j];
			pthread_rwlock_rdlock(&players.lock);
			strcpy(name, players.info[player_num].name);
			pthread_rwlock_unlock(&players.lock);
			if (FAIL(es_write_string(fd, name)))
				return (-1);
		}
	}

	return (0);
}


static int player_msg_to_sized(int in, int out) 
{
	char buf[4096];
	int size;
	
	if ( (size = read(in, buf, 4096)) <= 0)
		return(-1);
	
	if (FAIL(es_write_int(out, size)) ||
	    FAIL(es_writen(out, buf, size)))
		return(-1);

	return size;
}


static int player_msg_from_sized(int in, int out) 
{
	char buf[4096];
	int size;
	
	if (FAIL(es_read_int(in, &size)) ||
	    FAIL(es_readn(in, buf, size)))
		return (-1);
	
	return (es_writen(out, buf, size));
}


static int player_send_chat(int p_index, char* name, char* msg) 
{
	int fd;
	
	fd = players.info[p_index].fd;	
	if (FAIL(es_write_int(fd, MSG_CHAT)) ||
	    FAIL(es_write_string(fd, name)) ||
	    FAIL(es_write_string(fd, msg)))
		return(-1);
	
	return 0;
}


static int player_chat(int p_index, int p_fd) 
{
	char* tmp;
	char msg[MAX_CHAT_LEN + 1];
	int status;

	dbg_msg("Handling chat for player %d", p_index);

	if (FAIL(es_read_string_alloc(p_fd, &tmp)))
		return(-1);
	
	strncpy(msg, tmp, MAX_CHAT_LEN);
	free(tmp);
	msg[MAX_CHAT_LEN] = '\0';  /* Make sure strings are null-terminated */
	status = chat_add(p_index, msg);

	if (FAIL(es_write_int(p_fd, RSP_TABLE_LIST)) ||
	    FAIL(es_write_int(p_fd, status))) 
		return (-1);

	return 0;
}


/*
 * Utility function for reading in names
 */
static int read_name(int sock, char name[MAX_USER_NAME_LEN])
{

	char *tmp;
	int status = -1;

	if (!FAIL(es_read_string_alloc(sock, &(tmp)))) {

		strncpy(name, tmp, MAX_USER_NAME_LEN);
		free(tmp);

		/* Make sure names are null-terminated */
		name[MAX_USER_NAME_LEN] = '\0';

		status = 0;
	}

	return status;
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
		 || (type >= 0 && type == game_tables.info[num].type_index)
		 || (type == NG_TYPE_OPEN && game_tables.info[num].open_seats)
		 || type == NG_TYPE_RES);
}
		

