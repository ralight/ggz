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
	return (_ggzcore_state.id >= GGZ_STATE_IN_ROOM);
}


int ggzcore_state_is_at_table(void)
{
	return (_ggzcore_state.id >= GGZ_STATE_AT_TABLE);
}


/* FIXME: need better way of checking validity */
unsigned char _ggzcore_state_event_isvalid(GGZEventID id)
{
	unsigned char valid = 0;

	switch (_ggzcore_state.id) {
	case GGZ_STATE_OFFLINE:
		valid = (id == GGZ_USER_LOGIN);
		break;

	case GGZ_STATE_ONLINE:
		valid = (id == GGZ_USER_LOGIN || id == GGZ_USER_LOGOUT);
		break;

	case GGZ_STATE_LOGGED_IN:
		valid = (id == GGZ_USER_LOGOUT || id == GGZ_USER_JOIN_ROOM);
		break;

	case GGZ_STATE_IN_ROOM:
		valid = (id == GGZ_USER_LOGOUT || id == GGZ_USER_JOIN_ROOM
			 || id == GGZ_USER_CHAT || id == GGZ_USER_CHAT_PRVMSG
			 || id == GGZ_USER_CHAT_BEEP);
		break;
		
	default:
		valid = 0;
	}

	return valid;
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
