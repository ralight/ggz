/* 
 * File: dlg_prefs.h
 * Author: Jason Short
 * Project: GGZCards Client
 * Date: 08/20/2000
 * Desc: Create the "Preferences" Gtk dialog
 * $Id: dlg_prefs.c 4656 2002-09-23 00:48:07Z jdorje $
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

#include "ggzintl.h"

#include "game.h"
#include "dlg_prefs.h"
#include "preferences.h"

static void on_pref_toggled(GtkToggleButton * togglebutton, gpointer value)
{
	*(int *) value = togglebutton->active;
}


GtkWidget *create_dlg_prefs(void)
{
	GtkWidget *dialog;
	GtkWidget *vbox;
	GtkWidget *button;
	GtkWidget *action_area;
	GtkWidget *close_button;
	GtkTooltips *tooltips;

	PrefType *pref;

	/* 
	 * Create outer window.
	 */
	dialog = gtk_dialog_new();
	gtk_object_set_data(GTK_OBJECT(dialog), "dlg_prefs", dialog);
	gtk_window_set_title(GTK_WINDOW(dialog), _("Preferences"));
	GTK_WINDOW(dialog)->type = GTK_WINDOW_DIALOG;
	gtk_window_set_policy(GTK_WINDOW(dialog), TRUE, TRUE, FALSE);

	/* 
	 * Set up tooltips.
	 */
	/* FIXME: do the tooltips need to be freed when the window is
	   destroyed? */
	tooltips = gtk_tooltips_new();

	/* 
	 * Get vertical box packing widget.
	 */
	vbox = GTK_DIALOG(dialog)->vbox;
	gtk_object_set_data(GTK_OBJECT(dialog), "vbox", vbox);
	gtk_container_set_border_width(GTK_CONTAINER(vbox), 10);
	gtk_widget_show(vbox);

	/* Make preferences buttons. */
	for (pref = pref_types; pref->name; pref++) {
		button = gtk_check_button_new_with_label(_(pref->desc));
		gtk_tooltips_set_tip(tooltips, button,
				     _(pref->fulldesc), NULL);
		gtk_widget_ref(button);
		gtk_widget_show(button);
		gtk_box_pack_start(GTK_BOX(vbox), button, FALSE, FALSE, 0);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button),
					     *pref->value);
		(void) gtk_signal_connect(GTK_OBJECT(button), "toggled",
					  GTK_SIGNAL_FUNC(on_pref_toggled),
					  pref->value);
	}

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
