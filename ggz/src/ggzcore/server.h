/*
 * File: server.h
 * Author: Brent Hendricks
 * Project: GGZ Core Client Lib
 * Date: 1/19/01
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

#ifndef __SERVER_H__
#define __SERVER_H__

#include "ggzcore.h"
#include "event.h"
#include "hook.h"
#include "state.h"
#include "roomlist.h"

#define GGZ_NUM_SERVER_EVENTS 13

/* The GGZServer structure manages information about a particular
   GGZ server connection */
struct _GGZServer {
	
	/* Host name of server */
	char *host;

	/* Port on which GGZ server in running */
	unsigned int port;

	/* Login type: one of GGZ_LOGIN, GGZ_LOGIN_GUEST, GGZ_LOGIN_NEW */
	GGZLoginType type;

	/* User handle on this server */
	char* handle;

	/* Password for this server (optional) */
	char* password;

	/* File descriptor for communication with server */
	int fd;
	
	/* Current state */
	GGZStateID state;
	
	/* List of rooms in this server */
	GGZRoomList *room_list;

	/* List of game types */
	struct _ggzcore_list *gametype_list;

	/* Current room on game server */
	int room;

	/* Room to which we are transitioning */
	int new_room;

	/* Server events */
	GGZHookList *event_hooks[GGZ_NUM_SERVER_EVENTS];

	/* Room verbosity (need to save) */
	char room_verbose;
};


#endif /* __SERVER_H__ */
