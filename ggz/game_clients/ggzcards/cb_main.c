/* 
 * File: cb_main.c
 * Author: Rich Gade, Jason Short
 * Project: GGZCards Client
 * Date: 08/14/2000
 * Desc: Callbacks for GGZCards main Gtk window
 * $Id: cb_main.c 3698 2002-03-28 00:11:43Z jdorje $
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

#include "cb_main.h"
#include "dlg_main.h"
#include "dlg_about.h"
#include "dlg_players.h"
#include "dlg_prefs.h"
#include "game.h"
#include "main.h"
#include "table.h"

GtkWidget *player_dialog = NULL;

static void try_to_exit_game(void)
{
	/* Really, we shouldn't be checking game_started here.  We
	   should happily leave between games too.  But for now,
	   this is fine. */
	if (game_started && preferences.confirm_exit) {
		ggz_show_exit_dialog(TRUE);
	} else {
		gtk_main_quit();
	}
}

void on_mnu_startgame_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	game_send_newgame();
}

void on_mnu_sync_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	game_request_sync();
}

void on_mnu_preferences_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	static GtkWidget *dlg_prefs = NULL;

	if (dlg_prefs != NULL) {
		gdk_window_show(dlg_prefs->window);
		gdk_window_raise(dlg_prefs->window);
	} else {
		dlg_prefs = create_dlg_prefs();
		(void) gtk_signal_connect(GTK_OBJECT(dlg_prefs),
				   "destroy",
				   GTK_SIGNAL_FUNC(gtk_widget_destroyed),
				   &dlg_prefs);
		gtk_widget_show(dlg_prefs);
	}
}

void on_mnu_playerlist_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	if (player_dialog != NULL) {
		gdk_window_show(player_dialog->window);
		gdk_window_raise(player_dialog->window);
	} else {
		player_dialog = create_dlg_players();
		(void) gtk_signal_connect(GTK_OBJECT(player_dialog),
				   "destroy",
				   GTK_SIGNAL_FUNC(gtk_widget_destroyed),
				   &player_dialog);
		gtk_widget_show(player_dialog);
	}
}

void on_mnu_forceredraw_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	table_redraw();
}

void on_mnu_exit_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	try_to_exit_game();
}

void on_mnu_about_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	static GtkWidget *dlg_about = NULL;

	if (dlg_about != NULL) {
		gdk_window_show(dlg_about->window);
		gdk_window_raise(dlg_about->window);
	} else {
		dlg_about = create_dlg_about();
		(void) gtk_signal_connect(GTK_OBJECT(dlg_about),
				   "destroy",
				   GTK_SIGNAL_FUNC(gtk_widget_destroyed),
				   &dlg_about);
		gtk_widget_show(dlg_about);
	}
}


gboolean on_dlg_main_delete_event(GtkWidget * widget, GdkEvent * event,
				  gpointer user_data)
{
	try_to_exit_game();
	return TRUE;
}


gboolean on_fixed1_button_press_event(GtkWidget * widget,
				      GdkEventButton * event,
				      gpointer user_data)
{
	if (event->button == 1)
		table_handle_click_event(event);

	return FALSE;
}


gboolean on_table_expose_event(GtkWidget *widget, GdkEventExpose *event,
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
