
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

static MBReturn mb_status;

/* TODO  - Make the icon take up less toom on the left of the dialog
 */


void CloseTheApp (GtkWidget *window, gpointer data)
{
	gtk_main_quit();
}


void DialogOKClicked (GtkButton *button, gpointer data)
{
	mb_status = MSGBOX_OK;
	gtk_widget_destroy (GTK_WIDGET (data));
}

void DialogCancelClicked (GtkButton *button, gpointer data)
{
	mb_status = MSGBOX_CANCEL;
	gtk_widget_destroy (GTK_WIDGET (data));
}

void DialogYesClicked (GtkButton *button, gpointer data)
{
	mb_status = MSGBOX_YES;
	gtk_widget_destroy (GTK_WIDGET (data));
}

void DialogNoClicked (GtkButton *button, gpointer data)
{
	mb_status = MSGBOX_NO;
	gtk_widget_destroy (GTK_WIDGET (data));
}

GtkWidget *AddWidget (GtkWidget *widget, GtkWidget *packingbox)
{
	gtk_box_pack_start (GTK_BOX (packingbox), widget, TRUE, TRUE, 2);
	return widget;
}


/* XPM */
static char *stop[] = {
/* width height ncolors chars_per_pixel */
"32 32 16 1",
/* colors */
"  c #FFFFFFFFFFFF",
"! c #800000000000",
"# c #000080000000",
"$ c #800080000000",
"% c #000000008000",
"& c #800000008000",
"' c #000080008000",
"( c #800080008000",
") c #c000c000c000",
"* c #ff0000000000",
"+ c #0000ff000000",
", c #ff00ff000000",
"- c #00000000ff00",
". c #ff000000ff00",
"/ c #0000ff00ff00",
"0 c None",
/* pixels */
"00000000000000000000000000000000",
"0000000******************0000000",
"000000**                **000000",
"00000** **************** **00000",
"0000** ****************** **0000",
"000** ******************** **000",
"00** ********************** **00",
"0** ************************ **0",
"** ************************** **",
"* **************************** *",
"* **************************** *",
"* **************************** *",
"* ****    *     **   **    *** *",
"* *** ******* *** *** * *** ** *",
"* *** ******* *** *** * *** ** *",
"* *** ******* *** *** *    *** *",
"* ****   **** *** *** * ****** *",
"* ******* *** *** *** * ****** *",
"* ******* *** *** *** * ****** *",
"* ******* *** *** *** * ****** *",
"* ***    **** ****   ** ****** *",
"* **************************** *",
"* **************************** *",
"** ************************** **",
"0** ************************ **0",
"00** ********************** **00",
"000** ******************** **000",
"0000** ****************** **0000",
"00000** **************** **00000",
"000000**                **000000",
"0000000******************0000000"
} ;


