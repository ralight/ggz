/*
 * File: cb_pref.h
 * Author: Rich Gade
 * Project: GGZ Connect the Dots Client
 * Date: 01/22/2001
 * Desc: Callback functions for the "Preferences" Gtk dialog
 *
 * Copyright (C) 2001 Brent Hendricks.
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

gboolean
on_bg_colorspot_button_press_event(GtkWidget * widget,
				   GdkEventButton * event,
				   gpointer user_data);

gboolean
on_p2_colorspot_button_press_event(GtkWidget * widget,
				   GdkEventButton * event,
				   gpointer user_data);

gboolean
on_p1_colorspot_button_press_event(GtkWidget * widget,
				   GdkEventButton * event,
				   gpointer user_data);

gboolean
on_fg_colorspot_button_press_event(GtkWidget * widget,
				   GdkEventButton * event,
				   gpointer user_data);

void on_pref_button_ok_clicked(GtkButton * button, gpointer user_data);

void on_pref_button_apply_clicked(GtkButton * button, gpointer user_data);

void on_pref_button_cancel_clicked(GtkButton * button, gpointer user_data);

gboolean
on_dlg_pref_expose_event(GtkWidget * widget,
			 GdkEventExpose * event, gpointer user_data);

gboolean
on_dlg_pref_delete_event(GtkWidget * widget,
			 GdkEvent * event, gpointer user_data);
