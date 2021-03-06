/*
 * File: playerlist.c
 * Author: GGZ Dev Team
 * Project: GGZ GTK Client
 * Date: 11/03/2002
 * $Id: playerlist.c 10939 2009-07-13 19:34:11Z josef $
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

#include <stdio.h>
#include <string.h>
#include <strings.h> /* For strcasecmp */

#include <gtk/gtk.h>
#include <ggzcore.h>

#include "chat.h"
#include "client.h"
#include "ggzutils.h"
#include "playerinfo.h"
#include "playerlist.h"
#include "server.h"
#include "support.h"

enum {
	PLAYER_COLUMN_TYPE,
	PLAYER_COLUMN_LAG,
	PLAYER_COLUMN_TABLE,
	PLAYER_COLUMN_STATS,
	PLAYER_COLUMN_NAME,
	PLAYER_COLUMNS
};

static GtkWidget *player_list;

static void client_player_info_activate(GtkMenuItem * menuitem, gpointer data)
{
	/* Pop up an info dialog about the player */
	GGZPlayer *player = data;

	player_info_create_or_raise(player);
}

static void client_player_friends_activate(GtkMenuItem * menuitem,
					   gpointer data)
{
	GGZPlayer *player = data;

	if (GTK_CHECK_MENU_ITEM(menuitem)->active)
		chat_add_friend(ggzcore_player_get_name(player), TRUE);
	else
		chat_remove_friend(ggzcore_player_get_name(player));
}

static void client_player_ignore_activate(GtkMenuItem * menuitem,
					  gpointer data)
{
	GGZPlayer *player = data;

	if (GTK_CHECK_MENU_ITEM(menuitem)->active)
		chat_add_ignore(ggzcore_player_get_name(player), TRUE);
	else
		chat_remove_ignore(ggzcore_player_get_name(player));
}

static void client_player_kick_activate(GtkMenuItem * menuitem, gpointer data)
{
	GGZPlayer *player = data;

	ggzcore_room_admin(ggzcore_server_get_cur_room(ggz_gtk.server),
			   GGZ_ADMIN_KICK,
			   ggzcore_player_get_name(player), NULL);
}

static void client_player_gag_activate(GtkMenuItem * menuitem, gpointer data)
{
	GGZPlayer *player = data;

	ggzcore_room_admin(ggzcore_server_get_cur_room(ggz_gtk.server),
			   GGZ_ADMIN_GAG,
			   ggzcore_player_get_name(player), NULL);
}

static void client_player_ungag_activate(GtkMenuItem * menuitem, gpointer data)
{
	GGZPlayer *player = data;

	ggzcore_room_admin(ggzcore_server_get_cur_room(ggz_gtk.server),
			   GGZ_ADMIN_UNGAG,
			   ggzcore_player_get_name(player), NULL);
}

static void client_player_ban_activate(GtkMenuItem * menuitem, gpointer data)
{
	/* TODO */
}

typedef struct {
	GGZPlayer *player;
	GGZPerm perm;
} PermData;

static void permdata_free(gpointer data, GClosure *closure)
{
	g_free(data);
}

static void client_player_perm_activate(GtkMenuItem * menuitem, gpointer data)
{
	PermData *pdata = data;
	gboolean isset
	  = gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(menuitem));

	if (ggzcore_player_set_perm(pdata->player, pdata->perm, isset) < 0) {
		chat_display_local(CHAT_LOCAL_NORMAL, NULL,
				   _("Could not change permission."));
	}
}

static bool is_admin(void)
{
	const char *handle = ggzcore_server_get_handle(ggz_gtk.server);
	const GGZPlayer *player = ggzcore_server_get_player(ggz_gtk.server,
							    handle);

	if (player) {
		GGZPlayerType type = ggzcore_player_get_type(player);

		return (type == GGZ_PLAYER_HOST || type == GGZ_PLAYER_ADMIN);
	} else {
		return FALSE;
	}
}

