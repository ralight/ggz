/*
 * File: event.c
 * Author: Brent Hendricks
 * Project: GGZ Server
 * Date: 5/9/00
 * Desc: Functions for handling/manipulating GGZ events
 * $Id: event.c 4532 2002-09-13 01:35:13Z jdorje $
 *
 * Copyright (C) 2000 Brent Hendricks.
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

#include <pthread.h>
#include <stdlib.h>
#include <ctype.h>

#include <err_func.h>
#include <event.h>
#include <room.h>
#include <datatypes.h>
#include <protocols.h>
#include <players.h>
#include <hash.h>
#include <net.h>
#include "client.h"
#include "table.h"

/* Server wide data structures */
extern Options opt;


/* Local support functions */
static void event_player_do_enqueue(GGZPlayer* player, GGZEvent* event);
static void event_table_do_enqueue(GGZTable* table, GGZEvent* event);
#ifdef DEBUG
static void event_room_spew(int room);
static void event_player_spew(GGZPlayer* player);
static void event_table_spew(GGZTable* table);
#endif

/* Place an event into the room-specific event queue */
int event_room_enqueue(int room, GGZEventFunc func, size_t size, 
		       void* data)
		       
{
	GGZEvent *event;
	GGZPlayer* player;
	int i;

	/* Check for illegal room # */	
	if(room < 0) {
		dbg_msg(GGZ_DBG_LISTS, "event_room_enqueue() called from -1");
		return 0;
	}

	/* Allocate a new event item */
	event = ggz_malloc(sizeof(GGZEvent));
	dbg_msg(GGZ_DBG_LISTS, "Allocated event %p", event);

	pthread_rwlock_wrlock(&rooms[room].lock);

	/* Check for empty room (event might be last player leaving) */
	if (rooms[room].player_count == 0) {
		pthread_rwlock_unlock(&rooms[room].lock);
		ggz_free(event);
		if (data)
			ggz_free(data);
		dbg_msg(GGZ_DBG_LISTS,
			"Deallocated event %p (empty room)", event);
		return 0;
	}

	/* Fill in event structure */
	event->ref_count = rooms[room].player_count;
	event->next = NULL;
	event->size = size;
	event->data = data;
	event->handle = func;

	/* Put this event as first for anyone who doesn't have a list now */
	for (i = 0; i < rooms[room].player_count; i++) {
		player = rooms[room].players[i];
		if (player->room_events == NULL)
			player->room_events = event;
	}

	/* Finally, add this event to the room list */
	if (rooms[room].event_tail)
		rooms[room].event_tail->next = event;
	rooms[room].event_tail = event;

#ifdef DEBUG
	if (rooms[room].event_head == NULL)
		rooms[room].event_head = event;
	event_room_spew(room);
#endif

	pthread_rwlock_unlock(&rooms[room].lock);
	return 0;
}


/* Process queued-up room-specific events for player */
int event_room_handle(GGZPlayer* player)
{
	GGZEvent *event, *rm_list = NULL;
	int room, status;

	/* 
	 * We don't need player lock here, since our room can't change 
	 * unless we change it 
	 */
	room = player->room;

	pthread_rwlock_rdlock(&rooms[room].lock);
	event = player->room_events;
	/* 
	 * This player obviously has a reference to the event, so it
	 * can't be removed out from under us.  We might as well
	 * release the room lock 
	 */
	pthread_rwlock_unlock(&rooms[room].lock);

	while (event) {
		
		/* Invoke callback for this event */
		status = (*event->handle)((void*)player, event->size, 
					  event->data);

		/* If there was a fatal error, return immedately */
		if (status == GGZ_EVENT_ERROR)
			return -1;

		/* We need the lock now to alter the event list */
		pthread_rwlock_wrlock(&rooms[room].lock);
		
		/* Update player pointer into list */
		player->room_events = event->next;
		
		/* And add the event to the remove list if necessary */
		if (--(event->ref_count) == 0) {
			if (rooms[room].event_tail == event)
				rooms[room].event_tail = NULL;
#ifdef DEBUG
			rooms[room].event_head = event->next;
#endif /* DEBUG */
			event->next = rm_list;
			rm_list = event;
		}
#ifdef DEBUG
		event_room_spew(room);
#endif /* DEBUG */

		/* 
		 * Grab next event from player list in case event was 
		 * added to remove list (and its next got overwritten)
		 */
		event = player->room_events;
		pthread_rwlock_unlock(&rooms[room].lock);
	}
	
	/* Finally, free the list of events to remove */
	while ( (event = rm_list) != NULL) {
		rm_list = event->next;
		if (event->data)
			ggz_free(event->data);
		ggz_free(event);
	}
	
	return 0;
}


