/*
 * File: dlg_exit.c
 * Author: Brent Hendricks
 * Project: GGZ Gtk Games (taken from NetSpades)
 * Date: 1/29/99
 * $Id: dlg_exit.c 6225 2004-10-28 05:48:01Z jdorje $
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

static GtkWidget *make_exit_dialog(int can_return, GtkWidget *parent_window)
{
	GtkWidget *window;
	GtkWidget *dialog_vbox1;
	GtkWidget *label;
	GtkWidget *dialog_action_area1;
	GtkWidget *buttonBox;
	GtkWidget *labelBox;
	GtkWidget *exitButton;
	GtkWidget *cancelButton;
	GtkWidget *image;
	char *text;
	
	if (can_return) {
		text = _("Are you sure you want to exit?");
	} else {
		text = _("Are you sure you want to exit?\n"
	               "You will not be able to rejoin.");
	}

	window = gtk_dialog_new();
	if (parent_window)
		gtk_window_set_transient_for(GTK_WINDOW(window),
					     GTK_WINDOW(parent_window));
	gtk_window_set_title(GTK_WINDOW(window), _("Really Exit?"));
	gtk_window_set_policy(GTK_WINDOW(window), FALSE, FALSE, FALSE);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_MOUSE);

	/* Label widgets */
	label = gtk_label_new(text);
	gtk_widget_show(label);

	image = gtk_image_new_from_stock(GTK_STOCK_DIALOG_QUESTION,
					 GTK_ICON_SIZE_DIALOG);
	gtk_widget_show(image);

	/* Button widgets */
	exitButton = gtk_button_new_from_stock(GTK_STOCK_QUIT);
	cancelButton = gtk_button_new_from_stock(GTK_STOCK_CANCEL);

	gtk_signal_connect(GTK_OBJECT(exitButton), "clicked",
			   GTK_SIGNAL_FUNC(gtk_main_quit), NULL);
	gtk_signal_connect_object(GTK_OBJECT(exitButton), "clicked",
				  GTK_SIGNAL_FUNC(gtk_widget_destroy),
				  GTK_OBJECT(window));

	gtk_signal_connect_object(GTK_OBJECT(cancelButton), "clicked",
				  GTK_SIGNAL_FUNC(gtk_widget_destroy),
				  GTK_OBJECT(window));

	gtk_widget_show(exitButton);
	gtk_widget_show(cancelButton);


	/* And the layout... */
	labelBox = gtk_hbox_new(FALSE, 10);
	buttonBox = gtk_hbutton_box_new();
	dialog_vbox1 = GTK_DIALOG(window)->vbox;
	dialog_action_area1 = GTK_DIALOG(window)->action_area;

	gtk_container_set_border_width(GTK_CONTAINER(labelBox), 10);
	gtk_container_set_border_width(GTK_CONTAINER(dialog_action_area1),
				       10);

	gtk_box_pack_start(GTK_BOX(buttonBox), exitButton, TRUE, TRUE,
			   0);
	gtk_box_pack_start(GTK_BOX(buttonBox), cancelButton, TRUE, TRUE,
			   0);

	gtk_box_pack_start(GTK_BOX(labelBox), image, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(labelBox), label, FALSE, FALSE, 0);

	gtk_box_pack_start(GTK_BOX(dialog_action_area1), buttonBox, TRUE,
			   TRUE, 0);
	gtk_box_pack_start(GTK_BOX(dialog_vbox1), labelBox, TRUE, TRUE, 0);

	GTK_WIDGET_SET_FLAGS(exitButton, GTK_CAN_DEFAULT);
	GTK_WIDGET_SET_FLAGS(cancelButton, GTK_CAN_DEFAULT);
	gtk_widget_grab_default(exitButton);

	gtk_widget_show(labelBox);
	gtk_widget_show(buttonBox);
	gtk_widget_show(dialog_vbox1);
	gtk_widget_show(dialog_action_area1);
	
        return window;
}

void ggz_show_exit_dialog(int can_return, GtkWidget *parent_window)
{
	static GtkWidget *dialog = NULL;
	
	if (dialog) {
		gdk_window_show(dialog->window);
		gdk_window_raise(dialog->window);
	} else {
        	dialog = make_exit_dialog(can_return, parent_window);
		
		gtk_signal_connect(GTK_OBJECT(dialog),
		                   "destroy",
		                   GTK_SIGNAL_FUNC(gtk_widget_destroyed),
		                   &dialog);
		
		gtk_widget_show(dialog);
	}
}

