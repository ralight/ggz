/*
 * File: roomlist.h
 * Author: GGZ Dev Team
 * Project: GGZ GTK Client
 * Date: 11/05/2004
 * $Id: roomlist.c 6279 2004-11-06 03:15:23Z jdorje $
 * 
 * List of rooms in the server
 * 
 * Copyright (C) 2000-2004 GGZ Development Team
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

#define GTK_DISABLE_DEPRECATED

#include <assert.h>

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <ggz.h>
#include <ggzcore.h>

#include "client.h"
#include "msgbox.h"
#include "roominfo.h"
#include "roomlist.h"
#include "server.h"
#include "support.h"

enum {
	ROOM_COLUMN_PTR,
	ROOM_COLUMN_NAME,
	ROOM_COLUMN_PLAYERS,
	ROOM_COLUMNS
};

static void client_join_room(GGZRoom *room)
{
	gchar *err_msg = NULL;
	gint singleclick, status = -1;
	GGZRoom *ggzroom;
	int id = ggzcore_room_get_id(room);

	switch (ggzcore_server_get_state(server)) {
	case GGZ_STATE_OFFLINE:
	case GGZ_STATE_CONNECTING:
	case GGZ_STATE_ONLINE:
	case GGZ_STATE_LOGGING_IN:
	case GGZ_STATE_LOGGING_OUT:
		err_msg = _("You can't join a room, you're not logged int");
		break;
	case GGZ_STATE_BETWEEN_ROOMS:
	case GGZ_STATE_ENTERING_ROOM:
		err_msg = _("You're already in between rooms");
		break;
	case GGZ_STATE_JOINING_TABLE:
	case GGZ_STATE_AT_TABLE:
	case GGZ_STATE_LEAVING_TABLE:
		err_msg = _("You can't switch rooms while playing a game");
		break;
	case GGZ_STATE_IN_ROOM:
	case GGZ_STATE_LOGGED_IN:
		status = 0;
		break;
	default:
		err_msg = _("Unknown error");
		status = -1;
	}

	if (status == 0) {
		ggzroom = ggzcore_server_get_cur_room(server);
			if (ggzcore_server_join_room(server, id) == 0) {
	
			/* Only desensitize with single click, dues to
	                some weird bug that freezes the mouse if we
        	        doubleclick and then desensitize */
			singleclick = ggzcore_conf_read_int("OPTIONS",
							    "ROOMENTRY",
							    FALSE);
			if (singleclick) {
				/* Set roomlist insensitive */
				sensitize_room_list(FALSE);
			}
			return;
		}
		else {
			err_msg = _("Error joining room");
		}
	}

	/* If we get here, there was an error */
	msgbox(err_msg, _("Error joining room"), MSGBOX_OKONLY, MSGBOX_STOP, 
	       MSGBOX_NORMAL);
}

static void client_room_join_activate(GtkMenuItem *menuitem, gpointer data)
{
	GGZRoom *room = data;

	client_join_room(room);
}

static void client_room_info_activate(GtkMenuItem *menuitem, gpointer data)
{
	/* Display room's info in a nice dialog */
	GGZRoom *room = data;

	room_info_create_or_raise(room);		
}

static GtkWidget *create_mnu_room(GGZRoom *room)
{
	GtkWidget *mnu_room;
	GtkWidget *info;
	GtkWidget *join;
	GtkAccelGroup *accel_group;

	accel_group = gtk_accel_group_new ();

	mnu_room = gtk_menu_new ();
	g_object_set_data(G_OBJECT (mnu_room), "mnu_room", mnu_room);

	info = gtk_menu_item_new_with_label (_("Info"));
	gtk_widget_ref (info);
	g_object_set_data_full(G_OBJECT (mnu_room), "info", info,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (info);
	gtk_container_add (GTK_CONTAINER (mnu_room), info);
	gtk_widget_add_accelerator (info, "activate", accel_group,
				    GDK_I, GDK_CONTROL_MASK,
				    GTK_ACCEL_VISIBLE);

	join = gtk_menu_item_new_with_label (_("Join"));
	gtk_widget_ref (join);
	g_object_set_data_full(G_OBJECT (mnu_room), "join", join,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (join);
	gtk_container_add (GTK_CONTAINER (mnu_room), join);
	gtk_widget_add_accelerator (join, "activate", accel_group,
				    GDK_J, GDK_CONTROL_MASK,
				    GTK_ACCEL_VISIBLE);

	g_signal_connect (GTK_OBJECT (info), "activate",
			  GTK_SIGNAL_FUNC (client_room_info_activate),
			  room);
	g_signal_connect (GTK_OBJECT (join), "activate",
			  GTK_SIGNAL_FUNC (client_room_join_activate),
			  room);

	gtk_menu_set_accel_group (GTK_MENU (mnu_room), accel_group);

	return mnu_room;
}

static gboolean room_list_event(GtkWidget *widget, GdkEvent *event,
				gpointer data)
{
	GdkEventButton *buttonevent = (GdkEventButton *) event;
	gboolean single_join;
	GtkWidget *menu;
	GtkWidget *tree = lookup_widget(win_main, "room_list");
	GtkTreeSelection *select
	  = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree));
	GtkTreeModel *model;
	GtkTreeIter iter;
	GGZRoom *room;

	if (!gtk_tree_selection_get_selected(select, &model, &iter)) {
		return FALSE;
	}

	single_join = ggzcore_conf_read_int("OPTIONS", "ROOMENTRY", FALSE);

	gtk_tree_model_get(model, &iter, ROOM_COLUMN_PTR, &room, -1);

	switch(event->type) { 
	case GDK_2BUTTON_PRESS:
		/* Double click */
		if (buttonevent->button == 1 && !single_join) {
			client_join_room(room);
			return TRUE;
		}
		break;
	case GDK_BUTTON_PRESS:
		/* Single click */
		if (buttonevent->button == 1 && single_join) {
			client_join_room(room);
			return TRUE;
		} else if (buttonevent->button == 3) {
			/* Right mouse button */
			/* Create and display the menu */
			menu = create_mnu_room(room);
			gtk_menu_popup(GTK_MENU(menu), NULL, NULL, NULL,
				       NULL, buttonevent->button, 0);
		}
		break;
	default:
		/* Some Other event */
		break;
	}

	return FALSE;
}

