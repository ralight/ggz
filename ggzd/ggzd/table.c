/*
 * File: table.c
 * Author: Brent Hendricks
 * Project: GGZ Server
 * Date: 1/9/00
 * Desc: Functions for handling tables
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

#include <sys/socket.h>
#include <sys/signal.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

#include <ggzd.h>
#include <datatypes.h>
#include <protocols.h>
#include <err_func.h>
#include <table.h>
#include <players.h>
#include <seats.h>
#include <transit.h>
#include <room.h>

#define GGZ_RESYNC_SEC  0
#define GGZ_RESYNC_USEC 500000

/* Server wide data structures*/
extern struct GameTypes game_types;
extern struct GameTables tables;
extern struct Users players;

/* Local functions for handling tables */
static int   table_check(int p_index, TableInfo table);
static int   table_handler_launch(int t_index);
static void* table_new(void *index_ptr);
static void  table_fork(int t_index);
static void  table_loop(int t_index);
static int   table_handle(int op, int index, int fd, char* transit);
static void  table_remove(int t_index);
static void  table_run_game(int t_index, char *path);
static int   table_send_opt(int t_index);
static int   table_game_over(int index, int fd);
static int   table_log(int index, int fd, char debug);
static int   table_game_launch(int index, int fd);
static int   table_game_join(int index, int fd, char* transit);
static int   table_game_leave(int index, int fd, char* transit);
static int   table_event_enqueue(unsigned int table, unsigned char opcode);
static int   table_transit_event_enqueue(unsigned int table, 
					 unsigned char opcode,
					 unsigned int player,
					 unsigned int seat);
static int   table_pack(void** data, unsigned char opcode, unsigned int table);
static int table_state_pack(void** data, unsigned char opcode, 
			    unsigned int table, unsigned char state);
static int   table_transit_pack(void** data, unsigned char opcode, 
				unsigned int table, unsigned int player, 
				unsigned int seat);
static int   table_event_callback(int p_index, int size, void* data);


/*
 * table_check() - Logs seating assignments to debug
 *                 and verifies their validity
 *
 * Returns:  0 on success
 *           E_BAD_OPTIONS if invalid
 */
static int table_check(int p_index, TableInfo table)
{
	int i, status=0;
	int ai_total=seats_bot(table);
	int seat_total=seats_num(table);
	int g_type=table.type_index;
        char allow_bits[] = { GGZ_ALLOW_ZERO, GGZ_ALLOW_ONE, GGZ_ALLOW_TWO,
                GGZ_ALLOW_THREE, GGZ_ALLOW_FOUR, GGZ_ALLOW_FIVE,
                GGZ_ALLOW_SIX, GGZ_ALLOW_SEVEN, GGZ_ALLOW_EIGHT };

	dbg_msg(GGZ_DBG_TABLE, "Player %d launching table of type %d", p_index,
		g_type);

	/* Display and verify total seats and bot seats */
	if(game_types.info[g_type].player_allow_mask & allow_bits[seat_total])
		dbg_msg(GGZ_DBG_TABLE, "Num_seats  : %d (accept)", seat_total);
	else {
		dbg_msg(GGZ_DBG_TABLE, "Num_seats  : %d (invalid)", seat_total);
		status = E_BAD_OPTIONS;
	}
	if(game_types.info[g_type].bot_allow_mask & allow_bits[ai_total]
	   || ai_total == 0)
		dbg_msg(GGZ_DBG_TABLE, "AI Players : %d (accept)", ai_total);
	else {
		dbg_msg(GGZ_DBG_TABLE, "AI Players : %d (invalid)", ai_total);
		status = E_BAD_OPTIONS;
	}

	dbg_msg(GGZ_DBG_TABLE, "Open_seats : %d", seats_open(table));
	dbg_msg(GGZ_DBG_TABLE, "Num_reserve: %d", seats_reserved(table));
	dbg_msg(GGZ_DBG_TABLE, "State      : %d", table.state);
	dbg_msg(GGZ_DBG_TABLE, "Control fd : %d", table.fd_to_game);
	for (i = 0; i < seat_total; i++)
		switch (table.seats[i]) {
		case GGZ_SEAT_OPEN:
			dbg_msg(GGZ_DBG_TABLE, "Seat[%d]: open", i);
			break;
		case GGZ_SEAT_BOT:
			dbg_msg(GGZ_DBG_TABLE, "Seat[%d]: computer", i);
			break;
		case GGZ_SEAT_RESV:
			dbg_msg(GGZ_DBG_TABLE, "Seat[%d]: reserved for %d", i,
				table.reserve[i]);
			break;
		default:
			dbg_msg(GGZ_DBG_TABLE,
				"Seat[%d]: player %d", i, table.seats[i]);
		}

	return status;
}


