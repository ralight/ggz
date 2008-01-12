/*
 * File: client.h
 * Author: Brent Hendricks
 * Project: GGZ Server
 * Date: 4/26/02
 * Desc: Functions for handling client connections
 * $Id: client.h 9525 2008-01-12 22:03:18Z josef $
 *
 * Copyright (C) 2002 Brent Hendricks.
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


#ifndef _GGZ_CLIENT_H
#define _GGZ_CLIENT_H

#include <pthread.h>

#include "ggzd.h"


/* Connection to a client */
struct GGZClient {
	
	/* Client Type (player, game channel, server, etc) */
	GGZClientType type;

	/* Network IO object for communicating with the client */
	GGZNetIO *net;

	/* IP (or hostname) address from which client connected */
	char addr[64];

	/* ID of the thread handling this client's requests */
	pthread_t thread;

	/* Client data object */
	void *data;

	/* Flag to stop client session */
	char session_over;

	/* Preferred language to communicate with the client */
	const char *language;
};

void client_handler_launch(int sock);
#if 0
void client_set_ip_ban_list(int count, char **list);
#endif
void client_set_type(GGZClient *client, GGZClientType type);
void client_set_language(GGZClient *client, const char *language);
void client_end_session(GGZClient *client);

#endif /*_GGZ_CLIENT_H*/

