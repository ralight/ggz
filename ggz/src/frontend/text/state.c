/*
 * File: server.c
 * Author: Brent Hendricks
 * Project: GGZ Text Client 
 * Date: 9/26/00
 * $Id: state.c 4868 2002-10-11 19:35:05Z jdorje $
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

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ggzcore.h>

#include "output.h"
#include "server.h"
#include "state.h"


char* state_get(void)
{
	GGZStateID state;

	if (!server)
		return "Offline";
	
	state = ggzcore_server_get_state(server);

	switch (state) {
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
	case GGZ_STATE_LAUNCHING_TABLE:
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
