/*
 * File: state.c
 * Author: Brent Hendricks
 * Project: GGZ Core Client Lib
 * Date: 9/22/00
 *
 * Code for handling state manipulations
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
#include <state.h>
#include <hook.h>

#include <stdlib.h>

/* Structure to represent state transition pairs */
struct _GGZTransition {
	
	/* Current state */
	GGZEventID event;

	/* Next state */
	GGZStateID next;
};


/* Structure for a particular client state */
struct _GGZState {

	/* Unique id number */
	GGZStateID id;
	
	/* Descriptive string (mainly for debugging purposes) */
	const char *name;

	/* Array of valid state transitions */
	struct _GGZTransition *transitions;

	/* List of callback functions */
	GGZHookList *hooks;

};

/* Giant list of transitions for each state */
static struct _GGZTransition _offline_transitions[] = {
	{GGZ_USER_LOGIN,             GGZ_STATE_CONNECTING},
	{-1, -1}
};

static struct _GGZTransition _connecting_transitions[] = {
	{GGZ_SERVER_CONNECT,         GGZ_STATE_ONLINE},
	{GGZ_SERVER_CONNECT_FAIL,    GGZ_STATE_OFFLINE},
	{-1, -1}
};

static struct _GGZTransition _online_transitions[] = {
	{GGZ_SERVER_MOTD,            GGZ_STATE_ONLINE},
	{GGZ_SERVER_LIST_ROOMS,      GGZ_STATE_ONLINE},
	{GGZ_SERVER_ERROR,           GGZ_STATE_ONLINE},
	{GGZ_SERVER_TABLE_UPDATE,    GGZ_STATE_ONLINE},
	{GGZ_USER_LOGIN,             GGZ_STATE_LOGGING_IN},
	{GGZ_USER_LIST_ROOMS,        GGZ_STATE_ONLINE},
	{GGZ_USER_LIST_TYPES,        GGZ_STATE_ONLINE},
	{GGZ_USER_MOTD,              GGZ_STATE_ONLINE},
	{GGZ_USER_LOGOUT,            GGZ_STATE_LOGGING_OUT},
	{-1, -1}
};

static struct _GGZTransition _logging_in_transitions[] = {
	{GGZ_SERVER_MOTD,            GGZ_STATE_LOGGING_IN},
	{GGZ_SERVER_LOGIN,           GGZ_STATE_LOGGED_IN},
        {GGZ_SERVER_LOGIN_FAIL,      GGZ_STATE_ONLINE},
	{GGZ_SERVER_LIST_ROOMS,      GGZ_STATE_LOGGING_IN},
	{GGZ_SERVER_ERROR,           GGZ_STATE_LOGGING_IN},
	{GGZ_SERVER_TABLE_UPDATE,    GGZ_STATE_LOGGING_IN},
	{GGZ_USER_LIST_ROOMS,        GGZ_STATE_LOGGING_IN},
	{GGZ_USER_LIST_TYPES,        GGZ_STATE_LOGGING_IN},
	{GGZ_USER_MOTD,              GGZ_STATE_LOGGING_IN},
	{GGZ_USER_LOGOUT,            GGZ_STATE_LOGGING_OUT},
	{-1, -1}
};

static struct _GGZTransition _logged_in_transitions[] = {
	{GGZ_SERVER_MOTD,            GGZ_STATE_LOGGED_IN},
	{GGZ_SERVER_LIST_ROOMS,      GGZ_STATE_LOGGED_IN},
	{GGZ_SERVER_CHAT_ANNOUNCE,   GGZ_STATE_LOGGED_IN},
	{GGZ_SERVER_ERROR,           GGZ_STATE_LOGGED_IN},
	{GGZ_SERVER_TABLE_UPDATE,    GGZ_STATE_LOGGED_IN},
	{GGZ_USER_LIST_ROOMS,        GGZ_STATE_LOGGED_IN},
	{GGZ_USER_LIST_TYPES,        GGZ_STATE_LOGGED_IN},
	{GGZ_USER_JOIN_ROOM,         GGZ_STATE_ENTERING_ROOM},
	{GGZ_USER_MOTD,              GGZ_STATE_LOGGED_IN},
	{GGZ_USER_LOGOUT,            GGZ_STATE_LOGGING_OUT},
	{-1, -1}
};

