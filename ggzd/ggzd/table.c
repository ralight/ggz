/*
 * File: table.c
 * Author: Brent Hendricks
 * Project: NetGames
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
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <datatypes.h>
#include <protocols.h>
#include <err_func.h>
#include <table.h>
#include <players.h>

/* Server wide data structures*/
extern struct GameTypes game_types;
extern struct GameTables game_tables;
extern struct Users players;

/* Local functions for handling tables */
static void* table_new(void *index_ptr);
static void  table_fork(int t_index);
static void  table_loop(int t_index);
static int   table_handle(int op, int index, int fd);
static void  table_remove(int t_index);
static void run_game(int t_index, int fd, char *path);
static int pass_options(int t_index);
static int game_over(int index, int fd);

/* FIXME: This should actually do checking */
int table_check(int p_index, TableInfo table)
{

	int i;

	dbg_msg("Player %d launching table of type %d", p_index,
		table.type_index);
	dbg_msg("Num_seats  : %d", table.num_seats);
	dbg_msg("AI Players : %d", table.comp_players);
	dbg_msg("Open_seats : %d", table.open_seats);
	dbg_msg("Num_reserve: %d", table.num_reserves);
	dbg_msg("Playing    : %d", table.playing);
	dbg_msg("Control fd : %d", table.fd_to_game);
	for (i = 0; i < table.num_seats; i++)
		dbg_msg("Players[i]: %d, fd of %d", table.players[i],
			table.player_fd[i]);
	for (i = 0; i < table.num_reserves; i++)
		dbg_msg("Reserve[i]: %d", table.reserve[i]);

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
	if (FAIL(index_ptr = malloc(sizeof(int))))
		err_sys_exit("malloc error");
	*index_ptr = t_index;
	status = pthread_create(&thread, NULL, table_new, index_ptr);
	if (status != 0) {
		free(index_ptr);
	}

	return status;

}


/*
 * new_player accepts a pointer to the table index of a new table
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

	dbg_msg("Table %d thread detached", t_index);

	/* Wait for enough players to join */
	pthread_mutex_lock(&game_tables.info[t_index].seats_lock);
	while (game_tables.info[t_index].open_seats > 0) {
		dbg_msg("Table %d waiting for seats to fill", t_index);
		pthread_cond_wait(&game_tables.info[t_index].seats_cond,
				  &game_tables.info[t_index].seats_lock);
	}
	pthread_mutex_unlock(&game_tables.info[t_index].seats_lock);

	table_fork(t_index);

	table_remove(t_index);
	return (NULL);
}


/*
 * do_table is responsible for setting up a new game table process.
 * It forks the child to play the game, and kills the child when the
 * game is over.
 */
static void table_fork(int t_index)
{

	pid_t pid;
	char path[MAX_PATH_LEN];
	int type_index, i, fd[2];

	/* Create socketpair for communication */
	socketpair(PF_UNIX, SOCK_STREAM, 0, fd);
	pthread_rwlock_wrlock(&game_tables.lock);
	game_tables.info[t_index].fd_to_game = fd[1];
	game_tables.info[t_index].playing = 1;
	pthread_rwlock_unlock(&game_tables.lock);

	/* Get path for game server */
	type_index = game_tables.info[t_index].type_index;
	pthread_rwlock_rdlock(&game_types.lock);
	strncpy(path, game_types.info[type_index].path, MAX_PATH_LEN);
	pthread_rwlock_unlock(&game_types.lock);

#ifdef DEBUG
	table_check(-1, game_tables.info[t_index]);
#endif

	/* Fork table process */
	if ((pid = fork()) < 0)
		err_sys_exit("fork failed");
	else if (pid == 0) {
		run_game(t_index, fd[0], path);
		err_sys_exit("exec failed");
	} else {
		/* Close the remote ends of the socket pairs */
		close(fd[0]);
		for (i = 0; i < game_tables.info[t_index].num_humans; i++)
			close(game_tables.info[t_index].player_fd[i]);

		pthread_rwlock_wrlock(&game_tables.lock);
		game_tables.info[t_index].pid = pid;
		pthread_rwlock_unlock(&game_tables.lock);

		if (pass_options(t_index) == 0)
			table_loop(t_index);

		/* Make sure game server is dead */
		kill(pid, SIGINT);
	}
}


