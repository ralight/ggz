/*
 * File: server.c
 * Author: Brent Hendricks
 * Project: GGZ Core Client Lib
 * Date: 1/19/01
 *
 * Code for handling server connection state and properties
 *
 * Copyright (C) 2001 Brent Hendricks.
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

#include "ggzcore.h"
#include "hook.h"
#include "state.h"
#include "server.h"
#include "protocol.h"
#include "net.h"
#include "errno.h"
#include "lists.h"
#include "player.h"

#include <stdlib.h>
#include <string.h>
#include <easysock.h>


/* Functions for manipulating list of rooms */
static void _ggzcore_server_init_roomlist(struct _GGZServer *server,
					  const int num);
static void _ggzcore_server_free_roomlist(struct _GGZServer *server);
static void _ggzcore_server_add_room(struct _GGZServer *server, 
				     struct _GGZRoom *room);

/* Functions for manipulating list of gametypes */
static void _ggzcore_server_init_typelist(struct _GGZServer *server, 
					  const int num);
static void _ggzcore_server_free_typelist(struct _GGZServer *server);
static void _ggzcore_server_add_type(struct _GGZServer *server, 
				     struct _GGZGameType *type);


static int _ggzcore_server_event_is_valid(GGZServerEvent event);
static void _ggzcore_server_change_state(GGZServer *server, GGZTransID trans);
static GGZHookReturn _ggzcore_server_event(GGZServer*, GGZServerEvent, void*);


/* Handlers for various server commands */
static void _ggzcore_server_handle_server_id(GGZServer *server);
static void _ggzcore_server_handle_login(GGZServer *server);
static void _ggzcore_server_handle_login_anon(GGZServer *server);
static void _ggzcore_server_handle_motd(GGZServer *server);
static void _ggzcore_server_handle_logout(GGZServer *server);
static void _ggzcore_server_handle_list_rooms(GGZServer *server);
static void _ggzcore_server_handle_room_join(GGZServer *server);
static void _ggzcore_server_handle_list_players(GGZServer *server);
static void _ggzcore_server_handle_rsp_chat(GGZServer *server);
static void _ggzcore_server_handle_chat(GGZServer *server);
static void _ggzcore_server_handle_update_players(GGZServer *server);
static void _ggzcore_server_handle_update_tables(GGZServer *server);
static void _ggzcore_server_handle_list_tables(GGZServer *server);
static void _ggzcore_server_handle_list_types(GGZServer *server);


/* Array of GGZServerEvent messages */
static char* _ggzcore_server_events[] = {
	"GGZ_CONNECTED",
	"GGZ_CONNECT_FAIL",
	"GGZ_NEGOTIATED",
	"GGZ_NEGOTIATE_FAIL",
	"GGZ_LOGGED_IN",
	"GGZ_LOGIN_FAIL",
	"GGZ_MOTD_LOADED",
	"GGZ_ROOM_LIST",
	"GGZ_TYPE_LIST",
	"GGZ_ENTERED",
	"GGZ_ENTER_FAIL",
	"GGZ_LOGOUT",
	"GGZ_NET_ERROR",
	"GGZ_STATE_CHANGE"
};


/* Total number of server events messages */
static unsigned int _ggzcore_num_events = sizeof(_ggzcore_server_events)/sizeof(char*);

/* The GGZServer structure manages information about a particular
   GGZ server connection */
struct _GGZServer {
	
	/* Host name of server */
	char *host;

	/* Port on which GGZ server in running */
	unsigned int port;

	/* Login type: one of GGZ_LOGIN, GGZ_LOGIN_GUEST, GGZ_LOGIN_NEW */
	GGZLoginType type;

	/* User handle on this server */
	char* handle;

	/* Password for this server (optional) */
	char* password;

	/* File descriptor for communication with server */
	int fd;
	
	/* Current state */
	GGZStateID state;
	
	/* Number of gametypes */
	int num_gametypes;

	/* List of game types */
	struct _GGZGameType **gametypes;

	/* Gametype verbosity (need to save) */
	char gametype_verbose;

	/* Number of rooms */
	int num_rooms;
	
	/* List of rooms in this server */
	struct _GGZRoom **rooms;
	
