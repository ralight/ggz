/*
 * File: dlg_login.c
 * Author: Brent Hendricks
 * Project: GGZ Client
 * Date: 4/29/00
 *
 * GGZ Client window for logging in to server and associated callbacks
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

#include <stdio.h>
#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include "connect.h"
#include "dlg_details.h"
#include "dlg_error.h"
#include "dlg_login.h"
#include "datatypes.h"
#include "support.h"


/* Globals neaded by this dialog */
extern struct ConnectInfo connection;
extern GtkWidget *detail_window;
extern GtkWidget *main_win;

/* Global widget for this dialog */
GtkWidget *dlg_login;

/* Local callbacks which no other file will call */
static void login_fill_defaults(GtkWidget* win, gpointer data);
static void login_guest_toggled(GtkWidget* button, gpointer window);
static void login_edit_profiles(GtkWidget* button, gpointer window);
static void login_input_options(GtkWidget* button, gpointer window);
static void login_start_session(GtkWidget* button, gpointer window);
static void login_show_details(GtkWidget* button, gpointer data);


/*                              *
 *           Callbacks          *
 *                              */


/* Put default entries into dialog upon realization */
static void login_fill_defaults(GtkWidget* win, gpointer data)
{
        GtkWidget* tmp;
        gchar* port;
	
        tmp = lookup_widget(win, "name_entry");
        if (connection.username)
                gtk_entry_set_text(GTK_ENTRY(tmp), connection.username);
        else
                gtk_entry_set_text(GTK_ENTRY(tmp), g_get_user_name());


        tmp = lookup_widget(win, "host_entry");
        if (connection.server)
                gtk_entry_set_text(GTK_ENTRY(tmp), connection.server);
        else
                gtk_entry_set_text(GTK_ENTRY(tmp), "localhost");

        tmp = lookup_widget(win, "port_entry");
        if (connection.port) {
                port = g_strdup_printf("%d", connection.port);
                gtk_entry_set_text(GTK_ENTRY(tmp), port);
		g_free(port);
        }
        else
                gtk_entry_set_text(GTK_ENTRY(tmp), "5688");

        tmp = lookup_widget(win, "guest_radio");
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(tmp), TRUE);
}


/* Hide password box for guest logins */
static void login_guest_toggled(GtkWidget* button, gpointer window)
{ 
	GtkWidget* tmp = lookup_widget(GTK_WIDGET(window), "password_box");

        if (GTK_TOGGLE_BUTTON(button)->active) 
                gtk_widget_hide(tmp);
        else
                gtk_widget_show(tmp);
}


static void login_edit_profiles(GtkWidget* button, gpointer window)
{

}


/* Grab and store options from entires*/
static void login_input_options(GtkWidget* button, gpointer window)
{
        gpointer tmp;

        tmp = lookup_widget(window, "name_entry");
        connection.username = g_strdup(gtk_entry_get_text(GTK_ENTRY(tmp)));

        tmp = lookup_widget(window, "pass_entry");
        connection.password = g_strdup(gtk_entry_get_text(GTK_ENTRY(tmp)));

        tmp = lookup_widget(window, "host_entry");
        connection.server = g_strdup(gtk_entry_get_text(GTK_ENTRY(tmp)));

        tmp = lookup_widget(window, "port_entry");
        connection.port = atoi(gtk_entry_get_text(GTK_ENTRY(tmp)));

        tmp = lookup_widget(window, "normal_radio");
        if (GTK_TOGGLE_BUTTON(tmp)->active)
                connection.login_type = GGZ_LOGIN;

        tmp = lookup_widget(window, "guest_radio");
        if (GTK_TOGGLE_BUTTON(tmp)->active)
                connection.login_type = GGZ_LOGIN_ANON;

        tmp = lookup_widget(window, "first_radio");
        if (GTK_TOGGLE_BUTTON(tmp)->active)
                connection.login_type = GGZ_LOGIN_NEW;
}


