/*
 * File: ggzcore.h
 * Author: Brent Hendricks
 * Project: GGZ Core Client Lib
 * Date: 9/15/00
 *
 * Interface file to be included by client frontends
 *
 * Copyright (C) 2000, 2001 Brent Hendricks.
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

#include <ggz_client.h>

/** @file ggzcore.h
 *  @brief The interface for the ggzcore library used by GGZ clients
 */

#ifdef __cplusplus
extern "C" {
#endif

/* GGZCore library features */
typedef enum {
	GGZ_OPT_PARSER      = 0x0001,   /* %0000 0000 */
	GGZ_OPT_MODULES     = 0x0002,   /* %0000 0010 */
	GGZ_OPT_THREADED_IO = 0x0004    /* %0000 0100 */
} GGZOptionFlags;


/** Debugging categories */
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
	GGZ_DBG_HOOK      = 0x00000800,
	GGZ_DBG_INIT      = 0x00001000,
	GGZ_DBG_MEMORY    = 0x00002000,
	GGZ_DBG_MEMDETAIL = 0x00004000,
	GGZ_DBG_MODULE    = 0x00008000,
	GGZ_DBG_GAME      = 0x00010000,
	GGZ_DBG_XML       = 0x00020000,
	GGZ_DBG_ALL       = 0xFFFFFFFF
} GGZDebugLevel;

/** Options structure for ggzcore library */
typedef struct _GGZOptions {
	
	/** Option flags */
	GGZOptionFlags flags;

	/** Debugging file */
	char* debug_file;

	/** Debugging levels */
	GGZDebugLevel debug_levels;

} GGZOptions;


/** ggzcore_init() - Initializtion function for ggzcore lib.
 *
 * @param options options structure
 *
 * @return int : 0 if successful, -1 on failure
 */
int ggzcore_init(GGZOptions options);


/** ggzcore_destroy() - Cleanup function for ggzcore lib.
 */
void ggzcore_destroy(void);


/** GGZ Hook function return types */
typedef enum {
      GGZ_HOOK_OK,
      GGZ_HOOK_REMOVE,
      GGZ_HOOK_ERROR,
      GGZ_HOOK_CRISIS
} GGZHookReturn;

/** GGZ Event hook function type, used as a vallback for events */
typedef GGZHookReturn (*GGZHookFunc)(unsigned int id, 
				     void* event_data, 
				     void* user_data);

/** GGZ object destroy function type */
typedef void (*GGZDestroyFunc)(void* data);


/**
 * This controls the type of login a user chooses.  A different
 * value will require different information to be sent to
 * the server.
 */
typedef enum {
	/** Standard login; uname and correct passwd needed. */
	GGZ_LOGIN,
	
	/** Guest login; only a uname is required. */	
	GGZ_LOGIN_GUEST,
	
	/** New user login; only a uname is required.  Password will be
	 *  assigned by the server. */
	GGZ_LOGIN_NEW
} GGZLoginType;


typedef enum {
	GGZ_PLAYER_NONE,
	GGZ_PLAYER_NORMAL,
	GGZ_PLAYER_GUEST,
	GGZ_PLAYER_ADMIN
} GGZPlayerType;

/**
 * A GGZServerEvent is an event triggered by a communication from the
 * server.  Each time an event occurs, the associated event handler
 * will be called.
 * @see ggzcore_server_add_event_hook
 */
typedef enum {
	/** We have just made a connection to the server.
	 *  @note This just means we've established a connection socket.
	 *  @see ggzcore_server_connect */
	GGZ_CONNECTED,
	
	/** Error: we have failed to connect to the server. */
	GGZ_CONNECT_FAIL,
	
	/** We have negotiated a connection to the server.
	 *  @note This just means we've determined ggzd is at the other end.
	 *  @see ggzcore_server_connect  */
	GGZ_NEGOTIATED,
	
	/** Error: negotiation failure.  Could be the wrong version... */
	GGZ_NEGOTIATE_FAIL,
	
	/** We have successfully logged in.  We can now start doing stuff.
	 *  @see ggzcore_server_login */
	GGZ_LOGGED_IN,
	
	/** Error: login failure */
	GGZ_LOGIN_FAIL,
	
	GGZ_MOTD_LOADED,
	GGZ_ROOM_LIST,
	GGZ_TYPE_LIST,
	
	/** We have successfully entered a room.
	 *  @see ggzcore_server_join_room */
	GGZ_ENTERED,
	
	/** Error: we have tried to enter a room and failed. */
	GGZ_ENTER_FAIL,
	GGZ_LOGOUT,
	
	/** Error: a network error occurred. */
	GGZ_NET_ERROR,
	
	/** Error: a communication protocol error occured. */
	GGZ_PROTOCOL_ERROR,
	
	GGZ_CHAT_FAIL,
	GGZ_STATE_CHANGE
} GGZServerEvent;


