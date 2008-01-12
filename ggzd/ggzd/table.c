/*
 * File: table.c
 * Author: Brent Hendricks
 * Project: GGZ Server
 * Date: 1/9/00
 * Desc: Functions for handling tables
 * $Id: table.c 9525 2008-01-12 22:03:18Z josef $
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

#ifdef HAVE_CONFIG_H
# include <config.h>		/* Site specific config */
#endif

#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/signal.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include "ggzdmod.h"
#include "ggzdmod-ggz.h"

#include "client.h"
#include "datatypes.h"
#include "err_func.h"
#include "ggzd.h"
#include "ggzdb.h"
#include "hash.h"
#include "net.h"
#include "players.h"
#include "protocols.h"
#include "room.h"
#include "seats.h"
#include "stats.h"
#include "table.h"
#include "transit.h"


pthread_key_t table_key;

/* Packaging for table events */
typedef struct {
	GGZTableUpdateType opcode;
	GGZTable *table;
	/* player and seat are only used for some operations */
	char player[MAX_USER_NAME_LEN + 1];
	int seat;
} GGZTableEventData;

/* Local functions for handling tables */
static GGZTable *table_copy(GGZTable *table);
static GGZReturn table_check(GGZTable* table);
static GGZReturn table_handler_launch(GGZTable* table);
static void* table_new_thread(void *index_ptr);
static GGZReturn table_start_game(GGZTable *table);
static void  table_loop(GGZTable* table);
static void  table_remove(GGZTable* table);

/* Handlers for ggzdmod events */
static void table_handle_state(GGZdMod *mod, GGZdModEvent event,
			       const void *data);
static void table_log(GGZdMod *ggzdmod, GGZdModEvent event, const void *data);
static void table_game_report(GGZdMod *ggzdmod, GGZdModEvent event,
			      const void *data);
static void table_game_savegame(GGZdMod *ggzdmod, GGZdModEvent event,
			      const void *data);
static void table_game_req_num_seats(GGZdMod *ggzdmod, GGZdModEvent event,
				     const void *data);
static void table_game_req_boot(GGZdMod *ggzdmod, GGZdModEvent event,
				const void *data);
static void table_game_req_bot(GGZdMod *ggzdmod, GGZdModEvent event,
			       const void *data);
static void table_game_req_open(GGZdMod *ggzdmod, GGZdModEvent event,
				const void *data);
static void table_error(GGZdMod *ggzdmod, GGZdModEvent event,
			const void *data);

static GGZReturn table_event_enqueue(GGZTable* table,
				     GGZTableUpdateType opcode);
static GGZReturn table_update_event_enqueue(GGZTable* table,
					    GGZTableUpdateType opcode,
					    const char* name,
					    unsigned int seat);

static void table_event_data_free(void* data);
static GGZEventFuncReturn table_event_callback(void* target, size_t size,
					       void* data);
static GGZEventFuncReturn table_kill_callback(void* target, size_t size,
					      void* data);
static GGZReturn table_launch_event(char* name,
				    GGZClientReqError status, int index);

static int   type_match_table(int type, GGZTable* table);



GGZTable* table_new(void)
{
	GGZTable *table;
	
	/* Allocate a new table structure */
	table = ggz_malloc(sizeof(*table));
	
	pthread_rwlock_init(&table->lock, NULL);
	table->type = -1;
	table->room = -1;
	table->index = -1;
	table->state = GGZ_TABLE_CREATED;
	table->ggzdmod = NULL;

	table->num_seats = 0;
	table->seat_types = NULL;
	table->seat_names = NULL;

	table->max_num_spectators = 0;
	table->spectators = NULL;

	table->savegame = NULL;

	return table;
}


/* Copy a table, so that you can read it later.  The new table is not a
   substitute for the old - it is just a snapshot of the state of the
   table.  Thus transit, event, ggzdmod, and lock information on the copy
   cannot be used.  But you can look and see who's at the table, etc.  The
   table may be safely freed with table_free().

   You must have a read lock on the table before calling this function. */
static GGZTable *table_copy(GGZTable *table)
{
	size_t size;
	GGZTable *new_table;

	size = sizeof(GGZTable);
	new_table = ggz_malloc(size);
	memcpy(new_table, table, size);

	/* Some elements need to be copied explicitly. */
	if (new_table->num_seats > 0) {
		size = new_table->num_seats * sizeof(*new_table->seat_names);
		new_table->seat_names = ggz_malloc(size);
		memcpy(new_table->seat_names, table->seat_names, size);

		size = new_table->num_seats * sizeof(*new_table->seat_types);
		new_table->seat_types = ggz_malloc(size);
		memcpy(new_table->seat_types, table->seat_types, size);
	}

	if (new_table->max_num_spectators > 0) {
		size = new_table->max_num_spectators
			* sizeof(*new_table->spectators);
		new_table->spectators = ggz_malloc(size);
		memcpy(new_table->spectators, table->spectators, size);
	} else
		new_table->spectators = NULL;

	/* Some elements just shouldn't be copied. */
	new_table->ggzdmod = NULL;
	new_table->events_head = NULL;
	new_table->events_tail = NULL;
	new_table->savegame = NULL;
	/* What about table->lock? */

	return new_table;
}


/*
 * table_check() - Logs seating assignments to debug and verifies
 * their validity.  Returns GGZ_ERROR if the table is invalid.
 */
