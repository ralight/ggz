/*
 * File: msgbox.c
 * Author: Justin Zaun
 * Project: GGZ GTK Client
 *
 * This is the main program body for the GGZ client
 *
 * Copyright (C) 2000, 2001 Justin Zaun.
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
#include <config.h>
#include "msgbox.h"

MBReturn status;

void CloseTheApp (GtkWidget *window, gpointer data)
{
	gtk_main_quit();
}


void DialogOKClicked (GtkButton *button, gpointer data)
{
	status = MSGBOX_OK;
	gtk_widget_destroy (GTK_WIDGET (data));
}

void DialogCancelClicked (GtkButton *button, gpointer data)
{
	status = MSGBOX_CANCEL;
	gtk_widget_destroy (GTK_WIDGET (data));
}

void DialogYesClicked (GtkButton *button, gpointer data)
{
	status = MSGBOX_YES;
	gtk_widget_destroy (GTK_WIDGET (data));
}

void DialogNoClicked (GtkButton *button, gpointer data)
{
	status = MSGBOX_NO;
	gtk_widget_destroy (GTK_WIDGET (data));
}

GtkWidget *AddWidget (GtkWidget *widget, GtkWidget *packingbox)
{
	gtk_box_pack_start (GTK_BOX (packingbox), widget, TRUE, TRUE, 2);
	return widget;
}

MBReturn msgbox (gchar *textmessage, gchar *title, MBType type)
{
	GtkWidget *dialogwindow;
	GtkWidget *packingbox;
	GtkWidget *packingbox2;
	GtkWidget *dialogwidget;
	GtkWidget *btnok;
	GtkWidget *btncancel;
	GtkWidget *btnyes;
	GtkWidget *btnno;

	dialogwindow = gtk_window_new (GTK_WINDOW_DIALOG);
	gtk_window_set_title (GTK_WINDOW(dialogwindow), title);
	gtk_container_set_border_width (GTK_CONTAINER (dialogwindow), 10);
	gtk_window_set_position (GTK_WINDOW(dialogwindow), GTK_WIN_POS_CENTER);

	packingbox = gtk_vbox_new (TRUE, 5);
	gtk_container_add (GTK_CONTAINER(dialogwindow), packingbox);

	dialogwidget = AddWidget (gtk_label_new (textmessage), packingbox);
	dialogwidget = AddWidget (gtk_hseparator_new(), packingbox);
	packingbox2 = gtk_hbox_new (TRUE, 2);
	dialogwidget = AddWidget (packingbox2, packingbox);

	btnok = gtk_button_new_with_label ("OK");
	btncancel = gtk_button_new_with_label ("Cancel");
	btnyes = gtk_button_new_with_label ("Yes");
	btnno = gtk_button_new_with_label ("No");

	if (type == MSGBOX_OKCANCEL)
	{
		dialogwidget = AddWidget (btnok, packingbox2);
		dialogwidget = AddWidget (btncancel, packingbox2);
	} else if (type == MSGBOX_YESNO) {
		dialogwidget = AddWidget (btnyes, packingbox2);
		dialogwidget = AddWidget (btnno, packingbox2);
	} else if (type == MSGBOX_OKONLY) {
		dialogwidget = AddWidget (btnok, packingbox2);
	}

	gtk_signal_connect (GTK_OBJECT (btnok),
				"clicked",
				GTK_SIGNAL_FUNC (DialogOKClicked),
				dialogwindow);
	gtk_signal_connect (GTK_OBJECT (btncancel),
				"clicked",
				GTK_SIGNAL_FUNC (DialogCancelClicked),
				dialogwindow);
	gtk_signal_connect (GTK_OBJECT (btnyes),
				"clicked",
				GTK_SIGNAL_FUNC (DialogYesClicked),
				dialogwindow);
	gtk_signal_connect (GTK_OBJECT (btnno),
				"clicked",
				GTK_SIGNAL_FUNC (DialogNoClicked),
				dialogwindow);
	gtk_signal_connect (GTK_OBJECT (dialogwidget),
				"destroy",
				GTK_SIGNAL_FUNC (CloseTheApp),
				dialogwindow);

	gtk_window_set_modal (GTK_WINDOW (dialogwindow), TRUE);
	gtk_widget_show_all (dialogwindow);
	gtk_main();
	return status;
}
