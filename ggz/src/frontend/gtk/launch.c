/*
 * File: launch.c
 * Author: Justin Zaun
 * Project: GGZ GTK Client
 * $Id: launch.c 4405 2002-09-04 18:50:29Z dr_maux $
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

#include <config.h>

#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include <launch.h>
#include <support.h>
#include <chat.h>
#include <game.h>
#include "ggzcore.h"
#include <msgbox.h>

/* The maximum number of characters in the reserved seat name
   (not counting \0) */
#define MAX_RESERVED_NAME_LEN 16

extern GGZServer *server;

static void launch_fill_defaults(GtkWidget *widget, gpointer data);
static void launch_seats_changed(GtkWidget *widget, gpointer data);
static void launch_resv_toggle(GtkWidget *widget, gpointer data);
static void launch_start_game(GtkWidget *widget, gpointer data);
static void launch_cancel_button_clicked(GtkWidget *widget, gpointer data);
static void launch_seat_show(gint seat, gchar show);
static GtkWidget* create_dlg_launch (void);

static GtkWidget *launch_dialog;
static char _launching = 0;

void launch_create_or_raise(void)
{
        if (!launch_dialog) {
		/* Dialog for setting table seats */
		launch_dialog = create_dlg_launch();
		gtk_widget_show(launch_dialog);
        }
        else {
		/* It already exists, so raise it */
                gdk_window_show(launch_dialog->window);
                gdk_window_raise(launch_dialog->window);
        }
}
   

static void launch_fill_defaults(GtkWidget *widget, gpointer data)
{
	GtkWidget *tmp;
	gchar *text, *seatstring;
	GGZRoom *room;
	GGZGameType *gt;
	GList *items = NULL;
	gint maxplayers, maxbots, x;

	room  = ggzcore_server_get_cur_room(server);
	gt = ggzcore_room_get_gametype(room);

	/* Set the labels */
        tmp = gtk_object_get_data(GTK_OBJECT(launch_dialog), "type_label");
	text = g_strdup_printf(_("Game Type:  %s"), ggzcore_gametype_get_name(gt));
	gtk_label_set_text(GTK_LABEL(tmp), text);
	g_free(text);

        tmp = gtk_object_get_data(GTK_OBJECT(launch_dialog), "author_label");
	text = g_strdup_printf(_("Author:  %s"), ggzcore_gametype_get_author(gt));
	gtk_label_set_text(GTK_LABEL(tmp), text);
	g_free(text);

        tmp = gtk_object_get_data(GTK_OBJECT(launch_dialog), "type_desc_label");
	text = g_strdup_printf(_("Description:  %s"), ggzcore_gametype_get_desc(gt));
	gtk_label_set_text(GTK_LABEL(tmp), text);
	g_free(text);

        tmp = gtk_object_get_data(GTK_OBJECT(launch_dialog), "web_label");
	text = g_strdup_printf(_("Home Page:  %s"), ggzcore_gametype_get_url(gt));
	gtk_label_set_text(GTK_LABEL(tmp), text);
	g_free(text);


	/* Set the number of players combo */
	maxplayers = ggzcore_gametype_get_max_players(gt);
	for (x=1; x <= maxplayers; x++)
	{
		if (ggzcore_gametype_num_players_is_valid(gt, x) == TRUE)
			items = g_list_append(items, g_strdup_printf("%d", x));
	}
	tmp = gtk_object_get_data(GTK_OBJECT(launch_dialog), "seats_combo");
	gtk_combo_set_popdown_strings(GTK_COMBO(tmp), items);

	/* Hide seats that should be hidden and show seats that should be shown */
	for (x = 1; x <= maxplayers; x++)
		launch_seat_show(x, TRUE);
	for (x = maxplayers + 1; x <= 8; x++)
		launch_seat_show(x, FALSE);

	/* Default to reserving us a seat */
	tmp = gtk_object_get_data(GTK_OBJECT(launch_dialog), "seat1_name");
	gtk_entry_set_text(GTK_ENTRY(tmp), ggzcore_server_get_handle(server));

	/* FIXME: once reservations work, default to resv*/
	tmp = gtk_object_get_data(GTK_OBJECT(launch_dialog), "seat1_open");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(tmp), TRUE);

	/* If bots are allowed, default to bot players, otherwise open */
	maxbots = ggzcore_gametype_get_max_bots(gt);
	if (maxbots > 0) 
		seatstring = "seat%d_bot";
	else
		seatstring = "seat%d_open";

	for (x = 2; x <= maxplayers; x++) {
		text = g_strdup_printf(seatstring, x);
		tmp = lookup_widget(launch_dialog, text);
		g_free(text);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(tmp), TRUE);
	}
}