/*
 * launch_table accepts the index to a new game table a creates
 * a new dedicated handler thread.
 *
 * returns 0 if successful, or a non-zero error if the thread 
 * cannot be created
 *
 */
int table_handler_launch(int t_index)
{
	pthread_t thread;
	int *index_ptr;
	int status;
	
	/* Temporary storage to pass fd */
	if ( (index_ptr = malloc(sizeof(int))) == NULL)
		err_sys_exit("malloc error");
	*index_ptr = t_index;
	status = pthread_create(&thread, NULL, table_new, index_ptr);
	if (status != 0) {
		free(index_ptr);
	}

	return status;
}


/*
 * table_new accepts a pointer to the table index of a new table
 * It then waits for all seats at the table to be filled before
 * starting the game.  At the end of the game, it removes the table
 * 
 */
static void* table_new(void *index_ptr)
{
	int t_index, status;

	t_index = *((int *) index_ptr);
	free(index_ptr);

	/* Detach thread since no one needs to join us */
	if ((status = pthread_detach(pthread_self())) != 0) {
		errno = status;
		err_sys_exit("pthread_detach error");
	}

	dbg_msg(GGZ_DBG_PROCESS, "Table %d thread detached", t_index);
 
	table_fork(t_index);
	table_remove(t_index);
	return (NULL);
}


/*
 * table_fork is responsible for setting up a new game table process.
 * It forks the child to play the game, and kills the child when the
 * game is over.
 */
static void table_fork(int t_index)
{
	pid_t pid;
	char path[MAX_PATH_LEN];
	char game[MAX_GAME_NAME_LEN + 1];
	char fd_name[MAX_GAME_NAME_LEN + 12];
	int type_index, sock, fd;
	struct stat buf;
	
	/* Get path for game server */
	type_index = tables.info[t_index].type_index;

	pthread_rwlock_rdlock(&game_types.lock);
	strncpy(path, game_types.info[type_index].path, MAX_PATH_LEN);
	strncpy(game, game_types.info[type_index].name, MAX_GAME_NAME_LEN);
	pthread_rwlock_unlock(&game_types.lock);

	/* Fork table process */
	if ( (pid = fork()) < 0)
		err_sys_exit("fork failed");
	else if (pid == 0) {
		/* Make sure the parent's socket is created before we go on */
		sprintf(fd_name, "/tmp/%s.%d", game, getpid());
		while (stat(fd_name, &buf) < 0)
			sleep(1);

		table_run_game(t_index, path);
		/* table_run_game() should never return */
		err_sys_exit("exec failed");
	} else {
		/* Create Unix doamin socket for communication*/
		sprintf(fd_name, "/tmp/%s.%d", game, pid);
		sock = es_make_unix_socket(ES_SERVER, fd_name);
		/* FIXME: need to check validity of fd */
		
		if (listen(sock, 1) < 0) 
			err_sys_exit("listen failed");

		if ( (fd = accept(sock, NULL, NULL)) < 0)
			err_sys_exit("accept failed");
		
		pthread_rwlock_wrlock(&tables.lock);
		tables.info[t_index].pid = pid;
		tables.info[t_index].fd_to_game = fd;
		pthread_rwlock_unlock(&tables.lock);
		
		if (table_send_opt(t_index) == 0)
			table_loop(t_index);
		
		/* Make sure game server is dead */
		kill(pid, SIGINT);
		close(tables.info[t_index].fd_to_game);
		unlink(fd_name);
	}
}


static void table_run_game(int t_index, char *path)
{
	/* Comment out for now */
	/*dbg_msg(GGZ_DBG_PROCESS, "Process forked.  Game running on table %d", 
		t_index);*/

	/* FIXME: Close all other fd's and kill threads? */
	execv(path, NULL);
}


/* 
 * FIXME: Fix this function like this:
 * 1) acquire locks
 * 2) copy all necessary info to local vars
 * 3) release locks
 * 4) pass local copy of info to table
 */
