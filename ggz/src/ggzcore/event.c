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
#include <hook.h>
#include <state.h>
#include <net.h>

#include <errno.h>
#include <poll.h>
#include <stdlib.h>
#include <unistd.h>


/* Structure for a particular event type */
struct _GGZEvent {

	/* Unique id number */
	GGZEventID id;
	
	/* Descriptive string (mainly for debugging purposes) */
	const char *name;

	/* List of callback functions */
	GGZHookList *hooks;
};

/* Array of all GGZ events */
static struct _GGZEvent ggz_events[] = {
	{GGZ_SERVER_CONNECT, 	    "server_connect"}, 
	{GGZ_SERVER_CONNECT_FAIL,   "server_connect_fail"}, 
	{GGZ_SERVER_MOTD, 	    "server_motd"},
	{GGZ_SERVER_LOGIN,          "server_login_ok"},
	{GGZ_SERVER_LOGIN_FAIL,	    "server_login_fail"},
	{GGZ_SERVER_LIST_ROOMS,     "server_list_rooms"},
	{GGZ_SERVER_ROOM_JOIN,      "server_room_join"},
	{GGZ_SERVER_ROOM_JOIN_FAIL, "server_room_join_fail"},             
	{GGZ_SERVER_LIST_PLAYERS,   "server_list_players"},               
	{GGZ_SERVER_CHAT,           "server_chat"},                       
	{GGZ_SERVER_CHAT_FAIL,      "server_chat_fail"},                  
	{GGZ_SERVER_CHAT_MSG,       "server_chat_msg"},                   
	{GGZ_SERVER_CHAT_ANNOUNCE,  "server_chat_announce"},              
	{GGZ_SERVER_CHAT_PRVMSG,    "server_chat_prvmsg"},                
	{GGZ_SERVER_CHAT_BEEP,      "server_chat_beep"},                  
	{GGZ_SERVER_LOGOUT,         "server_logout"},                     
	{GGZ_SERVER_ERROR,          "server_error"},                      
	{GGZ_SERVER_ROOM_ENTER,     "server_room_enter"},
	{GGZ_SERVER_ROOM_LEAVE,     "server_room_leave"},
	{GGZ_SERVER_TABLE_UPDATE,   "server_table_update"},
	{GGZ_NET_ERROR,             "net_error"},                         
	{GGZ_USER_LOGIN,            "user_login"},                        
	{GGZ_USER_LIST_ROOMS,       "user_list_rooms"},                   
	{GGZ_USER_LIST_TYPES,       "user_list_types"},                   
	{GGZ_USER_JOIN_ROOM,        "user_join_room"},                    
	{GGZ_USER_LIST_TABLES,      "user_list_tables"},                  
	{GGZ_USER_LIST_PLAYERS,     "user_list_players"},                 
	{GGZ_USER_CHAT,             "user_chat"},                         
	{GGZ_USER_CHAT_PRVMSG,      "user_chat_prvmsg"},                  
	{GGZ_USER_CHAT_BEEP,        "user_chat_beep"},                    
	{GGZ_USER_MOTD,             "user_motd"},                         
	{GGZ_USER_LOGOUT,           "user_logout"}
};

/* Number of events */
static unsigned int num_events;

/* Pipe used for queueing events */
static int event_pipe[2];

/* Private functions */
static void _ggzcore_event_process(GGZEventID id, void *data, 
				   GGZDestroyFunc func);

static void _ggzcore_event_dump_callbacks(GGZEventID id);

static int _ggzcore_event_dequeue(GGZEventID *id, void **data, 
				  GGZDestroyFunc *func);

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
		if (ggz_events[i].id != i)
			ggzcore_debug(GGZ_DBG_EVENT, "ID mismatch: %d != %d",
				      ggz_events[i].id, i);
		else {
			ggz_events[i].hooks = _ggzcore_hook_list_init(i);
			ggzcore_debug(GGZ_DBG_EVENT, 
				      "Setting up event %s with id %d", 
				      ggz_events[i].name, ggz_events[i].id);
		}
	}

	/* Create fd */
	/* FIXME: should we make it non-blocking? */
	if (pipe(event_pipe) < 0)
		ggzcore_error_sys_exit("pipe() failed in _ggzcore_event_init");
}


/* ggzcore_event_add_callback() - Register callback for specific GGZ-event
 *
 * Receives:
 * GGZEventID id         : ID code of event
 * GGZCallback callback : Callback function
 *
 * Returns:
 * int : id for this callback 
 */
int ggzcore_event_add_callback(const GGZEventID id, const GGZCallback func)
{
	return ggzcore_event_add_callback_full(id, func, NULL, NULL);
}


/* ggzcore_event_add_callback_full() - Register callback for specific GGZ-event
 *                                     specifying all parameters
 * 
 * Receives:
 * GGZEventID id          : ID code of event
 * GGZCallback func       : Callback function
 * void* data             : pointer to pass to callback
 * GGZDestroyFunc destroy : function to call to free user data
 *
 * Returns:
 * int : id for this callback 
 */
int ggzcore_event_add_callback_full(const GGZEventID id, 
				    const GGZCallback func,
				    void* data, GGZDestroyFunc destroy)
{
	int status = _ggzcore_hook_add_full(ggz_events[id].hooks, func, data, 
					    destroy);

	_ggzcore_event_dump_callbacks(id);
	
	return status;
}


/* ggzcore_event_remove_callback() - Remove specific callback from an event
 *
 * Receives:
 * GGZEventID id     : ID code of event
 * GGZCallback func  : pointer to callback function to remove
 *
 * Returns:
 * int : 0 if successful, -1 on error
 */
