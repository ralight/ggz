/*
 * File: msgbox.c
 * Author: Justin Zaun
 * Project: GGZ GTK Client
 * $Id: msgbox.c 5196 2002-11-04 00:07:45Z jdorje $
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

#ifdef HAVE_CONFIG_H
#  include <config.h>			/* Site-specific config */
#endif

#include <gtk/gtk.h>

#include "msgbox.h"
#include "msg_help.xpm"
#include "msg_info.xpm"
#include "msg_stop.xpm"

static MBReturn mb_status;

static void CloseTheApp (GtkWidget *window, gpointer data)
{
	gtk_main_quit();
}


static void DialogOKClicked (GtkButton *button, gpointer data)
{
	mb_status = MSGBOX_OK;
	gtk_widget_destroy (GTK_WIDGET (data));
}

static void DialogCancelClicked (GtkButton *button, gpointer data)
{
	mb_status = MSGBOX_CANCEL;
	gtk_widget_destroy (GTK_WIDGET (data));
}

static void DialogYesClicked (GtkButton *button, gpointer data)
{
	mb_status = MSGBOX_YES;
	gtk_widget_destroy (GTK_WIDGET (data));
}

static void DialogNoClicked (GtkButton *button, gpointer data)
{
	mb_status = MSGBOX_NO;
	gtk_widget_destroy (GTK_WIDGET (data));
}

static GtkWidget *AddWidget (GtkWidget *widget, GtkWidget *packingbox)
{
	gtk_box_pack_start (GTK_BOX (packingbox), widget, TRUE, TRUE, 2);
	return widget;
}


extern GtkWidget *win_main;

MBReturn msgbox (gchar *textmessage, gchar *title, MBType type, MBIcon itype, MBModal modal)
{
	GtkWidget *dialogwindow;
	GtkWidget *vbox;
	GtkWidget *buttonbox;
	GtkWidget *hbox;
	GtkWidget *icon = NULL;
	GdkColormap *colormap;
	GdkPixmap *pixmap = NULL;
	GdkPixmap *mask;
	GtkWidget *dialogwidget;
	GtkWidget *btnok;
	GtkWidget *btncancel;
	GtkWidget *btnyes;
	GtkWidget *btnno;

	dialogwindow = gtk_dialog_new();
	gtk_window_set_title (GTK_WINDOW(dialogwindow), title);
	gtk_window_set_transient_for(GTK_WINDOW(dialogwindow),
				     GTK_WINDOW(win_main));
	gtk_container_set_border_width (GTK_CONTAINER (dialogwindow), 10);
	gtk_window_set_position (GTK_WINDOW(dialogwindow), GTK_WIN_POS_CENTER);

	vbox = GTK_DIALOG(dialogwindow)->vbox;

	hbox = gtk_hbox_new (FALSE, 2);
	dialogwidget = AddWidget (hbox, vbox);

	if (itype == MSGBOX_STOP)
	{
		colormap = gtk_widget_get_colormap (dialogwidget);
		pixmap = gdk_pixmap_colormap_create_from_xpm_d (NULL, colormap, &mask, NULL, stop);
		if (pixmap == NULL)
			g_error ("Couldn't create replacement pixmap.");
		icon = gtk_pixmap_new (pixmap, mask);
	}
	if (itype == MSGBOX_INFO)
	{
		colormap = gtk_widget_get_colormap (dialogwidget);
		pixmap = gdk_pixmap_colormap_create_from_xpm_d (NULL, colormap, &mask, NULL, info);
		if (pixmap == NULL)
			g_error ("Couldn't create replacement pixmap.");
		icon = gtk_pixmap_new (pixmap, mask);
	}
	if (itype == MSGBOX_QUESTION)
	{
		colormap = gtk_widget_get_colormap (dialogwidget);
		pixmap = gdk_pixmap_colormap_create_from_xpm_d (NULL, colormap, &mask, NULL, help);
		if (pixmap == NULL)
			g_error ("Couldn't create replacement pixmap.");
		icon = gtk_pixmap_new (pixmap, mask);
	}

	if(icon) {
		gdk_pixmap_unref (pixmap);
		gdk_bitmap_unref (mask);
		dialogwidget = AddWidget (icon, hbox);
		gtk_widget_set_usize(GTK_WIDGET(icon), 40, 40);
	}

	dialogwidget = AddWidget (gtk_label_new (textmessage), hbox);

	buttonbox = gtk_hbutton_box_new();
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialogwindow)->action_area),
			   buttonbox, FALSE, FALSE, 0);

	btnok = gtk_button_new_with_label ("OK");
	GTK_WIDGET_SET_FLAGS (btnok, GTK_CAN_DEFAULT);
	btncancel = gtk_button_new_with_label ("Cancel");
	GTK_WIDGET_SET_FLAGS (btncancel, GTK_CAN_DEFAULT);
	btnyes = gtk_button_new_with_label ("Yes");
	GTK_WIDGET_SET_FLAGS (btnyes, GTK_CAN_DEFAULT);
	btnno = gtk_button_new_with_label ("No");
	GTK_WIDGET_SET_FLAGS (btnno, GTK_CAN_DEFAULT);

	if (type == MSGBOX_OKCANCEL)
	{
		dialogwidget = AddWidget (btnok, buttonbox);
		dialogwidget = AddWidget (btncancel, buttonbox);
		gtk_widget_grab_default (btnok);
	} else if (type == MSGBOX_YESNO) {
		dialogwidget = AddWidget (btnyes, buttonbox);
		dialogwidget = AddWidget (btnno, buttonbox);
		gtk_widget_grab_default (btnno);
	} else if (type == MSGBOX_OKONLY) {
		dialogwidget = AddWidget (btnok, buttonbox);
		gtk_widget_grab_default (btnok);
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

	if (modal == MSGBOX_MODAL)
	{
		gtk_window_set_modal (GTK_WINDOW (dialogwindow), TRUE);
	}
	gtk_widget_show_all (dialogwindow);
	gtk_main();
	return mb_status;
}

