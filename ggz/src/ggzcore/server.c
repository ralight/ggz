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
#include "event.h"
#include "hook.h"
#include "state.h"
#include "server.h"
#include "protocol.h"
#include "net.h"
#include "errno.h"
#include "roomlist.h"

#include <stdlib.h>
#include <string.h>
#include <easysock.h>

/* Pricate functions */
static void _ggzcore_server_init(void);
static int _ggzcore_server_event_is_valid(GGZServerEvent event);
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
	{MSG_SERVER_ID, "msg_server_id", _ggzcore_server_handle_server_id},
	{MSG_SERVER_FULL, "msg_server_full", NULL },
	{MSG_MOTD, "msg_motd", _ggzcore_server_handle_motd},
	{MSG_CHAT, "msg_chat", _ggzcore_server_handle_chat},
	{MSG_UPDATE_PLAYERS, "msg_update_players", _ggzcore_server_handle_update_players},
	{MSG_UPDATE_TYPES, "msg_update_types", NULL},
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
static unsigned int _ggzcore_num_messages;


/* Publicly exported functions */

GGZServer* ggzcore_server_new(const char *host,
			      const unsigned int port,
			      const GGZLoginType type,
			      const char *handle,
			      const char *password)
{
	int i;
	GGZServer *server;
	
	/* If we haven't already initialized, do it now */
	if (_ggzcore_num_messages == 0)
		_ggzcore_server_init();
	
	/* Return nothing if we didn't get the necessary info */
	if (host == NULL || handle == NULL)
		return NULL;

	if (type == GGZ_LOGIN && password == NULL)
		return NULL;
		
	/* Allocate and zero space for GGZServer object */
	if (!(server = calloc(1, sizeof(GGZServer))))
		ggzcore_error_sys_exit("malloc() failed in ggzcore_server_new");
	
	server->host = strdup(host);
	server->port = port;
	server->type = type;
	server->handle = strdup(handle);
	if (type == GGZ_LOGIN)
		server->password = strdup(password);
	server->state = GGZ_STATE_OFFLINE;
	server->fd = -1;
	server->room = -1;

	ggzcore_debug(GGZ_DBG_INIT, "New GGZServer: %s:%d, %d, %s, %s",
		      server->host, server->port, server->type, server->handle,
		      server->password);
	
	/* Setup event hook lists */
	for (i = 0; i < GGZ_NUM_SERVER_EVENTS; i++)
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
	/* Free room list */
	/* Free type list */
	for (i = 0; i < GGZ_NUM_SERVER_EVENTS; i++)
		_ggzcore_hook_list_destroy(server->event_hooks[i]);

	free(server);
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


int ggzcore_server_get_cur_room(GGZServer *server)
{
	if (!server)
		return -1;

	return server->room;
}


int ggzcore_server_get_num_rooms(GGZServer *server)
{
	if (!server)
		return -1;

	return _ggzcore_roomlist_get_num(server->room_list);
}

GGZRoomList* ggzcore_server_get_roomlist(GGZServer *server)
{
	if (!server)
		return NULL;

	return server->room_list;
}


char** ggzcore_server_get_room_names(GGZServer *server)
{
	if (!server)
		return NULL;

	return _ggzcore_roomlist_get_room_names(server->room_list);
}


char* ggzcore_server_get_room_name(GGZServer *server, const unsigned int id)
{
	if (!server || !server->room_list)
		return NULL;
	
	return _ggzcore_roomlist_get_room_name(server->room_list, id);
}

				    
char* ggzcore_server_get_room_desc(GGZServer *server, const unsigned int id)
{
	if (!server || !server->room_list)
		return NULL;
	
	return _ggzcore_roomlist_get_room_desc(server->room_list, id);
}

				    
int ggzcore_server_get_room_gametype(GGZServer *server, const unsigned int id)
{
	if (!server || !server->room_list)
		return -1;
	
	return _ggzcore_roomlist_get_room_type(server->room_list, id);
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

	_ggzcore_state_transition(GGZ_TRANS_CONN_TRY, &server->state);
	server->fd = _ggzcore_net_connect(server->host, server->port);
	
	if (server->fd < 0) {
		_ggzcore_state_transition(GGZ_TRANS_CONN_FAIL, &server->state);
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

	_ggzcore_state_transition(GGZ_TRANS_LOGIN_TRY, &server->state);
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


void ggzcore_server_join_room(GGZServer *server, const int room)
{
	/* FIXME: check validity of this action */
	int status;

	if (!server)
		return;

	/* FIXME: check validty of room */
	status = _ggzcore_net_send_join_room(server->fd, room);
	/* FIXME: handle errors */

	server->new_room = room;
	_ggzcore_state_transition(GGZ_TRANS_ENTER_TRY, &server->state);
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

	_ggzcore_state_transition(GGZ_TRANS_LOGOUT_TRY, &server->state);
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

/* Internal library functions (prototypes in connection.h) */

/* Static functions internal to this file */

/* FIXME: Handle threaded I/O */
static void _ggzcore_server_init(void)
{
	/* Obtain accurate message count */
	_ggzcore_num_messages = sizeof(_ggzcore_server_msgs)/sizeof(struct _GGZServerMsg);
}


static int _ggzcore_server_event_is_valid(GGZServerEvent event)
{
	return (event >= 0 && event < GGZ_NUM_SERVER_EVENTS);
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
		_ggzcore_state_transition(GGZ_TRANS_CONN_OK, &server->state);
		_ggzcore_server_event(server, GGZ_NEGOTIATED, NULL);
	}
	else {
		_ggzcore_state_transition(GGZ_TRANS_CONN_FAIL, &server->state);
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
		_ggzcore_state_transition(GGZ_TRANS_LOGIN_OK, &server->state);
		_ggzcore_server_event(server, GGZ_LOGGED_IN, NULL);
		break;
	case E_ALREADY_LOGGED_IN:
		_ggzcore_state_transition(GGZ_TRANS_LOGIN_FAIL, &server->state);
		_ggzcore_server_event(server, GGZ_LOGIN_FAIL, "Already logged in");
		break;
	case E_USR_LOOKUP:
		_ggzcore_state_transition(GGZ_TRANS_LOGIN_FAIL, &server->state);
		_ggzcore_server_event(server, GGZ_LOGIN_FAIL, "Name taken");
		break;
	}

#if 0
	/* FIXME: Add to its own function */
	/* Get list of game types */
	es_write_int(fd, REQ_LIST_TYPES);
	es_write_char(fd, 1);
#endif
}


static void _ggzcore_server_handle_logout(GGZServer *server)
{
	int status;
	char ok;

	status = _ggzcore_net_read_logout(server->fd, &ok);
	/* FIXME: handle errors */

	_ggzcore_net_disconnect(server->fd);
	server->fd = -1;

	_ggzcore_state_transition(GGZ_TRANS_LOGOUT_OK, &server->state);
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

	if (server->room_list)
		_ggzcore_roomlist_free(server->room_list);

	server->room_list = _ggzcore_roomlist_new();
	
	status = _ggzcore_net_read_num_rooms(server->fd, &num);
	/* FIXME: handle errors */

	ggzcore_debug(GGZ_DBG_SERVER, "Server sending %d rooms", num);

	for (i = 0; i < num; i++) {
		status = _ggzcore_net_read_room(server->fd, 
						server->room_verbose,
						&id, &name, &game, &desc);
		/* FIXME: handle errors */
		_ggzcore_roomlist_add(server->room_list, id, name, game, desc);

		/* Free allocated memory */
		if (desc)
			free(desc);
		if (name)
			free(name);
	}
	_ggzcore_server_event(server, GGZ_ROOM_LIST, NULL);
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
		_ggzcore_state_transition(GGZ_TRANS_ENTER_OK, &server->state);
		server->room = server->new_room;
		_ggzcore_server_event(server, GGZ_ENTERED, NULL);
		/* FIXME: create new GGZRoom object */
		break;
	case E_AT_TABLE:
		_ggzcore_state_transition(GGZ_TRANS_ENTER_FAIL, &server->state);
		server->new_room = server->room;
		_ggzcore_server_event(server, GGZ_ENTER_FAIL,
				      "Can't change rooms while at a table");
		break;
		
	case E_IN_TRANSIT:
		_ggzcore_state_transition(GGZ_TRANS_ENTER_FAIL, &server->state);
		server->new_room = server->room;
		_ggzcore_server_event(server, GGZ_ENTER_FAIL,
				      "Can't change rooms while joining/leaving a table");
		break;
		
	case E_BAD_OPTIONS:
		_ggzcore_state_transition(GGZ_TRANS_ENTER_FAIL, &server->state);
		server->new_room = server->room;
		_ggzcore_server_event(server, GGZ_ENTER_FAIL, 
				      "Bad room number");
				      
		break;
	}

#if 0
	/* Get list of tables */
        es_write_int(fd, REQ_LIST_TABLES);
        es_write_int(fd, -1);
        es_write_char(fd, 0);
#endif
}


static void _ggzcore_server_handle_list_players(GGZServer *server){}
static void _ggzcore_server_handle_rsp_chat(GGZServer *server){}
static void _ggzcore_server_handle_list_tables(GGZServer *server){}
static void _ggzcore_server_handle_list_types(GGZServer *server){}


/* completely bogus functions to avoid messiness */
static void _ggzcore_server_handle_chat(GGZServer *server)
{
	_ggzcore_net_read_chat(server->fd);
	ggzcore_event_process_all();
}


static void _ggzcore_server_handle_update_players(GGZServer *server)
{
	_ggzcore_net_read_update_players(server->fd);
	ggzcore_event_process_all();
}

static void _ggzcore_server_handle_update_tables(GGZServer *server)
{
	_ggzcore_net_read_update_tables(server->fd);
	ggzcore_event_process_all();
}
