/*
 * File: login.c
 * Author: Justin Zaun
 * Project: GGZ GTK Client
 * $Id: login.c 6273 2004-11-05 21:49:00Z jdorje $
 *
 * This is the main program body for the GGZ client
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
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include <ggz.h>
#include <ggzcore.h>

#include "client.h"
#include "ggzclient.h"
#include "login.h"
#include "msgbox.h"
#include "playerlist.h"
#include "props.h"
#include "server.h"
#include "support.h"

GtkWidget *login_dialog;
gint entries_update;
static GtkWidget *create_dlg_login(void);

/* Callbacks login dialog box */
static void login_fill_defaults(GtkWidget *widget, gpointer data);
static void login_profile_changed(GtkEditable *editable, gpointer data);
static void login_edit_profiles(GtkButton *button, gpointer data);
static void login_entry_changed (GtkEditable *editable, gpointer data);
static void login_normal_toggled(GtkToggleButton *togglebutton, gpointer data);
static void login_guest_toggled(GtkToggleButton *togglebutton, gpointer data);
static void login_first_toggled(GtkToggleButton *togglebutton, gpointer data);
static void login_connect_button_clicked(GtkButton *button, gpointer data);
static void login_cancel_button_clicked(GtkButton *button, gpointer data);
static void login_start_session(void);
static void login_relogin(void);
#if 0 /* currently unused */
static GGZHookReturn login_reconnect(GGZServerEvent id, void* event_data, void* user_data);
#endif
static void login_set_entries(Server server);

void
login_create_or_raise(void)
{
	if (!login_dialog) {
		login_dialog = create_dlg_login();
		gtk_widget_show(login_dialog);
	}
	else {
		gdk_window_show(login_dialog->window);
		gdk_window_raise(login_dialog->window);
	}
}


void
login_connect_failed(void)
{
	/* Destroy server object */
	server_disconnect();
}


void login_failed(GGZErrorEventData *error)
{
	GtkWidget *tmp;
	char msg[1024];
	
	/* First, disconnect from the server. */
	if (ggzcore_server_logout(server) < 0)
		ggz_error_msg("Error logging out in login_failed");

	/* Re-enable the "connect" button and change it say "Login" */
	tmp = lookup_widget(login_dialog, "connect_button");
	gtk_label_set_text(GTK_LABEL(GTK_BIN(tmp)->child),"Login");
	gtk_widget_set_sensitive(tmp, TRUE);

	tmp = lookup_widget(login_dialog, "top_panel");
	gtk_notebook_set_page(GTK_NOTEBOOK(tmp), 1);
	
	tmp = lookup_widget(login_dialog, "profile_frame");
	gtk_frame_set_label(GTK_FRAME(tmp), _("Sorry!"));
	
	tmp = lookup_widget(login_dialog, "msg_label");

	switch (error->status) {
	case E_ALREADY_LOGGED_IN:
		snprintf(msg, sizeof(msg),
			 _("That username is already in usage."));
		break;
	case E_USR_LOOKUP:
		snprintf(msg, sizeof(msg),
			 _("That username is already in usage,\nor not "
			   "permitted on this server.\n\nPlease choose a "
			   "different name"));
		break;
	case E_TOO_LONG:
		snprintf(msg, sizeof(msg),
			 _("The username is too long!"));
		break;
	default:
		snprintf(msg, sizeof(msg),
			 _("Login failed for unknown reason: %s"),
			 error->message);
		break;
	}
	gtk_label_set_text(GTK_LABEL(tmp), msg);
}


void
login_destroy(void)
{
	if (login_dialog) {
		gtk_widget_destroy(login_dialog);
		login_dialog = NULL;
	}
}


void login_goto_server(gchar *server_url)
{
	GtkWidget *tmp;

	login_create_or_raise();
	tmp = lookup_widget(GTK_WIDGET(login_dialog), "host_entry");
	if (!strncasecmp (server_url, "ggz://", 6))
		gtk_entry_set_text(GTK_ENTRY(tmp), server_url+6);
	else
		gtk_entry_set_text(GTK_ENTRY(tmp), server_url);

	tmp = lookup_widget(GTK_WIDGET(login_dialog), "name_entry");
	gtk_entry_set_text(GTK_ENTRY(tmp), (gchar*)ggzcore_server_get_handle(server));

	tmp = lookup_widget(GTK_WIDGET(login_dialog), "guest_radio");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(tmp), TRUE);
}

