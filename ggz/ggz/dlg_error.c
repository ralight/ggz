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
#include "support.h"

void msg_dlg(gchar *str, gint type)
{
	GtkWidget *window;
	GtkWidget *dialog_vbox1;
	GtkWidget *label1;
	GtkWidget *label2;
	GtkWidget *dialog_action_area1;
	GtkWidget *buttonBox;
	GtkWidget *labelBox;
	GtkWidget *okButton;
	gchar* message=str;

#ifdef DEBUG
	message = g_strdup_printf("[%d]: %s", getpid(), str);
#endif

	window = gtk_dialog_new();
	gtk_window_set_policy(GTK_WINDOW(window), FALSE, FALSE, FALSE);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_MOUSE);

	/* Label widgets */
	switch(type) {
		case 0:
			gtk_window_set_title(GTK_WINDOW(window), _("Notice"));
			label1 = gtk_label_new(_("Please note!"));
			break;
		case 1:
			gtk_window_set_title(GTK_WINDOW(window), _("Warning!"));
			label1 = gtk_label_new(_("Please note!"));
			break;
		case 2:
		default:
			gtk_window_set_title(GTK_WINDOW(window), _("Error!"));
			label1 = gtk_label_new(_("An error occurred!"));
			break;
	}
	
	label2 = gtk_label_new(message);

	gtk_widget_show(label1);
	gtk_widget_show(label2);


	/* Button widgets */
	okButton = gtk_button_new_with_label(_("OK"));

	if (type > 1)
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
	msg_dlg(message, 2);
	va_end(ap);
	g_free(message);
}


void warn_dlg(const gchar *fmt, ...)
{
	gchar* message;
	va_list ap;

	va_start(ap,fmt);	
	message = g_strdup_vprintf(fmt, ap);
	msg_dlg(message, 1);
	va_end(ap);
	g_free(message);
}


void note_dlg(const gchar *fmt, ...)
{
	gchar *message;
	va_list ap;

	va_start(ap, fmt);
	message = g_strdup_vprintf(fmt, ap);
	msg_dlg(message, 0);
	va_end(ap);
	g_free(message);
}
