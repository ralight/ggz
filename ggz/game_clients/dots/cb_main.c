/*
 * File: cb_main.c
 * Author: Rich Gade
 * Project: GGZ Connect the Dots Client
 * Date: 08/14/2000
 * Desc: Callback functions for the main Gtk window
 * $Id: cb_main.c 6284 2004-11-06 06:21:54Z jdorje $
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

#include <stdio.h>
#include <gtk/gtk.h>

#include "dlg_exit.h"

#include "cb_main.h"
#include "dlg_main.h"
#include "dlg_pref.h"
#include "support.h"
#include "main.h"
#include "game.h"
#include "cb_new.h"

GtkWidget *dlg_pref = NULL;


static void try_to_quit(void)
{

	/* In CHOOSE state, exiting means NO */
	if(game.state == DOTS_STATE_CHOOSE)
		handle_newgame(FALSE);

	/* Are you sure? / Cleanup */
	/* See also on_mnu_exit_activate */
	if (game.state == DOTS_STATE_CHOOSE ||
	    game.state == DOTS_STATE_INIT ||
	    game.state == DOTS_STATE_WAIT)
		gtk_main_quit();
	else
		ggz_show_exit_dialog(1, main_win);
}

void
on_mnu_exit_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	try_to_quit();
}


void
on_mnu_preferences_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	if(dlg_pref != NULL) {
		gdk_window_show(dlg_pref->window);
		gdk_window_raise(dlg_pref->window);
	} else {
		dlg_pref = create_dlg_pref();
		g_signal_connect(GTK_OBJECT(dlg_pref),
				   "destroy",
				   GTK_SIGNAL_FUNC(gtk_widget_destroyed),
				   &dlg_pref);
		gtk_widget_show(dlg_pref);
	}
}


gboolean
on_window_delete_event                 (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
	try_to_quit();
	return TRUE;
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