static void
login_fill_defaults                    (GtkWidget       *widget,
                                        gpointer         user_data)
{
	GtkWidget* tmp;
	GList *items;
	char *last;

	tmp = lookup_widget(login_dialog, "profile_combo");

	/* Fill profile combo box if there are any profiles*/
	if ( (items = server_get_name_list())) {
		gtk_combo_set_popdown_strings(GTK_COMBO(tmp), items);
	}
	else
		gtk_widget_set_sensitive(tmp, FALSE);

	/* Set to last server connected to */
	last = ggzcore_conf_read_string("OPTIONS", "LASTPROFILE", "NONE");
	if(strcmp(last, "NONE"))
	{
		tmp = lookup_widget(login_dialog, "profile_entry");
		gtk_entry_set_text(GTK_ENTRY(tmp), last);
	}
	ggz_free(last);
}


static void
login_profile_changed                  (GtkEditable     *editable,
                                        gpointer         user_data)
{
	GtkWidget *tmp;
	Server* server = NULL;

	tmp = lookup_widget(login_dialog, "profile_entry");
	server = server_get(gtk_entry_get_text(GTK_ENTRY(tmp)));

	if (!server) {
		/* Profile Not Found */
		return;
	}

	login_set_entries(*server);
}


static void
login_edit_profiles                    (GtkButton       *button,
                                        gpointer         user_data)
{
	props_create_or_raise();
}



static void
login_entry_changed                    (GtkEditable     *editable,
                                        gpointer         user_data)
{
	GtkWidget *tmp = NULL;

	if(!entries_update)
	{
		tmp = lookup_widget(login_dialog, "profile_entry");
		gtk_entry_set_text(GTK_ENTRY(tmp), "");
	}
}


/* Show password box for normal logins only */
static void
login_normal_toggled                   (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
	GtkWidget* tmp = lookup_widget(GTK_WIDGET(user_data), "password_box");

        if (GTK_TOGGLE_BUTTON(togglebutton)->active) 
                gtk_widget_show(tmp);
        else
                gtk_widget_hide(tmp);

	if(!entries_update)
	{
		tmp = lookup_widget(login_dialog, "profile_entry");
		gtk_entry_set_text(GTK_ENTRY(tmp), "");
	}
}


static void
login_guest_toggled                    (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
	GtkWidget *tmp;

	if(!entries_update)
	{
		tmp = lookup_widget(login_dialog, "profile_entry");
		gtk_entry_set_text(GTK_ENTRY(tmp), "");
	}
}


static void
login_first_toggled                    (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
	GtkWidget *tmp;

	if(!entries_update)
	{
		tmp = lookup_widget(login_dialog, "profile_entry");
		gtk_entry_set_text(GTK_ENTRY(tmp), "");
	}
}


static void login_connect_button_clicked(GtkButton *button, gpointer data)
{
	/* If the server object doesn't exist, create a new one */
	if (!server) {
		login_start_session();
	}

	/* If we're not logged in yet, assume second login attempt */
	else if (ggzcore_server_get_state(server) == GGZ_STATE_ONLINE) {
		login_relogin();
	}

	/* otherwise disconnect then reconnect */
	else {
		/* FIXME: should popup modal "Are you sure?" */
		if (ggzcore_server_logout(server) < 0)
			ggz_error_msg("Error logging out in "
			              "login_connect_button_clicked");
		/* FIXME: should provide service to reconnect */
	}
}


static void login_cancel_button_clicked(GtkButton *button, gpointer data)
{
	/* If we're already connected, disconnect */
	if (server) {
		if (ggzcore_server_logout(server) < 0)
			ggz_error_msg("Error logging out in "
			              "login_cancel_button_clicked");
	}
	gtk_widget_destroy(login_dialog);
	login_dialog = NULL;
}


