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

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include "connect.h"
#include "dlg_details.h"
#include "dlg_error.h"
#include "dlg_login.h"
#include "dlg_props.h"
#include "datatypes.h"
#include "server.h"
#include "support.h"
#include "xtext.h"
#include "err_func.h"

/* Globals neaded by this dialog */
extern struct ConnectInfo client;
extern GtkWidget *dlg_details;
extern GtkWidget *dlg_props;
extern GtkWidget *main_win;

/* Global widget for this dialog */
GtkWidget *dlg_login;

/* Local callbacks which no other file will call */
static void login_fill_defaults(GtkWidget* win, gpointer data);
static void login_normal_toggled(GtkWidget* button, gpointer window);
static void login_guest_toggled(GtkWidget* button, gpointer window);
static void login_first_toggled(GtkWidget* button, gpointer window);
static void login_edit_profiles(GtkWidget* button, gpointer window);
static void login_start_session(GtkWidget* button, gpointer window);
static void login_show_details(GtkWidget* button, gpointer data);
static void login_profile_changed(GtkWidget* entry, gpointer data);
static void login_entry_changed(GtkWidget* entry, gpointer window);
static void login_set_entries(Server server);
static void login_get_entries(GtkWidget* button, gpointer window);



static char profile_override = 0;
static char entries_override = 0;



/*                              *
 *           Callbacks          *
 *                              */

/* Put default entries into dialog upon realization */
static void login_fill_defaults(GtkWidget* win, gpointer data)
{
        GtkWidget* tmp;
	GList *items;

	tmp = lookup_widget(win, "profile_combo");

	/* Fill profile combo box if there are any profiles*/
	if ( (items = server_get_name_list())) {
		gtk_combo_set_popdown_strings(GTK_COMBO(tmp), items);
	}
	else 
		/* 
		 * Disable combo if there are no profiles.  This is a
		 * workaround for a Gtk bug when combo's are empty
		 */
		gtk_widget_set_sensitive(tmp, FALSE);
	
	/* Override with the current selected profile */
	if (items == NULL || client.server.name != NULL)
		login_set_entries(client.server);
}


/* Show password box for normal logins only */
static void login_normal_toggled(GtkWidget* button, gpointer window)
{ 
	GtkWidget* tmp = lookup_widget(GTK_WIDGET(window), "password_box");

        if (GTK_TOGGLE_BUTTON(button)->active) 
                gtk_widget_show(tmp);
        else
                gtk_widget_hide(tmp);
}


static void login_guest_toggled(GtkWidget* button, gpointer window)
{ 
	GtkWidget* tmp;

	/* Ignore changes if we're overriding */
	if (entries_override) {
		return;
	}

	tmp = lookup_widget(dlg_login, "profile_entry");

	/* Don't allow profile "changed" callback to occur */
	profile_override = 1;
	gtk_entry_set_text(GTK_ENTRY(tmp), "");
	profile_override = 0;

}

static void login_first_toggled(GtkWidget* button, gpointer window)
{ 
	GtkWidget* tmp;

	/* Ignore changes if we're overriding */
	if (entries_override) {
		return;
	}

	tmp = lookup_widget(dlg_login, "profile_entry");

	/* Don't allow profile "changed" callback to occur */
	profile_override = 1;
	gtk_entry_set_text(GTK_ENTRY(tmp), "");
	profile_override = 0;

}


static void login_profile_changed(GtkWidget* entry, gpointer data)
{
	Server* server = NULL;

	/* Ignore changes if we're overriding */
	if (profile_override) {
		return;
	}

	server = server_get(gtk_entry_get_text(GTK_ENTRY(entry)));
	if (!server) {
		dbg_msg("Profile not found");
		return;
	}

	login_set_entries(*server);
}


static void login_entry_changed(GtkWidget* entry, gpointer data)
{
	GtkWidget* tmp;

	/* Ignore changes if we're overriding */
	if (entries_override) {
		return;
	}

	tmp = lookup_widget(dlg_login, "profile_entry");

	/* Don't allow profile "changed" callback to occur */
	profile_override = 1;
	gtk_entry_set_text(GTK_ENTRY(tmp), "");
	profile_override = 0;
}


static void login_edit_profiles(GtkWidget* button, gpointer window)
{
	/* If it already exists, bring it to the front */
	if (dlg_props) {
		gdk_window_show(dlg_props->window);
		gdk_window_raise(dlg_props->window);
	} else {
		dlg_props = create_dlg_props();
		gtk_widget_show(dlg_props);
	}
}


