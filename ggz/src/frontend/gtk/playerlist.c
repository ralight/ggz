/*
 * File: playerlist.c
 * Author: GGZ Dev Team
 * Project: GGZ GTK Client
 * Date: 11/03/2002
 * $Id: playerlist.c 5890 2004-02-10 08:59:29Z jdorje $
 * 
 * List of players in the current room
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
#include <stdio.h>
#include <string.h>

#include <ggzcore.h>

#include "chat.h"
#include "client.h"
#include "playerinfo.h"
#include "playerlist.h"
#include "server.h"
#include "support.h"


#define PLAYER_LIST_COL_LAG 0
#define PLAYER_LIST_COL_TABLE 1
#define PLAYER_LIST_COL_STATS 2
#define PLAYER_LIST_COL_NAME 3

static char *client_get_players_index(guint row);
static GtkWidget *create_mnu_player(int player_num, gboolean is_friend,
				    gboolean is_ignore);

static gboolean client_player_clist_event(GtkWidget * widget,
					  GdkEvent * event, gpointer data);
static void client_player_info_activate(GtkMenuItem * menuitem,
					gpointer data);
static void client_player_friends_click(GtkMenuItem * menuitem,
					gpointer data);
static void client_player_ignore_click(GtkMenuItem * menuitem, gpointer data);
static void client_player_clist_select_row(GtkCList * clist, gint row,
					   gint column, GdkEvent * event,
					   gpointer data);

static char *client_get_players_index(guint row)
{
	GtkWidget *tmp;
	GdkPixmap *pixmap;
	GdkBitmap *mask;
	guint8 space;
	gchar *text = NULL;

	tmp = lookup_widget(win_main, "player_clist");
	gtk_clist_get_pixtext(GTK_CLIST(tmp), row, PLAYER_LIST_COL_NAME,
			      &text, &space, &pixmap, &mask);

	return text;
}


static void client_player_info_activate(GtkMenuItem * menuitem, gpointer data)
{
	/* Pop up an info dialog about the player */
	int player_num = GPOINTER_TO_INT(data);
	GGZRoom *room = ggzcore_server_get_cur_room(server);
	GGZPlayer *player = ggzcore_room_get_nth_player(room, player_num);
	player_info_create_or_raise(player);
}

static void client_player_friends_click(GtkMenuItem * menuitem, gpointer data)
{
	int player_num = GPOINTER_TO_INT(data);
	char *pname = client_get_players_index(player_num);

	if (GTK_CHECK_MENU_ITEM(menuitem)->active)
		chat_add_friend(pname, TRUE);
	else
		chat_remove_friend(pname);
}

static void client_player_ignore_click(GtkMenuItem * menuitem, gpointer data)
{
	int player_num = GPOINTER_TO_INT(data);
	char *pname = client_get_players_index(player_num);

	if (GTK_CHECK_MENU_ITEM(menuitem)->active)
		chat_add_ignore(pname, TRUE);
	else
		chat_remove_ignore(pname);
}

static gboolean
client_player_clist_event(GtkWidget * widget, GdkEvent * event, gpointer data)
{
	gint row, column;
	GdkEventButton *buttonevent = (GdkEventButton *) event;
	GtkWidget *clist = lookup_widget(win_main, "player_clist");

	if (!gtk_clist_get_selection_info(GTK_CLIST(clist),
					  buttonevent->x,
					  buttonevent->y, &row, &column)) {
		return FALSE;
	}

	if (event->type == GDK_BUTTON_PRESS) {
		/* Single click */
		if (buttonevent->button == 3) {
			/* Right mouse button */
			/* Create and display the menu */
			const char *player = client_get_players_index(row);
			int is_friend = chat_is_friend(player);
			int is_ignore = chat_is_ignore(player);
			GtkWidget *menu;
			gtk_clist_select_row(GTK_CLIST(clist), row, column);
			menu = create_mnu_player(row, is_friend, is_ignore);

			gtk_menu_popup(GTK_MENU(menu), NULL, NULL, NULL,
				       NULL, buttonevent->button, 0);
		}
	}

	return FALSE;
}


