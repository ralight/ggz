/*
 * File: cb_main.c
 * Author: Rich Gade
 * Project: GGZ La Pocha Client
 * Date: 08/14/2000
 * Desc: Callbacks for La Pocha main Gtk window
 * $Id: cb_main.c 5165 2002-11-03 07:54:39Z jdorje $
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

#include <assert.h>
#include <gtk/gtk.h>

#include "dlg_exit.h"

#include "cb_main.h"
#include "dlg_main.h"
#include "game.h"
#include "main.h"
#include "support.h"
#include "table.h"


static void try_to_quit(void)
{
	if (game.state == LP_STATE_INIT ||
	    game.state == LP_STATE_DONE)
		gtk_main_quit();
	else
		ggz_show_exit_dialog(1, dlg_main);
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
	assert(0);
}


gboolean
on_dlg_main_delete_event               (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
	try_to_quit();

	return TRUE;
}


gboolean
on_fixed1_button_press_event           (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
	if(event->button == 1)
		table_handle_click_event(event);

	return FALSE;
}


gboolean
on_fixed1_expose_event                 (GtkWidget       *widget,
                                        GdkEventExpose  *event,
                                        gpointer         user_data)
{
	table_handle_expose_event(event);

	return TRUE;
}


void
on_fixed1_style_set		       (GtkWidget	*widget,
					GtkStyle	*previous_style,
					gpointer	 user_data)
{
	table_style_change();
}