static void login_start_session(void)
{
	GtkWidget *tmp;
	const char *host = NULL, *login = NULL, *password = NULL;
	char *sessiondump;
	int port;
	GGZLoginType type = GGZ_LOGIN_GUEST;

	/* Clearout the room, table and player lists */
	/* FIXME: this should be done as a function in client.c */
	tmp = lookup_widget(win_main, "room_clist");
	gtk_clist_clear(GTK_CLIST(tmp));
	tmp = lookup_widget(win_main, "table_clist");
	gtk_clist_clear(GTK_CLIST(tmp));
	clear_player_list();

	/* FIXME: perhaps this should be done elsewhere? 
	   tmp = g_object_get_data(G_OBJECT(login_dialog), "connect_button");
	gtk_widget_set_sensitive(tmp, FALSE);*/

	/* Get connection and login data */
	tmp = g_object_get_data(G_OBJECT(login_dialog), "host_entry");
	host = gtk_entry_get_text(GTK_ENTRY(tmp));

	tmp = g_object_get_data(G_OBJECT(login_dialog), "port_entry");
	port = atoi(gtk_entry_get_text(GTK_ENTRY(tmp)));

	tmp = g_object_get_data(G_OBJECT(login_dialog), "name_entry");
	login = gtk_entry_get_text(GTK_ENTRY(tmp));

	tmp = g_object_get_data(G_OBJECT(login_dialog), "normal_radio");
	if (GTK_TOGGLE_BUTTON (tmp)->active)
	{
		type = GGZ_LOGIN;
		tmp = g_object_get_data(G_OBJECT(login_dialog), "pass_entry");
		password = gtk_entry_get_text(GTK_ENTRY(tmp));
	}
	tmp = g_object_get_data(G_OBJECT(login_dialog), "guest_radio");
	if (GTK_TOGGLE_BUTTON (tmp)->active)
		type = GGZ_LOGIN_GUEST;
	tmp = g_object_get_data(G_OBJECT(login_dialog), "first_radio");
	if (GTK_TOGGLE_BUTTON (tmp)->active)
		type = GGZ_LOGIN_NEW;

	/* Create new server object and set connection/login info */
	server = ggzcore_server_new();
	ggzcore_server_set_hostinfo(server, host, port, 0);
	ggzcore_server_set_logininfo(server, type, login, password);
	
	/* Log server communications to file */
	sessiondump = ggzcore_conf_read_string("Debug", "SessionLog", NULL);
	ggzcore_server_log_session(server, sessiondump);
	ggz_free(sessiondump);

	/* Save as last profile */
	tmp = lookup_widget(login_dialog, "profile_entry");
	if(strcmp(gtk_entry_get_text(GTK_ENTRY(tmp)), ""))
	{
		ggzcore_conf_write_string("OPTIONS", "LASTPROFILE", gtk_entry_get_text(GTK_ENTRY(tmp)));
		ggzcore_conf_commit();
	}

	/* Setup callbacks on server */
	ggz_event_init(server);

	/* Start connection */
	ggzcore_server_connect(server);
}


static void login_relogin(void)
{
	GtkWidget *tmp;
	const char *login = NULL, *password = NULL;
	GGZLoginType type = GGZ_LOGIN_GUEST;

	/* FIXME: perhaps this should be done elsewhere? 
	tmp = lookup_widget(login_dialog, "connect_button");
	gtk_widget_set_sensitive(tmp, FALSE);*/

	/* Get login data */
	tmp = lookup_widget(login_dialog, "name_entry");
	login = gtk_entry_get_text(GTK_ENTRY(tmp));

	tmp = lookup_widget(login_dialog, "normal_radio");
	if(GTK_TOGGLE_BUTTON (tmp)->active)
	{
		type = GGZ_LOGIN;
		tmp = g_object_get_data(G_OBJECT(login_dialog), "pass_entry");
		password = gtk_entry_get_text(GTK_ENTRY(tmp));
	}
	tmp = g_object_get_data(G_OBJECT(login_dialog), "guest_radio");
	if(GTK_TOGGLE_BUTTON (tmp)->active)
		type = GGZ_LOGIN_GUEST;
	tmp = g_object_get_data(G_OBJECT(login_dialog), "first_radio");
	if(GTK_TOGGLE_BUTTON (tmp)->active)
		type = GGZ_LOGIN_NEW;

	ggzcore_server_set_logininfo(server, type, login, password);
	ggzcore_server_login(server);
}