/* Flush queued up room-specific events for player */
int event_room_flush(GGZPlayer* player)
{
	GGZEvent *event;
	int room = player->room;

	dbg_msg(GGZ_DBG_LISTS, "Flushing all events for %s in room %d", 
		player->name, room);

	while ( (event = player->room_events)) {
		player->room_events = event->next;
		if (--(event->ref_count) == 0) {
			dbg_msg(GGZ_DBG_LISTS, "Removing event %p", event);
			if (rooms[room].event_tail == event)
				rooms[room].event_tail = NULL;
#ifdef DEBUG
			rooms[room].event_head = event->next;
#endif /* DEBUG */
			if (event->data)
				ggz_free(event->data);
			ggz_free(event);
		}
	}

#ifdef DEBUG
	event_room_spew(room);
#endif /* DEBUG */

	return 0;
}


/* Place an event into the player-specific event queue */
int event_player_enqueue(char* name, GGZEventFunc func, size_t size, 
			 void* data)
			 
{
	GGZEvent *event;
	GGZPlayer *player;

	/* Allocate a new event item */
	event = ggz_malloc(sizeof(GGZEvent));
	dbg_msg(GGZ_DBG_LISTS, "Allocated event %p", event);

	/* Fill in event structure */
	event->ref_count = 1;
	event->next = NULL;
	event->size = size;
	event->data = data;
	event->handle = func;

	/* Find target player.  Returns with player write-locked */
	if ( (player = hash_player_lookup(name)) == NULL ) {
		if (data)
			ggz_free(data);
		ggz_free(event);
		dbg_msg(GGZ_DBG_LISTS, "Deallocated event %p (no user)", 
			event);		
		return -1;
	}

	/* Check to see if player is connected */
	if (net_get_fd(player->client->net) == -1) {
		pthread_rwlock_unlock(&player->lock);
		if (data)
			ggz_free(data);
		ggz_free(event);
		dbg_msg(GGZ_DBG_LISTS, "Deallocated event %p (no user)", 
			event);
		return -1;
	}

	/* FIXME: Should we be doing more checking?  logged-in, etc? */

	event_player_do_enqueue(player, event);
	pthread_rwlock_unlock(&player->lock);

	return 0;
}


/* Process queued-up player-specific events */
int event_player_handle(GGZPlayer* player)
{
	int status;
	GGZEvent *event, *next;
	
	/* Grab list of personal events to handle */
	pthread_rwlock_wrlock(&player->lock);
	event = player->my_events_head;
	player->my_events_head = NULL;
	player->my_events_tail = NULL;
	pthread_rwlock_unlock(&player->lock);	
	
	while (event) {
		next = event->next;
		
		/* Invoke callback for this event */
		status = (*event->handle)((void*)player, event->size, 
					  event->data);

		switch (status) {
		case GGZ_EVENT_ERROR:
			/* If there was a fatal error, return immedately */
			return -1;
			break;
		case GGZ_EVENT_OK:
			/* Remove event if necessary (always) */
			if (--(event->ref_count) == 0) {
				dbg_msg(GGZ_DBG_LISTS, "Removing event %p", 
					event);
				if (event->data)
					ggz_free(event->data);
				ggz_free(event);
			}
			break;
		case GGZ_EVENT_DEFER:
			dbg_msg(GGZ_DBG_LISTS, "Deferring event %p", event);
			pthread_rwlock_wrlock(&player->lock);
			event_player_do_enqueue(player, event);
			pthread_rwlock_unlock(&player->lock);
			break;
		}
		
		event = next;
	}

#ifdef DEBUG
	event_player_spew(player);
#endif

	return 0;
}


