/* 
 * File: dlg_about.c
 * Author: GGZ Development Team
 * Project: GGZ GTK games
 * Date: 10/12/2002
 * Desc: Create the "About" Gtk dialog
 * $Id: dlg_about.c 6268 2004-11-05 08:00:15Z jdorje $
 *
 * Copyright (C) 2000-2002 Brent Hendricks.
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
#include <string.h>

#include "dlg_about.h"
#include "ggzintl.h"

static const char *dlg_title = NULL;
static const char *dlg_header = NULL;
static const char *dlg_content = NULL;

static GtkWidget *create_dlg_about(void)
{
	GtkWidget *dialog;
	GtkWidget *vbox;
	GtkWidget *title_label;
	GtkWidget *body_label;
	GtkWidget *action_area;
	GtkWidget *close_button;

	/* 
	 * Create outer window.
	 */
	dialog = gtk_dialog_new();
	g_object_set_data(G_OBJECT(dialog), "dlg_about", dialog);
	gtk_window_set_title(GTK_WINDOW(dialog), dlg_title);

	/* 
	 * Get vertical box packing widget.
	 */
	vbox = GTK_DIALOG(dialog)->vbox;
	g_object_set_data(G_OBJECT(dialog), "vbox", vbox);
	gtk_container_set_border_width(GTK_CONTAINER(vbox), 10);
	gtk_widget_show(vbox);

	/* 
	 * Make title label
	 */
	title_label = gtk_label_new(dlg_header);
	gtk_widget_ref(title_label);
	g_object_set_data_full(G_OBJECT(dialog), "title_label",
			       title_label,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(title_label);
	gtk_box_pack_start(GTK_BOX(vbox), title_label, FALSE, FALSE, 0);

	/* 
	 * Make body label
	 */
	body_label = gtk_label_new(dlg_content);
	gtk_widget_ref(body_label);
	g_object_set_data_full(G_OBJECT(dialog), "body_label", body_label,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(body_label);
	gtk_box_pack_start(GTK_BOX(vbox), body_label, FALSE, FALSE, 0);
	gtk_label_set_justify(GTK_LABEL(body_label), GTK_JUSTIFY_LEFT);

	/* 
	 * Get "action area"
	 */
	action_area = GTK_DIALOG(dialog)->action_area;
	g_object_set_data(G_OBJECT(dialog), "dialog_action_area1",
			  action_area);
	gtk_widget_show(action_area);

	/* 
	 * Make "close" button
	 */
	close_button = gtk_button_new_from_stock(GTK_STOCK_CLOSE);
	gtk_widget_ref(close_button);
	g_object_set_data_full(G_OBJECT(dialog), "close_button",
			       close_button,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(close_button);
	gtk_box_pack_start(GTK_BOX(action_area), close_button, FALSE, FALSE,
			   0);
	g_signal_connect_swapped(close_button, "clicked",
				 GTK_SIGNAL_FUNC(gtk_widget_destroy),
				 dialog);

	/* 
	 * Set up callbacks
	 */
	g_signal_connect_swapped(dialog, "delete_event",
				 GTK_SIGNAL_FUNC(gtk_widget_destroy),
				 dialog);

	/* 
	 * Done!
	 */
	return dialog;
}

void init_dlg_about(const char *title, const char *header,
		    const char *about)
{
	/* The memory allocated here is leaked and lost.  But that's OK. */
	dlg_title = strdup(title);
	dlg_header = strdup(header);
	dlg_content = strdup(about);
}

void create_or_raise_dlg_about(void)
{
	static GtkWidget *dlg_about = NULL;

	if (dlg_about != NULL) {
		gdk_window_show(dlg_about->window);
		gdk_window_raise(dlg_about->window);
	} else {
		dlg_about = create_dlg_about();
		g_signal_connect(dlg_about, "destroy",
				 GTK_SIGNAL_FUNC(gtk_widget_destroyed),
				 &dlg_about);
		gtk_widget_show(dlg_about);
	}
}