#if 0 /* currently unused */
static GGZHookReturn login_reconnect(GGZServerEvent id, void* event_data, void* user_data)
{
	/* Now that we're disconnected, login to new server */
	/* FIXME: provide a destroy function that frees profile */

	return GGZ_HOOK_REMOVE;
}
#endif


static void login_set_entries(Server server)
{
	GtkWidget *tmp;
	gchar *port;
	
	entries_update = TRUE;

	/*
	tmp = lookup_widget(login_dialog, "profile_entry");
	if (server.name)
		gtk_entry_set_text(GTK_ENTRY(tmp), server.name);
	*/

	tmp = lookup_widget(login_dialog, "host_entry");
	gtk_entry_set_text(GTK_ENTRY(tmp), server.host);
	
	tmp = lookup_widget(login_dialog, "port_entry");
	port = g_strdup_printf("%d", server.port);
	gtk_entry_set_text(GTK_ENTRY(tmp), port);
	g_free(port);

	switch (server.type) {
		case GGZ_LOGIN:
		tmp = lookup_widget(login_dialog, "normal_radio");
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(tmp), TRUE);
		break;	
		case GGZ_LOGIN_GUEST:
		tmp = lookup_widget(login_dialog, "guest_radio");
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(tmp), TRUE);
		break;	
		case GGZ_LOGIN_NEW:
		tmp = lookup_widget(login_dialog, "first_radio");
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(tmp), TRUE);
		break;	
	}

	tmp = lookup_widget(login_dialog, "name_entry");
	gtk_entry_set_text(GTK_ENTRY(tmp), server.login);

	if (server.type == GGZ_LOGIN && server.password != NULL) {
		tmp = lookup_widget(login_dialog, "pass_entry");
		gtk_entry_set_text(GTK_ENTRY(tmp), server.password);
	}

	entries_update = FALSE;
}





