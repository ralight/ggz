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
#include <hash.h>

#define GGZ_RESYNC_SEC  0
#define GGZ_RESYNC_USEC 500000

/* Server wide data structures*/
extern struct GameInfo game_types[MAX_GAME_TYPES];
extern struct GGZState state;
extern Options opt;

/* Local functions for handling tables */
static int   table_check(GGZTable* table);
static int   table_handler_launch(GGZTable* table);
static void* table_new(void *index_ptr);
static void  table_fork(GGZTable* table);
static void  table_loop(GGZTable* table);
static int   table_handle(int op, GGZTable* table);
static void  table_remove(GGZTable* table);
static void  table_run_game(GGZTable* table, char *path);
static int   table_send_opt(GGZTable* table);
static int   table_game_over(GGZTable* table);
static int   table_log(GGZTable* table, char debug);
static int   table_game_launch(GGZTable* table);
static int   table_game_join(GGZTable* table);
static int   table_game_leave(GGZTable* table);
static int   table_event_enqueue(GGZTable* table, unsigned char opcode);
static int   table_update_event_enqueue(GGZTable* table,
					 unsigned char opcode, char* name,
					 unsigned int seat);
static int   table_pack(void** data, unsigned char opcode, GGZTable* table);
static int   table_transit_pack(void** data, unsigned char opcode, 
				GGZTable* table, char* name, 
				unsigned int seat);
static int   table_event_callback(void* target, int size, void* data);
static int   table_launch_event(char* name, int status, int index);
static void  table_free(GGZTable* table);

static int   type_match_table(int type, GGZTable* table);


/*
 * table_check() - Logs seating assignments to debug and verifies
 * their validity
 *
 *
 * Returns:  0 on success
 *           E_BAD_OPTIONS if invalid 
 */
static int table_check(GGZTable* table)
{
	int i, status = 0;
	int ai_total = seats_bot(table);
	int seat_total = seats_num(table);
	int g_type = table->type;
	int room_type;
        char allow_bits[] = { GGZ_ALLOW_ZERO, GGZ_ALLOW_ONE, GGZ_ALLOW_TWO,
			      GGZ_ALLOW_THREE, GGZ_ALLOW_FOUR, GGZ_ALLOW_FIVE,
			      GGZ_ALLOW_SIX, GGZ_ALLOW_SEVEN, GGZ_ALLOW_EIGHT};
	
	pthread_rwlock_rdlock(&rooms[table->room].lock);
	room_type = rooms[table->room].game_type;
	pthread_rwlock_unlock(&rooms[table->room].lock);

	if (room_type == g_type) 
		dbg_msg(GGZ_DBG_TABLE, 
			"Launching table of type %d in room %d (accept)", 
			g_type, table->room);
	else {
		dbg_msg(GGZ_DBG_TABLE, 
			"Launching table of type %d in room %d (invalid)", 
			g_type, table->room);
		return E_BAD_OPTIONS;
	}

	/* FIXME: should technically lock game_types */
	/* Display and verify total seats and bot seats */
	if(game_types[g_type].player_allow_mask & allow_bits[seat_total])
		dbg_msg(GGZ_DBG_TABLE, "Seats  : %d (accept)", seat_total);
	else {
		dbg_msg(GGZ_DBG_TABLE, "Seats  : %d (invalid)", seat_total);
		status = E_BAD_OPTIONS;
	}
	
	if(game_types[g_type].bot_allow_mask & allow_bits[ai_total]
	   || ai_total == 0)
		dbg_msg(GGZ_DBG_TABLE, "Bots   : %d (accept)", ai_total);
	else {
		dbg_msg(GGZ_DBG_TABLE, "Bots   : %d (invalid)", ai_total);
		status = E_BAD_OPTIONS;
	}

	dbg_msg(GGZ_DBG_TABLE, "Open Seats : %d", seats_open(table));
	dbg_msg(GGZ_DBG_TABLE, "Resv.Seats : %d", seats_reserved(table));
	dbg_msg(GGZ_DBG_TABLE, "State      : %d", table->state);
	dbg_msg(GGZ_DBG_TABLE, "Control fd : %d", table->fd);
	for (i = 0; i < seat_total; i++) {
		switch (seats_type(table, i)) {
		case GGZ_SEAT_OPEN:
			dbg_msg(GGZ_DBG_TABLE, "Seat[%d]: open", i);
			break;
		case GGZ_SEAT_BOT:
			dbg_msg(GGZ_DBG_TABLE, "Seat[%d]: computer", i);
			break;
		case GGZ_SEAT_RESV:
			dbg_msg(GGZ_DBG_TABLE, "Seat[%d]: reserved for %s", i,
				table->reserve[i]);
			break;
		case GGZ_SEAT_PLAYER:
			dbg_msg(GGZ_DBG_TABLE, "Seat[%d]: player %s", i, 
				table->seats[i]);
			break;
		}
	}
	return status;
}


