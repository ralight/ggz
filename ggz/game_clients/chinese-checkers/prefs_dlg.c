/*
 * File: prefs_dlg.h
 * Author: Rich Gade
 * Project: GGZ Chinese Checkers Client
 * Date: 2001
 * Desc: Preferences dialog
 * $Id: prefs_dlg.c 6337 2004-11-12 17:10:03Z jdorje $
 *
 * Copyright (C) 2001-2004 GGZ Development Team
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
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include "prefs_cb.h"
#include "prefs_dlg.h"
#include "support.h"

static GtkWidget *theme_list_new(GtkWidget * parent)
{
	GtkListStore *store;
	GtkWidget *tree;
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;
	GtkTreeSelection *select;

	assert(THEME_COLUMNS == 1);
	store = gtk_list_store_new(THEME_COLUMNS, G_TYPE_STRING);
	tree = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
	g_object_unref(store);

	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes("", renderer,
							  "text",
							  THEME_COLUMN_NAME,
							  NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(tree), column);

	gtk_widget_ref(tree);
	g_object_set_data_full(G_OBJECT(parent), "theme_list",
			       tree, (GtkDestroyNotify) gtk_widget_unref);
	g_object_set_data(G_OBJECT(parent), "table_list_store", store);
	gtk_widget_show(tree);
	GTK_WIDGET_UNSET_FLAGS(tree, GTK_CAN_FOCUS);
	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(tree), FALSE);

	select = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree));
	gtk_tree_selection_set_mode(select, GTK_SELECTION_SINGLE);

	g_signal_connect(select, "changed",
			 GTK_SIGNAL_FUNC(on_theme_list_select_changed),
			 NULL);

	return tree;
}

GtkWidget *create_dlg_prefs(void)
{
	GtkWidget *dlg_prefs;
	GtkWidget *dialog_vbox1;
	GtkWidget *vbox1;
	GtkWidget *frame1;
	GtkWidget *check_beep;
	GtkWidget *frame2;
	GtkWidget *scrolledwindow1;
	GtkWidget *theme_list;
	GtkWidget *dialog_action_area1;
	GtkWidget *ok_button;
	GtkWidget *apply_button;
	GtkWidget *cancel_button;
	GtkTooltips *tooltips;

	tooltips = gtk_tooltips_new();

	dlg_prefs = gtk_dialog_new();
	gtk_widget_set_name(dlg_prefs, "dlg_prefs");
	g_object_set_data(G_OBJECT(dlg_prefs), "dlg_prefs", dlg_prefs);
	gtk_window_set_title(GTK_WINDOW(dlg_prefs),
			     "Chinese Checkers Preferences");
	gtk_window_set_policy(GTK_WINDOW(dlg_prefs), TRUE, TRUE, FALSE);

	dialog_vbox1 = GTK_DIALOG(dlg_prefs)->vbox;
	gtk_widget_set_name(dialog_vbox1, "dialog_vbox1");
	g_object_set_data(G_OBJECT(dlg_prefs), "dialog_vbox1",
			  dialog_vbox1);
	gtk_widget_show(dialog_vbox1);

	vbox1 = gtk_vbox_new(FALSE, 0);
	gtk_widget_set_name(vbox1, "vbox1");
	gtk_widget_ref(vbox1);
	g_object_set_data_full(G_OBJECT(dlg_prefs), "vbox1", vbox1,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox1);
	gtk_box_pack_start(GTK_BOX(dialog_vbox1), vbox1, TRUE, TRUE, 0);

	frame1 = gtk_frame_new("General Preferences");
	gtk_widget_set_name(frame1, "frame1");
	gtk_widget_ref(frame1);
	g_object_set_data_full(G_OBJECT(dlg_prefs), "frame1", frame1,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(frame1);
	gtk_box_pack_start(GTK_BOX(vbox1), frame1, FALSE, FALSE, 0);

	check_beep = gtk_check_button_new_with_label("Beep on our turn");
	gtk_widget_set_name(check_beep, "check_beep");
	gtk_widget_ref(check_beep);
	g_object_set_data_full(G_OBJECT(dlg_prefs), "check_beep",
			       check_beep,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(check_beep);
	gtk_container_add(GTK_CONTAINER(frame1), check_beep);
	gtk_container_set_border_width(GTK_CONTAINER(check_beep), 2);

	frame2 = gtk_frame_new("Board Theme Selection");
	gtk_widget_set_name(frame2, "frame2");
	gtk_widget_ref(frame2);
	g_object_set_data_full(G_OBJECT(dlg_prefs), "frame2", frame2,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(frame2);
	gtk_box_pack_start(GTK_BOX(vbox1), frame2, TRUE, TRUE, 0);

	scrolledwindow1 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_set_name(scrolledwindow1, "scrolledwindow1");
	gtk_widget_ref(scrolledwindow1);
	g_object_set_data_full(G_OBJECT(dlg_prefs), "scrolledwindow1",
			       scrolledwindow1,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(scrolledwindow1);
	gtk_container_add(GTK_CONTAINER(frame2), scrolledwindow1);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow1),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_AUTOMATIC);

	theme_list = theme_list_new(dlg_prefs);
	gtk_container_add(GTK_CONTAINER(scrolledwindow1), theme_list);

	dialog_action_area1 = GTK_DIALOG(dlg_prefs)->action_area;
	gtk_widget_set_name(dialog_action_area1, "dialog_action_area1");
	g_object_set_data(G_OBJECT(dlg_prefs), "dialog_action_area1",
			  dialog_action_area1);
	gtk_widget_show(dialog_action_area1);
	gtk_container_set_border_width(GTK_CONTAINER(dialog_action_area1),
				       10);

	ok_button = gtk_button_new_with_label("OK");
	gtk_widget_set_name(ok_button, "ok_button");
	gtk_widget_ref(ok_button);
	g_object_set_data_full(G_OBJECT(dlg_prefs), "ok_button", ok_button,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(ok_button);
	gtk_box_pack_start(GTK_BOX(dialog_action_area1), ok_button, FALSE,
			   FALSE, 0);
	gtk_tooltips_set_tip(tooltips, ok_button,
			     "Save changes and dismiss preferences dialog",
			     NULL);

	apply_button = gtk_button_new_with_label("Apply");
	gtk_widget_set_name(apply_button, "apply_button");
	gtk_widget_ref(apply_button);
	g_object_set_data_full(G_OBJECT(dlg_prefs), "apply_button",
			       apply_button,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(apply_button);
	gtk_box_pack_start(GTK_BOX(dialog_action_area1), apply_button,
			   FALSE, FALSE, 0);
	gtk_tooltips_set_tip(tooltips, apply_button,
			     "Save changes, keeping preferences dialog open",
			     NULL);

	cancel_button = gtk_button_new_with_label("Cancel");
	gtk_widget_set_name(cancel_button, "cancel_button");
	gtk_widget_ref(cancel_button);
	g_object_set_data_full(G_OBJECT(dlg_prefs), "cancel_button",
			       cancel_button,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(cancel_button);
	gtk_box_pack_start(GTK_BOX(dialog_action_area1), cancel_button,
			   FALSE, FALSE, 0);
	gtk_tooltips_set_tip(tooltips, cancel_button,
			     "Cancel changes since the last apply", NULL);

	g_signal_connect(GTK_OBJECT(ok_button), "clicked",
			 GTK_SIGNAL_FUNC(on_ok_button_clicked), NULL);
	g_signal_connect(GTK_OBJECT(apply_button), "clicked",
			 GTK_SIGNAL_FUNC(on_apply_button_clicked), NULL);
	g_signal_connect(GTK_OBJECT(cancel_button), "clicked",
			 GTK_SIGNAL_FUNC(on_cancel_button_clicked), NULL);

	g_object_set_data(G_OBJECT(dlg_prefs), "tooltips", tooltips);

	return dlg_prefs;
}
