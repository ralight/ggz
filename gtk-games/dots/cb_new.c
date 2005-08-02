/*
 * File: cb_new.c
 * Author: Rich Gade
 * Project: GGZ Connect the Dots Client
 * Date: 08/14/2000
 * Desc: Callbacks for the "New Game?" Gtk dialog
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

#include "main.h"
#include "game.h"
#include "cb_new.h"
#include "dlg_new.h"
#include "support.h"

static gboolean allow_dlg_new_delete = FALSE;

gboolean
on_dlg_new_delete_event(GtkWidget * widget,
			GdkEvent * event, gpointer user_data)
{
	/* Do not allow this dialog to close until they choose */
	if (allow_dlg_new_delete)
		return FALSE;
	else
		return TRUE;
}


void on_new_btn_yes_clicked(GtkButton * button, gpointer user_data)
{
	handle_newgame(TRUE);
}


void on_new_btn_no_clicked(GtkButton * button, gpointer user_data)
{
	handle_newgame(FALSE);
	gtk_main_quit();
}

void handle_newgame(gboolean opt)
{
	if (opt == TRUE)
		handle_req_newgame();

	allow_dlg_new_delete = TRUE;
}