/* Launch game session */
static void login_start_session(GtkWidget* button, gpointer window)
{
	GtkWidget *tmp;

	/* Desensitive connect button to prevent multiple connection attempts*/
	gtk_widget_set_sensitive(button, FALSE);
	
	/* 
	 * FIXME: Do we need to get these again? 
	 * login_input_options should have already grabbed these 
	 */
        if (connection.connected) {
        	tmp = lookup_widget((window), "name_entry");
	        connection.username = g_strdup(gtk_entry_get_text(GTK_ENTRY(tmp)));
                
        	tmp = lookup_widget((window), "pass_entry");
	        connection.password = g_strdup(gtk_entry_get_text(GTK_ENTRY(tmp)));

                switch (connection.login_type) {
                case GGZ_LOGIN: /*Normal login */  
			normal_login();
			break;
                case GGZ_LOGIN_ANON: /*Anonymous login */
                        anon_login();
			break;
                case GGZ_LOGIN_NEW: /*First time login */
			new_login();
			break;
                }

                return;
        } 
	
	/* FIXME: Initialize for new game session */
	if (connect_to_server() < 0) {
		err_dlg("Could not connect");
		return;
	}
	
        connection.connected = TRUE;

        /*FIXME: Other session starting things ? */
}


/* Show detail windows */
static void login_show_details(GtkWidget* button, gpointer user_data)
{
	/* Don't allow for multiple detail windows */
	gtk_widget_set_sensitive(button, FALSE);
	
        detail_window = create_dlg_details();
        gtk_widget_show(detail_window);
}


void login_ok()
{
	if(dlg_login)
		gtk_widget_destroy(dlg_login);	
}


void login_bad_name()
{
        GtkWidget *tmp;

        tmp = lookup_widget((dlg_login), "connect_button");
	gtk_label_set_text(GTK_LABEL(GTK_BIN(tmp)->child),"Login");
	gtk_widget_set_sensitive(GTK_WIDGET(tmp),TRUE);
        tmp = lookup_widget((dlg_login), "server_frame");
	gtk_widget_hide(tmp);
}


void login_disconnect()
{
        GtkWidget *tmp;

	/* Set sensitive FALSE */
        tmp = lookup_widget((main_win), "disconnect");
	gtk_widget_set_sensitive(GTK_WIDGET(tmp),FALSE);

        tmp = lookup_widget((main_win), "game_menu");
	gtk_widget_set_sensitive(GTK_WIDGET(tmp),FALSE);
        tmp = lookup_widget((main_win), "edit");
	gtk_widget_set_sensitive(GTK_WIDGET(tmp),FALSE);
        tmp = lookup_widget((main_win), "view_menu");
	gtk_widget_set_sensitive(GTK_WIDGET(tmp),FALSE);

        tmp = lookup_widget((main_win), "msg_entry");
	gtk_widget_set_sensitive(GTK_WIDGET(tmp),FALSE);
        tmp = lookup_widget((main_win), "msg_button");
	gtk_widget_set_sensitive(GTK_WIDGET(tmp),FALSE);
        tmp = lookup_widget((main_win), "room_combo");
	gtk_widget_set_sensitive(GTK_WIDGET(tmp),FALSE);

        tmp = lookup_widget((main_win), "launch_button");
	gtk_widget_set_sensitive(GTK_WIDGET(tmp),FALSE);
        tmp = lookup_widget((main_win), "join_button");
	gtk_widget_set_sensitive(GTK_WIDGET(tmp),FALSE);
        tmp = lookup_widget((main_win), "props_button");
	gtk_widget_set_sensitive(GTK_WIDGET(tmp),FALSE);
        tmp = lookup_widget((main_win), "stats_button");
	gtk_widget_set_sensitive(GTK_WIDGET(tmp),FALSE);
        tmp = lookup_widget((main_win), "disconnect_button");
	gtk_widget_set_sensitive(GTK_WIDGET(tmp),FALSE);

        tmp = lookup_widget((main_win), "player_list");
	gtk_widget_set_sensitive(GTK_WIDGET(tmp),FALSE);
        tmp = lookup_widget((main_win), "table_tree");
	gtk_widget_set_sensitive(GTK_WIDGET(tmp),FALSE);

	/* Set sensitive TRUE */
        tmp = lookup_widget((main_win), "connect");
	gtk_widget_set_sensitive(GTK_WIDGET(tmp),TRUE);
        tmp = lookup_widget((main_win), "connect_button");
	gtk_widget_set_sensitive(GTK_WIDGET(tmp),TRUE);
}