/* Flush queued up room-specific events for player */
int event_player_flush(GGZPlayer* player)
{
	GGZEvent *event, *next;
	
	/* Grab list of personal events to flush */
	pthread_rwlock_wrlock(&player->lock);
	event = player->my_events_head;
	player->my_events_head = NULL;
	player->my_events_tail = NULL;
	pthread_rwlock_unlock(&player->lock);	

	while (event) {
		next = event->next;
		/* Remove event if necessary (always) */
		if (--(event->ref_count) == 0) {
			dbg_msg(GGZ_DBG_LISTS, "Removing event %p", event);
			if (event->data)
				ggz_free(event->data);
			ggz_free(event);
		}
		event = next;
	}

#ifdef DEBUG
	event_player_spew(player);
#endif

	return 0;
}


/*
 * event_player_do_enqueue() actually adds an event to a player's queue
 *
 * Receives:
 * GGZPlayer *player : pointer to player we are adding event to
 * GGZEvent  *event  : pointer to event we're adding
 *
 * Note: Should be called with player write lock acquired
 */
static void event_player_do_enqueue(GGZPlayer* player, GGZEvent* event) {

	/* Add this event to end of the list */
	if (player->my_events_tail)
		((GGZEvent*)(player->my_events_tail))->next = event;
	player->my_events_tail = event;

	if (player->my_events_head == NULL)
		player->my_events_head = event;
	
#ifdef DEBUG
	event_player_spew(player);
#endif

}


/* Place an event into the table-specific event queue */
int event_table_enqueue(int room, int index, GGZEventFunc func, 
			size_t size, void* data)
{
	GGZEvent *event;
	GGZTable *table;
	
	/* Allocate a new event item */
	event = ggz_malloc(sizeof(GGZEvent));
	dbg_msg(GGZ_DBG_LISTS, "Allocated event %p", event);

	/* Fill in event structure */
	event->ref_count = 1;
	event->next = NULL;
	event->size = size;
	event->data = data;
	event->handle = func;

	/* Find target table.  Returns with table write-locked */
	if ( (table = table_lookup(room, index)) == NULL) {
		if (data)
			ggz_free(data);
		ggz_free(event);
		dbg_msg(GGZ_DBG_LISTS, "Deallocated event %p (no table)", 
			event);				
		return E_NO_TABLE;
	}

	/* FIXME: Should we be doing more checking? state, etc? */

	event_table_do_enqueue(table, event);
	pthread_rwlock_unlock(&table->lock);

	return 0;
}


/* Process queued-up table-specific events */
int event_table_handle(GGZTable* table)
{
	int status;
	GGZEvent *event, *next;
	
	/* Grab list of private events to handle */
	pthread_rwlock_wrlock(&table->lock);
	event = table->events_head;
	table->events_head = NULL;
	table->events_tail = NULL;
	pthread_rwlock_unlock(&table->lock);	
	
	while (event) {
		next = event->next;
		
		/* Invoke callback for this event */
		status = (*event->handle)((void*)table, event->size, 
					  event->data);
		
		switch (status) {
		case GGZ_EVENT_ERROR:
			/* If there was a fatal error, return immedately */
			/* FIXME: should we flush the rest of the events? */
			return -1;
			break;
		case GGZ_EVENT_OK:
			/* Remove event if necessary (always) */
			if (--(event->ref_count) == 0) {
				dbg_msg(GGZ_DBG_LISTS, "Removing event %p", 
					event);
				if (event->data)
					ggz_free(event->data);
				ggz_free(event);
			}
			break;
		case GGZ_EVENT_DEFER:
			dbg_msg(GGZ_DBG_LISTS, "Deferring event %p", event);
			pthread_rwlock_wrlock(&table->lock);
			event_table_do_enqueue(table, event);
			pthread_rwlock_unlock(&table->lock);
			break;
		}
		
		event = next;
	}

#ifdef DEBUG
	event_table_spew(table);
#endif

	return 0;
}