	/* Room verbosity (need to save) */
	char room_verbose;

	/* Current room on game server */
	struct _GGZRoom *room;

	/* Room to which we are transitioning */
	struct _GGZRoom *new_room;

       	/* Server events */
	GGZHookList *event_hooks[sizeof(_ggzcore_server_events)/sizeof(char*)];

};


/* Datatype for a message from the server */
struct _GGZServerMsg {

	/* Opcode from server */
	GGZServerOp opcode;

	/* Name of opcode */
	char* name;

	/* Handler */
	void (*handler)(GGZServer *server); 
};

/* Array of all server messages */
static struct _GGZServerMsg _ggzcore_server_msgs[] = {
	{MSG_SERVER_ID,      "msg_server_id", _ggzcore_server_handle_server_id},
	{MSG_SERVER_FULL,    "msg_server_full", NULL },
	{MSG_MOTD,           "msg_motd", _ggzcore_server_handle_motd},
	{MSG_CHAT,           "msg_chat", _ggzcore_server_handle_chat},
	{MSG_UPDATE_PLAYERS, "msg_update_players", _ggzcore_server_handle_update_players},
	{MSG_UPDATE_TYPES,   "msg_update_types", NULL},
	{MSG_UPDATE_TABLES,  "msg_update_tables", _ggzcore_server_handle_update_tables},
	{MSG_UPDATE_ROOMS,   "msg_update_rooms", NULL},
	{MSG_ERROR,          "msg_error", NULL},
	{RSP_LOGIN_NEW,      "rsp_login_new", NULL},
	{RSP_LOGIN,          "rsp_login", _ggzcore_server_handle_login},
	{RSP_LOGIN_ANON,     "rsp_login_anon", _ggzcore_server_handle_login_anon},
	{RSP_LOGOUT,         "rsp_logout", _ggzcore_server_handle_logout},
	{RSP_PREF_CHANGE,    "rsp_pref_change", NULL},
	{RSP_REMOVE_USER,    "rsp_remove_user", NULL},
	{RSP_LIST_PLAYERS,   "rsp_list_players", _ggzcore_server_handle_list_players},
	{RSP_LIST_TYPES,     "rsp_list_types", _ggzcore_server_handle_list_types},
	{RSP_LIST_TABLES,    "rsp_list_tables", _ggzcore_server_handle_list_tables},
	{RSP_LIST_ROOMS,     "rsp_list_rooms", _ggzcore_server_handle_list_rooms},
	{RSP_TABLE_OPTIONS,  "rsp_table_options", NULL},
	{RSP_USER_STAT,      "rsp_user_stat", NULL},
	{RSP_TABLE_LAUNCH,   "rsp_table_launch", NULL},
	{RSP_TABLE_JOIN,     "rsp_table_join", NULL},
	{RSP_TABLE_LEAVE,    "rsp_table_leave", NULL},
	{RSP_GAME,           "rsp_game", NULL},
	{RSP_CHAT,           "rsp_chat", _ggzcore_server_handle_rsp_chat},
        {RSP_MOTD,           "rsp_motd", _ggzcore_server_handle_motd},
	{RSP_ROOM_JOIN,      "rsp_room_join", _ggzcore_server_handle_room_join}
};


/* Total number of recognized messages */
static unsigned int _ggzcore_num_messages = sizeof(_ggzcore_server_msgs)/sizeof(struct _GGZServerMsg);


/* Publicly exported functions */

GGZServer* ggzcore_server_new(const char *host,
			      const unsigned int port,
			      const GGZLoginType type,
			      const char *handle,
			      const char *password)
{
	int i;
	GGZServer *server;
	
	/* Return nothing if we didn't get the necessary info */
	if (host == NULL || handle == NULL)
		return NULL;

	if (type == GGZ_LOGIN && password == NULL)
		return NULL;
		
	/* Allocate and zero space for GGZServer object */
	server = ggzcore_malloc(sizeof(GGZServer));
	
	server->host = strdup(host);
	server->port = port;
	server->type = type;
	server->handle = strdup(handle);
	if (type == GGZ_LOGIN)
		server->password = strdup(password);
	server->state = GGZ_STATE_OFFLINE;
	server->fd = -1;
	server->num_rooms = 0;

	ggzcore_debug(GGZ_DBG_INIT, "New GGZServer: %s:%d, %d, %s, %s",
		      server->host, server->port, server->type, server->handle,
		      server->password);
	
	/* Setup event hook lists */
	for (i = 0; i < _ggzcore_num_events; i++)
		server->event_hooks[i] = _ggzcore_hook_list_init(i);

	return server;
}


