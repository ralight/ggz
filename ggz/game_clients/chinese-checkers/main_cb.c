/*
 * File: main_cb.c
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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

#include "main_cb.h"
#include "main_dlg.h"
#include "prefs_dlg.h"
#include "support.h"
#include "display.h"
#include "about_dlg.h"
#include "game.h"

GtkWidget *dlg_prefs = NULL;


gboolean
on_dlg_main_delete_event               (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
	gtk_main_quit();
	return FALSE;
}


void
on_exit_menu_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	gtk_main_quit();
}


void
on_preferences_menu_activate	       (GtkMenuItem	*menuitem,
					gpointer	 user_data)
{
	GtkWidget *entry, *toggle;

	if(dlg_prefs != NULL) {
		gdk_window_show(dlg_prefs->window);
		gdk_window_raise(dlg_prefs->window);
	} else {
		dlg_prefs = create_dlg_prefs();
		gtk_signal_connect(GTK_OBJECT(dlg_prefs),
				   "destroy",
				   GTK_SIGNAL_FUNC(gtk_widget_destroyed),
				   &dlg_prefs);
		entry = gtk_object_get_data(GTK_OBJECT(dlg_prefs), "dir_entry");
		toggle = gtk_object_get_data(GTK_OBJECT(dlg_prefs), "check_beep");
		gtk_entry_set_text(GTK_ENTRY(entry), game.pixmap_dir);
		if(game.beep == 1)
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(toggle),
						     TRUE);
		else
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(toggle),
						     FALSE);
		gtk_widget_show(dlg_prefs);
	}
}


gboolean
on_draw_area_expose_event              (GtkWidget       *widget,
                                        GdkEventExpose  *event,
                                        gpointer         user_data)
{
	display_handle_expose_event(event);
	return FALSE;
}


gboolean
on_draw_area_button_press_event        (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
	if(event->button == 1)
		display_handle_click_event(event);
	return TRUE;
}


void
on_about_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	static GtkWidget *dlg_about = NULL;

	if(dlg_about != NULL) {
		gdk_window_show(dlg_about->window);
		gdk_window_raise(dlg_about->window);
	} else {
		dlg_about = create_dlg_about();
		gtk_signal_connect(GTK_OBJECT(dlg_about),
				   "destroy",
				   GTK_SIGNAL_FUNC(gtk_widget_destroyed),
				   &dlg_about);
		gtk_widget_show(dlg_about);
	}
}

