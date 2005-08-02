/*
 * File: main_cb.h
 * Author: Rich Gade
 * Project: GGZ Chinese Checkers Client
 * Date: 01/01/2001
 * Desc: Callbacks for the main dialog window
 *
 * Copyright (C) 2001 Richard Gade.
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

extern GtkWidget *dlg_prefs;


gboolean
on_dlg_main_delete_event(GtkWidget * widget,
			 GdkEvent * event, gpointer user_data);

gboolean
on_draw_area_expose_event(GtkWidget * widget,
			  GdkEventExpose * event, gpointer user_data);
gboolean on_draw_area_configure_event(GtkWidget * widget,
				      GdkEventExpose * event,
				      gpointer user_data);

gboolean
on_draw_area_button_press_event(GtkWidget * widget,
				GdkEventButton * event,
				gpointer user_data);

void
on_preferences_menu_activate(GtkMenuItem * menuitem, gpointer user_data);