GtkWidget*
create_dlg_login (void)
{
  GtkWidget *dlg_login;
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

  dlg_login = gtk_dialog_new ();
  gtk_window_set_transient_for(GTK_WINDOW(dlg_login), GTK_WINDOW(win_main));
  g_object_set_data(G_OBJECT (dlg_login), "dlg_login", dlg_login);
  gtk_window_set_title (GTK_WINDOW (dlg_login), _("Connect and Login"));
  gtk_window_set_policy (GTK_WINDOW (dlg_login), FALSE, TRUE, TRUE);

  dialog_vbox1 = GTK_DIALOG (dlg_login)->vbox;
  g_object_set_data(G_OBJECT (dlg_login), "dialog_vbox1", dialog_vbox1);
  gtk_widget_show (dialog_vbox1);

  profile_frame = gtk_frame_new (_("Server Profile"));
  gtk_widget_ref (profile_frame);
  g_object_set_data_full(G_OBJECT (dlg_login), "profile_frame", profile_frame,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (profile_frame);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), profile_frame, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (profile_frame), 10);

  entries_box = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (entries_box);
  g_object_set_data_full(G_OBJECT (dlg_login), "entries_box", entries_box,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (entries_box);
  gtk_container_add (GTK_CONTAINER (profile_frame), entries_box);
  gtk_container_set_border_width (GTK_CONTAINER (entries_box), 5);

  top_panel = gtk_notebook_new ();
  gtk_widget_ref (top_panel);
  g_object_set_data_full(G_OBJECT (dlg_login), "top_panel", top_panel,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (top_panel);
  gtk_box_pack_start (GTK_BOX (entries_box), top_panel, TRUE, TRUE, 3);
  GTK_WIDGET_UNSET_FLAGS (top_panel, GTK_CAN_FOCUS);
  gtk_notebook_set_show_tabs (GTK_NOTEBOOK (top_panel), FALSE);
  gtk_notebook_set_show_border (GTK_NOTEBOOK (top_panel), FALSE);

  top_box = gtk_vbox_new (FALSE, 3);
  gtk_widget_ref (top_box);
  g_object_set_data_full(G_OBJECT (dlg_login), "top_box", top_box,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (top_box);
  gtk_container_add (GTK_CONTAINER (top_panel), top_box);
  gtk_container_set_border_width (GTK_CONTAINER (top_box), 3);

  profile_box = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (profile_box);
  g_object_set_data_full(G_OBJECT (dlg_login), "profile_box", profile_box,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (profile_box);
  gtk_box_pack_start (GTK_BOX (top_box), profile_box, TRUE, TRUE, 0);

  profile_label = gtk_label_new (_("Profile: "));
  gtk_widget_ref (profile_label);
  g_object_set_data_full(G_OBJECT (dlg_login), "profile_label", profile_label,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (profile_label);
  gtk_box_pack_start (GTK_BOX (profile_box), profile_label, FALSE, TRUE, 0);
  gtk_misc_set_alignment (GTK_MISC (profile_label), 1, 0.5);

  profile_combo = gtk_combo_new ();
  gtk_widget_ref (profile_combo);
  g_object_set_data_full(G_OBJECT (dlg_login), "profile_combo", profile_combo,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (profile_combo);
  gtk_box_pack_start (GTK_BOX (profile_box), profile_combo, FALSE, TRUE, 5);
  gtk_combo_set_value_in_list (GTK_COMBO (profile_combo), TRUE, FALSE);

  profile_entry = GTK_COMBO (profile_combo)->entry;
  gtk_widget_ref (profile_entry);
  g_object_set_data_full(G_OBJECT (dlg_login), "profile_entry", profile_entry,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (profile_entry);
  gtk_entry_set_editable (GTK_ENTRY (profile_entry), FALSE);

  profile_button_box = gtk_hbutton_box_new ();
  gtk_widget_ref (profile_button_box);
  g_object_set_data_full(G_OBJECT (dlg_login), "profile_button_box", profile_button_box,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (profile_button_box);
  gtk_box_pack_start (GTK_BOX (profile_box), profile_button_box, FALSE, FALSE, 5);

  edit_profiles_button = stockbutton_new(GTK_STOCK_PREFERENCES,
					 _("Edit Profiles..."));
  gtk_widget_ref (edit_profiles_button);
  g_object_set_data_full(G_OBJECT (dlg_login), "edit_profiles_button", edit_profiles_button,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (edit_profiles_button);
  gtk_container_add (GTK_CONTAINER (profile_button_box), edit_profiles_button);
  GTK_WIDGET_SET_FLAGS (edit_profiles_button, GTK_CAN_DEFAULT);

  server_box = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (server_box);
  g_object_set_data_full(G_OBJECT (dlg_login), "server_box", server_box,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (server_box);
  gtk_box_pack_start (GTK_BOX (top_box), server_box, TRUE, TRUE, 0);

  server_label = gtk_label_new (_("Server: "));
  gtk_widget_ref (server_label);
  g_object_set_data_full(G_OBJECT (dlg_login), "server_label", server_label,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (server_label);
  gtk_box_pack_start (GTK_BOX (server_box), server_label, FALSE, TRUE, 0);
  gtk_misc_set_alignment (GTK_MISC (server_label), 1, 0.5);

  host_entry = gtk_entry_new_with_max_length (256);
  gtk_widget_ref (host_entry);
  g_object_set_data_full(G_OBJECT (dlg_login), "host_entry", host_entry,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (host_entry);
  gtk_box_pack_start (GTK_BOX (server_box), host_entry, FALSE, FALSE, 5);
  gtk_entry_set_text (GTK_ENTRY (host_entry), _("localhost"));

  port_label = gtk_label_new (_("Port: "));
  gtk_widget_ref (port_label);
  g_object_set_data_full(G_OBJECT (dlg_login), "port_label", port_label,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (port_label);
  gtk_box_pack_start (GTK_BOX (server_box), port_label, FALSE, TRUE, 2);
  gtk_misc_set_alignment (GTK_MISC (port_label), 1, 0.5);

  port_entry = gtk_entry_new_with_max_length (5);
  gtk_widget_ref (port_entry);
  g_object_set_data_full(G_OBJECT (dlg_login), "port_entry", port_entry,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (port_entry);
  gtk_box_pack_start (GTK_BOX (server_box), port_entry, FALSE, FALSE, 0);
  gtk_entry_set_text (GTK_ENTRY (port_entry), _("5688"));

  msg_label = gtk_label_new ("");
  gtk_widget_ref (msg_label);
  g_object_set_data_full(G_OBJECT (dlg_login), "msg_label", msg_label,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (msg_label);
  gtk_container_add (GTK_CONTAINER (top_panel), msg_label);

  hseparator = gtk_hseparator_new ();
  gtk_widget_ref (hseparator);
  g_object_set_data_full(G_OBJECT (dlg_login), "hseparator", hseparator,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hseparator);
  gtk_box_pack_start (GTK_BOX (entries_box), hseparator, TRUE, TRUE, 7);

  login_box = gtk_hbox_new (FALSE, 10);
  gtk_widget_ref (login_box);
  g_object_set_data_full(G_OBJECT (dlg_login), "login_box", login_box,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (login_box);
  gtk_box_pack_start (GTK_BOX (entries_box), login_box, FALSE, FALSE, 1);

  user_box = gtk_vbox_new (TRUE, 5);
  gtk_widget_ref (user_box);
  g_object_set_data_full(G_OBJECT (dlg_login), "user_box", user_box,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (user_box);
  gtk_box_pack_start (GTK_BOX (login_box), user_box, FALSE, FALSE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (user_box), 10);

  username_box = gtk_hbox_new (FALSE, 5);
  gtk_widget_ref (username_box);
  g_object_set_data_full(G_OBJECT (dlg_login), "username_box", username_box,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (username_box);
  gtk_box_pack_start (GTK_BOX (user_box), username_box, TRUE, TRUE, 0);

  user_label = gtk_label_new (_("Username: "));
  gtk_widget_ref (user_label);
  g_object_set_data_full(G_OBJECT (dlg_login), "user_label", user_label,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (user_label);
  gtk_box_pack_start (GTK_BOX (username_box), user_label, TRUE, TRUE, 0);
  gtk_misc_set_alignment (GTK_MISC (user_label), 1, 0.5);

  name_entry = gtk_entry_new_with_max_length (16);
  gtk_widget_ref (name_entry);
  g_object_set_data_full(G_OBJECT (dlg_login), "name_entry", name_entry,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (name_entry);
  gtk_box_pack_start (GTK_BOX (username_box), name_entry, FALSE, TRUE, 0);

  password_box = gtk_hbox_new (FALSE, 5);
  gtk_widget_ref (password_box);
  g_object_set_data_full(G_OBJECT (dlg_login), "password_box", password_box,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (password_box);
  gtk_box_pack_start (GTK_BOX (user_box), password_box, TRUE, TRUE, 0);

  pass_label = gtk_label_new (_("Password: "));
  gtk_widget_ref (pass_label);
  g_object_set_data_full(G_OBJECT (dlg_login), "pass_label", pass_label,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (pass_label);
  gtk_box_pack_start (GTK_BOX (password_box), pass_label, TRUE, TRUE, 0);
  gtk_misc_set_alignment (GTK_MISC (pass_label), 1, 0.5);

  pass_entry = gtk_entry_new ();
  gtk_widget_ref (pass_entry);
  g_object_set_data_full(G_OBJECT (dlg_login), "pass_entry", pass_entry,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (pass_entry);
  gtk_box_pack_start (GTK_BOX (password_box), pass_entry, FALSE, TRUE, 0);
  gtk_entry_set_visibility (GTK_ENTRY (pass_entry), FALSE);

  radio_box = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (radio_box);
  g_object_set_data_full(G_OBJECT (dlg_login), "radio_box", radio_box,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (radio_box);
  gtk_box_pack_start (GTK_BOX (login_box), radio_box, FALSE, FALSE, 0);

  normal_radio = gtk_radio_button_new_with_label (login_type_group, _("Normal Login"));
  login_type_group = gtk_radio_button_group (GTK_RADIO_BUTTON (normal_radio));
  gtk_widget_ref (normal_radio);
  g_object_set_data_full(G_OBJECT (dlg_login), "normal_radio", normal_radio,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (normal_radio);
  gtk_box_pack_start (GTK_BOX (radio_box), normal_radio, TRUE, TRUE, 0);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (normal_radio), TRUE);

  guest_radio = gtk_radio_button_new_with_label (login_type_group, _("Guest Login"));
  login_type_group = gtk_radio_button_group (GTK_RADIO_BUTTON (guest_radio));
  gtk_widget_ref (guest_radio);
  g_object_set_data_full(G_OBJECT (dlg_login), "guest_radio", guest_radio,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (guest_radio);
  gtk_box_pack_start (GTK_BOX (radio_box), guest_radio, TRUE, TRUE, 0);

  first_radio = gtk_radio_button_new_with_label (login_type_group, _("First-time Login"));
  login_type_group = gtk_radio_button_group (GTK_RADIO_BUTTON (first_radio));
  gtk_widget_ref (first_radio);
  g_object_set_data_full(G_OBJECT (dlg_login), "first_radio", first_radio,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (first_radio);
  gtk_box_pack_start (GTK_BOX (radio_box), first_radio, TRUE, TRUE, 0);

  dialog_action_area1 = GTK_DIALOG (dlg_login)->action_area;
  g_object_set_data(G_OBJECT (dlg_login), "dialog_action_area1", dialog_action_area1);
  gtk_widget_show (dialog_action_area1);
  gtk_container_set_border_width (GTK_CONTAINER (dialog_action_area1), 10);

  hbuttonbox = gtk_hbutton_box_new ();
  gtk_widget_ref (hbuttonbox);
  g_object_set_data_full(G_OBJECT (dlg_login), "hbuttonbox", hbuttonbox,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbuttonbox);
  gtk_box_pack_start (GTK_BOX (dialog_action_area1), hbuttonbox, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (hbuttonbox), 5);

  connect_button = stockbutton_new(GTK_STOCK_JUMP_TO, _("Connect"));
  gtk_widget_ref (connect_button);
  g_object_set_data_full(G_OBJECT (dlg_login), "connect_button", connect_button,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (connect_button);
  gtk_container_add (GTK_CONTAINER (hbuttonbox), connect_button);
  GTK_WIDGET_SET_FLAGS (connect_button, GTK_CAN_DEFAULT);

  cancel_button = gtk_button_new_from_stock(GTK_STOCK_CANCEL);
  gtk_widget_ref (cancel_button);
  g_object_set_data_full(G_OBJECT (dlg_login), "cancel_button", cancel_button,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (cancel_button);
  gtk_container_add (GTK_CONTAINER (hbuttonbox), cancel_button);
  GTK_WIDGET_SET_FLAGS (cancel_button, GTK_CAN_DEFAULT);

  g_signal_connect (GTK_OBJECT (dlg_login), "realize",
                      GTK_SIGNAL_FUNC (login_fill_defaults),
                      NULL);
  g_signal_connect (GTK_OBJECT (dlg_login), "destroy",
                      GTK_SIGNAL_FUNC (gtk_widget_destroyed),
                      &login_dialog);
  g_signal_connect (GTK_OBJECT (profile_entry), "changed",
                      GTK_SIGNAL_FUNC (login_profile_changed),
                      dlg_login);
  g_signal_connect (GTK_OBJECT (edit_profiles_button), "clicked",
                      GTK_SIGNAL_FUNC (login_edit_profiles),
                      dlg_login);
  g_signal_connect (GTK_OBJECT (host_entry), "changed",
                      GTK_SIGNAL_FUNC (login_entry_changed),
                      NULL);
  g_signal_connect (GTK_OBJECT (name_entry), "changed",
                      GTK_SIGNAL_FUNC (login_entry_changed),
                      NULL);
  g_signal_connect (GTK_OBJECT (pass_entry), "changed",
                      GTK_SIGNAL_FUNC (login_entry_changed),
                      NULL);
  g_signal_connect (GTK_OBJECT (normal_radio), "toggled",
                      GTK_SIGNAL_FUNC (login_normal_toggled),
                      dlg_login);
  g_signal_connect (GTK_OBJECT (guest_radio), "toggled",
                      GTK_SIGNAL_FUNC (login_guest_toggled),
                      NULL);
  g_signal_connect (GTK_OBJECT (first_radio), "toggled",
                      GTK_SIGNAL_FUNC (login_first_toggled),
                      NULL);
  g_signal_connect (GTK_OBJECT (connect_button), "clicked",
                      GTK_SIGNAL_FUNC (login_connect_button_clicked),
                      NULL);
  g_signal_connect (GTK_OBJECT (cancel_button), "clicked",
                      GTK_SIGNAL_FUNC (login_cancel_button_clicked),
                      NULL);

  gtk_widget_grab_default (connect_button);
  return dlg_login;
}