typedef enum {
	GGZ_PLAYER_LIST,
	GGZ_TABLE_LIST,
	GGZ_CHAT,
	GGZ_ANNOUNCE,
	GGZ_PRVMSG,
	GGZ_BEEP,
	GGZ_ROOM_ENTER,
	GGZ_ROOM_LEAVE,
	GGZ_TABLE_UPDATE,
	GGZ_TABLE_LAUNCHED,
	GGZ_TABLE_LAUNCH_FAIL,
	GGZ_TABLE_JOINED,
	GGZ_TABLE_JOIN_FAIL,
	GGZ_TABLE_LEFT,
	GGZ_TABLE_LEAVE_FAIL,
	GGZ_TABLE_DATA,
	GGZ_PLAYER_LAG
} GGZRoomEvent;


typedef enum {
	GGZ_GAME_LAUNCHED,
	GGZ_GAME_LAUNCH_FAIL,
	GGZ_GAME_NEGOTIATED,
	GGZ_GAME_NEGOTIATE_FAIL,
	GGZ_GAME_DATA,
	GGZ_GAME_OVER,
	GGZ_GAME_IO_ERROR,
	GGZ_GAME_PROTO_ERROR,
} GGZGameEvent;


typedef enum {
	GGZ_STATE_OFFLINE,
	GGZ_STATE_CONNECTING,
	GGZ_STATE_ONLINE,
	GGZ_STATE_LOGGING_IN,
	GGZ_STATE_LOGGED_IN,
	GGZ_STATE_ENTERING_ROOM,
	GGZ_STATE_IN_ROOM,
	GGZ_STATE_BETWEEN_ROOMS,
	GGZ_STATE_LAUNCHING_TABLE,
	GGZ_STATE_JOINING_TABLE,
	GGZ_STATE_AT_TABLE,
	GGZ_STATE_LEAVING_TABLE,
	GGZ_STATE_LOGGING_OUT,
} GGZStateID;

/** @brief Chat subops.
 *
 *  Each time we send a chat to the server, it will have one of these
 *  types associated with it.
 *  @see ggzcore_room_chat
 */					/* PMCCCCCC */
typedef enum {
	/** A normal chat, just a message sent to the current room. */
	GGZ_CHAT_NORMAL = 	0x40,	/* 01000000 */
	
	/** An announcement, usually triggered with /wall.  Only
	 *  admins can do this, and it is announced in all rooms. */
	GGZ_CHAT_ANNOUNCE =	0x60,	/* 01100000 */
	
	/** A beep.  We beep a player, and their client will run
	 *  the beep. */
	GGZ_CHAT_BEEP = 	0x80,	/* 10000000 */
	
	/** A personal message to another player.  It consists of both
	 *  a message and a target player. */
	GGZ_CHAT_PERSONAL =	0xC0,	/* 11000000 */
} GGZChatOp;



typedef struct _GGZNet      GGZNet;
typedef struct _GGZServer   GGZServer;
typedef struct _GGZRoom     GGZRoom;
typedef struct _GGZPlayer   GGZPlayer;
typedef struct _GGZTable    GGZTable;
typedef struct _GGZGameType GGZGameType;
typedef struct _GGZModule   GGZModule;
typedef struct _GGZGame     GGZGame;

/** @brief Create a new server object.
 *
 *  Call this function to create a server object.  This object holds
 *  all state data for communicating with a ggz server.  It is
 *  necessary for any kind of connection.
 */
GGZServer* ggzcore_server_new(void);

