/*
 * File: dlg_error.c
 * Author: Brent Hendricks
 * Project: GGZ Client
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


#include <gtk/gtk.h>
#include <unistd.h>

#include "dlg_error.h"
#include "connect.h"

void msg_dlg(gchar *str, gboolean quit)
{
	GtkWidget *window;
	GtkWidget *dialog_vbox1;
	GtkWidget *label1;
	GtkWidget *label2;
	GtkWidget *dialog_action_area1;
	GtkWidget *buttonBox;
	GtkWidget *labelBox;
	GtkWidget *okButton;
	gchar* message;

	message = g_strdup_printf("[%d]: %s", getpid(), str);

	window = gtk_dialog_new();
	if (quit) {
		gtk_window_set_title(GTK_WINDOW(window), "Error!");
	} else {
		gtk_window_set_title(GTK_WINDOW(window), "Warning!");
	}
	gtk_window_set_policy(GTK_WINDOW(window), FALSE, FALSE, FALSE);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_MOUSE);


	/* Label widgets */
	if (quit)
		label1 = gtk_label_new("An error occurred!");
	else
		label1 = gtk_label_new("Please note!");
	
	label2 = gtk_label_new(message);

	gtk_widget_show(label1);
	gtk_widget_show(label2);


	/* Button widgets */
	okButton = gtk_button_new_with_label("OK");

	if (quit)
		gtk_signal_connect(GTK_OBJECT(okButton), "clicked",
				   GTK_SIGNAL_FUNC(disconnect), NULL);

	gtk_signal_connect_object(GTK_OBJECT(okButton), "clicked",
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


void err_dlg(const gchar *fmt, ...)
{
	gchar* message;
	va_list ap;

	va_start(ap,fmt);	
	message = g_strdup_vprintf(fmt, ap);
	msg_dlg(message, TRUE);
	va_end(ap);
	g_free(message);
}


void warn_dlg(const gchar *fmt, ...)
{
	gchar* message;
	va_list ap;

	va_start(ap,fmt);	
	message = g_strdup_vprintf(fmt, ap);
	msg_dlg(message, FALSE);
	va_end(ap);
	g_free(message);
}