int ggzcore_server_add_event_hook(GGZServer *server,
				  const GGZServerEvent event, 
				  const GGZHookFunc func)
{
	if (!server)
		return -1;

	return ggzcore_server_add_event_hook_full(server, event, func, NULL);
}


int ggzcore_server_add_event_hook_full(GGZServer *server,
				       const GGZServerEvent event, 
				       const GGZHookFunc func,
				       void *data)
{
	if (!server)
		return -1;

	if (!_ggzcore_server_event_is_valid(event))
		return -1;

	return _ggzcore_hook_add_full(server->event_hooks[event], func, data);
}


int ggzcore_server_remove_event_hook(GGZServer *server,
				     const GGZServerEvent event, 
				     const GGZHookFunc func)
{
	if (!server)
		return -1;

	if (!_ggzcore_server_event_is_valid(event))
		return -1;

	return _ggzcore_hook_remove(server->event_hooks[event], func);
}


int ggzcore_server_remove_event_hook_id(GGZServer *server,
					    const GGZServerEvent event, 
					    const unsigned int hook_id)
{
	if (!server)
		return -1;

	if (!_ggzcore_server_event_is_valid(event))
		return -1;

	return _ggzcore_hook_remove_id(server->event_hooks[event], hook_id);
}


void ggzcore_server_free(GGZServer *server)
{
	int i;

	if (!server)
		return;

	free(server->host);
	free(server->handle);
	if (server->password)
		free(server->password);

	if (server->num_rooms > 0)
		_ggzcore_server_free_roomlist(server);

	if (server->num_gametypes > 0)
		_ggzcore_server_free_typelist(server);

	for (i = 0; i < _ggzcore_num_events; i++)
		_ggzcore_hook_list_destroy(server->event_hooks[i]);

	ggzcore_free(server);
}


char* ggzcore_server_get_host(GGZServer *server)
{
	if (!server)
		return NULL;
	
	return server->host;
}


int ggzcore_server_get_port(GGZServer *server)
{
	if (!server)
		return -1;

	return server->port;
}


GGZLoginType ggzcore_server_get_type(GGZServer *server)
{
	if (!server)
		return -1;

	return server->type;
}

char* ggzcore_server_get_handle(GGZServer *server)
{
	if (!server)
		return NULL;

	return server->handle;
}


char* ggzcore_server_get_password(GGZServer *server)
{
	if (!server)
		return NULL;

	return server->password;
}


/* ggzcore_server_get_fd() - Get a copy of the network socket
 * Receives:
 *
 * Returns:
 * int : network socket fd
 *
 * Note: this is for detecting network data arrival only.  Do *NOT* attempt
 * to write to this fd.
 */
int ggzcore_server_get_fd(GGZServer *server)
{
	if (!server)
		return -1;

	return server->fd;
}


GGZStateID ggzcore_server_get_state(GGZServer *server)
{
	if (!server)
		return -1;

	return server->state;
}


GGZRoom* ggzcore_server_get_cur_room(GGZServer *server)
{
	if (!server)
		return NULL;

	return server->room;
}


GGZRoom* ggzcore_server_get_nth_room(GGZServer *server, const unsigned int num)
{
	if (!server || num >= server->num_rooms)
		return NULL;

	return server->rooms[num];
}


int ggzcore_server_get_num_rooms(GGZServer *server)
{
	if (!server)
		return -1;

	return server->num_rooms;
}


char** ggzcore_server_get_room_names(GGZServer *server)
{
	int i = 0;
	char **names = NULL;
	struct _GGZRoom *room;
	
	if (!server)
		return NULL;
	
	if (!(names = calloc((server->num_rooms + 1), sizeof(char*))))
		ggzcore_error_sys_exit("calloc() failed in ggzcore_server_get_room_names");

	for (i = 0; i < server->num_rooms; i++) {
		room = server->rooms[i];
		names[i] = _ggzcore_room_get_name(room);
	}
	
	return names;
}