static GtkWidget *create_mnu_player(GGZPlayer *player, gboolean is_friend,
				    gboolean is_ignore)
{
	GtkWidget *mnu_player;
	GtkWidget *info;
	GtkWidget *separator;
	GtkWidget *friends;
	GtkWidget *ignore;
	GtkWidget *kick;
	GtkWidget *gag;
	GtkWidget *ungag;
	GtkWidget *ban;
	GtkWidget *perms[GGZ_PERM_COUNT];
	GGZPerm p;

	mnu_player = gtk_menu_new();

	info = gtk_menu_item_new_with_label(_("Info"));
	gtk_container_add(GTK_CONTAINER(mnu_player), info);

	separator = gtk_menu_item_new();
	gtk_container_add(GTK_CONTAINER(mnu_player), separator);
	gtk_widget_set_sensitive(separator, FALSE);

	friends = gtk_check_menu_item_new_with_label(_("Friends"));
	gtk_container_add(GTK_CONTAINER(mnu_player), friends);
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(friends),
				       is_friend);

	ignore = gtk_check_menu_item_new_with_label(_("Ignore"));
	gtk_container_add(GTK_CONTAINER(mnu_player), ignore);
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(ignore),
				       is_ignore);

	separator = gtk_menu_item_new();
	gtk_container_add(GTK_CONTAINER(mnu_player), separator);
	gtk_widget_set_sensitive(separator, FALSE);

	kick = gtk_menu_item_new_with_label(_("Kick player"));
	gtk_container_add(GTK_CONTAINER(mnu_player), kick);
	gtk_widget_set_sensitive(kick, is_admin());

	gag = gtk_menu_item_new_with_label(_("Gag player"));
	gtk_container_add(GTK_CONTAINER(mnu_player), gag);
	gtk_widget_set_sensitive(gag, is_admin());

	ungag = gtk_menu_item_new_with_label(_("Ungag player"));
	gtk_container_add(GTK_CONTAINER(mnu_player), ungag);
	gtk_widget_set_sensitive(ungag, is_admin());

	ban = gtk_menu_item_new_with_label(_("Ban player"));
	gtk_container_add(GTK_CONTAINER(mnu_player), ban);
	gtk_widget_set_sensitive(ban, FALSE);

	separator = gtk_menu_item_new();
	gtk_container_add(GTK_CONTAINER(mnu_player), separator);
	gtk_widget_set_sensitive(separator, FALSE);

	for (p = 0; p < GGZ_PERM_COUNT; p++) {
		const char *name = perm_name(p);
		bool isset = ggzcore_player_has_perm(player, p);
		PermData *pdata = g_malloc(sizeof(*pdata));

		if (name == NULL) continue;
		perms[p] = gtk_check_menu_item_new_with_label(name);

		gtk_container_add(GTK_CONTAINER(mnu_player), perms[p]);
		gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(perms[p]),
					       isset);
		gtk_widget_set_sensitive(perms[p], is_admin());

		pdata->player = player;
		pdata->perm = p;

		g_signal_connect_data(GTK_OBJECT(perms[p]), "activate",
			G_CALLBACK(client_player_perm_activate),
			pdata, permdata_free, 0);
	}

	g_signal_connect(GTK_OBJECT(info), "activate",
			 G_CALLBACK(client_player_info_activate),
			 player);
	g_signal_connect(GTK_OBJECT(friends), "activate",
			 G_CALLBACK(client_player_friends_activate),
			 player);
	g_signal_connect(GTK_OBJECT(ignore), "activate",
			 G_CALLBACK(client_player_ignore_activate),
			 player);
	g_signal_connect(GTK_OBJECT(kick), "activate",
			 G_CALLBACK(client_player_kick_activate),
			 player);
	g_signal_connect(GTK_OBJECT(gag), "activate",
			 G_CALLBACK(client_player_gag_activate),
			 player);
	g_signal_connect(GTK_OBJECT(ungag), "activate",
			 G_CALLBACK(client_player_ungag_activate),
			 player);
	g_signal_connect(GTK_OBJECT(ban), "activate",
			 G_CALLBACK(client_player_ban_activate),
			 player);

	return mnu_player;
}