static void launch_seats_changed(GtkWidget *widget, gpointer data)
{
	gpointer tmp;
	gint i, seats, max;
	gchar* buf;
	GGZGameType *gt;
	GGZRoom *room;

        seats = atoi(gtk_entry_get_text(GTK_ENTRY(widget)));
	room  = ggzcore_server_get_cur_room(server);
        gt = ggzcore_room_get_gametype(room);
        max = ggzcore_gametype_get_max_players(gt);

        /* Desensitize seats with numbers bigger than what we want
           and sensitize others */
        for (i = 1; i <= seats; i++) {
                buf = g_strdup_printf("seat%d_box", i);
                tmp = gtk_object_get_data(GTK_OBJECT(launch_dialog), buf);
                g_free(buf);
                gtk_widget_set_sensitive(tmp, TRUE);
        }
        for (i = (seats + 1); i <= max; i++){
                buf = g_strdup_printf("seat%d_box", i);
                tmp = gtk_object_get_data(GTK_OBJECT(launch_dialog), buf);
                g_free(buf);
                gtk_widget_set_sensitive(tmp, FALSE);
        }
}


static void launch_resv_toggle(GtkWidget *widget, gpointer data)
{
        if (GTK_TOGGLE_BUTTON(widget)->active) {
                gtk_widget_set_sensitive(GTK_WIDGET(data), TRUE);
                gtk_widget_grab_focus(data);
        } else
                gtk_widget_set_sensitive(GTK_WIDGET(data), FALSE);
}


char launch_in_process(void)
{
	return _launching;
}

void launch_table(void)
{
	GtkWidget *tmp;
	GGZRoom *room;
	GGZTable *table;
	GGZGameType *gt;
	gint seats;
	int x, status;
	gchar *widget_name;
	
	_launching = 0;

	/* Grab the number of seats */
	tmp = lookup_widget(launch_dialog, "seats_combo");
	seats = atoi(gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(tmp)->entry)));
	
	/* Create a table for sending to the server */
	table = ggzcore_table_new();
	room = ggzcore_server_get_cur_room(server);
	gt = ggzcore_room_get_gametype(room);
	tmp = lookup_widget(launch_dialog, "desc_entry");
	ggzcore_table_init(table, gt, gtk_entry_get_text(GTK_ENTRY(tmp)), seats);

	for( x = 0; x < seats; x++ )
	{
		/* Check to see if the seat is a bot. */
                widget_name = g_strdup_printf("seat%d_bot", (x+1));
		tmp = lookup_widget(launch_dialog, widget_name);
		g_free(widget_name);
		if (GTK_TOGGLE_BUTTON(tmp)->active)
			if (ggzcore_table_set_seat(table, x,
						   GGZ_SEAT_BOT, NULL) < 0)
				assert(0);
			
		/* Check to see if the seat is reserved. */
		widget_name = g_strdup_printf("seat%d_resv", (x+1));
		tmp = lookup_widget(launch_dialog, widget_name);
		g_free(widget_name);
		if (GTK_TOGGLE_BUTTON(tmp)->active) {
			gchar *name;
			widget_name = g_strdup_printf("seat%d_name", (x+1));
			tmp = lookup_widget(launch_dialog, widget_name);
			g_free(widget_name);
			name = gtk_entry_get_text(GTK_ENTRY(tmp));
			
			if (ggzcore_table_set_seat(table, x,
						   GGZ_SEAT_RESERVED, name) < 0)
				assert(0);
		}
	}

	status = ggzcore_room_launch_table(room, table);
	ggzcore_table_free(table);

	if (status < 0) {
		msgbox(_("Failed to launch table.\n Launch aborted."), _("Launch Error"), MSGBOX_OKONLY, MSGBOX_STOP, MSGBOX_NORMAL);
		game_destroy();
	}

	gtk_widget_destroy(launch_dialog);
}



static void launch_start_game(GtkWidget *widget, gpointer data)
{
	GtkWidget *tmp;
	GGZRoom *room;
	GGZGameType *gt;
	gint x, seats, bots;
	gchar *widget_name;

	/* Grab the number of seats */
	tmp = lookup_widget(launch_dialog, "seats_combo");
	seats = atoi(gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(tmp)->entry)));

	/* Let's go bot counting....*/
	bots = 0;
	for( x = 0; x < seats; x++ )
	{
                widget_name = g_strdup_printf("seat%d_bot", (x+1));
		tmp = lookup_widget(launch_dialog, widget_name);
		g_free(widget_name);
		if (GTK_TOGGLE_BUTTON(tmp)->active)
			bots++;
	}

	room = ggzcore_server_get_cur_room(server);
	gt = ggzcore_room_get_gametype(room);

	if (! ggzcore_gametype_num_bots_is_valid(gt, bots)) {
		msgbox(_("Invalid number of bots specified"), _("Error"), MSGBOX_OKONLY, MSGBOX_STOP, MSGBOX_NORMAL);
		
		return;
	}

	/* Create new game object */
	if (game_init(0) == 0) {
		if (game_launch() < 0) {
			msgbox(_("Error launching game module."), _("Game Error"), MSGBOX_OKONLY, MSGBOX_INFO, MSGBOX_NORMAL);
			game_destroy();
		}
		else 
			_launching = 1;
	}
}