static GGZReturn table_check(GGZTable* table)
{
	GGZReturn status = GGZ_OK;
	int ai_total = seats_count(table, GGZ_SEAT_BOT);
	int seat_total = seats_num(table);
	int spectator_total = spectators_count(table);
	int g_type = table->type, room_type, i;
	
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
		return GGZ_ERROR;
	}

	/* FIXME: should technically lock game_types */
	/* Display and verify total seats and bot seats */
	if (ggz_numberlist_isset(&game_types[g_type].player_allow_list,
				  seat_total))
		dbg_msg(GGZ_DBG_TABLE, "Seats  : %d (accept)", seat_total);
	else {
		dbg_msg(GGZ_DBG_TABLE, "Seats  : %d (invalid)", seat_total);
		status = GGZ_ERROR;
	}
	
	if(ai_total == 0
	   || ggz_numberlist_isset(&game_types[g_type].bot_allow_list,
				   ai_total))
		dbg_msg(GGZ_DBG_TABLE, "Bots   : %d (accept)", ai_total);
	else {
		dbg_msg(GGZ_DBG_TABLE, "Bots   : %d (invalid)", ai_total);
		status = GGZ_ERROR;
	}

	if (game_types[g_type].allow_spectators)
		dbg_msg(GGZ_DBG_TABLE, "Spectators: %d", spectator_total);
	else {
		if (spectator_total > 0)
			status = GGZ_ERROR;
		dbg_msg(GGZ_DBG_TABLE, "Spectators not supported.");
	}

	dbg_msg(GGZ_DBG_TABLE, "Open Seats : %d", seats_count(table, GGZ_SEAT_OPEN));
	dbg_msg(GGZ_DBG_TABLE, "Resv.Seats : %d", seats_count(table, GGZ_SEAT_RESERVED));
	dbg_msg(GGZ_DBG_TABLE, "State      : %d", table->state);
	dbg_msg(GGZ_DBG_TABLE, "GGZdMod    : %p", table->ggzdmod);
	
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
			status = GGZ_ERROR;
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
static GGZReturn table_handler_launch(GGZTable* table)
{
	pthread_t thread;
	GGZTable **index_ptr;
	int status;
	
	/* Temporary storage to pass table pointer */
	index_ptr = ggz_malloc(sizeof(*index_ptr));
	*index_ptr = table;
	status = pthread_create(&thread, NULL, table_new_thread, index_ptr);
	if (status != 0) {
		ggz_free(index_ptr);
		return GGZ_ERROR;
	}

	log_create_table();

	return GGZ_OK;
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
	ggz_intlstring *rname;
	char *gname;

	table = *((GGZTable**)index_ptr);
	ggz_free(index_ptr);

	table->thread = pthread_self();
	pthread_setspecific(table_key, table);

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
	rname = ggz_intlstring_fromintlstring(rooms[table->room].name);
	if (rooms[table->room].table_count == rooms[table->room].max_tables) {
		pthread_rwlock_unlock(&rooms[table->room].lock);
		log_msg(GGZ_LOG_NOTICE,
			"ROOM_FULL - %s could not create table in %s",
			table->owner, ggz_intlstring_translated(rname, NULL));
		ggz_intlstring_free(rname);
		table_launch_event(table->owner, E_ROOM_FULL, 0);
		return NULL;
	}
	count = ++rooms[table->room].table_count;
	/* Find empty spot in room */ 
	/* FIXME - there are problems with table 0 locking up, probably
	 * due to something being incorrectly initialised somewhere.
	 * A 1 based index is better anyway.and will hopefully help us
	 * find the problem more easily. Roger 20061230 */
	for (i = 1; i < rooms[table->room].max_tables; i++)
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
	gname = ggz_strdup(ggz_intlstring_translated(game_types[table->type].name, NULL));
	pthread_rwlock_unlock(&game_types[table->type].lock);

	/* Let the game begin...*/
	if (table_start_game(table) == GGZ_OK) {
		log_msg(GGZ_LOG_TABLES,
			"TABLE_START - %s started a new game of %s in %s",
			table->owner, gname, ggz_intlstring_translated(rname, NULL));
		ggz_intlstring_free(rname);

		table_loop(table);

		log_msg(GGZ_LOG_TABLES,
			"TABLE_END - Game of %s started by %s has ended",
			gname, table->owner);
		ggz_free(gname);
	}
	else {
		dbg_msg(GGZ_DBG_TABLE, "Table %d failed to start game module", 
			table->index);
		/* ggzdmod already does that */
		/*table_launch_event(table->owner, E_LAUNCH_FAIL, 0);*/
	}
	
	table_remove(table);
	table_free(table);

	return NULL;
}


/* Create and start game module */
/* FIXME: this function needs better error handling. */
static GGZReturn table_start_game(GGZTable *table)
{
#if 0
	char *args[] = {"logmod", NULL};
#endif
	char **args;
	char *pwd;
	char *game;
	int type, i, num_seats;
	GGZReturn status = GGZ_OK;
	GGZSeat seat;

	pthread_rwlock_wrlock(&table->lock);
	table->ggzdmod = ggzdmod_new(GGZDMOD_GGZ);
	pthread_rwlock_unlock(&table->lock);	

	/* Some weird error in creating the module */
	if (!table->ggzdmod)
		return GGZ_ERROR;

	/* Build our argument list */
	type = table->type;
	pthread_rwlock_rdlock(&game_types[type].lock);
	pwd = game_types[type].data_dir;
	args = rooms[table->room].exec_args;
	game = game_types[type].game;
	pthread_rwlock_unlock(&game_types[type].lock);

	/* Set a pointer to the table so we can get it back in the
           event handlers */
	ggzdmod_set_gamedata(table->ggzdmod, table);

	/* Setup handlers for game module events */
	ggzdmod_set_handler(table->ggzdmod, GGZDMOD_EVENT_STATE, &table_handle_state);
	ggzdmod_set_handler(table->ggzdmod, GGZDMOD_EVENT_LOG, &table_log);
	ggzdmod_set_handler(table->ggzdmod, GGZDMOD_EVENT_GAMEREPORT,
			    &table_game_report);
	ggzdmod_set_handler(table->ggzdmod, GGZDMOD_EVENT_SAVEGAMEREPORT,
			    &table_game_savegame);
	ggzdmod_set_handler(table->ggzdmod, GGZDMOD_EVENT_REQ_NUM_SEATS,
			    &table_game_req_num_seats);
	ggzdmod_set_handler(table->ggzdmod, GGZDMOD_EVENT_REQ_BOOT,
			    &table_game_req_boot);
	ggzdmod_set_handler(table->ggzdmod, GGZDMOD_EVENT_REQ_BOT,
			    &table_game_req_bot);
	ggzdmod_set_handler(table->ggzdmod, GGZDMOD_EVENT_REQ_OPEN,
			    &table_game_req_open);
	ggzdmod_set_handler(table->ggzdmod, GGZDMOD_EVENT_ERROR, &table_error);
	
	/* Setup seats for game table */
	num_seats = seats_num(table);
	ggzdmod_set_num_seats(table->ggzdmod, num_seats);
	for (i = 0; i < num_seats; i++) {
		seat.num = i;
		seat.type = seats_type(table, i);
		seat.playerdata = NULL;
		if (seat.type == GGZ_SEAT_RESERVED
		    || seat.type == GGZ_SEAT_BOT) {
			seat.name = table->seat_names[i];
		} else {
			seat.name = NULL;
		}
		seat.fd = -1;
		if (ggzdmod_set_seat(table->ggzdmod, &seat) < 0)
			status = GGZ_ERROR;
	}

	/* And start the game */
	log_msg(GGZ_LOG_TABLES, "Launching table: %s", args[0]);
	ggzdmod_set_module(table->ggzdmod, game, pwd, args);
	if (ggzdmod_connect(table->ggzdmod) < 0)
		status = GGZ_ERROR;

	return status;
}