static int table_send_opt(int t_index)
{
	int i, fd, uid;
	char status = 0;
	char name[MAX_USER_NAME_LEN];
	TableInfo table;

	pthread_rwlock_rdlock(&tables.lock);
	table = tables.info[t_index];
	pthread_rwlock_unlock(&tables.lock);

	fd = table.fd_to_game;
	
	/* Send number of seats */
	if (es_write_int(fd, REQ_GAME_LAUNCH) < 0
	    || es_write_int(fd, seats_num(table)) < 0)
		return -1;

	/* Send seat assignments, names, and fd's */
	for (i = 0; i < seats_num(table); i++) {

		if (es_write_int(fd, table.seats[i]) < 0)
			return -1;

		switch(tables.info[t_index].seats[i]) {
		case GGZ_SEAT_OPEN:
		case GGZ_SEAT_BOT:
			break;  /* no name for these */
		case GGZ_SEAT_RESV:
			uid = table.reserve[i];
			/* FIXME: Look up player name by uid */
			strcpy(name,"reserved");
			if (es_write_string(fd, name) < 0)
				return -1;
			break;
		}
	}

	return status;
}


static void table_loop(int t_index)
{
	int opcode, fd, status;
	fd_set active_fd_set, read_fd_set;
	struct timeval timer;
	char in_transit = 0;
	
	fd = tables.info[t_index].fd_to_game;
	FD_ZERO(&active_fd_set);
	FD_SET(fd, &active_fd_set);

	for (;;) {
		/* Check for player transits if not already in one */
		if (!in_transit)
			if ( (in_transit = transit_handle(t_index, fd)) < 0)
				break;
		
		read_fd_set = active_fd_set;
		
		/* Setup timeout for select*/
		timer.tv_sec = GGZ_RESYNC_SEC;
		timer.tv_usec = GGZ_RESYNC_USEC;
		
		status = select((fd + 1), &read_fd_set, NULL, NULL, &timer);
		
		if (status <= 0) {
			if (status == 0 || errno == EINTR)
				continue;
			else
				err_sys_exit("select error");
		}

		if (es_read_int(fd, &opcode) < 0)
			break;
		
		if (table_handle(opcode, t_index, fd, &in_transit) < 0)
			break;
	}
	
	/* Table is done.  Release transit lock if we still have it */
	if (in_transit == 1)
		pthread_mutex_unlock(&tables.info[t_index].transit_lock);
	
}


static int table_handle(int opcode, int index, int fd, char* transit)
{
	int status;
	TableToControl request = (TableToControl)opcode;

	switch (request) {
	case RSP_GAME_LAUNCH:
		status = table_game_launch(index, fd);
		break;
		
	case RSP_GAME_JOIN:
		status = table_game_join(index, fd, transit);
		break;

	case RSP_GAME_LEAVE:
		status = table_game_leave(index, fd, transit);
		break;
		
	case REQ_GAME_OVER:
		table_game_over(index, fd);
		status = -1;
		break;

	case MSG_LOG:
		status = table_log(index, fd, 0);
		break;

	case MSG_DBG:
		status = table_log(index, fd, 1);
		break;

	default:
		dbg_msg(GGZ_DBG_PROTOCOL, "Table %d sent unimplemented op %d",
			index, request);
		status = -1;
		break;
	}

	return status;
}


static int table_game_launch(int index, int fd)
{
	char status;
	
	if (tables.info[index].state != GGZ_TABLE_CREATED
	    || es_read_char(fd, &status) < 0
	    || status < 0)
		return -1;
	
	pthread_rwlock_wrlock(&tables.lock);
	tables.info[index].state = GGZ_TABLE_LAUNCHED;
	pthread_rwlock_unlock(&tables.lock);
	
	table_event_enqueue(index, GGZ_UPDATE_ADD);

	pthread_mutex_lock(&tables.info[index].state_lock);
	pthread_cond_signal(&tables.info[index].state_cond);
	pthread_mutex_unlock(&tables.info[index].state_lock);

	return 0;
}


/*
 * table_game_join handles the RSP_GAME_JOIN from the table
 * Must be called with transit locked
 * Returns with transit unlocked
 */
