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
#include <poll.h>

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
	GGZ_SERVER_LIST_PLAYERS,
	GGZ_SERVER_CHAT,
	GGZ_SERVER_CHAT_FAIL,
	GGZ_SERVER_CHAT_MSG,
	GGZ_SERVER_CHAT_ANNOUNCE,
	GGZ_SERVER_CHAT_PRVMSG,
	GGZ_SERVER_CHAT_BEEP,
	GGZ_SERVER_LOGOUT,
	GGZ_SERVER_ERROR,
	GGZ_SERVER_ROOM_ENTER,
	GGZ_SERVER_ROOM_LEAVE,
	GGZ_SERVER_TABLE_UPDATE,
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
	GGZ_USER_MOTD,
	GGZ_USER_LOGOUT
} GGZEventID;


/* Event-callback function type */
typedef void (*GGZCallback)(unsigned int id, 
			    void* callback_data, 
			    void* user_data);
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



/* ggzcore_event_add_callback() - Register callback for specific GGZ-event
 *
 * Receives:
 * GGZEventID id         : ID code of event
 * GGZCallback callback : Callback function
 *
 * Returns:
 * int : id for this callback 
 */
int ggzcore_event_add_callback(const GGZEventID id, const GGZCallback func);
			  

/* ggzcore_event_add_callback_full() - Register callback for specific GGZ-event
 *                                     specifying all parameters
 * 
 * Receives:
 * GGZEventID id         : ID code of event
 * GGZCallback callback : Callback function
 * void* user_data       : "User" data to pass to callback
 * GGZDestroyFunc func   : function to call to free user data
 *
 * Returns:
 * int : id for this callback 
 */
int ggzcore_event_add_callback_full(const GGZEventID id, 
				    const GGZCallback func, 
				    void* user_data,
				    GGZDestroyFunc destroy);


/* ggzcore_event_remove_callback() - Remove specific callback from an event
 * ggzcore_event_remove_callback_id() - Remove specific callback from an event
 *
 * Receives:
 * GGZEventID id            : ID code of event
 * unsigned int callback_id : ID of callback to remove
 *
 * Returns:
 * int : 0 if successful, -1 on error
 */
int ggzcore_event_remove_callback(const GGZEventID id, const GGZCallback func);
int ggzcore_event_remove_callback_id(const GGZEventID id, 
				     const unsigned int callback_id);



/* ggzcore_event_get_fd() - Get a copy of the event pipe fd
 * Receives:
 *
 * Returns:
 * int : event pipe fd
 *
 * Note: this is for detecting event arrivals only.  Do *NOT* attempt
 * to write to this fd.
 */
int ggzcore_event_get_fd(void);


/* ggzcore_event_ispending() - Determine if there are pending GGZ events
 *
 * Receives:
 *
 * Returns:
 * int : 1 if there is at least one event pending, 0 otherwise
 */
int ggzcore_event_ispending(void);


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
int ggzcore_event_poll(struct pollfd *ufds, unsigned int nfds, int timeout);


/* ggzcore_event_process_all() - Process all pending events
 *
 * Receives:
 *
 * Returns:
 * int : 0 if successful, -1 otherwise
 */
int ggzcore_event_process_all(void);


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
int ggzcore_event_enqueue(const GGZEventID id, void *data, 
			  const GGZDestroyFunc func);


typedef enum {
	GGZ_STATE_OFFLINE,
	GGZ_STATE_CONNECTING,
	GGZ_STATE_ONLINE,
	GGZ_STATE_LOGGING_IN,
	GGZ_STATE_LOGGED_IN,
	GGZ_STATE_ENTERING_ROOM,
	GGZ_STATE_IN_ROOM,
	GGZ_STATE_BETWEEN_ROOMS,
	GGZ_STATE_JOINING_TABLE,
	GGZ_STATE_AT_TABLE,
	GGZ_STATE_LEAVING_TABLE,
	GGZ_STATE_LOGGING_OUT,
	GGZ_STATE_NONE
} GGZStateID;


int ggzcore_state_add_callback(const GGZStateID id, const GGZCallback func);
int ggzcore_state_add_callback_full(const GGZStateID id, 
				    const GGZCallback func, 
				    void* user_data,
				    GGZDestroyFunc destroy);


