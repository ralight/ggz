/*
 * File: event.c
 * Author: Brent Hendricks
 * Project: GGZ Core Client Lib
 * Date: 9/15/00
 *
 * This is the code for handling high-level client events
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
#include <ggzcore.h>
#include <event.h>

#include <poll.h>
#include <stdlib.h>
#include <unistd.h>


/* GGZCallback : data type for representing single event callback in a list */
typedef struct GGZCallback {

	/* Callback ID (unique withing this event) */
	unsigned int id;
	
	/* Actual callback function */
	GGZEventFunc func;
	
	/* Pointer to user data */
	void* user_data;

	/* Pointer to next GGZCallback */
	struct GGZCallback* next;

} GGZCallback;


/* GGZEvent : data type for representing an event */
typedef struct GGZEvent {

	/* Unique event number */
	GGZEventID id;
	
	/* Descriptive string (mainly for debugging purposes) */
	const char* name;

	/* Sequence number for callbacks */
	int seq_id;

	/* Linked list of callbacks */
	GGZCallback* callbacks;

	/* Pointer to event-specific data */
	/* FIXME: do we really need to store this? */
	void* event_data;
} GGZEvent;


/* Array of all GGZ events */
static GGZEvent ggz_events[] = {
	{GGZ_SERVER_LOGIN_OK,   "server_login_ok",   1, NULL, NULL},
	{GGZ_SERVER_LOGIN_FAIL, "server_login_fail", 1, NULL, NULL},
	{GGZ_USER_LOGIN_GUEST,  "user_login_guest",  1, NULL, NULL},
	{GGZ_USER_CHAT,         "user_chat",         1, NULL, NULL},
	{GGZ_USER_LOGOUT,       "user_logout",       1, NULL, NULL}
};

/* Number of events */
static unsigned int ggzcore_num_events;

/* Pipe used for queueing events */
static int ggzcore_event_pipe[2];

/* Private functions */
static void event_process(GGZEventID id);
static void event_dump_callbacks(GGZEventID id);


/* ggzcore_event_init() - Initialize event system
 *
 * Receives:
 *
 * Returns:
 */
void ggzcore_event_init(void)
{
	int i;

	/* Setup events */
	ggzcore_num_events = sizeof(ggz_events)/sizeof(GGZEvent);
	for (i = 0; i < ggzcore_num_events; i++) 
		ggzcore_debug("Setting up %s event with id %d", 
			      ggz_events[i].name, ggz_events[i].id);

	/* Create fd */
	/* FIXME: should we make it non-blocking? */
	if (pipe(ggzcore_event_pipe) < 0)
		ggzcore_error_sys_exit("pipe() failed in ggzcore_event_init");
}


/* ggzcore_event_connect() - Register callback for specific GGZ-event
 *
 * Receives:
 * GGZEventID id     : ID code of event
 * GGZEventFunc func : Callback function
 * void* user_data   : "User" data
 *
 * Returns:
 * int : id for this callback 
 */
int ggzcore_event_connect(const GGZEventID id, const GGZEventFunc func, void* data)
{
	GGZCallback *callback, *cur, *next;
	
	/* This should never happen, but....*/
	if (id < 0 || id >= ggzcore_num_events)
		return -1;
	
	if ( (callback = calloc(1, sizeof(GGZCallback))) == NULL)
		ggzcore_error_sys_exit("calloc() failed in ggzcore_event_connect");
	
	/* Assign unique ID */
	callback->id = ggz_events[id].seq_id++;
	callback->func = func;
	callback->user_data = data;

	/* Insert into list of callbacks */
	if ( (next = ggz_events[id].callbacks) == NULL)
		ggz_events[id].callbacks = callback;
	else {
		while (next) {
			cur = next;
			next = cur->next;
		}
		cur->next = callback;
	}
	
	event_dump_callbacks(id);

	return callback->id;
}


/* ggzcore_event_remove_all() - Remove all callbacks from an event
 *
 * Receives:
 * GGZEventID id     : ID code of event
 *
 * Returns:
 * int : 0 if successful, -1 on error
 */
int ggzcore_event_remove_all(const GGZEventID id)
{
	GGZCallback *cur, *next;

	ggzcore_debug("Removing all callbacks from event %s", 
		      ggz_events[id].name);
	next = ggz_events[id].callbacks;
	while (next) {
		cur = next;
		next = cur->next;
		free(cur);
	}
	ggz_events[id].callbacks = NULL;
	
	return 0;
}


/* ggzcore_event_remove() - Remove specific callback from an event
 *
 * Receives:
 * GGZEventID id            : ID code of event
 * unsigned int callback_id : ID of callback to remove
 *
 * Returns:
 * int : 0 if successful, -1 on error
 */