static void client_player_clist_select_row(GtkCList * clist, gint row,
					   gint column, GdkEvent * event,
					   gpointer data)
{

}


static GtkWidget *create_mnu_player(int player_num, gboolean is_friend,
				    gboolean is_ignore)
{
	GtkWidget *mnu_player;
#ifndef GTK2
	GtkAccelGroup *mnu_player_accels;
#endif
	GtkWidget *info;
	GtkWidget *separator9;
	GtkWidget *friends;
	GtkWidget *ignore;

	mnu_player = gtk_menu_new();
	gtk_object_set_data(GTK_OBJECT(mnu_player), "mnu_player", mnu_player);
#ifndef GTK2
	mnu_player_accels =
		gtk_menu_ensure_uline_accel_group(GTK_MENU(mnu_player));
#endif

	info = gtk_menu_item_new_with_label(_("Info"));
	gtk_widget_ref(info);
	gtk_object_set_data_full(GTK_OBJECT(mnu_player), "info", info,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(info);
	gtk_container_add(GTK_CONTAINER(mnu_player), info);

	separator9 = gtk_menu_item_new();
	gtk_widget_ref(separator9);
	gtk_object_set_data_full(GTK_OBJECT(mnu_player), "separator9",
				 separator9,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(separator9);
	gtk_container_add(GTK_CONTAINER(mnu_player), separator9);
	gtk_widget_set_sensitive(separator9, FALSE);

	friends = gtk_check_menu_item_new_with_label(_("Friends"));
	gtk_widget_ref(friends);
	gtk_object_set_data_full(GTK_OBJECT(mnu_player), "friends", friends,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(friends);
	gtk_container_add(GTK_CONTAINER(mnu_player), friends);
	gtk_check_menu_item_set_show_toggle(GTK_CHECK_MENU_ITEM(friends),
					    TRUE);
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(friends),
				       is_friend);

	ignore = gtk_check_menu_item_new_with_label(_("Ignore"));
	gtk_widget_ref(ignore);
	gtk_object_set_data_full(GTK_OBJECT(mnu_player), "ignore", ignore,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(ignore);
	gtk_container_add(GTK_CONTAINER(mnu_player), ignore);
	gtk_check_menu_item_set_show_toggle(GTK_CHECK_MENU_ITEM(ignore),
					    TRUE);
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(ignore),
				       is_ignore);

	gtk_signal_connect(GTK_OBJECT(info), "activate",
			   GTK_SIGNAL_FUNC(client_player_info_activate),
			   GINT_TO_POINTER(player_num));
	gtk_signal_connect(GTK_OBJECT(friends), "activate",
			   GTK_SIGNAL_FUNC(client_player_friends_click),
			   GINT_TO_POINTER(player_num));
	gtk_signal_connect(GTK_OBJECT(ignore), "activate",
			   GTK_SIGNAL_FUNC(client_player_ignore_click),
			   GINT_TO_POINTER(player_num));

	return mnu_player;
}


void display_players(void)
{
	GtkWidget *tmp;
	gint i, num;
	/* Some of the fields of the clist receive pixmaps
	 * instead, and are therefore set to NULL. */
	gchar *player[5] = { NULL, NULL, NULL, NULL, NULL };
	gchar *path = NULL;
	GGZPlayer *p;
	GGZTable *table;
	GGZRoom *room = ggzcore_server_get_cur_room(server);
	GdkPixmap *pixmap1 = NULL, *pixmap2 = NULL;
	GdkBitmap *mask1, *mask2;
	int wins, losses, ties, forfeits, rating, ranking;
	long highscore;
	char stats[512];

	/* Retrieve the player list (CList) widget. */
	tmp = lookup_widget(win_main, "player_clist");

	/* "Freeze" the clist.  This prevents any graphical updating
	 * until we "thaw" it later. */
	gtk_clist_freeze(GTK_CLIST(tmp));

	/* Clear current list of players */
	gtk_clist_clear(GTK_CLIST(tmp));

	/* Display current list of players */
	num = ggzcore_room_get_num_players(room);

	for (i = 0; i < num; i++) {
		p = ggzcore_room_get_nth_player(room, i);

		table = ggzcore_player_get_table(p);

		/* These values are freed down below. */
		if (!table)
			player[PLAYER_LIST_COL_TABLE] = g_strdup("--");
		else
			player[PLAYER_LIST_COL_TABLE]
				= g_strdup_printf("%d",
						  ggzcore_table_get_id
						  (table));

		if (ggzcore_player_get_ranking(p, &ranking)) {
			snprintf(stats, sizeof(stats), _("#%d"), ranking);
		} else if (ggzcore_player_get_highscore(p, &highscore)) {
			snprintf(stats, sizeof(stats), "%ld", highscore);
		} else if (ggzcore_player_get_rating(p, &rating)) {
			snprintf(stats, sizeof(stats), "%d", rating);
		} else if (ggzcore_player_get_record(p, &wins, &losses,
						     &ties, &forfeits)) {
			snprintf(stats, sizeof(stats), "%d-%d", wins, losses);
			if (ties > 0) {
				snprintf(stats + strlen(stats),
					 sizeof(stats) - strlen(stats),
					 "-%d", ties);
			}
			if (forfeits > 0) {
				snprintf(stats + strlen(stats),
					 sizeof(stats) - strlen(stats),
					 " (%d)", forfeits);
			}
		} else
			snprintf(stats, sizeof(stats), "%s", "");
		player[PLAYER_LIST_COL_STATS] = stats;

		player[PLAYER_LIST_COL_NAME]
			= g_strdup(ggzcore_player_get_name(p));

		gtk_clist_append(GTK_CLIST(tmp), player);

		if (ggzcore_player_get_lag(p) == -1
		    || ggzcore_player_get_lag(p) == 0)
			path = g_strjoin(G_DIR_SEPARATOR_S, GGZDATADIR,
					 "ggz_gtk_lag0.xpm", NULL);
		else if (ggzcore_player_get_lag(p) == 1)
			path = g_strjoin(G_DIR_SEPARATOR_S, GGZDATADIR,
					 "ggz_gtk_lag1.xpm", NULL);
		else if (ggzcore_player_get_lag(p) == 2)
			path = g_strjoin(G_DIR_SEPARATOR_S, GGZDATADIR,
					 "ggz_gtk_lag2.xpm", NULL);
		else if (ggzcore_player_get_lag(p) == 3)
			path = g_strjoin(G_DIR_SEPARATOR_S, GGZDATADIR,
					 "ggz_gtk_lag3.xpm", NULL);
		else if (ggzcore_player_get_lag(p) == 4)
			path = g_strjoin(G_DIR_SEPARATOR_S, GGZDATADIR,
					 "ggz_gtk_lag4.xpm", NULL);
		else if (ggzcore_player_get_lag(p) == 5)
			path = g_strjoin(G_DIR_SEPARATOR_S, GGZDATADIR,
					 "ggz_gtk_lag5.xpm", NULL);

		if (path) {
			pixmap1 =
				gdk_pixmap_create_from_xpm(tmp->window,
							   &mask1, NULL,
							   path);
			if (pixmap1)
				gtk_clist_set_pixmap(GTK_CLIST(tmp), i,
						     PLAYER_LIST_COL_LAG,
						     pixmap1, mask1);
			g_free(path);
		}


		if (ggzcore_player_get_type(p) == GGZ_PLAYER_GUEST)
			path = g_strjoin(G_DIR_SEPARATOR_S, GGZDATADIR,
					 "ggz_gtk_guest.xpm", NULL);
		else if (ggzcore_player_get_type(p) == GGZ_PLAYER_NORMAL)
			path = g_strjoin(G_DIR_SEPARATOR_S, GGZDATADIR,
					 "ggz_gtk_registered.xpm", NULL);
		else if (ggzcore_player_get_type(p) == GGZ_PLAYER_ADMIN)
			path = g_strjoin(G_DIR_SEPARATOR_S, GGZDATADIR,
					 "ggz_gtk_admin.xpm", NULL);

		if (path) {
			pixmap2 =
				gdk_pixmap_create_from_xpm(tmp->window,
							   &mask2, NULL,
							   path);
			if (pixmap2)
				gtk_clist_set_pixtext
					(GTK_CLIST(tmp), i,
					 PLAYER_LIST_COL_NAME,
					 player[PLAYER_LIST_COL_NAME],
					 5, pixmap2, mask2);
			g_free(path);
		}

		/* Note the name is used right above, so these calls
		   have to come way down here. */
		g_free(player[PLAYER_LIST_COL_TABLE]);
		g_free(player[PLAYER_LIST_COL_NAME]);
	}

	/* "Thaw" the clist (it was "frozen" up above). */
	gtk_clist_thaw(GTK_CLIST(tmp));
}

GtkWidget *create_player_list(GtkWidget * window)
{
	GtkWidget *player_clist;
	GtkWidget *player_lag_label;
	GtkWidget *player_table_label;
	GtkWidget *player_record_label;
	GtkWidget *player_name_label;

	player_clist = gtk_clist_new(4);
	gtk_widget_ref(player_clist);
	gtk_object_set_data_full(GTK_OBJECT(window), "player_clist",
				 player_clist,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(player_clist);
	gtk_widget_set_sensitive(player_clist, FALSE);
	GTK_WIDGET_UNSET_FLAGS(player_clist, GTK_CAN_FOCUS);
	gtk_clist_set_column_width(GTK_CLIST(player_clist), 0, 15);
	gtk_clist_set_column_width(GTK_CLIST(player_clist), 1, 15);
	gtk_clist_set_column_width(GTK_CLIST(player_clist), 2, 35);
	gtk_clist_set_column_width(GTK_CLIST(player_clist), 3, 65);
	gtk_clist_column_titles_show(GTK_CLIST(player_clist));

	player_lag_label = gtk_label_new(_("L"));
	gtk_widget_ref(player_lag_label);
	gtk_object_set_data_full(GTK_OBJECT(window), "player_lag_label",
				 player_lag_label,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(player_lag_label);
	gtk_clist_set_column_widget(GTK_CLIST(player_clist), 0,
				    player_lag_label);

	player_table_label = gtk_label_new(_("T"));
	gtk_widget_ref(player_table_label);
	gtk_object_set_data_full(GTK_OBJECT(window), "player_table_label",
				 player_table_label,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(player_table_label);
	gtk_clist_set_column_widget(GTK_CLIST(player_clist), 1,
				    player_table_label);

	player_record_label = gtk_label_new(_("R"));
	gtk_widget_ref(player_record_label);
	gtk_object_set_data_full(GTK_OBJECT(window), "player_record_label",
				 player_record_label,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(player_record_label);
	gtk_clist_set_column_widget(GTK_CLIST(player_clist), 2,
				    player_record_label);

	player_name_label = gtk_label_new(_("Player"));
	gtk_widget_ref(player_name_label);
	gtk_object_set_data_full(GTK_OBJECT(window), "player_name_label",
				 player_name_label,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(player_name_label);
	gtk_clist_set_column_widget(GTK_CLIST(player_clist), 3,
				    player_name_label);

	gtk_signal_connect(GTK_OBJECT(player_clist), "event",
			   GTK_SIGNAL_FUNC(client_player_clist_event), NULL);
	gtk_signal_connect(GTK_OBJECT(player_clist), "select_row",
			   GTK_SIGNAL_FUNC(client_player_clist_select_row),
			   NULL);

	return player_clist;
}