/*
 * table_handler_launch() accepts the index to a new game table a
 * creates a new dedicated handler thread.
 *
 * returns 0 if successful, or a non-zero error if the thread 
 * cannot be created
 */
int table_handler_launch(GGZTable* table)
{
	pthread_t thread;
	GGZTable **index_ptr;
	int status;
	
	/* Temporary storage to pass table pointer */
	if ( (index_ptr = malloc(sizeof(GGZTable*))) == NULL)
		err_sys_exit("malloc error");
	*index_ptr = table;
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
	GGZTable* table;
	int i, status, count;

	table = *((GGZTable**)index_ptr);
	free(index_ptr);

	/* Detach thread since no one needs to join us */
	if ((status = pthread_detach(pthread_self())) != 0) {
		errno = status;
		err_sys_exit("pthread_detach error");
	}

	dbg_msg(GGZ_DBG_PROCESS, "Thread detached for table %d in room %d", 
		table->index, table->room);
 
	/* Check to see if we've exeeced total capacity for tables */
	pthread_rwlock_wrlock(&state.lock);
	if (state.tables == MAX_TABLES) {
		pthread_rwlock_unlock(&state.lock);
		table_launch_event(table->owner, E_ROOM_FULL, 0);
		return NULL;
	}
	state.tables++;
	pthread_rwlock_unlock(&state.lock);
	

	/* Setup an entry in the rooms */
	pthread_rwlock_wrlock(&rooms[table->room].lock);
	if (rooms[table->room].table_count == rooms[table->room].max_tables) {
		pthread_rwlock_unlock(&rooms[table->room].lock);
		table_launch_event(table->owner, E_ROOM_FULL, 0);
		return NULL;
	}
	count = ++rooms[table->room].table_count;
	/* Find empty spot in room */ 
	for (i = 0; i < rooms[table->room].max_tables; i++)
		if (rooms[table->room].tables[i] == NULL)
			break;
	rooms[table->room].tables[i] = table;
	pthread_rwlock_wrlock(&table->lock);
	table->index = i;
	pthread_rwlock_unlock(&table->lock);
	pthread_rwlock_unlock(&rooms[table->room].lock);

	dbg_msg(GGZ_DBG_ROOM, "Room %d table count now = %d", table->room, 
		count);

	table_fork(table);
	table_remove(table);
	table_free(table);

	return (NULL);
}


/*
 * table_fork is responsible for setting up a new game table process.
 * It forks the child to play the game, and kills the child when the
 * game is over.
 */