void login_online()
{
        GtkWidget *tmp;

	/* Set sensitive FALSE */
        tmp = lookup_widget((main_win), "connect");
	gtk_widget_set_sensitive(GTK_WIDGET(tmp),FALSE);
        tmp = lookup_widget((main_win), "connect_button");
	gtk_widget_set_sensitive(GTK_WIDGET(tmp),FALSE);

	/* Set sensitive TRUE */
        tmp = lookup_widget((main_win), "disconnect");
	gtk_widget_set_sensitive(GTK_WIDGET(tmp),TRUE);
        tmp = lookup_widget((main_win), "game_menu");
	gtk_widget_set_sensitive(GTK_WIDGET(tmp),TRUE);
        tmp = lookup_widget((main_win), "edit");
	gtk_widget_set_sensitive(GTK_WIDGET(tmp),TRUE);
        tmp = lookup_widget((main_win), "view_menu");
	gtk_widget_set_sensitive(GTK_WIDGET(tmp),TRUE);

        tmp = lookup_widget((main_win), "msg_entry");
	gtk_widget_set_sensitive(GTK_WIDGET(tmp),TRUE);
        tmp = lookup_widget((main_win), "msg_button");
	gtk_widget_set_sensitive(GTK_WIDGET(tmp),TRUE);
        tmp = lookup_widget((main_win), "room_combo");
	gtk_widget_set_sensitive(GTK_WIDGET(tmp),TRUE);

        tmp = lookup_widget((main_win), "launch_button");
	gtk_widget_set_sensitive(GTK_WIDGET(tmp),TRUE);
        tmp = lookup_widget((main_win), "join_button");
	gtk_widget_set_sensitive(GTK_WIDGET(tmp),TRUE);
        tmp = lookup_widget((main_win), "props_button");
	gtk_widget_set_sensitive(GTK_WIDGET(tmp),TRUE);
        tmp = lookup_widget((main_win), "stats_button");
	gtk_widget_set_sensitive(GTK_WIDGET(tmp),TRUE);
        tmp = lookup_widget((main_win), "disconnect_button");
	gtk_widget_set_sensitive(GTK_WIDGET(tmp),TRUE);

        tmp = lookup_widget((main_win), "player_list");
	gtk_widget_set_sensitive(GTK_WIDGET(tmp),TRUE);
        tmp = lookup_widget((main_win), "table_tree");
	gtk_widget_set_sensitive(GTK_WIDGET(tmp),TRUE);

}