static void login_set_entries(Server server)
{
	GtkWidget* tmp;
	gchar* port;

        tmp = lookup_widget(dlg_login, "profile_entry");
	/* Set profile override so "changed" callbacks don't go */
	profile_override = 1;
	if (server.name)
		gtk_entry_set_text(GTK_ENTRY(tmp), server.name);
	profile_override = 0;

	/* Set entries override so their "changed" callbacks don't go */
	entries_override = 1;
	
        tmp = lookup_widget(dlg_login, "host_entry");
	gtk_entry_set_text(GTK_ENTRY(tmp), server.host);

        tmp = lookup_widget(dlg_login, "port_entry");
	port = g_strdup_printf("%d", server.port);
	gtk_entry_set_text(GTK_ENTRY(tmp), port);
	g_free(port);

	switch (server.type) {
	case GGZ_LOGIN:
		tmp = lookup_widget(dlg_login, "normal_radio");
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(tmp), TRUE);
		break;
	case GGZ_LOGIN_GUEST:
		tmp = lookup_widget(dlg_login, "guest_radio");
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(tmp), TRUE);
		break;
	case GGZ_LOGIN_NEW:
		tmp = lookup_widget(dlg_login, "first_radio");
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(tmp), TRUE);
		break;
	}

        tmp = lookup_widget(dlg_login, "name_entry");
	gtk_entry_set_text(GTK_ENTRY(tmp), server.login);

	if (server.type == GGZ_LOGIN && server.password != NULL) {
		tmp = lookup_widget(dlg_login, "pass_entry");
		gtk_entry_set_text(GTK_ENTRY(tmp), server.password);
	}

	/* Clear entries override so their "changed" callbacks can go */
	entries_override = 0;
}


/* Grab and store options from entires*/
static void login_get_entries(GtkWidget* button, gpointer window)
{
        gpointer tmp;

        tmp = lookup_widget(window, "profile_entry");
        client.server.name = g_strdup(gtk_entry_get_text(GTK_ENTRY(tmp)));

        tmp = lookup_widget(window, "name_entry");
        client.server.login = g_strdup(gtk_entry_get_text(GTK_ENTRY(tmp)));

        tmp = lookup_widget(window, "pass_entry");
        client.server.password = g_strdup(gtk_entry_get_text(GTK_ENTRY(tmp)));

        tmp = lookup_widget(window, "host_entry");
        client.server.host = g_strdup(gtk_entry_get_text(GTK_ENTRY(tmp)));

        tmp = lookup_widget(window, "port_entry");
        client.server.port = atoi(gtk_entry_get_text(GTK_ENTRY(tmp)));

        tmp = lookup_widget(window, "normal_radio");
        if (GTK_TOGGLE_BUTTON(tmp)->active)
                client.server.type = GGZ_LOGIN;

        tmp = lookup_widget(window, "guest_radio");
        if (GTK_TOGGLE_BUTTON(tmp)->active)
		client.server.type = GGZ_LOGIN_GUEST;

        tmp = lookup_widget(window, "first_radio");
        if (GTK_TOGGLE_BUTTON(tmp)->active)
                client.server.type = GGZ_LOGIN_NEW;
}



