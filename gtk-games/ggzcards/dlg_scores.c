/* 
 * File: dlg_scores.c
 * Author: Jason Short
 * Project: GGZCards Client
 * Date: August 7, 2006
 * Desc: Creates the scores dialog
 * $Id: dlg_scores.c 8493 2006-08-07 22:50:35Z jdorje $
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

#define MAX_COLS 20

static GtkWidget *window = NULL;
static GtkListStore *store;
static GtkTreeViewColumn *columns[MAX_COLS];

static void dlg_scores_closed(GtkWidget * widget, gpointer data)
{
	gdk_window_hide(window->window);
}

void dlg_scores_raise(void)
{
	GtkWidget *tree;
	GtkCellRenderer *renderer;
	GType types[MAX_COLS];
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
		types[i + 1] = G_TYPE_STRING;
	}
	assert(i < MAX_COLS);
	store = gtk_list_store_newv(i + 1, types);

	tree = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
	g_object_set_data(G_OBJECT(tree), "store", store);
	g_object_unref(store);

	renderer = gtk_cell_renderer_text_new();
	columns[0] = gtk_tree_view_column_new_with_attributes(_("Hand #"),
							      renderer,
							      "text", 0,
							      NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(tree), columns[0]);

	for (i = 0; i < ggzcards.num_teams; i++) {
		char teamname[128];

		snprintf(teamname, sizeof(teamname), _("Team %d"), i + 1);
		renderer = gtk_cell_renderer_text_new();
		columns[i + 1]
		    = gtk_tree_view_column_new_with_attributes(teamname,
							       renderer,
							       "text",
							       i + 1,
							       NULL);
		gtk_tree_view_append_column(GTK_TREE_VIEW(tree),
					    columns[i + 1]);
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

	for (team = 0; team < ggzcards.num_teams; team++) {
		int p, count = 0;
		char text[128] = "";

		for (p = 0; p < ggzcards.num_players; p++) {
			if (ggzcards.players[p].team == team) {
				if (count == 0) {
					snprintf(text, sizeof(text), "%s",
						 ggzcards.players[p].name);
				} else {
					snprintf(text + strlen(text),
						 sizeof(text) -
						 strlen(text), " / %s",
						 ggzcards.players[p].name);
				}
				count++;
			}
		}
		gtk_tree_view_column_set_title(columns[team + 1], text);
	}

	gtk_list_store_clear(store);
	for (hand = 0; hand <= ggzcards.hand_num; hand++) {
		GtkTreeIter iter;

		gtk_list_store_append(store, &iter);

		gtk_list_store_set(store, &iter, 0, hand + 1, -1);
		for (team = 0; team < ggzcards.num_teams; team++) {
			score_data_t score =
			    ggzcards.teams[team].scores[hand];
			char text[128];

			if (ggzcards.gametype
			    && strcmp(ggzcards.gametype, "spades") == 0) {
				snprintf(text, sizeof(text), "%d (%d)",
					 score.score, score.extra[0]);
			} else {
				snprintf(text, sizeof(text), "%d",
					 score.score);
			}
			gtk_list_store_set(store, &iter, team + 1, text,
					   -1);
		}
	}
}