static void table_fork(GGZTable* table)
{
	pid_t pid;
	char path[MAX_PATH_LEN];
	char game[MAX_GAME_NAME_LEN + 1];
	char fd_name[MAX_PATH_LEN];
	int type, sock, fd;
	struct stat buf;
	
	/* Get path for game server */
	type = table->type;

	pthread_rwlock_rdlock(&game_types[type].lock);
	strncpy(path, game_types[type].path, MAX_PATH_LEN);
	strncpy(game, game_types[type].name, MAX_GAME_NAME_LEN);
	pthread_rwlock_unlock(&game_types[type].lock);

	/* Fork table process */
	if ( (pid = fork()) < 0)
		err_sys_exit("fork failed");
	else if (pid == 0) {
		/* Make sure the parent's socket is created before we go on */
		sprintf(fd_name, "%s/%s.%d", opt.tmp_dir, game, getpid());
		while (stat(fd_name, &buf) < 0)
			sleep(1);

		table_run_game(table, path);
		/* table_run_game() should never return */
		/* FIXME: handle more gracefully */
		err_sys_exit("exec of %s failed", path);
	} else {
		/* Create Unix domain socket for communication*/
		sprintf(fd_name, "%s/%s.%d", opt.tmp_dir, game, pid);
		sock = es_make_unix_socket(ES_SERVER, fd_name);
		/* FIXME: need to check validity of fd */
		
		if (listen(sock, 1) < 0) 
			err_sys_exit("listen failed");

		if ( (fd = accept(sock, NULL, NULL)) < 0)
			err_sys_exit("accept failed");

		/* We don't accept any more connections */
		close(sock);
		
		pthread_rwlock_wrlock(&table->lock);
		table->pid = pid;
		table->fd = fd;
		pthread_rwlock_unlock(&table->lock);
		
		if (table_send_opt(table) == 0)
			table_loop(table);
		
		/* Make sure game server is dead */
		kill(pid, SIGINT);
		close(table->fd);
		unlink(fd_name);
	}
}


static void table_run_game(GGZTable* table, char *path)
{
	/* Comment out for now */
	/*dbg_msg(GGZ_DBG_PROCESS, "Process forked.  Game running on table %d", 
		t_index);*/

	/* FIXME: Close all other fd's and kill threads? */
	/* FIXME: Not necessary to close other fd's if we use CLOSE_ON_EXEC */
	execv(path, NULL);
}


static int table_send_opt(GGZTable* table)
{
	GGZTable copy;
	int i, fd, seat;

	pthread_rwlock_rdlock(&table->lock);
	copy = *table;
	pthread_rwlock_unlock(&table->lock);

	fd = copy.fd;
	
	/* Send number of seats */
	if (es_write_int(fd, REQ_GAME_LAUNCH) < 0
	    || es_write_int(fd, seats_num(&copy)) < 0)
		return -1;

	/* Send seat assignments, names, and fd's */
	for (i = 0; i < seats_num(&copy); i++) {

		seat = seats_type(&copy, i);
		if (es_write_int(fd, seat) < 0)
			return -1;
		
		if (seat == GGZ_SEAT_RESV 
		    && es_write_string(fd, copy.reserve[i]) < 0)
			return -1;
	}

	return 0;
}


static void table_loop(GGZTable* table)
{
	int opcode, fd, status;
	fd_set active_fd_set, read_fd_set;
	struct timeval timer;
	
	fd = table->fd;
	FD_ZERO(&active_fd_set);
	FD_SET(fd, &active_fd_set);

	for (;;) {
		/* Process queued up events */
		if (table->events_head && event_table_handle(table) < 0)
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
		
		if (table_handle(opcode, table) < 0)
			break;
	}
}


static int table_handle(int opcode, GGZTable* table)
{
	int status;
	TableToControl request = (TableToControl)opcode;

	switch (request) {
	case RSP_GAME_LAUNCH:
		status = table_game_launch(table);
		break;
		
	case RSP_GAME_JOIN:
		status = table_game_join(table);
		break;

	case RSP_GAME_LEAVE:
		status = table_game_leave(table);
		break;
		
	case REQ_GAME_OVER:
		table_game_over(table);
		status = -1;
		break;

	case MSG_LOG:
		status = table_log(table, 0);
		break;

	case MSG_DBG:
		status = table_log(table, 1);
		break;

	default:
		dbg_msg(GGZ_DBG_PROTOCOL, 
			"Table %d in room %d sent unimplemented op %d",
			table->index, table->room, request);
		status = -1;
		break;
	}

	return status;
}


