/* 
 * File: dlg_players.c
 * Author: Jason Short
 * Project: GGZ GTK Games
 * Date: 10/13/2002 (moved from GGZCards)
 * Desc: Create the "Players" Gtk dialog
 * $Id: dlg_players.c 4917 2002-10-14 22:27:43Z jdorje $
 *
 * Copyright (C) 2002 GGZ Development Team
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
#  include <config.h>		/* Site-specific config */
#endif

#include <assert.h>
#include <gtk/gtk.h>

#include "dlg_players.h"
#include "ggzintl.h"

GGZMod *ggz = NULL;

typedef struct playerlists {
	GtkWidget *this;
	struct playerlists *next;
} PlayerLists;

static PlayerLists *player_lists = NULL;
static GtkWidget *dlg_players = NULL;

static void update_player_clist(GtkWidget * player_clist);
static void update_player_dialog(void);

void update_player_lists(void)
{
	PlayerLists *list;

	update_player_dialog();

	for (list = player_lists; list; list = list->next)
		update_player_clist(list->this);
}

static void handle_ggz_seat_event(GGZMod *ggzmod, GGZModEvent e, void *data)
{
	update_player_lists();
}

void init_player_list(GGZMod *ggzmod)
{
	ggz = ggzmod;
	assert(ggz);
	ggzmod_set_handler(ggzmod, GGZMOD_EVENT_SEAT, handle_ggz_seat_event);
	ggzmod_set_handler(ggzmod, GGZMOD_EVENT_STATE, handle_ggz_seat_event);
}

static void update_player_clist(GtkWidget * player_clist)
{
	int p;
	int num = ggzmod_get_num_seats(ggz);
	gchar *player[4] = { NULL, NULL, NULL, NULL };

	assert(ggz);

	gtk_clist_freeze(GTK_CLIST(player_clist));

	gtk_clist_clear(GTK_CLIST(player_clist));

	for (p = 0; p < num; p++) {
		GGZSeat seat = ggzmod_get_seat(ggz, p);

		player[0] = g_strdup_printf("%d", p);

		switch (seat.type) {
		case GGZ_SEAT_PLAYER:
			player[1] = _("Occupied");
			player[2] = seat.name;
			break;
		case GGZ_SEAT_OPEN:
			player[1] = _("Empty");
			player[2] = "-";
			break;
		case GGZ_SEAT_BOT:
			player[1] = _("Bot");
			player[2] = seat.name;
			break;
		case GGZ_SEAT_RESERVED:
			player[1] = _("Reserved");
			player[2] = seat.name;
			break;
		case GGZ_SEAT_NONE:
			player[1] = _("-");
			player[2] = seat.name;
			break;
		}

		gtk_clist_append(GTK_CLIST(player_clist), player);

		g_free(player[0]);
	}

	gtk_clist_thaw(GTK_CLIST(player_clist));
}

static void update_player_dialog(void)
{
	GtkWidget *player_clist;

	if (!dlg_players)
		return;

	player_clist = gtk_object_get_data(GTK_OBJECT(dlg_players),
					   "player_clist");

	update_player_clist(player_clist);
}

static GtkWidget *create_player_clist(void)
{
	GtkWidget *player_clist;
	GtkWidget *label;

	player_clist = gtk_clist_new(3);
	gtk_widget_ref(player_clist);
	gtk_widget_show(player_clist);

#if 0
	gtk_widget_set_sensitive(player_clist, FALSE);
#endif
	gtk_clist_column_titles_show(GTK_CLIST(player_clist));

	gtk_clist_set_column_width(GTK_CLIST(player_clist), 0, 15);
	gtk_clist_set_column_width(GTK_CLIST(player_clist), 1, 75);
	gtk_clist_set_column_width(GTK_CLIST(player_clist), 2, 80);

	label = gtk_label_new(_("#"));
	gtk_widget_ref(label);
	gtk_object_set_data_full(GTK_OBJECT(player_clist),
				 "player_num_label", label,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label);
	gtk_clist_set_column_widget(GTK_CLIST(player_clist), 0, label);

	label = gtk_label_new(_("Status"));
	gtk_widget_ref(label);
	gtk_object_set_data_full(GTK_OBJECT(player_clist),
				 "player_num_label", label,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label);
	gtk_clist_set_column_widget(GTK_CLIST(player_clist), 1, label);

	label = gtk_label_new(_("Name"));
	gtk_widget_ref(label);
	gtk_object_set_data_full(GTK_OBJECT(player_clist),
				 "player_num_label", label,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label);
	gtk_clist_set_column_widget(GTK_CLIST(player_clist), 2, label);

	update_player_clist(player_clist);

	return player_clist;
}

