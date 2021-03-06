/* 
 * File: launch.c
 * Author: Justin Zaun
 * Project: GGZ GTK Client
 * $Id: launch.c 10939 2009-07-13 19:34:11Z josef $
 *
 * Code for launching games through the GTK client
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

#include <assert.h>
#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <ggz.h>

#include "launch.h"
#include "support.h"
#include "chat.h"
#include "client.h"
#include "game.h"
#include "ggzcore.h"
#include "msgbox.h"
#include "pick_module.h"
#include "server.h"

/* The maximum number of characters in the reserved seat name (not counting
   termination) */
#define MAX_RESERVED_NAME_LEN 16

static void launch_fill_defaults(GtkWidget * widget, gpointer data);
static void launch_seats_changed(GtkWidget * widget, gpointer data);
static void launch_resv_toggle(GtkWidget * widget, gpointer data);
static void launch_start_game(GtkWidget * widget, gpointer data);
static void launch_cancel_button_clicked(GtkWidget * widget, gpointer data);
static void launch_seat_show(gint seat, gchar show);
static GtkWidget *create_dlg_launch(void);

/* Close the launch dialog.  This is done when switching rooms. */
void launch_dialog_close(void)
{
	if (!ggz_gtk.launch_dialog) return;

	/* Free up game we allocated but never launched */
	cancel_module_picking();
	game_destroy();

	gtk_widget_destroy(ggz_gtk.launch_dialog);
	ggz_gtk.launch_dialog = NULL;
}

void launch_create_or_raise(void)
{
	GGZRoom *room;
	GGZGameType *gt;
	int num;

	room = ggzcore_server_get_cur_room(ggz_gtk.server);
	gt = ggzcore_room_get_gametype(room);

	const char *name = ggzcore_gametype_get_name(gt);
	const char *engine = ggzcore_gametype_get_prot_engine(gt);
	const char *version = ggzcore_gametype_get_prot_version(gt);

	/* Check how many modules are registered for this game type */
	ggzcore_reload();
	num = ggzcore_module_get_num_by_type(name, engine, version);

	if (num == 0) {
		gchar *message;
		message = g_strdup_printf(_("You don't have this game "
					    "installed. You can download\n"
					    "it from %s."),
					  ggzcore_gametype_get_url(gt));
		msgbox(message, _("Launch Error"), MSGBOX_OKONLY,
		       MSGBOX_STOP, MSGBOX_NORMAL);
		g_free(message);

		return;
	}

	if (!ggz_gtk.launch_dialog) {
		/* Dialog for setting table seats */
		ggz_gtk.launch_dialog = create_dlg_launch();
		if (ggz_gtk.launch_dialog) {
			gtk_widget_show_all(ggz_gtk.launch_dialog);
		} else {
			/* We shouldn't have gotten this far - menu and
			   buttons should be disabled. */
			ggz_error_msg("Couldn't create launch dialog.");
		}
	} else {
		/* It already exists, so raise it */
		gdk_window_show(ggz_gtk.launch_dialog->window);
		gdk_window_raise(ggz_gtk.launch_dialog->window);
	}
}


