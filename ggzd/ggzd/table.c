/*
 * File: table.c
 * Author: Brent Hendricks
 * Project: GGZ Server
 * Date: 1/9/00
 * Desc: Functions for handling tables
 * $Id: table.c 4529 2002-09-12 21:44:40Z jdorje $
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

#include "config.h"

#include "ggzd.h"
#include "datatypes.h"
#include "protocols.h"
#include "err_func.h"
#include "table.h"
#include "players.h"
#include "seats.h"
#include "transit.h"
#include "room.h"
#include "hash.h"
#include "net.h"

#include <ggz_common.h>
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
#include "client.h"


#define GGZ_RESYNC_SEC  0
#define GGZ_RESYNC_USEC 500000

/* Packaging for seat events */
struct GGZSeatChange {
	int table;
	int num_seats;
	struct GGZTableSeat seat;
};

/* Packaging for spectator events */
struct GGZSpectatorChange {
	int table;
	int num_spectators;
	struct GGZTableSpectator spectator;
};

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
static void table_game_seatchange(GGZdMod *ggzdmod, GGZdModEvent event, void *data);
static void table_game_spectator_join(GGZdMod *ggzdmod, GGZdModEvent event, void *data);
static void table_game_spectator_leave(GGZdMod *ggzdmod, GGZdModEvent event, void *data);
static void table_log(GGZdMod *ggzdmod, GGZdModEvent event, void *data);
static void table_error(GGZdMod *ggzdmod, GGZdModEvent event, void *data);

static int   table_event_enqueue(GGZTable* table, GGZUpdateOpcode opcode);
static int   table_update_event_enqueue(GGZTable* table,
					GGZUpdateOpcode opcode, char* name,
					 unsigned int seat);
static int   table_seat_event_enqueue(GGZTable* table, GGZUpdateOpcode opcode,
				      unsigned int seat);
/*static int   table_spectator_event_enqueue(GGZTable* table, GGZUpdateOpcode opcode,
				      unsigned int spectator);*/
			      
static int   table_pack(void** data, unsigned char opcode, GGZTable* table);
static int   table_transit_pack(void** data, unsigned char opcode, 
				GGZTable* table, char* name, 
				unsigned int seat);
static GGZEventFuncReturn table_event_callback(void* target, int size,
                                                 void* data);
static GGZEventFuncReturn table_seat_event_callback(void* target, int size,
						    void* data);
/*static GGZEventFuncReturn table_spectator_event_callback(void* target, int size,
						    void* data);*/
static GGZEventFuncReturn table_kill_callback(void* target, int size, void* data);
static int   table_launch_event(char* name, int status, int index);

static int   type_match_table(int type, GGZTable* table);