static gboolean player_list_event(GtkWidget *widget,
				  GdkEvent *event, gpointer data)
{
	GdkEventButton *buttonevent = (GdkEventButton *) event;
	GtkTreeView *tree = GTK_TREE_VIEW(widget);
	GtkTreeModel *model = gtk_tree_view_get_model(tree);
	GtkTreeIter iter;
	GGZPlayer *player = NULL;
	char *name;
	GtkTreePath *path = NULL;

	if (!gtk_tree_view_get_path_at_pos(tree,
					   buttonevent->x, buttonevent->y,
					   &path, NULL, NULL, NULL)) {
		return FALSE;
	}
	/* FIXME: must free path variable? */
	gtk_tree_model_get_iter(model, &iter, path);

	gtk_tree_model_get(model, &iter, PLAYER_COLUMN_NAME, &name, -1);
	player = ggzcore_server_get_player(ggz_gtk.server, name);

	if (event->type == GDK_BUTTON_PRESS
	    && buttonevent->button == 3
	    && player) {
		/* Right mouse button:
		 * Create and display the menu */
		int is_friend = chat_is_friend(name);
		int is_ignore = chat_is_ignore(name);
		GtkWidget *menu;

		menu = create_mnu_player(player, is_friend, is_ignore);
		gtk_widget_show_all(menu);
		gtk_menu_popup(GTK_MENU(menu), NULL, NULL, NULL,
			       NULL, buttonevent->button, 0);
	}

	g_free(name);

	return FALSE;
}

void sensitize_player_list(gboolean sensitive)
{
	GtkWidget *tree = player_list;

	gtk_widget_set_sensitive(tree, sensitive);
}

/* Clear current list of players */
void clear_player_list(void)
{
	GtkWidget *store = ggz_lookup_widget(player_list, "player_list_store");

        gtk_list_store_clear(GTK_LIST_STORE(store));
}

#define LAG_CATEGORIES 6
static gboolean pixmaps_initted = FALSE;
static GdkPixbuf *lag[LAG_CATEGORIES], *guest, *registered, *admin, *host, *bot;

void update_player_list(void)
{
	GtkListStore *store;
	gint i, num, l;
	GGZPlayer *p;
	GGZTable *table;
	GGZRoom *room = ggzcore_server_get_cur_room(ggz_gtk.server);
	int wins, losses, ties, forfeits, rating, ranking, highscore;
	char stats[512];

	/* Retrieve the player list widget. */
	store = GTK_LIST_STORE(ggz_lookup_widget(player_list,
					     "player_list_store"));

	/* Clear current list of players */
	gtk_list_store_clear(GTK_LIST_STORE(store));

	/* Display current list of players */
	num = ggzcore_room_get_num_players(room);

	/* Ensure the lag graphics are loaded. */
	if (!pixmaps_initted) {
		for (i = 0; i < LAG_CATEGORIES; i++) {
			gchar name[512];

#define ICON_SIZE 14
			snprintf(name, sizeof(name), "ggz_gtk_lag%d", i);
			lag[i] = load_svg_pixbuf(name, ICON_SIZE, ICON_SIZE);
		}
		guest = load_svg_pixbuf("ggz_gtk_guest", 16, 24);
		registered = load_svg_pixbuf("ggz_gtk_registered", 16, 24);
		admin = load_svg_pixbuf("ggz_gtk_admin", 16, 24);
		host = load_svg_pixbuf("ggz_gtk_host", 16, 24);
		bot = load_svg_pixbuf("ggz_gtk_bot", 16, 24);
		pixmaps_initted = TRUE;
	}

	for (i = 0; i < num; i++) {
		GtkTreeIter iter;
		gchar tabletext[32];

		p = ggzcore_room_get_nth_player(room, i);
		table = ggzcore_player_get_table(p);

		gtk_list_store_append(store, &iter);

		/* Name */
		gtk_list_store_set(store, &iter, PLAYER_COLUMN_NAME,
				   ggzcore_player_get_name(p), -1);

		/* Table # */
		if (!table) {
			snprintf(tabletext, sizeof(tabletext), "--");
		} else {
			snprintf(tabletext, sizeof(tabletext), "%d",
				 ggzcore_table_get_id(table));
		}
		gtk_list_store_set(store, &iter,
				   PLAYER_COLUMN_TABLE, tabletext, -1);

		/* Stats (ranking, etc.) */
		if (ggzcore_player_get_ranking(p, &ranking)) {
			snprintf(stats, sizeof(stats), _("#%d"), ranking);
		} else if (ggzcore_player_get_highscore(p, &highscore)) {
			snprintf(stats, sizeof(stats), "%d", highscore);
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
		} else {
			snprintf(stats, sizeof(stats), "%s", "");
		}
		gtk_list_store_set(store, &iter,
				   PLAYER_COLUMN_STATS, stats, -1);

		/* Lag icon */
		l = ggzcore_player_get_lag(p);
		if (l >= 0 && l < LAG_CATEGORIES && lag[l]) {
			gtk_list_store_set(store, &iter,
					   PLAYER_COLUMN_LAG, lag[l], -1);
		}

		/* "Type" icon */
		switch (ggzcore_player_get_type(p)) {
		case GGZ_PLAYER_GUEST:
			gtk_list_store_set(store, &iter,
					   PLAYER_COLUMN_TYPE, guest, -1);
			break;
		case GGZ_PLAYER_NORMAL:
			gtk_list_store_set(store, &iter,
					   PLAYER_COLUMN_TYPE, registered, -1);
			break;
		case GGZ_PLAYER_HOST:
			gtk_list_store_set(store, &iter,
					   PLAYER_COLUMN_TYPE, host, -1);
			break;
		case GGZ_PLAYER_ADMIN:
			gtk_list_store_set(store, &iter,
					   PLAYER_COLUMN_TYPE, admin, -1);
			break;
		case GGZ_PLAYER_BOT:
			gtk_list_store_set(store, &iter,
					   PLAYER_COLUMN_TYPE, bot, -1);
			break;
		case GGZ_PLAYER_UNKNOWN:

			break;
		}

	}
}