static int table_game_join(int index, int fd, char* transit)
{
	char status, flag;
	int p, seat;

	dbg_msg(GGZ_DBG_TABLE, "Table %d responded to join", index);

	/* Error: we didn't request a join! */
	if (!(*transit))
		return -1;

	/* Return error if not in process of joining */
	flag = tables.info[index].transit_flag;
	if (!(flag & GGZ_TRANSIT_JOIN) || !(flag & GGZ_TRANSIT_SENT))
		return -1;
	
	if (es_read_char(fd, &status) < 0)
		return -1;

	if (status < 0)
		tables.info[index].transit_flag |= GGZ_TRANSIT_ERR;
	else {
		/* Assign seat */
		p = tables.info[index].transit;
		seat = tables.info[index].transit_seat;
		dbg_msg(GGZ_DBG_TABLE, "Player %d at table %d, seat %d ", p,
			index, seat);
		
		pthread_rwlock_wrlock(&tables.lock);
		tables.info[index].seats[seat] = p;
		if (!seats_open(tables.info[index])) {
			dbg_msg(GGZ_DBG_TABLE, "Table %d full now", index);
			tables.info[index].state = GGZ_TABLE_PLAYING;
			pthread_rwlock_unlock(&tables.lock);
			table_transit_event_enqueue(index, GGZ_UPDATE_JOIN, p,
						    seat);
			table_event_enqueue(index, GGZ_UPDATE_STATE);
			
		} else {
			pthread_rwlock_unlock(&tables.lock);
			table_transit_event_enqueue(index, GGZ_UPDATE_JOIN, p,
						    seat);
		}
	}

	/* Mark transit as done and signal player*/
	tables.info[index].transit_flag &= ~GGZ_TRANSIT_JOIN;
	tables.info[index].transit_flag |= GGZ_TRANSIT_DONE;
	pthread_cond_broadcast(&tables.info[index].transit_cond);
	pthread_mutex_unlock(&tables.info[index].transit_lock);

	*transit = 0;
	return 0;
}


/*
 * table_game_join handles the RSP_GAME_LEAVE from the table
 * Must be called with transit locked
 * Returns with transit unlocked
 */
static int table_game_leave(int index, int fd, char* transit)
{
	char status, flag;
	int p, seat;
	int ret_val = 0;

	dbg_msg(GGZ_DBG_TABLE, "Table %d responded to leave", index);

	/* Error: we didn't request a leave! */
	if (!(*transit))
		return -1;

	/* Return error if not in process of leaving */
	flag = tables.info[index].transit_flag;
	if (!(flag & GGZ_TRANSIT_LEAVE) || !(flag & GGZ_TRANSIT_SENT))
		return -1;
	
	if (es_read_char(fd, &status) < 0)
		return -1;
	
	if (status < 0)
		tables.info[index].transit_flag |= GGZ_TRANSIT_ERR;
	else {
		/* Vacate seat */
		p = tables.info[index].transit;
		seat = tables.info[index].transit_seat;
		dbg_msg(GGZ_DBG_TABLE, "Player %d left table %d seat %d", p, 
			index, seat);

		pthread_rwlock_wrlock(&tables.lock);
		tables.info[index].seats[seat] = GGZ_SEAT_OPEN;
		if (!seats_human(tables.info[index])) {
			dbg_msg(GGZ_DBG_TABLE, "Table %d now empty: removing",
				index);
			ret_val = -1;
		}
		
		pthread_rwlock_unlock(&tables.lock);
		table_transit_event_enqueue(index, GGZ_UPDATE_LEAVE, p, seat);
	}

	/* Mark transit as done and signal player*/
	tables.info[index].transit_flag &= ~GGZ_TRANSIT_LEAVE;
	tables.info[index].transit_flag |= GGZ_TRANSIT_DONE;
	pthread_cond_broadcast(&tables.info[index].transit_cond);
	pthread_mutex_unlock(&tables.info[index].transit_lock);

	*transit = 0;
	return ret_val;
}


