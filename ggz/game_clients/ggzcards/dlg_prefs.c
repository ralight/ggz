/* 
 * File: dlg_prefs.h
 * Author: Jason Short
 * Project: GGZCards Client
 * Date: 08/20/2000
 * Desc: Create the "Preferences" Gtk dialog
 * $Id: dlg_prefs.c 3351 2002-02-13 09:15:18Z jdorje $
 *
 * Copyright (C) 2000-2002 GGZ Development Team
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
}

static void on_fasteranimation_toggled(GtkToggleButton * togglebutton,
				 gpointer user_data)
{
	preferences.faster_animation = togglebutton->active;
}

static void on_smootheranimation_toggled(GtkToggleButton * togglebutton,
				 gpointer user_data)
{
	preferences.smoother_animation = togglebutton->active;
}

static void on_autostart_toggled(GtkToggleButton * togglebutton,
				 gpointer user_data)
{
	preferences.autostart = togglebutton->active;
}

static void on_cardlists_toggled(GtkToggleButton * togglebutton,
				 gpointer user_data)
{
	preferences.cardlists = togglebutton->active;
}

static void on_defaultoptions_toggled(GtkToggleButton *togglebutton,
				      gpointer user_data)
{
	preferences.use_default_options = togglebutton->active;
}

GtkWidget *create_dlg_prefs(void)
{
	GtkWidget *dialog;
	GtkWidget *vbox;
	GtkWidget *button;
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
	button = gtk_check_button_new_with_label(_("Use animation"));
	gtk_widget_ref(button);
	gtk_widget_show(button);
	gtk_box_pack_start(GTK_BOX(vbox), button, FALSE, FALSE, 0);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button),
				     preferences.animation);
	(void) gtk_signal_connect(GTK_OBJECT(button), "toggled",
			   GTK_SIGNAL_FUNC(on_animation_toggled), NULL);

	/*
	 * Make "faster animation" button
	 */
	button = gtk_check_button_new_with_label(_("Faster animation"));
	gtk_widget_ref(button);
	gtk_widget_show(button);
	gtk_box_pack_start(GTK_BOX(vbox), button, FALSE, FALSE, 0);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button),
				     preferences.faster_animation);
	(void) gtk_signal_connect(GTK_OBJECT(button), "toggled",
			   GTK_SIGNAL_FUNC(on_fasteranimation_toggled), NULL);

	/*
	 * Make "smoother animation" button
	 */
	button = gtk_check_button_new_with_label(_("Smoother animation"));
	gtk_widget_ref(button);
	gtk_widget_show(button);
	gtk_box_pack_start(GTK_BOX(vbox), button, FALSE, FALSE, 0);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button),
				     preferences.smoother_animation);
	(void) gtk_signal_connect(GTK_OBJECT(button), "toggled",
			   GTK_SIGNAL_FUNC(on_smootheranimation_toggled), NULL);

	/* 
	 * Make "cardlists" button
	 */
	button = gtk_check_button_new_with_label(_("Show graphical "
		"cardlists (may not take effect immediately)"));
	gtk_widget_ref(button);
	gtk_widget_show(button);
	gtk_box_pack_start(GTK_BOX(vbox), button, FALSE, FALSE, 0);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button),
				     preferences.cardlists);
	(void) gtk_signal_connect(GTK_OBJECT(button), "toggled",
			   GTK_SIGNAL_FUNC(on_cardlists_toggled), NULL);

	/* 
	 * Make "autostart" button
	 */
	button = gtk_check_button_new_with_label(_("Automatically "
						   "start game"));
	gtk_widget_ref(button);
	gtk_widget_show(button);
	gtk_box_pack_start(GTK_BOX(vbox), button, FALSE, FALSE, 0);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button),
				     preferences.autostart);
	(void) gtk_signal_connect(GTK_OBJECT(button), "toggled",
			   GTK_SIGNAL_FUNC(on_autostart_toggled), NULL);
			
	/*
	 * Make "use_default_options" button
	 */
	button = gtk_check_button_new_with_label(_("Always use "
						   "default options"));
	gtk_widget_ref(button);
	gtk_widget_show(button);
	gtk_box_pack_start(GTK_BOX(vbox), button, FALSE, FALSE, 0);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button),
				     preferences.use_default_options);
	(void) gtk_signal_connect(GTK_OBJECT(button), "toggled",
			   GTK_SIGNAL_FUNC(on_defaultoptions_toggled), NULL);

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
