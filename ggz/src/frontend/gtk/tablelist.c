/*
 * File: tablelist.c
 * Author: GGZ Dev Team
 * Project: GGZ GTK Client
 * Date: 11/03/2002
 * $Id: tablelist.c 6272 2004-11-05 21:19:52Z jdorje $
 * 
 * List of tables in the current room
 * 
 * Copyright (C) 2000-2002 Justin Zaun.
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

#include <gtk/gtk.h>
#include <stdlib.h>

#include <ggzcore.h>

#include "client.h"
#include "tablelist.h"
#include "server.h"
#include "support.h"

static int client_get_table_index(guint row);
#if 0
static GtkWidget *create_mnu_table(void);
#endif

static void client_table_clist_select_row(GtkCList * clist, gint row,
					  gint column, GdkEvent * event,
					  gpointer data);
static void client_table_clist_click_column(GtkCList * clist, gint column,
					    gpointer data);
static gboolean client_table_event(GtkWidget * widget, GdkEvent * event,
				   gpointer data);



static gint numtables;
static gint table_selection_id = -1;

static void
client_table_clist_select_row(GtkCList * clist,
			      gint row,
			      gint column, GdkEvent * event, gpointer data)
{
	table_selection_id = client_get_table_index(row);
}


static int client_get_table_index(guint row)
{
	GtkWidget *tmp;
	char *text;
	int index;

	tmp = lookup_widget(win_main, "table_clist");
	gtk_clist_get_text(GTK_CLIST(tmp), row, 0, &text);
	index = atoi(text);

	return index;
}


static void
client_table_clist_click_column(GtkCList * clist, gint column, gpointer data)
{
}


static gboolean
client_table_event(GtkWidget * widget, GdkEvent * event, gpointer data)
{
	/* Check to see if the event was a mouse button press */
	if (event->type == GDK_2BUTTON_PRESS)
		client_start_table_join();
	return FALSE;
}


static int client_find_table_by_id(int table_id)
{
	int i;

	for (i = 0; i < numtables; i++)
		if (client_get_table_index(i) == table_id)
			return i;

	return -1;
}