static int table_game_over(int index, int fd)
{
	int i, num, p_index, won, lost;

	dbg_msg(GGZ_DBG_TABLE,
		"Handling game-over request from table %d", index);
	
	/* Read number of statistics */
	if (es_read_int(fd, &num) < 0)
		return (-1);
	
	for (i = 0; i < num; i++) {
		if (es_read_int(fd, &p_index) < 0
		    || es_read_int(fd, &won) < 0 
		    || es_read_int(fd, &lost) < 0)
			return (-1);

		/* FIXME: Do something with these statistics */

	}

	/* Mark table as done, so people don't attempt transits */
	pthread_rwlock_wrlock(&tables.lock);
	tables.info[index].state = GGZ_TABLE_DONE;
	pthread_rwlock_unlock(&tables.lock);
	table_event_enqueue(index, GGZ_UPDATE_STATE);

	/* Send response back to game server, allowing termination */
	if (es_write_int(fd, RSP_GAME_OVER) < 0)
		return -1;

	return 0;
}


static int table_log(int index, int fd, char debug) 
{
	int level, type, len, pid;
	char name[MAX_GAME_NAME_LEN];
	char* msg;
	char* buf;

	if (es_read_int(fd, &level) < 0
	    || es_read_string_alloc(fd, &msg) < 0)
		return -1;
	
	if (log_info.options & GGZ_LOGOPT_INC_GAMETYPE) {
		pthread_rwlock_rdlock(&tables.lock);
		type = tables.info[index].type_index;
		pid = tables.info[index].pid;
		pthread_rwlock_unlock(&tables.lock);
		
		pthread_rwlock_rdlock(&game_types.lock);
		strcpy(name, game_types.info[type].name);
		pthread_rwlock_unlock(&game_types.lock);
		
		len = strlen(msg) + strlen(name) + 5;
		
		if (debug || (log_info.options & GGZ_LOGOPT_INC_PID))
			len += 6;
		
		if ( (buf = malloc(len)) == NULL)
			err_sys_exit("malloc failed");

		memset(buf, 0, len);

		if (debug || (log_info.options & GGZ_LOGOPT_INC_PID))
			snprintf(buf, (len - 1), "(%s:%d) ", name, pid);
		else
			snprintf(buf, (len - 1), "(%s) ", name);
		
		snprintf((buf + strlen(buf)), (len - strlen(buf) - 1), msg);
		free(msg);
		msg = buf;
	}
	
	if (debug) 
		dbg_msg(level, msg);
	else
		log_msg(level, msg);

	free(msg);
	
	return 0;
}


static void table_remove(int t_index)
{
	int room, count, last, i, p;

	dbg_msg(GGZ_DBG_TABLE, "Removing table %d", t_index);

	/* First get it off the list in rooms */
	room = tables.info[t_index].room;
	pthread_rwlock_wrlock(&rooms[room].lock);
	count = --rooms[room].table_count;
	last = rooms[room].table_index[count];
	for (i = 0; i <= count; i++)
		if (rooms[room].table_index[i] == t_index) {
			rooms[room].table_index[i] = last;
			break;
		}
	dbg_msg(GGZ_DBG_ROOM, "Room %d table count = %d", room, count);
	pthread_rwlock_unlock(&rooms[room].lock);

	pthread_rwlock_wrlock(&tables.lock);
	tables.info[t_index].type_index = -1;
	tables.info[t_index].state = GGZ_TABLE_ERROR;
	tables.count--;
	pthread_rwlock_unlock(&tables.lock);

	/* Send out table-leave messages for remaining players */
	for (i = 0; i < seats_num(tables.info[t_index]); i++) {
		p = tables.info[t_index].seats[i];
		switch (p) {
		case GGZ_SEAT_OPEN:
		case GGZ_SEAT_BOT:
		case GGZ_SEAT_RESV:
			continue;
		}
		
		table_transit_event_enqueue(t_index, GGZ_UPDATE_LEAVE, p, i);
	}

	table_event_enqueue(t_index, GGZ_UPDATE_DELETE);

	/* Signal anyone waiting for this table */
	/* FIXME: do we need to do this? */
	pthread_mutex_lock(&tables.info[t_index].state_lock);
	pthread_cond_signal(&tables.info[t_index].state_cond);
	pthread_mutex_unlock(&tables.info[t_index].state_lock);

	/* Signal anyone waiting for transits */
	pthread_mutex_lock(&tables.info[t_index].transit_lock);
	tables.info[t_index].transit_flag |= GGZ_TRANSIT_ERR;
	tables.info[t_index].transit_flag |= GGZ_TRANSIT_CLR;
	tables.info[t_index].transit_flag &= ~GGZ_TRANSIT_JOIN;
	tables.info[t_index].transit_flag &= ~GGZ_TRANSIT_LEAVE;
	pthread_cond_broadcast(&tables.info[t_index].transit_cond);
	pthread_mutex_unlock(&tables.info[t_index].transit_lock);
}