int ggzcore_event_remove_callback(const GGZEventID id, const GGZCallback func)
{
	int status;

	status = _ggzcore_hook_remove(ggz_events[id].hooks, func);
	
	if (status == 0)
		ggzcore_debug(GGZ_DBG_EVENT, 
			      "Removing callback from event %s", 
			      ggz_events[id].name);
	else
		ggzcore_debug(GGZ_DBG_EVENT, 
			      "Can't find callback to remove from event %s",
			      ggz_events[id].name);
	
	return status; 
}


/* ggzcore_event_remove_callback_id() - Remove callback (specified by id) 
 *                                      from an event
 *
 * Receives:
 * GGZEventID id            : ID code of event
 * unsigned int callback_id : ID of callback to remove
 *
 * Returns:
 * int : 0 if successful, -1 on error
 */
int ggzcore_event_remove_callback_id(const GGZEventID id, 
				     const unsigned int callback_id)
{
	int status;

	status = _ggzcore_hook_remove_id(ggz_events[id].hooks, callback_id);
	
	if (status == 0)
		ggzcore_debug(GGZ_DBG_EVENT, 
			      "Removing callback %d from event %s", 
			      callback_id, ggz_events[id].name);
	else
		ggzcore_debug(GGZ_DBG_EVENT, 
			      "Can't find callback %d to remove from event %s",
			      callback_id, ggz_events[id].name);
	
	return status; 
}


/* _ggzcore_event_remove_all_callbacks() - Remove all callbacks from an event
 *
 * Receives:
 * GGZEventID id     : ID code of event
 *
 * Returns:
 * int : 0 if successful, -1 on error
 */
int _ggzcore_event_remove_all_callbacks(const GGZEventID id)
{
	ggzcore_debug(GGZ_DBG_EVENT, "Removing all callbacks from event %s", 
		      ggz_events[id].name);
	
	return _ggzcore_hook_remove_all(ggz_events[id].hooks);
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
	void* data;
	GGZDestroyFunc destroy;
	
	while (ggzcore_event_ispending()) {
		ggzcore_debug(GGZ_DBG_EVENT, "Procesing events");
		_ggzcore_event_dequeue(&id, &data, &destroy);
		/* FIXME: do validity checking on id */
		_ggzcore_event_process(id, data, destroy);
	}
	
	return 0;
}


/* _ggzcore_event_process() - Process an event (invoking callbacks)
 *
 * Receives:
 * GGZEventID id          : ID of event to process
 * void *data             : Pointer to common data for this event
 * GGZDestroyFunc destroy : Destroy function for common data
 *
 * Returns:
 * int : 0 if successful, -1 otherwise
 */
static void _ggzcore_event_process(GGZEventID id, void* data, 
				   GGZDestroyFunc destroy)
{
	ggzcore_debug(GGZ_DBG_EVENT, "Processing %s event/invoking callbacks",
		      ggz_events[id].name);
	
	if (_ggzcore_state_event_is_valid(id)) {
		_ggzcore_hook_list_invoke(ggz_events[id].hooks, data);
		_ggzcore_state_transition(id);
	}
	else {
		ggzcore_debug(GGZ_DBG_EVENT, "NOTE: Skipping invalid %s event",
			      ggz_events[id].name);
	}

	/* Free event specific data */
	if (data && destroy)
		(*destroy)(data);
}


/* ggzcore_event_enqueue() - Queue up an event for processing
 *
 * Receives:
 * GGZEventID id       : ID code of event
 * void* data          : Data specific to this occurance of the event
 * GGZDestroyFunc func : function to free event data
 *
 * Returns:
 * int : 0 if successful, -1 otherwise
 *
 * Note that event_data should *not* point to local variables, as they
 * will not be in scope when the event is processed
 */
int ggzcore_event_enqueue(const GGZEventID id, void* data, GGZDestroyFunc func)
{
	ggzcore_debug(GGZ_DBG_EVENT, "Queued event %s", ggz_events[id].name);
	
	/* Queue-up event in pipe */
	write(event_pipe[1], &id, sizeof(GGZEventID));
	write(event_pipe[1], &data, sizeof(void*));
	write(event_pipe[1], &func, sizeof(GGZDestroyFunc));

	return 0;
}


/* _ggzcore_event_dequeue() - Take next event from queue
 *
 * Receives:
 * GGZEventID *id       : Address to store ID of event
 * void** data          : Address to store pointer to event date
 * GGZDestroyFunc *func : Address to store pointer to destroy function
 *
 * Returns:
 * int : 0 if successful, -1 otherwise 
 *
 * Returns by reference the ID, event data, and destroy function for
 * the event
 */
static int _ggzcore_event_dequeue(GGZEventID *id, void **data, 
				  GGZDestroyFunc *func)
{
	read(event_pipe[0], id, sizeof(GGZEventID));
	read(event_pipe[0], data, sizeof(void*));
	read(event_pipe[0], func, sizeof(GGZDestroyFunc));

	ggzcore_debug(GGZ_DBG_EVENT, "Dequeued event %s", ggz_events[*id].name);

	return 0;
}


/* Debugging function to examine state of callback list */
static void _ggzcore_event_dump_callbacks(GGZEventID id)
{
	struct _GGZHook *cur;

	ggzcore_debug(GGZ_DBG_EVENT, "-- Event: %s", ggz_events[id].name);
	for (cur = ggz_events[id].hooks->hooks; cur != NULL; cur = cur->next)
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
		_ggzcore_hook_list_destroy(ggz_events[i].hooks);
		ggz_events[i].hooks = NULL;
	}
}