static int table_game_launch(GGZTable* table)
{
	char status;

	dbg_msg(GGZ_DBG_TABLE, "Table %d in room %d responded to launch", 
		table->index, table->room);
	
	if (table->state != GGZ_TABLE_CREATED
	    || es_read_char(table->fd, &status) < 0
	    || status < 0)
		return -1;
	
	pthread_rwlock_wrlock(&table->lock);
	table->state = GGZ_TABLE_LAUNCHED;
	pthread_rwlock_unlock(&table->lock);

	/* Signal owner that all is good */
	table_launch_event(table->owner, 0, table->index);
	
	/* Trigger a table update in this room */
	table_event_enqueue(table, GGZ_UPDATE_ADD);
		
	return 0;
}


/*
 * table_game_join handles the RSP_GAME_JOIN from the table
 * Note: table->transit_name contains malloced mem on entry
 */
static int table_game_join(GGZTable* table)
{
	char status;
	char* name;
	int seat, fd;

	dbg_msg(GGZ_DBG_TABLE, "Table %d in room %d responded to join", 
		table->index, table->room);

	/* Error: we didn't request a join! */
	if (!table->transit)
		return -1;

	if (es_read_char(table->fd, &status) < 0)
		return -1;

	/* Read saved transit data */
	name = table->transit_name;
	seat = table->transit_seat;
	fd = table->transit_fd;

	if (status == 0) {
		/* Assign seat */
		dbg_msg(GGZ_DBG_TABLE, "%s in seat %d at table %d of room %d",
			name, seat, table->index, table->room);

		pthread_rwlock_wrlock(&table->lock);
		/*free(table->seats[seat]);*/
		strcpy(table->seats[seat], name);
		if (!seats_open(table)) {
			dbg_msg(GGZ_DBG_TABLE, 
				"Table %d in room %d now full", 
				table->index, table->room);

			table->state = GGZ_TABLE_PLAYING;
			pthread_rwlock_unlock(&table->lock);
			table_update_event_enqueue(table, GGZ_UPDATE_JOIN, 
						    name, seat);
			table_event_enqueue(table, GGZ_UPDATE_STATE);
			
		} else {
			pthread_rwlock_unlock(&table->lock);
			table_update_event_enqueue(table, GGZ_UPDATE_JOIN, 
						    name, seat);
		}
	}

	/* Notify player and mark transit as done */
	transit_player_event(name, GGZ_TRANSIT_JOIN, status, table->index, fd);
			     
	/* Free strdup'd player name */
	free(table->transit_name);
				     
	table->transit = 0;
	table->transit_name = NULL;
	table->transit_fd = -1;

	return 0;
}


/*
 * table_game_leave handles the RSP_GAME_LEAVE from the table
 */
static int table_game_leave(GGZTable* table)
{
	char status;
	char* name;
	int seat;
	int ret_val = 0;

	dbg_msg(GGZ_DBG_TABLE, "Table %d in room %d responded to leave", 
		table->index, table->room);

	/* Error: we didn't request a leave! */
	if (!table->transit)
		return -1;

	if (es_read_char(table->fd, &status) < 0)
		return -1;

	/* Read in saved transit data */
	name = table->transit_name;
	seat = table->transit_seat;
	
	if (status == 0) {
		/* Vacate seat */
		dbg_msg(GGZ_DBG_TABLE, 
			"%s left seat %d at table %d of room %d", name, seat,
			table->index, table->room);
		
		pthread_rwlock_wrlock(&table->lock);
		/*free(table->seats[seat]);*/
		strcpy(table->seats[seat], "<open>");
		if (!seats_human(table)) {
			dbg_msg(GGZ_DBG_TABLE, "Table %d in room %d now empty",
				table->index, table->room);
			ret_val = -1;
		}
		
		pthread_rwlock_unlock(&table->lock);
		table_update_event_enqueue(table, GGZ_UPDATE_LEAVE, name, 
					    seat);
	}

	/* Notify player and mark transit as done */
	transit_player_event(name, GGZ_TRANSIT_LEAVE, status, 0, 0);

	/* Free strdup'd player name */
	free(table->transit_name);

	table->transit = 0;
	table->transit_name = NULL;
	table->transit_seat = -1;

	return ret_val;
}


