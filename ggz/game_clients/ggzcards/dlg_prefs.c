/*
 * File: dlg_prefs.h
 * Author: Jason Short
 * Project: GGZCards Client
 * Date: 08/20/2000
 * Desc: Create the "Preferences" Gtk dialog
 * $Id: dlg_prefs.c 2971 2001-12-21 01:22:05Z jdorje $
 *
 * Copyright (C) 2001 GGZ Development Team
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

#include <ggz.h>

#include "game.h"
#include "dlg_prefs.h"

static void on_animation_toggled(GtkToggleButton * togglebutton,
				 gpointer user_data)
{
	preferences.animation = togglebutton->active;

	ggz_debug("main", "Animation set to %d.", preferences.animation);
}

static void on_cardlists_toggled(GtkToggleButton * togglebutton,
				 gpointer user_data)
{
	preferences.cardlists = togglebutton->active;

	ggz_debug("main", "Graphical cardlists set to %d.",
		  preferences.animation);
}

GtkWidget *create_dlg_prefs(void)
{
	GtkWidget *dialog;
	GtkWidget *vbox;
	GtkWidget *animation_pref_button;
	GtkWidget *cardlist_pref_button;
	GtkWidget *action_area;
	GtkWidget *close_button;

	/*
	 * Create outer window.
	 */
	dialog = gtk_dialog_new();
	gtk_object_set_data(GTK_OBJECT(dialog), "dlg_prefs", dialog);
	gtk_window_set_title(GTK_WINDOW(dialog), _("Preferences"));
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
	 * Make "animation" button
	 */
	animation_pref_button =
		gtk_check_button_new_with_label(_("Use Animation"));
	gtk_widget_ref(animation_pref_button);
	gtk_object_set_data_full(GTK_OBJECT(dialog), "animation_pref_button",
				 animation_pref_button,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(animation_pref_button);
	gtk_box_pack_start(GTK_BOX(vbox), animation_pref_button, FALSE, FALSE,
			   0);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(animation_pref_button),
				     preferences.animation);
	gtk_signal_connect(GTK_OBJECT(animation_pref_button), "toggled",
			   GTK_SIGNAL_FUNC(on_animation_toggled), NULL);

	/*
	 * Make "cardlists" button
	 */
	cardlist_pref_button =
		gtk_check_button_new_with_label(_
						("Show Graphical Cardlists"));
	gtk_widget_ref(cardlist_pref_button);
	gtk_object_set_data_full(GTK_OBJECT(dialog), "cardlist_pref_button",
				 cardlist_pref_button,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(cardlist_pref_button);
	gtk_box_pack_start(GTK_BOX(vbox), cardlist_pref_button, FALSE, FALSE,
			   0);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cardlist_pref_button),
				     TRUE);
	gtk_signal_connect(GTK_OBJECT(cardlist_pref_button), "toggled",
			   GTK_SIGNAL_FUNC(on_cardlists_toggled), NULL);
	gtk_widget_set_sensitive(cardlist_pref_button, FALSE);

	/*
	 * Get "action area"
	 */
	action_area = GTK_DIALOG(dialog)->action_area;
	gtk_widget_show(action_area);

	/*
	 * Make "close" button
	 */
	close_button = gtk_button_new_with_label(_("Close"));
	gtk_widget_ref(close_button);
	gtk_widget_show(close_button);
	gtk_box_pack_start(GTK_BOX(action_area), close_button, FALSE, FALSE,
			   0);
	gtk_widget_set_usize(close_button, 64, -2);
	gtk_signal_connect_object(GTK_OBJECT(close_button), "clicked",
				  GTK_SIGNAL_FUNC(gtk_widget_destroy),
				  GTK_OBJECT(dialog));

	/*
	 * Set up callbacks
	 */
	gtk_signal_connect_object(GTK_OBJECT(dialog), "delete_event",
				  GTK_SIGNAL_FUNC(gtk_widget_destroy),
				  GTK_OBJECT(dialog));

	/*
	 * Done!
	 */
	return dialog;
}
