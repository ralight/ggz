/* 
 * File: dlg_about.c
 * Author: GGZ Development Team
 * Project: GGZ GTK games
 * Date: 10/12/2002
 * Desc: Create the "About" Gtk dialog
 * $Id: dlg_about.c 6982 2005-03-11 07:35:30Z jdorje $
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
#include "ggz_gtk.h"

static const char *dlg_title = NULL;
static const char *dlg_header = NULL;
static const char *dlg_content = NULL;

static GtkWidget *create_dlg_about(GtkWindow * parent)
{
	GtkWidget *dialog;
	GtkWidget *vbox;
	GtkWidget *title_label;
	GtkWidget *body_label;

	/* 
	 * Create outer window.
	 */
	dialog = gtk_dialog_new_with_buttons(dlg_title, parent, 0,
					     GTK_STOCK_CLOSE,
					     GTK_RESPONSE_CLOSE, NULL);
	g_object_set_data(G_OBJECT(dialog), "dlg_about", dialog);

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
	 * Set up callbacks
	 */
	g_signal_connect(dialog, "delete_event",
			 GTK_SIGNAL_FUNC(gtk_widget_destroy), NULL);
	g_signal_connect(dialog, "response",
			 GTK_SIGNAL_FUNC(gtk_widget_destroy), NULL);

	/* 
	 * Done!
	 */
	return dialog;
}

void init_dlg_about(const char *title, const char *header,
		    const char *about)
{
	/* The memory allocated here is leaked and lost.  But that's OK. */
	dlg_title = ggz_strdup(title);
	dlg_header = ggz_strdup(header);
	dlg_content = ggz_strdup(about);
}

void create_or_raise_dlg_about(void)
{
	static GtkWidget *dlg_about = NULL;

	if (dlg_about != NULL) {
		gdk_window_show(dlg_about->window);
		gdk_window_raise(dlg_about->window);
	} else {
		dlg_about = create_dlg_about(ggz_game_main_window);
		g_signal_connect(dlg_about, "destroy",
				 GTK_SIGNAL_FUNC(gtk_widget_destroyed),
				 &dlg_about);
		gtk_widget_show(dlg_about);
	}
}
