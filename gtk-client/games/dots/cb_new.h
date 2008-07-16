/*
 * File: cb_new.h
 * Author: Rich Gade
 * Project: GGZ Connect the Dots Client
 * Date: 08/14/2000
 * Desc: Callback functions for the "New Game" Gtk dialog
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

#include <gtk/gtk.h>


gboolean
on_dlg_new_delete_event(GtkWidget * widget,
			GdkEvent * event, gpointer user_data);

void on_new_btn_yes_clicked(GtkButton * button, gpointer user_data);

void on_new_btn_no_clicked(GtkButton * button, gpointer user_data);

void handle_newgame(gboolean);
