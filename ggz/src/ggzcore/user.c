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

#include <stdlib.h>

/* Callbacks */
static void handle_user_login(GGZEventID, void*, void*);
static void handle_user_chat(GGZEventID, void*, void*);


/* ggzcore_user_register() - Register callbacks for UI events
 *
 * Receives:
 *
 * Returns:
 */
void ggzcore_user_register(void)
{
	ggzcore_event_connect(GGZ_USER_LOGIN_GUEST, handle_user_login, NULL);
	ggzcore_event_connect(GGZ_USER_CHAT, handle_user_chat, NULL);
}


/* handle_user_login() - Callback for user login events
 *
 * Receives:
 * GGZEventID id    : ID code of triggered event
 * void* event_data : Event-specific data
 * void* user_data  : "User" data
 *
 * Returns:
 */
static void handle_user_login(GGZEventID id, void* event_data, void* user_data)
{
	ggzcore_debug("Executing handle_user_login");
}


/* handle_user_chat() - Callback for user chat events
 *
 * Receives:
 * GGZEventID id    : ID code of triggered event
 * void* event_data : Event-specific data
 * void* user_data  : "User" data
 *
 * Returns:
 */
static void handle_user_chat(GGZEventID id, void* event_data, void* user_data)
{
	ggzcore_debug("Executing handle_user_chat");
	if (event_data)
		ggzcore_debug("  data is %s", (char*)event_data);
}


