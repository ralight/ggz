/*
 * File: event.h
 * Author: Brent Hendricks
 * Project: GGZ Server
 * Date: 5/8/00
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

#ifndef _GGZ_EVENT_H
#define _GGZ_EVENT_H

/* Event callback function type */
/* Should return 0 if OK, and -1 if fatal error occurs */
typedef int (*GGZEventFunc)(int p_index, int size, void* data);


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
	unsigned int size;

	/* Pointer to data for event */
	void *data;

	/* Callback for processing event */
	GGZEventFunc handle;
	
} GGZEvent;


/* Place an event into the room-specific event queue */
/* data must be dynamically allocated, or can be NULL */
int event_room_enqueue(int room, GGZEventFunc func, unsigned int size, 
		       void* data);
		       
/* Process queued-up room-specific events for player */
int event_room_handle(int p_index);

/* Flush queued-up room-specific events for player */
int event_room_flush(int p_index);



/* Place an event into the player-specific event queue */
/* data must be dynamically allocated, or can be NULL */
int event_player_enqueue(int p_index, GGZEventFunc func, unsigned int size, 
			 void* data);
			 
/* Process queued-up player-specific events */
int event_player_handle(int p_index);

/* Flush queued-up player-specific events */
int event_player_flush(int p_index);

#endif
