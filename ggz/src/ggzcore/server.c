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
#include "net.h"
#include "errno.h"
#include "lists.h"
#include "player.h"
#include "protocol.h"

#include <stdlib.h>
#include <string.h>
#include <easysock.h>


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
	"GGZ_PROTOCOL_ERROR",
	"GGZ_CHAT_FAIL",
	"GGZ_STATE_CHANGE"
};

/* Total number of server events messages */
static unsigned int _ggzcore_num_events = sizeof(_ggzcore_server_events)/sizeof(char*);


/* The GGZServer structure manages information about a particular
   GGZ server connection */
struct _GGZServer {
	
	/* Network object for doing net IO */
	struct _GGZNet *net;

	/* Login type: one of GGZ_LOGIN, GGZ_LOGIN_GUEST, GGZ_LOGIN_NEW */
	GGZLoginType type;

	/* User handle on this server */
	char* handle;

	/* Password for this server (optional) */
	char* password;

	/* Current state */
	GGZStateID state;
	
	/* Number of gametypes */
	int num_gametypes;

	/* List of game types */
	struct _GGZGameType **gametypes;

	/* Number of rooms */
	int num_rooms;
	
	/* List of rooms in this server */
	struct _GGZRoom **rooms;
	
	/* Current room on game server */
	struct _GGZRoom *room;

       	/* Server events */
	GGZHookList *event_hooks[sizeof(_ggzcore_server_events)/sizeof(char*)];

};


/* Publicly exported functions */

GGZServer* ggzcore_server_new(void)
{
	return _ggzcore_server_new();
}


int ggzcore_server_reset(GGZServer *server)
{
	if (!server)
		return -1;

	_ggzcore_server_reset(server);
	
	return 0;
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
	if (!server || !_ggzcore_server_event_is_valid(event))
		return -1;

	return _ggzcore_hook_add_full(server->event_hooks[event], func, data);
}


int ggzcore_server_remove_event_hook(GGZServer *server,
				     const GGZServerEvent event, 
				     const GGZHookFunc func)
{
	if (!server || !_ggzcore_server_event_is_valid(event))
		return -1;

	return _ggzcore_hook_remove(server->event_hooks[event], func);
}


int ggzcore_server_remove_event_hook_id(GGZServer *server,
					    const GGZServerEvent event, 
					    const unsigned int hook_id)
{
	if (!server || !_ggzcore_server_event_is_valid(event))
		return -1;

	return _ggzcore_hook_remove_id(server->event_hooks[event], hook_id);
}


void ggzcore_server_free(GGZServer *server)
{
	if (!server)
		return;

	_ggzcore_server_free(server);
}


int ggzcore_server_set_hostinfo(GGZServer *server, const char *host, const unsigned int port)
{
	/* Check for valid arguments */
	if (server && host && server->state == GGZ_STATE_OFFLINE) {
		_ggzcore_net_init(server->net, server, host, port);
		return 0;
	}
	else
		return -1;
}


int ggzcore_server_set_logininfo(GGZServer *server, const GGZLoginType type, const char *handle, const char *password)
{
	int status;

	/* Check for valid arguments */
	if (!server || !handle || (type == GGZ_LOGIN && !password))
		return -1;

	/* Check for valid state */
	switch (server->state) {
	case GGZ_STATE_OFFLINE:
	case GGZ_STATE_CONNECTING:
	case GGZ_STATE_ONLINE:
		_ggzcore_server_set_logintype(server, type);
		_ggzcore_server_set_handle(server, handle);
		if (password)
			_ggzcore_server_set_password(server, password);
		status = 0;
		break;
	default:
		status = -1;
		break;
	}

	return status;
}


char* ggzcore_server_get_host(GGZServer *server)
{
	if (server && server->net)
		return _ggzcore_net_get_host(server->net);
	else
		return NULL;
}


int ggzcore_server_get_port(GGZServer *server)
{
	if (server && server->net)
		return _ggzcore_net_get_port(server->net);
	else
		return -1;
}


GGZLoginType ggzcore_server_get_type(GGZServer *server)
{
	if (server)
		return _ggzcore_server_get_type(server);
	else
		return -1;
}


char* ggzcore_server_get_handle(GGZServer *server)
{
	if (server)
		return _ggzcore_server_get_handle(server);
	else
		return NULL;
}