static void table_loop(GGZTable* table)
{
	int fd, status;
	fd_set active_fd_set, read_fd_set;
	sigset_t set;
	
	fd = ggzdmod_get_fd(table->ggzdmod);
	FD_ZERO(&active_fd_set);
	FD_SET(fd, &active_fd_set);

	sigemptyset(&set);
	sigaddset(&set, TABLE_EVENT_SIGNAL);

	while (table->state != GGZ_TABLE_DONE && !table->error) {
		read_fd_set = active_fd_set;
		status = select((fd + 1), &read_fd_set, NULL, NULL, NULL);
		if (status <= 0) {
			if (status == 0 || errno == EINTR)
				continue;
			else
				err_sys_exit("select error in table_loop()");
		}

		pthread_sigmask(SIG_BLOCK, &set, NULL);
		if (ggzdmod_dispatch(table->ggzdmod) < 0)
			break;
		pthread_sigmask(SIG_UNBLOCK, &set, NULL);

		pthread_kill(pthread_self(), TABLE_EVENT_SIGNAL);
	}

	/* Don't want signals interrupting after this point */
	pthread_sigmask(SIG_BLOCK, &set, NULL);

	dbg_msg(GGZ_DBG_TABLE, "Table %d in room %d loop completed", 
		table->index, table->room);
}


/*
 * table_game_join handles the RSP_GAME_JOIN from the table
 * Note: table->transit_name contains allocated mem on entry
 */
void table_game_join(GGZTable *table, const char *name,
		     GGZJoinType reason, int num)
{
#if 0
	int i;
#endif

	dbg_msg(GGZ_DBG_TABLE, "Table %d in room %d responded to join", 
		table->index, table->room);

	/* Transit successful: Assign seat */
	pthread_rwlock_wrlock(&table->lock);
	table->seat_types[num] = GGZ_SEAT_PLAYER;
	strcpy(table->seat_names[num], name);
	pthread_rwlock_unlock(&table->lock);

	if (transit_player_event(name, GGZ_TRANSIT_JOIN,
				 reason == GGZ_JOIN_REQUEST ? E_OK
				 : E_NO_STATUS,
				 name, reason, table->index) != GGZ_OK) {
		/* If player notification failed, they must've logged out.
		   We have to get them out of the table, but since they've
		   already joined we have to do this the long way. */
		struct GGZTableSeat seat = {.index = num,
					    .type = GGZ_SEAT_OPEN,
					    .name = "",
					    .fd = -1};

		dbg_msg(GGZ_DBG_TABLE, "%s logged out during join", name);
		transit_seat_event(table->room, table->index,
				   GGZ_TRANSIT_LEAVE, seat, name,
				   GGZ_LEAVE_GAMEERROR);
		return;
	}

	/* Notify everyone in the room about the change. */
	table_update_event_enqueue(table, GGZ_TABLE_UPDATE_JOIN, 
				   name, num);

#if 0
	/* This is probably better handled by limiting the number of
	   reservations to one per person on table launch.  But that
	   won't help in the case where a player watches a table, then
	   sits down in a seat other than what was reserved for them.  It
	   may still be necessary to always double-check. */
	/* Remove any seat reservations this player may have. */
	for (i = 0; i < table->num_seats; i++) {
		if ((table->seat_types[i] == GGZ_SEAT_RESERVED
		     || table->seat_types[i] == GGZ_SEAT_ABANDONED)
		    && !strcasecmp(table->seat_names[i], name)) {
			pthread_rwlock_wrlock(&table->lock);
			table->seat_types[i] = GGZ_SEAT_OPEN;
			table->seat_names[i][0] = '\0';
			pthread_rwlock_unlock(&table->lock);
			table_update_event_enqueue(table,
						   GGZ_TABLE_UPDATE_SEAT,
						   "", i);
		}
	}
#endif
	log_msg(GGZ_LOG_TABLES,
		"TABLE_JOIN - %s joined seat %d at table %d of room %s in game of %s",
		name, num, table->index,
		ggz_intlstring_translated(rooms[table->room].name, NULL),
		ggz_intlstring_translated(game_types[rooms[table->room].game_type].name, NULL));
}


/*
 * table_game_leave handles the RSP_GAME_LEAVE from the table
 */
void table_game_leave(GGZTable *table, const char *caller,
		      GGZLeaveType reason, int num)
{
	bool empty;
	char player[MAX_USER_NAME_LEN + 1];
	strcpy(player, table->seat_names[num]);

	dbg_msg(GGZ_DBG_TABLE, "Table %d in room %d responded to leave", 
		table->index, table->room);

	/* Vacate seat */
	log_msg(GGZ_LOG_TABLES,
		"TABLE_LEAVE - %s left seat %d at table %d of room %s in game of %s",
		player, num, table->index,
		ggz_intlstring_translated(rooms[table->room].name, NULL),
		ggz_intlstring_translated(game_types[rooms[table->room].game_type].name, NULL));
	
	pthread_rwlock_wrlock(&table->lock);
	table->seat_types[num] = GGZ_SEAT_OPEN;
	table->seat_names[num][0] = '\0';
	pthread_rwlock_unlock(&table->lock);

	empty = (seats_count(table, GGZ_SEAT_PLAYER) == 0);
	if (empty)
		dbg_msg(GGZ_DBG_TABLE, "Table %d in room %d now empty",
			table->index, table->room);

	/* Notify player.  We don't care if this fails; that just means
	   the player logged out in the meantime. */
	transit_player_event(player, GGZ_TRANSIT_LEAVE, E_OK,
			     caller, reason, 0);

	/* Notify everyone in the room about the change. */
	table_update_event_enqueue(table, GGZ_TABLE_UPDATE_LEAVE,
				   player, num);

	/* If the game has set the KillWhenEmpty option, we kill it
	   when the last player leaves.  If not, we rely on the game
	   to halt itself. */
	if (empty && game_types[table->type].kill_when_empty)
		(void)ggzdmod_disconnect(table->ggzdmod);
}