static struct _GGZTransition _entering_room_transitions[] = {
	{GGZ_SERVER_MOTD,            GGZ_STATE_ENTERING_ROOM},
	{GGZ_SERVER_LIST_ROOMS,      GGZ_STATE_ENTERING_ROOM},
	{GGZ_SERVER_ROOM_JOIN,       GGZ_STATE_IN_ROOM},
	{GGZ_SERVER_ROOM_JOIN_FAIL,  GGZ_STATE_LOGGED_IN},
	{GGZ_SERVER_CHAT_FAIL,       GGZ_STATE_ENTERING_ROOM},
	{GGZ_SERVER_CHAT_ANNOUNCE,   GGZ_STATE_ENTERING_ROOM},
	{GGZ_SERVER_ERROR,           GGZ_STATE_ENTERING_ROOM},
	{GGZ_SERVER_TABLE_UPDATE,    GGZ_STATE_ENTERING_ROOM},
	{GGZ_USER_LIST_ROOMS,        GGZ_STATE_ENTERING_ROOM},
	{GGZ_USER_LIST_TYPES,        GGZ_STATE_ENTERING_ROOM},
	{GGZ_USER_MOTD,              GGZ_STATE_ENTERING_ROOM},
	{GGZ_USER_LOGOUT,            GGZ_STATE_LOGGING_OUT},
	{-1, -1}
};

static struct _GGZTransition _in_room_transitions[] = {
	{GGZ_SERVER_MOTD,            GGZ_STATE_IN_ROOM},
	{GGZ_SERVER_LIST_ROOMS,      GGZ_STATE_IN_ROOM},
	{GGZ_SERVER_LIST_PLAYERS,    GGZ_STATE_IN_ROOM},
	{GGZ_SERVER_CHAT,            GGZ_STATE_IN_ROOM},
	{GGZ_SERVER_CHAT_FAIL,       GGZ_STATE_IN_ROOM},
	{GGZ_SERVER_CHAT_MSG,        GGZ_STATE_IN_ROOM},
	{GGZ_SERVER_CHAT_ANNOUNCE,   GGZ_STATE_IN_ROOM},
	{GGZ_SERVER_CHAT_PRVMSG,     GGZ_STATE_IN_ROOM},
	{GGZ_SERVER_CHAT_BEEP,       GGZ_STATE_IN_ROOM},
	{GGZ_SERVER_ERROR,           GGZ_STATE_IN_ROOM},
	{GGZ_SERVER_ROOM_ENTER,      GGZ_STATE_IN_ROOM},
	{GGZ_SERVER_ROOM_LEAVE,      GGZ_STATE_IN_ROOM},
	{GGZ_SERVER_TABLE_UPDATE,    GGZ_STATE_IN_ROOM},
	{GGZ_USER_LIST_ROOMS,        GGZ_STATE_IN_ROOM},
	{GGZ_USER_LIST_TYPES,        GGZ_STATE_IN_ROOM},
	{GGZ_USER_JOIN_ROOM,         GGZ_STATE_BETWEEN_ROOMS},
	{GGZ_USER_LIST_TABLES,       GGZ_STATE_IN_ROOM},
	{GGZ_USER_LIST_PLAYERS,      GGZ_STATE_IN_ROOM},
	{GGZ_USER_CHAT,              GGZ_STATE_IN_ROOM},
	{GGZ_USER_CHAT_PRVMSG,       GGZ_STATE_IN_ROOM},
	{GGZ_USER_CHAT_BEEP,         GGZ_STATE_IN_ROOM},
	{GGZ_USER_MOTD,              GGZ_STATE_IN_ROOM},
	{GGZ_USER_LOGOUT,            GGZ_STATE_LOGGING_OUT},
	{-1, -1}
};

