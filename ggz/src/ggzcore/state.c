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

/* Global state variable */
struct _GGZClientState _ggzcore_state;


/* Structure for a particular client state */
struct _GGZState {

	/* Unique id number */
	GGZStateID id;
	
	/* Descriptive string (mainly for debugging purposes) */
	const char *name;

	/* List of callback functions */
	GGZHookList *hooks;
};

/* Array of all GGZ states */
static struct _GGZState ggz_states[] = {
	{GGZ_STATE_OFFLINE,       "offline"},
	{GGZ_STATE_CONNECTING,    "connecting"},
	{GGZ_STATE_ONLINE,        "online"}, 
	{GGZ_STATE_LOGGING_IN,    "logging_in"},
	{GGZ_STATE_LOGGED_IN,     "logged_in"},
	{GGZ_STATE_ENTERING_ROOM, "entering_room"},
	{GGZ_STATE_IN_ROOM,       "in_room"},
	{GGZ_STATE_BETWEEN_ROOMS, "between_rooms"},
	{GGZ_STATE_JOINING_TABLE, "joining_table"},
	{GGZ_STATE_AT_TABLE,      "at_table"},
	{GGZ_STATE_LEAVING_TABLE, "leaving_table"},
	{GGZ_STATE_LOGGING_OUT,   "logging_out"},
	{GGZ_STATE_NONE,          "** none **"}
};

static void _ggzcore_state_dump_callbacks(GGZStateID id);

void _ggzcore_state_init(void)
{	
	int i, num_states;

	/* Setup states */
	num_states = sizeof(ggz_states)/sizeof(struct _GGZState);
	for (i = 0; i < num_states; i++) {
		if (ggz_states[i].id != i)
			ggzcore_debug(GGZ_DBG_STATE, "ID mismatch: %d != %d",
				      ggz_states[i].id, i);
		else {
			ggz_states[i].hooks = _ggzcore_hook_list_init(i);
			ggzcore_debug(GGZ_DBG_STATE, 
				      "Setting up state %s with id %d", 
				      ggz_states[i].name, ggz_states[i].id);
		}
	}

	/* Initialize state variable */
	_ggzcore_state.id = GGZ_STATE_OFFLINE;
	_ggzcore_state.room = -1;
	_ggzcore_state.table = -1;
	_ggzcore_state.trans_room = -1;
	_ggzcore_state.trans_table = -1;
}


/* ggzcore_state_add_callback() - Register callback for specific GGZ-state
 *
 * Receives:
 * GGZStateID id         : ID code of state
 * GGZCallback callback : Callback function
 *
 * Returns:
 * int : id for this callback 
 */
int ggzcore_state_add_callback(const GGZStateID id, const GGZCallback func)
{
	return ggzcore_state_add_callback_full(id, func, NULL, NULL);
}


/* ggzcore_state_add_callback_full() - Register callback for specific GGZ-state
 *                                     specifying all parameters
 * 
 * Receives:
 * GGZStateID id          : ID code of state
 * GGZCallback func       : Callback function
 * void* data             : pointer to pass to callback
 * GGZDestroyFunc destroy : function to call to free user data
 *
 * Returns:
 * int : id for this callback 
 */
int ggzcore_state_add_callback_full(const GGZStateID id, 
				    const GGZCallback func,
				    void* data, GGZDestroyFunc destroy)
{
	int status = _ggzcore_hook_add_full(ggz_states[id].hooks, func, data, 
					    destroy);

	_ggzcore_state_dump_callbacks(id);
	
	return status;
}


/* ggzcore_state_remove_callback() - Remove specific callback from an state
 *
 * Receives:
 * GGZStateID id     : ID code of state
 * GGZCallback func  : pointer to callback function to remove
 *
 * Returns:
 * int : 0 if successful, -1 on error
 */
int ggzcore_state_remove_callback(const GGZStateID id, const GGZCallback func)
{
	int status;

	status = _ggzcore_hook_remove(ggz_states[id].hooks, func);
	
	if (status == 0)
		ggzcore_debug(GGZ_DBG_STATE, 
			      "Removing callback from state %s", 
			      ggz_states[id].name);
	else
		ggzcore_debug(GGZ_DBG_STATE, 
			      "Can't find callback to remove from state %s",
			      ggz_states[id].name);
	
	return status; 
}


/* ggzcore_state_remove_callback_id() - Remove callback (specified by id) 
 *                                      from an state
 *
 * Receives:
 * GGZStateID id            : ID code of state
 * unsigned int callback_id : ID of callback to remove
 *
 * Returns:
 * int : 0 if successful, -1 on error
 */
int ggzcore_state_remove_callback_id(const GGZStateID id, 
				     const unsigned int callback_id)
{
	int status;

	status = _ggzcore_hook_remove_id(ggz_states[id].hooks, callback_id);
	
	if (status == 0)
		ggzcore_debug(GGZ_DBG_STATE, 
			      "Removing callback %d from state %s", 
			      callback_id, ggz_states[id].name);
	else
		ggzcore_debug(GGZ_DBG_STATE, 
			      "Can't find callback %d to remove from state %s",
			      callback_id, ggz_states[id].name);
	
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


void _ggzcore_state_set(GGZStateID id)
{
	/* FIXME: should we check for valid state? */
	ggzcore_debug(GGZ_DBG_STATE, "State transition %s -> %s", 
		      ggz_states[_ggzcore_state.id].name, 
		      ggz_states[id].name);
	
	_ggzcore_state.id = id;

	_ggzcore_hook_list_invoke(ggz_states[id].hooks, NULL);

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

	ggzcore_debug(GGZ_DBG_STATE, "-- State: %s", ggz_states[id].name);
	for (cur = ggz_states[id].hooks->hooks; cur != NULL; cur = cur->next)
		ggzcore_debug(GGZ_DBG_STATE, "  Callback id %d", cur->id);
	ggzcore_debug(GGZ_DBG_STATE, "-- End of state");
}


/* _ggzcore_state_destroy() - Cleanup state system
 *
 * Receives:
 *
 * Returns:
 */
void _ggzcore_state_destroy(void)
{
	int i, num_states;

	num_states = sizeof(ggz_states)/sizeof(struct _GGZState);
	for (i = 0; i < num_states; i++) {
		ggzcore_debug(GGZ_DBG_STATE, "Cleaning up %s state", 
			      ggz_states[i].name);
		_ggzcore_hook_list_destroy(ggz_states[i].hooks);
		ggz_states[i].hooks = NULL;
	}
}