int ggzcore_server_get_num_gametypes(GGZServer *server)
{
	if (!server)
		return -1;

	return server->num_gametypes;
}


char** ggzcore_server_get_gametype_names(GGZServer *server)
{
	int i = 0;
	char **names = NULL;
	struct _GGZGameType *type;
	
	if (!server)
		return NULL;
	
	if (!(names = calloc((server->num_gametypes + 1), sizeof(char*))))
		ggzcore_error_sys_exit("calloc() failed in ggzcore_server_get_gametype_names");

	for (i = 0; i < server->num_gametypes; i++) {
		type = server->gametypes[i];
		names[i] = _ggzcore_gametype_get_name(type);
	}
		
	return names;
}


GGZGameType* ggzcore_server_get_nth_gametype(GGZServer *server, 
					    const unsigned int num)
{
	if (!server || num >= server->num_gametypes)
		return NULL;

	return server->gametypes[num];
}


int ggzcore_server_is_online(GGZServer *server)
{
	if (!server)
		return 0;

	return (server->state >= GGZ_STATE_ONLINE);
}


int ggzcore_server_is_logged_in(GGZServer *server)
{
	if (!server)
		return 0;

	return (server->state >= GGZ_STATE_LOGGED_IN);
}


int ggzcore_server_is_in_room(GGZServer *server)
{
	if (!server)
		return 0;

	return (server->state >= GGZ_STATE_IN_ROOM
		&& server->state < GGZ_STATE_LOGGING_OUT);
}


int ggzcore_server_is_at_table(GGZServer *server)
{
	if (!server)
		return 0;

	return (server->state >= GGZ_STATE_AT_TABLE
		&& server->state <= GGZ_STATE_LEAVING_TABLE);
}


void ggzcore_server_connect(GGZServer *server)
{
	/* FIXME: check validity of this action */

	_ggzcore_server_change_state(server, GGZ_TRANS_CONN_TRY);

	server->fd = _ggzcore_net_connect(server->host, server->port);
	
	if (server->fd < 0) {
		_ggzcore_server_change_state(server, GGZ_TRANS_CONN_FAIL);
		_ggzcore_server_event(server, GGZ_CONNECT_FAIL, 
				      strerror(errno));
	}
	else {
		_ggzcore_server_event(server, GGZ_CONNECTED, NULL);
	}
}


void ggzcore_server_login(GGZServer *server)
{
	/* FIXME: check validity of this action */
	int status; 

	if (!server)
		return;

	status = _ggzcore_net_send_login(server->fd, server->type, 
					 server->handle,  server->password);
	/* FIXME: handle errors */

	_ggzcore_server_change_state(server, GGZ_TRANS_LOGIN_TRY);
}


void ggzcore_server_motd(GGZServer *server)
{
	int status;

	if (!server)
		return;

	status = _ggzcore_net_send_motd(server->fd);
	/* FIXME: handle errors */
}


void ggzcore_server_list_rooms(GGZServer *server, 
			       const int type, 
			       const char verbose)
{
	/* FIXME: check validity of this action */
	int status;

	if (!server)
		return;

	server->room_verbose = verbose;
	status = _ggzcore_net_send_list_rooms(server->fd, type, verbose);
	/* FIXME: handle errors */
}


void ggzcore_server_list_gametypes(GGZServer *server, const char verbose)
{
	int status;

	if (!server)
		return;

	server->gametype_verbose = verbose;
	status = _ggzcore_net_send_list_types(server->fd, verbose);
	/* FIXME: handle errors */
}


void ggzcore_server_join_room(GGZServer *server, const int room)
{
	/* FIXME: check validity of this action */
	int status;

	if (!server)
		return;

	/* FIXME: check validty of room */
	status = _ggzcore_net_send_join_room(server->fd, room);
	/* FIXME: handle errors */

	server->new_room = ggzcore_server_get_nth_room(server, room);
	
	_ggzcore_server_change_state(server, GGZ_TRANS_ENTER_TRY);
}