/* XPM */
static char *info[] = {
/* width height ncolors chars_per_pixel */
"32 32 4 1",
/* colors */
"` c black",
"a c cyan",
"b c None",
"c c slate grey",
/* pixels */
"bbbbbbbbbbbbbbbaaaabbbbbbbbbbbbb",
"bbbbbbbbbbbbbb````aabbbbbbbbbbbb",
"bbbbbbbbbbbbb``````abbbbbbbbbbbb",
"bbbbbbbbbbbbb``````abbbbbbbbbbbb",
"bbbbbbbbbbbbb``````abbbbbbbbbbbb",
"bbbbbbbbbbbbb``````bbbbbbbbbbbbb",
"bbbbbbbbbbbbbb````bbbbbbbbbbbbbb",
"bbbbbbbbbbbccccccccccbbbbbbbbbbb",
"bbbbbbbbbbaaaaaaaaaacbbbbbbbbbbb",
"bbbbbbbbb``````````acbbbbbbbbbbb",
"bbbbbbbbb``````````acbbbbbbbbbbb",
"bbbbbbbbb``````````acbbbbbbbbbbb",
"bbbbbbbbb``````````acbbbbbbbbbbb",
"bbbbbbbbbbbbb``````acbbbbbbbbbbb",
"bbbbbbbbbbbbb``````acbbbbbbbbbbb",
"bbbbbbbbbbbbb``````acbbbbbbbbbbb",
"bbbbbbbbbbbbb``````acbbbbbbbbbbb",
"bbbbbbbbbbbbb``````acbbbbbbbbbbb",
"bbbbbbbbbbbbb``````acbbbbbbbbbbb",
"bbbbbbbbbbbbb``````acbbbbbbbbbbb",
"bbbbbbbbbbbbb``````acbbbbbbbbbbb",
"bbbbbbbbbbbbb``````acbbbbbbbbbbb",
"bbbbbbbbbbbbb``````acbbbbbbbbbbb",
"bbbbbbbbbbbbb``````acbbbbbbbbbbb",
"bbbbbbbbbbbbb``````acbbbbbbbbbbb",
"bbbbbbbbbbbbb``````acbbbbbbbbbbb",
"bbbbbbbbbbbcc``````acccccbbbbbbb",
"bbbbbbbbbbaaa``````aaaaacbbbbbbb",
"bbbbbbbbb``````````````acbbbbbbb",
"bbbbbbbbb``````````````acbbbbbbb",
"bbbbbbbbb``````````````abbbbbbbb",
"bbbbbbbbb``````````````bbbbbbbbb"
};



MBReturn msgbox (gchar *textmessage, gchar *title, MBType type, MBIcon itype, MBModal modal)
{
	GtkWidget *dialogwindow;
	GtkWidget *packingbox;
	GtkWidget *buttonbox;
	GtkWidget *packingbox3;
	GtkWidget *icon = NULL;
	GdkColormap *colormap;
	GdkPixmap *pixmap;
	GdkPixmap *mask;
	GtkWidget *dialogwidget;
	GtkWidget *btnok;
	GtkWidget *btncancel;
	GtkWidget *btnyes;
	GtkWidget *btnno;

	dialogwindow = gtk_window_new (GTK_WINDOW_DIALOG);
	gtk_window_set_title (GTK_WINDOW(dialogwindow), title);
	gtk_container_set_border_width (GTK_CONTAINER (dialogwindow), 10);
	gtk_window_set_position (GTK_WINDOW(dialogwindow), GTK_WIN_POS_CENTER);

	packingbox = gtk_vbox_new (FALSE, 5);
	gtk_container_add (GTK_CONTAINER(dialogwindow), packingbox);

	packingbox3 = gtk_hbox_new (FALSE, 2);
	dialogwidget = AddWidget (packingbox3, packingbox);

	if (itype == MSGBOX_STOP)
	{
		colormap = gtk_widget_get_colormap (dialogwidget);
		pixmap = gdk_pixmap_colormap_create_from_xpm_d (NULL, colormap, &mask, NULL, stop);
		if (pixmap == NULL)
			g_error ("Couldn't create replacement pixmap.");
		icon = gtk_pixmap_new (pixmap, mask);
		gdk_pixmap_unref (pixmap);
		gdk_bitmap_unref (mask);
		dialogwidget = AddWidget (icon, packingbox3);
	}
	if (itype == MSGBOX_INFO)
	{
		colormap = gtk_widget_get_colormap (dialogwidget);
		pixmap = gdk_pixmap_colormap_create_from_xpm_d (NULL, colormap, &mask, NULL, info);
		if (pixmap == NULL)
			g_error ("Couldn't create replacement pixmap.");
		icon = gtk_pixmap_new (pixmap, mask);
		gdk_pixmap_unref (pixmap);
		gdk_bitmap_unref (mask);
		dialogwidget = AddWidget (icon, packingbox3);
	}
	gtk_widget_set_usize(GTK_WIDGET(icon), 40, 40);

	dialogwidget = AddWidget (gtk_label_new (textmessage), packingbox3);
	dialogwidget = AddWidget (gtk_hseparator_new(), packingbox);
	buttonbox = gtk_hbutton_box_new();
	dialogwidget = AddWidget (buttonbox, packingbox);

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

