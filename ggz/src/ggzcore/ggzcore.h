/*
 * File: ggzcore.h
 * Author: Brent Hendricks
 * Project: GGZ Core Client Lib
 * Date: 9/15/00
 *
 * Interface file to be included by client frontends
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

#ifndef __GGZCORE_H__
#define __GGZCORE_H__

#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif 


/* GGZCore library features */
typedef enum {
	GGZ_OPT_PARSER      = 0x0001,   /* %0000 0000 */
	GGZ_OPT_MODULES     = 0x0002,   /* %0000 0010 */
	GGZ_OPT_THREADED_IO = 0x0004    /* %0000 0100 */
} GGZOptionFlags;


/* Options structure for ggzcore library */
typedef struct _GGZOptions {
	
	/* Option flags */
	GGZOptionFlags flags;

	/* Global config file */
	char* global_conf;

	/* User config file */
	char* user_conf;

	/* Local Override conf file */
	char* local_conf;

} GGZOptions;


/* ggzcore_init() - Initializtion function for ggzcore lib.
 *
 * Receives:
 * GGZOption options : options structure
 *
 * Returns:
 * int : 0 if successful, -1 on failure
 */
int ggzcore_init(GGZOptions options);


/* ggzcore_destroy() - Cleanup function for ggzcore lib.
 *
 * Receives:
 *
 * Returns:
 */
void ggzcore_destroy(void);



/* IDs for all GGZ events */
typedef enum {
	GGZ_SERVER_CONNECT,
	GGZ_SERVER_CONNECT_FAIL,
	GGZ_SERVER_MOTD,
	GGZ_SERVER_LOGIN,
	GGZ_SERVER_LOGIN_FAIL, 
	GGZ_SERVER_LIST_ROOMS,
	GGZ_SERVER_ROOM_JOIN,
	GGZ_SERVER_ROOM_JOIN_FAIL,
	GGZ_SERVER_CHAT,
	GGZ_SERVER_CHAT_FAIL,
	GGZ_SERVER_CHAT_MSG,
	GGZ_SERVER_CHAT_ANNOUNCE,
	GGZ_SERVER_CHAT_PRVMSG,
	GGZ_SERVER_CHAT_BEEP,
	GGZ_SERVER_LOGOUT,
	GGZ_SERVER_ERROR,
	GGZ_NET_ERROR,
	GGZ_USER_LOGIN, 
	GGZ_USER_LIST_ROOMS,
	GGZ_USER_LIST_TYPES,
	GGZ_USER_JOIN_ROOM,
	GGZ_USER_LIST_TABLES,
	GGZ_USER_LIST_PLAYERS,
	GGZ_USER_CHAT,
	GGZ_USER_CHAT_PRVMSG,
	GGZ_USER_CHAT_BEEP,
	GGZ_USER_LOGOUT
} GGZEventID;


/* Event-callback function type */
typedef void (*GGZEventFunc)(GGZEventID id, void* event_data, void* user_data);
typedef void (*GGZDestroyFunc)(void* data);


typedef enum {
	GGZ_LOGIN,
	GGZ_LOGIN_GUEST,
	GGZ_LOGIN_NEW
} GGZLoginType;

typedef enum {
	GGZ_SEAT_OPEN   = -1,
	GGZ_SEAT_COMP   = -2,
	GGZ_SEAT_RESV   = -3,
	GGZ_SEAT_NONE   = -4,
	GGZ_SEAT_PLAYER = -5
} GGZSeatType;

/* The GGZProfile describes a server/login profile */
typedef struct _GGZProfile {
	
	/* Name of profile */
	char* name;
	
	/* Hostname of GGZ server */
	char* host;

	/* Port on which GGZ server in running */
	unsigned int port;

	/* Login type: one of GGZ_LOGIN, GGZ_LOGIN_GUEST, GGZ_LOGIN_NEW */
	GGZLoginType type;

	/* Login name on this server */
	char* login;

	/* Password for this server (optional) */
	char* password;

} GGZProfile;



/* ggzcore_event_connect() - Register callback for specific GGZ-event
 *
 * Receives:
 * GGZEventID id         : ID code of event
 * GGZEventFunc callback : Callback function
 *
 * Returns:
 * int : id for this callback 
 */
int ggzcore_event_connect(const GGZEventID id, const GGZEventFunc callback);
			  

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
int ggzcore_event_connect_full(const GGZEventID id, 
			       const GGZEventFunc callback, 
			       void* user_data,
			       GGZDestroyFunc destroy);


/* ggzcore_event_remove_all() - Remove all callbacks from an event
 *
 * Receives:
 * GGZEventID id     : ID code of event
 *
 * Returns:
 * int : 0 if successful, -1 on error
 */
int ggzcore_event_remove_all(const GGZEventID id);


/* ggzcore_event_remove() - Remove specific callback from an event
 *
 * Receives:
 * GGZEventID id            : ID code of event
 * unsigned int callback_id : ID of callback to remove
 *
 * Returns:
 * int : 0 if successful, -1 on error
 */
int ggzcore_event_remove(const GGZEventID id, const unsigned int callback_id);


/* ggzcore_event_ispending() - Determine if there are any pending events
 *
 * Receives:
 *
 * Returns:
 * int : 1 if there is at least one event pending, 0 otherwise
 */
int ggzcore_event_ispending(void);


/* ggzcore_event_process_all() - Process all pending events
 *
 * Receives:
 *
 * Returns:
 * int : 0 if successful, -1 otherwise
 */
int ggzcore_event_process_all(void);


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
int ggzcore_event_trigger(const GGZEventID id, 
			  void* event_data,
			  GGZDestroyFunc func);


/* ggzcore_state_is_XXXX()
 * 
 * These functions return 1 if ggzcore is in the specified state, and 
 * 0 otherwise 
 */
int ggzcore_state_is_online(void);
int ggzcore_state_is_logged_in(void);
int ggzcore_state_is_in_room(void);
int ggzcore_state_is_at_table(void);


char* ggzcore_state_get_profile_login(void);
char* ggzcore_state_get_profile_name(void);
char* ggzcore_state_get_profile_host(void);
int ggzcore_state_get_room(void);


/* Debugging categories */
typedef enum {
	GGZ_DBG_EVENT  = 0x00000001,
	GGZ_DBG_NET    = 0x00000002,
	GGZ_DBG_USER   = 0x00000004,
	GGZ_DBG_SERVER = 0x00000008,
	GGZ_DBG_CONF   = 0x00000010,
	GGZ_DBG_POLL   = 0x00000020,
	GGZ_DBG_STATE  = 0x00000040,
	GGZ_DBG_ALL    = 0xFFFFFFFF
} GGZDebugLevel;


void ggzcore_debug(const GGZDebugLevel level, const char *fmt, ...);

void ggzcore_error_sys(const char *fmt, ...);

void ggzcore_error_sys_exit(const char *fmt, ...);

void ggzcore_error_msg(const char *fmt, ...);

void ggzcore_error_msg_exit(const char *fmt, ...);



#ifdef __cplusplus
}
#endif 

#endif  /* __GGZCORE_H__ */