void ggzcore_server_logout(GGZServer *server)
{
	/* FIXME: check validity of this action */
	int status;

	if (!server)
		return;

	if (server->state == GGZ_STATE_OFFLINE)
		return;

	status = _ggzcore_net_send_logout(server->fd);
	/* FIXME: handle errors */

	_ggzcore_server_change_state(server, GGZ_TRANS_LOGOUT_TRY);
}


int ggzcore_server_data_is_pending(GGZServer *server)
{
	int status = 0;
	struct pollfd fd[1] = {{server->fd, POLLIN, 0}};
	
	if (!server)
		return 0;

	if (server->fd == -1)
		return 0;

	ggzcore_debug(GGZ_DBG_POLL, "Checking for net events");	
	if ( (status = poll(fd, 1, 0)) < 0) {
		if (errno == EINTR) 
			/* Ignore interruptions */
			status = 0;
		else 
			ggzcore_error_sys_exit("poll failed in ggzcore_server_data_is_pending");
	}
	else if (status)
		ggzcore_debug(GGZ_DBG_POLL, "Found a net event!");

	return status;
}


int ggzcore_server_read_data(GGZServer *server)
{
	GGZServerOp opcode;
	void (*handler)(GGZServer *server);
	int status;

	if (!server)
		return -1;

	ggzcore_debug(GGZ_DBG_SERVER, "Processing server events");
	status = _ggzcore_net_read_opcode(server->fd, &opcode);
	/* FIXME: check for errors */
	
	if (opcode < 0 || opcode >= _ggzcore_num_messages) {
		ggzcore_debug(GGZ_DBG_SERVER, "Bad opcode from server %d/%d", opcode, _ggzcore_num_messages);
		/* FIXME: issue protocol error */
		return -1;
	}
	
	/* Call handler function */
	if (!(handler = _ggzcore_server_msgs[opcode].handler))
		ggzcore_debug(GGZ_DBG_SERVER, "Server msg: %s not handled yet", 
			      _ggzcore_server_msgs[opcode].name);
	else {
		ggzcore_debug(GGZ_DBG_SERVER, "Handling %s", 
			      _ggzcore_server_msgs[opcode].name);
		handler(server);
	}
	
	return 0;
}

int ggzcore_server_write_data(GGZServer *server)
{
	return 0;
}


/* Internal library functions (prototypes in server.h) */

void _ggzcore_server_list_players(GGZServer *server)
{
	int status;

	status = _ggzcore_net_send_list_players(server->fd);
}


void _ggzcore_server_list_tables(GGZServer *server, const int type, const char global)
{
	int status;

	status = _ggzcore_net_send_list_tables(server->fd, type, global);
}


struct _GGZRoom* _ggzcore_server_get_room_by_id(struct _GGZServer *server,
						const unsigned int id)
{
	return NULL;
}

struct _GGZGameType* _ggzcore_server_get_type_by_id(struct _GGZServer *server,
						    const unsigned int id)
{
	int i;

	for (i = 0; i < server->num_gametypes; i++)
		if (_ggzcore_gametype_get_id(server->gametypes[i]) == id)
			return server->gametypes[i];

	return NULL;
}


void _ggzcore_server_chat(GGZServer *server, 
			  const GGZChatOp opcode,
			  const char *player,
			  const char *msg)
{
	int status;

	status = _ggzcore_net_send_chat(server->fd, opcode, player, msg);
	/* FIXME: check for errors */
}



/* Static functions internal to this file */

static void _ggzcore_server_init_roomlist(struct _GGZServer *server,
					  const int num)
{
	server->num_rooms = num;
	server->rooms = ggzcore_malloc(num * sizeof(struct _GGZRoom*));
}


static void _ggzcore_server_free_roomlist(struct _GGZServer *server)
{
	int i;

	for (i = 0; i < server->num_rooms; i++) {
		_ggzcore_room_free(server->rooms[i]);
	}

	ggzcore_free(server->rooms);
	server->num_rooms = 0;
}


static void _ggzcore_server_add_room(struct _GGZServer *server, 
				     struct _GGZRoom *room)
{
	int i = 0;

