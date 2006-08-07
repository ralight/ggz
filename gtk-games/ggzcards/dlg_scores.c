/* 
 * File: dlg_scores.c
 * Author: Jason Short
 * Project: GGZCards Client
 * Date: August 7, 2006
 * Desc: Creates the scores dialog
 * $Id: dlg_scores.c 8481 2006-08-07 07:35:04Z jdorje $
 *
 * Copyright (C) 2006 GGZ Dev Team.
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
#include <string.h>

#include <gtk/gtk.h>

#include <ggz.h>

#include "ggzintl.h"

#include "dlg_scores.h"
#include "main.h"

static GtkWidget *window = NULL;
static GtkListStore *store;

static void dlg_scores_closed(GtkWidget * widget, gpointer data)
{
	gdk_window_hide(window->window);
}

void dlg_scores_raise(void)
{
	GtkWidget *tree;
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;
	GType types[20];
	int i;

	if (window) {
		gdk_window_show(window->window);
		gdk_window_raise(window->window);
		return;
	}

	window = gtk_dialog_new_with_buttons(_("Scores"),
					     GTK_WINDOW(dlg_main), 0,
					     GTK_STOCK_CLOSE,
					     GTK_RESPONSE_CLOSE, NULL);

	types[0] = G_TYPE_INT;
	for (i = 0; i < ggzcards.num_teams; i++) {
		types[i + 1] = G_TYPE_INT;
	}
	assert(i < 20);
	store = gtk_list_store_newv(i + 1, types);

	tree = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
	g_object_set_data(G_OBJECT(tree), "store", store);
	g_object_unref(store);

	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes(_("Hand #"),
							  renderer,
							  "text", 0, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(tree), column);

	for (i = 0; i < ggzcards.num_teams; i++) {
		char teamname[128];

		snprintf(teamname, sizeof(teamname), _("Team %d"), i + 1);
		renderer = gtk_cell_renderer_text_new();
		column
		    = gtk_tree_view_column_new_with_attributes(teamname,
							       renderer,
							       "text",
							       i + 1,
							       NULL);
		gtk_tree_view_append_column(GTK_TREE_VIEW(tree), column);
	}

	gtk_container_add(GTK_CONTAINER(GTK_DIALOG(window)->vbox), tree);

	g_signal_connect(window, "response",
			 GTK_SIGNAL_FUNC(dlg_scores_closed), NULL);
	g_signal_connect(window, "delete_event",
			 GTK_SIGNAL_FUNC(dlg_scores_closed), NULL);

	dlg_scores_update();

	gtk_widget_show_all(window);
}

void dlg_scores_update(void)
{
	int hand, team;

	if (!window) {
		return;
	}

	gtk_list_store_clear(store);
	for (hand = 0; hand <= ggzcards.hand_num; hand++) {
		GtkTreeIter iter;

		gtk_list_store_append(store, &iter);

		gtk_list_store_set(store, &iter, 0, hand + 1, -1);
		for (team = 0; team < ggzcards.num_teams; team++) {
			int score
			    = ggzcards.teams[team].scores[hand].score;

			gtk_list_store_set(store, &iter, team + 1, score,
					   -1);
		}
	}
}
