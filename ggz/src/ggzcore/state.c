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


/* Global state variable */
struct _GGZState _ggzcore_state;


void _ggzcore_state_init(void)
{	
	/* Initialize state variable */
	_ggzcore_state.id = GGZ_STATE_OFFLINE;
	_ggzcore_state.room = -1;
	_ggzcore_state.table = -1;
	_ggzcore_state.trans_room = -1;
	_ggzcore_state.trans_table = -1;
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
	ggzcore_debug(GGZ_DBG_STATE, "State transition %d -> %d", 
		      _ggzcore_state.id, id);
	
	_ggzcore_state.id = id;

	switch(id) {
	case GGZ_STATE_OFFLINE:
		_ggzcore_state.room = -1;
		_ggzcore_state.table = -1;
		break;
		
	default:
		break;
	}
}
