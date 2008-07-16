/*
 * File: callbacks.c
 * Author: Ismael Orenstein
 * Project: GGZ Combat game module
 * Desc: Combat client GTK callback functions
 * $Id: callbacks.h 10185 2008-07-08 03:11:00Z jdorje $
 *
 * Copyright (C) 2002 Ismael Orenstein.
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
on_main_window_configure_event(GtkWidget * widget,
			       GdkEventConfigure * event,
			       gpointer user_data);

void
on_main_window_draw(GtkWidget * widget,
		    GdkRectangle * area, gpointer user_data);

gboolean
on_main_window_delete_event(GtkWidget * widget,
			    GdkEvent * event, gpointer user_data);

gboolean
on_mainarea_expose_event(GtkWidget * widget,
			 GdkEventExpose * event, gpointer user_data);

gboolean
main_window_exit(GtkWidget * widget, GdkEvent * event, gpointer user_data);


gboolean
on_mainarea_configure_event(GtkWidget * widget,
			    GdkEventConfigure * event, gpointer user_data);

gboolean
on_mainarea_button_press_event(GtkWidget * widget,
			       GdkEventButton * event, gpointer user_data);

void callback_widget_set_enabled(char *name, gboolean mode);

void on_send_setup_clicked(GtkButton * button, gpointer user_data);

void on_save_map_menu_activate(void);

void on_show_game_options_activate(void);

void change_show_enemy(GtkWidget * button, gpointer user_data);

void on_remember_enemy_units_toggled(void);