GtkWidget*
create_dlg_login (void)
{
  GtkWidget *dlg_login;
  GtkWidget *dialog_vbox1;
  GtkWidget *profile_frame;
  GtkWidget *entries_box;
  GtkWidget *profile_box;
  GtkWidget *profile_label;
  GtkWidget *profile_combo;
  GtkWidget *profile_entry;
  GtkWidget *profile_button_box;
  GtkWidget *edit_profiles_button;
  GtkWidget *server_box;
  GtkWidget *server_label;
  GtkWidget *host_entry;
  GtkWidget *port_label;
  GtkWidget *port_entry;
  GtkWidget *hseparator;
  GtkWidget *login_box;
  GtkWidget *user_box;
  GtkWidget *username_box;
  GtkWidget *user_label;
  GtkWidget *name_entry;
  GtkWidget *password_box;
  GtkWidget *pass_label;
  GtkWidget *pass_entry;
  GtkWidget *radio_box;
  GSList *login_type_group = NULL;
  GtkWidget *normal_radio;
  GtkWidget *guest_radio;
  GtkWidget *first_radio;
  GtkWidget *dialog_action_area1;
  GtkWidget *hbuttonbox;
  GtkWidget *connect_button;
  GtkWidget *cancel_button;
  GtkWidget *details_button;

  dlg_login = gtk_dialog_new ();
  gtk_object_set_data (GTK_OBJECT (dlg_login), "dlg_login", dlg_login);
  gtk_window_set_title (GTK_WINDOW (dlg_login), _("Connect and Login"));
  gtk_window_set_policy (GTK_WINDOW (dlg_login), FALSE, FALSE, FALSE);

  dialog_vbox1 = GTK_DIALOG (dlg_login)->vbox;
  gtk_object_set_data (GTK_OBJECT (dlg_login), "dialog_vbox1", dialog_vbox1);
  gtk_widget_show (dialog_vbox1);

  profile_frame = gtk_frame_new (_("Server Profile"));
  gtk_widget_ref (profile_frame);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "profile_frame", profile_frame,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (profile_frame);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), profile_frame, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (profile_frame), 10);

  entries_box = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (entries_box);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "entries_box", entries_box,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (entries_box);
  gtk_container_add (GTK_CONTAINER (profile_frame), entries_box);
  gtk_container_set_border_width (GTK_CONTAINER (entries_box), 5);

  profile_box = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (profile_box);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "profile_box", profile_box,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (profile_box);
  gtk_box_pack_start (GTK_BOX (entries_box), profile_box, TRUE, TRUE, 0);

  profile_label = gtk_label_new (_("Profile: "));
  gtk_widget_ref (profile_label);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "profile_label", profile_label,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (profile_label);
  gtk_box_pack_start (GTK_BOX (profile_box), profile_label, FALSE, FALSE, 0);
  gtk_widget_set_usize (profile_label, 44, -2);
  gtk_misc_set_alignment (GTK_MISC (profile_label), 1, 0.5);

  profile_combo = gtk_combo_new ();
  gtk_widget_ref (profile_combo);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "profile_combo", profile_combo,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (profile_combo);
  gtk_box_pack_start (GTK_BOX (profile_box), profile_combo, FALSE, TRUE, 5);
  gtk_combo_set_value_in_list (GTK_COMBO (profile_combo), TRUE, FALSE);

  profile_entry = GTK_COMBO (profile_combo)->entry;
  gtk_widget_ref (profile_entry);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "profile_entry", profile_entry,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (profile_entry);

  profile_button_box = gtk_hbutton_box_new ();
  gtk_widget_ref (profile_button_box);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "profile_button_box", profile_button_box,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (profile_button_box);
  gtk_box_pack_start (GTK_BOX (profile_box), profile_button_box, TRUE, TRUE, 5);

  edit_profiles_button = gtk_button_new_with_label (_("Edit Profiles..."));
  gtk_widget_ref (edit_profiles_button);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "edit_profiles_button", edit_profiles_button,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (edit_profiles_button);
  gtk_container_add (GTK_CONTAINER (profile_button_box), edit_profiles_button);
  GTK_WIDGET_SET_FLAGS (edit_profiles_button, GTK_CAN_DEFAULT);

  server_box = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (server_box);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "server_box", server_box,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (server_box);
  gtk_box_pack_start (GTK_BOX (entries_box), server_box, TRUE, TRUE, 5);

  server_label = gtk_label_new (_("Server: "));
  gtk_widget_ref (server_label);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "server_label", server_label,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (server_label);
  gtk_box_pack_start (GTK_BOX (server_box), server_label, FALSE, FALSE, 0);
  gtk_misc_set_alignment (GTK_MISC (server_label), 1, 0.5);

  host_entry = gtk_entry_new_with_max_length (256);
  gtk_widget_ref (host_entry);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "host_entry", host_entry,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (host_entry);
  gtk_box_pack_start (GTK_BOX (server_box), host_entry, FALSE, FALSE, 5);

  port_label = gtk_label_new (_("Port: "));
  gtk_widget_ref (port_label);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "port_label", port_label,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (port_label);
  gtk_box_pack_start (GTK_BOX (server_box), port_label, FALSE, FALSE, 5);

  port_entry = gtk_entry_new_with_max_length (5);
  gtk_widget_ref (port_entry);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "port_entry", port_entry,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (port_entry);
  gtk_box_pack_start (GTK_BOX (server_box), port_entry, FALSE, FALSE, 0);
  gtk_widget_set_usize (port_entry, 50, -2);

  hseparator = gtk_hseparator_new ();
  gtk_widget_ref (hseparator);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "hseparator", hseparator,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hseparator);
  gtk_box_pack_start (GTK_BOX (entries_box), hseparator, FALSE, FALSE, 7);

  login_box = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (login_box);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "login_box", login_box,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (login_box);
  gtk_box_pack_start (GTK_BOX (entries_box), login_box, TRUE, TRUE, 0);

  user_box = gtk_vbox_new (FALSE, 5);
  gtk_widget_ref (user_box);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "user_box", user_box,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (user_box);
  gtk_box_pack_start (GTK_BOX (login_box), user_box, FALSE, FALSE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (user_box), 10);

  username_box = gtk_hbox_new (FALSE, 5);
  gtk_widget_ref (username_box);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "username_box", username_box,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (username_box);
  gtk_box_pack_start (GTK_BOX (user_box), username_box, TRUE, TRUE, 0);

  user_label = gtk_label_new (_("Username: "));
  gtk_widget_ref (user_label);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "user_label", user_label,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (user_label);
  gtk_box_pack_start (GTK_BOX (username_box), user_label, FALSE, FALSE, 0);
  gtk_misc_set_alignment (GTK_MISC (user_label), 1, 0.5);

  name_entry = gtk_entry_new_with_max_length (8);
  gtk_widget_ref (name_entry);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "name_entry", name_entry,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (name_entry);
  gtk_box_pack_start (GTK_BOX (username_box), name_entry, FALSE, TRUE, 0);
  gtk_widget_set_usize (name_entry, 100, -2);

  password_box = gtk_hbox_new (FALSE, 5);
  gtk_widget_ref (password_box);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "password_box", password_box,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (password_box);
  gtk_box_pack_start (GTK_BOX (user_box), password_box, TRUE, TRUE, 0);

  pass_label = gtk_label_new (_("Password: "));
  gtk_widget_ref (pass_label);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "pass_label", pass_label,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (pass_label);
  gtk_box_pack_start (GTK_BOX (password_box), pass_label, FALSE, FALSE, 0);
  gtk_widget_set_usize (pass_label, 62, -2);
  gtk_misc_set_alignment (GTK_MISC (pass_label), 1, 0.5);

  pass_entry = gtk_entry_new ();
  gtk_widget_ref (pass_entry);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "pass_entry", pass_entry,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (pass_entry);
  gtk_box_pack_start (GTK_BOX (password_box), pass_entry, FALSE, TRUE, 0);
  gtk_widget_set_usize (pass_entry, 100, -2);
  gtk_entry_set_visibility (GTK_ENTRY (pass_entry), FALSE);

  radio_box = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (radio_box);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "radio_box", radio_box,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (radio_box);
  gtk_box_pack_start (GTK_BOX (login_box), radio_box, FALSE, FALSE, 0);

  normal_radio = gtk_radio_button_new_with_label (login_type_group, _("Normal Login"));
  login_type_group = gtk_radio_button_group (GTK_RADIO_BUTTON (normal_radio));
  gtk_widget_ref (normal_radio);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "normal_radio", normal_radio,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (normal_radio);
  gtk_box_pack_start (GTK_BOX (radio_box), normal_radio, FALSE, FALSE, 0);

  guest_radio = gtk_radio_button_new_with_label (login_type_group, _("Guest Login"));
  login_type_group = gtk_radio_button_group (GTK_RADIO_BUTTON (guest_radio));
  gtk_widget_ref (guest_radio);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "guest_radio", guest_radio,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (guest_radio);
  gtk_box_pack_start (GTK_BOX (radio_box), guest_radio, FALSE, FALSE, 0);

  first_radio = gtk_radio_button_new_with_label (login_type_group, _("First-time Login"));
  login_type_group = gtk_radio_button_group (GTK_RADIO_BUTTON (first_radio));
  gtk_widget_ref (first_radio);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "first_radio", first_radio,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (first_radio);
  gtk_box_pack_start (GTK_BOX (radio_box), first_radio, FALSE, FALSE, 0);

  dialog_action_area1 = GTK_DIALOG (dlg_login)->action_area;
  gtk_object_set_data (GTK_OBJECT (dlg_login), "dialog_action_area1", dialog_action_area1);
  gtk_widget_show (dialog_action_area1);
  gtk_container_set_border_width (GTK_CONTAINER (dialog_action_area1), 10);

  hbuttonbox = gtk_hbutton_box_new ();
  gtk_widget_ref (hbuttonbox);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "hbuttonbox", hbuttonbox,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbuttonbox);
  gtk_box_pack_start (GTK_BOX (dialog_action_area1), hbuttonbox, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (hbuttonbox), 5);

  connect_button = gtk_button_new_with_label (_("Connect"));
  gtk_widget_ref (connect_button);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "connect_button", connect_button,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (connect_button);
  gtk_container_add (GTK_CONTAINER (hbuttonbox), connect_button);
  GTK_WIDGET_SET_FLAGS (connect_button, GTK_CAN_DEFAULT);

  cancel_button = gtk_button_new_with_label (_("Cancel"));
  gtk_widget_ref (cancel_button);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "cancel_button", cancel_button,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (cancel_button);
  gtk_container_add (GTK_CONTAINER (hbuttonbox), cancel_button);
  GTK_WIDGET_SET_FLAGS (cancel_button, GTK_CAN_DEFAULT);

  details_button = gtk_button_new_with_label (_("Details..."));
  gtk_widget_ref (details_button);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "details_button", details_button,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (details_button);
  gtk_container_add (GTK_CONTAINER (hbuttonbox), details_button);
  GTK_WIDGET_SET_FLAGS (details_button, GTK_CAN_DEFAULT);

  gtk_signal_connect (GTK_OBJECT (dlg_login), "realize",
                      GTK_SIGNAL_FUNC (login_fill_defaults),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (dlg_login), "delete_event",
                      GTK_SIGNAL_FUNC (gtk_widget_destroy),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (dlg_login), "destroy_event",
                      GTK_SIGNAL_FUNC (gtk_widget_destroy),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (edit_profiles_button), "clicked",
                      GTK_SIGNAL_FUNC (login_edit_profiles),
                      dlg_login);
  gtk_signal_connect (GTK_OBJECT (guest_radio), "toggled",
                      GTK_SIGNAL_FUNC (login_guest_toggled),
                      dlg_login);
  gtk_signal_connect (GTK_OBJECT (connect_button), "clicked",
                      GTK_SIGNAL_FUNC (login_input_options),
                      dlg_login);
  gtk_signal_connect (GTK_OBJECT (connect_button), "clicked",
                      GTK_SIGNAL_FUNC (login_start_session),
                      dlg_login);
  gtk_signal_connect_object (GTK_OBJECT (cancel_button), "clicked",
                             GTK_SIGNAL_FUNC (gtk_widget_destroy),
                             GTK_OBJECT (dlg_login));
  gtk_signal_connect (GTK_OBJECT (details_button), "clicked",
                      GTK_SIGNAL_FUNC (login_show_details),
                      dlg_login);

  gtk_widget_grab_default (connect_button);
  return dlg_login;
}

