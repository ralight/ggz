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
#include <msg.h>

#include <stdlib.h>



/* Structure to represent state transition pairs */
struct _GGZTransition {
	
	/* Transition ID */
	GGZTransID id;

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

	/* List of hook functions */
	GGZHookList *hooks;

};

/* Giant list of transitions for each state */
static struct _GGZTransition _offline_transitions[] = {
	{GGZ_TRANS_CONN_TRY,     GGZ_STATE_CONNECTING},
	{-1, -1}
};

static struct _GGZTransition _connecting_transitions[] = {
	{GGZ_TRANS_CONN_OK,      GGZ_STATE_ONLINE},
	{GGZ_TRANS_CONN_FAIL,    GGZ_STATE_OFFLINE},
	{-1, -1}
};

static struct _GGZTransition _online_transitions[] = {
	{GGZ_TRANS_LOGIN_TRY,    GGZ_STATE_LOGGING_IN},
	{GGZ_TRANS_LOGOUT_TRY,   GGZ_STATE_LOGGING_OUT},
	{-1, -1}
};

static struct _GGZTransition _logging_in_transitions[] = {
	{GGZ_TRANS_LOGIN_OK,     GGZ_STATE_LOGGED_IN},
        {GGZ_TRANS_LOGIN_FAIL,   GGZ_STATE_ONLINE},
	{GGZ_TRANS_LOGOUT_TRY,   GGZ_STATE_LOGGING_OUT},
	{-1, -1}
};

static struct _GGZTransition _logged_in_transitions[] = {
	{GGZ_TRANS_ENTER_TRY,    GGZ_STATE_ENTERING_ROOM},
	{GGZ_TRANS_LOGOUT_TRY,   GGZ_STATE_LOGGING_OUT},
	{-1, -1}
};

static struct _GGZTransition _entering_room_transitions[] = {
	{GGZ_TRANS_ENTER_OK,     GGZ_STATE_IN_ROOM},
	{GGZ_TRANS_ENTER_FAIL,   GGZ_STATE_LOGGED_IN},
	{GGZ_TRANS_LOGOUT_TRY,   GGZ_STATE_LOGGING_OUT},
	{-1, -1}
};

static struct _GGZTransition _in_room_transitions[] = {
	{GGZ_TRANS_ENTER_TRY,    GGZ_STATE_BETWEEN_ROOMS},
	{GGZ_TRANS_LOGOUT_TRY,   GGZ_STATE_LOGGING_OUT},
	{-1, -1}
};

static struct _GGZTransition _between_rooms_transitions[] = {
	{GGZ_TRANS_ENTER_OK,     GGZ_STATE_IN_ROOM},
	{GGZ_TRANS_ENTER_FAIL,   GGZ_STATE_IN_ROOM},
	{GGZ_TRANS_LOGOUT_TRY,   GGZ_STATE_LOGGING_OUT},
	{-1, -1}
};

static struct _GGZTransition _joining_table_transitions[] = {
	{-1, -1}
};

static struct _GGZTransition _at_table_transitions[] = {
	{-1, -1}
};

static struct _GGZTransition _leaving_table_transitions[] = {
	{-1, -1}
};

