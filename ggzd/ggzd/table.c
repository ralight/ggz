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

#define NG_RESYNC_SEC  5

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
static int   table_handle(int op, int index, int fd);
static void  table_remove(int t_index);
static void  table_run_game(int t_index, char *path);
static int   table_send_opt(int t_index);
static int   table_game_over(int index, int fd);
static int   table_handle_transits(int index, int fd);
static int   table_handle_join(int index, int fd);
static int   table_handle_leave(int index, int fd);

/* FIXME: This should actually do checking */
int table_check(int p_index, TableInfo table)
{

	int i;

	dbg_msg(GGZ_DBG_TABLE, "Player %d launching table of type %d", p_index,
		table.type_index);
	dbg_msg(GGZ_DBG_TABLE, "Num_seats  : %d", seats_num(table));
	dbg_msg(GGZ_DBG_TABLE, "AI Players : %d", seats_comp(table));
	dbg_msg(GGZ_DBG_TABLE, "Open_seats : %d", seats_open(table));
	dbg_msg(GGZ_DBG_TABLE, "Num_reserve: %d", seats_reserved(table));
	dbg_msg(GGZ_DBG_TABLE, "State    : %d", table.state);
	dbg_msg(GGZ_DBG_TABLE, "Control fd : %d", table.fd_to_game);
	for (i = 0; i < seats_num(table); i++)
		switch (table.seats[i]) {
		case GGZ_SEAT_OPEN:
			dbg_msg(GGZ_DBG_TABLE, "Seat[%d]: open", i);
			break;
		case GGZ_SEAT_COMP:
			dbg_msg(GGZ_DBG_TABLE, "Seat[%d]: computer", i);
			break;
		case GGZ_SEAT_RESV:
			dbg_msg(GGZ_DBG_TABLE, "Seat[%d]: reserved for %d", i,
				table.reserve[i]);
			break;
		default:
			dbg_msg(GGZ_DBG_TABLE,
				"Seat[%d]: player %d, fd of %d", i,
				table.seats[i], table.player_fd[i]);
		}

	return 0;
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
	int type_index, i, sock, fd;
	
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
		table_run_game(t_index, path);
		/* table_run_game() should never return */
		err_sys_exit("exec failed");
	} else {
		/* Create Unix doamin socket for communication*/
		sprintf(fd_name, "/tmp/%s.%d", game, pid);
		sock = es_make_unix_socket(ES_SERVER, fd_name);
		/* FIXME: need to check validity of fd */
		
		if (listen(sock, 1) < 0) 
			err_sys_exit("listen falied");

		if ( (fd = accept(sock, NULL, NULL)) < 0)
			err_sys_exit("accpet failed");
		
		pthread_rwlock_wrlock(&tables.lock);
		tables.info[t_index].pid = pid;
		tables.info[t_index].fd_to_game = fd;
		pthread_rwlock_unlock(&tables.lock);
		
		/* Close the remote ends of the socket pairs */
		pthread_rwlock_rdlock(&tables.lock);
		for (i = 0; i < seats_num(tables.info[t_index]); i++)
			if (tables.info[t_index].seats[i] >= 0)
				close(tables.info[t_index].player_fd[i]);
		pthread_rwlock_unlock(&tables.lock);

		if (table_send_opt(t_index) == 0) {
			pthread_rwlock_wrlock(&tables.lock);
			tables.info[t_index].state = GGZ_TABLE_LAUNCH;
			pthread_rwlock_unlock(&tables.lock);
			
			pthread_mutex_lock(&tables.info[t_index].state_lock);
			pthread_cond_signal(&tables.info[t_index].state_cond);
			pthread_mutex_unlock(&tables.info[t_index].state_lock);
			
			table_loop(t_index);
		}
		
		/* Make sure game server is dead */
		kill(pid, SIGINT);
		close(tables.info[t_index].fd_to_game);
		unlink(fd_name);
	}
}