	/* Find first open spot and stick it in */
	while (i < server->num_rooms) {
		if (server->rooms[i] == NULL) {
			server->rooms[i] = room;
			break;
		}
		++i;
	}
}


static void _ggzcore_server_init_typelist(struct _GGZServer *server, 
					  const int num)
{
	server->num_gametypes = num;
	server->gametypes = ggzcore_malloc(num * sizeof(struct _GGZGameType*));
}


static void _ggzcore_server_free_typelist(struct _GGZServer *server)
{
	int i;

	for (i = 0; i < server->num_gametypes; i++) {
		_ggzcore_gametype_free(server->gametypes[i]);
	}

	ggzcore_free(server->gametypes);
	server->num_gametypes = 0;
}


static void _ggzcore_server_add_type(struct _GGZServer *server, 
				     struct _GGZGameType *type)
{
	int i = 0;

	/* Find first open spot and stick it in */
	while (i < server->num_gametypes) {
		if (server->gametypes[i] == NULL) {
			server->gametypes[i] = type;
			break;
		}
		++i;
	}
}

static void _ggzcore_server_change_state(GGZServer *server, GGZTransID trans)
{
	_ggzcore_state_transition(trans, &server->state);
	_ggzcore_server_event(server, GGZ_STATE_CHANGE, NULL);
}


static int _ggzcore_server_event_is_valid(GGZServerEvent event)
{
	return (event >= 0 && event < _ggzcore_num_events);
}
	

static GGZHookReturn _ggzcore_server_event(GGZServer *server, 
					   GGZServerEvent id, 
					   void *data)
{
	return _ggzcore_hook_list_invoke(server->event_hooks[id], data);
}


static void _ggzcore_server_handle_server_id(GGZServer *server)
{
	int status, protocol;

	status = _ggzcore_net_read_server_id(server->fd, &protocol);
	/* FIXME: handle errors */

	if (protocol == GGZ_CS_PROTO_VERSION) {
		_ggzcore_server_change_state(server, GGZ_TRANS_CONN_OK);
		_ggzcore_server_event(server, GGZ_NEGOTIATED, NULL);
	}
	else {
		_ggzcore_server_change_state(server, GGZ_TRANS_CONN_FAIL);
		_ggzcore_server_event(server, GGZ_NEGOTIATE_FAIL, "Protocol mismatch");
	}
}


static void _ggzcore_server_handle_login(GGZServer *server)
{
#if 0	
	switch (status) {
	case 0:
		reservations = (int)res;
		ggzcore_event_enqueue(GGZ_SERVER_LOGIN, (void*)reservations, NULL);
		break;
	case E_ALREADY_LOGGED_IN:
		ggzcore_event_enqueue(GGZ_SERVER_LOGIN_FAIL, 
				      "Already logged in", NULL);
		break;
	case E_USR_LOOKUP:
		ggzcore_event_enqueue(GGZ_SERVER_LOGIN_FAIL, 
				      "Name taken", NULL);
		break;
	}

	/* FIXME: Add to its own function */
	/* Get list of game types */
	es_write_int(fd, REQ_LIST_TYPES);
	es_write_char(fd, 1);
#endif
}

static void _ggzcore_server_handle_login_anon(GGZServer *server)
{
	char ok;
	int status;

	status = _ggzcore_net_read_login_anon(server->fd, &ok);
	/* FIXME: handle errors */

	ggzcore_debug(GGZ_DBG_SERVER, "Status of login: %d", ok);

	switch (ok) {
	case 0:
		_ggzcore_server_change_state(server, GGZ_TRANS_LOGIN_OK);
		_ggzcore_server_event(server, GGZ_LOGGED_IN, NULL);
		break;
	case E_ALREADY_LOGGED_IN:
		_ggzcore_server_change_state(server, GGZ_TRANS_LOGIN_FAIL);
		_ggzcore_server_event(server, GGZ_LOGIN_FAIL, "Already logged in");
		break;
	case E_USR_LOOKUP:
		_ggzcore_server_change_state(server, GGZ_TRANS_LOGIN_FAIL);
		_ggzcore_server_event(server, GGZ_LOGIN_FAIL, "Name taken");
		break;
	}
}