#if 0
static GtkWidget *create_mnu_table(void)
{
	GtkWidget *mnu_table;
	GtkWidget *join;
	GtkWidget *leave;
	GtkWidget *menuitem3;
	GtkWidget *info;

	mnu_table = gtk_menu_new();
	g_object_set_data(G_OBJECT(mnu_table), "mnu_table", mnu_table);

	join = gtk_menu_item_new_with_label(_("Join"));
	gtk_widget_ref(join);
	g_object_set_data_full(G_OBJECT(mnu_table), "join", join,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(join);
	gtk_container_add(GTK_CONTAINER(mnu_table), join);

	leave = gtk_menu_item_new_with_label(_("Leave"));
	gtk_widget_ref(leave);
	g_object_set_data_full(G_OBJECT(mnu_table), "leave", leave,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(leave);
	gtk_container_add(GTK_CONTAINER(mnu_table), leave);

	menuitem3 = gtk_menu_item_new();
	gtk_widget_ref(menuitem3);
	g_object_set_data_full(G_OBJECT(mnu_table), "menuitem3",
				 menuitem3,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(menuitem3);
	gtk_container_add(GTK_CONTAINER(mnu_table), menuitem3);
	gtk_widget_set_sensitive(menuitem3, FALSE);

	info = gtk_menu_item_new_with_label(_("Info"));
	gtk_widget_ref(info);
	g_object_set_data_full(G_OBJECT(mnu_table), "info", info,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(info);
	gtk_container_add(GTK_CONTAINER(mnu_table), info);

#if 0				/* not implemented */
	g_signal_connect(GTK_OBJECT(join), "activate",
			   GTK_SIGNAL_FUNC(client_join_table_activate),
			   GINT_TO_POINTER(table_num));
	g_signal_connect(GTK_OBJECT(leave), "activate",
			   GTK_SIGNAL_FUNC(client_leave_activate), NULL);
	g_signal_connect(GTK_OBJECT(info), "activate",
			   GTK_SIGNAL_FUNC(client_table_info_activate),
			   GINT_TO_POINTER(table_num));
#endif

	return mnu_table;
}
#endif

int get_selected_table_row(void)
{
	return client_find_table_by_id(table_selection_id);
}


int get_selected_table_id(void)
{
	return table_selection_id;
}


void clear_tables(void)
{
	GtkWidget *tmp;

	tmp = lookup_widget(win_main, "table_clist");
	gtk_clist_freeze(GTK_CLIST(tmp));
	gtk_clist_clear(GTK_CLIST(tmp));
	gtk_clist_thaw(GTK_CLIST(tmp));

	table_selection_id = -1;
}


void display_tables(void)
{
	GtkWidget *tmp;
	gchar *table[4] = { NULL, NULL, NULL, NULL };
	const char *desc;
	gint i, num, avail, seats;
	GGZRoom *room;
	GGZTable *t = NULL;
	int tablerow = -1;

	/* Retrieve the list. */
	tmp = lookup_widget(win_main, "table_clist");

	/* "Freeze" the clist.  This prevents any graphical updating
	 * until we "thaw" it later. */
	gtk_clist_freeze(GTK_CLIST(tmp));

	/* Clear the table */
	gtk_clist_clear(GTK_CLIST(tmp));

	room = ggzcore_server_get_cur_room(server);

	/* Display current list of players
	   if (!(numbers = ggzcore_room_get_numbers()))
	   return GGZ_HOOK_OK; */

	numtables = ggzcore_room_get_num_tables(room);
	for (i = 0; i < numtables; i++) {

		t = ggzcore_room_get_nth_table(room, i);
		num = ggzcore_table_get_id(t);
		avail = ggzcore_table_get_seat_count(t, GGZ_SEAT_OPEN)
			+ ggzcore_table_get_seat_count(t, GGZ_SEAT_RESERVED);
		seats = ggzcore_table_get_num_seats(t);
		desc = ggzcore_table_get_desc(t);
		if (!desc) {
			desc = _("No description available.");
		}

		if (num == table_selection_id)
			tablerow = i;

		/* FIXME: we have a significant problem here.  Do we
		   display the number of open seats, the number of
		   available-to-us seats, or the total number of
		   unfilled seats?  Any way we do it we'll have
		   problems.  Right now I just show the total
		   number of unfilled seats. */

		table[0] = g_strdup_printf("%d", num);
		table[1] = g_strdup_printf("%d/%d", avail, seats);
		table[2] = g_strdup_printf("%s", desc);
		gtk_clist_append(GTK_CLIST(tmp), table);
		g_free(table[0]);
		g_free(table[1]);
		g_free(table[2]);
	}

	if (tablerow >= 0)
		gtk_clist_select_row(GTK_CLIST(tmp), tablerow, 0);
	else
		table_selection_id = -1;

	/* "Thaw" the clist (it was "frozen" up above). */
	gtk_clist_thaw(GTK_CLIST(tmp));
}

GtkWidget *create_table_list(GtkWidget * window)
{
	GtkWidget *table_clist;
	GtkWidget *table_no_label;
	GtkWidget *table_steats_label;
	GtkWidget *tabel_desc_label;

	table_clist = gtk_clist_new(3);
	gtk_widget_ref(table_clist);
	g_object_set_data_full(G_OBJECT(window), "table_clist",
				 table_clist,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(table_clist);
	gtk_widget_set_sensitive(table_clist, FALSE);
	GTK_WIDGET_UNSET_FLAGS(table_clist, GTK_CAN_FOCUS);
	gtk_clist_set_column_width(GTK_CLIST(table_clist), 0, 26);
	gtk_clist_set_column_width(GTK_CLIST(table_clist), 1, 38);
	gtk_clist_set_column_width(GTK_CLIST(table_clist), 2, 80);
	gtk_clist_column_titles_show(GTK_CLIST(table_clist));

	table_no_label = gtk_label_new(_("No."));
	gtk_widget_ref(table_no_label);
	g_object_set_data_full(G_OBJECT(window), "table_no_label",
				 table_no_label,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(table_no_label);
	gtk_clist_set_column_widget(GTK_CLIST(table_clist),
				    0, table_no_label);

	table_steats_label = gtk_label_new(_("Seats"));
	gtk_widget_ref(table_steats_label);
	g_object_set_data_full(G_OBJECT(window),
				 "table_steats_label", table_steats_label,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(table_steats_label);
	gtk_clist_set_column_widget(GTK_CLIST(table_clist), 1,
				    table_steats_label);

	tabel_desc_label = gtk_label_new(_("Description"));
	gtk_widget_ref(tabel_desc_label);
	g_object_set_data_full(G_OBJECT(window), "tabel_desc_label",
				 tabel_desc_label,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(tabel_desc_label);
	gtk_clist_set_column_widget(GTK_CLIST(table_clist), 2,
				    tabel_desc_label);

	g_signal_connect(GTK_OBJECT(table_clist), "select_row",
			   GTK_SIGNAL_FUNC(client_table_clist_select_row),
			   NULL);
	g_signal_connect(GTK_OBJECT(table_clist), "click_column",
			   GTK_SIGNAL_FUNC(client_table_clist_click_column),
			   NULL);
	g_signal_connect(GTK_OBJECT(table_clist), "button-press-event",
			   GTK_SIGNAL_FUNC(client_table_event), NULL);


	return table_clist;
}
