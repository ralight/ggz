/*
 * File: table.c
 * Author: Brent Hendricks
 * Project: GGZ Server
 * Date: 1/9/00
 * Desc: Functions for handling tables
 * $Id: table.c 3071 2002-01-12 02:00:40Z jdorje $
 *
 * Copyright (C) 1999-2002 Brent Hendricks.
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

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/signal.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

#include <ggzdmod.h>

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
#include <net.h>

#define GGZ_RESYNC_SEC  0
#define GGZ_RESYNC_USEC 500000

/* Server wide data structures*/
extern struct GameInfo game_types[MAX_GAME_TYPES];
extern struct GGZState state;
extern Options opt;

/* Local functions for handling tables */
static int   table_check(GGZTable* table);
static int   table_handler_launch(GGZTable* table);
static void* table_new_thread(void *index_ptr);
static int   table_start_game(GGZTable *table);
static void  table_loop(GGZTable* table);
static void  table_remove(GGZTable* table);

/* Handlers for ggzdmod events */
static void table_handle_state(GGZdMod *mod, GGZdModEvent event, void *data);
static void table_game_join(GGZdMod *ggzdmod, GGZdModEvent event, void *data);
static void table_game_leave(GGZdMod *ggzdmod, GGZdModEvent event, void *data);
static void table_log(GGZdMod *ggzdmod, GGZdModEvent event, void *data);
static void table_error(GGZdMod *ggzdmod, GGZdModEvent event, void *data);

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



GGZTable* table_new(void)
{
	int i;
	GGZTable *table;
	
	/* Allocate a new table structure */
	if ( (table = calloc(1, sizeof(GGZTable))) == NULL)
		err_sys_exit("malloc error in net_handle_table()");
	
	
	pthread_rwlock_init(&table->lock, NULL);
	table->type = -1;
	table->room = -1;
	table->index = -1;
	table->state = GGZ_TABLE_CREATED;
	table->transit = 0;
	table->transit_name = NULL;
	table->transit_fd = -1;
	table->transit_seat = -1;
	table->ggzdmod = NULL;
	table->pid = -1;

	for (i = 0; i < MAX_TABLE_SIZE; i++)
		strcpy(table->seats[i], "<none>");

	return table;
}



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
	dbg_msg(GGZ_DBG_TABLE, "GGZdMod    : %x", (unsigned int)table->ggzdmod);
	if (table->ggzdmod)
		dbg_msg(GGZ_DBG_TABLE, "Control fd : %d", ggzdmod_get_fd(table->ggzdmod));
			
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
		default:
			dbg_msg(GGZ_DBG_TABLE, "Seat[%d]: **invalid**", i);
			status = E_BAD_OPTIONS;
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
	status = pthread_create(&thread, NULL, table_new_thread, index_ptr);
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
static void* table_new_thread(void *index_ptr)
{
	GGZTable* table;
	int i, status, count;
	char *rname, *gname;

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
		log_msg(GGZ_LOG_NOTICE,
			"SERVER_FULL - %s could not create a new table",
			table->owner);
		table_launch_event(table->owner, E_ROOM_FULL, 0);
		return NULL;
	}
	state.tables++;
	pthread_rwlock_unlock(&state.lock);
	

	/* Setup an entry in the rooms */
	pthread_rwlock_wrlock(&rooms[table->room].lock);
	rname = strdup(rooms[table->room].name);
	if (rooms[table->room].table_count == rooms[table->room].max_tables) {
		pthread_rwlock_unlock(&rooms[table->room].lock);
		log_msg(GGZ_LOG_NOTICE,
			"ROOM_FULL - %s could not create table in %s",
			table->owner, rname);
		free(rname);
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

	/* Get the name of this game type */
	pthread_rwlock_rdlock(&game_types[table->type].lock);
	gname = strdup(game_types[table->type].name);
	pthread_rwlock_unlock(&game_types[table->type].lock);

	/* Let the game begin...*/
	if (table_start_game(table) == 0) {
		log_msg(GGZ_LOG_TABLES,
			"TABLE_START - %s started a new game of %s in %s",
			table->owner, gname, rname);
		free(rname);

		table_loop(table);

		log_msg(GGZ_LOG_TABLES,
			"TABLE_END - Game of %s started by %s has ended",
			gname, table->owner);
		free(gname);
	}
	else {
		dbg_msg(GGZ_DBG_TABLE, "Table %d failed to start game module", 
			table->index);
		table_launch_event(table->owner, E_LAUNCH_FAIL, 0);
	}
	
	table_remove(table);
	table_free(table);

	return (NULL);
}