void table_game_reseat(GGZTable *table,
		       GGZReseatType op,
		       const char *name,
		       int old_seat, int new_seat)
{
	int empty = 0;
	GGZTransitType transit = GGZ_TRANSIT_SIT;

	dbg_msg(GGZ_DBG_TABLE, "Table %d in room %d reseat %d: %d->%d",
		table->index, table->room, op, old_seat, new_seat);

	switch (op) {
	case GGZ_RESEAT_SIT:
		pthread_rwlock_wrlock(&table->lock);
		table->spectators[old_seat][0] = '\0';
		table->seat_types[new_seat] = GGZ_SEAT_PLAYER;
		strcpy(table->seat_names[new_seat], name);
		pthread_rwlock_unlock(&table->lock);
		transit = GGZ_TRANSIT_SIT;
		break;
	case GGZ_RESEAT_STAND:
		pthread_rwlock_wrlock(&table->lock);
		table->seat_types[old_seat] = GGZ_SEAT_OPEN;
		table->seat_names[old_seat][0] = '\0';
		strcpy(table->spectators[new_seat], name);
		pthread_rwlock_unlock(&table->lock);	
		empty = (seats_count(table, GGZ_SEAT_PLAYER) == 0);
		transit = GGZ_TRANSIT_STAND;
		break;
	case GGZ_RESEAT_MOVE:
		pthread_rwlock_wrlock(&table->lock);
		table->seat_types[old_seat] = GGZ_SEAT_OPEN;
		table->seat_names[old_seat][0] = '\0';
		table->seat_types[new_seat] = GGZ_SEAT_PLAYER;
		strcpy(table->seat_names[new_seat], name);
		pthread_rwlock_unlock(&table->lock);
		transit = GGZ_TRANSIT_MOVE;	
		break;
	}

	if (empty)
		dbg_msg(GGZ_DBG_TABLE, "Table %d in room %d now empty",
			table->index, table->room);

	if (transit_player_event(name, transit, E_OK,
				 name, -1, table->index) != GGZ_OK) {
		/* If player notification failed, they must've logged out.
		   We have to get them out of the table, but since they've
		   already joined we have to do this the long way. */
		struct GGZTableSeat seat = {.index = new_seat,
					    .type = GGZ_SEAT_OPEN,
					    .name = "",
					    .fd = -1};

		if (op == GGZ_RESEAT_STAND)
			transit = GGZ_TRANSIT_LEAVE_SPECTATOR;
		else
			transit = GGZ_TRANSIT_LEAVE;

		dbg_msg(GGZ_DBG_TABLE, "%s logged out during reseat", name);
		transit_seat_event(table->room, table->index,
				   transit, seat, name,
				   GGZ_LEAVE_GAMEERROR);
		return;
	}

	/* FIXME: send RESEAT update instead */
	if (op == GGZ_RESEAT_SIT) {
		table_update_event_enqueue(table,
					   GGZ_TABLE_UPDATE_SPECTATOR_LEAVE,
					   name, old_seat);
	} else {
		table_update_event_enqueue(table,
					   GGZ_TABLE_UPDATE_LEAVE,
					   name, old_seat);
	}

	if (op == GGZ_RESEAT_STAND) {
		table_update_event_enqueue(table,
					   GGZ_TABLE_UPDATE_SPECTATOR_JOIN,
					   name, new_seat);
	} else {
		table_update_event_enqueue(table,
					   GGZ_TABLE_UPDATE_JOIN,
					   name, new_seat);
	}
}


void table_game_seatchange(GGZTable *table, GGZSeatType type, int num)
{
	dbg_msg(GGZ_DBG_TABLE,
		"Seat %d changes to %s at table %d of room %d",
		num, ggz_seattype_to_string(type),
		table->index, table->room);

	pthread_rwlock_wrlock(&table->lock);
	table->seat_types[num] = type;
	pthread_rwlock_unlock(&table->lock);

	table_update_event_enqueue(table, GGZ_TABLE_UPDATE_SEAT, "", num);
}


/*
 * table_game_spectator_join handles the RSP_GAME_JOIN_SPECTATOR from the table
 * Note: table->transit_name contains allocated mem on entry
 */
void table_game_spectator_join(GGZTable *table, const char *name,
			       GGZJoinType reason, int num)
{
	dbg_msg(GGZ_DBG_TABLE,
		"Table %d in room %d responded to spectator join", 
		table->index, table->room);

	/* Transit successful: Assign seat */
	pthread_rwlock_wrlock(&table->lock);
	strcpy(table->spectators[num], name);
	pthread_rwlock_unlock(&table->lock);

	if (transit_player_event(name, GGZ_TRANSIT_JOIN_SPECTATOR,
				 E_OK, name, reason,
				 table->index) != GGZ_OK) {
		/* If player notification failed, they must've logged out.
		   We have to get them out of the table, but since they've
		   already joined we have to do this the long way. */
		struct GGZTableSeat seat = {.index = num,
					    .name = "",
					    .fd = -1};

		dbg_msg(GGZ_DBG_TABLE, "%s logged out during spectator join",
			name);

		transit_seat_event(table->room, table->index,
				   GGZ_TRANSIT_LEAVE_SPECTATOR,
				   seat, name, GGZ_LEAVE_GAMEERROR);
		return;
	}

	/* Notify everyone in the room about the change. */
	table_update_event_enqueue(table, GGZ_TABLE_UPDATE_SPECTATOR_JOIN,
				   name, num);
	dbg_msg(GGZ_DBG_TABLE, 
		"%s in spectator %d at table %d of room %d",
		name, num, table->index, table->room);
}


