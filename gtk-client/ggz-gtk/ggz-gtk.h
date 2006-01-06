/*
 * File: ggz-gtk.h
 * Author: Jason Short
 * Project: GGZ GTK Client Library
 * $Id: client.h 7735 2006-01-06 08:27:50Z jdorje $
 *
 * This library provides interface functions so external programs can embed
 * GGZ via a GTK interface
 *
 * Copyright (C) 2000-2006 GGZ Development Team
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

#include <ggzcore.h>

void ggz_gtk_initialize(void (*connected)(GGZServer *server),
		       void (*launched)(void),
		       char *protocol_engine,
		       char *protocol_version);

void ggz_gtk_login_raise(void);

GtkWidget *ggz_gtk_create_main_area(GtkWidget *main_win);
