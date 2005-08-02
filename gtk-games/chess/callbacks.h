/*
 * File: callbacks.h
 * Author: Ismael Orenstein
 * Project: GGZ Chess game module
 * Desc: GTK Callback functions
 * $Id: callbacks.h 6293 2004-11-07 05:51:47Z jdorje $
 *
 * Copyright (C) 2000-2002 Ismael Orenstein.
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
#include "board.h"


gboolean
ExitDialog(GtkWidget * widget, GdkEvent * event, gpointer user_data);

void on_exit_activate(GtkMenuItem * menuitem, gpointer user_data);

gboolean
on_board_configure_event(GtkWidget * widget,
			 GdkEventConfigure * event, gpointer user_data);

gboolean
on_board_expose_event(GtkWidget * widget,
		      GdkEventExpose * event, gpointer user_data);

gboolean
on_board_button_press_event(GtkWidget * widget,
			    GdkEventButton * event, gpointer user_data);

void
on_board_drag_begin(GtkWidget * widget,
		    GdkDragContext * drag_context, gpointer user_data);

gboolean
on_board_drag_motion(GtkWidget * widget,
		     GdkDragContext * drag_context,
		     gint x, gint y, guint time, gpointer user_data);

gboolean
on_board_drag_drop(GtkWidget * widget,
		   GdkDragContext * drag_context,
		   gint x, gint y, guint time, gpointer user_data);

void
on_auto_call_flag_activate(GtkMenuItem * menuitem, gpointer user_data);