/** @brief Reset the server object.
 *
 *  After you've disconnected, call this function to discard all state
 *  data and reset the state of the server object.  You can then connect
 *  again.
 *  @note You should disconnect before resetting.
 */
int ggzcore_server_reset(GGZServer *server);

/*
 * Functions for attaching hooks to GGZServer events
 */

/** @brief Register a callback handler for a server event.
 *
 *  Call this function to register the given GGZHookFunc as a
 *  handler for the given event.  Then any time that event
 *  happens the handler function will be called.
 *  @param server The GGZ server object.
 *  @param event The server event to be handled.
 *  @param func The handler function to be called when the event occurs.
 *  @return A hook ID value to identify this handler.
 *  @note Equivalent to ggzcore_server_add_event_hook_full with data==NULL.
 *  @note More than one handler can be registered for each event.
 */
int ggzcore_server_add_event_hook(GGZServer *server,
				  const GGZServerEvent event, 
				  const GGZHookFunc func);

/** @brief Register a callback handler for a server event.
 *  @see ggzcore_server_add_event_hook
 *  @param data An arbitrary pointer that will be passed to the hook function.
 */					
int ggzcore_server_add_event_hook_full(GGZServer *server,
				       const GGZServerEvent event, 
				       const GGZHookFunc func,
				       void *data);

/* Functions for removing hooks from GGZServer events */
int ggzcore_server_remove_event_hook(GGZServer *server,
				     const GGZServerEvent event, 
				     const GGZHookFunc func);

int ggzcore_server_remove_event_hook_id(GGZServer *server,
					const GGZServerEvent event, 
					const unsigned int hook_id);

/* Functions for setting GGZServer data */
int ggzcore_server_set_hostinfo(GGZServer *server, 
				const char *host, 
				const unsigned int port);

int ggzcore_server_set_logininfo(GGZServer *server, 
				 const GGZLoginType type, 
				 const char *handle, 
				 const char *password);
int ggzcore_server_log_session(GGZServer *server, const char *filename);


/* Functions for querying a GGZServer object for information */
char*        ggzcore_server_get_host(GGZServer *server);
int          ggzcore_server_get_port(GGZServer *server);
GGZLoginType ggzcore_server_get_type(GGZServer *server);
char*        ggzcore_server_get_handle(GGZServer *server);
char*        ggzcore_server_get_password(GGZServer *server);
int          ggzcore_server_get_fd(GGZServer *server);
GGZStateID   ggzcore_server_get_state(GGZServer *server);

/** @brief Return the number of rooms on the server, or -1 on error.
 *  @note Until we retrieve the list of rooms, this will return 0.
 */
int          ggzcore_server_get_num_rooms(GGZServer *server);

/** @brief Return the current room, or NULL if there is none. */
GGZRoom*     ggzcore_server_get_cur_room(GGZServer *server);

/** @brief Return the nth room on the server, or NULL on error. */
GGZRoom*     ggzcore_server_get_nth_room(GGZServer *server, 
					 const unsigned int num);

int          ggzcore_server_get_num_gametypes(GGZServer *server);
GGZGameType* ggzcore_server_get_nth_gametype(GGZServer *server, 
					    const unsigned int num);

/* ggzcore_server_is_XXXX()
 * 
 * These functions return 1 if the server connection is in the
 * specified state, and 0 otherwise 
*/
int ggzcore_server_is_online(GGZServer *server);
int ggzcore_server_is_logged_in(GGZServer *server);
int ggzcore_server_is_in_room(GGZServer *server);
int ggzcore_server_is_at_table(GGZServer *server);

/* GGZ Server Actions */
int ggzcore_server_connect(GGZServer *server);
int ggzcore_server_login(GGZServer *server);
int ggzcore_server_motd(GGZServer *server);
int ggzcore_server_list_rooms(GGZServer *server, const int type, const char verbose);
int ggzcore_server_list_gametypes(GGZServer *server, const char verbose);
int ggzcore_server_join_room(GGZServer *server, const unsigned int room);
int ggzcore_server_logout(GGZServer *server);
int ggzcore_server_disconnect(GGZServer *server);

/* Functions for data processing */
int ggzcore_server_data_is_pending(GGZServer *server);
int ggzcore_server_read_data(GGZServer *server);
int ggzcore_server_write_data(GGZServer *server);