int ggzcore_state_remove_callback(const GGZStateID id, const GGZCallback func);
int ggzcore_state_remove_callback_id(const GGZStateID id, 
				     const unsigned int callback_id);


/* ggzcore_state_is_XXXX()
 * 
 * These functions return 1 if ggzcore is in the specified state, and 
 * 0 otherwise 
 */
int ggzcore_state_is_online(void);
int ggzcore_state_is_logged_in(void);
int ggzcore_state_is_in_room(void);
int ggzcore_state_is_at_table(void);


GGZStateID ggzcore_state_get_id(void);
char* ggzcore_state_get_profile_login(void);
char* ggzcore_state_get_profile_name(void);
char* ggzcore_state_get_profile_host(void);
int ggzcore_state_get_room(void);


/* ggzcore_net_get_fd() - Get a copy of the network socket
 * Receives:
 *
 * Returns:
 * int : network socket fd
 *
 * Note: this is for detecting network data arrival only.  Do *NOT* attempt
 * to write to this fd.
 */
int ggzcore_net_get_fd(void);

unsigned int ggzcore_room_get_num(void);
char* ggzcore_room_get_name(const unsigned int);
char* ggzcore_room_get_desc(const unsigned int);
char** ggzcore_room_get_names(void);
int ggzcore_room_get_gametype(const unsigned int id);

unsigned int ggzcore_player_get_num(void);
char** ggzcore_player_get_names(void);
int ggzcore_player_get_table(char *name);


/* Debugging categories */
typedef enum {
	GGZ_DBG_EVENT     = 0x00000001,
	GGZ_DBG_NET       = 0x00000002,
	GGZ_DBG_USER      = 0x00000004,
	GGZ_DBG_SERVER    = 0x00000008,
	GGZ_DBG_CONF      = 0x00000010,
	GGZ_DBG_POLL      = 0x00000020,
	GGZ_DBG_STATE     = 0x00000040,
	GGZ_DBG_PLAYER    = 0x00000080,
	GGZ_DBG_ROOM      = 0x00000100,
	GGZ_DBG_TABLE     = 0x00000200,
	GGZ_DBG_GAMETYPE  = 0x00000400,
	GGZ_DBG_ALL       = 0xFFFFFFFF
} GGZDebugLevel;


void ggzcore_debug(const GGZDebugLevel level, const char *fmt, ...);

void ggzcore_error_sys(const char *fmt, ...);

void ggzcore_error_sys_exit(const char *fmt, ...);

void ggzcore_error_msg(const char *fmt, ...);

void ggzcore_error_msg_exit(const char *fmt, ...);


/* ggzcore_conf_write_string() - Write a string to the user config file
 *
 * Receives:
 * char *section            : section to store value in
 * char *key                : key to store value under
 * char *value              : value to store
 *
 * Returns:
 * int : 0 if successful, -1 on error
 */
int ggzcore_conf_write_string(char *section, char *key, char *value);

/* ggzcore_conf_write_int() - Write a integer to the user config file
 *
 * Receives:
 * char *section            : section to store value in
 * char *key                : key to store value under
 * int value                : value to store
 *
 * Returns:
 * int : 0 if successful, -1 on error
 */
int ggzcore_conf_write_int(char *section, char *key, int value);

/* ggzcore_conf_write_list() - Write a list to the user config file
 *
 * Receives:
 * char *section            : section to store value in
 * char *key                : key to store value under
 * int argc                 : count of string arguments in list
 * char **argv              : array of NULL terminated strings
 *
 * Returns:
 * int : 0 if successful, -1 on error
 */
int ggzcore_conf_write_list(char *section, char *key, int argc, char **argv);

/* ggzcore_conf_read_string() - Read a string from the configuration file(s)
 *
 * Receives:
 * char *section            : section to get value from
 * char *key                : key value was stored under
 * char *def                : default value to return if none is found
 *
 * Returns:
 *   a dynamically allocated string from the configuration file
 * OR
 *   a dynamically allocated copy of the default string
 *
 * Note that the default may be set to NULL, in which case a NULL will be
 * returned if the value could not be found in either configuration file.
 */
char * ggzcore_conf_read_string(char *section, char *key, char *def);