/* Launch game session */
static void login_start_session(GtkWidget* button, gpointer window)
{
	GtkWidget *tmp;

	/* Desensitive connect button to prevent multiple client attempts*/
	gtk_widget_set_sensitive(button, FALSE);
	
	/* 
	 * FIXME: Do we need to get these again? 
	 * login_input_options should have already grabbed these 
	 */
        if (client.connected) {
        	tmp = lookup_widget((window), "name_entry");
	        client.server.login = g_strdup(gtk_entry_get_text(GTK_ENTRY(tmp)));
                
        	tmp = lookup_widget((window), "pass_entry");
	        client.server.password = g_strdup(gtk_entry_get_text(GTK_ENTRY(tmp)));

                switch (client.server.type) {
                case GGZ_LOGIN: /*Normal login */  
			normal_login();
			break;
                case GGZ_LOGIN_GUEST: /*Anonymous login */
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
		err_dlg(_("Could not connect"));
		return;
	}
	
        client.connected = TRUE;

        /*FIXME: Other session starting things ? */
}


/* Show detail windows */
static void login_show_details(GtkWidget* button, gpointer user_data)
{
	/* If it already exists, bring it to the front */
	if (dlg_details) {
		gdk_window_show(dlg_details->window);
		gdk_window_raise(dlg_details->window);
	} else {
		dlg_details = create_dlg_details();
		gtk_widget_show(dlg_details);
	}
}


void login_ok()
{
	GtkWidget* tmp;
	gchar* buf;


	if(dlg_login != NULL) {
		gtk_widget_destroy(dlg_login);	
		dlg_login = NULL;
	}

	/* Clear old lists */
	tmp = lookup_widget(main_win, "table_tree");
	gtk_clist_clear(GTK_CLIST(tmp));
	
	tmp = lookup_widget(main_win, "player_list");
	gtk_clist_clear(GTK_CLIST(tmp));

	/* Clear chat box */
	tmp = lookup_widget(main_win, "chat_text");
	gtk_xtext_remove_lines(GTK_XTEXT(tmp), (GTK_XTEXT(tmp))->num_lines, 1);

	/* Print some intro text in cha tbox */
	buf = g_strdup_printf("Client Version:\00314 %s",VERSION);
	gtk_xtext_append_indent(GTK_XTEXT(tmp),"---",3, buf, strlen(buf));
	g_free(buf);

	buf = g_strdup_printf(_("GTK+ Version:\00314 %d.%d.%d\n"),
			      gtk_major_version, gtk_minor_version, 
			      gtk_micro_version);
	gtk_xtext_append_indent(GTK_XTEXT(tmp),"---",3,buf,strlen(buf));
	g_free(buf);

	gtk_xtext_append_indent(GTK_XTEXT(tmp),"---",3,_("Options:"), 8);
#ifndef DEBUG
	gtk_xtext_append_indent(GTK_XTEXT(tmp),"---",3,_("  Debug: \00314No "), 15);
#else
	gtk_xtext_append_indent(GTK_XTEXT(tmp),"---",3,_("  Debug: \00314Yes"), 15);
#endif
#ifndef SOCKET_DEBUG
	gtk_xtext_append_indent(GTK_XTEXT(tmp),"---",3,_("  Socket Debug: \00314No "), 22);
#else
	gtk_xtext_append_indent(GTK_XTEXT(tmp),"---",3,_("  Socket Debug: \00314Yes"), 22);
#endif
	buf = g_strdup_printf(_("Connected to server: %s"), client.server.host);
	gtk_xtext_append_indent(GTK_XTEXT(tmp),"---",3,buf,strlen(buf));
}


void login_bad_name(gchar* title, gchar* msg)
{
        GtkWidget *tmp;

        tmp = lookup_widget((dlg_login), "connect_button");
	gtk_label_set_text(GTK_LABEL(GTK_BIN(tmp)->child),"Login");
	gtk_widget_set_sensitive(GTK_WIDGET(tmp),TRUE);

        tmp = lookup_widget((dlg_login), "top_panel");
	gtk_notebook_set_page(GTK_NOTEBOOK(tmp), 1);

	tmp = lookup_widget((dlg_login), "profile_frame");
	gtk_frame_set_label(GTK_FRAME(tmp), title);

	tmp = lookup_widget((dlg_login), "msg_label");
	gtk_label_set_text(GTK_LABEL(tmp), msg);
}


void login_disconnect()
{
        GtkWidget *tmp;

	/* Set sensitive FALSE */
        tmp = lookup_widget((main_win), "disconnect");
	gtk_widget_set_sensitive(GTK_WIDGET(tmp),FALSE);

        tmp = lookup_widget((main_win), "game_menu");
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
  GtkWidget *dialog_vbox1;
  GtkWidget *profile_frame;
  GtkWidget *entries_box;
  GtkWidget *top_panel;
  GtkWidget *top_box;
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
  GtkWidget *msg_label;
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
  gtk_window_set_policy (GTK_WINDOW (dlg_login), FALSE, FALSE, TRUE);

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

  top_panel = gtk_notebook_new ();
  gtk_widget_ref (top_panel);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "top_panel", top_panel,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (top_panel);
  gtk_box_pack_start (GTK_BOX (entries_box), top_panel, TRUE, TRUE, 3);
  GTK_WIDGET_UNSET_FLAGS (top_panel, GTK_CAN_FOCUS);
  gtk_notebook_set_show_tabs (GTK_NOTEBOOK (top_panel), FALSE);
  gtk_notebook_set_show_border (GTK_NOTEBOOK (top_panel), FALSE);

  top_box = gtk_vbox_new (FALSE, 3);
  gtk_widget_ref (top_box);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "top_box", top_box,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (top_box);
  gtk_container_add (GTK_CONTAINER (top_panel), top_box);
  gtk_container_set_border_width (GTK_CONTAINER (top_box), 3);

  profile_box = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (profile_box);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "profile_box", profile_box,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (profile_box);
  gtk_box_pack_start (GTK_BOX (top_box), profile_box, TRUE, TRUE, 0);

  profile_label = gtk_label_new (_("Profile: "));
  gtk_widget_ref (profile_label);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "profile_label", profile_label,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (profile_label);
  gtk_box_pack_start (GTK_BOX (profile_box), profile_label, FALSE, TRUE, 0);
  gtk_widget_set_usize (profile_label, 55, -2);
  gtk_misc_set_alignment (GTK_MISC (profile_label), 1, 0.5);

  profile_combo = gtk_combo_new ();
  gtk_widget_ref (profile_combo);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "profile_combo", profile_combo,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (profile_combo);
  gtk_box_pack_start (GTK_BOX (profile_box), profile_combo, FALSE, TRUE, 5);
  gtk_widget_set_usize (profile_combo, 175, -2);
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
  gtk_box_pack_start (GTK_BOX (profile_box), profile_button_box, FALSE, FALSE, 5);

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
  gtk_box_pack_start (GTK_BOX (top_box), server_box, TRUE, TRUE, 0);

  server_label = gtk_label_new (_("Server: "));
  gtk_widget_ref (server_label);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "server_label", server_label,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (server_label);
  gtk_box_pack_start (GTK_BOX (server_box), server_label, FALSE, TRUE, 0);
  gtk_widget_set_usize (server_label, 55, -2);
  gtk_misc_set_alignment (GTK_MISC (server_label), 1, 0.5);

  host_entry = gtk_entry_new_with_max_length (256);
  gtk_widget_ref (host_entry);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "host_entry", host_entry,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (host_entry);
  gtk_box_pack_start (GTK_BOX (server_box), host_entry, FALSE, FALSE, 5);
  gtk_widget_set_usize (host_entry, 175, -2);

  port_label = gtk_label_new (_("Port: "));
  gtk_widget_ref (port_label);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "port_label", port_label,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (port_label);
  gtk_box_pack_start (GTK_BOX (server_box), port_label, FALSE, TRUE, 2);
  gtk_widget_set_usize (port_label, 45, -2);
  gtk_misc_set_alignment (GTK_MISC (port_label), 1, 0.5);

  port_entry = gtk_entry_new_with_max_length (5);
  gtk_widget_ref (port_entry);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "port_entry", port_entry,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (port_entry);
  gtk_box_pack_start (GTK_BOX (server_box), port_entry, FALSE, FALSE, 0);
  gtk_widget_set_usize (port_entry, 50, -2);
  gtk_entry_set_text (GTK_ENTRY (port_entry), "5688");

  msg_label = gtk_label_new ("");
  gtk_widget_ref (msg_label);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "msg_label", msg_label,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (msg_label);
  gtk_container_add (GTK_CONTAINER (top_panel), msg_label);

  hseparator = gtk_hseparator_new ();
  gtk_widget_ref (hseparator);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "hseparator", hseparator,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hseparator);
  gtk_box_pack_start (GTK_BOX (entries_box), hseparator, TRUE, TRUE, 7);

  login_box = gtk_hbox_new (FALSE, 10);
  gtk_widget_ref (login_box);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "login_box", login_box,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (login_box);
  gtk_box_pack_start (GTK_BOX (entries_box), login_box, FALSE, FALSE, 1);

  user_box = gtk_vbox_new (TRUE, 5);
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
  gtk_widget_set_usize (user_label, 62, -2);
  gtk_misc_set_alignment (GTK_MISC (user_label), 1, 0.5);

  name_entry = gtk_entry_new_with_max_length (16);
  gtk_widget_ref (name_entry);
  gtk_object_set_data_full (GTK_OBJECT (dlg_login), "name_entry", name_entry,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (name_entry);
  gtk_box_pack_start (GTK_BOX (username_box), name_entry, FALSE, TRUE, 0);
  gtk_widget_set_usize (name_entry, 150, -2);

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
  gtk_widget_set_usize (pass_entry, 150, -2);
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
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (normal_radio), TRUE);

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
  gtk_signal_connect (GTK_OBJECT (dlg_login), "destroy",
                      GTK_SIGNAL_FUNC (gtk_widget_destroyed),
                      &dlg_login);
  gtk_signal_connect (GTK_OBJECT (profile_entry), "changed",
                      GTK_SIGNAL_FUNC (login_profile_changed),
                      dlg_login);
  gtk_signal_connect (GTK_OBJECT (edit_profiles_button), "clicked",
                      GTK_SIGNAL_FUNC (login_edit_profiles),
                      dlg_login);
  gtk_signal_connect (GTK_OBJECT (host_entry), "changed",
                      GTK_SIGNAL_FUNC (login_entry_changed),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (name_entry), "changed",
                      GTK_SIGNAL_FUNC (login_entry_changed),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (pass_entry), "changed",
                      GTK_SIGNAL_FUNC (login_entry_changed),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (normal_radio), "toggled",
                      GTK_SIGNAL_FUNC (login_normal_toggled),
                      dlg_login);
  gtk_signal_connect (GTK_OBJECT (guest_radio), "toggled",
                      GTK_SIGNAL_FUNC (login_guest_toggled),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (first_radio), "toggled",
                      GTK_SIGNAL_FUNC (login_first_toggled),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (connect_button), "clicked",
                      GTK_SIGNAL_FUNC (login_get_entries),
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