static int table_game_over(GGZTable* table)
{
	int i, num, p_index, won, lost;

	dbg_msg(GGZ_DBG_TABLE,
		"Handling game-over request from table %d in room %d", 
		table->index, table->room);
	
	/* Read number of statistics */
	if (es_read_int(table->fd, &num) < 0)
		return (-1);
	
	for (i = 0; i < num; i++) {
		if (es_read_int(table->fd, &p_index) < 0
		    || es_read_int(table->fd, &won) < 0 
		    || es_read_int(table->fd, &lost) < 0)
			return (-1);

		/* FIXME: Do something with these statistics */

	}

	/* Mark table as done, so people don't attempt transits */
	pthread_rwlock_wrlock(&table->lock);
	table->state = GGZ_TABLE_DONE;
	pthread_rwlock_unlock(&table->lock);
	table_event_enqueue(table, GGZ_UPDATE_STATE);

	/* Send response back to game server, allowing termination */
	if (es_write_int(table->fd, RSP_GAME_OVER) < 0)
		return -1;

	return 0;
}


static int table_log(GGZTable* table, char debug) 
{
	int level, type, len, pid;
	char name[MAX_GAME_NAME_LEN];
	char* msg;
	char* buf;

	if (es_read_int(table->fd, &level) < 0
	    || es_read_string_alloc(table->fd, &msg) < 0)
		return -1;
	
	if (log_info.options & GGZ_LOGOPT_INC_GAMETYPE) {
		pthread_rwlock_rdlock(&table->lock);
		type = table->type;
		pid = table->pid;
		pthread_rwlock_unlock(&table->lock);
		
		pthread_rwlock_rdlock(&game_types[type].lock);
		strcpy(name, game_types[type].name);
		pthread_rwlock_unlock(&game_types[type].lock);
		
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


static void table_remove(GGZTable* table)
{
	int room, count, index, i;

	/* First get it off the list in rooms */
	room = table->room;
	index = table->index;

	dbg_msg(GGZ_DBG_TABLE, "Removing table %d from room %d", table->index,
		table->room);

	pthread_rwlock_wrlock(&rooms[room].lock);
	count = --rooms[room].table_count;
	rooms[room].tables[index] = NULL;
	pthread_rwlock_unlock(&rooms[room].lock);

	dbg_msg(GGZ_DBG_ROOM, "Room %d table count now = %d", room, count);

	pthread_rwlock_wrlock(&state.lock);
	state.tables--;
	pthread_rwlock_unlock(&state.lock);

	pthread_rwlock_wrlock(&table->lock);
	table->type = -1;
	table->state = GGZ_TABLE_ERROR;
	pthread_rwlock_unlock(&table->lock);

	/* Send out table-leave messages for remaining players */
	for (i = 0; i < seats_num(table); i++) {
		if (seats_type(table, i) == GGZ_SEAT_PLAYER) {
			table_update_event_enqueue(table, GGZ_UPDATE_LEAVE, 
						   table->seats[i], i);
			transit_player_event(table->seats[i], 
					     GGZ_TRANSIT_LEAVE, 0, 0, 0);
		}
	}

	table_event_enqueue(table, GGZ_UPDATE_DELETE);


	/* Process any remaining events */
	event_table_handle(table);
}


/* FIXME: redo using events to notify player */
int table_launch(char* name, int type, int room, char* desc, int seats[], 
		 char* names[MAX_TABLE_SIZE])
{
	int i;
	GGZTable *table;

	/* Allocate a new table structure */
	if ( (table = calloc(1, sizeof(GGZTable))) == NULL)
		err_sys_exit("malloc error in table_launch()");

	/* Fill in the table structure */
	pthread_rwlock_init(&table->lock, NULL);
	table->type = type;
	table->room = room;
	table->index = -1;
	table->state = GGZ_TABLE_CREATED;
	table->transit = 0;
	table->transit_name = NULL;
	table->transit_fd = -1;
	table->transit_seat = -1;
	table->fd = -1;
	table->pid = -1;
	strcpy(table->owner, name);	
	strcpy(table->desc, desc);	
	for (i = 0; i < MAX_TABLE_SIZE; i++)
		switch (seats[i]) {
		case GGZ_SEAT_OPEN:
			strcpy(table->seats[i], "<open>");
			break;
		case GGZ_SEAT_BOT:
			strcpy(table->seats[i], "<bot>");
			break;
		case GGZ_SEAT_NONE:
			strcpy(table->seats[i], "<none>");
			break;
		case GGZ_SEAT_RESV:
			strcpy(table->seats[i], "<reserved>");
			/*
			 * FIXME: lookup reserve name to verify.
			 * upon error, send E_USR_LOOKUP
			 */
			strcpy(table->reserve[i], names[i]);
			break;
		}
	
	
	/* Check validity of table info */
	if (table_check(table) < 0) {
		table_free(table);
		return E_BAD_OPTIONS;
	}

	/* Attempt to do launch of table-controller */
	if (table_handler_launch(table) != 0) {
		table_free(table);
		return E_LAUNCH_FAIL;
	}

	return 0;
}


/* Search for tables */
int table_search(char* name, int room, int type, char global, 
		 GGZTable** tables)
{
	GGZTable* t;
	int max, i, t_count, count = 0;


	if (global) {
		/* FIXME: need to do something about global counting */
		return 0;

#if 0
		if ( (*my_tables = calloc(MAX_TABLES, sizeof(GGZTable))) == NULL)
			err_sys_exit("calloc error in player_list_tables()");
		if ( (*indices = calloc(MAX_TABLES, sizeof(int))) == NULL)
			err_sys_exit("calloc error in player_list_tables()");
		
		pthread_rwlock_rdlock(&state.lock);
		t_count = state.tables;
		pthread_rwlock_unlock(&state.lock);

		/* Copy all tables of interest to local list */
		for (i = 0; (i < MAX_TABLES && count < t_count); i++) {
			pthread_rwlock_rdlock(&tables[i].lock);
			if (tables[i].type != -1 && type_match_table(type, i)){
				(*my_tables)[count] = tables[i];
				(*indices)[count++] = i;
			}
			pthread_rwlock_unlock(&tables[i].lock);
		}

		return count;
#endif
	}
		

	/* Copy pointers to tables we are interested in */
	pthread_rwlock_rdlock(&rooms[room].lock);
	t_count = rooms[room].table_count;
	max = rooms[room].max_tables;

	/* Take care of degenerate case, no tables */
	if (t_count == 0) {
		pthread_rwlock_unlock(&rooms[room].lock);
		return 0;
	}

	if ( (*tables = calloc(t_count, sizeof(GGZTable))) == NULL) {
		pthread_rwlock_unlock(&rooms[room].lock);
		err_sys_exit("calloc error in player_list_tables()");
	}

	/* Copy the tables we want */
	if (type == -1) {
		for (i = 0; (i < max && count < t_count); i++) {
			if ( (t = rooms[room].tables[i])) {
				pthread_rwlock_rdlock(&t->lock);
				(*tables)[count++] = *t;
				pthread_rwlock_unlock(&t->lock);
			}
		}
	} else {
		for (i = 0; (i < max && count < t_count); i++) {
			if ( (t = rooms[room].tables[i]) 
			     && type_match_table(type, t)) {
				pthread_rwlock_rdlock(&t->lock);
				(*tables)[count++] = *t;
				pthread_rwlock_unlock(&t->lock);
			}
		}
	}

	pthread_rwlock_unlock(&rooms[room].lock);

	return count;
}


static int table_event_enqueue(GGZTable* table, unsigned char opcode)
{
	void* data = NULL;
	int size, status, room;

	room = table->room;
	
	size = table_pack(&data, opcode, table);

	/* Queue table event for whole room */
	status = event_room_enqueue(room, table_event_callback, size, data);
	
	return status;
}


static int table_update_event_enqueue(GGZTable* table, unsigned char opcode,
				       char* name, unsigned int seat)
{
	void* data = NULL;
	int size, status, room;

	room = table->room;
	
	/* Pack up table update data */
	size = table_transit_pack(&data, opcode, table, name, seat);

	/* Queue table event for whole room */
	status = event_room_enqueue(room, table_event_callback, size, data);
	
	return status;
}


static int table_pack(void** data, unsigned char opcode, GGZTable* table)
{
	int size, index;
	char* current;
	unsigned char table_state;
	GGZTable info;

	/* Always allocate space for opcode and table index*/
	size = sizeof(char) + sizeof(int);

	pthread_rwlock_rdlock(&table->lock);
	index = table->index;
	pthread_rwlock_unlock(&table->lock);
	
	switch (opcode) {
	case GGZ_UPDATE_ADD:
		/* Pack entire table for ADD */
		size += sizeof(GGZTable);
		break;
	case GGZ_UPDATE_STATE:
		/* Pack table state for STATE */
		size += sizeof(char);
		break;
	}
		
	if ( (*data = malloc(size)) == NULL)
		err_sys_exit("malloc failed in table_pack");
	
	current = (char*)*data;
	
	*(unsigned char*)current = opcode;
	current += sizeof(char);

	*(int*)current = index;
	current += sizeof(int);

	switch (opcode) {
	case GGZ_UPDATE_ADD:
		pthread_rwlock_rdlock(&table->lock);
		info = *table;
		pthread_rwlock_unlock(&table->lock);
		
		*(GGZTable*)current = info;
		current += sizeof(GGZTable);
		break;
	case GGZ_UPDATE_STATE:
		pthread_rwlock_rdlock(&table->lock);
		table_state = table->state;
		pthread_rwlock_unlock(&table->lock);

		*(unsigned char*)current = table_state;
		current += sizeof(char);
		break;
	}

	return size;
}


static int table_transit_pack(void** data, unsigned char opcode,
			      GGZTable* table, char* name, unsigned int seat)
{
	int size, index;
	char* current;

	size = sizeof(char) + 2*sizeof(int) + strlen(name)+1;

	pthread_rwlock_rdlock(&table->lock);
	index = table->index;
	pthread_rwlock_unlock(&table->lock);
	
	if ( (*data = malloc(size)) == NULL)
		err_sys_exit("malloc failed in table_transit_pack");
	
	current = (char*)*data;
	
	*(unsigned char*)current = opcode;
	current += sizeof(char);

	*(int*)current = index;
	current += sizeof(int);

	strcpy(current, name);
	current += (strlen(name) + 1);

	*(unsigned int*)current = seat;
	current += sizeof(int);

	return size;
}


/* Event callback for delivering table list update to a player */
static int table_event_callback(void* target, int size, void* data)
{
	unsigned char opcode, table_state;
	char* name = NULL;
	char* current;
	GGZTable info;
	int fd, seat, i, index;
	GGZPlayer* player;

	player = (GGZPlayer*)target;
	fd = player->fd;

	/* Unpack event data */
	current = (char*)data;

	opcode = *(unsigned char*)current;
	current += sizeof(char);

	index = *(int*)current;
	current += sizeof(int);

	/* Always send opcode and table index */
	if (es_write_int(fd, MSG_UPDATE_TABLES) < 0 
	    || es_write_char(fd, opcode) < 0
	    || es_write_int(fd, index) < 0)
		return GGZ_EVENT_ERROR;
	
	switch (opcode) {
	case GGZ_UPDATE_DELETE:
		dbg_msg(GGZ_DBG_UPDATE, "%s sees table %d deleted",
			player->name, index);
		break;

	case GGZ_UPDATE_ADD:
		info = *(GGZTable*)current;
		current += sizeof(GGZTable);

		dbg_msg(GGZ_DBG_UPDATE, "%s sees table %d added",
			player->name, index);
		if (es_write_int(fd, info.room) < 0
		    || es_write_int(fd, info.type) < 0
		    || es_write_string(fd, info.desc) < 0
		    || es_write_char(fd, info.state) < 0
		    || es_write_int(fd, seats_num(&info)) < 0)
			return GGZ_EVENT_ERROR;
		
		/* Now send seat assignments */
		for (i = 0; i < seats_num(&info); i++) {
			seat = seats_type(&info, i);
			if (es_write_int(fd, seat) < 0)
				return GGZ_EVENT_ERROR;

			switch (seat) {
			case GGZ_SEAT_OPEN:
			case GGZ_SEAT_BOT:
				continue;  /* no name for these */
			case GGZ_SEAT_RESV:
				name = info.reserve[i];
				break;
			case GGZ_SEAT_PLAYER: 
				name = info.seats[i];
				break;
			}

			if (es_write_string(fd, name) < 0)
				return GGZ_EVENT_ERROR;
		}
		break;
		
	case GGZ_UPDATE_LEAVE:
	case GGZ_UPDATE_JOIN:
		name = (char*)current;
		current += (strlen(name) + 1);
		seat = *(int*)current;
		current += sizeof(int);

		dbg_msg(GGZ_DBG_UPDATE, "%s sees %s %s seat %d at table %d", 
			player->name, name, 
			(opcode == GGZ_UPDATE_JOIN ? "join" : "leave"),
			seat, index);
		if (es_write_int(fd, seat) < 0
		    || es_write_string(fd, name) < 0) {
			return GGZ_EVENT_ERROR;
		}
		break;
	case GGZ_UPDATE_STATE:
		table_state = *(unsigned char*)current;
		current += sizeof(char);

		dbg_msg(GGZ_DBG_UPDATE, "%s sees table %d new state %d",
			player->name, index, table_state);
		if (es_write_char(fd, table_state) < 0)
			return GGZ_EVENT_ERROR;
		break;
	}
	
	return GGZ_EVENT_OK;
}


static int table_launch_event(char* name, int status, int index)
{
	int size;
	char* current;
	void* data;

	size = sizeof(int);
	
	/* We pass back the table index if launch was successful */
	if (status == 0)
		size += sizeof(int);

	if ( (data = malloc(size)) == NULL)
		err_sys_exit("malloc failed in table_launch_event");
	
	/* Start packing the data */
	current = (char*)data;
	
	*(int*)current = status;
	current += sizeof(int);
	
	if (status == 0) {
		*(int*)current = index;
		current += sizeof(int);
	}

	return event_player_enqueue(name, player_launch_callback, size, data);
}


/* Free dynamically allocated memory associated with a table*/
static void table_free(GGZTable* table)
{
	int i;

	/* FIXME: do we need to free transit too? */

	for (i = 0; i < MAX_TABLE_SIZE; i++) {
		/*free(table->seats[i]);*/
		/*if (table->reserve[i])
		  free (table->reserve[i]);*/
	}
	free(table);
}


/* FIXME: move to type.c */
int type_match_table(int type, GGZTable* table)
{
	/* FIXME: Do reservation checking properly */
	return ( type == GGZ_TYPE_ALL
		 || (type >= 0 && type == table->type)
		 || (type == GGZ_TYPE_OPEN && seats_open(table))
		 || type == GGZ_TYPE_RES);
}


/*
 * table_lookup() looks up a table inside a room
 *
 * Receives:
 * int room        : index of room in which table resides
 * int index       : index of table in room to lookup
 *
 * Returns:
 * GGZTable *table : pointer to desired table
 *
 * Note: table is returned with write lock acquired
 */
GGZTable* table_lookup(int room, int index)
{
	GGZTable* table = NULL;

	if (room != -1 && index != -1) {
		/* Grab pointer to table from room index */
		pthread_rwlock_rdlock(&rooms[room].lock);
		if ( (table = rooms[room].tables[index]) != NULL)
			pthread_rwlock_wrlock(&table->lock);
		pthread_rwlock_unlock(&rooms[room].lock);
	}
	
	return table;
}