int table_launch(int p, TableInfo table, int* t_index)
{
	int i, index;
	char state;
	
	/* Check validity of table info */
	if (table_check(p, table) < 0)
		return E_BAD_OPTIONS;
		
	/* Find open table */
	pthread_rwlock_wrlock(&tables.lock);
	if (tables.count == MAX_TABLES) {
		pthread_rwlock_unlock(&tables.lock);
		return E_ROOM_FULL;
	}

	for (i = 0; i < MAX_TABLES; i++)
		if (tables.info[i].type_index < 0)
			break;
		
	index = i;
	tables.info[index] = table;
	tables.count++;
	pthread_rwlock_unlock(&tables.lock);

	/* Attempt to do launch of table-controller */
	if (table_handler_launch(index) != 0) {
		table_remove(index);
		return E_LAUNCH_FAIL;
	}

	/* Wait for successful launch */
	pthread_mutex_lock(&tables.info[index].state_lock);
	while ( (state = tables.info[index].state) == GGZ_TABLE_CREATED) {
		dbg_msg(GGZ_DBG_TABLE, "Player %d waiting for table %d launch",
			p, index);
		pthread_cond_wait(&tables.info[index].state_cond,
				  &tables.info[index].state_lock);
	}
	pthread_mutex_unlock(&tables.info[index].state_lock);

	if (state == GGZ_TABLE_ERROR)
		return E_LAUNCH_FAIL;

	*t_index = index;
	return 0;
}


int table_join(int p, int index, int* t_fd)
{
	int flag, fd;

	pthread_rwlock_rdlock(&tables.lock);
	if (tables.info[index].type_index == -1) {
		pthread_rwlock_unlock(&tables.lock);
		return E_TABLE_EMPTY;
	}

	if (!seats_open(tables.info[index])) {
		pthread_rwlock_unlock(&tables.lock);
		return E_TABLE_FULL;
	}
	pthread_rwlock_unlock(&tables.lock);
	
	/* Wait for "all-clear" to transit */
	pthread_mutex_lock(&tables.info[index].transit_lock);
	flag = tables.info[index].transit_flag;
	while (!(flag & GGZ_TRANSIT_CLR)) {
		dbg_msg(GGZ_DBG_TABLE,
			"Player %d waiting for transit to table %d", p, index);
		pthread_cond_wait(&tables.info[index].transit_cond,
				  &tables.info[index].transit_lock);
		flag = tables.info[index].transit_flag;
	}

	/* If transit is all clear but error occurred, table is gone */
	if ((flag & GGZ_TRANSIT_STATUS) != GGZ_TRANSIT_OK) {
		pthread_mutex_unlock(&tables.info[index].transit_lock);
		return E_NO_TABLE;
	}

	tables.info[index].transit = p;
	tables.info[index].transit_flag = GGZ_TRANSIT_JOIN;
	pthread_cond_broadcast(&tables.info[index].transit_cond);
	
	/* Wait for join to occur */
	flag = tables.info[index].transit_flag;
	while (flag & GGZ_TRANSIT_JOIN) {
		dbg_msg(GGZ_DBG_TABLE, "Player %d waiting for table %d join",
			p, index);
		pthread_cond_wait(&tables.info[index].transit_cond,
				  &tables.info[index].transit_lock);
		flag = tables.info[index].transit_flag;
	}

	fd = tables.info[index].transit_fd;

	/* Clear transit flag and notify anyone else waiting */
	tables.info[index].transit_flag = GGZ_TRANSIT_CLR;
	pthread_cond_broadcast(&tables.info[index].transit_cond);
	pthread_mutex_unlock(&tables.info[index].transit_lock);
	
	if (!(flag & GGZ_TRANSIT_DONE))
		return E_JOIN_FAIL;

	*t_fd = fd;
 	return 0;
}