/* Free GGZServer object and accompanying data */
void ggzcore_server_free(GGZServer *server);


/* Functions for manipulating GGZRoom objects */


/** Allocate space for a new room object */
GGZRoom* ggzcore_room_new(void);

/** Initialize room object */
int ggzcore_room_init(GGZRoom *room, 
		      const GGZServer *server, 
		      const unsigned int id, 
		      const char *name, 
		      const unsigned int game, 
		      const char *desc);

/** De-allocate room object and its children */
void ggzcore_room_free(GGZRoom *room);


/* Functions for querying a GGZRoom object for information */
char*        ggzcore_room_get_name(GGZRoom *room);
char*        ggzcore_room_get_desc(GGZRoom *room);
GGZGameType* ggzcore_room_get_gametype(GGZRoom *room);

int        ggzcore_room_get_num_players(GGZRoom *room);
GGZPlayer* ggzcore_room_get_nth_player(GGZRoom *room, const unsigned int num);

int       ggzcore_room_get_num_tables(GGZRoom *room);
GGZTable* ggzcore_room_get_nth_table(GGZRoom *room, const unsigned int num);


/* Functions for manipulating hooks to GGZRoom events */
int ggzcore_room_add_event_hook(GGZRoom *room,
				const GGZRoomEvent event, 
				const GGZHookFunc func);
int ggzcore_room_add_event_hook_full(GGZRoom *room,
				     const GGZRoomEvent event, 
				     const GGZHookFunc func,
				     void *data);
int ggzcore_room_remove_event_hook(GGZRoom *room,
				   const GGZRoomEvent event, 
				   const GGZHookFunc func);
int ggzcore_room_remove_event_hook_id(GGZRoom *room,
				      const GGZRoomEvent event, 
				      const unsigned int hook_id);


/* Functions for performing actions on a room */
int ggzcore_room_list_players(GGZRoom *room);
int ggzcore_room_list_tables(GGZRoom *room, 
			     const int type,
			     const char global);

int ggzcore_room_chat(GGZRoom *room,
		      const GGZChatOp opcode,
		      const char *player,
		      const char *msg);

int ggzcore_room_launch_table(GGZRoom *room, GGZTable *table);
int ggzcore_room_join_table(GGZRoom *room, const unsigned int num);
int ggzcore_room_leave_table(GGZRoom *room);
int ggzcore_room_send_game_data(GGZRoom *room, char *buffer);


/* Functions for manipulating GGZPlayer objects */

char*         ggzcore_player_get_name(GGZPlayer *player);
GGZPlayerType ggzcore_player_get_type(GGZPlayer *player);
GGZTable*     ggzcore_player_get_table(GGZPlayer *player);
int	      ggzcore_player_get_lag(GGZPlayer *player);


GGZTable* ggzcore_table_new(void);
int ggzcore_table_init(GGZTable *table,
		       GGZGameType *gametype,
		       char *desc,
		       const unsigned int num_seats);
			 
void ggzcore_table_free(GGZTable *table);

int ggzcore_table_add_player(GGZTable *table, 
			    char *name, 
			     const unsigned int seat);
int ggzcore_table_add_bot(GGZTable *table, 
			  char *name, 
			  const unsigned int seat);
int ggzcore_table_add_reserved(GGZTable *table, 
			       char *name, 
			       const unsigned int seat);
int ggzcore_table_remove_player(GGZTable *table, char *name);

int          ggzcore_table_get_id(GGZTable *table);
GGZGameType* ggzcore_table_get_type(GGZTable *table);
char*        ggzcore_table_get_desc(GGZTable *table);
char         ggzcore_table_get_state(GGZTable *table);
int          ggzcore_table_get_num_seats(GGZTable *table);
int          ggzcore_table_get_num_open(GGZTable *table);
int          ggzcore_table_get_num_bots(GGZTable *table);
char*        ggzcore_table_get_nth_player_name(GGZTable *table, 
					       const unsigned int num);
GGZSeatType  ggzcore_table_get_nth_player_type(GGZTable *table, 
					       const unsigned int num);