int ggzcore_event_remove(const GGZEventID id, const unsigned int callback_id)
{
	GGZCallback *cur, *prev = NULL;

	cur = ggz_events[id].callbacks;
	while (cur && cur->id != id) {
		prev = cur;
		cur = cur->next;
	}

	/* Couldn't find it! */
	if (!cur) {
		ggzcore_debug("Can't find callback %d to remove from event %s",
			      id, ggz_events[id].name);
		return -1;
	}
	else {
		ggzcore_debug("Removing callback %d from event %s", id, 
			      ggz_events[id].name);

		/* Special case if it was first in the list */
		if (!prev)
			ggz_events[id].callbacks = cur->next;
		else
			prev->next = cur->next;

		free(cur);
		return 0;
	}
}


/* ggzcore_event_pending() - Determine if there are any pending events
 *
 * Receives:
 *
 * Returns:
 * int : 1 if there is at least one event pending, 0 otherwise
 */
int ggzcore_event_pending(void)
{
	int status;
	struct pollfd fd[1] = {{ggzcore_event_pipe[0], POLLIN, 0}};

	ggzcore_debug("Checking for events");	
	if ( (status = poll(fd, 1, 0)) < 0)
		ggzcore_error_sys_exit("poll failed in ggzcore_event_pending");

	return status;
}


/* ggzcore_event_process_all() - Process all pending events
 *
 * Receives:
 *
 * Returns:
 * int : 0 if successful, -1 otherwise
 */
int ggzcore_event_process_all(void)
{
	unsigned int size;
	GGZEventID id;

	while (ggzcore_event_pending()) {
		ggzcore_debug("Procesing events");
		read(ggzcore_event_pipe[0], &id, sizeof(GGZEventID));
		/* FIXME: do validity checking */
		read(ggzcore_event_pipe[0], &size, sizeof(int));
		ggzcore_debug("Received %s event (%d bytes user data)", 
			      ggz_events[id].name, size);
		if (size) {
			if (!(ggz_events[id].event_data = malloc(size)))
				ggzcore_error_sys_exit("malloc() failed in ggzcore_event_process_all");
			read(ggzcore_event_pipe[0], ggz_events[id].event_data,
			     size);
		}
		
		event_process(id);
	}
		
	return 0;
}


/* event_process() - Process an event (invoking callbacks)
 *
 * Receives:
 *
 * Returns:
 * int : 0 if successful, -1 otherwise
 */
static void event_process(GGZEventID id)
{
	GGZCallback *cur;

	/*ggz_events[id].event_data = data;*/

	ggzcore_debug("Invoking %s event callbacks ", ggz_events[id].name);
	for (cur = ggz_events[id].callbacks; cur != NULL; cur = cur->next) {
		(*cur->func)(id, ggz_events[id].event_data, cur->user_data);
	}

	/* FIXME: should we free event_data? */
}


/* ggzcore_event_trigger() - Trigger (place in the queue) a specific event
 *
 * Receives:
 * GGZEventID id     : ID code of event
 * void* event_data  : Data specific to this occurance of the event
 * unsigned int size : Number of bytes of event data
 *
 * Returns:
 * int : 0 if successful, -1 otherwise
 */
int ggzcore_event_trigger(const GGZEventID id, void* data, const unsigned int size)
{
	ggzcore_debug("%s event triggered (%d bytes of user data)", 
		      ggz_events[id].name, size);
	
	/* Queue-up event in pipe */
	write(ggzcore_event_pipe[1], &id, sizeof(id));
	write(ggzcore_event_pipe[1], &size, sizeof(int));
	write(ggzcore_event_pipe[1], data, size);

	/* FIXME: Can we alternatively do sync. events just by storing
	   event data and calling event_process(id); ? */

	return 0;
}


/* Debugging function to examine state of callback list */
static void event_dump_callbacks(GGZEventID id)
{
	GGZCallback *cur;

	ggzcore_debug("-- Event: %s", ggz_events[id].name);
	for (cur = ggz_events[id].callbacks; cur != NULL; cur = cur->next)
		ggzcore_debug("  Callback id %d", cur->id);
	ggzcore_debug("-- End of event");
}


/* ggzcore_event_destroy() - Cleanup event system
 *
 * Receives:
 *
 * Returns:
 */
void ggzcore_event_destroy(void)
{
	int i;

	ggzcore_num_events = sizeof(ggz_events)/sizeof(GGZEvent);
	for (i = 0; i < ggzcore_num_events; i++) {
		ggzcore_debug("Cleaning up %s event", ggz_events[i].name);
		ggzcore_event_remove_all(ggz_events[i].id);
	}
}