int table_leave(int p, int index)
{
	int flag, type;
	char state;

	if (index == -1)
		return E_NO_TABLE;

	pthread_rwlock_rdlock(&tables.lock);
	type = tables.info[index].type_index;
	state = tables.info[index].state;
	pthread_rwlock_unlock(&tables.lock);

	/* Make sure table is valid */
	if (type < 0 || state == GGZ_TABLE_DONE) {
		dbg_msg(GGZ_DBG_TABLE, "Player %d leaving removed table", p);
		return E_NO_TABLE;
	}
	
      	/* Only allow leave during gameplay if game type supports it */
	pthread_rwlock_rdlock(&game_types.lock);
	if (state == GGZ_TABLE_PLAYING && !game_types.info[type].allow_leave) {
		pthread_rwlock_unlock(&game_types.lock);
		return E_LEAVE_FORBIDDEN;
	}
	pthread_rwlock_unlock(&game_types.lock);
	
	/* Wait for "all-clear" to transit */
	pthread_mutex_lock(&tables.info[index].transit_lock);
	flag = tables.info[index].transit_flag;
	while (!(flag &GGZ_TRANSIT_CLR)) {
		dbg_msg(GGZ_DBG_TABLE,
			"Player %d waiting for transit from table %d", p,
			index);
		pthread_cond_wait(&tables.info[index].transit_cond,
				  &tables.info[index].transit_lock);
	}

	/* If transit is all clear but error occurred, table is gone */
	if ((flag & GGZ_TRANSIT_STATUS) != GGZ_TRANSIT_OK) {
		pthread_mutex_unlock(&tables.info[index].transit_lock);
		return E_NO_TABLE;
	}

	tables.info[index].transit = p;
	tables.info[index].transit_flag = GGZ_TRANSIT_LEAVE;
	pthread_cond_broadcast(&tables.info[index].transit_cond);
	
	/* Wait for leave to occur */
	flag = tables.info[index].transit_flag;
	while (flag & GGZ_TRANSIT_LEAVE) {
		dbg_msg(GGZ_DBG_TABLE, "Player %d waiting for table %d leave",
			p, index);
		pthread_cond_wait(&tables.info[index].transit_cond,
				  &tables.info[index].transit_lock);
		flag = tables.info[index].transit_flag;
	}

	/* Clear transit flag and notify anyone else waiting */
	tables.info[index].transit_flag = GGZ_TRANSIT_CLR;
	pthread_cond_broadcast(&tables.info[index].transit_cond);
	pthread_mutex_unlock(&tables.info[index].transit_lock);
	
	if (!(flag & GGZ_TRANSIT_DONE))
		return E_LEAVE_FAIL;
	
 	return 0;
}


static int table_event_enqueue(unsigned int table, unsigned char opcode)
{
	void* data = NULL;
	int size, status, room;
	unsigned char state;

	room = tables.info[table].room;
	
	/* Pack up table update data */
	if (opcode == GGZ_UPDATE_STATE) {
		pthread_rwlock_rdlock(&tables.lock);
		state = tables.info[table].state;
		pthread_rwlock_unlock(&tables.lock);
		size = table_state_pack(&data, opcode, table, state);
	}
	else
		size = table_pack(&data, opcode, table);

	/* Queue table event for whole room */
	status = event_room_enqueue(room, table_event_callback, size, data);
	
	return status;
}


static int table_transit_event_enqueue(unsigned int table, 
				       unsigned char opcode,
				       unsigned int player,
				       unsigned int seat)
{
	void* data = NULL;
	int size, status, room;

	room = tables.info[table].room;
	
	/* Pack up table update data */
	size = table_transit_pack(&data, opcode, table, player, seat);

	/* Queue table event for whole room */
	status = event_room_enqueue(room, table_event_callback, size, data);
	
	return status;
}


static int table_pack(void** data, unsigned char opcode, unsigned int table)
{
	int size;
	char* current;

	size = sizeof(int) + sizeof(char);
	
	if ( (*data = malloc(size)) == NULL)
		err_sys_exit("malloc failed in table_pack");
	
	current = (char*)*data;
	
	*(unsigned char*)current = opcode;
	current += sizeof(char);

	*(unsigned int*)current = table;

	return size;
}


static int table_state_pack(void** data, unsigned char opcode, 
			    unsigned int table, unsigned char state)
{
	int size;
	char* current;

	size = 2 * sizeof(char) + sizeof(int);
	
	if ( (*data = malloc(size)) == NULL)
		err_sys_exit("malloc failed in table_transit_pack");
	
	current = (char*)*data;
	
	*(unsigned char*)current = opcode;
	current += sizeof(char);

	*(unsigned int*)current = table;
	current += sizeof(int);

	*(unsigned char*)current = state;
	current += sizeof(char);

	return size;
}


