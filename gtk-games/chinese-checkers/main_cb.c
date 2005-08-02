/*
 * File: main_cb.c
 * Author: Rich Gade
 * Project: GGZ Chinese Checkers Client
 * Date: 01/01/2001
 * Desc: Callbacks for the main dialog window
 * $Id: main_cb.c 6385 2004-11-16 05:21:05Z jdorje $
 *
 * Copyright (C) 2001-2002 Richard Gade.
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

#include <assert.h>
#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>

#include "dlg_exit.h"
#include "menus.h"

#include "main_cb.h"
#include "main_dlg.h"
#include "prefs_dlg.h"
#include "support.h"
#include "display.h"
#include "game.h"

GtkWidget *dlg_prefs = NULL;


gboolean
on_dlg_main_delete_event(GtkWidget * widget,
			 GdkEvent * event, gpointer user_data)
{
	game_exit();
	return TRUE;
}


void game_exit(void)
{
	ggz_show_exit_dialog(TRUE);
}


void game_resync(void)
{
	assert(FALSE);
}

static void update_theme_list(void)
{
	gchar *name;
	GtkTreeView *tree;
	GtkListStore *store;
	GtkTreeSelection *select;
	int i;

	tree = GTK_TREE_VIEW(lookup_widget(dlg_prefs, "theme_list"));
	store =
	    GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(tree)));
	select = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree));

	gtk_list_store_clear(GTK_LIST_STORE(store));
	for (i = 0; i < game.num_themes; i++) {
		GtkTreeIter iter;

		name = game.theme_names[i];
		gtk_list_store_append(GTK_LIST_STORE(store), &iter);
		gtk_list_store_set(store, &iter,
				   THEME_COLUMN_NAME, game.theme_names[i],
				   -1);

		if (strcmp(game.theme_names[i], game.theme) == 0) {
			gtk_tree_selection_select_iter(select, &iter);
		}
	}
}

void
on_preferences_menu_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkWidget *toggle;

	if (dlg_prefs != NULL) {
		gdk_window_show(dlg_prefs->window);
		gdk_window_raise(dlg_prefs->window);
	} else {
		dlg_prefs = create_dlg_prefs();
		g_signal_connect(GTK_OBJECT(dlg_prefs),
				 "destroy",
				 GTK_SIGNAL_FUNC(gtk_widget_destroyed),
				 &dlg_prefs);
		update_theme_list();

		toggle =
		    g_object_get_data(G_OBJECT(dlg_prefs), "check_beep");
		if (game.beep == 1)
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
						     (toggle), TRUE);
		else
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
						     (toggle), FALSE);
		gtk_widget_show(dlg_prefs);
	}
}


gboolean
on_draw_area_expose_event(GtkWidget * widget,
			  GdkEventExpose * event, gpointer user_data)
{
	display_handle_expose_event(event);
	return FALSE;
}

gboolean on_draw_area_configure_event(GtkWidget * widget,
				      GdkEventExpose * event,
				      gpointer user_data)
{
	display_resized();
	return TRUE;
}


gboolean
on_draw_area_button_press_event(GtkWidget * widget,
				GdkEventButton * event, gpointer user_data)
{
	if (event->button == 1)
		display_handle_click_event(event);
	return TRUE;
}
