/* 
 * File: dlg_about.c
 * Author: Rich Gade, Jason Short
 * Project: GGZCards Client
 * Date: 08/20/2000
 * Desc: Create the "About" Gtk dialog
 * $Id: dlg_about.c 3908 2002-04-12 07:10:20Z jdorje $
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

#include "ggzintl.h"

#include "dlg_about.h"
#include "game.h"		/* for _( macro only */

#define VERSION "0.0.5"

GtkWidget *create_dlg_about(void)
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
	gtk_object_set_data(GTK_OBJECT(dialog), "dlg_about", dialog);
	gtk_window_set_title(GTK_WINDOW(dialog), _("About GGZCards"));
	GTK_WINDOW(dialog)->type = GTK_WINDOW_DIALOG;
	gtk_window_set_policy(GTK_WINDOW(dialog), TRUE, TRUE, FALSE);

	/* 
	 * Get vertical box packing widget.
	 */
	vbox = GTK_DIALOG(dialog)->vbox;
	gtk_object_set_data(GTK_OBJECT(dialog), "vbox", vbox);
	gtk_container_set_border_width(GTK_CONTAINER(vbox), 10);
	gtk_widget_show(vbox);

	/* 
	 * Make title label
	 */
	title_label =
		gtk_label_new(_
			      ("GGZ Gaming Zone\n"
			       "GGZ Cards Version " VERSION));
	gtk_widget_ref(title_label);
	gtk_object_set_data_full(GTK_OBJECT(dialog), "title_label",
				 title_label,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(title_label);
	gtk_box_pack_start(GTK_BOX(vbox), title_label, FALSE, FALSE, 0);

	/* 
	 * Make body label
	 */
	body_label = gtk_label_new(_("Authors:\n"
				     "        Gtk+ Client:\n"
				     "            Rich Gade        <rgade@users.sourceforge.net>\n"
				     "            Jason Short      <jdorje@users.sourceforge.net>\n"
				     "\n"
				     "        Game Server:\n"
				     "            Jason Short      <jdorje@users.sourceforge.net>\n"
				     "            Rich Gade        <rgade@users.sourceforge.net>\n"
				     "\n"
				     "        Game Modules:\n"
				     "            Jason Short      <jdorje@users.sourceforge.net>\n"
				     "            Rich Gade        <rgade@users.sourceforge.net>\n"
				     "            Ismael Orenstein <perdig@users.sourceforge.net>\n"
				     "\n"
				     "        AI Modules:\n"
				     "            Jason Short      <jdorje@users.sourceforge.net>\n"
				     "            Brent Hendricks  <bmh@users.sourceforge.net>\n"
				     "\n"
				     "Website:\n"
				     "        http://ggz.sourceforge.net/"));
	gtk_widget_ref(body_label);
	gtk_object_set_data_full(GTK_OBJECT(dialog), "body_label", body_label,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(body_label);
	gtk_box_pack_start(GTK_BOX(vbox), body_label, FALSE, FALSE, 0);
	gtk_label_set_justify(GTK_LABEL(body_label), GTK_JUSTIFY_LEFT);

	/* 
	 * Get "action area"
	 */
	action_area = GTK_DIALOG(dialog)->action_area;
	gtk_object_set_data(GTK_OBJECT(dialog), "dialog_action_area1",
			    action_area);
	gtk_widget_show(action_area);

	/* 
	 * Make "close" button
	 */
	close_button = gtk_button_new_with_label(_("Close"));
	gtk_widget_ref(close_button);
	gtk_object_set_data_full(GTK_OBJECT(dialog), "close_button",
				 close_button,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(close_button);
	gtk_box_pack_start(GTK_BOX(action_area), close_button, FALSE, FALSE,
			   0);
	gtk_widget_set_usize(close_button, 64, -2);
	(void) gtk_signal_connect_object(GTK_OBJECT(close_button), "clicked",
					 GTK_SIGNAL_FUNC(gtk_widget_destroy),
					 GTK_OBJECT(dialog));

	/* 
	 * Set up callbacks
	 */
	(void) gtk_signal_connect_object(GTK_OBJECT(dialog), "delete_event",
					 GTK_SIGNAL_FUNC(gtk_widget_destroy),
					 GTK_OBJECT(dialog));

	/* 
	 * Done!
	 */
	return dialog;
}
