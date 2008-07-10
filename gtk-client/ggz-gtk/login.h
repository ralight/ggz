/*
 * File: client.c
 * Author: Justin Zaun
 * Project: GGZ GTK Client
 * $Id: login.h 10275 2008-07-10 22:26:54Z jdorje $
 *
 * This is the main program body for the GGZ client
 *
 * Copyright (C) 2000 Justin Zaun.
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

#include "server.h"
#include "support.h"

void INTERNAL login_connect_failed(void);
void INTERNAL login_failed(const GGZErrorEventData * error);
void INTERNAL login_destroy(void);
void INTERNAL login_goto_server(const gchar * server_url);

void login_set_entries(Server server);
void INTERNAL login_set_sensitive(gboolean sensitive);
void INTERNAL login_set_option_log(const char *option_log);

GtkWidget INTERNAL *create_dlg_login(const char *default_profile);