static struct _GGZTransition _logging_out_transitions[] = {
	{GGZ_TRANS_LOGOUT_OK,      GGZ_STATE_OFFLINE},
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

static void _ggzcore_state_dump_hooks(GGZStateID id);
static void _ggzcore_state_set(GGZStateID id);
static GGZHookReturn _ggzcore_state_event_handler(GGZEventID id, 
						  void *event_data,
						  void *user_data);

/* Global state variable */
struct _GGZClientState _ggzcore_state;


/* Publicly Exported functions (prototypes in ggzcore.h) */

int ggzcore_state_add_hook(const GGZStateID id, const GGZHookFunc func)
{
	return ggzcore_state_add_hook_full(id, func, NULL);
}


int ggzcore_state_add_hook_full(const GGZStateID id, 
				    const GGZHookFunc func,
				    void* data)
{
	int status = _ggzcore_hook_add_full(_ggz_states[id].hooks, func, data);
					    
	_ggzcore_state_dump_hooks(id);
	
	return status;
}


int ggzcore_state_remove_hook(const GGZStateID id, const GGZHookFunc func)
{
	int status;

	status = _ggzcore_hook_remove(_ggz_states[id].hooks, func);
	
	if (status == 0)
		ggzcore_debug(GGZ_DBG_STATE, 
			      "Removing hook from state %s", 
			      _ggz_states[id].name);
	else
		ggzcore_debug(GGZ_DBG_STATE, 
			      "Can't find hook to remove from state %s",
			      _ggz_states[id].name);
	
	return status; 
}


int ggzcore_state_remove_hook_id(const GGZStateID id, 
				     const unsigned int hook_id)
{
	int status;

	status = _ggzcore_hook_remove_id(_ggz_states[id].hooks, hook_id);
	
	if (status == 0)
		ggzcore_debug(GGZ_DBG_STATE, 
			      "Removing hook %d from state %s", 
			      hook_id, _ggz_states[id].name);
	else
		ggzcore_debug(GGZ_DBG_STATE, 
			      "Can't find hook %d to remove from state %s",
			      hook_id, _ggz_states[id].name);
	
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
			ggzcore_debug(GGZ_DBG_INIT, 
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

	ggzcore_event_add_hook(GGZ_SERVER_CONNECT, 
				   _ggzcore_state_event_handler);
	ggzcore_event_add_hook(GGZ_SERVER_CONNECT_FAIL, 
				   _ggzcore_state_event_handler);
	ggzcore_event_add_hook(GGZ_SERVER_LOGIN, 
				   _ggzcore_state_event_handler);
	ggzcore_event_add_hook(GGZ_SERVER_LOGIN_FAIL,  
				   _ggzcore_state_event_handler);
	ggzcore_event_add_hook(GGZ_SERVER_ROOM_JOIN, 
				   _ggzcore_state_event_handler);
	ggzcore_event_add_hook(GGZ_SERVER_ROOM_JOIN_FAIL, 
				   _ggzcore_state_event_handler);
	ggzcore_event_add_hook(GGZ_SERVER_LOGOUT, 
				   _ggzcore_state_event_handler);
	ggzcore_event_add_hook(GGZ_NET_ERROR, 
				   _ggzcore_state_event_handler);
	ggzcore_event_add_hook(GGZ_USER_LOGIN,  
				   _ggzcore_state_event_handler);
	ggzcore_event_add_hook(GGZ_USER_JOIN_ROOM, 
				   _ggzcore_state_event_handler);
	ggzcore_event_add_hook(GGZ_USER_LOGOUT,
				   _ggzcore_state_event_handler);
}


GGZStateID _ggzcore_state_get_id(void)
{
	return _ggzcore_state.id;
}


void _ggzcore_state_transition(GGZTransID id)
{
	int i = 0;
	struct _GGZTransition *transitions;
	GGZStateID next = -1;

	transitions = _ggz_states[_ggzcore_state.id].transitions;
	
	/* Look through valid transitions to see if this one is OK */
	while (transitions[i].id != -1) {
		if (transitions[i].id == id) {
			next = transitions[i].next;
			break;
		}
		++i;
	}

	if (next != _ggzcore_state.id && next != -1) {
		ggzcore_debug(GGZ_DBG_STATE, "State transition %s -> %s", 
			      _ggz_states[_ggzcore_state.id].name, 
			      _ggz_states[next].name);
		_ggzcore_state_set(next);
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


/* Debugging function to examine state of hook list */
static void _ggzcore_state_dump_hooks(GGZStateID id)
{
	ggzcore_debug(GGZ_DBG_HOOK, "-- State: %s", _ggz_states[id].name);
	_ggzcore_hook_list_dump(_ggz_states[id].hooks);
	ggzcore_debug(GGZ_DBG_HOOK, "-- End of state");
}


static GGZHookReturn _ggzcore_state_event_handler(GGZEventID id, 
						  void *event_data,
						  void *user_data)
{
	switch (id) {
	case GGZ_USER_LOGIN: 
		if (_ggzcore_state.id == GGZ_STATE_OFFLINE)
			_ggzcore_state_transition(GGZ_TRANS_CONN_TRY);
		else
			_ggzcore_state_transition(GGZ_TRANS_LOGIN_TRY);
		break;
	case GGZ_SERVER_CONNECT:
		_ggzcore_state_transition(GGZ_TRANS_CONN_OK);
		break;
	case GGZ_SERVER_CONNECT_FAIL:
		_ggzcore_state_transition(GGZ_TRANS_CONN_FAIL);
		break;

	case GGZ_SERVER_LOGIN:
		_ggzcore_state_transition(GGZ_TRANS_LOGIN_OK);
		break;
	case GGZ_SERVER_LOGIN_FAIL:
		_ggzcore_state_transition(GGZ_TRANS_LOGIN_FAIL);
		break;

	case GGZ_USER_JOIN_ROOM:
		_ggzcore_state_transition(GGZ_TRANS_ENTER_TRY);
		break;
	case GGZ_SERVER_ROOM_JOIN:
		_ggzcore_state_transition(GGZ_TRANS_ENTER_OK);
		break;
	case GGZ_SERVER_ROOM_JOIN_FAIL:
		_ggzcore_state_transition(GGZ_TRANS_ENTER_FAIL);
		break;

	case GGZ_USER_LOGOUT:
		_ggzcore_state_transition(GGZ_TRANS_LOGOUT_TRY);
		break;
	case GGZ_SERVER_LOGOUT:
		_ggzcore_state_transition(GGZ_TRANS_LOGOUT_OK);
		break;

	default:
		break;
	}

	return GGZ_HOOK_OK;
}