static void _ggzcore_server_handle_logout(GGZServer *server)
{
	int status;
	char ok;

	status = _ggzcore_net_read_logout(server->fd, &ok);
	/* FIXME: handle errors */

	_ggzcore_net_disconnect(server->fd);
	server->fd = -1;

	_ggzcore_server_change_state(server, GGZ_TRANS_LOGOUT_OK);
	_ggzcore_server_event(server, GGZ_LOGOUT, NULL);
}


static void _ggzcore_server_handle_motd(GGZServer *server)
{
	int status, lines;
	char **motd;

	status = _ggzcore_net_read_motd(server->fd, &lines, &motd);
	/* FIXME: handle errors */
	
	/* FIXME: store somewhere */
	_ggzcore_server_event(server, GGZ_MOTD_LOADED, motd);
}


static void _ggzcore_server_handle_list_rooms(GGZServer *server)
{
	int i, num, id, game, status;
	char *name;
	char *desc = NULL;
	struct _GGZRoom *room;

	/* Clear existing list (if any) */
	if (server->num_rooms > 0)
		_ggzcore_server_free_roomlist(server);

	status = _ggzcore_net_read_num_rooms(server->fd, &num);
	/* FIXME: handle errors */

	ggzcore_debug(GGZ_DBG_SERVER, "Server sending %d rooms", num);
	_ggzcore_server_init_roomlist(server, num);

	for (i = 0; i < num; i++) {
		status = _ggzcore_net_read_room(server->fd, 
						server->room_verbose,
						&id, &name, &game, &desc);
		/* FIXME: handle errors */
		
		ggzcore_debug(GGZ_DBG_ROOM, "Adding room %d to room list", id);
		
		room = _ggzcore_room_new();
		_ggzcore_room_init(room, server, id, name, game, desc);
		_ggzcore_server_add_room(server, room);
		
		/* Free allocated memory */
		if (desc)
			free(desc);
		if (name)
			free(name);
	}
	_ggzcore_server_event(server, GGZ_ROOM_LIST, NULL);
}


static void _ggzcore_server_handle_list_types(GGZServer *server)
{
	int i, num, status;
	struct _GGZGameType *type;
	
	/* Free previous list of types*/
	if (server->num_gametypes > 0)
		_ggzcore_server_free_typelist(server);

	status = _ggzcore_net_read_num_types(server->fd, &num);
	/* FIXME: handle errors */

	ggzcore_debug(GGZ_DBG_SERVER, "Server sending %d types", num);
	_ggzcore_server_init_typelist(server, num);

	for(i = 0; i < num; i++) {
		type = _ggzcore_gametype_new();
		status = _ggzcore_net_read_type(server->fd, type);
		/* FIXME: handle errors */
		
		_ggzcore_server_add_type(server, type);
	}

	_ggzcore_server_event(server, GGZ_TYPE_LIST, NULL);
}


static void _ggzcore_server_handle_room_join(GGZServer *server)
{
	char ok;
	int status;

	status = _ggzcore_net_read_room_join(server->fd, &ok);
	/* FIXME: handle errors */

	ggzcore_debug(GGZ_DBG_SERVER, "Status of room join: %d", ok);

	switch (status) {
	case 0:
		/* Stop monitoring old room and start monitoring new one */
		if (server->room)
			_ggzcore_room_set_monitor(server->room, 0);
		_ggzcore_room_set_monitor(server->new_room, 1);
		server->room = server->new_room;
		_ggzcore_server_change_state(server, GGZ_TRANS_ENTER_OK);
		_ggzcore_server_event(server, GGZ_ENTERED, NULL);
		break;
	case E_AT_TABLE:
		server->new_room = server->room;
		_ggzcore_server_change_state(server, GGZ_TRANS_ENTER_FAIL);
		_ggzcore_server_event(server, GGZ_ENTER_FAIL,
				      "Can't change rooms while at a table");
		break;
		
	case E_IN_TRANSIT:
		server->new_room = server->room;
		_ggzcore_server_change_state(server, GGZ_TRANS_ENTER_FAIL);
		_ggzcore_server_event(server, GGZ_ENTER_FAIL,
				      "Can't change rooms while joining/leaving a table");
		break;
		
	case E_BAD_OPTIONS:
		server->new_room = server->room;
		_ggzcore_server_change_state(server, GGZ_TRANS_ENTER_FAIL);
		_ggzcore_server_event(server, GGZ_ENTER_FAIL, 
				      "Bad room number");
				      
		break;
	}

}