/* These function are lookups to gametype information. */
char* ggzcore_gametype_get_name(GGZGameType *type);
char* ggzcore_gametype_get_prot_engine(GGZGameType *type);
char* ggzcore_gametype_get_prot_version(GGZGameType *type);
char* ggzcore_gametype_get_version(GGZGameType *type);
char* ggzcore_gametype_get_author(GGZGameType *type);
char* ggzcore_gametype_get_url(GGZGameType *type);
char* ggzcore_gametype_get_desc(GGZGameType *type);

/* Return the maximum number of allowed players/bots */
int ggzcore_gametype_get_max_players(GGZGameType *type);
int ggzcore_gametype_get_max_bots(GGZGameType *type);

/* Verify that a paticular number of players/bots is valid */
int ggzcore_gametype_num_players_is_valid(GGZGameType *type, unsigned int num);
int ggzcore_gametype_num_bots_is_valid(GGZGameType *type, unsigned int num);


/* ggzcore_error_sys()
 * ggzcore_error_sys_exit()
 * ggzcore_error_msg()
 * ggzcore_error_msg_exit()
 * 
 * Receives:
 * GGZEventID id        : ID code of event
 * unsigned int hook_id : ID of hook to remove
 *
 * Returns:
 * int : 0 if successful, -1 on error
 */
void ggzcore_debug(const GGZDebugLevel level, const char *fmt, ...);
void ggzcore_error_sys(const char *fmt, ...);
void ggzcore_error_sys_exit(const char *fmt, ...);
void ggzcore_error_msg(const char *fmt, ...);
void ggzcore_error_msg_exit(const char *fmt, ...);


/* ggzcore_conf_initialize()
 *	Opens the global and/or user configuration files for the frontend.
 *	Either g_path or u_path can be NULL if the file is not to be used.
 *	The user config file will be created if it does not exist.
 *
 *	@return:
 *	  -1 on error
 *	  0 on success
 */
int ggzcore_conf_initialize	(const char	*g_path,
				 const char	*u_path);

/** ggzcore_conf_write_string() - Write a string to the user config file
 *
 * @param section	section to store value in
 * @param key		key to store value under
 * @param value		value to store
 *
 * @return int : 0 if successful, -1 on error
 */
int ggzcore_conf_write_string(const char *section, 
			      const char *key, 
			      const char *value);

/** ggzcore_conf_write_int() - Write a integer to the user config file
 *
 * @param section	section to store value in
 * @param key		key to store value under
 * @param value		value to store
 *
 * @return int : 0 if successful, -1 on error
 */
int ggzcore_conf_write_int(const char *section, 
			   const char *key, 
			   int value);

/** ggzcore_conf_write_list() - Write a list to the user config file
 *
 * @param section	section to store value in
 * @param key		key to store value under
 * @param argc		count of string arguments in list
 * @param argv		array of NULL terminated strings
 *
 * @return int : 0 if successful, -1 on error
 */
int ggzcore_conf_write_list(const char *section, 
			    const char *key, 
			    int argc, 
			    char **argv);

/** ggzcore_conf_read_string() - Read a string from the configuration file(s)
 *
 * @param section	section to get value from
 * @param key		key value was stored under
 * @param def		default value to return if none is found
 *
 * @return
 *   a dynamically allocated string from the configuration file
 * OR
 *   a dynamically allocated copy of the default string
 *
 * @note The default may be set to NULL, in which case a NULL will be
 * returned if the value could not be found in either configuration file.
 */
char * ggzcore_conf_read_string(const char *section, 
				const char *key, 
				const char *def);

/** ggzcore_conf_read_int() - Read a integer from the configuration file(s)
 *
 * @param section	section to get value from
 * @param key		key value was stored under
 * @param def		default value to return if none is found
 *
 * @return
 *   an integer from the configuration file
 * OR
 *   the default value
 *
 * @note There is no guaranteed way to find if the call failed.  If you
 * must know, call ggzcore_conf_read_string with a NULL default value and
 * check for the NULL return.
 */
int ggzcore_conf_read_int(const char *section, const char *key, int def);

/** ggzcore_conf_read_list() - Read a list from the configuration file(s)
 *
 * @param section	section to get value from
 * @param key		key value was stored under
 * @param argcp		ptr to int which will receive the list entry count
 * @param argvp		a pointer to a dynamically allocated array
 * 			that ggzcore_conf_read_list() will build
 *
 * @return int : 0 if successful, -1 on error
 */
