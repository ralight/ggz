/*
 * File: dlg_launch.c
 * Author: Brent Hendricks
 * Project: GGZ Client
 * Date: 3/29/00
 *
 * GGZ Client window for launching new table and associated callbacks
 *
 * Copyright (C) 2000 Brent Hendricks.
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

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include "datatypes.h"
#include "dlg_launch.h"
#include "dlg_error.h"
#include "game.h"
#include "err_func.h"
#include "support.h"
#include "protocols.h"
#include "seats.h"


/* Globals neaded by this dialog */
extern struct ConnectInfo client;
extern struct GameTypes game_types;
extern struct Rooms room_info;

/* Global GtkWidget for this dialog */
GtkWidget *dlg_launch;
gint new_type;

/* Local callbacks which no other file will call */
static void launch_fill_defaults(GtkWidget* widget, gpointer data);
static void launch_seats_changed(GtkWidget* widget, gpointer data);
static void launch_start_game(GtkWidget *btn_launch, gpointer user_data);
static void launch_seat_show(gint i, gchar show);
static gint max_allowed_players(guchar mask);
static void launch_resv_toggle(GtkWidget* button, GtkWidget *entry);
static gint launch_seat_type(gint i);
static gchar* launch_get_reserve_name(gint seat);



/*                              *
 *           Callbacks          *
 *                              */

static void launch_fill_defaults(GtkWidget* widget, gpointer data)
{
        GList *items = NULL;
        GtkWidget *tmp;
        gint type, max, i;
	gchar* buf;

	/* Set game type information */
	type = room_info.info[client.cur_room].game_type;
	
	/* Display game type info */
	tmp = gtk_object_get_data(GTK_OBJECT(dlg_launch), "game_label");
	gtk_label_set_text(GTK_LABEL(tmp), game_types.info[type].name);

        tmp = gtk_object_get_data(GTK_OBJECT(dlg_launch), "author_label");
	buf = g_strdup_printf(_("Author: %s"), game_types.info[type].author); 
        gtk_label_set_text(GTK_LABEL(tmp), buf);
	g_free(buf);

        tmp = gtk_object_get_data(GTK_OBJECT(dlg_launch), "type_desc_label");
	buf = g_strdup_printf(_("Desc: %s"), game_types.info[type].desc); 
        gtk_label_set_text(GTK_LABEL(tmp), buf);
	g_free(buf);

        tmp = gtk_object_get_data(GTK_OBJECT(dlg_launch), "web_label");
	buf = g_strdup_printf(_("Homepage: %s"), game_types.info[type].web); 
        gtk_label_set_text(GTK_LABEL(tmp), buf);
	g_free(buf);

	/* Set allowable players */
	if(game_types.info[type].num_play_allow & GGZ_ALLOW_ONE)
                items = g_list_append(items, "1");
	if(game_types.info[type].num_play_allow & GGZ_ALLOW_TWO)
                items = g_list_append(items, "2");
	if(game_types.info[type].num_play_allow & GGZ_ALLOW_THREE)
                items = g_list_append(items, "3");
	if(game_types.info[type].num_play_allow & GGZ_ALLOW_FOUR)
                items = g_list_append(items, "4");
	if(game_types.info[type].num_play_allow & GGZ_ALLOW_FIVE)
                items = g_list_append(items, "5");
	if(game_types.info[type].num_play_allow & GGZ_ALLOW_SIX)
                items = g_list_append(items, "6");
	if(game_types.info[type].num_play_allow & GGZ_ALLOW_SEVEN)
                items = g_list_append(items, "7");
	if(game_types.info[type].num_play_allow & GGZ_ALLOW_EIGHT)
                items = g_list_append(items, "8");
        tmp = gtk_object_get_data(GTK_OBJECT(dlg_launch), "seats_combo");
        gtk_combo_set_popdown_strings(GTK_COMBO(tmp), items);
        g_list_free(items);
	
        /* Hide seats with numbers bigger than we allow and show others */
        max = max_allowed_players(game_types.info[type].num_play_allow);
	for (i = 1; i <= max; i++)
                launch_seat_show(i, TRUE);
        for (i = max + 1; i <= 8; i++)
                launch_seat_show(i, FALSE);

	/* Default to reserving us a seat */
	tmp = gtk_object_get_data(GTK_OBJECT(dlg_launch), "seat1_name");
        gtk_entry_set_text(GTK_ENTRY(tmp), client.server.login);
	/* FIXME: once reservations work, default to resv*/
	tmp = gtk_object_get_data(GTK_OBJECT(dlg_launch), "seat1_open");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(tmp), TRUE);

	/* Default to computer players for everyone else */
	for (i = 2; i<= max; i++) {
		buf = g_strdup_printf("seat%d_bot", i);
		tmp = gtk_object_get_data(GTK_OBJECT(dlg_launch), buf);
		g_free(buf);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(tmp), TRUE);
	}
}


