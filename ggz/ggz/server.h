/*
 * File: server.h
 * Author: Brent Hendricks
 * Project: GGZ Client
 * Date: 6/19/00
 *
 * This file contains functions for handling server connection profiles
 *
 * Copyright (C) 1998 Brent Hendricks.
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


#ifndef _GGZ_SERVER_H
#define _GGZ_SERVER_H

/* 
 * The Server structure is meant to be a node in a linked list of
 * the GGZ server profiles
 */

#include <glib.h>

typedef struct Server {
	
	/* Name of profile */
	gchar* name;
	
	/* Hostname of GGZ server */
	gchar* host;

	/* Port on which GGZ server in running */
	guint port;

	/* Login type: one of GGZ_LOGIN, GGZ_LOGIN_GUEST, GGZ_LOGIN_NEW */
	guchar type;

	/* Login name on this server */
	gchar* login;

	/* Password for this server (optional) */
	gchar* password;

} Server;


void server_profiles_load(void);

void server_list_add(Server* server);

GList* server_get_names(void);

Server* server_get(gchar* name);

#endif
