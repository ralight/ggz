/* 
 * File: main.h
 * Author: Rich Gade, Jason Short
 * Project: GGZCards Client
 * Date: 08/14/2000
 * Desc: Main loop and core logic
 * $Id: main.h 4787 2002-10-03 03:02:07Z jdorje $
 *
 * Copyright (C) 2000-2002 Brent Hendricks.
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

#include "client.h"

#define DBG_MAIN "main"

extern GtkWidget *dlg_main;

void statusbar_message(const char *msg);
void messagebar_message(const char *msg);
void menubar_text_message(const char *mark, const char *msg);
void menubar_cardlist_message(const char *mark, int *lengths,
			      card_t ** cardlist);

/* Call this function to listen to/ignore the server. */
void listen_for_server(bool listen);