void sensitize_room_list(gboolean sensitive)
{
	GtkWidget *tree = lookup_widget(win_main, "room_list");

	gtk_widget_set_sensitive(tree, sensitive);
}

void clear_room_list(void)
{
	/* Clear current list of rooms */
	GtkListStore *store;

	store = GTK_LIST_STORE(lookup_widget(win_main, "room_list_store"));
	gtk_list_store_clear(store);
}

void select_room(GGZRoom *room)
{
	GtkTreeView *tree;
	GtkListStore *store;
	GtkTreeSelection *select;
	GtkTreeIter iter;
	int id = ggzcore_room_get_id(room), i;

	tree = GTK_TREE_VIEW(lookup_widget(win_main, "room_list"));
	store = GTK_LIST_STORE(gtk_tree_view_get_model(tree));
	select = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree));

	if (!gtk_tree_model_get_iter_first(GTK_TREE_MODEL(store), &iter)) {
		return;
	}
	for (i = 0; i < id; i++) {
		if (!gtk_tree_model_iter_next(GTK_TREE_MODEL(store), &iter)) {
			return;
		}
	}

	gtk_tree_selection_select_iter(select, &iter);
}

void update_one_room(GGZRoom *room)
{
	int players, id, i;
	char *name;
	GtkListStore *store;
	GtkTreeIter iter;

	/* Retrieve the player list widget. */
	store = GTK_LIST_STORE(lookup_widget(win_main, "room_list_store"));

	name = ggzcore_room_get_name(room);
	id = ggzcore_room_get_id(room);
	players = ggzcore_room_get_num_players(room);

	if (!gtk_tree_model_get_iter_first(GTK_TREE_MODEL(store), &iter)) {
		return;
	}
	for (i = 0; i < id; i++) {
		if (!gtk_tree_model_iter_next(GTK_TREE_MODEL(store), &iter)) {
			return;
		}
	}

	gtk_list_store_set(store, &iter,
			   ROOM_COLUMN_PTR, room,
			   ROOM_COLUMN_NAME, name, -1);
	if (players >= 0) {
		gtk_list_store_set(store, &iter,
				   ROOM_COLUMN_PLAYERS, players, -1);
	}
}

void update_room_list(void)
{
	GtkListStore *store;
	int i;
	const int numrooms = ggzcore_server_get_num_rooms(server);

	/* Retrieve the player list widget. */
	store = GTK_LIST_STORE(lookup_widget(win_main, "room_list_store"));

	gtk_list_store_clear(store);

	for (i = 0; i < numrooms; i++) {
		GGZRoom *room = ggzcore_server_get_nth_room(server, i);
		gchar *name = ggzcore_room_get_name(room);
		gint players = ggzcore_room_get_num_players(room);
		GtkTreeIter iter;

		gtk_list_store_append(store, &iter);
		gtk_list_store_set(store, &iter,
				   ROOM_COLUMN_PTR, room,
				   ROOM_COLUMN_NAME, name, -1);

		if (players >= 0) {
			gtk_list_store_set(store, &iter,
					   ROOM_COLUMN_PLAYERS, players, -1);
		}
	}
}

GtkWidget *create_room_list(GtkWidget *window)
{
	GtkListStore *store;
	GtkWidget *tree;
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;
	GtkTreeSelection *select;


	assert(ROOM_COLUMNS == 3);
	store = gtk_list_store_new(ROOM_COLUMNS,
				   G_TYPE_POINTER,
				   G_TYPE_STRING,
				   G_TYPE_INT);
	tree = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
	g_object_unref(store);

	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes(_("Room"),
							  renderer,
				"text", ROOM_COLUMN_NAME, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(tree), column);

	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes("#", renderer,
				"text", ROOM_COLUMN_PLAYERS, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(tree), column);

	gtk_widget_ref(tree);
	g_object_set_data_full(G_OBJECT(window), "room_list",
			       tree,
			       (GtkDestroyNotify) gtk_widget_unref);
	g_object_set_data(G_OBJECT(window), "room_list_store", store);
	gtk_widget_show(tree);
	gtk_widget_set_sensitive(tree, FALSE);
	GTK_WIDGET_UNSET_FLAGS(tree, GTK_CAN_FOCUS);

	select = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree));
	gtk_tree_selection_set_mode(select, GTK_SELECTION_SINGLE);

	g_signal_connect(tree, "button-press-event",
			 GTK_SIGNAL_FUNC(room_list_event), NULL);

	return tree;
}
