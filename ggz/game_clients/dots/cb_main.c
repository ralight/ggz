/*
 * File: cb_main.c
 * Author: Rich Gade
 * Project: GGZ Connect the Dots Client
 * Date: 08/14/2000
 * Desc: Callback functions for the main Gtk window
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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdio.h>
#include <gtk/gtk.h>

#include "cb_main.h"
#include "dlg_main.h"
#include "dlg_about.h"
#include "support.h"
#include "main.h"
#include "game.h"
#include "cb_new.h"


void
on_mnu_exit_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	/* In CHOOSE state, exiting means NO */
	if(game.state == DOTS_STATE_CHOOSE)
		handle_newgame(FALSE);

	/* Are you sure? / Cleanup */
	/* See also on_window_delete_event */
	gtk_main_quit();
}


void
on_mnu_preferences_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_mnu_about_activate                  (GtkMenuItem     *menuitem,
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

gboolean
on_window_delete_event                 (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
	/* In CHOOSE state, exiting means NO */
	if(game.state == DOTS_STATE_CHOOSE)
		handle_newgame(FALSE);

	/* Are you sure? / Cleanup */
	/* See also on_mnu_exit_activate */
	gtk_main_quit();
	return FALSE;
}

gboolean
on_board_expose_event                  (GtkWidget       *widget,
                                        GdkEventExpose  *event,
                                        gpointer         user_data)
{
	board_handle_expose_event(widget, event);
	return FALSE;
}


gboolean
on_board_button_press_event            (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
	if(event->button == 1)
		board_handle_click(widget, event);

	return TRUE;
}


gboolean
on_p1b_expose_event                    (GtkWidget       *widget,
                                        GdkEventExpose  *event,
                                        gpointer         user_data)
{
	board_handle_pxb_expose();
	return FALSE;
}


gboolean
on_p2b_expose_event                    (GtkWidget       *widget,
                                        GdkEventExpose  *event,
                                        gpointer         user_data)
{
	board_handle_pxb_expose();
	return FALSE;
}

