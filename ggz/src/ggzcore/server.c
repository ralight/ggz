/*
 * File: server.c
 * Author: Brent Hendricks
 * Project: GGZ Core Client Lib
 * Date: 1/19/01
 * $Id: server.c 6866 2005-01-24 01:39:48Z jdorje $
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

#ifdef HAVE_CONFIG_H
#  include <config.h>		/* Site-specific config */
#endif

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif

#include <ggz.h>

#include "ggzcore.h"
#include "ggzcore-ggz.h"

#include "hook.h"
#include "net.h"
#include "player.h"
#include "protocol.h"
#include "state.h"
#include "server.h"

#if 0
/* Array of GGZServerEvent messages.  This is now unused, but could be used
 * in the future. */
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
	"GGZ_SERVER_PLAYERS_CHANGED",
	"GGZ_ENTERED",
	"GGZ_ENTER_FAIL",
	"GGZ_LOGOUT",
	"GGZ_NET_ERROR",
	"GGZ_PROTOCOL_ERROR",
	"GGZ_CHAT_FAIL",
	"GGZ_STATE_CHANGE",
	"GGZ_CHANNEL_CONNECTED",
	"GGZ_CHANNEL_READY",
	"GGZ_CHANNEL_FAIL"
};
#endif

/* The GGZServer structure manages information about a particular
   GGZ server connection */
struct _GGZServer {
	
	/* Network object for doing net IO */
	GGZNet *net;

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
	GGZGameType **gametypes;

	/* Number of rooms */
	int num_rooms;
	
	/* List of rooms in this server */
	GGZRoom **rooms;
	
	/* Current room on game server */
	GGZRoom *room;

	/* New (targeted) room - the room we're trying to move into. */
	GGZRoom *new_room;

	/* Current game on game server */
	GGZGame *game;

	/* Whether this is a primary ggzcore server connection or a
	 * channel. */
	int is_channel;
	int channel_complete;
	int channel_failed;

       	/* Callbacks for server events.  Every time an event happens all the
	 * hooks in the list for that event are called. */
	GGZHookList *event_hooks[GGZ_NUM_SERVER_EVENTS];

	/* This struct is used when queueing events.  Events that may happen
	 * many times during a single network read may instead be queued and
	 * later popped off the queue when we leave the network code. */
	struct {
		int players_changed;
	} queued_events;
};

static void _ggzcore_server_main_negotiate_status(GGZServer *server,
						  GGZClientReqError status);
static void _ggzcore_server_channel_negotiate_status(GGZServer *server,
						     GGZClientReqError status);

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


int ggzcore_server_set_hostinfo(GGZServer *server, const char *host,
				const unsigned int port, const unsigned int use_tls)
{
	/* Check for valid arguments */
	if (server && host && server->state == GGZ_STATE_OFFLINE) {
		_ggzcore_net_init(server->net, server, host, port, use_tls);
		return 0;
	}
	else
		return -1;
}


int ggzcore_server_set_logininfo(GGZServer *server, const GGZLoginType type,
				 const char *handle, const char *password)
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


int ggzcore_server_log_session(GGZServer *server, const char *filename)
{
	if (!server)
		return -1;

	return _ggzcore_server_log_session(server, filename);
}


const char* ggzcore_server_get_host(GGZServer *server)
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


