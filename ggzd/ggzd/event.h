/*
 * File: event.h
 * Author: Brent Hendricks
 * Project: GGZ Server
 * Date: 5/8/00
 * Desc: Functions for handling/manipulating GGZ events
 * $Id: event.h 4532 2002-09-13 01:35:13Z jdorje $
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


#ifndef _GGZ_EVENT_H
#define _GGZ_EVENT_H

#include <ggzd.h>
#include <players.h>

/* 
 * Event callback function type 
 * Receives:
 * void* target  : pointer to event target (some table or player)
 * size_t size   : size of data (in bytes) to pass to handler
 * void* data    : pointer to dynamically allocated data (or NULL)
 *
 * Returns:
 * one of
 * GGZ_EVENT_ERROR  : fatal error
 * GGZ_EVENT_OK     : event processed successfully
 * GGZ_EVENT_DEFER  : postpone event for later (no effect for room events)
 */
typedef GGZEventFuncReturn (*GGZEventFunc)(void* target,
					   size_t size, void* data);


/* 
 * The GGZEvent structure is meant to be a node in a linked list
 * of events.
 */
typedef struct GGZEvent {

	/* Pointer to next node in the linked list */
	struct GGZEvent *next;

	/* 
	 * Reference count for this event.  When refcount reaches 0,
	 * typically the event will be removed from the list.  
	 */
	unsigned int ref_count;

	/* Size of event data (in bytes) */
	size_t size;

	/* Pointer to data for event */
	void *data;

	/* Callback for processing event */
	GGZEventFunc handle;
	
} GGZEvent;


/*
 * event_room_enqueue() adds an event to the common queue in a room
 *
 * Receives:
 * int room          : index of room in which to enqueue event
 * GGZEventFunc func : callback function for handling this event
 * size_t size       : size of data (in bytes) to pass to handler
 * void* data        : pointer to dynamically allocated data (or NULL)
 *
 * Note: memory pointed to by data MUST be dynamcially allocated
 */
int event_room_enqueue(int room, GGZEventFunc func,
		       size_t size, void* data);
		       
/* Process queued-up room-specific events for player */
int event_room_handle(GGZPlayer* player);

/* Flush queued-up room-specific events for player */
int event_room_flush(GGZPlayer* player);


/*
 * event_player_enqueue() adds an event to a player's private event queue
 *
 * Receives:
 * char *name        : name of player to whom this event is targetted
 * GGZEventFunc func : callback function for handling this event
 * size_t size       : size of data (in bytes) to pass to handler
 * void* data        : pointer to dynamically allocated data (or NULL)
 *
 * Note: memory pointed to by data MUST be dynamcially allocated
 */
int event_player_enqueue(char* name, GGZEventFunc func,
			 size_t size, void* data);
			 
/*
 * event_player_handle() processes all events currently in the private
 * event queue of the specified player
 *
 * Receives:
 * GGZPlayer *player : pointer to the player whose events we're processing
 */
int event_player_handle(GGZPlayer* player);

/*
 * event_player_flush() discards all events currently in the private
 * event queue of the specified player
 *
 * Receives:
 * GGZPlayer *player : pointer to the player whose events we're flushing
 */
int event_player_flush(GGZPlayer* player);


/*
 * event_table_enqueue() adds an event to a table's private event queue
 *
 * Receives:
 * int room          : index of room in which table resides
 * int index         : index of table to which this event is targetted
 * GGZEventFunc func : callback function for handling this event
 * size_t size       : size of data (in bytes) to pass to handler
 * void* data        : pointer to dynamically allocated data (or NULL)
 *
 * Note: memory pointed to by data MUST be dynamcially allocated
 */
int event_table_enqueue(int room, int index, GGZEventFunc func, 
			size_t, void* data);

/*
 * event_table_handle() processes all events currently in the private
 * event queue of the specified table
 *
 * Receives:
 * GGZTable *table : pointer to the table whose events we're processing
 */
int event_table_handle(GGZTable* table);

/*
 * event_table_flush() discards all events currently in the private
 * event queue of the specified table
 *
 * Receives:
 * GGZTable *table : pointer to the table whose events we're flushing
 */
int event_table_flush(GGZTable* table);

#endif

