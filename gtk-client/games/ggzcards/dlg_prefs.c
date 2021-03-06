/* 
 * File: dlg_prefs.h
 * Author: Jason Short
 * Project: GGZCards Client
 * Date: 08/20/2000
 * Desc: Create the "Preferences" Gtk dialog
 * $Id: dlg_prefs.c 8515 2006-08-16 02:59:53Z jdorje $
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

#include "dlg_prefs.h"
#include "game.h"
#include "main.h"
#include "preferences.h"
#include "table.h"

static gboolean pref_toggled_redraw(gpointer data)
{
	/* Some preferences cause the hands to be drawn differently, so we
	   have to redraw when they are changed. */
	table_display_all_hands(FALSE);
	gtk_widget_queue_draw(table);
	return FALSE;
}

static void on_pref_toggled(GtkToggleButton * togglebutton, gpointer value)
{
	*(int *)value = togglebutton->active;
	g_idle_add(pref_toggled_redraw, NULL);
}

static void dlg_prefs_closed(GtkWidget * widget, gpointer data)
{
	save_preferences();
	gtk_widget_destroy(widget);

}

GtkWidget *create_dlg_prefs(void)
{
	GtkWidget *dialog;
	GtkWidget *vbox;
	GtkWidget *button;
	GtkTooltips *tooltips;

	PrefType *pref;

	/* 
	 * Create outer window.
	 */
	dialog = gtk_dialog_new_with_buttons(_("Preferences"),
					     GTK_WINDOW(dlg_main), 0,
					     GTK_STOCK_CLOSE,
					     GTK_RESPONSE_CLOSE, NULL);
	g_object_set_data(G_OBJECT(dialog), "dlg_prefs", dialog);

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
	g_object_set_data(G_OBJECT(dialog), "vbox", vbox);
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
		g_signal_connect(button, "toggled",
				 GTK_SIGNAL_FUNC(on_pref_toggled),
				 pref->value);
	}

	/* 
	 * Set up callbacks
	 */
	g_signal_connect(dialog, "delete_event",
			 GTK_SIGNAL_FUNC(dlg_prefs_closed), NULL);
	g_signal_connect(dialog, "response",
			 GTK_SIGNAL_FUNC(dlg_prefs_closed), NULL);

	/* 
	 * Done!
	 */
	return dialog;
}
