/*
 * File: server.c
 * Author: Brent Hendricks
 * Project: GGZ Text Client 
 * Date: 9/26/00
 *
 * Functions for handling server events
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
#include "server.h"
#include "output.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


static GGZStateID ggz_state;


/* Callback for state change events */
static void state_change(GGZStateID id, void *state_data, void *user_data);


void state_register(void)
{
	ggzcore_state_add_callback(GGZ_STATE_OFFLINE, state_change);
	ggzcore_state_add_callback(GGZ_STATE_CONNECTING, state_change);
	ggzcore_state_add_callback(GGZ_STATE_ONLINE, state_change);
	ggzcore_state_add_callback(GGZ_STATE_LOGGING_IN, state_change);
	ggzcore_state_add_callback(GGZ_STATE_LOGGED_IN, state_change);
	ggzcore_state_add_callback(GGZ_STATE_BETWEEN_ROOMS, state_change);
	ggzcore_state_add_callback(GGZ_STATE_ENTERING_ROOM, state_change);
	ggzcore_state_add_callback(GGZ_STATE_IN_ROOM, state_change);
	ggzcore_state_add_callback(GGZ_STATE_JOINING_TABLE, state_change);
	ggzcore_state_add_callback(GGZ_STATE_AT_TABLE, state_change);
	ggzcore_state_add_callback(GGZ_STATE_LEAVING_TABLE, state_change);
	ggzcore_state_add_callback(GGZ_STATE_LOGGING_OUT, state_change);
}


char* state_get(void)
{
	switch (ggz_state) {
	case GGZ_STATE_OFFLINE:
		return "Offline";
		break;
	case GGZ_STATE_CONNECTING:
		return "Connecting";
		break;
	case GGZ_STATE_ONLINE:
		return "Online";
		break;
	case GGZ_STATE_LOGGING_IN:
		return "Logging In";
		break;
	case GGZ_STATE_LOGGED_IN:
		return "Logged In";
		break;
	case GGZ_STATE_BETWEEN_ROOMS:
	case GGZ_STATE_ENTERING_ROOM:
		return "--> Room";
		break;
	case GGZ_STATE_IN_ROOM:
		return "Chatting";
		break;
	case GGZ_STATE_JOINING_TABLE:
		return "--> Table";
		break;
	case GGZ_STATE_AT_TABLE:
		return "Playing";
		break;
	case GGZ_STATE_LEAVING_TABLE:
		return "<-- Table";
		break;
	case GGZ_STATE_LOGGING_OUT:
		return "Logging Out";
		break;
	default:
		return "**none**";
		
	}
}


static void state_change(GGZStateID id, void *state_data, void *user_data)
{
	ggz_state = id;
	
	output_status();
}