char* ggzcore_server_get_password(GGZServer *server)
{
	if (server)
		return _ggzcore_server_get_password(server);
	else
		return NULL;
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
	if (server && server->net)
		return _ggzcore_net_get_fd(server->net);
	else
		return -1;
}


GGZStateID ggzcore_server_get_state(GGZServer *server)
{
	if (server)
		return _ggzcore_server_get_state(server);
	else
		return -1;
}


GGZRoom* ggzcore_server_get_cur_room(GGZServer *server)
{
	if (server)
		return _ggzcore_server_get_cur_room(server);
	else
		return NULL;
}


GGZRoom* ggzcore_server_get_nth_room(GGZServer *server, const unsigned int num)
{
	if (server && num < _ggzcore_server_get_num_rooms(server))
		return _ggzcore_server_get_nth_room(server, num);
	else
		return NULL;
}


int ggzcore_server_get_num_rooms(GGZServer *server)
{
	if (server)
		return _ggzcore_server_get_num_rooms(server);
	else
		return -1;
}


int ggzcore_server_get_num_gametypes(GGZServer *server)
{
	if (server)
		return _ggzcore_server_get_num_gametypes(server);
	else
		return -1;
}


GGZGameType* ggzcore_server_get_nth_gametype(GGZServer *server, 
					     const unsigned int num)
{
	if (server && num < _ggzcore_server_get_num_gametypes(server))
		return _ggzcore_server_get_nth_gametype(server, num);
	else
		return NULL;
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


int ggzcore_server_connect(GGZServer *server)
{
	if (server && server->net && server->state == GGZ_STATE_OFFLINE) {
		return _ggzcore_server_connect(server);
	}
	else
		return -1;
}


int ggzcore_server_login(GGZServer *server)
{
	/* Return nothing if we didn't get the necessary info */
	if (!server || !server->handle || server->state != GGZ_STATE_ONLINE)
		return -1;

	if (server->type == GGZ_LOGIN && !server->password)
		return -1;

	return _ggzcore_server_login(server);
}


int ggzcore_server_motd(GGZServer *server)
{
	if (server && server->state >= GGZ_STATE_LOGGED_IN)
		return _ggzcore_server_load_motd(server);
	else
		return -1;
}


int ggzcore_server_list_rooms(GGZServer *server, 
			       const int type, 
			       const char verbose)
{
	if (server && server->state >= GGZ_STATE_LOGGED_IN)
		return _ggzcore_server_load_roomlist(server, type, verbose);
	else
		return -1;
}


int ggzcore_server_list_gametypes(GGZServer *server, const char verbose)
{
	if (server && server->state >= GGZ_STATE_LOGGED_IN)
		return _ggzcore_server_load_typelist(server, verbose);
	else 
		return -1;
}


int ggzcore_server_join_room(GGZServer *server, const unsigned int room_num)
{
	/* FIXME: check validity of this action */
	if (server && (room_num < server->num_rooms) 
	    && (server->state == GGZ_STATE_IN_ROOM || server->state == GGZ_STATE_LOGGED_IN))
		return _ggzcore_server_join_room(server, room_num);
	else
		return -1;
}


int ggzcore_server_logout(GGZServer *server)
{
	if (server 
	    && server->state != GGZ_STATE_OFFLINE 
	    && server->state != GGZ_STATE_LOGGING_OUT)
		return _ggzcore_server_logout(server);
	else
		return -1;
}


int ggzcore_server_data_is_pending(GGZServer *server)
{
	int pending = 0;
	
	if (server && server->net && server->state != GGZ_STATE_OFFLINE)
		pending = _ggzcore_net_data_is_pending(server->net);
	
	return pending;
}


int ggzcore_server_read_data(GGZServer *server)
{
	int status = -1;

	if (server && server->net && server->state != GGZ_STATE_OFFLINE)
		status = _ggzcore_net_read_data(server->net);
	
	return status;
}


int ggzcore_server_write_data(GGZServer *server)
{
	return 0;
}


/* Internal library functions (prototypes in server.h) */

struct _GGZServer* _ggzcore_server_new(void)
{
	struct _GGZServer *server;

	server = ggzcore_malloc(sizeof(struct _GGZServer));
	_ggzcore_server_reset(server);
	
	return server;
}


struct _GGZNet* _ggzcore_server_get_net(struct _GGZServer *server)
{
	return server->net;
}


GGZLoginType _ggzcore_server_get_type(struct _GGZServer *server)
{
	return server->type;
}


char* _ggzcore_server_get_handle(struct _GGZServer *server)
{
	return server->handle;
}


char* _ggzcore_server_get_password(struct _GGZServer *server)
{
	return server->password;
}


GGZStateID _ggzcore_server_get_state(struct _GGZServer *server)
{
	return server->state;
}


int _ggzcore_server_get_num_rooms(struct _GGZServer *server)
{
	return server->num_rooms;
}


struct _GGZRoom* _ggzcore_server_get_cur_room(struct _GGZServer *server)
{
	return server->room;
}


struct _GGZRoom* _ggzcore_server_get_room_by_id(struct _GGZServer *server,
						const unsigned int id)
{
	int i;

	for (i = 0; i < server->num_rooms; i++)
		if (_ggzcore_room_get_id(server->rooms[i]) == id)
			return server->rooms[i];

	return NULL;
}


struct _GGZRoom* _ggzcore_server_get_nth_room(struct _GGZServer *server, const unsigned int num)
{
	return server->rooms[num];
}


int _ggzcore_server_get_num_gametypes(struct _GGZServer *server)
{
	return server->num_gametypes;
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


struct _GGZGameType* _ggzcore_server_get_nth_gametype(struct _GGZServer *server, 
						      const unsigned int num)
{
	return server->gametypes[num];
}


void _ggzcore_server_set_logintype(struct _GGZServer *server, const GGZLoginType type)
{
	server->type = type;
}


void _ggzcore_server_set_handle(struct _GGZServer *server, const char *handle)
{
	/* Free old handle if one existed */
	if (server->handle)
		ggzcore_free(server->handle);
	
	server->handle = ggzcore_strdup(handle);
}


void _ggzcore_server_set_password(struct _GGZServer *server, const char *password)
{
	/* Free old password if one existed */
	if (server->password)
		ggzcore_free(server->password);
	
	server->password = ggzcore_strdup(password);
}


void _ggzcore_server_set_room(struct _GGZServer *server, struct _GGZRoom *room)
{
	struct _GGZRoom *old;
	
	/* Stop monitoring old room/start monitoring new one */
	if ( (old = _ggzcore_server_get_cur_room(server)))
		_ggzcore_room_set_monitor(old, 0);

	server->room = room;
	
	_ggzcore_room_set_monitor(room, 1);
}


void _ggzcore_server_set_negotiate_status(struct _GGZServer *server, int status)
{
	if (status == 0) {
		_ggzcore_server_change_state(server, GGZ_TRANS_CONN_OK);
		_ggzcore_server_event(server, GGZ_NEGOTIATED, NULL);
	}
	else {
		_ggzcore_server_change_state(server, GGZ_TRANS_CONN_FAIL);
		_ggzcore_server_event(server, GGZ_NEGOTIATE_FAIL, "Protocol mismatch");
	}
}


void _ggzcore_server_set_login_status(struct _GGZServer *server, int status)
{
	ggzcore_debug(GGZ_DBG_SERVER, "Status of login: %d", status);

	switch (status) {
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

	default:
		_ggzcore_server_change_state(server, GGZ_TRANS_LOGIN_FAIL);
		_ggzcore_server_event(server, GGZ_LOGIN_FAIL, "Unknown login error");
				      
		break;
	}
}


void _ggzcore_server_set_room_join_status(struct _GGZServer *server, int status)
{
	switch (status) {
	case 0:
		_ggzcore_server_change_state(server, GGZ_TRANS_ENTER_OK);
		_ggzcore_server_event(server, GGZ_ENTERED, NULL);
		break;

	case E_ROOM_FULL:
		_ggzcore_server_change_state(server, GGZ_TRANS_ENTER_FAIL);
		_ggzcore_server_event(server, GGZ_ENTER_FAIL,
				      "Room full");
		break;

	case E_AT_TABLE:
		_ggzcore_server_change_state(server, GGZ_TRANS_ENTER_FAIL);
		_ggzcore_server_event(server, GGZ_ENTER_FAIL,
				      "Can't change rooms while at a table");
		break;
		
	case E_IN_TRANSIT:
		_ggzcore_server_change_state(server, GGZ_TRANS_ENTER_FAIL);
		_ggzcore_server_event(server, GGZ_ENTER_FAIL,
				      "Can't change rooms while joining/leaving a table");
		break;
		
	case E_BAD_OPTIONS:
		_ggzcore_server_change_state(server, GGZ_TRANS_ENTER_FAIL);
		_ggzcore_server_event(server, GGZ_ENTER_FAIL, 
				      "Bad room number");
		break;

	default:
		_ggzcore_server_change_state(server, GGZ_TRANS_ENTER_FAIL);
		_ggzcore_server_event(server, GGZ_ENTER_FAIL, 
				      "Unknown room-joining error");
		break;
	}
}


void _ggzcore_server_set_table_launching(struct _GGZServer *server)
{
	_ggzcore_server_change_state(server, GGZ_TRANS_LAUNCH_TRY);
}


void _ggzcore_server_set_table_joining(struct _GGZServer *server)
{
	_ggzcore_server_change_state(server, GGZ_TRANS_JOIN_TRY);
}


void _ggzcore_server_set_table_leaving(struct _GGZServer *server)
{
	_ggzcore_server_change_state(server, GGZ_TRANS_LEAVE_TRY);
}


void _ggzcore_server_set_table_launch_status(struct _GGZServer *server, int status)
{
	if (status == 0)
		_ggzcore_server_change_state(server, GGZ_TRANS_LAUNCH_OK);
	else
		_ggzcore_server_change_state(server, GGZ_TRANS_LAUNCH_FAIL);
}

					     
void _ggzcore_server_set_table_join_status(struct _GGZServer *server, int status)
{
	if (status == 0)
		_ggzcore_server_change_state(server, GGZ_TRANS_JOIN_OK);
	else
		_ggzcore_server_change_state(server, GGZ_TRANS_JOIN_FAIL);

}					   


void _ggzcore_server_set_table_leave_status(struct _GGZServer *server, int status)
{
	if (status == 0)
		_ggzcore_server_change_state(server, GGZ_TRANS_LEAVE_OK);
	else
		_ggzcore_server_change_state(server, GGZ_TRANS_LEAVE_FAIL);

}


void _ggzcore_server_set_logout_status(struct _GGZServer *server, int status)
{
	_ggzcore_server_change_state(server, GGZ_TRANS_LOGOUT_OK);
	_ggzcore_server_event(server, GGZ_LOGOUT, NULL);
}

					    
void _ggzcore_server_reset(struct _GGZServer *server)
{
	int i;

	_ggzcore_server_clear(server);

	/* Set initial state */
	server->state = GGZ_STATE_OFFLINE;

	/* Allocate network object */
	server->net = _ggzcore_net_new();

	/* Setup event hook lists */
	for (i = 0; i < _ggzcore_num_events; i++)
		server->event_hooks[i] = _ggzcore_hook_list_init(i);
}


int _ggzcore_server_connect(struct _GGZServer *server)
{
	int status;
	
	_ggzcore_server_change_state(server, GGZ_TRANS_CONN_TRY);
	status = _ggzcore_net_connect(server->net);
	
	if (status < 0) {
		_ggzcore_server_change_state(server, GGZ_TRANS_CONN_FAIL);
		_ggzcore_server_event(server, GGZ_CONNECT_FAIL, 
				      strerror(errno));
	}
	else
		_ggzcore_server_event(server, GGZ_CONNECTED, NULL);
	
	return status;
}

			    
int _ggzcore_server_login(struct _GGZServer *server)
{
	int status;

	ggzcore_debug(GGZ_DBG_SERVER, "Login (%d), %s, %s", server->type, 
		      server->handle, server->password);

	status = _ggzcore_net_send_login(server->net);

	if (status == 0)
		_ggzcore_server_change_state(server, GGZ_TRANS_LOGIN_TRY);

	return status;
}


int _ggzcore_server_load_motd(struct _GGZServer *server)
{
	 return _ggzcore_net_send_motd(server->net);
}


int _ggzcore_server_load_typelist(struct _GGZServer *server, const char verbose)
{
	return _ggzcore_net_send_list_types(server->net, verbose);
}


int _ggzcore_server_load_roomlist(struct _GGZServer *server, const int type, const char verbose)
{
	return _ggzcore_net_send_list_rooms(server->net, type, verbose);
}


int _ggzcore_server_join_room(struct _GGZServer *server, const unsigned int room_num)
{
	int status;
	int room_id;
	struct _GGZRoom *room;

	/* We need to send the room's ID on the server */
	room = _ggzcore_server_get_nth_room(server, room_num);
	room_id = _ggzcore_room_get_id(room);
	
	ggzcore_debug(GGZ_DBG_SERVER, "Moving to room %d", room_num);
	
	status = _ggzcore_net_send_join_room(server->net, room_id);

	if (status == 0)
		_ggzcore_server_change_state(server, GGZ_TRANS_ENTER_TRY);
	
	return status;
}


int _ggzcore_server_logout(struct _GGZServer *server)
{
	int status;

	ggzcore_debug(GGZ_DBG_SERVER, "Logging out");
	status = _ggzcore_net_send_logout(server->net);
	if (status == 0)
		_ggzcore_server_change_state(server, GGZ_TRANS_LOGOUT_TRY);

	return status;
}


void _ggzcore_server_clear(struct _GGZServer *server)
{
	int i;

	/* Clear all members */
	if (server->net) {
		_ggzcore_net_free(server->net);
		server->net = NULL;
	}

	if (server->handle) {
		ggzcore_free(server->handle);
		server->handle = NULL;
	}

	if (server->password) {
		ggzcore_free(server->password);
		server->password = NULL;
	}

	if (server->rooms) {
		_ggzcore_server_free_roomlist(server);
		server->rooms = NULL;
		server->num_rooms = 0;
	}
	server->room = NULL;

	if (server->gametypes) {
		_ggzcore_server_free_typelist(server);
		server->gametypes = NULL;
		server->num_gametypes = 0;
	}

	for (i = 0; i < _ggzcore_num_events; i++) {
		if (server->event_hooks[i]) {
			_ggzcore_hook_list_destroy(server->event_hooks[i]);
			server->event_hooks[i] = NULL;
		}
	}
}


void _ggzcore_server_free(struct _GGZServer *server)
{
	_ggzcore_server_clear(server);
	ggzcore_free(server);
}


/* Static functions internal to this file */

void _ggzcore_server_init_roomlist(struct _GGZServer *server,
					  const int num)
{
	int i;

	server->num_rooms = num;
	server->rooms = ggzcore_malloc(num * sizeof(struct _GGZRoom*));
	for(i = 0; i < num; i++)
		server->rooms[i] = NULL;
}


void _ggzcore_server_free_roomlist(struct _GGZServer *server)
{
	int i;

	for (i = 0; i < server->num_rooms; i++) {
		if(server->rooms[i])
			_ggzcore_room_free(server->rooms[i]);
	}

	ggzcore_free(server->rooms);
	server->num_rooms = 0;
}


void _ggzcore_server_add_room(struct _GGZServer *server, 
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


void _ggzcore_server_init_typelist(struct _GGZServer *server, 
					  const int num)
{
	server->num_gametypes = num;
	server->gametypes = ggzcore_malloc(num * sizeof(struct _GGZGameType*));
}


void _ggzcore_server_free_typelist(struct _GGZServer *server)
{
	int i;

	for (i = 0; i < server->num_gametypes; i++) {
		_ggzcore_gametype_free(server->gametypes[i]);
	}

	ggzcore_free(server->gametypes);
	server->num_gametypes = 0;
}


void _ggzcore_server_add_type(struct _GGZServer *server, 
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

void _ggzcore_server_change_state(GGZServer *server, GGZTransID trans)
{
	_ggzcore_state_transition(trans, &server->state);
	_ggzcore_server_event(server, GGZ_STATE_CHANGE, NULL);
}


int _ggzcore_server_event_is_valid(GGZServerEvent event)
{
	return (event >= 0 && event < _ggzcore_num_events);
}
	

GGZHookReturn _ggzcore_server_event(GGZServer *server, 
					   GGZServerEvent id, 
					   void *data)
{
	return _ggzcore_hook_list_invoke(server->event_hooks[id], data);
}


void _ggzcore_server_net_error(struct _GGZServer *server, char* message)
{
	_ggzcore_server_change_state(server, GGZ_TRANS_NET_ERROR);
	_ggzcore_server_event(server, GGZ_NET_ERROR, message);
}


void _ggzcore_server_protocol_error(GGZServer *server, char* message)
{
	ggzcore_debug(GGZ_DBG_SERVER, "Protocol error: disconnecting");
	_ggzcore_net_disconnect(server->net);
	_ggzcore_server_change_state(server, GGZ_TRANS_PROTO_ERROR);
	_ggzcore_server_event(server, GGZ_PROTOCOL_ERROR, message);
}


