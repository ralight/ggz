/*
 * File: cb_opt.c
 * Author: Rich Gade
 * Project: GGZ Connect the Dots Client
 * Date: 08/14/2000
 * Desc: Callbacks for the game options Gtk dialog
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

#include <gtk/gtk.h>
#include <stdio.h>

#include "cb_opt.h"
#include "dlg_opt.h"
#include "support.h"
#include "main.h"
#include "game.h"


static gboolean allow_dlg_opt_delete = FALSE;

void
on_opt_btn_ok_clicked                  (GtkButton       *button,
                                        gpointer         user_data)
{
	GtkWidget *sb_x, *sb_y;

	sb_x = g_object_get_data(G_OBJECT(opt_dialog), "opt_spin_x");
	sb_y = g_object_get_data(G_OBJECT(opt_dialog), "opt_spin_y");

	/* Get the number of squares wide and high */
	board_width = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(sb_x));
	board_height = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(sb_y));

	/* Convert from squares to dots */
	board_width++;
	board_height++;

	allow_dlg_opt_delete = TRUE;

	if(send_options() < 0)
		gtk_main_quit();
}


gboolean
on_dlg_opt_delete_event                (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
	/* Do not allow this dialog to close before we've sent the options */
	if(allow_dlg_opt_delete)
		return FALSE;
	else
		return TRUE;
}

