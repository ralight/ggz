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
#include <state.h>
#include <net.h>

#include <errno.h>
#include <poll.h>
#include <stdlib.h>
#include <unistd.h>


/* _GGZCallback : data type for representing single event callback in a list */
struct _GGZCallback {

	/* Callback ID (unique withing this event) */
	unsigned int id;
	
	/* Actual callback function */
	GGZEventFunc func;
	
	/* Pointer to user data */
	void* user_data;
	
	/* Function to free user data */
	GGZDestroyFunc destroy;

	/* Pointer to next GGZCallback */
	struct _GGZCallback* next;

};


/* GGZEvent : data type for representing an event */
struct _GGZEvent {

	/* Unique event number */
	GGZEventID id;
	
	/* Descriptive string (mainly for debugging purposes) */
	const char* name;

	/* Sequence number for callbacks */
	int seq_id;

	/* Linked list of callbacks */
	struct _GGZCallback* callbacks;

	/* Pointer to event-specific data */
	/* FIXME: do we really need to store this? */
	void* event_data;
};


/* Array of all GGZ events */
static struct _GGZEvent ggz_events[] = {
	{GGZ_SERVER_CONNECT,        "server_connect",        1},
	{GGZ_SERVER_CONNECT_FAIL,   "server_connect_fail",   1},
	{GGZ_SERVER_MOTD        ,   "server_motd",           1},
	{GGZ_SERVER_LOGIN,          "server_login_ok",       1},
	{GGZ_SERVER_LOGIN_FAIL,     "server_login_fail",     1},
	{GGZ_SERVER_LIST_ROOMS,     "server_list_rooms",     1},
	{GGZ_SERVER_ROOM_JOIN,      "server_room_join",      1},
	{GGZ_SERVER_ROOM_JOIN_FAIL, "server_room_join_fail", 1},
	{GGZ_SERVER_LIST_PLAYERS,   "server_list_players",   1},
	{GGZ_SERVER_CHAT,           "server_chat",           1},
	{GGZ_SERVER_CHAT_FAIL,      "server_chat_fail",      1},
	{GGZ_SERVER_CHAT_MSG,       "server_chat_msg",       1},
	{GGZ_SERVER_CHAT_ANNOUNCE,  "server_chat_announce",  1},
	{GGZ_SERVER_CHAT_PRVMSG,    "server_chat_prvmsg",    1},
	{GGZ_SERVER_CHAT_BEEP,      "server_chat_beep",      1},
	{GGZ_SERVER_LOGOUT,         "server_logout",         1},
	{GGZ_SERVER_ERROR,          "server_error",          1},
	{GGZ_SERVER_ROOM_LEAVE,     "room_leave",            1},
	{GGZ_SERVER_ROOM_ENTER,     "room_enter",            1},
	{GGZ_NET_ERROR,             "net_error",             1},
	{GGZ_USER_LOGIN,            "user_login",            1},
	{GGZ_USER_LIST_ROOMS,       "user_list_rooms",       1},
	{GGZ_USER_LIST_TYPES,       "user_list_types",       1},
	{GGZ_USER_JOIN_ROOM,        "user_join_room",        1},
	{GGZ_USER_LIST_TABLES,      "user_list_tables",      1},
	{GGZ_USER_LIST_PLAYERS,     "user_list_players",     1},
	{GGZ_USER_CHAT,             "user_chat",             1},
	{GGZ_USER_CHAT_PRVMSG,      "user_chat_prvmsg",      1},
	{GGZ_USER_CHAT_BEEP,        "user_chat_beep",        1},
	{GGZ_USER_MOTD,             "user_motd",             1},
	{GGZ_USER_LOGOUT,           "user_logout",           1}
};

/* Number of events */
static unsigned int num_events;

/* Pipe used for queueing events */
static int event_pipe[2];

/* Private functions */
static void _ggzcore_event_process(GGZEventID id);
static void _ggzcore_event_dump_callbacks(GGZEventID id);


/* _ggzcore_event_init() - Initialize event system
 *
 * Receives:
 *
 * Returns:
 */
void _ggzcore_event_init(void)
{
	int i;

	/* Setup events */
	num_events = sizeof(ggz_events)/sizeof(struct _GGZEvent);
	for (i = 0; i < num_events; i++) {
		ggzcore_debug(GGZ_DBG_EVENT, "Setting up %s event with id %d", 
			      ggz_events[i].name, ggz_events[i].id);
		if (ggz_events[i].id != i)
			ggzcore_debug(GGZ_DBG_EVENT, "ID mismatch: %d != %d",
				      ggz_events[i].id, i);
	}
  

	/* Create fd */
	/* FIXME: should we make it non-blocking? */
	if (pipe(event_pipe) < 0)
		ggzcore_error_sys_exit("pipe() failed in _ggzcore_event_init");
}


/* ggzcore_event_connect() - Register callback for specific GGZ-event
 *
 * Receives:
 * GGZEventID id         : ID code of event
 * GGZEventFunc callback : Callback function
 *
 * Returns:
 * int : id for this callback 
 */