/*
 * table_game_leave handles the RSP_GAME_LEAVE_SPECTATOR from the table
 */
void table_game_spectator_leave(GGZTable *table, const char *caller,
				GGZLeaveType reason, int num)
{
	char player[MAX_USER_NAME_LEN + 1];
	strcpy(player, table->spectators[num]);

	dbg_msg(GGZ_DBG_TABLE,
		"Table %d in room %d responded to spectator leave", 
		table->index, table->room);

	/* Vacate seat */
	dbg_msg(GGZ_DBG_TABLE, 
		"%s left spectator %d at table %d of room %d",
		player, num,
		table->index, table->room);

	pthread_rwlock_wrlock(&table->lock);
	table->spectators[num][0] = '\0';
	pthread_rwlock_unlock(&table->lock);

	/* Notify player.  We don't care if this fails; that just means
	   the player logged out in the meantime. */
	transit_player_event(player, GGZ_TRANSIT_LEAVE_SPECTATOR,
			     E_OK, caller, reason, table->index);

	/* Notify everyone in the room about the change. */
	table_update_event_enqueue(table, GGZ_TABLE_UPDATE_SPECTATOR_LEAVE,
				   player, num);
}


static void table_handle_state(GGZdMod *mod, GGZdModEvent event,
			       const void *data)
{
	GGZTable* table = ggzdmod_get_gamedata(mod);
#if 0
	const GGZdModState *prev = data;
#endif
	GGZdModState cur = ggzdmod_get_state(mod);
	GGZTableState origstate;
	GGZTableState ggz_cur = table->state;

	switch (cur) {
	case GGZDMOD_STATE_WAITING:
		dbg_msg(GGZ_DBG_TABLE, "Table %d in room %d now waiting", 
			table->index, table->room);

		/* FIXME: maybe we shouldn't keep track of table state
                   here any more? */
		pthread_rwlock_wrlock(&table->lock);
		table->state = GGZ_TABLE_WAITING;
		pthread_rwlock_unlock(&table->lock);

		if (ggz_cur == GGZ_TABLE_CREATED) {
			/* Signal owner that all is good */
			table_launch_event(table->owner, E_OK, table->index);
	
			/* Trigger a table update in this room */
			table_event_enqueue(table, GGZ_TABLE_UPDATE_ADD);
		}
		return;
		
	case GGZDMOD_STATE_PLAYING:
		pthread_rwlock_wrlock(&table->lock);
		table->state = GGZ_TABLE_PLAYING;
		pthread_rwlock_unlock(&table->lock);

		table_event_enqueue(table, GGZ_TABLE_UPDATE_STATE);
		dbg_msg(GGZ_DBG_TABLE, "Table %d in room %d now full/playing",
			table->index, table->room);
		return;

	case GGZDMOD_STATE_DONE:
		dbg_msg(GGZ_DBG_TABLE,
			"Table %d in room %d game module is done", 
			table->index, table->room);

		/* Mark table as done, so people don't attempt transits */
		pthread_rwlock_wrlock(&table->lock);
		origstate = table->state;
		if (origstate != GGZ_TABLE_CREATED)
			table->state = GGZ_TABLE_DONE;
		pthread_rwlock_unlock(&table->lock);
		if (origstate != GGZ_TABLE_CREATED)
			table_event_enqueue(table, GGZ_TABLE_UPDATE_STATE);
		return;

	case GGZDMOD_STATE_CREATED:
		break;
	}

	/* It's an error if we reach here. */
	err_msg("Table %d in room %d changed to invalid state %d.",
		table->index, table->room, cur);
}


static void table_log(GGZdMod *ggzdmod, GGZdModEvent event, const void *data)
{
	GGZTable* table = ggzdmod_get_gamedata(ggzdmod);
	int type;
	const char *game_name, *msg = data;

	if (log_info.options & GGZ_LOGOPT_INC_GAMETYPE) {
		pthread_rwlock_rdlock(&table->lock);
		type = table->type;
		pthread_rwlock_unlock(&table->lock);
		
		pthread_rwlock_rdlock(&game_types[type].lock);
		game_name = ggz_strdup(ggz_intlstring_translated(game_types[type].name, NULL));
		pthread_rwlock_unlock(&game_types[type].lock);
		
		dbg_msg(GGZ_DBG_GAME_MSG, "(%s) %s", game_name, msg);
		
		ggz_free(game_name);
	} else {
		/* It's important that we use the %s so that we can
		   safely log messages containing % and other printf
		   meta-characters. */
		dbg_msg(GGZ_DBG_GAME_MSG, "%s", msg);
	}
}


static void table_game_report(GGZdMod *ggzdmod,
			      GGZdModEvent event, const void *data)
{
	GGZTable *table = ggzdmod_get_gamedata(ggzdmod);
	const GGZdModGameReportData *report = data;

	report_statistics(table->room, table->type, report, table->savegame);

	/* FIXME: we might want to auto-send the updated rankings here? */
}


static void table_game_savegame(GGZdMod *ggzdmod,
			      GGZdModEvent event, const void *data)
{
	GGZTable *table = ggzdmod_get_gamedata(ggzdmod);

	if (table->savegame)
		ggz_free(table->savegame);
	table->savegame = ggz_strdup((char*)data);

	report_savegame(table->type, table->owner, table->savegame);
}


/* Must be called with a write-lock on the table. */
static void table_change_num_seats(GGZTable *table, int num_seats)
{
	table->num_seats = num_seats;
	table->seat_types = ggz_realloc(table->seat_types, num_seats
					* sizeof(*table->seat_types));
	table->seat_names = ggz_realloc(table->seat_names, num_seats
					* sizeof(*table->seat_names));
}


