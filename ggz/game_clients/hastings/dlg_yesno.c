/*
 * File: dlg_yesno.c
 * Author: Josef Spillner
 * Project: GGZ Hastings1066 Client
 * Date: 11/01/2002
 * Desc: Create a "Yes/No" Gtk dialog
 *
 * Copyright (C) 2000 - 2002 Josef Spillnr
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

/* System includes */
#include <unistd.h>
#include <string.h>

/* Gtk+ includes */
#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

/* Hastings includes */
#include "dlg_yesno.h"
#include "ggzintl.h"

/* Glibal variables */
GtkWidget *dlg_yesno;

/* Callback for 'yes' button */
static void yesno_yes(GtkWidget * widget, gpointer data)
{
	gtk_widget_destroy(dlg_yesno);
	dlg_yesno = NULL;

	gtk_main_quit();
}

/* Create and show a yes/no question dialog */
GtkWidget *create_dlg_yesno(const char *question)
{
	GtkWidget *dialog_vbox1;
	GtkWidget *vbox1;
	GtkWidget *label3;
	GtkWidget *dialog_action_area1;
	GtkWidget *yes_button, *no_button;

	dlg_yesno = gtk_dialog_new();
	g_object_set_data(G_OBJECT(dlg_yesno), "dlg_yesno", dlg_yesno);
	gtk_window_set_title(GTK_WINDOW(dlg_yesno), _("Question"));
	gtk_window_set_policy(GTK_WINDOW(dlg_yesno), TRUE, TRUE, FALSE);

	dialog_vbox1 = GTK_DIALOG(dlg_yesno)->vbox;
	g_object_set_data(G_OBJECT(dlg_yesno), "dialog_vbox1",
			  dialog_vbox1);
	gtk_widget_show(dialog_vbox1);

	vbox1 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox1);
	g_object_set_data_full(G_OBJECT(dlg_yesno), "vbox1", vbox1,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox1);
	gtk_box_pack_start(GTK_BOX(dialog_vbox1), vbox1, TRUE, TRUE, 0);

	label3 = gtk_label_new(question);
	gtk_widget_ref(label3);
	g_object_set_data_full(G_OBJECT(dlg_yesno), "label3", label3,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label3);
	gtk_box_pack_start(GTK_BOX(vbox1), label3, FALSE, FALSE, 0);

	dialog_action_area1 = GTK_DIALOG(dlg_yesno)->action_area;
	g_object_set_data(G_OBJECT(dlg_yesno), "dialog_action_area1",
			  dialog_action_area1);
	gtk_widget_show(dialog_action_area1);
	gtk_container_set_border_width(GTK_CONTAINER(dialog_action_area1),
				       10);

	yes_button = gtk_button_new_with_label(_("Yes"));
	gtk_widget_ref(yes_button);
	g_object_set_data_full(G_OBJECT(dlg_yesno), "yes_button",
			       yes_button,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(yes_button);
	gtk_box_pack_start(GTK_BOX(dialog_action_area1), yes_button, FALSE,
			   FALSE, 0);

	no_button = gtk_button_new_with_label(_("No"));
	gtk_widget_ref(no_button);
	g_object_set_data_full(G_OBJECT(dlg_yesno), "no_button", no_button,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(no_button);
	gtk_box_pack_start(GTK_BOX(dialog_action_area1), no_button, FALSE,
			   FALSE, 0);

	g_signal_connect_swapped(GTK_OBJECT(dlg_yesno), "delete_event",
				 GTK_SIGNAL_FUNC(gtk_widget_destroy),
				 GTK_OBJECT(dlg_yesno));
	g_signal_connect_swapped(GTK_OBJECT(yes_button), "clicked",
				 GTK_SIGNAL_FUNC(yesno_yes), NULL);
	g_signal_connect_swapped(GTK_OBJECT(no_button), "clicked",
				 GTK_SIGNAL_FUNC(gtk_widget_destroy),
				 GTK_OBJECT(dlg_yesno));
	return dlg_yesno;
}