static gint player_sort_func(GtkTreeModel *model,
			   GtkTreeIter *a, GtkTreeIter *b, gpointer data)
{
	char *nameA, *nameB;

	gtk_tree_model_get(model, a, PLAYER_COLUMN_NAME, &nameA, -1);
	gtk_tree_model_get(model, b, PLAYER_COLUMN_NAME, &nameB, -1);

	return (strcasecmp(nameA, nameB));
}

GtkWidget *create_player_list(GtkWidget *parent)
{
	GtkListStore *store;
	GtkWidget *tree;
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;
	GtkTreeSelection *selection;

	store = gtk_list_store_new(PLAYER_COLUMNS,
				   GDK_TYPE_PIXBUF,
				   GDK_TYPE_PIXBUF,
				   G_TYPE_STRING, /* For now */
				   G_TYPE_STRING,
				   G_TYPE_STRING);
	tree = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
	g_object_unref(store);

	gtk_tree_sortable_set_sort_column_id(GTK_TREE_SORTABLE(store),
					     PLAYER_COLUMN_NAME,
					     GTK_SORT_ASCENDING);
	gtk_tree_sortable_set_sort_func(GTK_TREE_SORTABLE(store),
					PLAYER_COLUMN_NAME,
					player_sort_func,
					NULL, NULL);

	renderer = gtk_cell_renderer_pixbuf_new();
	column = gtk_tree_view_column_new_with_attributes("", renderer,
				"pixbuf", PLAYER_COLUMN_TYPE, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(tree), column);

	renderer = gtk_cell_renderer_pixbuf_new();
	column = gtk_tree_view_column_new_with_attributes(_("L"), renderer,
				"pixbuf", PLAYER_COLUMN_LAG, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(tree), column);

	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes(_("T#"), renderer,
				"text", PLAYER_COLUMN_TABLE, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(tree), column);

	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes(_("Stats"),
							  renderer,
				"text", PLAYER_COLUMN_STATS, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(tree), column);

	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes(_("Name"),
							  renderer,
				"text", PLAYER_COLUMN_NAME, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(tree), column);

	g_object_set_data(G_OBJECT(tree), "player_list_store", store);
	g_object_set_data(G_OBJECT(parent), "player_list_store", store);
	gtk_widget_set_sensitive(tree, FALSE);
	GTK_WIDGET_UNSET_FLAGS(tree, GTK_CAN_FOCUS);

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree));
	gtk_tree_selection_set_mode(selection, GTK_SELECTION_SINGLE);

	g_signal_connect(tree, "button-press-event",
			 G_CALLBACK(player_list_event), NULL);

	player_list = tree;
	return tree;
}