static int table_transit_pack(void** data, unsigned char opcode, 
			      unsigned int table, unsigned int player,
			      unsigned int seat)
{
	int size;
	char* current;

	size = sizeof(char) + 3 * sizeof(int);
	
	if ( (*data = malloc(size)) == NULL)
		err_sys_exit("malloc failed in table_transit_pack");
	
	current = (char*)*data;
	
	*(unsigned char*)current = opcode;
	current += sizeof(char);

	*(unsigned int*)current = table;
	current += sizeof(int);

	*(unsigned int*)current = player;
	current += sizeof(int);

	*(unsigned int*)current = seat;
	current += sizeof(int);

	

	return size;
}


/* Event callback for delivering table list update to a player */
static int table_event_callback(int p_index, int size, void* data)
{
	unsigned char opcode, state;
	char player[MAX_USER_NAME_LEN + 1];
	int table, fd, seat, p, i;
	char* current;
	TableInfo info;

	fd = players.info[p_index].fd;

	/* Unpack event data */
	current = (char*)data;

	opcode = *(unsigned char*)current;
	current += sizeof(char);

	table = *(int*)current;
	current += sizeof(int);

	/* Always send opcode and table index */
	if (es_write_int(fd, MSG_UPDATE_TABLES) < 0 
	    || es_write_char(fd, opcode) < 0
	    || es_write_int(fd, table) < 0)
		return -1;
	
	pthread_rwlock_rdlock(&tables.lock);
	info = tables.info[table];
	pthread_rwlock_unlock(&tables.lock);	

	switch (opcode) {
	case GGZ_UPDATE_DELETE:
		dbg_msg(GGZ_DBG_UPDATE, "Player %d sees table %d deleted",
			p_index, table);
		break;

	case GGZ_UPDATE_ADD:
		dbg_msg(GGZ_DBG_UPDATE, "Player %d sees table %d added",
			p_index, table);
		if (es_write_int(fd, info.room) < 0
		    || es_write_int(fd, info.type_index) < 0
		    || es_write_string(fd, info.desc) < 0
		    || es_write_char(fd, info.state) < 0
		    || es_write_int(fd, seats_num(info)) < 0)
			return -1;
		
		/* Now send seat assignments */
		for (i = 0; i < seats_num(info); i++) {
			if (es_write_int(fd, info.seats[i]) < 0)
				return -1;

			switch(info.seats[i]) {
			case GGZ_SEAT_OPEN:
			case GGZ_SEAT_BOT:
				continue;  /* no name for these */
			case GGZ_SEAT_RESV:
				/* Look up player name by uid */
				strcpy(player, "reserved");
				break;
			default: /* must be a player index */
				p = info.seats[i];
				/* FIXME: Race condition */
				pthread_rwlock_rdlock(&players.info[p].lock);
				strcpy(player, players.info[p].name);
				pthread_rwlock_unlock(&players.info[p].lock);
			}

			if (es_write_string(fd, player) < 0)
				return -1;
		}
		break;
		
	case GGZ_UPDATE_LEAVE:
	case GGZ_UPDATE_JOIN:
		p = *(int*)current;
		current += sizeof(int);
		seat = *(int*)current;

		pthread_rwlock_rdlock(&players.info[p].lock);
		strcpy(player, players.info[p].name);
		pthread_rwlock_unlock(&players.info[p].lock);
		dbg_msg(GGZ_DBG_UPDATE, 
			"Player %d sees player %d %s seat %d at table %d", 
			p_index, p, 
			(opcode == GGZ_UPDATE_JOIN ? "join" : "leave"),
			seat, table);
		if (es_write_int(fd, seat) < 0
		    || es_write_string(fd, player) < 0)
			return -1;
		break;
	case GGZ_UPDATE_STATE:
		state = *(unsigned char*)current;
		current += sizeof(char);

		dbg_msg(GGZ_DBG_UPDATE, "Player %d sees table %d new state %d",
			p_index, table, state);
		if (es_write_char(fd, state) < 0)
			return -1;
		break;
	}
	
	return 0;
}