static void launch_seats_changed(GtkWidget* widget, gpointer data)
{
	gpointer tmp;
	gint i, type, seats, max;
	gchar* buf;

	seats = atoi(gtk_entry_get_text(GTK_ENTRY(widget)));
	type = room_info.info[client.cur_room].game_type;
	max = max_allowed_players(game_types.info[type].num_play_allow);
	
        /* Desensitize seats with numbers bigger than what we want  
	   and sensitize others */
	for (i = 1; i <= seats; i++) {
		buf = g_strdup_printf("seat%d_box", i);
		tmp = gtk_object_get_data(GTK_OBJECT(dlg_launch), buf);
		g_free(buf);
		gtk_widget_set_sensitive(tmp, TRUE);
	}
	for (i = (seats + 1); i <= max; i++){
		buf = g_strdup_printf("seat%d_box", i);
		tmp = gtk_object_get_data(GTK_OBJECT(dlg_launch), buf);
		g_free(buf);
		gtk_widget_set_sensitive(tmp, FALSE);
	}
}


static void launch_start_game(GtkWidget *btn_launch, gpointer user_data)
{
        gint i, num, seats[8];
	GtkEntry* entry;
		
	if (!client.connected) {
                warn_dlg("Not connected!");
		gtk_widget_destroy(dlg_launch);
		return;
	}

        /* Hide it at this point, we still nead
           it there to get the information from */
        gtk_widget_hide(GTK_WIDGET(dlg_launch));
	
	/* Set game type information */
	new_type = room_info.info[client.cur_room].game_type;
        
	/* Get table launch info */
	
	entry = GTK_ENTRY(lookup_widget(dlg_launch, "desc_entry"));
	for (i = 0; i < MAX_TABLE_SIZE; i++)
		seats[i] = launch_seat_type(i);
	
	/* Get number of seats */
	for (i = 0; i < MAX_TABLE_SIZE; i++)
		if (seats[i] == GGZ_SEAT_NONE)
			break;
	num = i;
	
	/* Send launch game request to server */
	es_write_int(client.sock, REQ_TABLE_LAUNCH);
	es_write_int(client.sock, new_type);
	es_write_string(client.sock, gtk_entry_get_text(entry));
	es_write_int(client.sock, num);
	for (i = 0; i < num; i++) {
		es_write_int(client.sock, seats[i]);
		if (seats[i] == GGZ_SEAT_RESV)
			es_write_string(client.sock, 
					launch_get_reserve_name(i));
	}
	
}


static void launch_resv_toggle(GtkWidget* button, GtkWidget *entry)
{
        if (GTK_TOGGLE_BUTTON(button)->active) {
		gtk_widget_set_sensitive(GTK_WIDGET(entry), TRUE);
		gtk_widget_grab_focus(entry);
        } else
		gtk_widget_set_sensitive(GTK_WIDGET(entry), FALSE);
}


/*                              *
 *        Helper Functions      *
 *                              */

static void launch_seat_show(gint i, gchar show)
{
        gpointer tmp;
        gchar* widget;

        /* Show seat's hbox */
        widget = g_strdup_printf("seat%d_box", i);
        tmp = GTK_OBJECT(gtk_object_get_data(GTK_OBJECT(dlg_launch), widget));
        if (show)
                gtk_widget_show(GTK_WIDGET(tmp));
        else
                gtk_widget_hide(GTK_WIDGET(tmp));

}
                        
         
gint max_allowed_players(guchar mask)
{
        gchar i;
	
        for(i = 7; i >= 0; i--)
                if (mask / (1 << i))
                        return (i+1);
 
        return 0;
}       


