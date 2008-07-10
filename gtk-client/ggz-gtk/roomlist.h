/*
 * File: roomlist.h
 * Author: GGZ Dev Team
 * Project: GGZ GTK Client
 * Date: 11/05/2004
 * $Id: roomlist.h 10274 2008-07-10 21:38:34Z jdorje $
 * 
 * List of rooms in the server
 * 
 * Copyright (C) 2000-2004 GGZ Development Team
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

#include <gtk/gtk.h>

#include "support.h"

void INTERNAL sensitize_room_list(gboolean sensitive);
void INTERNAL clear_room_list(void);
void INTERNAL select_room(GGZRoom *room);

void INTERNAL update_one_room(const GGZRoom *room);
void INTERNAL update_room_list(void);

GtkWidget INTERNAL *create_room_list(GtkWidget *window);
