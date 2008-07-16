/* 
 * File: cb_main.h
 * Author: Rich Gade, Jason Short
 * Project: GGZCards Client
 * Date: 08/14/2000
 * Desc: Callbacks for GGZCards main Gtk window
 * $Id: cb_main.h 9053 2007-04-17 03:16:36Z jdorje $
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

#ifndef __CB_MAIN_H__
#define __CB_MAIN_H__

#include <gtk/gtk.h>

void on_mnu_startgame_activate(GtkMenuItem * menuitem, gpointer user_data);
void on_mnu_scores_activate(GtkMenuItem *menuitem, gpointer user_data);
void on_mnu_preferences_activate(GtkMenuItem * menuitem,
				 gpointer user_data);
void on_mnu_revealhand_activate(GtkMenuItem * menuitem,
				gpointer user_data);

void on_mnu_forceredraw_activate(GtkMenuItem * menuitem,
				 gpointer user_data);
void on_mnu_exit_activate(GtkMenuItem * menuitem, gpointer user_data);

gboolean on_dlg_main_delete_event(GtkWidget * widget, GdkEvent * event,
				  gpointer user_data);
gboolean on_fixed1_button_press_event(GtkWidget * widget,
				      GdkEventButton * event,
				      gpointer user_data);

/* Call this when there's an expose event for the table widget. */
gboolean on_table_expose_event(GtkWidget * widget, GdkEventExpose * event,
			       gpointer user_data);

void on_fixed1_redraw_event(GtkWidget * widget, GtkStyle * previous_style,
			    gpointer user_data);

#endif /* __CB_MAIN_H__ */
