/*
 * File: main_cb.c
 * Author: Rich Gade
 * Project: GGZ Chinese Checkers Client
 * Date: 01/01/2001
 * Desc: Callbacks for the main dialog window
 * $Id: main_cb.c 5165 2002-11-03 07:54:39Z jdorje $
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
on_dlg_main_delete_event               (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
	game_exit();
	return TRUE;
}


void game_exit(void)
{
	ggz_show_exit_dialog(1, dlg_main);
}


void game_resync(void)
{
	assert(FALSE);
}


void
on_preferences_menu_activate	       (GtkMenuItem	*menuitem,
					gpointer	 user_data)
{
	GtkWidget *list, *toggle;
	int i, j=-1, d=-1;
	char **text;

	if(dlg_prefs != NULL) {
		gdk_window_show(dlg_prefs->window);
		gdk_window_raise(dlg_prefs->window);
	} else {
		text = calloc(1, sizeof(char *));

		dlg_prefs = create_dlg_prefs();
		gtk_signal_connect(GTK_OBJECT(dlg_prefs),
				   "destroy",
				   GTK_SIGNAL_FUNC(gtk_widget_destroyed),
				   &dlg_prefs);
		list = gtk_object_get_data(GTK_OBJECT(dlg_prefs), "theme_list");
		toggle = gtk_object_get_data(GTK_OBJECT(dlg_prefs), "check_beep");

		gtk_clist_columns_autosize(GTK_CLIST(list));
		for(i=0; i<game.num_themes; i++) {
			text[0] = game.theme_names[i];
			gtk_clist_append(GTK_CLIST(list), text);
			if(!strcmp(game.theme_names[i], "default"))
				d = i;
			if(!strcmp(game.theme_names[i], game.theme))
				j = i;
		}
		if(j == -1) {
			if(d == -1)
				j = d = 0;
			else
				j = d;
		}
		if(gtk_clist_row_is_visible(GTK_CLIST(list), j) != GTK_VISIBILITY_FULL)
			gtk_clist_moveto(GTK_CLIST(list), j, 0, 0.5, 0);
		gtk_clist_select_row(GTK_CLIST(list), j, 0);

		if(game.beep == 1)
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(toggle),
						     TRUE);
		else
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(toggle),
						     FALSE);
		gtk_widget_show(dlg_prefs);
	}
}


gboolean
on_draw_area_expose_event              (GtkWidget       *widget,
                                        GdkEventExpose  *event,
                                        gpointer         user_data)
{
	display_handle_expose_event(event);
	return FALSE;
}


gboolean
on_draw_area_button_press_event        (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
	if(event->button == 1)
		display_handle_click_event(event);
	return TRUE;
}