int ggzcore_server_get_num_players(GGZServer *server)
{
	if (server)
		return _ggzcore_server_get_num_players(server);
	else
		return 0;
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


GGZGame* ggzcore_server_get_cur_game(GGZServer *server)
{
	if (server)
		return _ggzcore_server_get_cur_game(server);
	else
		return NULL;
}


int ggzcore_server_get_tls(GGZServer *server)
{
	if (server)
		return _ggzcore_server_get_tls(server);
	else
		return -1;
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


int ggzcore_server_disconnect(GGZServer *server)
{
	if (server && server->state != GGZ_STATE_OFFLINE)
		return _ggzcore_server_disconnect(server);
	else
		return -1;
}


int ggzcore_server_data_is_pending(GGZServer *server)
{
	int pending = 0;

	if (server && server->net && server->state != GGZ_STATE_OFFLINE) {
		pending = _ggzcore_net_data_is_pending(server->net);
	}

	return pending;
}


int ggzcore_server_read_data(GGZServer *server, int fd)
{
	int status = -1;

	if (!server || !server->net
	    || (fd = _ggzcore_net_get_fd(server->net)) < 0)
		return -1;

	if (server->state != GGZ_STATE_OFFLINE) {
		status = _ggzcore_net_read_data(server->net);

		/* See comment in
		 * ggzcore_server_queue_players_changed. */
		if (server->queued_events.players_changed) {
			_ggzcore_server_event(server,
				GGZ_SERVER_PLAYERS_CHANGED, NULL);
			server->queued_events.players_changed = 0;
		}
	} else {
		/* If we *don't* return an error here, the caller is likely
		 * to just keep calling this function again and again
		 * because no data will ever be read. */
		return -1;
	}

	return 0;
}


/* Internal library functions (prototypes in server.h) */

GGZServer* _ggzcore_server_new(void)
{
	GGZServer *server;

	server = ggz_malloc(sizeof(GGZServer));
	_ggzcore_server_reset(server);
	
	return server;
}


GGZNet* _ggzcore_server_get_net(GGZServer *server)
{
	return server->net;
}


GGZLoginType _ggzcore_server_get_type(GGZServer *server)
{
	return server->type;
}


char* _ggzcore_server_get_handle(GGZServer *server)
{
	return server->handle;
}


char* _ggzcore_server_get_password(GGZServer *server)
{
	return server->password;
}


GGZStateID _ggzcore_server_get_state(GGZServer *server)
{
	return server->state;
}


int _ggzcore_server_get_tls(struct _GGZServer *server)
{
	int tls;

	tls = 0;
	if (server && server->net)
		tls = _ggzcore_net_get_tls(server->net);
	return tls;
}


int _ggzcore_server_get_num_players(GGZServer *server)
{
	int rooms = ggzcore_server_get_num_rooms(server), i, total = 0;

	for (i = 0; i < rooms; i++) {
		GGZRoom *room = ggzcore_server_get_nth_room(server, i);

		/* Each room should track an approximate number of players
		 * inside it (or 0 which is just as good).  We just total
		 * those up. */
		total += ggzcore_room_get_num_players(room);
	}

	return total;
}


int _ggzcore_server_get_num_rooms(GGZServer *server)
{
	return server->num_rooms;
}


GGZRoom* _ggzcore_server_get_cur_room(GGZServer *server)
{
	return server->room;
}


GGZRoom* _ggzcore_server_get_room_by_id(GGZServer *server,
					const unsigned int id)
{
	int i;

	for (i = 0; i < server->num_rooms; i++)
		if (ggzcore_room_get_id(server->rooms[i]) == id)
			return server->rooms[i];

	return NULL;
}


GGZRoom* _ggzcore_server_get_nth_room(GGZServer *server,
				      const unsigned int num)
{
	return server->rooms[num];
}


int _ggzcore_server_get_num_gametypes(GGZServer *server)
{
	return server->num_gametypes;
}


GGZGameType* _ggzcore_server_get_type_by_id(GGZServer *server,
					    const unsigned int id)
{
	int i;

	for (i = 0; i < server->num_gametypes; i++)
		if (ggzcore_gametype_get_id(server->gametypes[i]) == id)
			return server->gametypes[i];

	return NULL;
}


GGZGameType* _ggzcore_server_get_nth_gametype(GGZServer *server, 
					      const unsigned int num)
{
	return server->gametypes[num];
}

GGZGame* _ggzcore_server_get_cur_game(GGZServer *server)
{
	return server->game;
}

void _ggzcore_server_set_cur_game(GGZServer *server,
				  GGZGame *game)
{
#define XOR(a, b) (((a) || (b)) && !((a) && (b)))
	assert(XOR(server->game == NULL, game == NULL));
	server->game = game;
}


void _ggzcore_server_set_logintype(GGZServer *server, const GGZLoginType type)
{
	server->type = type;
}


void _ggzcore_server_set_handle(GGZServer *server, const char *handle)
{
	/* Free old handle if one existed */
	if (server->handle)
		ggz_free(server->handle);
	
	server->handle = ggz_strdup(handle);
}


void _ggzcore_server_set_password(GGZServer *server, const char *password)
{
	/* Free old password if one existed */
	if (server->password)
		ggz_free(server->password);
	
	server->password = ggz_strdup(password);
}


void _ggzcore_server_set_room(GGZServer *server, GGZRoom *room)
{
	GGZRoom *old = _ggzcore_server_get_cur_room(server);
	int num_players = 0;
	
	/* Stop monitoring old room/start monitoring new one */
	if (old) {
		num_players = old->num_players;
		_ggzcore_room_set_monitor(old, 0);
	}

	server->room = room;
	
	_ggzcore_room_set_monitor(room, 1);
	/* No need to update the player_count in the new room. */

	if (old) {
		/* This is done last so this room will no longer be the
		 * current one. */
		_ggzcore_room_set_players(old, num_players - 1);
	}
}


void _ggzcore_server_set_negotiate_status(GGZServer *server, GGZNet *net,
					  GGZClientReqError status)
{
	if (net != server->net) {
		_ggzcore_server_net_error(server, "Unknown negotation");
	} else if (server->is_channel == 0) {
		_ggzcore_server_main_negotiate_status(server, status);
	} else {
		_ggzcore_server_channel_negotiate_status(server, status);
	}
}


void _ggzcore_server_set_login_status(GGZServer *server,
				      GGZClientReqError status)
{
	ggz_debug(GGZCORE_DBG_SERVER, "Status of login: %d", status);

	if (status == E_OK) {
		_ggzcore_server_change_state(server, GGZ_TRANS_LOGIN_OK);
		_ggzcore_server_event(server, GGZ_LOGGED_IN, NULL);
	} else {
		GGZErrorEventData error = {status: status};

		switch(status) {
		case E_ALREADY_LOGGED_IN:
			snprintf(error.message, sizeof(error.message),
				 "Already logged in");
			break;
		case E_USR_LOOKUP:
			snprintf(error.message, sizeof(error.message),
				 "Name taken");
			break;
		case E_TOO_LONG:
			snprintf(error.message, sizeof(error.message),
				 "Name too long");
			break;
		default:
			snprintf(error.message, sizeof(error.message),
				 "Unknown login error");
			break;
		}
		_ggzcore_server_change_state(server, GGZ_TRANS_LOGIN_FAIL);
		_ggzcore_server_event(server, GGZ_LOGIN_FAIL, &error);
	}

}


void _ggzcore_server_set_room_join_status(GGZServer *server,
					  GGZClientReqError status)
{
	if (status == E_OK) {
		_ggzcore_server_set_room(server, server->new_room);
		_ggzcore_server_change_state(server, GGZ_TRANS_ENTER_OK);
		_ggzcore_server_event(server, GGZ_ENTERED, NULL);
	} else {
		GGZErrorEventData error = {status: status};

		switch (status) {
		case E_ROOM_FULL:
			snprintf(error.message, sizeof(error.message),
				 "Room full");
			break;
		case E_AT_TABLE:
			snprintf(error.message, sizeof(error.message),
				 "Can't change rooms while at a table");
			break;
		case E_IN_TRANSIT:
			snprintf(error.message, sizeof(error.message),
				 "Can't change rooms while "
				 "joining/leaving a table");
			break;
		case E_BAD_OPTIONS:
			snprintf(error.message, sizeof(error.message),
				 "Bad room number");
			break;
		default:
			snprintf(error.message, sizeof(error.message),
				 "Unknown room-joining error");
			break;
		}

		_ggzcore_server_change_state(server, GGZ_TRANS_ENTER_FAIL);
		_ggzcore_server_event(server, GGZ_ENTER_FAIL, &error);
	}
}


void _ggzcore_server_set_table_launching(GGZServer *server)
{
	_ggzcore_server_change_state(server, GGZ_TRANS_LAUNCH_TRY);
}


void _ggzcore_server_set_table_joining(GGZServer *server)
{
	_ggzcore_server_change_state(server, GGZ_TRANS_JOIN_TRY);
}


void _ggzcore_server_set_table_leaving(GGZServer *server)
{
	_ggzcore_server_change_state(server, GGZ_TRANS_LEAVE_TRY);
}


void _ggzcore_server_set_table_launch_status(GGZServer *server,
					     GGZClientReqError status)
{
	if (status == E_OK)
		_ggzcore_server_change_state(server, GGZ_TRANS_LAUNCH_OK);
	else
		_ggzcore_server_change_state(server, GGZ_TRANS_LAUNCH_FAIL);
}

					     
void _ggzcore_server_set_table_join_status(GGZServer *server,
					   GGZClientReqError status)
{
	if (status == E_OK)
		_ggzcore_server_change_state(server, GGZ_TRANS_JOIN_OK);
	else
		_ggzcore_server_change_state(server, GGZ_TRANS_JOIN_FAIL);

}					   


void _ggzcore_server_set_table_leave_status(GGZServer *server,
					    GGZClientReqError status)
{
	if (status == E_OK)
		_ggzcore_server_change_state(server, GGZ_TRANS_LEAVE_OK);
	else
		_ggzcore_server_change_state(server, GGZ_TRANS_LEAVE_FAIL);

}


void _ggzcore_server_session_over(GGZServer *server, GGZNet *net)
{
	if (net != server->net) return;
	if (server->is_channel == 0) {
		/* Server is ending session */
		_ggzcore_net_disconnect(net);
		_ggzcore_server_change_state(server, GGZ_TRANS_LOGOUT_OK);
		_ggzcore_server_event(server, GGZ_LOGOUT, NULL);
	} else {
		/* Channel is ready! */
		_ggzcore_server_change_state(server, GGZ_TRANS_LOGOUT_OK);
		server->channel_complete = 1;
	}
}


int _ggzcore_server_log_session(GGZServer *server, 
				const char *filename)
{
	return _ggzcore_net_set_dump_file(server->net, filename);
}
	
				    
void _ggzcore_server_reset(GGZServer *server)
{
	int i;

	_ggzcore_server_clear(server);

	/* Set initial state */
	server->state = GGZ_STATE_OFFLINE;

	/* Allocate network object */
	server->net = _ggzcore_net_new();

	server->is_channel = 0;

	/* Setup event hook lists */
	for (i = 0; i < GGZ_NUM_SERVER_EVENTS; i++)
		server->event_hooks[i] = _ggzcore_hook_list_init(i);
}


int _ggzcore_server_connect(GGZServer *server)
{
	int status;
	char *errmsg;
	
	_ggzcore_server_change_state(server, GGZ_TRANS_CONN_TRY);
	status = _ggzcore_net_connect(server->net);
	
	if (status < 0) {
		_ggzcore_server_change_state(server, GGZ_TRANS_CONN_FAIL);
		if(status == -1) errmsg = strerror(errno);
		else {
#ifdef HAVE_HSTRERROR
			errmsg = (char*)hstrerror(h_errno);
#else
			/* Not all systems have hstrerror. */
			errmsg = "Unable to connect";
#endif
		}
		_ggzcore_server_event(server, GGZ_CONNECT_FAIL, errmsg);
	}
	else
		_ggzcore_server_event(server, GGZ_CONNECTED, NULL);
	
	return status;
}


int ggzcore_channel_connect(const char *host, unsigned int port,
			    const char *handle)
{
	GGZServer *server;
	int fd;

	/* Hack, hack, hack.  The below really needs to be fixed up with some
	 * error handling.  There's also a major problem that it blocks the
	 * whole time while the connection is being made. */
	server = ggzcore_server_new();
	ggzcore_server_log_session(server, "channel.xml");
	server->is_channel = 1;
	server->channel_complete = server->channel_failed = 0;
	if (ggzcore_server_set_hostinfo(server, host, port, 0) < 0
	    || ggzcore_server_set_logininfo(server, GGZ_LOGIN_GUEST,
					    handle, NULL) < 0) {
		ggzcore_server_free(server);
		return -1;
	}

	if (_ggzcore_server_connect(server) < 0) {
		ggzcore_server_free(server);
		return -1;
	}
	fd = _ggzcore_net_get_fd(server->net);

	while (1) {
		fd_set active_fd_set;
		int status;
		struct timeval timeout = {.tv_sec = 30, .tv_usec = 0};

		FD_ZERO(&active_fd_set);
		FD_SET(fd, &active_fd_set);

		status = select(fd + 1, &active_fd_set, NULL, NULL, &timeout);
		if (status < 0) {
			if (errno == EINTR) continue;
			ggzcore_server_free(server);
			return -1;
		} else if (status == 0) {
			/* Timed out. */
			return -1;
		} else if (status > 0 && FD_ISSET(fd, &active_fd_set)) {
			if (ggzcore_server_read_data(server, fd) < 0) {
				return -1;
			}
		}

		if (server->channel_complete != 0) {
			/* Set the socket to -1 so we don't
			 * accidentally close it. */
			_ggzcore_net_set_fd(server->net, -1);
			ggzcore_server_free(server);
			return fd;
		} else if (server->channel_failed != 0) {
			ggzcore_server_free(server);
			return -1;
		}
	}
}
			    
int _ggzcore_server_login(GGZServer *server)
{
	int status;

	ggz_debug(GGZCORE_DBG_SERVER, "Login (%d), %s, %s", server->type, 
		      server->handle, server->password);

	status = _ggzcore_net_send_login(server->net, server->type,
					 server->handle, server->password,
					 getenv("LANG"));

	if (status == 0)
		_ggzcore_server_change_state(server, GGZ_TRANS_LOGIN_TRY);

	return status;
}


int _ggzcore_server_load_motd(GGZServer *server)
{
	 return _ggzcore_net_send_motd(server->net);
}


int _ggzcore_server_load_typelist(GGZServer *server, const char verbose)
{
	return _ggzcore_net_send_list_types(server->net, verbose);
}


int _ggzcore_server_load_roomlist(GGZServer *server, const int type,
				  const char verbose)
{
	return _ggzcore_net_send_list_rooms(server->net, type, verbose);
}


int _ggzcore_server_join_room(GGZServer *server, const unsigned int room_num)
{
	int status;
	int room_id;
	GGZRoom *room;

	/* We need to send the room's ID on the server */
	room = _ggzcore_server_get_nth_room(server, room_num);
	room_id = ggzcore_room_get_id(room);

	ggz_debug(GGZCORE_DBG_SERVER, "Moving to room %d", room_num);
	
	status = _ggzcore_net_send_join_room(server->net, room_id);
	server->new_room = room;

	if (status == 0)
		_ggzcore_server_change_state(server, GGZ_TRANS_ENTER_TRY);
	
	return status;
}


int _ggzcore_server_logout(GGZServer *server)
{
	int status;

	ggz_debug(GGZCORE_DBG_SERVER, "Logging out");
	status = _ggzcore_net_send_logout(server->net);
	if (status == 0)
		_ggzcore_server_change_state(server, GGZ_TRANS_LOGOUT_TRY);

	return status;
}


int _ggzcore_server_disconnect(GGZServer *server)
{
	ggz_debug(GGZCORE_DBG_SERVER, "Disconnecting");
	_ggzcore_net_disconnect(server->net);

	/* Force the server object into the offline state */
	server->state = GGZ_STATE_OFFLINE;
	_ggzcore_server_event(server, GGZ_STATE_CHANGE, NULL);

	return 0;
}


void _ggzcore_server_clear(GGZServer *server)
{
	int i;

	/* Clear all members */
	if (server->net) {
		_ggzcore_net_free(server->net);
		server->net = NULL;
	}

	if (server->handle) {
		ggz_free(server->handle);
		server->handle = NULL;
	}

	if (server->password) {
		ggz_free(server->password);
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

	for (i = 0; i < GGZ_NUM_SERVER_EVENTS; i++) {
		if (server->event_hooks[i]) {
			_ggzcore_hook_list_destroy(server->event_hooks[i]);
			server->event_hooks[i] = NULL;
		}
	}
}


void _ggzcore_server_free(GGZServer *server)
{
	if (server->game)
		ggzcore_game_free(server->game);
	_ggzcore_server_clear(server);
	ggz_free(server);
}


/* Static functions internal to this file */

static void _ggzcore_server_main_negotiate_status(GGZServer *server,
						  GGZClientReqError status)
{
	if (status == E_OK) {
		_ggzcore_server_change_state(server, GGZ_TRANS_CONN_OK);
		_ggzcore_server_event(server, GGZ_NEGOTIATED, NULL);
	}
	else {
		_ggzcore_server_change_state(server, GGZ_TRANS_CONN_FAIL);
		_ggzcore_server_event(server, GGZ_NEGOTIATE_FAIL, "Protocol mismatch");
	}
}

static void _ggzcore_server_channel_negotiate_status(GGZServer *server,
						     GGZClientReqError status)
{
	int fd;
	
	if (status == E_OK) {
		fd = _ggzcore_net_get_fd(server->net);
		_ggzcore_net_send_channel(server->net, server->handle);
		_ggzcore_net_send_logout(server->net);
	}
	else {
		server->channel_failed = 1;
	}
}


void _ggzcore_server_init_roomlist(GGZServer *server, const int num)
{
	int i;

	server->num_rooms = num;
	server->rooms = ggz_malloc(num * sizeof(GGZRoom*));
	for(i = 0; i < num; i++)
		server->rooms[i] = NULL;
}


void _ggzcore_server_free_roomlist(GGZServer *server)
{
	int i;

	for (i = 0; i < server->num_rooms; i++) {
		if(server->rooms[i])
			_ggzcore_room_free(server->rooms[i]);
	}

	ggz_free(server->rooms);
	server->num_rooms = 0;
}


void _ggzcore_server_add_room(GGZServer *server, 
			      GGZRoom *room)
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


void _ggzcore_server_init_typelist(GGZServer *server, 
				   const int num)
{
	server->num_gametypes = num;
	server->gametypes = ggz_malloc(num * sizeof(GGZGameType*));
}


void _ggzcore_server_free_typelist(GGZServer *server)
{
	int i;

	for (i = 0; i < server->num_gametypes; i++) {
		_ggzcore_gametype_free(server->gametypes[i]);
	}

	ggz_free(server->gametypes);
	server->num_gametypes = 0;
}


void _ggzcore_server_add_type(GGZServer *server, 
			      GGZGameType *type)
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
	return (event >= 0 && event < GGZ_NUM_SERVER_EVENTS);
}
	

GGZHookReturn _ggzcore_server_event(GGZServer *server, 
					   GGZServerEvent id, 
					   void *data)
{
	return _ggzcore_hook_list_invoke(server->event_hooks[id], data);
}


void _ggzcore_server_queue_players_changed(GGZServer *server)
{
	/* This queues a GGZ_SERVER_PLAYERS_CHANGED event.  The event is
	 * activated when the network code is exited so that it's not
	 * needlessly activated multiple times, which would also cause the
	 * number to be inaccurate (but only for a short time). */
	server->queued_events.players_changed = 1;
}


void _ggzcore_server_net_error(GGZServer *server, char* message)
{
	_ggzcore_server_change_state(server, GGZ_TRANS_NET_ERROR);
	_ggzcore_server_event(server, GGZ_NET_ERROR, message);
	if (server->is_channel) {
		server->channel_failed = 1;
	}
}


void _ggzcore_server_protocol_error(GGZServer *server, char* message)
{
	ggz_debug(GGZCORE_DBG_SERVER, "Protocol error: disconnecting");
	_ggzcore_net_disconnect(server->net);
	_ggzcore_server_change_state(server, GGZ_TRANS_PROTO_ERROR);
	_ggzcore_server_event(server, GGZ_PROTOCOL_ERROR, message);
	if (server->is_channel) {
		server->channel_failed = 1;
	}
}