static void table_game_req_num_seats(GGZdMod *ggzdmod, GGZdModEvent event,
				     const void *data)
{
	GGZTable *table = ggzdmod_get_gamedata(ggzdmod);	
	const int *num_seats_ptr = data;
	const int num_seats = *num_seats_ptr;
	int old_seats = table->num_seats;
	int seat;

	if (num_seats <= 0) {
		err_msg("table_game_req_num_seats: table in room %d "
			"requested %d seats.",
			table->room, num_seats);
		return;
	}

	if (num_seats > old_seats) {
		/* We're increasing the number of seats. */

		pthread_rwlock_wrlock(&table->lock);
		table_change_num_seats(table, num_seats);
		for (seat = old_seats; seat < num_seats; seat++) {
			table->seat_types[seat] = GGZ_SEAT_OPEN;
			table->seat_names[seat][0] = '\0';
		}
		pthread_rwlock_unlock(&table->lock);


	} else if (num_seats < old_seats) {
		/* Remove the extra seats; booting anyone from them. */
		for (seat = num_seats; seat < old_seats; seat++) {
			if (table->seat_types[seat] != GGZ_SEAT_PLAYER)
				continue;
			/* Boot the player from the seat. */
			/* FIXME: would it be better to move them to a
			   lower-numbered seat?  Should we provide a
			   mechanism for the game to reseat players to
			   let them take care of it? */
			
			/* Notify player.  We don't care if this fails. */
			/* FIXME: better notification (don't just say
			   "[server]") */
			transit_player_event(table->seat_names[seat],
					     GGZ_TRANSIT_LEAVE,
					     E_NO_STATUS, "[server]",
					     GGZ_LEAVE_BOOT, 0);
		}
		pthread_rwlock_wrlock(&table->lock);
		table_change_num_seats(table, num_seats);
		pthread_rwlock_unlock(&table->lock);
	}

	/* FIXME: there's a bug whereby if a table requests a resize
	   immediately after launch, the resize will be reported to the
	   clients before the launch is.  This is quite bad. */
	table_update_event_enqueue(table, GGZ_TABLE_UPDATE_RESIZE,
				   NULL, 0);
}


static void table_game_req_boot(GGZdMod *ggzdmod,
				GGZdModEvent event, const void *data)
{
	GGZTable *table = ggzdmod_get_gamedata(ggzdmod);
	const char *name = data;
	int seat_num;
	bool is_spectator = false, found = true;
	int len = strlen(name);
	GGZTransitType transit;
	GGZTableUpdateType update;

	if (len == 0 || len > MAX_USER_NAME_LEN) {
		err_msg("table_game_req_boot: invalid name %s in room %d.",
			name, table->room);
		return;
	}

	/* FIXME: this code overlaps with the leaving code in transit.c */

	/* Find the player. */
	for (seat_num = 0; seat_num < table->num_seats; seat_num++) {
		if (table->seat_types[seat_num] == GGZ_SEAT_PLAYER
		    && strcasecmp(name, table->seat_names[seat_num]) == 0)
			break;
	}
	if (seat_num == table->num_seats) {
		is_spectator = true;
		for (seat_num = 0; seat_num < table->max_num_spectators;
		     seat_num++) {
			if (strcasecmp(table->spectators[seat_num], name) == 0)
				break;
		}
		if (seat_num == table->max_num_spectators)
			found = false;
	}

	/* Update the seat. */
	if (!found) {
		/* FIXME: send error to game... */
		return;
	} else if (is_spectator) {
		GGZSpectator seat = {.num = seat_num,
				     .name = NULL,
				     .type = GGZ_SEAT_NONE,
				     .fd = -1,
				     .playerdata = NULL};
		dbg_msg(GGZ_DBG_TABLE,
			"Table %d/%d: server is booting spectator %s.",
			table->index, table->room, name);
		ggzdmod_set_spectator(ggzdmod, &seat);
		transit = GGZ_TRANSIT_LEAVE_SPECTATOR;
		update = GGZ_TABLE_UPDATE_SPECTATOR_LEAVE;
	} else {
		GGZSeat seat = {.num = seat_num,
				.name = NULL,
				.type = GGZ_SEAT_OPEN,
				.fd = -1,
				.playerdata = NULL};
		dbg_msg(GGZ_DBG_TABLE,
			"Table %d/%d: server is booting seat %s.",
			table->index, table->room, name);
		ggzdmod_set_seat(ggzdmod, &seat);
		transit = GGZ_TRANSIT_LEAVE;
		update = GGZ_TABLE_UPDATE_LEAVE;
	}

	pthread_rwlock_wrlock(&table->lock);
	if (is_spectator) {
		table->spectators[seat_num][0] = '\0';
	} else {
		table->seat_types[seat_num] = GGZ_SEAT_OPEN;
		table->seat_names[seat_num][0] = '\0';
	}
	pthread_rwlock_unlock(&table->lock);

	/* Notify player.  We don't care if this fails. */
	/* FIXME: better notification (don't just say "[server]") */
	transit_player_event(name, transit, E_NO_STATUS,
			     "[server]", GGZ_LEAVE_BOOT, 0);

	/* FIXME: there's a bug whereby if a table requests a boot
	   immediately after launch (i.e. tries to boot the launching
	   player), the boot will be reported to the clients before the
	   launch is.  This is quite bad. */
	table_update_event_enqueue(table, update, name, seat_num);
}


static void table_game_req_bot(GGZdMod *ggzdmod,
			       GGZdModEvent event, const void *data)
{
	GGZTable *table = ggzdmod_get_gamedata(ggzdmod);
	const int *seat_num_ptr = data;
	const int seat_num = *seat_num_ptr;
	GGZSeat seat;

	/* FIXME: this code overlaps with the leaving code in transit.c */

	if (seat_num < 0 || seat_num >= table->num_seats
	    || table->seat_types[seat_num] != GGZ_SEAT_OPEN) {
		/* Actually, this might not be an error - the seat might
		   have just changed while the request was in transit */
		err_msg("table_game_req_bot: seat %d requested bot in"
			"table %d, room %d", seat_num, table->index,
			table->room);
		return;
	}

	seat.num = seat_num;
	seat.type = GGZ_SEAT_BOT;
	seat.name = NULL;
	seat.fd = -1;
	seat.playerdata = NULL;
	ggzdmod_set_seat(ggzdmod, &seat);

	pthread_rwlock_wrlock(&table->lock);
	table->seat_types[seat_num] = GGZ_SEAT_BOT;
	pthread_rwlock_unlock(&table->lock);

	/* FIXME: there's a bug whereby if a table requests a bot
	   immediately after launch, the update will be reported to the
	   clients before the launch is.  This is quite bad. */
	table_update_event_enqueue(table, GGZ_TABLE_UPDATE_SEAT,
				   "", seat_num);
}