GGZTable* table_new(void)
{
	GGZTable *table;
#if !defined UNLIMITED_SEATS || !defined UNLIMITED_SPECTATORS
	int i;
#endif
	
	/* Allocate a new table structure */
	table = ggz_malloc(sizeof(*table));
	
	pthread_rwlock_init(&table->lock, NULL);
	table->type = -1;
	table->room = -1;
	table->index = -1;
	table->state = GGZ_TABLE_CREATED;
	table->transit = 0;
	table->transit_name = NULL;
	table->transit_seat = -1;
	table->ggzdmod = NULL;

#ifdef UNLIMITED_SEATS
	table->num_seats = 0;
	table->seat_types = NULL;
	table->seat_names = NULL;
#else
	for (i = 0; i < MAX_TABLE_SIZE; i++)
		table->seat_types[i] = GGZ_SEAT_NONE;
#endif

#ifdef UNLIMITED_SPECTATORS
	table->max_num_spectators = 0;
	table->spectators = NULL;
#else
	for (i = 0; i < MAX_TABLE_SPECTATORS; i++)
		table->spectators[i][0] = '\0';
#endif

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
	int ai_total = seats_count(table, GGZ_SEAT_BOT);
	int seat_total = seats_num(table);
	int spectator_total = spectators_count(table);
	int g_type = table->type;
	int room_type;
	
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
	if (ggz_numberlist_isset(&game_types[g_type].player_allow_list,
				  seat_total))
		dbg_msg(GGZ_DBG_TABLE, "Seats  : %d (accept)", seat_total);
	else {
		dbg_msg(GGZ_DBG_TABLE, "Seats  : %d (invalid)", seat_total);
		status = E_BAD_OPTIONS;
	}
	
	if(ai_total == 0
	   || ggz_numberlist_isset(&game_types[g_type].bot_allow_list,
				   ai_total))
		dbg_msg(GGZ_DBG_TABLE, "Bots   : %d (accept)", ai_total);
	else {
		dbg_msg(GGZ_DBG_TABLE, "Bots   : %d (invalid)", ai_total);
		status = E_BAD_OPTIONS;
	}

	if (game_types[g_type].allow_spectators)
		dbg_msg(GGZ_DBG_TABLE, "Spectators: %d", spectator_total);
	else {
		if (spectator_total > 0)
			status = E_BAD_OPTIONS;
		dbg_msg(GGZ_DBG_TABLE, "Spectators not supported.");
	}

	dbg_msg(GGZ_DBG_TABLE, "Open Seats : %d", seats_count(table, GGZ_SEAT_OPEN));
	dbg_msg(GGZ_DBG_TABLE, "Resv.Seats : %d", seats_count(table, GGZ_SEAT_RESERVED));
	dbg_msg(GGZ_DBG_TABLE, "State      : %d", table->state);
	dbg_msg(GGZ_DBG_TABLE, "GGZdMod    : %x", (unsigned int)table->ggzdmod);
	
	/* FIXME: this correctly logs everything about the GGZdMod object, but it
	   will incorrectly be labeled as coming from the table itself instead of
	   being a GGZ internal log.  Oh well. */
	ggzdmod_check(table->ggzdmod);
			
	for (i = 0; i < seat_total; i++) {
		switch (seats_type(table, i)) {
		case GGZ_SEAT_OPEN:
			dbg_msg(GGZ_DBG_TABLE, "Seat[%d]: open", i);
			break;
		case GGZ_SEAT_BOT:
			dbg_msg(GGZ_DBG_TABLE, "Seat[%d]: computer", i);
			break;
		case GGZ_SEAT_RESERVED:
			dbg_msg(GGZ_DBG_TABLE, "Seat[%d]: reserved for %s", i,
				table->seat_names[i]);
			break;
		case GGZ_SEAT_PLAYER:
			dbg_msg(GGZ_DBG_TABLE, "Seat[%d]: player %s", i, 
				table->seat_names[i]);
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
	index_ptr = ggz_malloc(sizeof(*index_ptr));
	*index_ptr = table;
	status = pthread_create(&thread, NULL, table_new_thread, index_ptr);
	if (status != 0)
		ggz_free(index_ptr);

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
	ggz_free(index_ptr);

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
	rname = ggz_strdup(rooms[table->room].name);
	if (rooms[table->room].table_count == rooms[table->room].max_tables) {
		pthread_rwlock_unlock(&rooms[table->room].lock);
		log_msg(GGZ_LOG_NOTICE,
			"ROOM_FULL - %s could not create table in %s",
			table->owner, rname);
		ggz_free(rname);
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
	gname = ggz_strdup(game_types[table->type].name);
	pthread_rwlock_unlock(&game_types[table->type].lock);

	/* Let the game begin...*/
	if (table_start_game(table) == 0) {
		log_msg(GGZ_LOG_TABLES,
			"TABLE_START - %s started a new game of %s in %s",
			table->owner, gname, rname);
		ggz_free(rname);

		table_loop(table);

		log_msg(GGZ_LOG_TABLES,
			"TABLE_END - Game of %s started by %s has ended",
			gname, table->owner);
		ggz_free(gname);
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
	char **args;
	char *pwd;
	int type, i, num_seats, status = 0;
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
	pwd = game_types[type].data_dir;
	args = game_types[type].exec_args;
	pthread_rwlock_unlock(&game_types[type].lock);

	/* Set a pointer to the table so we can get it back in the
           event handlers */
	ggzdmod_set_gamedata(table->ggzdmod, table);

	/* Setup handlers for game module events */
        ggzdmod_set_handler(table->ggzdmod, GGZDMOD_EVENT_STATE, &table_handle_state);
        ggzdmod_set_handler(table->ggzdmod, GGZDMOD_EVENT_JOIN, &table_game_join);
        ggzdmod_set_handler(table->ggzdmod, GGZDMOD_EVENT_LEAVE, &table_game_leave);
        ggzdmod_set_handler(table->ggzdmod, GGZDMOD_EVENT_SEAT, &table_game_seatchange);
        ggzdmod_set_handler(table->ggzdmod, GGZDMOD_EVENT_SPECTATOR_JOIN, &table_game_spectator_join);
        ggzdmod_set_handler(table->ggzdmod, GGZDMOD_EVENT_SPECTATOR_LEAVE, &table_game_spectator_leave);
        ggzdmod_set_handler(table->ggzdmod, GGZDMOD_EVENT_LOG, &table_log);
	ggzdmod_set_handler(table->ggzdmod, GGZDMOD_EVENT_ERROR, &table_error);
	
        /* Setup seats for game table */
	num_seats = seats_num(table);
	ggzdmod_set_num_seats(table->ggzdmod, num_seats);
        for (i = 0; i < num_seats; i++) {
		seat.num = i;
		seat.type = seats_type(table, i);
		if (seat.type == GGZ_SEAT_RESERVED)
			seat.name = table->seat_names[i];
		else
			seat.name = NULL;
		seat.fd = -1;
		if (ggzdmod_set_seat(table->ggzdmod, &seat) < 0)
			status = 1;
        }

	/* And start the game */
	ggzdmod_set_module(table->ggzdmod, pwd, args);
	if (ggzdmod_connect(table->ggzdmod) < 0)
		status = -1;

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

		/* An event might cause the table to be done */
		if (table->state == GGZ_TABLE_DONE)
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
				err_sys_exit("select error in table_loop()");
		}

		if (ggzdmod_dispatch(table->ggzdmod) < 0)
			break;

		/* ggzdmod might cause the table to be done */
		if (table->state == GGZ_TABLE_DONE)
			break;
	}

	dbg_msg(GGZ_DBG_TABLE, "Table %d in room %d loop completed", 
		table->index, table->room);
}


/*
 * table_game_join handles the RSP_GAME_JOIN from the table
 * Note: table->transit_name contains allocated mem on entry
 */
static void table_game_join(GGZdMod *ggzdmod, GGZdModEvent event, void *data)
{
	GGZTable* table = ggzdmod_get_gamedata(ggzdmod);
	char* name;
	int seat_num, msg_status;
	struct GGZTableSeat seat;

	dbg_msg(GGZ_DBG_TABLE, "Table %d in room %d responded to join", 
		table->index, table->room);

	/* Error: we didn't request a join! */
	if (!table->transit)
		return;

	/* Read saved transit data */
	name = table->transit_name;
	seat_num = table->transit_seat;

	/* Notify player of transit status */
	msg_status = transit_player_event(name, GGZ_TRANSIT_JOIN, 0, 
					  table->index);
	/* FIXME: we still need to handle the case where the join fails */

	/* Transit successful: Assign seat */
	pthread_rwlock_wrlock(&table->lock);
	table->seat_types[seat_num] = GGZ_SEAT_PLAYER;
	strcpy(table->seat_names[seat_num], name);
	pthread_rwlock_unlock(&table->lock);
	
	/* If player notification failed, they must've logged out */
	if (msg_status < 0) {
		dbg_msg(GGZ_DBG_TABLE, "%s logged out during join",
			name);
		
		seat.index = seat_num;
		seat.type = GGZ_SEAT_OPEN;
		seat.name[0] = '\0';
		seat.fd = -1;
		
		transit_seat_event(table->room, table->index, seat, name);

	} else {
		table_update_event_enqueue(table, GGZ_UPDATE_JOIN, 
					   name, seat_num);
		dbg_msg(GGZ_DBG_TABLE, 
			"%s in seat %d at table %d of room %d",
			name, seat_num, table->index, table->room);
	}
	
	/* Clear table for next transit */
	pthread_rwlock_wrlock(&table->lock);
	table->transit = 0;
	ggz_free(table->transit_name);
	table->transit_name = NULL;
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
		table->seat_types[seat] = GGZ_SEAT_OPEN;
		
		if (!seats_count(table, GGZ_SEAT_PLAYER)) {
			dbg_msg(GGZ_DBG_TABLE, "Table %d in room %d now empty",
				table->index, table->room);
			empty = 1;
		}
		pthread_rwlock_unlock(&table->lock);
		table_update_event_enqueue(table, GGZ_UPDATE_LEAVE, name, 
					    seat);
	}

	/* Notify player and mark transit as done */
	transit_player_event(name, GGZ_TRANSIT_LEAVE, status, 0);

	/* Free strdup'd player name */
	ggz_free(table->transit_name);

	table->transit = 0;
	table->transit_name = NULL;
	table->transit_seat = -1;

	/* If the game has set the KillWhenEmpty option, we kill it
	   when the last player leaves.  If not, we rely on the game
	   to halt itself. */
	if (empty && game_types[table->type].kill_when_empty)
		(void)ggzdmod_disconnect(ggzdmod);
}


/*
 * table_game_seatchange handles the RSP_GAME_SEAT from the table
 * Note: table->transit_name contains allocated mem on entry
 */
static void table_game_seatchange(GGZdMod *ggzdmod, GGZdModEvent event, void *data)
{
	GGZSeat seat;
	GGZTable* table = ggzdmod_get_gamedata(ggzdmod);
	char* caller;
	int seat_num, msg_status;

	dbg_msg(GGZ_DBG_TABLE, "Table %d in room %d responded to seat change", 
		table->index, table->room);

	/* Error: we didn't request a seat change! */
	if (!table->transit)
		return;
	
	/* Read saved transit data */
	caller = table->transit_name;
	seat_num = table->transit_seat;

	/* Notify player of transit status */
	msg_status = transit_player_event(caller, GGZ_TRANSIT_SEAT, 0, 
					  table->index);

	/* Get new seat info */
	seat = ggzdmod_get_seat(ggzdmod, seat_num);

	/* Write new seat data to table */
	pthread_rwlock_wrlock(&table->lock);
	table->seat_types[seat_num] = seat.type;
	strcpy(table->seat_names[seat_num], seat.name);
	pthread_rwlock_unlock(&table->lock);
	
	/* If player notification failed, they must've logged out */
	if (msg_status < 0) {
		dbg_msg(GGZ_DBG_TABLE, "%s logged out during update", caller);
		
		/* FIXME: if this was a join, force a leave 
		   transit_table_event(table->room, table->index, 
		   GGZ_TRANSIT_LEAVE, name);
		*/
	}
	
	table_seat_event_enqueue(table, GGZ_UPDATE_SEAT, seat_num);
				   
	dbg_msg(GGZ_DBG_TABLE, 
		"Seat %d of table %d in room %d now %s with name %s", 
		seat.num, table->index, table->room, 
		ggz_seattype_to_string(seat.type), seat.name);
	
	/* Clear table for next transit */
	pthread_rwlock_wrlock(&table->lock);
	table->transit = 0;
	ggz_free(table->transit_name);
	table->transit_name = NULL;
	pthread_rwlock_unlock(&table->lock);
}

/*
 * table_game_spectator_join handles the RSP_GAME_JOIN_SPECTATOR from the table
 * Note: table->transit_name contains allocated mem on entry
 */
static void table_game_spectator_join(GGZdMod *ggzdmod, GGZdModEvent event, void *data)
{
	GGZTable* table = ggzdmod_get_gamedata(ggzdmod);
	char* name;
	int spectator_num, msg_status;
	struct GGZTableSpectator spectator;

	dbg_msg(GGZ_DBG_TABLE, "Table %d in room %d responded to spectator join", 
		table->index, table->room);

	/* Error: we didn't request a join! */
	if (!table->transit)
		return;

	/* Read saved transit data */
	name = table->transit_name;
	spectator_num = table->transit_seat;

	/* Notify player of transit status */
	msg_status = transit_player_event(name, GGZ_TRANSIT_JOIN_SPECTATOR, 0,
					  table->index);
	/* FIXME: we still need to handle the case where the join fails */

	/* Transit successful: Assign seat */
	pthread_rwlock_wrlock(&table->lock);
	strcpy(table->spectators[spectator_num], name);
	pthread_rwlock_unlock(&table->lock);
	
	/* If player notification failed, they must've logged out */
	if (msg_status < 0) {
		dbg_msg(GGZ_DBG_TABLE, "%s logged out during spectator join",
			name);
		
		spectator.index = spectator_num;
		spectator.name[0] = '\0';
		spectator.fd = -1;
		
		transit_spectator_event(table->room, table->index, spectator, name);

	} else {
		table_update_event_enqueue(table, GGZ_UPDATE_SPECTATOR_JOIN,
					   name, spectator_num);
		dbg_msg(GGZ_DBG_TABLE, 
			"%s in spectator %d at table %d of room %d",
			name, spectator_num, table->index, table->room);
	}
	
	/* Clear table for next transit */
	pthread_rwlock_wrlock(&table->lock);
	table->transit = 0;
	ggz_free(table->transit_name);
	table->transit_name = NULL;
	pthread_rwlock_unlock(&table->lock);
}


/*
 * table_game_leave handles the RSP_GAME_LEAVE_SPECTATOR from the table
 */
static void table_game_spectator_leave(GGZdMod *ggzdmod, GGZdModEvent event, void *data)
{
	GGZTable* table = ggzdmod_get_gamedata(ggzdmod);
	char* name;
	char status;
	int spectator;

	dbg_msg(GGZ_DBG_TABLE, "Table %d in room %d responded to spectator leave", 
		table->index, table->room);

	if(data) status = *(char*)data;
	else status = 0;

	/* Error: we didn't request a leave! */
	if (!table->transit)
		return ;

	/* Read in saved transit data */
	name = table->transit_name;
	spectator = table->transit_seat;
	
	if (status == 0) {
		/* Vacate seat */
		dbg_msg(GGZ_DBG_TABLE, 
			"%s left spectator %d at table %d of room %d", name, spectator,
			table->index, table->room);

		pthread_rwlock_wrlock(&table->lock);
		table->spectators[spectator][0] = '\0';
		pthread_rwlock_unlock(&table->lock);
		
		table_update_event_enqueue(table, GGZ_UPDATE_SPECTATOR_LEAVE, name,
					    spectator);
	}

	/* Notify player and mark transit as done */
	transit_player_event(name, GGZ_TRANSIT_LEAVE_SPECTATOR, status, 0);

	/* Free strdup'd player name */
	ggz_free(table->transit_name);

	table->transit = 0;
	table->transit_name = NULL;
	table->transit_seat = -1;
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
	int type;
	char *game_name, *msg = data;

	if (log_info.options & GGZ_LOGOPT_INC_GAMETYPE) {
		pthread_rwlock_rdlock(&table->lock);
		type = table->type;
		pthread_rwlock_unlock(&table->lock);
		
		pthread_rwlock_rdlock(&game_types[type].lock);
		game_name = ggz_strdup(game_types[type].name);
		pthread_rwlock_unlock(&game_types[type].lock);
		
		dbg_msg(GGZ_DBG_TABLE, "(%s) %s", game_name, msg);
		
		ggz_free(game_name);
	} else {
		/* It's important that we use the %s so that we can
		   safely log messages containing % and other printf
		   meta-characters. */
		dbg_msg(GGZ_DBG_TABLE, "%s", msg);
	}
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

	/* Send out table-leave messages for remaining players */
	for (i = 0; i < seats_num(table); i++) {
		if (seats_type(table, i) == GGZ_SEAT_PLAYER) {
			table_update_event_enqueue(table, GGZ_UPDATE_LEAVE, 
						   table->seat_names[i], i);
			transit_player_event(table->seat_names[i],
					     GGZ_TRANSIT_LEAVE, 0, 0);
		}
	}

	/* And send them out for spectators also */
	for (i = 0; i < spectator_seats_num(table); i++) {
		if (table->spectators[i][0] != '\0') {
			table_update_event_enqueue(table,
						   GGZ_UPDATE_SPECTATOR_LEAVE,
						   table->spectators[i], i);
			transit_player_event(table->spectators[i],
					     GGZ_TRANSIT_LEAVE_SPECTATOR,
					     0, 0);
		}
	}

	/* This effectively destroys the table, and so should come AFTER the
	   players are removed. */
	pthread_rwlock_wrlock(&state.lock);
	state.tables--;
	pthread_rwlock_unlock(&state.lock);

	pthread_rwlock_wrlock(&table->lock);
	table->type = -1;
	table->state = GGZ_TABLE_ERROR;
	pthread_rwlock_unlock(&table->lock);

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


/* Change table description of running table */
void table_set_desc(GGZTable *table, char *desc)
{
	dbg_msg(GGZ_DBG_TABLE, "Table %p new desc: '%s'", table, desc);
	
        pthread_rwlock_wrlock(&table->lock);
        strcpy(table->desc, desc);
        pthread_rwlock_unlock(&table->lock);
	
        table_event_enqueue(table, GGZ_UPDATE_DESC);
}


/* Kill the table */
int table_kill(int room, int index, char *name)
{
	int status;
	char *data;

	dbg_msg(GGZ_DBG_TABLE, "Kill request for table %d in room %d", index, 
		room);
	
	data = ggz_strdup(name);

	status = event_table_enqueue(room, index, table_kill_callback, 
				     strlen(data)+1, data);
	return status;
}


static GGZEventFuncReturn table_kill_callback(void* target, int size, void* data)
{
	GGZTable *table = target;
	char *caller = data;

	dbg_msg(GGZ_DBG_TABLE, "%s requested death of table %d in room %d",
		caller, table->index, table->room);

	/* FIXME: we should make sure that the caller is either the
           owner or one of the players first */

	table->state = GGZ_TABLE_DONE;
	return GGZ_EVENT_OK;
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
		*my_tables = ggz_malloc(MAX_TABLES * sizeof(GGZTable));
		*indices = ggz_malloc(MAX_TABLES * sizeof(int));
		
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

	*tables = ggz_malloc(t_count * sizeof(GGZTable));

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


/* Find a player at a table */
int table_find_player(int room, int index, char *name)
{
	int i, seats, seat = -1;
	GGZTable *table;

	/* grab handle to table (along with write lock) */
	table = table_lookup(room, index);
	if(table != NULL) {
		seats  = seats_num(table);
		for (i = 0; i < seats; i++)
			if (table->seat_types[i] == GGZ_SEAT_PLAYER
			    && strcasecmp(table->seat_names[i], name) == 0) {
				seat = i;
				break;
			}
		pthread_rwlock_unlock(&table->lock);
	}

	return seat;
}

/* Find a player at a table */
int table_find_spectator(int room, int index, char *name)
{
	int i, spectator = -1;
	GGZTable *table;

	/* grab handle to table (along with write lock) */
	table = table_lookup(room, index);
	if(table != NULL) {
		for (i = 0; i < spectator_seats_num(table); i++)
			if (table->spectators[i]
			    && strcasecmp(table->spectators[i], name) == 0) {
				spectator = i;
				break;
			}
		pthread_rwlock_unlock(&table->lock);
	}

	return spectator;
}


static int table_event_enqueue(GGZTable* table, GGZUpdateOpcode opcode)
{
	void* data = NULL;
	int size, status, room;

	room = table->room;
	
	size = table_pack(&data, opcode, table);

	/* Queue table event for whole room */
	status = event_room_enqueue(room, table_event_callback, size, data);
	
	return status;
}


static int table_update_event_enqueue(GGZTable* table, GGZUpdateOpcode opcode,
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


static int table_seat_event_enqueue(GGZTable *table, GGZUpdateOpcode opcode,
				    unsigned int seat_num)
{
	int status;
	struct GGZSeatChange *data;

	data = ggz_malloc(sizeof(*data));

	/* Copy seat data into structure for passing to event */
	data->seat.index = seat_num;
	data->seat.type = table->seat_types[seat_num];
	strcpy(data->seat.name, table->seat_names[seat_num]);
	data->table = table->index;
	data->num_seats = seats_num(table);

	/* Queue table event for whole room */
	status = event_room_enqueue(table->room, table_seat_event_callback,
				    sizeof(*data), data);
	
	return status;
}

#if 0
static int table_spectator_event_enqueue(GGZTable *table, GGZUpdateOpcode opcode,
	unsigned int spectator_num)
{
	int status;
	struct GGZSpectatorChange *data;

	data = ggz_malloc(sizeof(struct GGZSpectatorChange));

	/* Copy seat data into structure for passing to event */
	data->spectator.index = spectator_num;
	strcpy(data->spectator.name, table->spectators[spectator_num]);
	data->table = table->index;
	data->num_spectators = spectators_count(table);

	/* Queue table event for whole room */
	status = event_room_enqueue(table->room, table_spectator_event_callback, sizeof(data), data);
	
	return status;
}
#endif


static int table_pack(void** data, unsigned char opcode, GGZTable* table)
{
	int size;
	char* current;
	GGZTable info;

	/* Allocate space for opcode and table */
	size = sizeof(char) + sizeof(GGZTable);

	*data = ggz_malloc(size);
	
	pthread_rwlock_rdlock(&table->lock);
	info = *table;
	pthread_rwlock_unlock(&table->lock);
	
	current = (char*)*data;
	
	*(unsigned char*)current = opcode;
	current += sizeof(char);
	
	*(GGZTable*)current = info;
	current += sizeof(GGZTable);
	
	return size;
}


static int table_transit_pack(void** data, unsigned char opcode,
			      GGZTable* table, char* name, unsigned int seat)
{
	int size;
	char* current;
	GGZTable info;

	/* Allocate space for opcode, table, player name, and seat number */
	size = sizeof(char) + sizeof(GGZTable) + strlen(name)+1 + sizeof(int);

	pthread_rwlock_rdlock(&table->lock);
	info = *table;
	pthread_rwlock_unlock(&table->lock);
	
	*data = ggz_malloc(size);
	
	current = (char*)*data;
	
	*(unsigned char*)current = opcode;
	current += sizeof(char);

	*(GGZTable*)current = info;
	current += sizeof(GGZTable);

	strcpy(current, name);
	current += (strlen(name) + 1);

	*(unsigned int*)current = seat;
	current += sizeof(int);

	return size;
}


/* Event callback for delivering table list update to a player */
static GGZEventFuncReturn table_event_callback(void* target, int size,
                                               void* data)
{
	unsigned char opcode;
	char* current;
	GGZTable info;
	GGZPlayer* player;
	GGZTableSeat seat;
	GGZTableSpectator spectator;
	void* update_data = NULL;

	player = (GGZPlayer*)target;

	/* Unpack event data */
	current = (char*)data;
	
	opcode = *(unsigned char*)current;
	current += sizeof(char);
	
	info = *(GGZTable*)current;
	current += sizeof(GGZTable);

	
	switch (opcode) {
	case GGZ_UPDATE_DELETE:
		dbg_msg(GGZ_DBG_UPDATE, "%s sees table %d deleted",
			player->name, info.index);
		break;

	case GGZ_UPDATE_ADD:
		dbg_msg(GGZ_DBG_UPDATE, "%s sees table %d added",
			player->name, info.index);
		break;

	case GGZ_UPDATE_DESC:
		dbg_msg(GGZ_DBG_UPDATE, "%s sees table %d new desc '%s'",
			player->name, info.index, info.desc);
		break;		
		
	case GGZ_UPDATE_STATE:
		dbg_msg(GGZ_DBG_UPDATE, "%s sees table %d new state %d",
			player->name, info.index, info.state);
		break;

	case GGZ_UPDATE_LEAVE:
	case GGZ_UPDATE_JOIN:
		strcpy(seat.name, current);
		current += (strlen(seat.name) + 1);
		seat.index = *(int*)current;
		current += sizeof(int);
		seat.type = GGZ_SEAT_PLAYER;

		update_data = &seat;

		dbg_msg(GGZ_DBG_UPDATE, "%s sees %s %s seat %d at table %d", 
			player->name, seat.name, 
			(opcode == GGZ_UPDATE_JOIN ? "join" : "leave"),
			seat.index, info.index);
		break;

	case GGZ_UPDATE_SPECTATOR_LEAVE:
	case GGZ_UPDATE_SPECTATOR_JOIN:
		strcpy(spectator.name, current);
		current += strlen(spectator.name) + 1;
		spectator.index = *(int*)current;
		current += sizeof(int);

		update_data = &spectator;

		dbg_msg(GGZ_DBG_UPDATE,
			"%s sees %s %s spectator seat %d at table %d",
			player->name, spectator.name,
			opcode == GGZ_UPDATE_SPECTATOR_JOIN ? "join" : "leave",
			spectator.index, info.index);
		break;
	}

	if (net_send_table_update(player->client->net, opcode,
				  &info, update_data) < 0)
		return GGZ_EVENT_ERROR;
	
	return GGZ_EVENT_OK;
}


/* Event callback for delivering table list update to a player */
static GGZEventFuncReturn table_seat_event_callback(void* target, int size,
						    void* data)
{
	GGZTable info;
	GGZPlayer* player = (GGZPlayer*)target;
	struct GGZSeatChange *seat_change = data;
	struct GGZTableSeat *seat = &(seat_change->seat);

	info.index = seat_change->table;
	
	dbg_msg(GGZ_DBG_UPDATE, "%s sees seat %d change to %s (%s) at table %d", 
		player->name, seat->index, ggz_seattype_to_string(seat->type),
		seat->name, info.index);

	if (net_send_table_update(player->client->net, GGZ_UPDATE_SEAT,
				  &info, &seat) < 0)
		return GGZ_EVENT_ERROR;
	
	return GGZ_EVENT_OK;
}

/* Event callback for delivering table list update to a player */
/*static GGZEventFuncReturn table_spectator_event_callback(void* target, int size,
	void* data)
{
	GGZTable info;
	GGZPlayer* player = (GGZPlayer*)target;
	struct GGZSpectatorChange *spectator_change = data;
	struct GGZTableSpectator *spectator = &(spectator_change->spectator);

	info.index = spectator_change->table;
	strcpy(info.spectators[spectator->index], spectator->name);
	
	dbg_msg(GGZ_DBG_UPDATE, "%s sees spectator %d change to 'spectator' (%s) at table %d", 
		player->name, spectator->index, spectator->name, info.index);

	if (net_send_table_update(player->client->net, GGZ_UPDATE_SPECTATOR, &info, spectator->index) < 0)
		return GGZ_EVENT_ERROR;
	
	return GGZ_EVENT_OK;
}*/

static int table_launch_event(char* name, int status, int index)
{
	int size;
	char* current;
	void* data;

	size = sizeof(int);
	
	/* We pass back the table index if launch was successful */
	if (status == 0)
		size += sizeof(int);

	data = ggz_malloc(size);
	
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
void table_free(GGZTable* table)
{
#ifdef UNLIMITED_SEATS
	if (table->num_seats > 0) {
		ggz_free(table->seat_types);
		ggz_free(table->seat_names);
	}
#endif
#ifdef UNLIMITED_SPECTATORS
	if (table->max_num_spectators > 0)
		ggz_free(table->spectators);
#endif

	/* FIXME: do we need to free transit too? */

	ggzdmod_free(table->ggzdmod);
	ggz_free(table);
}


/* FIXME: move to type.c */
int type_match_table(int type, GGZTable* table)
{
	/* FIXME: Do reservation checking properly */
	return ( type == GGZ_TYPE_ALL
		 || (type >= 0 && type == table->type)
		 || (type == GGZ_TYPE_OPEN && seats_count(table, GGZ_SEAT_OPEN))
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
