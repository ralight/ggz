/*
 * File: gtk_dlg_error.c
 * Author: Brent Hendricks
 * Project: NetSpades
 * Date: 3/24/99
 *
 * This file contains functions for creating and handling the error box
 *
 * Copyright (C) 1998 Brent Hendricks.
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
#  include <config.h>	/* Site-specific config */
#endif

#include <gtk_connect.h>
#include <display.h>

#include <gtk/gtk.h>

#include "ggzintl.h"
#include "gtk_dlg_error.h"

void DisplayMessage(char *message, gboolean quit)
{

	GtkWidget *window;
	GtkWidget *dialog_vbox1;
	GtkWidget *label1;
	GtkWidget *label2;
	GtkWidget *dialog_action_area1;
	GtkWidget *buttonBox;
	GtkWidget *labelBox;
	GtkWidget *okButton;

	/*char *buf; */

	window = gtk_dialog_new();
	if (quit) {
		gtk_window_set_title(GTK_WINDOW(window), _("Error!"));
	} else {
		gtk_window_set_title(GTK_WINDOW(window), _("Warning!"));
	}
	gtk_window_set_policy(GTK_WINDOW(window), FALSE, FALSE, FALSE);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_MOUSE);


	/* Label widgets */
	if (quit) {
		label1 = gtk_label_new(_("An error occurred!"));
	} else {
		label1 = gtk_label_new(_("Please note!"));
	}
	label2 = gtk_label_new(message);

	gtk_widget_show(label1);
	gtk_widget_show(label2);


	/* Button widgets */
	okButton = gtk_button_new_with_label(_("OK"));

	if (quit) {
		g_signal_connect(GTK_OBJECT(okButton), "clicked",
				 GTK_SIGNAL_FUNC(Disconnect), NULL);
	}
	g_signal_connect_swapped(GTK_OBJECT(okButton), "clicked",
				 GTK_SIGNAL_FUNC(gtk_widget_destroy),
				 GTK_OBJECT(window));
	gtk_widget_show(okButton);


	/* And the layout... */
	labelBox = gtk_vbox_new(FALSE, 0);
	buttonBox = gtk_hbutton_box_new();
	dialog_vbox1 = GTK_DIALOG(window)->vbox;
	dialog_action_area1 = GTK_DIALOG(window)->action_area;

	gtk_container_set_border_width(GTK_CONTAINER(labelBox), 10);
	gtk_container_set_border_width(GTK_CONTAINER(dialog_action_area1),
				       10);

	gtk_box_pack_start(GTK_BOX(buttonBox), okButton, FALSE, FALSE, 0);

	gtk_box_pack_start(GTK_BOX(labelBox), label1, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(labelBox), label2, FALSE, FALSE, 0);

	gtk_box_pack_start(GTK_BOX(dialog_action_area1), buttonBox, TRUE,
			   TRUE, 0);
	gtk_box_pack_start(GTK_BOX(dialog_vbox1), labelBox, FALSE, FALSE,
			   0);

	GTK_WIDGET_SET_FLAGS(okButton, GTK_CAN_DEFAULT);
	gtk_widget_grab_default(okButton);

	gtk_widget_show(labelBox);
	gtk_widget_show(buttonBox);
	gtk_widget_show(dialog_vbox1);
	gtk_widget_show(dialog_action_area1);

	gtk_widget_show(window);

}


void DisplayError(char *message)
{
	DisplayMessage(message, TRUE);
}

void DisplayWarning(char *message)
{
	DisplayMessage(message, FALSE);
}