/* Create and start game module */
static int table_start_game(GGZTable *table)
{
#if 0
	char *args[] = {"logmod", NULL};
#endif
	char *path, **args;
	int type, n_args, i, num_seats, status = 0;
	GGZSeat seat;

	pthread_rwlock_wrlock(&table->lock);
	table->ggzdmod = ggzdmod_new(GGZDMOD_GGZ);
	pthread_rwlock_unlock(&table->lock);	

	/* Some weird error in creating the module */
	if (!table->ggzdmod)
		return -1;

	/* Build our argument list */
	type = table->type;
	pthread_rwlock_rdlock(&game_types[type].lock);
	path = game_types[type].path;
	n_args = game_types[type].n_args;
	/* Leave enough room for executable and terminating NULL */
	args = calloc(n_args + 2, sizeof(char *));
	if (!(args[0] = strdup(path)))
		err_sys_exit("strdup failure in table_fork()");
	for (i = 0; i < n_args; i++) {
		if (!(args[i+1] = strdup(game_types[type].args[i])))
			err_sys_exit("strdup failure in table_fork()");
	}
	pthread_rwlock_unlock(&game_types[type].lock);

	/* Set a pointer to the table so we can get it back in the
           event handlers */
	ggzdmod_set_gamedata(table->ggzdmod, table);

	/* Setup handlers for game module events */
        ggzdmod_set_handler(table->ggzdmod, GGZDMOD_EVENT_STATE, &table_handle_state);
        ggzdmod_set_handler(table->ggzdmod, GGZDMOD_EVENT_JOIN, &table_game_join);
        ggzdmod_set_handler(table->ggzdmod, GGZDMOD_EVENT_LEAVE, &table_game_leave);
        ggzdmod_set_handler(table->ggzdmod, GGZDMOD_EVENT_LOG, &table_log);
	ggzdmod_set_handler(table->ggzdmod, GGZDMOD_EVENT_ERROR, &table_error);
	
        /* Setup seats for game table */
	num_seats = seats_num(table);
	ggzdmod_set_num_seats(table->ggzdmod, num_seats);
        for (i = 0; i < num_seats; i++) {
		seat.num = i;
		seat.type = seats_type(table, i);
		if (seat.type == GGZ_SEAT_RESV)
			seat.name = table->reserve[i];
		else
			seat.name = NULL;
		seat.fd = -1;
		ggzdmod_set_seat(table->ggzdmod, &seat);
        }

	/* And start the game */
	ggzdmod_set_module(table->ggzdmod, args);
	if (ggzdmod_connect(table->ggzdmod) < 0)
		status = -1;

	/* Free arguments */
	for (i = 0; i < n_args + 1; i++)
		free(args[i]);
	free(args);

	return status;
}


static void table_loop(GGZTable* table)
{
	int fd, status;
	fd_set active_fd_set, read_fd_set;
	struct timeval timer;
	
	fd = ggzdmod_get_fd(table->ggzdmod);
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

		if (ggzdmod_dispatch(table->ggzdmod) < 0)
			break;

		if (table->state == GGZ_TABLE_DONE)
			break;
	}

	dbg_msg(GGZ_DBG_TABLE, "Table %d in room %d loop completed", 
		table->index, table->room);
}


/*
 * table_game_join handles the RSP_GAME_JOIN from the table
 * Note: table->transit_name contains malloced mem on entry
 */