static void run_game(int t_index, int fd, char *path)
{

	dbg_msg("Process forked.  Game running");

	/* FIXME: Close all other fd's and kill threads? */
	dup2(fd, STDIN_FILENO);
	dup2(fd, STDOUT_FILENO);
	close(fd);

	execv(path, NULL);

}


static int pass_options(int t_index)
{

	int i, fd, size, type, p_index, op;
	char status = 0;
	char name[MAX_USER_NAME_LEN];

	fd = game_tables.info[t_index].fd_to_game;
	type = game_tables.info[t_index].type_index;

	pthread_rwlock_rdlock(&game_types.lock);
	size = game_types.info[type].options_size;
	pthread_rwlock_unlock(&game_types.lock);

	/* Pass options struct and other seat info */
	if (FAIL(es_write_int(fd, REQ_GAME_LAUNCH)) ||
	    FAIL(es_writen(fd, game_tables.info[t_index].options, size)) ||
	    FAIL(es_write_int(fd, game_tables.info[t_index].num_seats)) ||
	    FAIL(es_write_char
		 (fd,
		  game_tables.info[t_index].comp_players))) return (-1);

	/* Send player names and file descriptors */
	for (i = 0; i < game_tables.info[t_index].num_humans; i++) {
		p_index = game_tables.info[t_index].players[i];
		pthread_rwlock_rdlock(&players.lock);
		strncpy(name, players.info[p_index].name,
			MAX_USER_NAME_LEN);
		pthread_rwlock_unlock(&players.lock);
		if (FAIL(es_write_string(fd, name)) ||
		    FAIL(es_write_int
			 (fd, game_tables.info[t_index].player_fd[i])))
			return (-1);
	}

	/* Make sure game server says everything is OK */
	if (FAIL(es_read_int(fd, &op)) || FAIL(es_read_char(fd, &status)))
		return (-1);

	if (op != RSP_GAME_LAUNCH)
		status = -1;

	return status;

}


static void table_loop(int t_index)
{

	int request, fd, status;

	fd = game_tables.info[t_index].fd_to_game;

	for (;;) {
		if (FAIL(status = es_read_int(fd, (int *) &request)))
			break;

		if (FAIL(status = table_handle(request, t_index, fd)))
			break;
	}
}


static int table_handle(int request, int index, int fd)
{

	int status;
	TableToControl op = (TableToControl) request;

	switch (op) {

	case MSG_GAME_OVER:
		game_over(index, fd);
		status = -1;
		break;

	default:
		dbg_msg("Table %d requested unimplemented op %d", index,
			op);
		status = -1;
	}

	return status;

}


static void table_remove(int t_index)
{

	dbg_msg("Removing table %d", t_index);

	pthread_rwlock_wrlock(&game_tables.lock);
	game_tables.info[t_index].type_index = -1;
	free(game_tables.info[t_index].options);
	game_tables.info[t_index].options = NULL;
	game_tables.count--;
	pthread_rwlock_unlock(&game_tables.lock);

	close(game_tables.info[t_index].fd_to_game);

}


static int game_over(int index, int fd)
{

	int i, num, p_index, won, lost;

	dbg_msg("Handling game-over message from table %d", index);

	/* Read number of statistics */
	if (FAIL(es_read_int(fd, &num)))
		return (-1);


	for (i = 0; i < num; i++) {
		if (FAIL(es_read_int(fd, &p_index)) ||
		    FAIL(es_read_int(fd, &won)) ||
		    FAIL(es_read_int(fd, &lost))) return (-1);

		/* FIXME: Do something with these statistics */

	}

	return 0;

}
