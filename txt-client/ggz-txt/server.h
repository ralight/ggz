/*
 * ggz-txt - Command-line core client for the GGZ Gaming Zone.
 * This application is part of the ggz-txt-client package.
 *
 * server.h: Functions for handling server events.
 *
 * Copyright (C) 2000 Brent Hendricks
 * Copyright (C) 2001-2008 GGZ Gaming Zone Development Team
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

#include <ggzcore.h>

#define COMMAND_CONNECT 0
#define COMMAND_LIST 1
#define COMMAND_JOIN 2

void server_init(char *host, int port, GGZLoginType type, char* login, char* password, int usetls, char* email);

void server_logout(void);
void server_disconnect(void);

void server_destroy(void);

void server_workinprogress(int command, int progress);
void server_progresswait(void);

/* Pointer to the current server */
extern GGZServer *server;

extern int players_on_server;
extern int players_in_room;