static struct _GGZTransition _between_rooms_transitions[] = {
	{GGZ_SERVER_MOTD,            GGZ_STATE_BETWEEN_ROOMS},
	{GGZ_SERVER_LIST_ROOMS,      GGZ_STATE_BETWEEN_ROOMS},
	{GGZ_SERVER_ROOM_JOIN,       GGZ_STATE_IN_ROOM},
	{GGZ_SERVER_ROOM_JOIN_FAIL,  GGZ_STATE_IN_ROOM},
	{GGZ_SERVER_CHAT_FAIL,       GGZ_STATE_BETWEEN_ROOMS},
	{GGZ_SERVER_CHAT_ANNOUNCE,   GGZ_STATE_BETWEEN_ROOMS},
	{GGZ_SERVER_ERROR,           GGZ_STATE_BETWEEN_ROOMS},
	{GGZ_SERVER_TABLE_UPDATE,    GGZ_STATE_BETWEEN_ROOMS},
	{GGZ_USER_LIST_ROOMS,        GGZ_STATE_BETWEEN_ROOMS},
	{GGZ_USER_LIST_TYPES,        GGZ_STATE_BETWEEN_ROOMS},
	{GGZ_USER_MOTD,              GGZ_STATE_BETWEEN_ROOMS},
	{GGZ_USER_LOGOUT,            GGZ_STATE_LOGGING_OUT},
	{-1, -1}
};

static struct _GGZTransition _joining_table_transitions[] = {
	{GGZ_SERVER_MOTD,            GGZ_STATE_JOINING_TABLE},
	{GGZ_SERVER_LIST_ROOMS,      GGZ_STATE_JOINING_TABLE},
	{GGZ_SERVER_LIST_PLAYERS,    GGZ_STATE_JOINING_TABLE},
	{GGZ_SERVER_CHAT,            GGZ_STATE_JOINING_TABLE},
	{GGZ_SERVER_CHAT_FAIL,       GGZ_STATE_JOINING_TABLE},
	{GGZ_SERVER_CHAT_MSG,        GGZ_STATE_JOINING_TABLE},
	{GGZ_SERVER_CHAT_ANNOUNCE,   GGZ_STATE_JOINING_TABLE},
	{GGZ_SERVER_CHAT_PRVMSG,     GGZ_STATE_JOINING_TABLE},
	{GGZ_SERVER_CHAT_BEEP,       GGZ_STATE_JOINING_TABLE},
	{GGZ_SERVER_ERROR,           GGZ_STATE_JOINING_TABLE},
	{GGZ_SERVER_ROOM_ENTER,      GGZ_STATE_JOINING_TABLE},
	{GGZ_SERVER_ROOM_LEAVE,      GGZ_STATE_JOINING_TABLE},
	{GGZ_SERVER_TABLE_UPDATE,    GGZ_STATE_JOINING_TABLE},
	{GGZ_USER_LIST_ROOMS,        GGZ_STATE_JOINING_TABLE},
	{GGZ_USER_LIST_TYPES,        GGZ_STATE_JOINING_TABLE},
	{GGZ_USER_LIST_TABLES,       GGZ_STATE_JOINING_TABLE},
	{GGZ_USER_LIST_PLAYERS,      GGZ_STATE_JOINING_TABLE},
	{GGZ_USER_CHAT,              GGZ_STATE_JOINING_TABLE},
	{GGZ_USER_CHAT_PRVMSG,       GGZ_STATE_JOINING_TABLE},
	{GGZ_USER_CHAT_BEEP,         GGZ_STATE_JOINING_TABLE},
	{GGZ_USER_MOTD,              GGZ_STATE_JOINING_TABLE},
	{-1, -1}
};

