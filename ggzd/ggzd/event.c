/*
 * File: event.c
 * Author: Brent Hendricks
 * Project: GGZ Server
 * Date: 5/9/00
 * Desc: Functions for handling/manipulating GGZ events
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

#include <easysock.h>
#include <err_func.h>
#include <event.h>
#include <room.h>
#include <datatypes.h>
#include <protocols.h>

/* Server wide data structures */
extern Options opt;
extern struct Users players;


/* Local support functions */
static void event_room_spew(int room);
static void event_player_spew(int p_index);


/* Place an event into the room-specific event queue */
int event_room_enqueue(int room, GGZEventFunc func, unsigned int size, 
		       void* data)
		       
{
	GGZEvent *event;
	int i, p_index;

	/* Allocate a new event item */
	if ( (event = malloc(sizeof(GGZEvent))) == NULL) {
		if (data)
			free(data);
		err_sys_exit("malloc failed in event_room_enqueue()");
	}
	dbg_msg(GGZ_DBG_LISTS, "Allocated event %p", event);

	pthread_rwlock_wrlock(&rooms[room].lock);

	/* Check for empty room (event might be last player leaving) */
	if (rooms[room].player_count == 0) {
		pthread_rwlock_unlock(&rooms[room].lock);
		free(event);
		if (data)
			free(data);
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
		p_index = rooms[room].player_index[i];
		if(players.info[p_index].room_events == NULL)
			players.info[p_index].room_events = event;
	}

	/* Finally, add this event to the room list */
	if (rooms[room].event_tail)
		rooms[room].event_tail->next = event;
	rooms[room].event_tail = event;

#ifdef DEBUG
	if (rooms[room].event_head == NULL)
		rooms[room].event_head = event;
	if (log_info.dbg_types & GGZ_DBG_LISTS)
		event_room_spew(room);
#endif

	pthread_rwlock_unlock(&rooms[room].lock);
	return 0;
}


/* Process queued-up room-specific events for player */
int event_room_handle(int p_index)
{
	GGZEvent *event, *rm_list = NULL;
	int room, status;

	/* 
	 * We don't need player lock here, since our room can't change 
	 * unless we change it 
	 */
	room = players.info[p_index].room;

	pthread_rwlock_rdlock(&rooms[room].lock);
	event = players.info[p_index].room_events;
	/* 
	 * This player obviously has a reference to the event, so it
	 * can't be removed out from under us.  We might as well
	 * release the room lock 
	 */
	pthread_rwlock_unlock(&rooms[room].lock);

	while (event) {
		
		/* Invoke callback for this event */
		status = (*event->handle)(p_index, event->size, event->data);

		/* If there was a fatal error, return immedately */
		if (status < 0)
			return -1;

		/* We need the lock now to alter the event list */
		pthread_rwlock_wrlock(&rooms[room].lock);
		
		/* Update player pointer into list */
		players.info[p_index].room_events = event->next;
		
		/* And add the event to the remove list if necessary */
		if (--(event->ref_count) == 0) {
			if (rooms[room].event_tail == event)
				rooms[room].event_tail = NULL;
#ifdef DEBUG
			rooms[room].event_head = event->next;
#endif DEBUG
			event->next = rm_list;
			rm_list = event;
		}
#ifdef DEBUG
		if (log_info.dbg_types & GGZ_DBG_LISTS)
			event_room_spew(room);
#endif

		/* 
		 * Grab next event from player list in case event was 
		 * added to remove list (and its next got overwritten)
		 */
		event = players.info[p_index].room_events;
		pthread_rwlock_unlock(&rooms[room].lock);
	}
	
	/* Finally, free the list of events to remove */
	while ( (event = rm_list) != NULL) {
		rm_list = event->next;
		if (event->data)
			free(event->data);
		free(event);
	}
	
	return 0;
}


/* Flush queued up room-specific events for player */
int event_room_flush(int p_index)
{
	GGZEvent *event;
	int room = players.info[p_index].room;

	dbg_msg(GGZ_DBG_LISTS, "Flushing all events for %d in room %d", 
		p_index, room);

	while ( (event = players.info[p_index].room_events)) {
		players.info[p_index].room_events = event->next;
		if (--(event->ref_count) == 0) {
			dbg_msg(GGZ_DBG_LISTS, "Removing event %p", event);
			if (rooms[room].event_tail == event)
				rooms[room].event_tail = NULL;
#ifdef DEBUG
			rooms[room].event_head = event->next;
#endif DEBUG
			if (event->data)
				free(event->data);
			free(event);
		}
	}

#ifdef DEBUG
	if (log_info.dbg_types & GGZ_DBG_LISTS)
		event_room_spew(room);
#endif

	return 0;
}