static void launch_fill_defaults(GtkWidget * widget, gpointer data)
{
	GtkWidget *tmp;
	gchar *label_text;
	GGZRoom *room;
	GGZGameType *gt;
	gint maxplayers, maxbots, x;

	room = ggzcore_server_get_cur_room(ggz_gtk.server);
	gt = ggzcore_room_get_gametype(room);

	/* Set the labels */
	tmp = g_object_get_data(G_OBJECT(ggz_gtk.launch_dialog),
				"type_label");
	label_text = g_strdup_printf(_("Game Type:  %s"),
				     ggzcore_gametype_get_name(gt));
	gtk_label_set_text(GTK_LABEL(tmp), label_text);
	g_free(label_text);

	tmp = g_object_get_data(G_OBJECT(ggz_gtk.launch_dialog),
				"author_label");
	label_text = g_strdup_printf(_("Author:  %s"),
				     ggzcore_gametype_get_author(gt));
	gtk_label_set_text(GTK_LABEL(tmp), label_text);
	g_free(label_text);

	tmp = g_object_get_data(G_OBJECT(ggz_gtk.launch_dialog),
				  "type_desc_label");
	label_text = g_strdup_printf(_("Description:  %s"),
				     ggzcore_gametype_get_desc(gt));
	gtk_label_set_text(GTK_LABEL(tmp), label_text);
	g_free(label_text);

	tmp = g_object_get_data(G_OBJECT(ggz_gtk.launch_dialog), "web_label");
	label_text = g_strdup_printf(_("Home Page:  %s"),
				     ggzcore_gametype_get_url(gt));
	gtk_label_set_text(GTK_LABEL(tmp), label_text);
	g_free(label_text);


	/* Set the number of players combo */
	maxplayers = ggzcore_gametype_get_max_players(gt);
	tmp = g_object_get_data(G_OBJECT(ggz_gtk.launch_dialog),
				"seats_combo");
	for (x = 1; x <= maxplayers; x++) {
		if (ggzcore_gametype_num_players_is_valid(gt, x)) {
			char buf[128];

			snprintf(buf, sizeof(buf), "%d", x);
			gtk_combo_box_append_text(GTK_COMBO_BOX(tmp), buf);
		}
	}
	gtk_combo_box_set_active(GTK_COMBO_BOX(tmp), 0);

	/* Show seats that should be shown (i.e. all of them). */
	for (x = 1; x <= maxplayers; x++)
		launch_seat_show(x, TRUE);

	/* Default to reserving us a seat */
	tmp = g_object_get_data(G_OBJECT(ggz_gtk.launch_dialog),
				"seat1_name");
	gtk_entry_set_text(GTK_ENTRY(tmp),
			   ggzcore_server_get_handle(ggz_gtk.server));

	/* FIXME: for guest users, the reservation won't work.  But it should
	   be easily converted by ggzd into an open seat, which should be
	   fine. */
	tmp = g_object_get_data(G_OBJECT(ggz_gtk.launch_dialog),
				"seat1_resv");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(tmp), TRUE);

	/* If bots are allowed, default to bot players, otherwise open */
	maxbots = ggzcore_gametype_get_max_bots(gt);

	if (maxbots < maxplayers) {
		tmp = ggz_lookup_widget(ggz_gtk.launch_dialog, "seat1_bot");
		gtk_widget_set_sensitive(GTK_WIDGET(tmp), FALSE);
	}

	for (x = 2; x <= maxplayers; x++) {
		char buf[128];

		if (maxbots >= x - 1) {
			snprintf(buf, sizeof(buf), "seat%d_bot", x);
		} else {
			snprintf(buf, sizeof(buf), "seat%d_bot", x);
			tmp = ggz_lookup_widget(ggz_gtk.launch_dialog, buf);
			gtk_widget_set_sensitive(GTK_WIDGET(tmp), FALSE);

			snprintf(buf, sizeof(buf), "seat%d_open", x);
		}
		tmp = ggz_lookup_widget(ggz_gtk.launch_dialog, buf);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(tmp), TRUE);
	}
}


static void launch_seats_changed(GtkWidget * widget, gpointer data)
{
	gpointer tmp;
	gint i, seats, max;
	GGZGameType *gt;
	GGZRoom *room;

	seats = atoi(gtk_combo_box_get_active_text(GTK_COMBO_BOX(widget)));
	room = ggzcore_server_get_cur_room(ggz_gtk.server);
	gt = ggzcore_room_get_gametype(room);
	max = ggzcore_gametype_get_max_players(gt);

	/* Desensitize seats with numbers bigger than what we want and
	   sensitize others */
	for (i = 1; i <= seats; i++) {
		char text[128];
		snprintf(text, sizeof(text), "seat%d_box", i);
		tmp = g_object_get_data(G_OBJECT(ggz_gtk.launch_dialog),
					text);
		gtk_widget_set_sensitive(tmp, TRUE);
	}
	for (i = (seats + 1); i <= max; i++) {
		char text[128];
		snprintf(text, sizeof(text), "seat%d_box", i);
		tmp = g_object_get_data(G_OBJECT(ggz_gtk.launch_dialog),
					text);
		gtk_widget_set_sensitive(tmp, FALSE);
	}
}


static void launch_resv_toggle(GtkWidget * widget, gpointer data)
{
	if (GTK_TOGGLE_BUTTON(widget)->active) {
		gtk_widget_set_sensitive(GTK_WIDGET(data), TRUE);
		gtk_widget_grab_focus(data);
	} else
		gtk_widget_set_sensitive(GTK_WIDGET(data), FALSE);
}


