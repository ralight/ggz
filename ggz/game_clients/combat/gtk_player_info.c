/*
 * File: gtk_player_info.h
 * Author: Ismael Orenstein
 * Project: GGZ Combat game module
 * Date: 10/14/2000
 * Desc: Player info widget
 * $Id: gtk_player_info.c 6293 2004-11-07 05:51:47Z jdorje $
 *
 * Copyright (C) 2000 Ismael Orenstein.
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

#include <gtk/gtk.h>

#include "game.h"

extern GdkColor *player_colors;

void game_unit_list_handle(GtkCList * clist, gint row, gint column,
			   GdkEventButton * event, gpointer user_data);


GtkWidget *gtk_player_info_new(GtkWidget * parent, char *name, int seat)
{
	GtkWidget *player_info;
	GtkWidget *player_name;
	GtkWidget *unit_view;
	GtkWidget *unit_list;
	GtkWidget *name_lbl;
	GtkWidget *power_lbl;
	GtkWidget *number_lbl;
	GtkStyle *label_style;
	int j;

	// Create the initial vbox
	player_info = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(player_info);
	g_object_set_data_full(G_OBJECT(parent), name, player_info,
			       (GtkDestroyNotify) gtk_widget_unref);

	// Name of the player
	player_name = gtk_label_new("Player Name");
	gtk_widget_ref(player_name);
	g_object_set_data_full(G_OBJECT(player_info), "player_name",
			       player_name,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(player_name);
	gtk_box_pack_start(GTK_BOX(player_info), player_name, FALSE, FALSE,
			   0);
	gtk_widget_set_usize(player_name, 70, -2);

	/* Color of label */
	label_style = gtk_style_new();
	gtk_style_set_font(label_style, gdk_font_load("fixed"));
	for (j = 0; j < 5; j++)
		label_style->fg[j] = player_colors[seat];
	gtk_widget_set_style(player_name, label_style);

	// List of units
	unit_view = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_ref(unit_view);
	g_object_set_data_full(G_OBJECT(player_info), "unit_view",
			       unit_view,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(unit_view);
	gtk_box_pack_start(GTK_BOX(player_info), unit_view, TRUE, TRUE, 0);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(unit_view),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);

	unit_list = gtk_clist_new(3);
	gtk_widget_ref(unit_list);
	g_object_set_data_full(G_OBJECT(player_info), "unit_list",
			       unit_list,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(unit_list);
	gtk_container_add(GTK_CONTAINER(unit_view), unit_list);
	gtk_widget_set_usize(unit_list, 150, -2);
	gtk_clist_set_column_width(GTK_CLIST(unit_list), 0, 50);
	gtk_clist_set_column_width(GTK_CLIST(unit_list), 1, 55);
	gtk_clist_set_column_width(GTK_CLIST(unit_list), 2, 45);
	gtk_clist_column_titles_show(GTK_CLIST(unit_list));

	// Ugly hack
	/*
	   gtk_widget_set_usize(unit_list, 150, 300);
	   gtk_widget_set_usize(unit_list, 150, 240);
	   gtk_widget_set_usize(unit_list, 150, -2);
	 */

	// Collums
	name_lbl = gtk_label_new("Name");
	gtk_widget_ref(name_lbl);
	g_object_set_data_full(G_OBJECT(player_info), "name_lbl", name_lbl,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(name_lbl);
	gtk_clist_set_column_widget(GTK_CLIST(unit_list), 0, name_lbl);

	number_lbl = gtk_label_new("Number");
	gtk_widget_ref(number_lbl);
	g_object_set_data_full(G_OBJECT(player_info), "number_lbl",
			       number_lbl,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(number_lbl);
	gtk_clist_set_column_widget(GTK_CLIST(unit_list), 1, number_lbl);

	power_lbl = gtk_label_new("Power");
	gtk_widget_ref(power_lbl);
	g_object_set_data_full(G_OBJECT(player_info), "Power", power_lbl,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(power_lbl);
	gtk_clist_set_column_widget(GTK_CLIST(unit_list), 2, power_lbl);


	// Connect signals
	g_signal_connect(GTK_OBJECT(unit_list), "select-row",
			 GTK_SIGNAL_FUNC(game_unit_list_handle),
			 GINT_TO_POINTER(1));
	g_signal_connect(GTK_OBJECT(unit_list), "unselect-row",
			 GTK_SIGNAL_FUNC(game_unit_list_handle),
			 GINT_TO_POINTER(-1));


	return player_info;
}
