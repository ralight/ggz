/* 
 * File: dlg_players.c
 * Author: Jason Short
 * Project: GGZ GTK Games
 * Date: 10/13/2002 (moved from GGZCards)
 * Desc: Create the "Players" Gtk dialog
 * $Id: dlg_players.c 5126 2002-10-31 00:21:09Z jdorje $
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
#include <string.h>

#include "dlg_players.h"
#include "ggzintl.h"

GGZMod *ggz = NULL;

typedef struct playerlists {
	GtkWidget *this;
	struct playerlists *next;
} PlayerLists;

static PlayerLists *player_lists = NULL;
static GtkWidget *dlg_players = NULL;
static int num_entries = 0;

static void update_player_clist(GtkWidget * player_clist);
static void update_player_dialog(void);

static gboolean player_clist_button_event(GtkWidget *widget,
					  GdkEventButton *event,
					  gpointer data);

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
	ggzmod_set_handler(ggzmod, GGZMOD_EVENT_SPECTATOR_SEAT,
			   handle_ggz_seat_event);
	ggzmod_set_handler(ggzmod, GGZMOD_EVENT_STATE, handle_ggz_seat_event);
}

static void update_player_clist(GtkWidget * player_clist)
{
	int p;
	int num;
	gchar *player[4] = { NULL, NULL, NULL, NULL };

	assert(ggz);

	gtk_clist_freeze(GTK_CLIST(player_clist));

	gtk_clist_clear(GTK_CLIST(player_clist));
	num_entries = 0;

	/* Put all players on the list. */
	num = ggzmod_get_num_seats(ggz);
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
		num_entries++;

		g_free(player[0]);
	}

	/* Append any spectators to the list */
	num = ggzmod_get_num_spectator_seats(ggz);
	for (p = 0; p < num; p++) {
		GGZSpectatorSeat seat = ggzmod_get_spectator_seat(ggz, p);

		if (!seat.name)
			continue;

		player[0] = "-";
		player[1] = _("Spectator");
		player[2] = seat.name;

		gtk_clist_append(GTK_CLIST(player_clist), player);
		num_entries++;
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

	/* Set up callbacks */
	gtk_signal_connect(GTK_OBJECT(player_clist), "button_press_event",
			   GTK_SIGNAL_FUNC(player_clist_button_event),
			   player_clist);

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

static gpointer encode_seat(int spectator, int seat_num)
{
	int which = ((!!spectator) << 31) | seat_num;
	assert((seat_num & (1 << 31)) == 0);
	return GINT_TO_POINTER(which);
}

static void decode_seat(gpointer data, int *spectator, int *seat_num)
{
	int which = GPOINTER_TO_INT(data);
	*spectator = which >> 31;
	*seat_num = which & ~(1 << 31);
}

/* Get info on the player (pop up a window) */
static void player_info_activate(GtkMenuItem *menuitem, gpointer data)
{
	int spectator, seat_num;

	decode_seat(data, &spectator, &seat_num);

	/* Not implemented */
}

/* Boot the player from the table */
static void player_boot_activate(GtkMenuItem *menuitem, gpointer data)
{
	int spectator, seat_num;
	char *name;

	decode_seat(data, &spectator, &seat_num);

	if (spectator)
		name = ggzmod_get_spectator_seat(ggz, seat_num).name;
	else
		name = ggzmod_get_seat(ggz, seat_num).name;

	assert(name);
	ggzmod_request_boot(ggz, name);
}

/* We (a spectator) will sit here. */
static void player_sit_activate(GtkMenuItem *menuitem, gpointer data)
{
	int spectator, seat_num;

	decode_seat(data, &spectator, &seat_num);

	assert(!spectator);
	ggzmod_request_sit(ggz, seat_num);
}

/* Replace the open seat with a bot */
static void player_bot_activate(GtkMenuItem *menuitem, gpointer data)
{
	int spectator, seat_num;

	decode_seat(data, &spectator, &seat_num);

	assert(!spectator);
	ggzmod_request_bot(ggz, seat_num);
}

/* Replace the bot or reserved seat with an open one */
static void player_open_activate(GtkMenuItem *menuitem, gpointer data)
{
	int spectator, seat_num;

	decode_seat(data, &spectator, &seat_num);

	assert(!spectator);
	ggzmod_request_open(ggz, seat_num);
}

void popup_player_menu(GGZSeat *seat, GGZSpectatorSeat *sseat, guint button)
{
	GtkWidget *menu;
	gpointer which = encode_seat(sseat ? 1 : 0,
				     seat ? seat->num : sseat->num);
	int is_spectator, my_seat_num;
	const char *my_name;

	my_name = ggzmod_get_player(ggz, &is_spectator, &my_seat_num);

	assert((seat || sseat) && !(seat && sseat));

	menu = gtk_menu_new();

	if (sseat || seat->type == GGZ_SEAT_PLAYER) {
		GtkWidget *info;

		/* FIXME: what about bot/reservation seats? */
		info = gtk_menu_item_new_with_label(_("Info"));
		gtk_widget_ref(info);
		gtk_object_set_data_full(GTK_OBJECT(menu), "info", info,
					 (GtkDestroyNotify) gtk_widget_unref);
		gtk_container_add(GTK_CONTAINER(menu), info);
		gtk_widget_set_sensitive(info, FALSE);
		gtk_signal_connect(GTK_OBJECT(info), "activate",
				   GTK_SIGNAL_FUNC(player_info_activate),
				   which);
	}

	if ((sseat && strcasecmp(sseat->name, my_name))
	     || (seat && seat->type == GGZ_SEAT_PLAYER
		 && strcasecmp(seat->name, my_name))) {
		GtkWidget *boot;

		/* FIXME: you shouldn't be able to boot yourself */
		boot = gtk_menu_item_new_with_label(_("Boot player"));
		gtk_widget_ref(boot);
		gtk_object_set_data_full(GTK_OBJECT(menu), "boot", boot,
					 (GtkDestroyNotify) gtk_widget_unref);
		gtk_container_add(GTK_CONTAINER(menu), boot);
		// gtk_widget_set_sensitive(boot, FALSE);
		gtk_signal_connect(GTK_OBJECT(boot), "activate",
				   GTK_SIGNAL_FUNC(player_boot_activate),
				   which);
	}

	if (seat
	    && (seat->type == GGZ_SEAT_OPEN
		|| (seat->type == GGZ_SEAT_RESERVED
		    && !strcasecmp(my_name, seat->name)))) {
		GtkWidget *sit;
		const char *label;

		if (is_spectator)
			label = _("Sit here");
		else
			label = _("Move here");

		sit = gtk_menu_item_new_with_label(label);
		gtk_widget_ref(sit);
		gtk_object_set_data_full(GTK_OBJECT(menu), "sit", sit,
					 (GtkDestroyNotify) gtk_widget_unref);
		gtk_container_add(GTK_CONTAINER(menu), sit);
		// gtk_widget_set_sensitive(sit, FALSE);
		gtk_signal_connect(GTK_OBJECT(sit), "activate",
				   GTK_SIGNAL_FUNC(player_sit_activate),
				   which);
	}

	if (seat && (seat->type == GGZ_SEAT_OPEN
		     || seat->type == GGZ_SEAT_RESERVED)) {
		GtkWidget *bot;

		bot = gtk_menu_item_new_with_label(_("Play with bot"));
		gtk_widget_ref(bot);
		gtk_object_set_data_full(GTK_OBJECT(menu), "bot", bot,
					 (GtkDestroyNotify) gtk_widget_unref);
		gtk_container_add(GTK_CONTAINER(menu), bot);
		gtk_signal_connect(GTK_OBJECT(bot), "activate",
				   GTK_SIGNAL_FUNC(player_bot_activate),
				   which);
	}

	if (seat && (seat->type == GGZ_SEAT_BOT
		     || seat->type == GGZ_SEAT_RESERVED)) {
		GtkWidget *open;
		const char *label;

		if (seat->type == GGZ_SEAT_RESERVED)
			label = _("Drop reservation");
		else
			label = _("Remove bot");

		open = gtk_menu_item_new_with_label(label);
		gtk_widget_ref(open);
		gtk_object_set_data_full(GTK_OBJECT(menu), "open", open,
					 (GtkDestroyNotify) gtk_widget_unref);
		gtk_container_add(GTK_CONTAINER(menu), open);
		gtk_signal_connect(GTK_OBJECT(open), "activate",
				   GTK_SIGNAL_FUNC(player_open_activate),
				   which);
	}

	gtk_widget_show_all(menu);

	gtk_menu_popup(GTK_MENU(menu), NULL, NULL, NULL, NULL, button, 0);
}

static gboolean player_clist_button_event(GtkWidget *widget,
					  GdkEventButton *buttonevent,
					  gpointer data)
{
	GtkWidget *player_clist = data;

	if (buttonevent->button == 3) {
		/* Right mouse button; create drop-down menu */
		gint row, col;
		int spectator;
		GGZSeat seat;
		GGZSpectatorSeat sseat;

		gtk_clist_get_selection_info(GTK_CLIST(player_clist),
					     buttonevent->x,
					     buttonevent->y,
					     &row, &col);

		/* Without this check, we could select a nonexistent row. */
		if (row < 0 || row >= num_entries)
			return FALSE;

		gtk_clist_select_row(GTK_CLIST(player_clist), row, 0);

		if (row < ggzmod_get_num_seats(ggz)) {
			spectator = 0;
			seat = ggzmod_get_seat(ggz, row);
		} else {
			gchar *name;
			int i, num;
			if (!gtk_clist_get_text(GTK_CLIST(player_clist),
						row, 2, &name))
				assert(0);
			num = ggzmod_get_num_spectator_seats(ggz);
			for (i = 0; i < num; i++) {
				sseat = ggzmod_get_spectator_seat(ggz, i);
				if (!strcasecmp(name, sseat.name))
					break;
			}
			assert(i < num);

			spectator = 1;
		}

		popup_player_menu(spectator ? NULL : &seat,
				  spectator ? &sseat : NULL,
				  buttonevent->button);
		return TRUE;
	}

	return FALSE;
}

void do_sit(void)
{
	ggzmod_request_sit(ggz, -1);
}

void do_stand(void)
{
	ggzmod_request_stand(ggz);
}