GtkWidget *create_playerlist_widget(void)
{
	PlayerLists *list = ggz_malloc(sizeof(*list));
	list->this = create_player_clist();
	list->next = player_lists;
	player_lists = list;

	(void) gtk_signal_connect(GTK_OBJECT(list->this),
				  "destroy",
				  GTK_SIGNAL_FUNC(gtk_widget_destroyed),
				  &list->this);

	return list->this;
}

static GtkWidget *create_dlg_players(void)
{
	GtkWidget *dialog;
	GtkWidget *vbox;
	GtkWidget *player_clist;
#if 0
	GtkWidget *label;
#endif
	GtkWidget *action_area;
	GtkWidget *close_button;

	/* 
	 * Create outer window.
	 */
	dialog = gtk_dialog_new();
	gtk_object_set_data(GTK_OBJECT(dialog), "dlg_players", dialog);
	gtk_window_set_title(GTK_WINDOW(dialog), _("Player List"));
	gtk_window_set_policy(GTK_WINDOW(dialog), TRUE, TRUE, FALSE);

	/* 
	 * Get vertical box packing widget.
	 */
	vbox = GTK_DIALOG(dialog)->vbox;
	gtk_object_set_data(GTK_OBJECT(dialog), "vbox", vbox);
	gtk_container_set_border_width(GTK_CONTAINER(vbox), 10);
	gtk_widget_show(vbox);

#if 0
	label = gtk_label_new(_("List of players:"));
	gtk_widget_ref(label);
	gtk_widget_show(label);
	gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 0);
#endif

	player_clist = create_player_clist();
	gtk_object_set_data_full(GTK_OBJECT(dialog),
				 "player_clist", player_clist,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_box_pack_start(GTK_BOX(vbox), player_clist, FALSE, FALSE, 0);


	/* 
	 * Get "action area"
	 */
	action_area = GTK_DIALOG(dialog)->action_area;
	gtk_widget_show(action_area);

	/* 
	 * Make "close" button
	 */
	close_button = gtk_button_new_with_label(_("Close"));
	gtk_widget_ref(close_button);
	gtk_widget_show(close_button);
	gtk_box_pack_start(GTK_BOX(action_area), close_button, FALSE, FALSE,
			   0);
	gtk_widget_set_usize(close_button, 64, -2);
	(void) gtk_signal_connect_object(GTK_OBJECT(close_button), "clicked",
					 GTK_SIGNAL_FUNC(gtk_widget_destroy),
					 GTK_OBJECT(dialog));

	/* 
	 * Set up callbacks
	 */
	(void) gtk_signal_connect_object(GTK_OBJECT(dialog), "delete_event",
					 GTK_SIGNAL_FUNC(gtk_widget_destroy),
					 GTK_OBJECT(dialog));

	/* 
	 * Done!
	 */
	return dialog;
}

void create_or_raise_dlg_players(void)
{
	if (dlg_players != NULL) {
		gdk_window_show(dlg_players->window);
		gdk_window_raise(dlg_players->window);
	} else {
		dlg_players = create_dlg_players();
		(void) gtk_signal_connect(GTK_OBJECT(dlg_players),
					  "destroy",
					  GTK_SIGNAL_FUNC
					  (gtk_widget_destroyed),
					  &dlg_players);
		gtk_widget_show(dlg_players);
	}
}