/* Flush queued up room-specific events for table */
int event_table_flush(GGZTable* table)
{
	GGZEvent *event, *next;
	
	/* Grab list of personal events to flush */
	pthread_rwlock_wrlock(&table->lock);
	event = table->events_head;
	table->events_head = NULL;
	table->events_tail = NULL;
	pthread_rwlock_unlock(&table->lock);	

	while (event) {
		next = event->next;
		/* Remove event if necessary (always) */
		if (--(event->ref_count) == 0) {
			dbg_msg(GGZ_DBG_LISTS, "Removing event %p", event);
			if (event->data)
				ggz_free(event->data);
			ggz_free(event);
		}
		event = next;
	}

#ifdef DEBUG
	event_table_spew(table);
#endif

	return 0;
}


/*
 * event_table_do_enqueue() actually adds an event to a table's queue
 *
 * Receives:
 * GGZTable *table : pointer to table we are adding event to
 * GGZEvent  *event  : pointer to event we're adding
 *
 * Note: Should be called with table write lock acquired
 */
static void event_table_do_enqueue(GGZTable* table, GGZEvent* event) {

	/* Add this event to end of the list */
	if (table->events_tail)
		((GGZEvent*)(table->events_tail))->next = event;
	table->events_tail = event;

	if (table->events_head == NULL)
		table->events_head = event;
	
#ifdef DEBUG
	event_table_spew(table);
#endif

}


#ifdef DEBUG
static void event_room_spew(int room)
{
	GGZEvent *event;

	dbg_msg(GGZ_DBG_LISTS, "------ Room %d Event List ------", room);
	event = rooms[room].event_head;
	dbg_msg(GGZ_DBG_LISTS, "Event head is %p", event);
	while (event) {
		dbg_msg(GGZ_DBG_LISTS, "  Chain item %p (%d)",
			event, event->ref_count);
		event = event->next;
	}
	dbg_msg(GGZ_DBG_LISTS, "Event tail is %p", rooms[room].event_tail);
	dbg_msg(GGZ_DBG_LISTS, "-----------------------");
}

static void event_player_spew(GGZPlayer* player)
{
	GGZEvent *event;

	dbg_msg(GGZ_DBG_LISTS, "------ %s Event List ------", player->name);
	event = player->my_events_head;
	dbg_msg(GGZ_DBG_LISTS, "Event head is %p", event);
	while (event) {
		dbg_msg(GGZ_DBG_LISTS, "  Chain item %p (%d)",
			event, event->ref_count);
		event = event->next;
	}
	dbg_msg(GGZ_DBG_LISTS, "Event tail is %p", 
		player->my_events_tail);
	dbg_msg(GGZ_DBG_LISTS, "-----------------------");
}

static void event_table_spew(GGZTable* table)
{
	GGZEvent *event;

	dbg_msg(GGZ_DBG_LISTS, "------ Table %d room %d Event List ------",
		table->index, table->room);
	event = table->events_head;
	dbg_msg(GGZ_DBG_LISTS, "Event head is %p", event);
	while (event) {
		dbg_msg(GGZ_DBG_LISTS, "  Chain item %p (%d)",
			event, event->ref_count);
		event = event->next;
	}
	dbg_msg(GGZ_DBG_LISTS, "Event tail is %p", 
		table->events_tail);
	dbg_msg(GGZ_DBG_LISTS, "-----------------------");
}

#endif /* DEBUG */