/* ggzcore_conf_read_int() - Read a integer from the configuration file(s)
 *
 * Receives:
 * char *section            : section to get value from
 * char *key                : key value was stored under
 * int def                  : default value to return if none is found
 *
 * Returns:
 *   an integer from the configuration file
 * OR
 *   the default value
 *
 * Note that there is no guaranteed way to find if the call failed.  If you
 * must know, call ggzcore_conf_read_string with a NULL default value and
 * check for the NULL return.
 */
int ggzcore_conf_read_int(char *section, char *key, int def);

/* ggzcore_conf_read_list() - Read a list from the configuration file(s)
 *
 * Receives:
 * char *section            : section to get value from
 * char *key                : key value was stored under
 * int *argcp               : ptr to int which will receive the list entry count
 * char ***argvp            : a pointer to a dynamically allocated array
 *                            that ggzcore_conf_read_list() will build
 *
 * Returns:
 * int : 0 if successful, -1 on error
 */
int ggzcore_conf_read_list(char *section, char *key, int *argcp, char ***argvp);

/* ggzcore_conf_remove_section() - Removes a section from the user config file
 *
 * Receives:
 * char *section            : section to remove
 *
 * Returns:
 * int : 0 if successful, -1 on error, 1 on soft error (section did not exist)
 */
int ggzcore_conf_remove_section(char *section);

/* ggzcore_conf_remove_key() - Removes a key entry from the user config file
 *
 * Receives:
 * char *section            : section to remove
 * char *key                : key entry to remove
 *
 * Returns:
 * int : 0 if success, -1 on error, 1 on soft error (section/key didn't exist)
 */
int ggzcore_conf_remove_key(char *section, char *key);

/* ggzcore_conf_remove_key() - Removes a key entry from the user config file
 *
 * Receives:	No arguments
 *
 * Returns:
 * int : 0 if successful, -1 on error
 */
int ggzcore_conf_commit(void);


/* This returns the number of registered modules */
unsigned int ggzcore_module_get_num(void);


/* This adds a local module to the list.  It returns 0 if successful or
   -1 on failure. */
int ggzcore_module_add(const char *game,
	               const char *version,
	               const char *protocol,
                       const char *author,
		       const char *frontend,
		       const char *url,
		       const char *exe_path,
		       const char *icon_path,
		       const char *help_path);
		       


/*This returns a dynamically allocated array of module IDs for the
  version of the game specified, or NULL if there are no modules
  registered for that game/version.  The last element in the array is
  the flag value -1.*/
int* ggzcore_module_get_by_type(const char *game, const char *version);


/* This attempts to launch the specified module and returns 0 is
   successful or -1 on error. */
int ggzcore_module_launch(const unsigned int id);


/* These functions lookup a particular property of a module.  I've added
   icon to the list we discussed at the meeting.  This is an optional xpm
   file that the module can provide to use for representing the game
   graphically.*/
char* ggzcore_module_get_game(const unsigned int id);
char* ggzcore_module_get_version(const unsigned int id);
char* ggzcore_module_get_protocol(const unsigned int id);
char* ggzcore_module_get_author(const unsigned int id);
char* ggzcore_module_get_frontend(const unsigned int id);
char* ggzcore_module_get_url(const unsigned int id);
char* ggzcore_module_get_path(const unsigned int id);
char* ggzcore_module_get_icon_path(const unsigned int id);
char* ggzcore_module_get_help_path(const unsigned int id);


/* These function are lookups to table information. All you need pass
   is the table number.*/
unsigned int ggzcore_table_get_num(void);
int ggzcore_table_get_seats(int number);
int ggzcore_table_get_open(int number);
char *ggzcore_table_get_desc(int number);
int *ggzcore_table_get_numbers(void);
int ggzcore_table_get_gametype(int number);


/* These function are lookups to gametype information. All you need pass
   is the index number.*/
unsigned int ggzcore_gametype_get_num(void);
char* ggzcore_gametype_get_name(const unsigned int id);
char* ggzcore_gametype_get_author(const unsigned int id);
char* ggzcore_gametype_get_url(const unsigned int id);
char* ggzcore_gametype_get_desc(const unsigned int id);
char** ggzcore_gametype_get_names(void);

#ifdef __cplusplus
}
#endif 

#endif  /* __GGZCORE_H__ */