static void launch_cancel_button_clicked(GtkWidget *widget, gpointer data)
{
	/* Free up game we allocated but never launched */
	game_destroy();

	gtk_widget_destroy(launch_dialog);
	launch_dialog = NULL;
}


static void launch_seat_show(gint seat, gchar show)
{
        gpointer tmp;
        gchar* widget;
                
        /* Show seat's hbox */
        widget = g_strdup_printf("seat%d_box", seat);
        tmp = GTK_OBJECT(gtk_object_get_data(GTK_OBJECT(launch_dialog), widget));
        if (show)
                gtk_widget_show(GTK_WIDGET(tmp));
        else
                gtk_widget_hide(GTK_WIDGET(tmp));
 
}



GtkWidget*
create_dlg_launch (void)
{
  GtkWidget *dlg_launch;
  GtkWidget *vbox1;
  GtkWidget *main_box;
  GtkWidget *seats_box;
  GtkWidget *type_label;
  GtkWidget *game_label;
  GtkWidget *seats_combo;
  GList *seats_combo_items = NULL;
  GtkWidget *seats_entry;
  GtkWidget *players_label;
  GtkWidget *author_box;
  GtkWidget *author_label;
  GtkWidget *type_desc_box;
  GtkWidget *type_desc_label;
  GtkWidget *web_box;
  GtkWidget *web_label;
  GtkWidget *separator;
  GtkWidget *seat1_box;
  GtkWidget *seat1_label;
  GSList *seat1_group = NULL;
  GtkWidget *seat1_bot;
  GtkWidget *seat1_open;
  GtkWidget *seat1_resv;
  GtkWidget *seat1_name;
  GtkWidget *seat2_box;
  GtkWidget *seat2_label;
  GSList *seat2_group = NULL;
  GtkWidget *seat2_bot;
  GtkWidget *seat2_open;
  GtkWidget *seat2_resv;
  GtkWidget *seat2_name;
  GtkWidget *seat3_box;
  GtkWidget *seat3_label;
  GSList *seat3_group = NULL;
  GtkWidget *seat3_bot;
  GtkWidget *seat3_open;
  GtkWidget *seat3_resv;
  GtkWidget *seat3_name;
  GtkWidget *seat4_box;
  GtkWidget *seat4_label;
  GSList *seat4_group = NULL;
  GtkWidget *seat4_bot;
  GtkWidget *seat4_open;
  GtkWidget *seat4_resv;
  GtkWidget *seat4_name;
  GtkWidget *seat5_box;
  GtkWidget *seat5_label;
  GSList *seat5_group = NULL;
  GtkWidget *seat5_bot;
  GtkWidget *seat5_open;
  GtkWidget *seat5_resv;
  GtkWidget *seat5_name;
  GtkWidget *seat6_box;
  GtkWidget *seat6_label;
  GSList *seat6_group = NULL;
  GtkWidget *seat6_bot;
  GtkWidget *seat6_open;
  GtkWidget *seat6_resv;
  GtkWidget *seat6_name;
  GtkWidget *seat7_box;
  GtkWidget *seat7_label;
  GSList *seat7_group = NULL;
  GtkWidget *seat7_bot;
  GtkWidget *seat7_open;
  GtkWidget *seat7_resv;
  GtkWidget *seat7_name;
  GtkWidget *seat8_box;
  GtkWidget *seat8_label;
  GSList *seat8_group = NULL;
  GtkWidget *seat8_bot;
  GtkWidget *seat8_open;
  GtkWidget *seat8_resv;
  GtkWidget *seat8_name;
  GtkWidget *desc_box;
  GtkWidget *game_desc_label;
  GtkWidget *desc_entry;
  GtkWidget *hbox11;
  GtkWidget *button_box;
  GtkWidget *launch_button;
  GtkWidget *cancel_button;

  dlg_launch = gtk_dialog_new ();
  gtk_object_set_data (GTK_OBJECT (dlg_launch), "dlg_launch", dlg_launch);
  gtk_window_set_title (GTK_WINDOW (dlg_launch), _("Seat Assignments"));
  GTK_WINDOW (dlg_launch)->type = GTK_WINDOW_DIALOG;
  gtk_window_set_policy (GTK_WINDOW (dlg_launch), FALSE, TRUE, TRUE);

  vbox1 = GTK_DIALOG (dlg_launch)->vbox;
  gtk_object_set_data (GTK_OBJECT (dlg_launch), "vbox1", vbox1);
  gtk_widget_show (vbox1);

  main_box = gtk_vbox_new (FALSE, 3);
  gtk_widget_ref (main_box);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "main_box", main_box,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (main_box);
  gtk_box_pack_start (GTK_BOX (vbox1), main_box, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (main_box), 10);

  seats_box = gtk_hbox_new (FALSE, 10);
  gtk_widget_ref (seats_box);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "seats_box", seats_box,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (seats_box);
  gtk_box_pack_start (GTK_BOX (main_box), seats_box, TRUE, TRUE, 5);

  type_label = gtk_label_new (_("Game Type:"));
  gtk_widget_ref (type_label);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "type_label", type_label,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (type_label);
  gtk_box_pack_start (GTK_BOX (seats_box), type_label, FALSE, FALSE, 0);

  game_label = gtk_label_new ("");
  gtk_widget_ref (game_label);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "game_label", game_label,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (game_label);
  gtk_box_pack_start (GTK_BOX (seats_box), game_label, FALSE, FALSE, 0);

  seats_combo = gtk_combo_new ();
  gtk_widget_ref (seats_combo);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "seats_combo", seats_combo,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (seats_combo);
  gtk_box_pack_end (GTK_BOX (seats_box), seats_combo, FALSE, TRUE, 0);
  gtk_widget_set_usize (seats_combo, 50, -2);
  gtk_combo_set_value_in_list (GTK_COMBO (seats_combo), TRUE, FALSE);
  seats_combo_items = g_list_append (seats_combo_items, (gpointer) "");
  gtk_combo_set_popdown_strings (GTK_COMBO (seats_combo), seats_combo_items);
  g_list_free (seats_combo_items);

  seats_entry = GTK_COMBO (seats_combo)->entry;
  gtk_widget_ref (seats_entry);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "seats_entry", seats_entry,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (seats_entry);

  players_label = gtk_label_new (_("Number of seats"));
  gtk_widget_ref (players_label);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "players_label", players_label,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (players_label);
  gtk_box_pack_end (GTK_BOX (seats_box), players_label, FALSE, FALSE, 0);

  author_box = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (author_box);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "author_box", author_box,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (author_box);
  gtk_box_pack_start (GTK_BOX (main_box), author_box, FALSE, FALSE, 0);

  author_label = gtk_label_new (_("Author:"));
  gtk_widget_ref (author_label);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "author_label", author_label,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (author_label);
  gtk_box_pack_start (GTK_BOX (author_box), author_label, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (author_label), GTK_JUSTIFY_LEFT);

  type_desc_box = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (type_desc_box);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "type_desc_box", type_desc_box,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (type_desc_box);
  gtk_box_pack_start (GTK_BOX (main_box), type_desc_box, FALSE, FALSE, 0);

  type_desc_label = gtk_label_new (_("Description:"));
  gtk_widget_ref (type_desc_label);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "type_desc_label", type_desc_label,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (type_desc_label);
  gtk_box_pack_start (GTK_BOX (type_desc_box), type_desc_label, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (type_desc_label), GTK_JUSTIFY_LEFT);

  web_box = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (web_box);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "web_box", web_box,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (web_box);
  gtk_box_pack_start (GTK_BOX (main_box), web_box, FALSE, FALSE, 0);

  web_label = gtk_label_new (_("Homepage:"));
  gtk_widget_ref (web_label);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "web_label", web_label,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (web_label);
  gtk_box_pack_start (GTK_BOX (web_box), web_label, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (web_label), GTK_JUSTIFY_LEFT);

  separator = gtk_hseparator_new ();
  gtk_widget_ref (separator);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "separator", separator,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (separator);
  gtk_box_pack_start (GTK_BOX (main_box), separator, TRUE, TRUE, 3);

  seat1_box = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (seat1_box);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "seat1_box", seat1_box,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (seat1_box);
  gtk_box_pack_start (GTK_BOX (main_box), seat1_box, FALSE, FALSE, 0);

  seat1_label = gtk_label_new (_("Seat 1:"));
  gtk_widget_ref (seat1_label);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "seat1_label", seat1_label,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (seat1_label);
  gtk_box_pack_start (GTK_BOX (seat1_box), seat1_label, FALSE, FALSE, 0);

  seat1_bot = gtk_radio_button_new_with_label (seat1_group, _("Computer"));
  seat1_group = gtk_radio_button_group (GTK_RADIO_BUTTON (seat1_bot));
  gtk_widget_ref (seat1_bot);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "seat1_bot", seat1_bot,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (seat1_bot);
  gtk_box_pack_start (GTK_BOX (seat1_box), seat1_bot, FALSE, FALSE, 3);

  seat1_open = gtk_radio_button_new_with_label (seat1_group, _("Open"));
  seat1_group = gtk_radio_button_group (GTK_RADIO_BUTTON (seat1_open));
  gtk_widget_ref (seat1_open);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "seat1_open", seat1_open,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (seat1_open);
  gtk_box_pack_start (GTK_BOX (seat1_box), seat1_open, FALSE, FALSE, 0);

  seat1_resv = gtk_radio_button_new_with_label (seat1_group, _("Reserved for"));
  seat1_group = gtk_radio_button_group (GTK_RADIO_BUTTON (seat1_resv));
  gtk_widget_ref (seat1_resv);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "seat1_resv", seat1_resv,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (seat1_resv);
  gtk_box_pack_start (GTK_BOX (seat1_box), seat1_resv, FALSE, FALSE, 0);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (seat1_resv), TRUE);

  seat1_name = gtk_entry_new_with_max_length (MAX_RESERVED_NAME_LEN);
  gtk_widget_ref (seat1_name);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "seat1_name", seat1_name,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (seat1_name);
  gtk_box_pack_start (GTK_BOX (seat1_box), seat1_name, FALSE, TRUE, 0);
  /* gtk_widget_set_usize (seat1_name, 100, -2); */

  seat2_box = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (seat2_box);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "seat2_box", seat2_box,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (seat2_box);
  gtk_box_pack_start (GTK_BOX (main_box), seat2_box, TRUE, TRUE, 0);

  seat2_label = gtk_label_new (_("Seat 2:"));
  gtk_widget_ref (seat2_label);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "seat2_label", seat2_label,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (seat2_label);
  gtk_box_pack_start (GTK_BOX (seat2_box), seat2_label, FALSE, FALSE, 0);

  seat2_bot = gtk_radio_button_new_with_label (seat2_group, _("Computer"));
  seat2_group = gtk_radio_button_group (GTK_RADIO_BUTTON (seat2_bot));
  gtk_widget_ref (seat2_bot);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "seat2_bot", seat2_bot,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (seat2_bot);
  gtk_box_pack_start (GTK_BOX (seat2_box), seat2_bot, FALSE, FALSE, 3);

  seat2_open = gtk_radio_button_new_with_label (seat2_group, _("Open"));
  seat2_group = gtk_radio_button_group (GTK_RADIO_BUTTON (seat2_open));
  gtk_widget_ref (seat2_open);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "seat2_open", seat2_open,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (seat2_open);
  gtk_box_pack_start (GTK_BOX (seat2_box), seat2_open, FALSE, FALSE, 0);

  seat2_resv = gtk_radio_button_new_with_label (seat2_group, _("Reserved for"));
  seat2_group = gtk_radio_button_group (GTK_RADIO_BUTTON (seat2_resv));
  gtk_widget_ref (seat2_resv);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "seat2_resv", seat2_resv,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (seat2_resv);
  gtk_box_pack_start (GTK_BOX (seat2_box), seat2_resv, FALSE, FALSE, 0);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (seat2_resv), TRUE);

  seat2_name = gtk_entry_new_with_max_length (MAX_RESERVED_NAME_LEN);
  gtk_widget_ref (seat2_name);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "seat2_name", seat2_name,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (seat2_name);
  gtk_box_pack_start (GTK_BOX (seat2_box), seat2_name, TRUE, TRUE, 0);
  /* gtk_widget_set_usize (seat2_name, 100, -2); */

  seat3_box = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (seat3_box);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "seat3_box", seat3_box,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (seat3_box);
  gtk_box_pack_start (GTK_BOX (main_box), seat3_box, TRUE, TRUE, 0);

  seat3_label = gtk_label_new (_("Seat 3:"));
  gtk_widget_ref (seat3_label);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "seat3_label", seat3_label,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (seat3_label);
  gtk_box_pack_start (GTK_BOX (seat3_box), seat3_label, FALSE, FALSE, 0);

  seat3_bot = gtk_radio_button_new_with_label (seat3_group, _("Computer"));
  seat3_group = gtk_radio_button_group (GTK_RADIO_BUTTON (seat3_bot));
  gtk_widget_ref (seat3_bot);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "seat3_bot", seat3_bot,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (seat3_bot);
  gtk_box_pack_start (GTK_BOX (seat3_box), seat3_bot, FALSE, FALSE, 3);

  seat3_open = gtk_radio_button_new_with_label (seat3_group, _("Open"));
  seat3_group = gtk_radio_button_group (GTK_RADIO_BUTTON (seat3_open));
  gtk_widget_ref (seat3_open);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "seat3_open", seat3_open,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (seat3_open);
  gtk_box_pack_start (GTK_BOX (seat3_box), seat3_open, FALSE, FALSE, 0);

  seat3_resv = gtk_radio_button_new_with_label (seat3_group, _("Reserved for"));
  seat3_group = gtk_radio_button_group (GTK_RADIO_BUTTON (seat3_resv));
  gtk_widget_ref (seat3_resv);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "seat3_resv", seat3_resv,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (seat3_resv);
  gtk_box_pack_start (GTK_BOX (seat3_box), seat3_resv, FALSE, FALSE, 0);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (seat3_resv), TRUE);

  seat3_name = gtk_entry_new_with_max_length (MAX_RESERVED_NAME_LEN);
  gtk_widget_ref (seat3_name);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "seat3_name", seat3_name,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (seat3_name);
  gtk_box_pack_start (GTK_BOX (seat3_box), seat3_name, TRUE, TRUE, 0);
  /* gtk_widget_set_usize (seat3_name, 100, -2); */

  seat4_box = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (seat4_box);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "seat4_box", seat4_box,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (seat4_box);
  gtk_box_pack_start (GTK_BOX (main_box), seat4_box, TRUE, TRUE, 0);

  seat4_label = gtk_label_new (_("Seat 4:"));
  gtk_widget_ref (seat4_label);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "seat4_label", seat4_label,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (seat4_label);
  gtk_box_pack_start (GTK_BOX (seat4_box), seat4_label, FALSE, FALSE, 0);

  seat4_bot = gtk_radio_button_new_with_label (seat4_group, _("Computer"));
  seat4_group = gtk_radio_button_group (GTK_RADIO_BUTTON (seat4_bot));
  gtk_widget_ref (seat4_bot);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "seat4_bot", seat4_bot,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (seat4_bot);
  gtk_box_pack_start (GTK_BOX (seat4_box), seat4_bot, FALSE, FALSE, 3);

  seat4_open = gtk_radio_button_new_with_label (seat4_group, _("Open"));
  seat4_group = gtk_radio_button_group (GTK_RADIO_BUTTON (seat4_open));
  gtk_widget_ref (seat4_open);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "seat4_open", seat4_open,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (seat4_open);
  gtk_box_pack_start (GTK_BOX (seat4_box), seat4_open, FALSE, FALSE, 0);

  seat4_resv = gtk_radio_button_new_with_label (seat4_group, _("Reserved for"));
  seat4_group = gtk_radio_button_group (GTK_RADIO_BUTTON (seat4_resv));
  gtk_widget_ref (seat4_resv);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "seat4_resv", seat4_resv,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (seat4_resv);
  gtk_box_pack_start (GTK_BOX (seat4_box), seat4_resv, FALSE, FALSE, 0);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (seat4_resv), TRUE);

  seat4_name = gtk_entry_new_with_max_length (MAX_RESERVED_NAME_LEN);
  gtk_widget_ref (seat4_name);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "seat4_name", seat4_name,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (seat4_name);
  gtk_box_pack_start (GTK_BOX (seat4_box), seat4_name, TRUE, TRUE, 0);
  /* gtk_widget_set_usize (seat4_name, 100, -2); */

  seat5_box = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (seat5_box);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "seat5_box", seat5_box,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (seat5_box);
  gtk_box_pack_start (GTK_BOX (main_box), seat5_box, TRUE, TRUE, 0);

  seat5_label = gtk_label_new (_("Seat 5:"));
  gtk_widget_ref (seat5_label);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "seat5_label", seat5_label,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (seat5_label);
  gtk_box_pack_start (GTK_BOX (seat5_box), seat5_label, FALSE, FALSE, 0);

  seat5_bot = gtk_radio_button_new_with_label (seat5_group, _("Computer"));
  seat5_group = gtk_radio_button_group (GTK_RADIO_BUTTON (seat5_bot));
  gtk_widget_ref (seat5_bot);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "seat5_bot", seat5_bot,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (seat5_bot);
  gtk_box_pack_start (GTK_BOX (seat5_box), seat5_bot, FALSE, FALSE, 3);

  seat5_open = gtk_radio_button_new_with_label (seat5_group, _("Open"));
  seat5_group = gtk_radio_button_group (GTK_RADIO_BUTTON (seat5_open));
  gtk_widget_ref (seat5_open);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "seat5_open", seat5_open,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (seat5_open);
  gtk_box_pack_start (GTK_BOX (seat5_box), seat5_open, FALSE, FALSE, 0);

  seat5_resv = gtk_radio_button_new_with_label (seat5_group, _("Reserved for"));
  seat5_group = gtk_radio_button_group (GTK_RADIO_BUTTON (seat5_resv));
  gtk_widget_ref (seat5_resv);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "seat5_resv", seat5_resv,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (seat5_resv);
  gtk_box_pack_start (GTK_BOX (seat5_box), seat5_resv, FALSE, FALSE, 0);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (seat5_resv), TRUE);

  seat5_name = gtk_entry_new_with_max_length (MAX_RESERVED_NAME_LEN);
  gtk_widget_ref (seat5_name);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "seat5_name", seat5_name,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (seat5_name);
  gtk_box_pack_start (GTK_BOX (seat5_box), seat5_name, TRUE, TRUE, 0);
  /* gtk_widget_set_usize (seat5_name, 100, -2); */

  seat6_box = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (seat6_box);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "seat6_box", seat6_box,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (seat6_box);
  gtk_box_pack_start (GTK_BOX (main_box), seat6_box, TRUE, TRUE, 0);

  seat6_label = gtk_label_new (_("Seat 6:"));
  gtk_widget_ref (seat6_label);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "seat6_label", seat6_label,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (seat6_label);
  gtk_box_pack_start (GTK_BOX (seat6_box), seat6_label, FALSE, FALSE, 0);

  seat6_bot = gtk_radio_button_new_with_label (seat6_group, _("Computer"));
  seat6_group = gtk_radio_button_group (GTK_RADIO_BUTTON (seat6_bot));
  gtk_widget_ref (seat6_bot);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "seat6_bot", seat6_bot,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (seat6_bot);
  gtk_box_pack_start (GTK_BOX (seat6_box), seat6_bot, FALSE, FALSE, 3);

  seat6_open = gtk_radio_button_new_with_label (seat6_group, _("Open"));
  seat6_group = gtk_radio_button_group (GTK_RADIO_BUTTON (seat6_open));
  gtk_widget_ref (seat6_open);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "seat6_open", seat6_open,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (seat6_open);
  gtk_box_pack_start (GTK_BOX (seat6_box), seat6_open, FALSE, FALSE, 0);

  seat6_resv = gtk_radio_button_new_with_label (seat6_group, _("Reserved for"));
  seat6_group = gtk_radio_button_group (GTK_RADIO_BUTTON (seat6_resv));
  gtk_widget_ref (seat6_resv);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "seat6_resv", seat6_resv,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (seat6_resv);
  gtk_box_pack_start (GTK_BOX (seat6_box), seat6_resv, FALSE, FALSE, 0);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (seat6_resv), TRUE);

  seat6_name = gtk_entry_new_with_max_length (MAX_RESERVED_NAME_LEN);
  gtk_widget_ref (seat6_name);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "seat6_name", seat6_name,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (seat6_name);
  gtk_box_pack_start (GTK_BOX (seat6_box), seat6_name, TRUE, TRUE, 0);
  /* gtk_widget_set_usize (seat6_name, 100, -2); */

  seat7_box = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (seat7_box);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "seat7_box", seat7_box,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (seat7_box);
  gtk_box_pack_start (GTK_BOX (main_box), seat7_box, TRUE, TRUE, 0);

  seat7_label = gtk_label_new (_("Seat 7:"));
  gtk_widget_ref (seat7_label);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "seat7_label", seat7_label,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (seat7_label);
  gtk_box_pack_start (GTK_BOX (seat7_box), seat7_label, FALSE, FALSE, 0);

  seat7_bot = gtk_radio_button_new_with_label (seat7_group, _("Computer"));
  seat7_group = gtk_radio_button_group (GTK_RADIO_BUTTON (seat7_bot));
  gtk_widget_ref (seat7_bot);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "seat7_bot", seat7_bot,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (seat7_bot);
  gtk_box_pack_start (GTK_BOX (seat7_box), seat7_bot, FALSE, FALSE, 3);

  seat7_open = gtk_radio_button_new_with_label (seat7_group, _("Open"));
  seat7_group = gtk_radio_button_group (GTK_RADIO_BUTTON (seat7_open));
  gtk_widget_ref (seat7_open);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "seat7_open", seat7_open,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (seat7_open);
  gtk_box_pack_start (GTK_BOX (seat7_box), seat7_open, FALSE, FALSE, 0);

  seat7_resv = gtk_radio_button_new_with_label (seat7_group, _("Reserved for"));
  seat7_group = gtk_radio_button_group (GTK_RADIO_BUTTON (seat7_resv));
  gtk_widget_ref (seat7_resv);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "seat7_resv", seat7_resv,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (seat7_resv);
  gtk_box_pack_start (GTK_BOX (seat7_box), seat7_resv, FALSE, FALSE, 0);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (seat7_resv), TRUE);

  seat7_name = gtk_entry_new_with_max_length (MAX_RESERVED_NAME_LEN);
  gtk_widget_ref (seat7_name);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "seat7_name", seat7_name,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (seat7_name);
  gtk_box_pack_start (GTK_BOX (seat7_box), seat7_name, TRUE, TRUE, 0);
  /* gtk_widget_set_usize (seat7_name, 100, -2); */

  seat8_box = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (seat8_box);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "seat8_box", seat8_box,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (seat8_box);
  gtk_box_pack_start (GTK_BOX (main_box), seat8_box, TRUE, TRUE, 0);

  seat8_label = gtk_label_new (_("Seat 8:"));
  gtk_widget_ref (seat8_label);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "seat8_label", seat8_label,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (seat8_label);
  gtk_box_pack_start (GTK_BOX (seat8_box), seat8_label, FALSE, FALSE, 0);

  seat8_bot = gtk_radio_button_new_with_label (seat8_group, _("Computer"));
  seat8_group = gtk_radio_button_group (GTK_RADIO_BUTTON (seat8_bot));
  gtk_widget_ref (seat8_bot);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "seat8_bot", seat8_bot,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (seat8_bot);
  gtk_box_pack_start (GTK_BOX (seat8_box), seat8_bot, FALSE, FALSE, 3);

  seat8_open = gtk_radio_button_new_with_label (seat8_group, _("Open"));
  seat8_group = gtk_radio_button_group (GTK_RADIO_BUTTON (seat8_open));
  gtk_widget_ref (seat8_open);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "seat8_open", seat8_open,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (seat8_open);
  gtk_box_pack_start (GTK_BOX (seat8_box), seat8_open, FALSE, FALSE, 0);

  seat8_resv = gtk_radio_button_new_with_label (seat8_group, _("Reserved for"));
  seat8_group = gtk_radio_button_group (GTK_RADIO_BUTTON (seat8_resv));
  gtk_widget_ref (seat8_resv);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "seat8_resv", seat8_resv,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (seat8_resv);
  gtk_box_pack_start (GTK_BOX (seat8_box), seat8_resv, FALSE, FALSE, 0);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (seat8_resv), TRUE);

  seat8_name = gtk_entry_new_with_max_length (MAX_RESERVED_NAME_LEN);
  gtk_widget_ref (seat8_name);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "seat8_name", seat8_name,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (seat8_name);
  gtk_box_pack_start (GTK_BOX (seat8_box), seat8_name, TRUE, TRUE, 0);
  /* gtk_widget_set_usize (seat8_name, 100, -2); */

  desc_box = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (desc_box);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "desc_box", desc_box,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (desc_box);
  gtk_box_pack_start (GTK_BOX (main_box), desc_box, TRUE, TRUE, 0);

  game_desc_label = gtk_label_new (_("Game Description   "));
  gtk_widget_ref (game_desc_label);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "game_desc_label", game_desc_label,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (game_desc_label);
  gtk_box_pack_start (GTK_BOX (desc_box), game_desc_label, FALSE, FALSE, 0);

  desc_entry = gtk_entry_new ();
  gtk_widget_ref (desc_entry);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "desc_entry", desc_entry,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (desc_entry);
  gtk_box_pack_start (GTK_BOX (desc_box), desc_entry, TRUE, TRUE, 0);

  hbox11 = GTK_DIALOG (dlg_launch)->action_area;
  gtk_object_set_data (GTK_OBJECT (dlg_launch), "hbox11", hbox11);
  gtk_widget_show (hbox11);
  gtk_container_set_border_width (GTK_CONTAINER (hbox11), 10);

  button_box = gtk_hbutton_box_new ();
  gtk_widget_ref (button_box);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "button_box", button_box,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (button_box);
  gtk_box_pack_start (GTK_BOX (hbox11), button_box, TRUE, TRUE, 0);

  launch_button = gtk_button_new_with_label (_("Launch"));
  gtk_widget_ref (launch_button);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "launch_button", launch_button,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (launch_button);
  gtk_container_add (GTK_CONTAINER (button_box), launch_button);
  GTK_WIDGET_SET_FLAGS (launch_button, GTK_CAN_DEFAULT);

  cancel_button = gtk_button_new_with_label (_("Cancel"));
  gtk_widget_ref (cancel_button);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "cancel_button", cancel_button,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (cancel_button);
  gtk_container_add (GTK_CONTAINER (button_box), cancel_button);
  GTK_WIDGET_SET_FLAGS (cancel_button, GTK_CAN_DEFAULT);

  gtk_signal_connect (GTK_OBJECT (dlg_launch), "destroy",
                      GTK_SIGNAL_FUNC (gtk_widget_destroyed),
                      &launch_dialog);
  gtk_signal_connect (GTK_OBJECT (dlg_launch), "realize",
                      GTK_SIGNAL_FUNC (launch_fill_defaults),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (seats_entry), "changed",
                      GTK_SIGNAL_FUNC (launch_seats_changed),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (seat1_resv), "toggled",
                      GTK_SIGNAL_FUNC (launch_resv_toggle),
                      seat1_name);
  gtk_signal_connect (GTK_OBJECT (seat2_resv), "toggled",
                      GTK_SIGNAL_FUNC (launch_resv_toggle),
                      seat2_name);
  gtk_signal_connect (GTK_OBJECT (seat3_resv), "toggled",
                      GTK_SIGNAL_FUNC (launch_resv_toggle),
                      seat3_name);
  gtk_signal_connect (GTK_OBJECT (seat4_resv), "toggled",
                      GTK_SIGNAL_FUNC (launch_resv_toggle),
                      seat4_name);
  gtk_signal_connect (GTK_OBJECT (seat5_resv), "toggled",
                      GTK_SIGNAL_FUNC (launch_resv_toggle),
                      seat5_name);
  gtk_signal_connect (GTK_OBJECT (seat6_resv), "toggled",
                      GTK_SIGNAL_FUNC (launch_resv_toggle),
                      seat6_name);
  gtk_signal_connect (GTK_OBJECT (seat7_resv), "toggled",
                      GTK_SIGNAL_FUNC (launch_resv_toggle),
                      seat7_name);
  gtk_signal_connect (GTK_OBJECT (seat8_resv), "toggled",
                      GTK_SIGNAL_FUNC (launch_resv_toggle),
                      seat8_name);
  gtk_signal_connect (GTK_OBJECT (launch_button), "clicked",
                      GTK_SIGNAL_FUNC (launch_start_game),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (cancel_button), "clicked",
                      GTK_SIGNAL_FUNC (launch_cancel_button_clicked),
                      NULL);

  return dlg_launch;
}