/* Place an event into the player-specific event queue */
int event_player_enqueue(int p_index, GGZEventFunc func, unsigned int size, 
			 void* data)
			 
{
	GGZEvent *event;

	/* Allocate a new event item */
	if ( (event = malloc(sizeof(GGZEvent))) == NULL) {
		if (data)
			free(data);
		err_sys_exit("malloc failed in event_room_enqueue()");
	}
	dbg_msg(GGZ_DBG_LISTS, "Allocated event %p", event);

	/* Fill in event structure */
	event->ref_count = 1;
	event->next = NULL;
	event->size = size;
	event->data = data;
	event->handle = func;

	/* Obtain player specific lock */
	pthread_rwlock_wrlock(&players.info[p_index].lock);

	/* Check to see if player is connected */
	if (players.info[p_index].fd == -1 ) {
		pthread_rwlock_unlock(&players.info[p_index].lock);
		if (data)
			free(data);
		free(event);
		dbg_msg(GGZ_DBG_LISTS, 
			"Deallocated event %p (no user)", event);
		return -1;
	}

	/* FIXME: Should we be doing more checking?  logged-in, etc? */

	/* Add this event to end of the list */
	if (players.info[p_index].my_events_tail)
		players.info[p_index].my_events_tail->next = event;
	players.info[p_index].my_events_tail = event;

	if (players.info[p_index].my_events_head == NULL)
		players.info[p_index].my_events_head = event;
	
	if (log_info.dbg_types & GGZ_DBG_LISTS)
		event_player_spew(p_index);
	
	pthread_rwlock_unlock(&players.info[p_index].lock);

	return 0;
}


/* Process queued-up player-specific events */
int event_player_handle(int p_index)
{
	int status;
	GGZEvent *event, *next;
	
	/* Grab list of personal events to handle */
	pthread_rwlock_wrlock(&players.info[p_index].lock);
	event = players.info[p_index].my_events_head;
	players.info[p_index].my_events_head = NULL;
	players.info[p_index].my_events_tail = NULL;
	pthread_rwlock_unlock(&players.info[p_index].lock);	

	while (event) {
		next = event->next;
		
		/* Invoke callback for this event */
		status = (*event->handle)(p_index, event->size, event->data);

		/* If there was a fatal error, return immedately */
		if (status < 0)
			return -1;
		
		/* Remove event if necessary (always) */
		if (--(event->ref_count) == 0) {
			dbg_msg(GGZ_DBG_LISTS, "Removing event %p", event);
			if (event->data)
				free(event->data);
			free(event);
		}
		event = next;
	}

#ifdef DEBUG
	if (log_info.dbg_types & GGZ_DBG_LISTS)
		event_player_spew(p_index);
#endif

	return 0;
}


/* Flush queued up room-specific events for player */
int event_player_flush(int p_index)
{
	GGZEvent *event, *next;
	
	/* Grab list of personal events to flush */
	pthread_rwlock_wrlock(&players.info[p_index].lock);
	event = players.info[p_index].my_events_head;
	players.info[p_index].my_events_head = NULL;
	players.info[p_index].my_events_tail = NULL;
	pthread_rwlock_unlock(&players.info[p_index].lock);	

	while (event) {
		next = event->next;
		/* Remove event if necessary (always) */
		if (--(event->ref_count) == 0) {
			dbg_msg(GGZ_DBG_LISTS, "Removing event %p", event);
			if (event->data)
				free(event->data);
			free(event);
		}
		event = next;
	}

#ifdef DEBUG
	if (log_info.dbg_types & GGZ_DBG_LISTS)
		event_player_spew(p_index);
#endif

	return 0;
}


#ifdef DEBUG
static void event_room_spew(int room)
{
	GGZEvent *event;

	dbg_msg(GGZ_DBG_LISTS, "------ Event List ------");
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

static void event_player_spew(int p_index)
{
	GGZEvent *event;

	dbg_msg(GGZ_DBG_LISTS, "------ Event List ------");
	event = players.info[p_index].my_events_head;
	dbg_msg(GGZ_DBG_LISTS, "Event head is %p", event);
	while (event) {
		dbg_msg(GGZ_DBG_LISTS, "  Chain item %p (%d)",
			event, event->ref_count);
		event = event->next;
	}
	dbg_msg(GGZ_DBG_LISTS, "Event tail is %p", 
		players.info[p_index].my_events_tail);
	dbg_msg(GGZ_DBG_LISTS, "-----------------------");
}
#endif /* DEBUG */


