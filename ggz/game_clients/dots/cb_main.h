/*
 * File: cb_main.h
 * Author: Rich Gade
 * Project: GGZ Connect the Dots Client
 * Date: 08/14/2000
 * Desc: Callback functions for the main Gtk window
 *
 * Copyright (C) 2000, 2001 Brent Hendricks.
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

extern GtkWidget *dlg_pref;

void on_mnu_exit_activate(GtkMenuItem * menuitem, gpointer user_data);

void
on_mnu_preferences_activate(GtkMenuItem * menuitem, gpointer user_data);

gboolean
on_window_delete_event(GtkWidget * widget,
		       GdkEvent * event, gpointer user_data);

gboolean
on_board_expose_event(GtkWidget * widget,
		      GdkEventExpose * event, gpointer user_data);

gboolean
on_board_button_press_event(GtkWidget * widget,
			    GdkEventButton * event, gpointer user_data);

gboolean
on_p1b_expose_event(GtkWidget * widget,
		    GdkEventExpose * event, gpointer user_data);

gboolean
on_p2b_expose_event(GtkWidget * widget,
		    GdkEventExpose * event, gpointer user_data);
