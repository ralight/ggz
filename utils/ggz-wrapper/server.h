/*
 * File: server.h
 * Author: Brent Hendricks
 * Project: GGZ Text Client 
 * Date: 9/26/00
 * $Id: server.h 9165 2007-07-04 13:58:17Z josef $
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

#include "ggzcore.h"

/* FIXME: why are game channels marked as deprecated at all? */
#define GGZ_ENABLE_DEPRECATED 1

int server_init(char *host, int port, GGZLoginType type, char *login,
		char *password);

void server_logout(void);
void server_disconnect(void);

void server_destroy(void);

void server_workinprogress(int command, int progress);
void server_progresswait(void);

/* Pointer to the current server */
extern GGZServer *server;