static void table_run_game(int t_index, char *path)
{
	dbg_msg(GGZ_DBG_PROCESS, "Process forked.  Game running on table %d", 
		t_index);

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
	int i, fd, size, type, op, uid, index;
	char status = 0;
	char name[MAX_USER_NAME_LEN];

	fd = tables.info[t_index].fd_to_game;
	type = tables.info[t_index].type_index;

	pthread_rwlock_rdlock(&game_types.lock);
	size = game_types.info[type].options_size;
	pthread_rwlock_unlock(&game_types.lock);

	/* Pass options struct and other seat info */
	if (es_write_int(fd, REQ_GAME_LAUNCH) < 0
	    || es_writen(fd, tables.info[t_index].options, size) < 0
	    || es_write_int(fd, seats_num(tables.info[t_index])) < 0)
		return (-1);

	/* Send seat assignments, names, and fd's */
	for (i = 0; i < seats_num(tables.info[t_index]); i++) {

		if (es_write_int(fd, tables.info[t_index].seats[i]) < 0)
			return -1;

		switch(tables.info[t_index].seats[i]) {

		case GGZ_SEAT_OPEN:
		case GGZ_SEAT_COMP:
			break;  /* no name for these */
		case GGZ_SEAT_RESV:
			uid = tables.info[t_index].reserve[i];
				/* Look up player name by uid */
			strcpy(name,"reserved");
			if (es_write_string(fd, name) < 0)
				return (-1);
			break;
		default: /* must be a player index */
			index = tables.info[t_index].seats[i];
			pthread_rwlock_rdlock(&players.lock);
			strcpy(name, players.info[index].name);
			pthread_rwlock_unlock(&players.lock);
			if (es_write_string(fd, name) < 0
			    || es_write_int(fd, tables.info[t_index].player_fd[i]) < 0)
				return (-1);
			
		}
	}

	/* Make sure game server says everything is OK */
	if (es_read_int(fd, &op) < 0 
	    || es_read_char(fd, &status) < 0)
		return (-1);

	if (op != RSP_GAME_LAUNCH)
		status = -1;

	return status;

}


static void table_loop(int t_index)
{
	int request, fd, status;
	fd_set active_fd_set, read_fd_set;
	struct timeval timer;

	fd = tables.info[t_index].fd_to_game;
	FD_ZERO(&active_fd_set);
	FD_SET(fd, &active_fd_set);

	for (;;) {
		/* Check for player transits */
		if (table_handle_transits(t_index, fd) < 0)
			break;
			
		read_fd_set = active_fd_set;
		
		/* Setup timeout for select*/
		timer.tv_sec = NG_RESYNC_SEC;
		timer.tv_usec = 0;
		
		status = select((fd + 1), &read_fd_set, NULL, NULL, &timer);
		
		if (status <= 0) {
			if (status == 0 || errno == EINTR)
				continue;
			else
				err_sys_exit("select error");
		}

		if (es_read_int(fd, &request) < 0)
			break;
		
		if (table_handle(request, t_index, fd) < 0)
			break;
	}
}


static int table_handle(int request, int index, int fd)
{

	int status;
	TableToControl op = (TableToControl) request;

	switch (op) {

	case MSG_GAME_OVER:
		table_game_over(index, fd);
		status = -1;
		break;

	default:
		dbg_msg(GGZ_DBG_PROTOCOL,
			"Table %d requested unimplemented op %d", index,
			op);
		status = -1;
	}

	return status;

}


static void table_remove(int t_index)
{
	dbg_msg(GGZ_DBG_TABLE, "Removing table %d", t_index);

	pthread_rwlock_wrlock(&tables.lock);
	tables.info[t_index].type_index = -1;
	free(tables.info[t_index].options);
	tables.info[t_index].options = NULL;
	tables.info[t_index].state = GGZ_TABLE_ERROR;
	tables.count--;
	tables.timestamp = time(NULL);
	pthread_rwlock_unlock(&tables.lock);

	/* FIXME: do we want to do this here? */
	pthread_mutex_lock(&tables.info[t_index].state_lock);
	pthread_cond_signal(&tables.info[t_index].state_cond);
	pthread_mutex_unlock(&tables.info[t_index].state_lock);
}


static int table_game_over(int index, int fd)
{

	int i, num, p_index, won, lost;

	dbg_msg(GGZ_DBG_TABLE,
		"Handling game-over message from table %d", index);

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

	return 0;

}

