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
//extern "C" {
#endif 


/* GGZCore library features */
typedef enum {
	GGZ_OPT_PARSER      = 1,   /* %0000 0000 */
	GGZ_OPT_MODULES     = 2,   /* %0000 0010 */
	GGZ_OPT_THREADED_IO = 4    /* %0000 0100 */
} GGZOptionFlags;


/* IDs for all GGZ events */
typedef enum {
	GGZ_SERVER_CONNECT,
	GGZ_SERVER_LOGIN_OK,
	GGZ_SERVER_LOGIN_FAIL, 
	GGZ_USER_LOGIN, 
	GGZ_USER_CHAT,
	GGZ_USER_LOGOUT
} GGZEventID;


/* Event-callback function type */
typedef void (*GGZEventFunc)(GGZEventID id, void* event_data, void* user_data);


typedef enum {
	GGZ_LOGIN,
	GGZ_LOGIN_GUEST,
	GGZ_LOGIN_NEW
} GGZLoginType;


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


/* ggzcore_init() - Initializtion function for ggzcore lib.
 *
 * Receives:
 * GGZOptionsFlags options : bitmask of enabled features
 * char* global_conf       : Path to system configuration file
 * char* local_conf        : Path to user configuration file
 *
 * Returns:
 * int : 0 if successful, -1 on failure
 */
int ggzcore_init(GGZOptionFlags options, 
		 const char* global_conf, 
		 const char* local_conf);
	

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
int ggzcore_event_connect(const GGZEventID id, 
			  const GGZEventFunc func, 
			  void* user_data);


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


/* ggzcore_event_pending() - Determine if there are any pending events
 *
 * Receives:
 *
 * Returns:
 * int : 1 if there is at least one event pending, 0 otherwise
 */
int ggzcore_event_pending(void);


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
 * GGZEventID id     : ID code of event
 * void* event_data  : Data specific to this occurance of the event
 * unsigned int size : Number of bytes of event data
 *
 * Returns:
 * int : 0 if successful, -1 otherwise
 */
int ggzcore_event_trigger(const GGZEventID id, 
			  void* event_data, 
			  const unsigned int size);


void ggzcore_debug(const char *fmt, ...);

void ggzcore_error_sys(const char *fmt, ...);

void ggzcore_error_sys_exit(const char *fmt, ...);

void ggzcore_error_msg(const char *fmt, ...);

void ggzcore_error_msg_exit(const char *fmt, ...);


/* ggzcore_destroy() - Cleanup function for ggzcore lib.
 *
 * Receives:
 *
 * Returns:
 */
void ggzcore_destroy(void);


#ifdef __cplusplus
}
#endif 

#endif  /* __GGZCORE_H__ */