static void _ggzcore_server_handle_chat(GGZServer *server)
{
	GGZChatOp op;
	char *name, *msg;
	int status;
	
	status = _ggzcore_net_read_chat(server->fd, &op, &name, &msg);
	/* FIXME: handle errors */
	
	_ggzcore_room_add_chat(server->room, op, name, msg);
	
	if (name)
		free(name);

	if (msg)
		free(msg);
}


static void _ggzcore_server_handle_list_players(GGZServer *server)
{
	int i, num, table, status;
	char *name;
	struct _ggzcore_list *list;
	struct _GGZPlayer *player;

	/* New list of players */
	list = _ggzcore_list_create(_ggzcore_player_compare, NULL,
				    _ggzcore_player_destroy, 0);
	
	status = _ggzcore_net_read_num_players(server->fd, &num);
	/* FIXME: handle errors */
	ggzcore_debug(GGZ_DBG_SERVER, "Server sending %d players", num);


	for (i = 0; i < num; i++) {
		status = _ggzcore_net_read_player(server->fd, &name, &table);
		player = _ggzcore_player_new();
		_ggzcore_player_init(player, name, server->room, table);
		_ggzcore_list_insert(list, player);
		
		if (name)
			free(name);
	}

	_ggzcore_room_set_player_list(server->room, num, list);
}


static void _ggzcore_server_handle_update_players(GGZServer *server)
{
	GGZUpdateOp op;
	char *name;
	int status;
	GGZRoom *room;

	/* Use current room (until server tells which one update is for) */
	room = server->room;

	status = _ggzcore_net_read_update_players(server->fd, &op, &name);
	/* FIXME: handler errors */

	switch (op) {
	case GGZ_UPDATE_DELETE:
		ggzcore_debug(GGZ_DBG_SERVER, "UPDATE_PLAYER: %s left", name);
		_ggzcore_room_remove_player(room, name);
		break;

	case GGZ_UPDATE_ADD:
		ggzcore_debug(GGZ_DBG_SERVER, "UPDATE_PLAYER: %s enter", name);
		_ggzcore_room_add_player(room, name);
		break;
	default:
		/* FIXME: handle invalid opcode? */
	}
	
	if (name)
		free(name);
}


static void _ggzcore_server_handle_list_tables(GGZServer *server)
{
	int i, num, status;
	struct _ggzcore_list *list;
	struct _GGZTable *table;
	
	list = _ggzcore_list_create(_ggzcore_table_compare, NULL,
				    _ggzcore_table_destroy, 0);

	
	status = _ggzcore_net_read_num_tables(server->fd, &num);
	/* FIXME: handle errors */
	ggzcore_debug(GGZ_DBG_SERVER, "Server sending %d tables", num);
	
	for (i = 0; i < num; i++) {
		table = _ggzcore_table_new();
		status = _ggzcore_net_read_table(server->fd, table); 
		_ggzcore_list_insert(list, table);
	}
	
	_ggzcore_room_set_table_list(server->room, num, list);
}


static void _ggzcore_server_handle_update_tables(GGZServer *server)
{
	_ggzcore_net_read_update_tables(server->fd);
}


/* completely bogus functions to avoid messiness */
static void _ggzcore_server_handle_rsp_chat(GGZServer *server)
{
	char status;

	_ggzcore_net_read_rsp_chat(server->fd, &status);

#if 0
	switch (status) {
	case 0:
		ggzcore_event_enqueue(GGZ_SERVER_CHAT, NULL, NULL);
		break;

	case E_NOT_IN_ROOM:
		ggzcore_event_enqueue(GGZ_SERVER_CHAT_FAIL, "Not in a room",
				      NULL);
		break;

	case E_BAD_OPTIONS:
		ggzcore_event_enqueue(GGZ_SERVER_CHAT_FAIL, "Bad options",
				      NULL);
		break;
	}
#endif
}