static struct _GGZTransition _at_table_transitions[] = {
	{GGZ_SERVER_MOTD,            GGZ_STATE_AT_TABLE},
	{GGZ_SERVER_LIST_ROOMS,      GGZ_STATE_AT_TABLE},
	{GGZ_SERVER_LIST_PLAYERS,    GGZ_STATE_AT_TABLE},
	{GGZ_SERVER_CHAT,            GGZ_STATE_AT_TABLE},
	{GGZ_SERVER_CHAT_FAIL,       GGZ_STATE_AT_TABLE},
	{GGZ_SERVER_CHAT_MSG,        GGZ_STATE_AT_TABLE},
	{GGZ_SERVER_CHAT_ANNOUNCE,   GGZ_STATE_AT_TABLE},
	{GGZ_SERVER_CHAT_BEEP,       GGZ_STATE_AT_TABLE},
	{GGZ_SERVER_ERROR,           GGZ_STATE_AT_TABLE},
	{GGZ_SERVER_ROOM_ENTER,      GGZ_STATE_AT_TABLE},
	{GGZ_SERVER_ROOM_LEAVE,      GGZ_STATE_AT_TABLE},
	{GGZ_SERVER_TABLE_UPDATE,    GGZ_STATE_AT_TABLE},
	{GGZ_USER_LIST_ROOMS,        GGZ_STATE_AT_TABLE},
	{GGZ_USER_LIST_TYPES,        GGZ_STATE_AT_TABLE},
	{GGZ_USER_LIST_TABLES,       GGZ_STATE_AT_TABLE},
	{GGZ_USER_LIST_PLAYERS,      GGZ_STATE_AT_TABLE},
	{GGZ_USER_CHAT,              GGZ_STATE_AT_TABLE},
	{GGZ_USER_MOTD,              GGZ_STATE_AT_TABLE},
	{-1, -1}
};

static struct _GGZTransition _leaving_table_transitions[] = {
	{GGZ_SERVER_MOTD,            GGZ_STATE_LEAVING_TABLE},
	{GGZ_SERVER_LIST_ROOMS,      GGZ_STATE_LEAVING_TABLE},
	{GGZ_SERVER_LIST_PLAYERS,    GGZ_STATE_LEAVING_TABLE},
	{GGZ_SERVER_CHAT,            GGZ_STATE_LEAVING_TABLE},
	{GGZ_SERVER_CHAT_FAIL,       GGZ_STATE_LEAVING_TABLE},
	{GGZ_SERVER_CHAT_MSG,        GGZ_STATE_LEAVING_TABLE},
	{GGZ_SERVER_CHAT_ANNOUNCE,   GGZ_STATE_LEAVING_TABLE},
	{GGZ_SERVER_CHAT_PRVMSG,     GGZ_STATE_LEAVING_TABLE},
	{GGZ_SERVER_CHAT_BEEP,       GGZ_STATE_LEAVING_TABLE},
	{GGZ_SERVER_ERROR,           GGZ_STATE_LEAVING_TABLE},
	{GGZ_SERVER_ROOM_ENTER,      GGZ_STATE_LEAVING_TABLE},
	{GGZ_SERVER_ROOM_LEAVE,      GGZ_STATE_LEAVING_TABLE},
	{GGZ_SERVER_TABLE_UPDATE,    GGZ_STATE_LEAVING_TABLE},
	{GGZ_USER_LIST_ROOMS,        GGZ_STATE_LEAVING_TABLE},
	{GGZ_USER_LIST_TYPES,        GGZ_STATE_LEAVING_TABLE},
	{GGZ_USER_LIST_TABLES,       GGZ_STATE_LEAVING_TABLE},
	{GGZ_USER_LIST_PLAYERS,      GGZ_STATE_LEAVING_TABLE},
	{GGZ_USER_CHAT,              GGZ_STATE_LEAVING_TABLE},
	{GGZ_USER_CHAT_PRVMSG,       GGZ_STATE_LEAVING_TABLE},
	{GGZ_USER_CHAT_BEEP,         GGZ_STATE_LEAVING_TABLE},
	{GGZ_USER_MOTD,              GGZ_STATE_LEAVING_TABLE},
	{-1, -1}
};

static struct _GGZTransition _logging_out_transitions[] = {
	{GGZ_SERVER_LOGOUT,          GGZ_STATE_OFFLINE},
	{GGZ_SERVER_ERROR,           GGZ_STATE_LOGGING_OUT},
	{-1, -1}
};


