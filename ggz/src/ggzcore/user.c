/*
 * File: user.c
 * Author: Brent Hendricks
 * Project: GGZ Core Client Lib
 * Date: 9/16/00
 *
 * Code for responding to user events
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
#include <user.h>
#include <net.h>
#include <state.h>

#include <stdlib.h>

/* Callbacks */
static void user_login(GGZEventID, void*, void*);
static void user_chat(GGZEventID, void*, void*);


/* ggzcore_user_register() - Register callbacks for UI events
 *
 * Receives:
 *
 * Returns:
 */
void _ggzcore_user_register(void)
{
	ggzcore_event_connect(GGZ_USER_LOGIN, user_login, NULL);
	ggzcore_event_connect(GGZ_USER_CHAT, user_chat, NULL);
}


/* user_login() - Callback for user login events
 *
 * Receives:
 * GGZEventID id    : ID code of triggered event
 * void* event_data : Event-specific data
 * void* user_data  : "User" data
 *
 * Returns:
 */
static void user_login(GGZEventID id, void* event_data, void* user_data)
{
	int sock; 
	
	GGZProfile* profile = (GGZProfile*)event_data;

	ggzcore_debug("Executing user_login");
	ggzcore_debug("Profile name %s", profile->name);
	ggzcore_debug("Profile host %s", profile->host);
	ggzcore_debug("Profile port %d", profile->port);
	ggzcore_debug("Profile type %d", profile->type);
	ggzcore_debug("Profile login %s", profile->login);
	ggzcore_debug("Profile password %s", profile->password);

	/* FIXME: Handle threaded I/O */
	if ( (sock = _ggzcore_net_connect(profile->host, profile->port)) < 0)
		ggzcore_event_trigger(GGZ_SERVER_LOGIN_FAIL, NULL, 0);
	else 
		/* FIXME: set a timeout for connecting */
		_ggzcore_state.sock = sock;
}


/* user_chat() - Callback for user chat events
 *
 * Receives:
 * GGZEventID id    : ID code of triggered event
 * void* event_data : Event-specific data
 * void* user_data  : "User" data
 *
 * Returns:
 */
static void user_chat(GGZEventID id, void* event_data, void* user_data)
{
	ggzcore_debug("Executing user_chat");
	if (event_data)
		ggzcore_debug("  data is %s", (char*)event_data);
}