static void table_game_join(GGZdMod *ggzdmod, GGZdModEvent event, void *data)
{
	GGZTable* table = ggzdmod_get_gamedata(ggzdmod);
	char* name;
	int seat, fd, msg_status;

	dbg_msg(GGZ_DBG_TABLE, "Table %d in room %d responded to join", 
		table->index, table->room);

	/* Error: we didn't request a join! */
	if (!table->transit)
		return;

	/* Read saved transit data */
	name = table->transit_name;
	seat = table->transit_seat;
	fd = table->transit_fd;

	/* Notify player of transit status */
	msg_status = transit_player_event(name, GGZ_TRANSIT_JOIN, 0, 
					  table->index, fd);
	/* FIXME: we still need to handle the case where the join fails */

	/* Transit successful: Assign seat */
	pthread_rwlock_wrlock(&table->lock);
	strcpy(table->seats[seat], name);
	pthread_rwlock_unlock(&table->lock);
	
	/* If player notification failed, they must've logged out */
	if (msg_status < 0) {
		dbg_msg(GGZ_DBG_TABLE, "%s logged out during join",
			name);
		transit_table_event(table->room, table->index, 
				    GGZ_TRANSIT_LEAVE, name);
	} else {
		table_update_event_enqueue(table, GGZ_UPDATE_JOIN, 
					   name, seat);
		dbg_msg(GGZ_DBG_TABLE, 
			"%s in seat %d at table %d of room %d",
			name, seat, table->index, table->room);
	}
	
	/* Clear table for next transit */
	pthread_rwlock_wrlock(&table->lock);
	table->transit = 0;
	free(table->transit_name);
	table->transit_name = NULL;
	table->transit_fd = -1;
	pthread_rwlock_unlock(&table->lock);
}


/*
 * table_game_leave handles the RSP_GAME_LEAVE from the table
 */
static void table_game_leave(GGZdMod *ggzdmod, GGZdModEvent event, void *data)
{
	GGZTable* table = ggzdmod_get_gamedata(ggzdmod);
	char* name;
	char status, empty = 0;
	int seat;

	dbg_msg(GGZ_DBG_TABLE, "Table %d in room %d responded to leave", 
		table->index, table->room);

	status = *(char*)data;

	/* Error: we didn't request a leave! */
	if (!table->transit)
		return ;

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
			empty = 1;
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

	/* If the game has set the KillWhenEmpty option, we kill it
	   when the last player leaves.  If not, we rely on the game
	   to halt itself. */
	if (empty && game_types[table->type].kill_when_empty)
		(void)ggzdmod_disconnect(ggzdmod);
}


static void table_handle_state(GGZdMod *mod, GGZdModEvent event, void *data)
{
	GGZTable* table = ggzdmod_get_gamedata(mod);
	GGZdModState cur, prev;

	prev = *(GGZdModState*)data;
	cur = ggzdmod_get_state(mod);

	switch (cur) {
	case GGZDMOD_STATE_WAITING:
		dbg_msg(GGZ_DBG_TABLE, "Table %d in room %d now waiting", 
			table->index, table->room);

		/* FIXME: maybe we shouldn't keep track of table state
                   here any more? */
		if (table->state != GGZ_TABLE_CREATED)
			return;	
		pthread_rwlock_wrlock(&table->lock);
		table->state = GGZ_TABLE_WAITING;
		pthread_rwlock_unlock(&table->lock);
		
		/* Signal owner that all is good */
		table_launch_event(table->owner, 0, table->index);
	
		/* Trigger a table update in this room */
		table_event_enqueue(table, GGZ_UPDATE_ADD);
		break;
		
	case GGZDMOD_STATE_PLAYING:
		pthread_rwlock_wrlock(&table->lock);
		table->state = GGZ_TABLE_PLAYING;
		pthread_rwlock_unlock(&table->lock);
		
		table_event_enqueue(table, GGZ_UPDATE_STATE);
		dbg_msg(GGZ_DBG_TABLE, "Table %d in room %d now full/playing",
			table->index, table->room);
		break;

	case GGZDMOD_STATE_DONE:
		dbg_msg(GGZ_DBG_TABLE,
			"Table %d in room %d game module is done", 
			table->index, table->room);

		/* Mark table as done, so people don't attempt transits */
		pthread_rwlock_wrlock(&table->lock);
		table->state = GGZ_TABLE_DONE;
		pthread_rwlock_unlock(&table->lock);
		table_event_enqueue(table, GGZ_UPDATE_STATE);
		break;
	default:
		/* FIXME: some kind of error handling here */
		break;
	}
}