/* Array of all GGZ states */
static struct _GGZState _ggz_states[] = {
	{GGZ_STATE_OFFLINE,       "offline",       _offline_transitions},
	{GGZ_STATE_CONNECTING,    "connecting",    _connecting_transitions},
	{GGZ_STATE_ONLINE,        "online",        _online_transitions}, 
	{GGZ_STATE_LOGGING_IN,    "logging_in",    _logging_in_transitions},
	{GGZ_STATE_LOGGED_IN,     "logged_in",     _logged_in_transitions},
	{GGZ_STATE_ENTERING_ROOM, "entering_room", _entering_room_transitions},
	{GGZ_STATE_IN_ROOM,       "in_room",       _in_room_transitions},
	{GGZ_STATE_BETWEEN_ROOMS, "between_rooms", _between_rooms_transitions},
	{GGZ_STATE_JOINING_TABLE, "joining_table", _joining_table_transitions},
	{GGZ_STATE_AT_TABLE,      "at_table",      _at_table_transitions},
	{GGZ_STATE_LEAVING_TABLE, "leaving_table", _leaving_table_transitions},
	{GGZ_STATE_LOGGING_OUT,   "logging_out",   _logging_out_transitions},
};

static void _ggzcore_state_dump_callbacks(GGZStateID id);
static void _ggzcore_state_set(GGZStateID id);


/* Global state variable */
struct _GGZClientState _ggzcore_state;


/* Publicly Exported functions (prototypes in ggzcore.h) */

int ggzcore_state_add_callback(const GGZStateID id, const GGZCallback func)
{
	return ggzcore_state_add_callback_full(id, func, NULL, NULL);
}


int ggzcore_state_add_callback_full(const GGZStateID id, 
				    const GGZCallback func,
				    void* data, GGZDestroyFunc destroy)
{
	int status = _ggzcore_hook_add_full(_ggz_states[id].hooks, func, data, 
					    destroy);

	_ggzcore_state_dump_callbacks(id);
	
	return status;
}


int ggzcore_state_remove_callback(const GGZStateID id, const GGZCallback func)
{
	int status;

	status = _ggzcore_hook_remove(_ggz_states[id].hooks, func);
	
	if (status == 0)
		ggzcore_debug(GGZ_DBG_STATE, 
			      "Removing callback from state %s", 
			      _ggz_states[id].name);
	else
		ggzcore_debug(GGZ_DBG_STATE, 
			      "Can't find callback to remove from state %s",
			      _ggz_states[id].name);
	
	return status; 
}


int ggzcore_state_remove_callback_id(const GGZStateID id, 
				     const unsigned int callback_id)
{
	int status;

	status = _ggzcore_hook_remove_id(_ggz_states[id].hooks, callback_id);
	
	if (status == 0)
		ggzcore_debug(GGZ_DBG_STATE, 
			      "Removing callback %d from state %s", 
			      callback_id, _ggz_states[id].name);
	else
		ggzcore_debug(GGZ_DBG_STATE, 
			      "Can't find callback %d to remove from state %s",
			      callback_id, _ggz_states[id].name);
	
	return status; 
}


int ggzcore_state_is_online(void)
{
	return (_ggzcore_state.id >= GGZ_STATE_ONLINE);
}


int ggzcore_state_is_logged_in(void)
{
	return (_ggzcore_state.id >= GGZ_STATE_LOGGED_IN);
}


int ggzcore_state_is_in_room(void)
{
	return (_ggzcore_state.id >= GGZ_STATE_IN_ROOM
		&& _ggzcore_state.id < GGZ_STATE_LOGGING_OUT);
}


int ggzcore_state_is_at_table(void)
{
	return (_ggzcore_state.id >= GGZ_STATE_AT_TABLE
		&& _ggzcore_state.id <= GGZ_STATE_LEAVING_TABLE);
}


GGZStateID ggzcore_state_get_id(void)
{
	return _ggzcore_state.id;
}