/* FIXME: Don't free options if options == NULL */
int table_launch(int p, TableInfo table, int* t_index)
{
	int i, index;
	char state;
	
	/* Check validity of table info (not options) */
	if (table_check(p, table) < 0) {
		free(table.options);
		return E_BAD_OPTIONS;
	}
		
	/* Find open table */
	pthread_rwlock_wrlock(&tables.lock);
	if (tables.count == MAX_TABLES) {
		pthread_rwlock_unlock(&tables.lock);
		free(table.options);
		return E_ROOM_FULL;
	}

	for (i = 0; i < MAX_TABLES; i++)
		if (tables.info[i].type_index < 0)
			break;
		
	index = i;
	tables.info[index] = table;
	tables.count++;
	tables.timestamp = time(NULL);
	pthread_rwlock_unlock(&tables.lock);

	/* Attempt to do launch of table-controller */
	if (table_handler_launch(index) != 0) {
		table_remove(index);
		return E_LAUNCH_FAIL;
	}

	/* Wait for successful launch */
	pthread_mutex_lock(&tables.info[index].state_lock);
	while ( (state = tables.info[index].state) == GGZ_TABLE_CREATE) {
		dbg_msg(GGZ_DBG_TABLE, "Waiting for table %d launch", index);
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
	int seats, flag, fd;
	
	pthread_rwlock_wrlock(&tables.lock);
	if (tables.info[index].type_index == -1) {
		pthread_rwlock_unlock(&tables.lock);
		return E_TABLE_EMPTY;
	}

	seats = seats_open(tables.info[index]);
	flag = tables.info[index].transit_flag;
	if (seats == 0 || (seats == 1 && flag == GGZ_TRANSIT_JOIN)) {
		pthread_rwlock_unlock(&tables.lock);
		return E_TABLE_FULL;
	}
	pthread_rwlock_unlock(&tables.lock);
	
	pthread_mutex_lock(&tables.info[index].transit_lock);
	while (tables.info[index].transit_flag != GGZ_TRANSIT_CLEAR) {
		dbg_msg(GGZ_DBG_TABLE, "Waiting for transit to table %d", 
			index);
		pthread_cond_wait(&tables.info[index].transit_cond,
				  &tables.info[index].transit_lock);
	}

	tables.info[index].transit_flag = GGZ_TRANSIT_JOIN;
	tables.info[index].transit = p;
	pthread_cond_broadcast(&tables.info[index].transit_cond);
	
	/* Wait for join */
	while ( (flag = tables.info[index].transit_flag) == GGZ_TRANSIT_JOIN) {
		dbg_msg(GGZ_DBG_TABLE, "Waiting for table %d join", index);
		pthread_cond_wait(&tables.info[index].transit_cond,
				  &tables.info[index].transit_lock);
	}

	fd = tables.info[index].transit_fd;
	tables.info[index].transit_flag = GGZ_TRANSIT_CLEAR;
	pthread_cond_broadcast(&tables.info[index].transit_cond);
	pthread_mutex_unlock(&tables.info[index].transit_lock);
	
	if (flag == GGZ_TRANSIT_ERROR)
		return E_JOIN_FAIL;

	*t_fd = fd;
 	return 0;
}


static int table_handle_transits(int index, int fd)
{
	int flag, status;
	
	pthread_mutex_lock(&tables.info[index].transit_lock);
	flag = tables.info[index].transit_flag;
	
	if (flag == GGZ_TRANSIT_JOIN)
		status = table_handle_join(index, fd);
	else if (flag == GGZ_TRANSIT_LEAVE)
		status = table_handle_leave(index, fd);
	else {
		pthread_mutex_unlock(&tables.info[index].transit_lock);
		return 0;
	}

	tables.info[index].transit_flag = status;
	pthread_cond_broadcast(&tables.info[index].transit_cond);
	pthread_mutex_unlock(&tables.info[index].transit_lock);

	if (status == GGZ_TRANSIT_ERROR)
		return -1;

	return 0;
}


static int table_handle_join(int index, int t_fd)
{
	int i, p, fd[2], op;
	char status = 0;
	char name[MAX_USER_NAME_LEN + 1];
	
	p = tables.info[index].transit;

	pthread_rwlock_wrlock(&tables.lock);
	/* Assign seat */
	for (i = 0; i < seats_num(tables.info[index]); i++)
		if (tables.info[index].seats[i] == GGZ_SEAT_OPEN) {
			tables.info[index].seats[i] = p;
				/* Create socketpair for comm*/
			socketpair(PF_UNIX, SOCK_STREAM, 0, fd);
			tables.info[index].player_fd[i] = fd[0];
			tables.info[index].transit_fd = fd[1];
			break;
		}
	
	tables.timestamp = time(NULL);
	pthread_rwlock_unlock(&tables.lock);

	pthread_rwlock_rdlock(&players.lock);
	strcpy(name, players.info[p].name);
	pthread_rwlock_unlock(&players.lock);

	/* Send MSG_TABLE_JOIN to table */
	if (es_write_int(t_fd, REQ_GAME_JOIN) < 0
	    || es_write_int(t_fd, i) < 0
	    || es_write_string(t_fd, name) < 0
	    || es_write_fd(t_fd, fd[0]) < 0)
		return GGZ_TRANSIT_ERROR;

	if (es_read_int(t_fd, &op) < 0
	    || es_read_char(t_fd, &status) < 0)
		return GGZ_TRANSIT_ERROR;
		
	/* FIXME: Check validity */

	dbg_msg(GGZ_DBG_TABLE, "Player %d in seat %d at table %d", p, i, index);
	return GGZ_TRANSIT_OK;
}

	
static int table_handle_leave(int index, int t_fd)
{
	/* FIXME: Write GGZ_TRANSIT_LEAVE code */
	return GGZ_TRANSIT_ERROR;

}