static void table_game_req_open(GGZdMod *ggzdmod,
				GGZdModEvent event, const void *data)
{
	GGZTable *table = ggzdmod_get_gamedata(ggzdmod);
	const int *seat_num_ptr = data;
	const int seat_num = *seat_num_ptr;
	GGZSeat seat;

	/* FIXME: this code overlaps with the leaving code in transit.c */

	if (seat_num < 0 || seat_num >= table->num_seats
	    || (table->seat_types[seat_num] != GGZ_SEAT_BOT
		&& table->seat_types[seat_num] != GGZ_SEAT_RESERVED)) {
		/* Actually, this might not be an error - the seat might
		   have just changed while the request was in transit */
		err_msg("table_game_req_bot: seat %d requested open in"
			"table %d, room %d", seat_num, table->index,
			table->room);
		return;
	}

	seat.num = seat_num;
	seat.type = GGZ_SEAT_OPEN;
	seat.name = NULL;
	seat.fd = -1;
	seat.playerdata = NULL;
	ggzdmod_set_seat(ggzdmod, &seat);

	pthread_rwlock_wrlock(&table->lock);
	table->seat_types[seat_num] = GGZ_SEAT_OPEN;
	table->seat_names[seat_num][0] = '\0';
	pthread_rwlock_unlock(&table->lock);

	/* FIXME: there's a bug whereby if a table requests an opening
	   immediately after launch, the update will be reported to the
	   clients before the launch is.  This is quite bad. */
	table_update_event_enqueue(table, GGZ_TABLE_UPDATE_SEAT,
				   "", seat_num);
}


static void table_error(GGZdMod *ggzdmod,
			GGZdModEvent event, const void *data)
{
	GGZTable* table = ggzdmod_get_gamedata(ggzdmod);
	
	dbg_msg(GGZ_DBG_TABLE, "GAME ERROR: %s", (char*)data);

	/* If we never made out of CREATED state, it's a launch error */
	if (ggzdmod_get_state(ggzdmod) == GGZDMOD_STATE_CREATED)
		table_launch_event(table->owner, E_LAUNCH_FAIL, 0);

	table->error = 1;

	/* Perhaps we should just set the state to done? */
	(void) ggzdmod_disconnect(ggzdmod);
}



static void table_remove(GGZTable* table)
{
	int room, count, index, i;
	GGZLeaveType reason;
	GGZTableState origstate;

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

	if (table->error)
		reason = GGZ_LEAVE_GAMEERROR;
	else
		reason = GGZ_LEAVE_GAMEOVER;

	/* Send out table-leave messages for remaining players */
	for (i = 0; i < seats_num(table); i++) {
		if (seats_type(table, i) == GGZ_SEAT_PLAYER) {
			table_update_event_enqueue(table,
						   GGZ_TABLE_UPDATE_LEAVE, 
						   table->seat_names[i], i);
			transit_player_event(table->seat_names[i],
					     GGZ_TRANSIT_LEAVE, E_OK,
					     table->seat_names[i],
					     reason, index);
		}
	}

	/* And send them out for spectators also */
	for (i = 0; i < spectator_seats_num(table); i++) {
		if (table->spectators[i][0] != '\0') {
			table_update_event_enqueue(table,
				GGZ_TABLE_UPDATE_SPECTATOR_LEAVE,
				table->spectators[i], i);
			transit_player_event(table->spectators[i],
					     GGZ_TRANSIT_LEAVE_SPECTATOR,
					     E_OK, table->seat_names[i],
					     reason, index);
		}
	}

	/* This effectively destroys the table, and so should come AFTER the
	   players are removed. */
	pthread_rwlock_wrlock(&state.lock);
	state.tables--;
	pthread_rwlock_unlock(&state.lock);

	pthread_rwlock_wrlock(&table->lock);
	table->type = -1;
	origstate = table->state;
	table->state = GGZ_TABLE_ERROR;
	pthread_rwlock_unlock(&table->lock);

	if (origstate != GGZ_TABLE_CREATED)
		table_event_enqueue(table, GGZ_TABLE_UPDATE_DELETE);

	/* Process any remaining events */
	event_table_handle(table);

	log_close_table();
}


/* FIXME: redo using events to notify player */
GGZClientReqError table_launch(GGZTable *table, const char *name)
{
	/* Fill in the table owner */
	strcpy(table->owner, name);	
	
	/* Check validity of table info */
	if (table_check(table) != GGZ_OK) {
		table_free(table);
		return E_BAD_OPTIONS;
	}

	/* Attempt to do launch of table-controller */
	if (table_handler_launch(table) != GGZ_OK) {
		table_free(table);
		return E_LAUNCH_FAIL;
	}
	
	return E_OK;
}


/* Change table description of running table */
void table_set_desc(GGZTable *table, const char *desc)
{
	dbg_msg(GGZ_DBG_TABLE, "Table %p new desc: '%s'", table, desc);
	
        pthread_rwlock_wrlock(&table->lock);
        strcpy(table->desc, desc);
        pthread_rwlock_unlock(&table->lock);
	
        table_event_enqueue(table, GGZ_TABLE_UPDATE_DESC);
}


/* Kill the table */
GGZClientReqError table_kill(int room, int index, const char *name)
{
	char *data;

	dbg_msg(GGZ_DBG_TABLE, "Kill request for table %d in room %d", index, 
		room);
	
	data = ggz_strdup(name);

	if (event_table_enqueue(room, index, table_kill_callback, 
				strlen(data)+1, data, NULL) != GGZ_OK)
		return E_NO_TABLE;

	return E_OK;
}