char* ggzcore_state_get_profile_login(void)
{
	return _ggzcore_state.profile.login;
}


char* ggzcore_state_get_profile_name(void)
{
	return _ggzcore_state.profile.name;
}


char* ggzcore_state_get_profile_host(void)
{
	return _ggzcore_state.profile.host;
}


int ggzcore_state_get_room(void)
{
	return _ggzcore_state.room;
}


/* Internal library functions (prototypes in state.h) */


void _ggzcore_state_init(void)
{	
	int i, num_states;

	/* Setup states */
	num_states = sizeof(_ggz_states)/sizeof(struct _GGZState);
	for (i = 0; i < num_states; i++) {
		if (_ggz_states[i].id != i)
			ggzcore_debug(GGZ_DBG_STATE, "ID mismatch: %d != %d",
				      _ggz_states[i].id, i);
		else {
			_ggz_states[i].hooks = _ggzcore_hook_list_init(i);
			ggzcore_debug(GGZ_DBG_STATE, 
				      "Setting up state %s with id %d", 
				      _ggz_states[i].name, _ggz_states[i].id);
		}
	}

	/* Initialize state variable */
	_ggzcore_state.id = GGZ_STATE_OFFLINE;
	_ggzcore_state.room = -1;
	_ggzcore_state.table = -1;
	_ggzcore_state.trans_room = -1;
	_ggzcore_state.trans_table = -1;
}


int _ggzcore_state_event_is_valid(GGZEventID id)
{
	int i = 0, valid = 0;
	struct _GGZTransition *transitions;

	transitions = _ggz_states[_ggzcore_state.id].transitions;
	
	/* Look through valid transitions to see if the event qualifies */
	while (transitions[i].event != -1) {
		if (transitions[i].event == id) {
			valid = 1;
			break;
		}
		++i;
	}

	return valid;
}


void _ggzcore_state_transition(GGZEventID id)
{
	int i = 0;
	struct _GGZTransition *transitions;
	GGZStateID next = -1;

	transitions = _ggz_states[_ggzcore_state.id].transitions;
	
	/* Look through valid transitions to see if the event qualifies */
	while (transitions[i].event != -1) {
		if (transitions[i].event == id) {
			next = transitions[i].next;
			break;
		}
		++i;
	}

	if (next != _ggzcore_state.id && next != -1) {
		_ggzcore_state_set(next);
		ggzcore_debug(GGZ_DBG_STATE, "State transition %s -> %s", 
			      _ggz_states[_ggzcore_state.id].name, 
			      _ggz_states[next].name);
	}
}


void _ggzcore_state_destroy(void)
{
	int i, num_states;
	
	num_states = sizeof(_ggz_states)/sizeof(struct _GGZState);
	for (i = 0; i < num_states; i++) {
		ggzcore_debug(GGZ_DBG_STATE, "Cleaning up %s state", 
			      _ggz_states[i].name);
		_ggzcore_hook_list_destroy(_ggz_states[i].hooks);
		_ggz_states[i].hooks = NULL;
	}
}


/* Static functions internal to this file */

static void _ggzcore_state_set(GGZStateID id)
{
	_ggzcore_state.id = id;

	_ggzcore_hook_list_invoke(_ggz_states[id].hooks, NULL);

	switch(id) {
	case GGZ_STATE_OFFLINE:
		_ggzcore_state.room = -1;
		_ggzcore_state.table = -1;
		break;
		
	default:
		break;
	}
}


/* Debugging function to examine state of callback list */
static void _ggzcore_state_dump_callbacks(GGZStateID id)
{
	struct _GGZHook *cur;

	ggzcore_debug(GGZ_DBG_STATE, "-- State: %s", _ggz_states[id].name);
	for (cur = _ggz_states[id].hooks->hooks; cur != NULL; cur = cur->next)
		ggzcore_debug(GGZ_DBG_STATE, "  Callback id %d", cur->id);
	ggzcore_debug(GGZ_DBG_STATE, "-- End of state");
}

