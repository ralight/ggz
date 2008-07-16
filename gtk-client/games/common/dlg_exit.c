/*
 * File: dlg_exit.c
 * Author: Brent Hendricks
 * Project: GGZ Gtk Games (taken from NetSpades)
 * Date: 1/29/99
 * $Id: dlg_exit.c 6385 2004-11-16 05:21:05Z jdorje $
 *
 * This file contains functions for creating and handling the 
 * exit dialog box.
 *
 * Copyright (C) 1998-2002 Brent Hendricks.
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
#include "ggzintl.h"
#include "ggz_gtk.h"

static void exit_response(GtkDialog * dialog, gint response, gpointer data)
{
	gtk_widget_destroy(GTK_WIDGET(dialog));
	if (response == GTK_RESPONSE_YES) {
		gtk_main_quit();
	}
}

static GtkWidget *make_exit_dialog(int can_return, GtkWindow * parent)
{
	GtkWidget *dialog;
	GtkWidget *dialog_vbox1;
	GtkWidget *label;
	GtkWidget *labelBox;
	GtkWidget *image;
	char *text;

	if (can_return) {
		text = _("Are you sure you want to exit?");
	} else {
		text = _("Are you sure you want to exit?\n"
			 "You will not be able to rejoin.");
	}

	dialog = gtk_dialog_new_with_buttons(_("Really Exit?"), parent, 0,
					     GTK_STOCK_NO,
					     GTK_RESPONSE_NO,
					     GTK_STOCK_YES,
					     GTK_RESPONSE_YES, NULL);
	gtk_window_set_title(GTK_WINDOW(dialog), _("Really Exit?"));
	gtk_window_set_resizable(GTK_WINDOW(dialog), FALSE);
	gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_MOUSE);

	/* Label widgets */
	label = gtk_label_new(text);
	gtk_widget_show(label);

	image = gtk_image_new_from_stock(GTK_STOCK_DIALOG_QUESTION,
					 GTK_ICON_SIZE_DIALOG);
	gtk_widget_show(image);

	g_signal_connect(dialog, "response",
			 GTK_SIGNAL_FUNC(exit_response), NULL);


	/* And the layout... */
	labelBox = gtk_hbox_new(FALSE, 10);
	dialog_vbox1 = GTK_DIALOG(dialog)->vbox;

	gtk_container_set_border_width(GTK_CONTAINER(labelBox), 10);

	gtk_box_pack_start(GTK_BOX(labelBox), image, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(labelBox), label, FALSE, FALSE, 0);

	gtk_box_pack_start(GTK_BOX(dialog_vbox1), labelBox, TRUE, TRUE, 0);

	gtk_widget_show_all(dialog);

	return dialog;
}

void ggz_show_exit_dialog(int can_return)
{
	static GtkWidget *dialog = NULL;

	if (dialog) {
		gdk_window_show(dialog->window);
		gdk_window_raise(dialog->window);
	} else {
		dialog =
		    make_exit_dialog(can_return, ggz_game_main_window);

		g_signal_connect(dialog, "destroy",
				 GTK_SIGNAL_FUNC(gtk_widget_destroyed),
				 &dialog);

		gtk_widget_show(dialog);
	}
}