static void table_log(GGZdMod *ggzdmod, GGZdModEvent event, void *data)
{
	GGZTable* table = ggzdmod_get_gamedata(ggzdmod);
	int type, len;
	char name[MAX_GAME_NAME_LEN];
	char *prescan = data, *msg, *p, *m;
	char *buf;
	int  pcts=0;

	/* If the message has any %'s in it they must be escaped. */
	/* We could eliminate them, but this would prevent games  */
	/* from putting the percent sign into messages at all. :( */
	for(p=prescan; *p!='\0'; p++)
		if(*p == '%') {
			p++;
			if(*p == '%')
				p++;
			else
				pcts++;
		}
	if(pcts > 0) {
		if((msg = malloc(strlen(prescan) + pcts + 1)) == NULL)
			err_sys_exit("malloc failed");
		for(p=prescan,m=msg; *p!='\0'; p++,m++) {
			if(*p == '%') {
				*m++ = '%';
				if(*++p != '%')
					*m++ = '%';
			}
			*m = *p;
		}
		*m = *p;
	} else
		msg = prescan;

	if (log_info.options & GGZ_LOGOPT_INC_GAMETYPE) {
		pthread_rwlock_rdlock(&table->lock);
		type = table->type;
		pthread_rwlock_unlock(&table->lock);
		
		pthread_rwlock_rdlock(&game_types[type].lock);
		strcpy(name, game_types[type].name);
		pthread_rwlock_unlock(&game_types[type].lock);
		
		len = strlen(msg) + strlen(name) + 5;
		
		if ( (buf = malloc(len)) == NULL)
			err_sys_exit("malloc failed");

		memset(buf, 0, len);
		snprintf(buf, (len - 1), "(%s) ", name);
		
		strncat(buf, msg, (len - 1));
		if(msg != prescan)
			free(msg);
		msg = buf;
	}
	
	dbg_msg(GGZ_DBG_TABLE, msg);

	if(msg != prescan)
		free(msg);
}


static void table_error(GGZdMod *ggzdmod, GGZdModEvent event, void *data)
{
	GGZTable* table = ggzdmod_get_gamedata(ggzdmod);
	
	dbg_msg(GGZ_DBG_TABLE, "GAME ERROR: %s", (char*)data);

	/* If we never made out of CREATED state, it's a launch error */
	if (ggzdmod_get_state(ggzdmod) == GGZDMOD_STATE_CREATED)
		table_launch_event(table->owner, E_LAUNCH_FAIL, 0);


	/* FIXME: instead, we should set state to done, once this is supported*/
	(void)ggzdmod_disconnect(ggzdmod);
}



static void table_remove(GGZTable* table)
{
	int room, count, index, i;

	/* Disconnect from the game server */
	(void)ggzdmod_disconnect(table->ggzdmod);

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
int table_launch(GGZTable *table, char* name)
{
	/* Fill in the table owner */
	strcpy(table->owner, name);	
	
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
	int seat = -1, index;
	GGZPlayer* player;

	player = (GGZPlayer*)target;

	/* Unpack event data */
	current = (char*)data;

	opcode = *(unsigned char*)current;
	current += sizeof(char);

	index = *(int*)current;
	current += sizeof(int);

	
	switch (opcode) {
	case GGZ_UPDATE_DELETE:
		info.index = index;
		dbg_msg(GGZ_DBG_UPDATE, "%s sees table %d deleted",
			player->name, index);
		break;

	case GGZ_UPDATE_ADD:
		info = *(GGZTable*)current;
		current += sizeof(GGZTable);
		info.index = index;
		dbg_msg(GGZ_DBG_UPDATE, "%s sees table %d added",
			player->name, index);
		break;
		
	case GGZ_UPDATE_STATE:
		table_state = *(unsigned char*)current;
		current += sizeof(char);
		info.index = index;
		info.state = table_state;

		dbg_msg(GGZ_DBG_UPDATE, "%s sees table %d new state %d",
			player->name, index, table_state);
		break;

	case GGZ_UPDATE_LEAVE:
	case GGZ_UPDATE_JOIN:
		name = (char*)current;
		current += (strlen(name) + 1);
		seat = *(int*)current;
		current += sizeof(int);
		info.index = index;
		strcpy(info.seats[seat], name);

		dbg_msg(GGZ_DBG_UPDATE, "%s sees %s %s seat %d at table %d", 
			player->name, name, 
			(opcode == GGZ_UPDATE_JOIN ? "join" : "leave"),
			seat, index);

		break;
	}

	if (net_send_table_update(player->net, opcode, &info, seat) < 0)
		return GGZ_EVENT_ERROR;
	
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
	ggzdmod_free(table->ggzdmod);
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
