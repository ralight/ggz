/*
 * File: gtk_dlg_disconnect.c
 * Author: Brent Hendricks
 * Project: NetSpades
 * Date: 1/29/99
 *
 * This file contains functions for creating and handling the 
 * disconnect dialog box.  
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


#include <config.h>

#include <gtk/gtk.h>

#include <gtk_dlg_disconnect.h>
#include <gtk_play.h>
#include <gtk_connect.h>
#include <gtk_io.h>
#include <client.h>

#include "ggzintl.h"

/* Global state of game variable */
extern playArea_t *playArea;

void DisconnectDialog(GtkWidget * widget, gpointer data)
{

	GtkWidget *window;
	GtkWidget *dialog_vbox1;
	GtkWidget *label;
	GtkWidget *dialog_action_area1;
	GtkWidget *buttonBox;
	GtkWidget *labelBox;
	GtkWidget *yesButton;
	GtkWidget *noButton;

	window = gtk_dialog_new();
	gtk_window_set_title(GTK_WINDOW(window), _("Disconnect from Server"));
	gtk_window_set_policy(GTK_WINDOW(window), FALSE, FALSE, FALSE);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_MOUSE);

	/* Label widgets */
	label = gtk_label_new(_("Really Disconnect?"));
	gtk_widget_show(label);


	/* Button widgets */
	noButton = gtk_button_new_with_label(_("No"));
	yesButton = gtk_button_new_with_label(_("Yes"));

	g_signal_connect(GTK_OBJECT(yesButton), "clicked",
			   GTK_SIGNAL_FUNC(Disconnect), NULL);
	g_signal_connect_swapped(GTK_OBJECT(yesButton), "clicked",
				  GTK_SIGNAL_FUNC(gtk_widget_destroy),
				  GTK_OBJECT(window));

	g_signal_connect_swapped(GTK_OBJECT(noButton), "clicked",
				  GTK_SIGNAL_FUNC(gtk_widget_destroy),
				  GTK_OBJECT(window));

	gtk_widget_show(noButton);
	gtk_widget_show(yesButton);


	/* And the layout... */
	labelBox = gtk_vbox_new(FALSE, 10);
	buttonBox = gtk_hbutton_box_new();
	dialog_vbox1 = GTK_DIALOG(window)->vbox;
	dialog_action_area1 = GTK_DIALOG(window)->action_area;

	gtk_container_set_border_width(GTK_CONTAINER(labelBox), 10);
	gtk_container_set_border_width(GTK_CONTAINER(dialog_action_area1),
				       10);

	gtk_box_pack_start(GTK_BOX(buttonBox), yesButton, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(buttonBox), noButton, FALSE, FALSE, 0);

	gtk_box_pack_start(GTK_BOX(labelBox), label, FALSE, FALSE, 0);

	gtk_box_pack_start(GTK_BOX(dialog_action_area1), buttonBox, TRUE,
			   TRUE, 0);
	gtk_box_pack_start(GTK_BOX(dialog_vbox1), labelBox, TRUE, TRUE, 0);

	GTK_WIDGET_SET_FLAGS(yesButton, GTK_CAN_DEFAULT);
	GTK_WIDGET_SET_FLAGS(noButton, GTK_CAN_DEFAULT);
	gtk_widget_grab_default(noButton);

	gtk_widget_show(labelBox);
	gtk_widget_show(buttonBox);
	gtk_widget_show(dialog_vbox1);
	gtk_widget_show(dialog_action_area1);

	gtk_widget_show(window);

}
