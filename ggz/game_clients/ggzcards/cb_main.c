/* 
 * File: cb_main.c
 * Author: Rich Gade, Jason Short
 * Project: GGZCards Client
 * Date: 08/14/2000
 * Desc: Callbacks for GGZCards main Gtk window
 * $Id: cb_main.c 6293 2004-11-07 05:51:47Z jdorje $
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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

#include "dlg_exit.h"
#include "menus.h"

#include "cb_main.h"
#include "dlg_main.h"
#include "dlg_players.h"
#include "dlg_prefs.h"
#include "game.h"
#include "main.h"
#include "table.h"

void game_exit(void)
{
	/* Really, we shouldn't be checking game_started here.  We should
	   happily leave between games too.  But for now, this is fine. */
	if (game_started && preferences.confirm_exit) {
		ggz_show_exit_dialog(TRUE, dlg_main);
	} else {
		gtk_main_quit();
	}
}

void on_mnu_startgame_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	game_send_newgame();
}

void on_mnu_preferences_activate(GtkMenuItem * menuitem,
				 gpointer user_data)
{
	static GtkWidget *dlg_prefs = NULL;

	if (dlg_prefs != NULL) {
		gdk_window_show(dlg_prefs->window);
		gdk_window_raise(dlg_prefs->window);
	} else {
		dlg_prefs = create_dlg_prefs();
		g_signal_connect(dlg_prefs, "destroy",
				 GTK_SIGNAL_FUNC(gtk_widget_destroyed),
				 &dlg_prefs);
		gtk_widget_show(dlg_prefs);
	}
}

void on_mnu_forceredraw_activate(GtkMenuItem * menuitem,
				 gpointer user_data)
{
	table_redraw();
}

gboolean on_dlg_main_delete_event(GtkWidget * widget, GdkEvent * event,
				  gpointer user_data)
{
	game_exit();
	return TRUE;
}


gboolean on_fixed1_button_press_event(GtkWidget * widget,
				      GdkEventButton * event,
				      gpointer user_data)
{
	if (event->button == 1) {
		/* Left-clicking selects cards */
		return table_handle_cardclick_event(event);
	} else if (event->button == 3) {
		/* Right-clicking may pop up a menu */
		return table_handle_menuclick_event(event);
	}

	return FALSE;
}


gboolean on_table_expose_event(GtkWidget * widget, GdkEventExpose * event,
			       gpointer user_data)
{
	table_handle_expose_event(event);

	return TRUE;
}


void on_fixed1_redraw_event(GtkWidget * widget, GtkStyle * previous_style,
			    gpointer user_data)
{
	table_redraw();
}