char launch_in_process(void)
{
	return ggz_gtk.launching;
}

void launch_table(void)
{
	GtkWidget *tmp;
	GGZRoom *room;
	GGZTable *table;
	GGZGameType *gt;
	gint seats;
	int x, status;

	ggz_gtk.launching = FALSE;

	/* Grab the number of seats */
	if (!ggz_gtk.launch_dialog)
		ggz_error_msg("Trying to launch table when "
			      "there is no launch dialog.");
	tmp = ggz_lookup_widget(ggz_gtk.launch_dialog, "seats_combo");
	seats = atoi(gtk_combo_box_get_active_text(GTK_COMBO_BOX(tmp)));

	/* Create a table for sending to the server */
	table = ggzcore_table_new();
	room = ggzcore_server_get_cur_room(ggz_gtk.server);
	gt = ggzcore_room_get_gametype(room);
	tmp = ggz_lookup_widget(ggz_gtk.launch_dialog, "desc_entry");
	ggzcore_table_init(table, gt, gtk_entry_get_text(GTK_ENTRY(tmp)),
			   seats);

	for (x = 0; x < seats; x++) {
		/* Check to see if the seat is a bot. */
		char text[128];
		snprintf(text, sizeof(text), "seat%d_bot", x + 1);
		tmp = ggz_lookup_widget(ggz_gtk.launch_dialog, text);
		if (GTK_TOGGLE_BUTTON(tmp)->active)
			if (ggzcore_table_set_seat(table, x,
						   GGZ_SEAT_BOT, NULL) < 0)
				assert(0);

		/* Check to see if the seat is reserved. */
		snprintf(text, sizeof(text), "seat%d_resv", x + 1);
		tmp = ggz_lookup_widget(ggz_gtk.launch_dialog, text);
		if (GTK_TOGGLE_BUTTON(tmp)->active) {
			const gchar *name;
			snprintf(text, sizeof(text), "seat%d_name", x + 1);
			tmp = ggz_lookup_widget(ggz_gtk.launch_dialog, text);
			name = gtk_entry_get_text(GTK_ENTRY(tmp));

			if (ggzcore_table_set_seat(table, x,
						   GGZ_SEAT_RESERVED,
						   name) < 0)
				assert(0);
		}
	}

	status = ggzcore_room_launch_table(room, table);
	ggzcore_table_free(table);

	if (status < 0) {
		msgbox(_("Failed to launch table.\n Launch aborted."),
		       _("Launch Error"), MSGBOX_OKONLY, MSGBOX_STOP,
		       MSGBOX_NORMAL);
		game_destroy();
	}

	gtk_widget_destroy(ggz_gtk.launch_dialog);
}



static void launch_start_game(GtkWidget * widget, gpointer data)
{
	GtkWidget *tmp;
	GGZRoom *room;
	GGZGameType *gt;
	gint x, seats, bots;

	/* Grab the number of seats */
	tmp = ggz_lookup_widget(ggz_gtk.launch_dialog, "seats_combo");
	seats = atoi(gtk_combo_box_get_active_text(GTK_COMBO_BOX(tmp)));

	/* Let's go bot counting.... */
	bots = 0;
	for (x = 0; x < seats; x++) {
		char text[128];
		snprintf(text, sizeof(text), "seat%d_bot", x + 1);
		tmp = ggz_lookup_widget(ggz_gtk.launch_dialog, text);
		if (GTK_TOGGLE_BUTTON(tmp)->active)
			bots++;
	}

	room = ggzcore_server_get_cur_room(ggz_gtk.server);
	gt = ggzcore_room_get_gametype(room);

	if (!ggzcore_gametype_num_bots_is_valid(gt, bots)) {
		msgbox(_("Invalid number of bots specified"), _("Error"),
		       MSGBOX_OKONLY, MSGBOX_STOP, MSGBOX_NORMAL);

		return;
	}

	/* Create new game object */
	if (game_initialize(0) == 0) {
		if (game_launch() < 0) {
			msgbox(_("Error launching game module."),
			       _("Game Error"), MSGBOX_OKONLY, MSGBOX_INFO,
			       MSGBOX_NORMAL);
			game_destroy();
		} else
			ggz_gtk.launching = TRUE;
	}
}


static void launch_cancel_button_clicked(GtkWidget * widget, gpointer data)
{
	launch_dialog_close();
}