static GGZEventFuncReturn table_kill_callback(void* target, size_t size,
					      void* data)
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
int table_search(const char *name, int room, int type, bool global, 
		 GGZTable*** tables)
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

	*tables = ggz_malloc(t_count * sizeof(GGZTable*));

	/* Copy the tables we want */
	if (type == -1) {
		for (i = 0; (i < max && count < t_count); i++) {
			if ( (t = rooms[room].tables[i])) {
				pthread_rwlock_rdlock(&t->lock);
				(*tables)[count++] = table_copy(t);
				pthread_rwlock_unlock(&t->lock);
			}
		}
	} else {
		for (i = 0; (i < max && count < t_count); i++) {
			if ( (t = rooms[room].tables[i]) 
			     && type_match_table(type, t)) {
				pthread_rwlock_rdlock(&t->lock);
				(*tables)[count++] = table_copy(t);
				pthread_rwlock_unlock(&t->lock);
			}
		}
	}

	pthread_rwlock_unlock(&rooms[room].lock);

	return count;
}


/* Find a player at a table */
int table_find_player(int room, int index, const char *name)
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
int table_find_spectator(int room, int index, const char *name)
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


static GGZReturn table_event_enqueue(GGZTable* table,
				     GGZTableUpdateType opcode)
{
	int room = table->room;
	GGZTableEventData *data = ggz_malloc(sizeof(*data));

	/* Pack data */
	data->opcode = opcode;
	pthread_rwlock_rdlock(&table->lock);
	data->table = table_copy(table);
	pthread_rwlock_unlock(&table->lock);
	
	/* Queue table event for whole room */
	return event_room_enqueue(room, table_event_callback,
				  sizeof(*data), data,
				  table_event_data_free);
}


static GGZReturn table_update_event_enqueue(GGZTable* table,
					    GGZTableUpdateType opcode,
					    const char* name,
					    unsigned int seat)
{
	int room = table->room;
	GGZTableEventData *data = ggz_malloc(sizeof(*data));

	/* Pack up table update data */
	data->opcode = opcode;
	pthread_rwlock_rdlock(&table->lock);
	data->table = table_copy(table);
	pthread_rwlock_unlock(&table->lock);

	strcpy(data->player, name);
	data->seat = seat;

	/* Queue table event for whole room */
	return event_room_enqueue(room, table_event_callback,
				  sizeof(*data), data,
				  table_event_data_free);
}


static void table_event_data_free(void* data)
{
	GGZTableEventData *event_data = data;

	table_free(event_data->table);
	ggz_free(event_data);
}


/* Event callback for delivering table list update to a player */
static GGZEventFuncReturn table_event_callback(void* target, size_t size,
                                               void* data)
{
	GGZPlayer* player = target;
	GGZTableSeat seat;
	GGZTableSpectator spectator;
	GGZTableEventData *event = data;
	void* update_data = NULL;

	switch (event->opcode) {
	case GGZ_TABLE_UPDATE_DELETE:
		dbg_msg(GGZ_DBG_UPDATE, "%s sees table %d deleted",
			player->name, event->table->index);
		break;

	case GGZ_TABLE_UPDATE_ADD:
		dbg_msg(GGZ_DBG_UPDATE, "%s sees table %d added",
			player->name, event->table->index);
		break;

	case GGZ_TABLE_UPDATE_DESC:
		dbg_msg(GGZ_DBG_UPDATE, "%s sees table %d new desc '%s'",
			player->name, event->table->index, event->table->desc);
		break;		
		
	case GGZ_TABLE_UPDATE_STATE:
		dbg_msg(GGZ_DBG_UPDATE, "%s sees table %d new state %d",
			player->name, 
			event->table->index, event->table->state);
		break;

	case GGZ_TABLE_UPDATE_LEAVE:
	case GGZ_TABLE_UPDATE_JOIN:
		strcpy(seat.name, event->player);
		seat.index = event->seat;
		seat.type = GGZ_SEAT_PLAYER;

		update_data = &seat;

		dbg_msg(GGZ_DBG_UPDATE, "%s sees %s %s seat %d at table %d", 
			player->name, seat.name, 
			event->opcode == GGZ_TABLE_UPDATE_JOIN ? "join"
							       : "leave",
			seat.index, event->table->index);
		break;

	case GGZ_TABLE_UPDATE_SEAT:
		seat.index = event->seat;
		seat.type = event->table->seat_types[event->seat];
		strcpy(seat.name, event->table->seat_names[event->seat]);
		update_data = &(seat);
		break;

	case GGZ_TABLE_UPDATE_SPECTATOR_LEAVE:
	case GGZ_TABLE_UPDATE_SPECTATOR_JOIN:
		strcpy(spectator.name, event->player);
		spectator.index = event->seat;

		update_data = &spectator;

		dbg_msg(GGZ_DBG_UPDATE,
			"%s sees %s %s spectator seat %d at table %d",
			player->name, spectator.name,
			event->opcode == GGZ_TABLE_UPDATE_SPECTATOR_JOIN
						? "join" : "leave",
			spectator.index, event->table->index);
		break;
	case GGZ_TABLE_UPDATE_RESIZE:
		/* TODO? */
		dbg_msg(GGZ_DBG_UPDATE, "%s sees table %d resized %d",
			player->name, 
			event->table->index, event->table->num_seats);
		break;
	}

	if (net_send_table_update(player->client->net, event->opcode,
				  event->table, update_data,
				  player_get_room(player)) < 0)
		return GGZ_EVENT_ERROR;
	
	return GGZ_EVENT_OK;
}


static GGZReturn table_launch_event(char* name,
				    GGZClientReqError status, int index)
{
	GGZLaunchEventData *data = ggz_malloc(sizeof(*data));

	data->status = status;
	data->table_index = (status == 0) ? index : -1;

	return event_player_enqueue(name, player_launch_callback,
				    sizeof(*data), data, NULL);
}


/* Free dynamically allocated memory associated with a table*/
void table_free(GGZTable* table)
{
	if (table->num_seats > 0) {
		ggz_free(table->seat_types);
		ggz_free(table->seat_names);
	}
	if (table->max_num_spectators > 0) {
		ggz_free(table->spectators);
	}

	if (table->ggzdmod)
		ggzdmod_free(table->ggzdmod);

	if (table->savegame)
		ggz_free(table->savegame);

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


RETSIGTYPE table_handle_event_signal(int signal)
{
	GGZTable *table = pthread_getspecific(table_key);

	/* Process queued up events */
	if (table->events_head && event_table_handle(table) < 0)
		table->state = GGZ_TABLE_DONE;
}