int ggzcore_conf_read_list(const char *section, 
			   const char *key, 
			   int *argcp, 
			   char ***argvp);

/** ggzcore_conf_remove_section() - Removes a section from the user config file
 *
 * @param section	section to remove
 *
 * @return
 * int : 0 if successful, -1 on error, 1 on soft error (section did not exist)
 */
int ggzcore_conf_remove_section(const char *section);

/** ggzcore_conf_remove_key() - Removes a key entry from the user config file
 *
 * @param section	section to remove
 * @param key		key entry to remove
 *
 * @return
 * int : 0 if success, -1 on error, 1 on soft error (section/key didn't exist)
 */
int ggzcore_conf_remove_key(const char *section, const char *key);

/** ggzcore_conf_commit() - Commits the core user config file to disk
 *
 * @return:
 * int : 0 if successful, -1 on error
 */
int ggzcore_conf_commit(void);



/* This returns the number of registered modules */
unsigned int ggzcore_module_get_num(void);


/* This adds a local module to the list.  It returns 0 if successful or
   -1 on failure. */
int ggzcore_module_add(const char *name,
	               const char *version,
	               const char *prot_engine,
	               const char *prot_version,
                       const char *author,
		       const char *frontend,
		       const char *url,
		       const char *exe_path,
		       const char *icon_path,
		       const char *help_path);		       


/* Returns how many modules support this game and protocol */
int ggzcore_module_get_num_by_type(const char *game, 
				   const char *engine,
				   const char *version);

/* Returns n-th module that supports this game and protocol */
GGZModule* ggzcore_module_get_nth_by_type(const char *game, 
					  const char *engine,
					  const char *version,
					  const unsigned int num);


/* This attempts to launch the specified module and returns 0 is
   successful or -1 on error. */
int ggzcore_module_launch(GGZModule *module);


/* These functions lookup a particular property of a module.  I've added
   icon to the list we discussed at the meeting.  This is an optional xpm
   file that the module can provide to use for representing the game
   graphically.*/
char* ggzcore_module_get_game(GGZModule *module);
char* ggzcore_module_get_version(GGZModule *module);
char* ggzcore_module_get_prot_engine(GGZModule *module);
char* ggzcore_module_get_prot_version(GGZModule *module);
char* ggzcore_module_get_author(GGZModule *module);
char* ggzcore_module_get_frontend(GGZModule *module);
char* ggzcore_module_get_url(GGZModule *module);
char* ggzcore_module_get_icon_path(GGZModule *module);
char* ggzcore_module_get_help_path(GGZModule *module);
char** ggzcore_module_get_argv(GGZModule *module);


GGZGame* ggzcore_game_new(void);
int ggzcore_game_init(GGZGame *game, GGZModule *module);
void ggzcore_game_free(GGZGame *game);

/* Functions for attaching hooks to GGZGame events */
int ggzcore_game_add_event_hook(GGZGame *game,
				const GGZGameEvent event, 
				const GGZHookFunc func);

int ggzcore_game_add_event_hook_full(GGZGame *game,
				     const GGZGameEvent event, 
				     const GGZHookFunc func,
				     void *data);

/* Functions for removing hooks from GGZGame events */
int ggzcore_game_remove_event_hook(GGZGame *game,
				   const GGZGameEvent event, 
				   const GGZHookFunc func);

int ggzcore_game_remove_event_hook_id(GGZGame *game,
				      const GGZGameEvent event, 
				      const unsigned int hook_id);


/* Functions for data processing */
int ggzcore_game_data_is_pending(GGZGame *game);
int ggzcore_game_read_data(GGZGame *game);
int ggzcore_game_write_data(GGZGame *game);


int        ggzcore_game_get_fd(GGZGame *game);
GGZModule* ggzcore_game_get_module(GGZGame *game);


int ggzcore_game_launch(GGZGame *game);
int ggzcore_game_join(GGZGame *game);
int ggzcore_game_send_data(GGZGame *game, char *buffer);
			   

#ifdef __cplusplus
}
#endif 

#endif  /* __GGZCORE_H__ */