static void launch_seat_show(gint seat, gchar show)
{
	gpointer tmp;
	char text[128];

	/* Show seat's hbox */
	snprintf(text, sizeof(text), "seat%d_box", seat);
	tmp = g_object_get_data(G_OBJECT(ggz_gtk.launch_dialog), text);
	if (show)
		gtk_widget_show(GTK_WIDGET(tmp));
	else
		gtk_widget_hide(GTK_WIDGET(tmp));

}



GtkWidget *create_dlg_launch(void)
{
	GtkWidget *dlg_launch;
	GtkWidget *vbox1;
	GtkWidget *main_box;
	GtkWidget *game_box;
	GtkWidget *seat_box;
	GtkWidget *type_label;
	GtkWidget *game_label;
	GtkWidget *seats_combo;
	GtkWidget *players_label;
	GtkWidget *author_box;
	GtkWidget *author_label;
	GtkWidget *type_desc_box;
	GtkWidget *type_desc_label;
	GtkWidget *web_box;
	GtkWidget *web_label;
	GtkWidget *separator;
	GtkWidget *desc_box;
	GtkWidget *game_desc_label;
	GtkWidget *desc_entry;
	GtkWidget *hbox11;
	GtkWidget *button_box;
	GtkWidget *launch_button;
	GtkWidget *cancel_button;

	GGZRoom *room = ggzcore_server_get_cur_room(ggz_gtk.server);
	GGZGameType *gametype = ggzcore_room_get_gametype(room);
	const int num_seats = ggzcore_gametype_get_max_players(gametype);
	struct {
		GtkWidget *box;
		GtkWidget *label;
		GSList *group;
		GtkWidget *bot;
		GtkWidget *open;
		GtkWidget *resv;
		GtkWidget *name;
	} seats[num_seats];
	int i;

	/* Sanity check.  This can actually fail if GGZ gets confused
	   elsewhere. */
	if (num_seats <= 0) {
		ggz_error_msg("Tried to launch table for invalid game.");
		return NULL;
	}

	dlg_launch = gtk_dialog_new();
	gtk_window_set_transient_for(GTK_WINDOW(dlg_launch),
				     GTK_WINDOW(ggz_gtk.main_window));
	g_object_set_data(G_OBJECT(dlg_launch), "dlg_launch", dlg_launch);
	gtk_window_set_title(GTK_WINDOW(dlg_launch), _("Seat Assignments"));
	gtk_window_set_resizable(GTK_WINDOW(dlg_launch), TRUE);

	vbox1 = GTK_DIALOG(dlg_launch)->vbox;
	g_object_set_data(G_OBJECT(dlg_launch), "vbox1", vbox1);
	gtk_widget_show(vbox1);

	main_box = gtk_vbox_new(FALSE, 3);
	g_object_set_data(G_OBJECT(dlg_launch), "main_box", main_box);
	gtk_box_pack_start(GTK_BOX(vbox1), main_box, TRUE, TRUE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(main_box), 10);

	game_box = gtk_hbox_new(FALSE, 10);
	g_object_set_data(G_OBJECT(dlg_launch), "game_box", game_box);
	gtk_box_pack_start(GTK_BOX(main_box), game_box, FALSE, FALSE, 5);

	type_label = gtk_label_new(_("Game Type:"));
	g_object_set_data(G_OBJECT(dlg_launch), "type_label", type_label);
	gtk_box_pack_start(GTK_BOX(game_box), type_label, FALSE, FALSE, 0);

	game_label = gtk_label_new(NULL);
	g_object_set_data(G_OBJECT(dlg_launch), "game_label", game_label);
	gtk_box_pack_start(GTK_BOX(game_box), game_label, FALSE, FALSE, 0);

	seats_combo = gtk_combo_box_new_text();
	g_object_set_data(G_OBJECT(dlg_launch), "seats_combo", seats_combo);
	gtk_box_pack_end(GTK_BOX(game_box), seats_combo, FALSE, TRUE, 0);

	players_label = gtk_label_new(_("Number of seats"));
	g_object_set_data(G_OBJECT(dlg_launch), "players_label",
			  players_label);
	gtk_box_pack_end(GTK_BOX(game_box), players_label, FALSE, FALSE, 0);

	author_box = gtk_hbox_new(FALSE, 0);
	g_object_set_data(G_OBJECT(dlg_launch), "author_box", author_box);
	gtk_box_pack_start(GTK_BOX(main_box), author_box, FALSE, FALSE, 0);

	author_label = gtk_label_new(_("Author:"));
	g_object_set_data(G_OBJECT(dlg_launch), "author_label",
			  author_label);
	gtk_box_pack_start(GTK_BOX(author_box), author_label, FALSE, FALSE,
			   0);
	gtk_label_set_justify(GTK_LABEL(author_label), GTK_JUSTIFY_LEFT);

	type_desc_box = gtk_hbox_new(FALSE, 0);
	g_object_set_data(G_OBJECT(dlg_launch), "type_desc_box",
			  type_desc_box);
	gtk_box_pack_start(GTK_BOX(main_box), type_desc_box, FALSE, FALSE, 0);

	type_desc_label = gtk_label_new(_("Description:"));
	g_object_set_data(G_OBJECT(dlg_launch), "type_desc_label",
			  type_desc_label);
	gtk_box_pack_start(GTK_BOX(type_desc_box), type_desc_label, FALSE,
			   FALSE, 0);
	gtk_label_set_justify(GTK_LABEL(type_desc_label), GTK_JUSTIFY_LEFT);

	web_box = gtk_hbox_new(FALSE, 0);
	g_object_set_data(G_OBJECT(dlg_launch), "web_box", web_box);
	gtk_box_pack_start(GTK_BOX(main_box), web_box, FALSE, FALSE, 0);

	web_label = gtk_label_new(_("Homepage:"));
	g_object_set_data(G_OBJECT(dlg_launch), "web_label", web_label);
	gtk_box_pack_start(GTK_BOX(web_box), web_label, FALSE, FALSE, 0);
	gtk_label_set_justify(GTK_LABEL(web_label), GTK_JUSTIFY_LEFT);

	separator = gtk_hseparator_new();
	gtk_box_pack_start(GTK_BOX(main_box), separator, FALSE, FALSE, 3);

	if (num_seats > 10) {
		GtkWidget *scroll;

		scroll = gtk_scrolled_window_new(NULL, NULL);
		g_object_set_data(G_OBJECT(dlg_launch),
				       "scrolledwindow", scroll);
		gtk_box_pack_start(GTK_BOX(main_box), scroll, TRUE, TRUE, 0);
		gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
					       GTK_POLICY_AUTOMATIC,
					       GTK_POLICY_AUTOMATIC);

		seat_box = gtk_vbox_new(FALSE, 3);
		g_object_set_data(G_OBJECT(dlg_launch),
				       "seat_box", seat_box);
		gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW
						      (scroll), seat_box);
	} else
		seat_box = main_box;

	memset(seats, 0, sizeof(seats));
	for (i = 0; i < num_seats; i++) {
		char text[128];

		seats[i].box = gtk_hbox_new(FALSE, 0);
		snprintf(text, sizeof(text), "seat%d_box", i + 1);
		g_object_set_data(G_OBJECT(dlg_launch), text,
				  seats[i].box);
		gtk_box_pack_start(GTK_BOX(seat_box), seats[i].box, FALSE,
				   FALSE, 0);

		snprintf(text, sizeof(text), _("Seat %d:"), i + 1);
		seats[i].label = gtk_label_new(text);
		snprintf(text, sizeof(text), "seat%d_label", i + 1);
		g_object_set_data(G_OBJECT(dlg_launch), text,
				  seats[i].label);
		gtk_box_pack_start(GTK_BOX(seats[i].box), seats[i].label,
				   FALSE, FALSE, 0);

		seats[i].bot = gtk_radio_button_new_with_label(seats[i].group,
							       _("Computer"));
		seats[i].group =
			gtk_radio_button_get_group(GTK_RADIO_BUTTON
					       (seats[i].bot));
		snprintf(text, sizeof(text), "seat%d_bot", i + 1);
		g_object_set_data(G_OBJECT(dlg_launch), text, seats[i].bot);
		gtk_box_pack_start(GTK_BOX(seats[i].box), seats[i].bot, FALSE,
				   FALSE, 3);

		seats[i].open =
			gtk_radio_button_new_with_label(seats[i].group,
							_("Open"));
		seats[i].group =
			gtk_radio_button_get_group(GTK_RADIO_BUTTON
					       (seats[i].open));
		snprintf(text, sizeof(text), "seat%d_open", i + 1);
		g_object_set_data(G_OBJECT(dlg_launch), text,
				  seats[i].open);
		gtk_box_pack_start(GTK_BOX(seats[i].box), seats[i].open,
				   FALSE, FALSE, 0);

		seats[i].resv =
			gtk_radio_button_new_with_label(seats[i].group,
							_("Reserved for"));
		seats[i].group =
			gtk_radio_button_get_group(GTK_RADIO_BUTTON
					       (seats[i].resv));
		snprintf(text, sizeof(text), "seat%d_resv", i + 1);
		g_object_set_data(G_OBJECT(dlg_launch), text,
				  seats[i].resv);
		gtk_box_pack_start(GTK_BOX(seats[i].box), seats[i].resv,
				   FALSE, FALSE, 0);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(seats[i].resv),
					     TRUE);

		seats[i].name = gtk_entry_new();
		gtk_entry_set_max_length(GTK_ENTRY(seats[i].name),
					 MAX_RESERVED_NAME_LEN);
		snprintf(text, sizeof(text), "seat%d_name", i + 1);
		g_object_set_data(G_OBJECT(dlg_launch), text,
				  seats[i].name);
		gtk_box_pack_start(GTK_BOX(seats[i].box), seats[i].name,
				   FALSE, TRUE, 0);
	}

	desc_box = gtk_hbox_new(FALSE, 0);
	g_object_set_data(G_OBJECT(dlg_launch), "desc_box", desc_box);
	gtk_box_pack_start(GTK_BOX(main_box), desc_box, FALSE, FALSE, 0);

	game_desc_label = gtk_label_new(_("Game Description   "));
	g_object_set_data(G_OBJECT(dlg_launch), "game_desc_label",
			  game_desc_label);
	gtk_box_pack_start(GTK_BOX(desc_box), game_desc_label, FALSE, FALSE,
			   0);

	desc_entry = gtk_entry_new();
	g_object_set_data(G_OBJECT(dlg_launch), "desc_entry",
			  desc_entry);
	gtk_box_pack_start(GTK_BOX(desc_box), desc_entry, TRUE, TRUE, 0);

	hbox11 = GTK_DIALOG(dlg_launch)->action_area;
	g_object_set_data(G_OBJECT(dlg_launch), "hbox11", hbox11);
	gtk_container_set_border_width(GTK_CONTAINER(hbox11), 10);

	button_box = gtk_hbutton_box_new();
	g_object_set_data(G_OBJECT(dlg_launch), "button_box",
			  button_box);
	gtk_box_pack_start(GTK_BOX(hbox11), button_box, TRUE, TRUE, 0);

	launch_button = gtk_button_new_with_label(_("Launch"));
	g_object_set_data(G_OBJECT(dlg_launch), "launch_button",
			  launch_button);
	gtk_container_add(GTK_CONTAINER(button_box), launch_button);
	GTK_WIDGET_SET_FLAGS(launch_button, GTK_CAN_DEFAULT);

	cancel_button = gtk_button_new_from_stock(GTK_STOCK_CANCEL);
	g_object_set_data(G_OBJECT(dlg_launch), "cancel_button",
			  cancel_button);
	gtk_container_add(GTK_CONTAINER(button_box), cancel_button);
	GTK_WIDGET_SET_FLAGS(cancel_button, GTK_CAN_DEFAULT);

	g_signal_connect(GTK_OBJECT(dlg_launch), "destroy",
			   G_CALLBACK(gtk_widget_destroyed),
			   &ggz_gtk.launch_dialog);
	g_signal_connect(GTK_OBJECT(dlg_launch), "realize",
			   G_CALLBACK(launch_fill_defaults), NULL);
	g_signal_connect(GTK_OBJECT(seats_combo), "changed",
			   G_CALLBACK(launch_seats_changed), NULL);
	for (i = 0; i < num_seats; i++) {
		g_signal_connect(GTK_OBJECT(seats[i].resv), "toggled",
				   G_CALLBACK(launch_resv_toggle),
				   seats[i].name);
	}
	g_signal_connect(GTK_OBJECT(launch_button), "clicked",
			   G_CALLBACK(launch_start_game), NULL);
	g_signal_connect(GTK_OBJECT(cancel_button), "clicked",
			   G_CALLBACK(launch_cancel_button_clicked),
			   NULL);

	return dlg_launch;
}
