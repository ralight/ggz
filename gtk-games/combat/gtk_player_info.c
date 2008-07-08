/*
 * File: gtk_player_info.h
 * Author: Ismael Orenstein
 * Project: GGZ Combat game module
 * Date: 10/14/2000
 * Desc: Player info widget
 * $Id: gtk_player_info.c 10235 2008-07-08 19:42:13Z jdorje $
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

#include <assert.h>

#include <gtk/gtk.h>

#include "ggz_gtk.h"

#include "game.h"

extern GdkColor *player_colors;

static GtkWidget *tree_new(GtkWidget * parent)
{
	GtkListStore *store;
	GtkWidget *tree;
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;
	GtkTreeSelection *selection;

	assert(UNIT_COLUMNS == 4);
	store = gtk_list_store_new(UNIT_COLUMNS, G_TYPE_INT,
				   G_TYPE_STRING, G_TYPE_STRING,
				   G_TYPE_STRING);
	tree = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
	g_object_unref(store);

	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes(_("Name"),
							  renderer, "text",
							  UNIT_COLUMN_NAME,
							  NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(tree), column);

	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes(_("Number"),
							  renderer, "text",
							  UNIT_COLUMN_NUMBER,
							  NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(tree), column);

	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes(_("Power"),
							  renderer, "text",
							  UNIT_COLUMN_POWER,
							  NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(tree), column);

	gtk_widget_ref(tree);
	g_object_set_data_full(G_OBJECT(parent), "unit_list",
			       tree, (GtkDestroyNotify) gtk_widget_unref);
	g_object_set_data(G_OBJECT(parent), "unit_list_store", store);
	gtk_widget_show(tree);

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree));
	gtk_tree_selection_set_mode(selection, GTK_SELECTION_SINGLE);

	g_signal_connect(selection, "changed",
			 GTK_SIGNAL_FUNC(game_unit_list_handle), parent);

	return tree;
}

GtkWidget *gtk_player_info_new(GtkWidget * parent, char *name, int seat)
{
	GtkWidget *player_info;
	GtkWidget *player_name;
	GtkWidget *unit_view;
	GtkWidget *unit_list;
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

	/* Color of label */
	label_style = gtk_style_new();
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

	unit_list = tree_new(player_info);
	gtk_container_add(GTK_CONTAINER(unit_view), unit_list);

	return player_info;
}