gint launch_seat_type(gint i)
{
        gchar* widget;
        GtkWidget *tmp;
	gint seats;
	
	tmp = gtk_object_get_data(GTK_OBJECT(dlg_launch), "seats_entry");
	seats = atoi(gtk_entry_get_text(GTK_ENTRY(tmp)));
	if (i >= seats)
		return GGZ_SEAT_NONE;
	
	widget = g_strdup_printf("seat%d_bot", i+1);
	tmp = gtk_object_get_data(GTK_OBJECT(dlg_launch), widget);
	g_free(widget);
	if (GTK_TOGGLE_BUTTON(tmp)->active)
		return GGZ_SEAT_COMP;
	
	widget = g_strdup_printf("seat%d_resv", i+1);
	tmp = gtk_object_get_data(GTK_OBJECT(dlg_launch), widget);
	g_free(widget);
	if (GTK_TOGGLE_BUTTON(tmp)->active)
		return GGZ_SEAT_RESV;

	/* If we get this far it has to have been an open seat */
	return GGZ_SEAT_OPEN;
}


gchar* launch_get_reserve_name(gint seat)
{
        gchar* widget;
        GtkWidget *temp_widget;

        widget = g_strdup_printf("seat%d_name", seat+1);
        temp_widget = gtk_object_get_data(GTK_OBJECT(dlg_launch), widget);
	g_free(widget);

	return gtk_entry_get_text(GTK_ENTRY(temp_widget));
}


GtkWidget*
create_dlg_launch (void)
{
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
  gtk_window_set_policy (GTK_WINDOW (dlg_launch), FALSE, FALSE, TRUE);

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
  seats_combo_items = g_list_append (seats_combo_items, "");
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

  seat1_name = gtk_entry_new_with_max_length (8);
  gtk_widget_ref (seat1_name);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "seat1_name", seat1_name,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (seat1_name);
  gtk_box_pack_start (GTK_BOX (seat1_box), seat1_name, FALSE, TRUE, 0);
  gtk_widget_set_usize (seat1_name, 100, -2);

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

  seat2_name = gtk_entry_new_with_max_length (8);
  gtk_widget_ref (seat2_name);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "seat2_name", seat2_name,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (seat2_name);
  gtk_box_pack_start (GTK_BOX (seat2_box), seat2_name, TRUE, TRUE, 0);
  gtk_widget_set_usize (seat2_name, 100, -2);

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

  seat3_name = gtk_entry_new_with_max_length (8);
  gtk_widget_ref (seat3_name);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "seat3_name", seat3_name,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (seat3_name);
  gtk_box_pack_start (GTK_BOX (seat3_box), seat3_name, TRUE, TRUE, 0);
  gtk_widget_set_usize (seat3_name, 100, -2);

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

  seat4_name = gtk_entry_new_with_max_length (8);
  gtk_widget_ref (seat4_name);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "seat4_name", seat4_name,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (seat4_name);
  gtk_box_pack_start (GTK_BOX (seat4_box), seat4_name, TRUE, TRUE, 0);
  gtk_widget_set_usize (seat4_name, 100, -2);

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

  seat5_name = gtk_entry_new_with_max_length (8);
  gtk_widget_ref (seat5_name);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "seat5_name", seat5_name,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (seat5_name);
  gtk_box_pack_start (GTK_BOX (seat5_box), seat5_name, TRUE, TRUE, 0);
  gtk_widget_set_usize (seat5_name, 100, -2);

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

  seat6_name = gtk_entry_new_with_max_length (8);
  gtk_widget_ref (seat6_name);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "seat6_name", seat6_name,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (seat6_name);
  gtk_box_pack_start (GTK_BOX (seat6_box), seat6_name, TRUE, TRUE, 0);
  gtk_widget_set_usize (seat6_name, 100, -2);

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

  seat7_name = gtk_entry_new_with_max_length (8);
  gtk_widget_ref (seat7_name);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "seat7_name", seat7_name,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (seat7_name);
  gtk_box_pack_start (GTK_BOX (seat7_box), seat7_name, TRUE, TRUE, 0);
  gtk_widget_set_usize (seat7_name, 100, -2);

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

  seat8_name = gtk_entry_new_with_max_length (8);
  gtk_widget_ref (seat8_name);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "seat8_name", seat8_name,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (seat8_name);
  gtk_box_pack_start (GTK_BOX (seat8_box), seat8_name, TRUE, TRUE, 0);
  gtk_widget_set_usize (seat8_name, 100, -2);

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

  gtk_signal_connect_object (GTK_OBJECT (dlg_launch), "delete_event",
                             GTK_SIGNAL_FUNC (gtk_widget_destroy),
                             GTK_OBJECT (dlg_launch));
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
  gtk_signal_connect_object (GTK_OBJECT (cancel_button), "clicked",
                             GTK_SIGNAL_FUNC (gtk_widget_destroy),
                             GTK_OBJECT (dlg_launch));

  return dlg_launch;
}