int ggzcore_event_connect(const GGZEventID id, const GGZEventFunc callback)
{
	return ggzcore_event_connect_full(id, callback, NULL, NULL);
}


/* ggzcore_event_connect_full() - Register callback for specific GGZ-event
 *                                specifying all parameters
 * 
 * Receives:
 * GGZEventID id         : ID code of event
 * GGZEventFunc callback : Callback function
 * void* user_data       : "User" data to pass to callback
 * GGZDestroyFunc func   : function to call to free user data
 *
 * Returns:
 * int : id for this callback 
 */
int ggzcore_event_connect_full(const GGZEventID id, const GGZEventFunc cb_func,
			       void* user_data, GGZDestroyFunc destroy)
{
	struct _GGZCallback *callback, *cur, *next;
	
	/* This should never happen, but....*/
	if (id < 0 || id >= num_events)
		return -1;
	
	if ( (callback = calloc(1, sizeof(struct _GGZCallback))) == NULL)
		ggzcore_error_sys_exit("calloc() failed in ggzcore_event_connect");
	
	/* Assign unique ID */
	callback->id = ggz_events[id].seq_id++;
	callback->func = cb_func;
	callback->user_data = user_data;
	callback->destroy = destroy;

	/* Append onto list of callbacks */
	if ( (next = ggz_events[id].callbacks) == NULL)
		ggz_events[id].callbacks = callback;
	else {
		while (next) {
			cur = next;
			next = cur->next;
		}
		cur->next = callback;
	}
	
	_ggzcore_event_dump_callbacks(id);

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
	struct _GGZCallback *cur, *next;

	ggzcore_debug(GGZ_DBG_EVENT, "Removing all callbacks from event %s", 
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
	struct _GGZCallback *cur, *prev = NULL;

	cur = ggz_events[id].callbacks;
	while (cur && cur->id != callback_id) {
		prev = cur;
		cur = cur->next;
	}

	/* Couldn't find it! */
	if (!cur) {
		ggzcore_debug(GGZ_DBG_EVENT, 
			      "Can't find callback %d to remove from event %s",
			      callback_id, ggz_events[id].name);
		return -1;
	}
	else {
		ggzcore_debug(GGZ_DBG_EVENT, 
			      "Removing callback %d from event %s", 
			      callback_id, ggz_events[id].name);
		
		/* Special case if it was first in the list */
		if (!prev)
			ggz_events[id].callbacks = cur->next;
		else
			prev->next = cur->next;

		free(cur);
		return 0;
	}
}


/* ggzcore_event_remove_func() - Remove specific callback from an event
 *
 * Receives:
 * GGZEventID id            : ID code of event
 * GGZEventFunc cb_func     : pointer to callback function 
 *
 * Returns:
 * int : 0 if successful, -1 on error
 */
int ggzcore_event_remove_func(const GGZEventID id, const GGZEventFunc cb_func)
{
	struct _GGZCallback *cur, *prev = NULL;

	cur = ggz_events[id].callbacks;
	while (cur && cur->func != cb_func) {
		prev = cur;
		cur = cur->next;
	}

	/* Couldn't find it! */
	if (!cur) {
		ggzcore_debug(GGZ_DBG_EVENT, 
			      "Can't find callback %d to remove from event %s",
			      id, ggz_events[id].name);
		return -1;
	}
	else {
		ggzcore_debug(GGZ_DBG_EVENT, 
			      "Removing callback %d from event %s", id, 
			      ggz_events[id].name);

		/* Free callback specific data */
		if (cur->user_data && cur->destroy)
			(*cur->destroy)(cur->user_data);
		
		/* Special case if it was first in the list */
		if (!prev)
			ggz_events[id].callbacks = cur->next;
		else
			prev->next = cur->next;

		free(cur);
		return 0;
	}
}


/* ggzcore_event_get_fd() - Get a copy of the event pipe fd
 * Receives:
 *
 * Returns:
 * int : event pipe fd
 *
 * Note: this is for detecting event arrivals only.  Do *NOT* attempt
 * to write to this fd.
 */
int ggzcore_event_get_fd(void)
{
	return event_pipe[0];
}


/* ggzcore_event_ispending() - Determine if there are any
 *                                     pending GGZ events
 * Receives:
 *
 * Returns:
 * int : 1 if there is at least one event pending, 0 otherwise
 */
int ggzcore_event_ispending(void)
{
	int status;
	struct pollfd fd[1] = {{event_pipe[0], POLLIN, 0}};

	ggzcore_debug(GGZ_DBG_POLL, "Checking for GGZ events");
	if ( (status = poll(fd, 1, 0)) < 0) {
		if (errno == EINTR) 
			/* Ignore interruptions */
			status = 0;
		else 
			ggzcore_error_sys_exit("poll failed in ggzcore_event_pending");
	} 
	else if (status)
		ggzcore_debug(GGZ_DBG_POLL, "Found a GGZ event!");

	return status;
}


/* ggzcore_event_poll() - Replacement poll command for use in event loops 
 *                        
 * Receives:
 * struct pollfd *ufds : array of file descriptors to poll
 * unsigned int nfds   : number of file descriptors to poll
 * int timeout         : poll timeout in milliseconds
 *
 * Returns:
 * int : number of fds on which data is waiting, 0 if timed out, -1 on error 
 */
int ggzcore_event_poll(struct pollfd *ufds, unsigned int nfds, int timeout)
{
	int count, total_fds, sock, do_net;
	struct pollfd *fds;
	
	total_fds = nfds +1;

	/* FIXME: only do for non-threaded I/O */
	do_net = 1;
	
	if (do_net) {
		if ( (sock = ggzcore_net_get_fd()) >= 0)
			total_fds++;
		else
			do_net = 0;
	}
	
	/* FIXME: add game module fd also */

	if (!(fds = calloc(total_fds, sizeof(struct pollfd))))
		ggzcore_error_sys_exit("calloc failed in ggz_poll");
	
	/* Copy user's poll fd structures into beginning of array */
	memcpy(fds, ufds, nfds*sizeof(struct pollfd));

	/* Add ours onto end */
	fds[nfds].fd = ggzcore_event_get_fd();
	fds[nfds].events = POLLIN;
	
	if (do_net) {
		fds[nfds+1].fd = sock;
		fds[nfds+1].events = POLLIN;
	}
	
	/* FIXME: add game module */

	if ( (count = poll(fds, total_fds, timeout)) < 0) {
		if (errno == EINTR) 
			/* Ignore interruptions */
			count = 0;
		else 
			ggzcore_error_sys_exit("poll failed in ggzcore_event_pending");
	}

	if (do_net && fds[nfds+1].revents) {
		if (fds[nfds+1].revents == POLLHUP)
			_ggzcore_net_disconnect();
		else 
			_ggzcore_net_process();
		count--;
	}

	if (fds[nfds].revents) {
		ggzcore_event_process_all();
		count--;
	}
	
	/* Copy user's poll fd structures back */
	memcpy(ufds, fds, nfds*sizeof(struct pollfd));

	return count;
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
	GGZEventID id;
	
	while (ggzcore_event_ispending()) {
		ggzcore_debug(GGZ_DBG_EVENT, "Procesing events");
		read(event_pipe[0], &id, sizeof(GGZEventID));
		/* FIXME: do validity checking */
		_ggzcore_event_process(id);
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
static void _ggzcore_event_process(GGZEventID id)
{
	void* data;
	GGZDestroyFunc destroy;
	struct _GGZEvent event = ggz_events[id];
	struct _GGZCallback *cur;
			
	read(event_pipe[0], &data, sizeof(void*));
	read(event_pipe[0], &destroy, sizeof(GGZDestroyFunc));
	
	ggzcore_debug(GGZ_DBG_EVENT, "Received %s event : invoking callbacks",
		      event.name);
	
	for (cur = event.callbacks; cur != NULL; cur = cur->next) {
		(*cur->func)(id, data, cur->user_data);
		
		/* Free callback specific data */
		if (cur->user_data && cur->destroy)
			(*cur->destroy)(cur->user_data);
	}

	/* Free event specific data */
	if (data && destroy)
		destroy(data);
}


/* ggzcore_event_trigger() - Trigger (place in the queue) a specific event
 *
 * Receives:
 * GGZEventID id       : ID code of event
 * void* event_data    : Data specific to this occurance of the event
 * GGZDestroyFunc func : function to free event data
 *
 * Returns:
 * int : 0 if successful, -1 otherwise
 *
 * Note that event_data should *not* point to local variables, as they
 * will not be in scope when the event is processed
 */
int ggzcore_event_trigger(const GGZEventID id, void* data, GGZDestroyFunc func)
{
	ggzcore_debug(GGZ_DBG_EVENT, "Triggered %s", ggz_events[id].name);
	
	/* Queue-up event in pipe */
	write(event_pipe[1], &id, sizeof(GGZEventID));
	write(event_pipe[1], &data, sizeof(void*));
	write(event_pipe[1], &func, sizeof(GGZDestroyFunc));

	/* FIXME: Can we alternatively do sync. events just by storing
	   event data and calling event_process(id); ? */

	return 0;
}


/* Debugging function to examine state of callback list */
static void _ggzcore_event_dump_callbacks(GGZEventID id)
{
	struct _GGZCallback *cur;

	ggzcore_debug(GGZ_DBG_EVENT, "-- Event: %s", ggz_events[id].name);
	for (cur = ggz_events[id].callbacks; cur != NULL; cur = cur->next)
		ggzcore_debug(GGZ_DBG_EVENT, "  Callback id %d", cur->id);
	ggzcore_debug(GGZ_DBG_EVENT, "-- End of event");
}


/* _ggzcore_event_destroy() - Cleanup event system
 *
 * Receives:
 *
 * Returns:
 */
void _ggzcore_event_destroy(void)
{
	int i;

	num_events = sizeof(ggz_events)/sizeof(struct _GGZEvent);
	for (i = 0; i < num_events; i++) {
		ggzcore_debug(GGZ_DBG_EVENT, "Cleaning up %s event", 
			      ggz_events[i].name);
		ggzcore_event_remove_all(ggz_events[i].id);
	}
}


