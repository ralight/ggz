/*
 * File: login.c
 * Author: Justin Zaun
 * Project: GGZ GTK Client
 * $Id: login.c 10242 2008-07-09 00:48:24Z jdorje $
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
#include <strings.h> /* For strcasecmp */
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include <ggz.h>
#include <ggzcore.h>

#include "client.h"
#include "ggzclient.h"
#include "ggz-gtk.h"
#include "login.h"
#include "msgbox.h"
#include "playerlist.h"
#include "props.h"
#include "roomlist.h"
#include "server.h"
#include "support.h"
#include "tablelist.h"

static struct {
	GtkWidget *dialog;
	gint entries_update;

	/* Global command line option */
	const char *option_log;
} login;

/* Callbacks login dialog box */
static void login_fill_defaults(GtkWidget * widget, gpointer data);
static void login_profile_changed(GtkWidget * widget, gpointer data);
static void login_edit_profiles(GtkButton * button, gpointer data);
static void login_entry_changed(GtkEditable * editable, gpointer data);
static void login_normal_toggled(GtkToggleButton * togglebutton,
				 gpointer data);
static void login_guest_toggled(GtkToggleButton * togglebutton,
				gpointer data);
static void login_first_toggled(GtkToggleButton * togglebutton,
				gpointer data);
static void login_connect_button_clicked(GtkButton * button,
					 gpointer data);
static void login_cancel_button_clicked(GtkButton * button, gpointer data);
static void login_start_session(void);
static void login_relogin(void);
#if 0	/* currently unused */
static GGZHookReturn login_reconnect(GGZServerEvent id, void *event_data,
				     void *user_data);
#endif

void login_connect_failed(void)
{
	/* Destroy server object */
	server_disconnect();
}


void login_failed(const GGZErrorEventData * error)
{
	GtkWidget *tmp;
	char *msg;

	/* First, disconnect from the server. */
	if (ggzcore_server_logout(server) < 0)
		ggz_error_msg("Error logging out in login_failed");

	/* Re-enable the "connect" button and change it say "Login" */
	tmp = ggz_lookup_widget(login.dialog, "connect_button");
	stockbutton_set_text(tmp, _("Login"));
	gtk_widget_set_sensitive(tmp, TRUE);

	tmp = ggz_lookup_widget(login.dialog, "top_panel");
	gtk_notebook_set_current_page(GTK_NOTEBOOK(tmp), 1);

	tmp = ggz_lookup_widget(login.dialog, "profile_frame");
	gtk_frame_set_label(GTK_FRAME(tmp), _("Sorry!"));

	tmp = ggz_lookup_widget(login.dialog, "msg_label");

	switch (error->status) {
	case E_ALREADY_LOGGED_IN:
		msg = g_strdup(_("That username is already in use."));
		break;
	case E_USR_LOOKUP:
		msg = g_strdup(_("Authentication has failed.\n"
				 "Please supply the correct password."));
		break;
	case E_TOO_LONG:
		msg = g_strdup(_("The username is too long!"));
		break;
	case E_BAD_USERNAME:
		msg = g_strdup(_("Invalid username; "
				 "do not use special characters!"));
		break;
	case E_BAD_PASSWORD:
		msg = g_strdup(_("Invalid password."));
		break;
	case E_USR_TYPE:
	case E_USR_TAKEN:
		msg = g_strdup(_("Username is already taken."));
		break;
	default:
		msg = g_strdup_printf(_("Login failed for unknown "
					"reason %d: %s"),
				      error->status, error->message);
		break;
	}
	gtk_label_set_text(GTK_LABEL(tmp), msg);
	g_free(msg);
}


void login_goto_server(const gchar * server_url)
{
	GtkWidget *tmp;

	main_activate();
	tmp = ggz_lookup_widget(GTK_WIDGET(login.dialog), "host_entry");
	if (!strncasecmp(server_url, "ggz://", 6))
		gtk_entry_set_text(GTK_ENTRY(tmp), server_url + 6);
	else
		gtk_entry_set_text(GTK_ENTRY(tmp), server_url);

	tmp = ggz_lookup_widget(GTK_WIDGET(login.dialog), "name_entry");
	gtk_entry_set_text(GTK_ENTRY(tmp),
			   (gchar *) ggzcore_server_get_handle(server));

	tmp = ggz_lookup_widget(GTK_WIDGET(login.dialog), "guest_radio");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(tmp), TRUE);
}

static void login_fill_defaults(GtkWidget * widget, gpointer user_data)
{
	GtkWidget *tmp;
	GList *items, *item;
	char *last;
	char *profile = user_data;
	int i = 0;

	tmp = ggz_lookup_widget(login.dialog, "profile_combo");

	/* Set to last server connected to */
	if (profile) {
		last = profile;
	} else {
		last = ggzcore_conf_read_string("OPTIONS", "LASTPROFILE",
						"NONE");
	}

	/* Fill profile combo box if there are any profiles */
	if ((items = server_get_name_list())) {
		for (item = g_list_first(items); item;
		     item = g_list_next(item)) {
			char *text = g_list_nth_data(item, 0);

			gtk_combo_box_append_text(GTK_COMBO_BOX(tmp), text);
			if (strcmp(last, text) == 0 || i == 0) {
				gtk_combo_box_set_active(GTK_COMBO_BOX(tmp),
							 i);
			}
			i++;
		}
	} else
		gtk_widget_set_sensitive(tmp, FALSE);

	if (profile) {
		ggz_free(profile);
	} else {
		ggz_free(last);
	}
}


static void login_profile_changed(GtkWidget *widget, gpointer user_data)
{
	Server *server = NULL;
	char *servername;

	servername = gtk_combo_box_get_active_text(GTK_COMBO_BOX(widget));
	if (servername) {
		server = server_get(servername);
	}

	if (!server) {
		/* Profile Not Found */
		return;
	}

	login_set_entries(*server);
}


static void login_edit_profiles(GtkButton * button, gpointer user_data)
{
	props_raise();
}



static void login_entry_changed(GtkEditable * editable, gpointer user_data)
{
	GtkWidget *tmp = NULL;

	if (!login.entries_update) {
		tmp = ggz_lookup_widget(login.dialog, "profile_combo");
		gtk_combo_box_set_active(GTK_COMBO_BOX(tmp), -1);
	}
}


/* Show password box for normal logins and first-time logins only */
/* Show email box for first-time logins only */
static void
login_normal_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
	GtkWidget *tmp;

	if (!login.entries_update) {
		tmp = ggz_lookup_widget(login.dialog, "profile_combo");
		gtk_combo_box_set_active(GTK_COMBO_BOX(tmp), -1);
	}
}


static void
login_guest_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
	GtkWidget *tmp;

	tmp = ggz_lookup_widget(GTK_WIDGET(user_data), "password_box");
	gtk_widget_set_sensitive(tmp, !togglebutton->active);

	if (!login.entries_update) {
		tmp = ggz_lookup_widget(login.dialog, "profile_combo");
		gtk_combo_box_set_active(GTK_COMBO_BOX(tmp), -1);
	}
}


static void
login_first_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
	GtkWidget *tmp;

	tmp = ggz_lookup_widget(GTK_WIDGET(user_data), "email_box");
	gtk_widget_set_sensitive(tmp, togglebutton->active);

	if (!login.entries_update) {
		tmp = ggz_lookup_widget(login.dialog, "profile_combo");
		gtk_combo_box_set_active(GTK_COMBO_BOX(tmp), -1);
	}
}


static void login_connect_button_clicked(GtkButton * button, gpointer data)
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


static void login_cancel_button_clicked(GtkButton * button, gpointer data)
{
	/* If we're already connected, disconnect */
	if (server) {
		if (ggzcore_server_logout(server) < 0)
			ggz_error_msg("Error logging out in "
				      "login_cancel_button_clicked");
		server = NULL;
	}
	if (ggz_closed_cb) {
		(ggz_closed_cb)();
	} else {
		gtk_main_quit();
	}
}


static void login_start_session(void)
{
	GtkWidget *tmp;
	const char *host = NULL, *username = NULL;
	const char *password = NULL, *email = NULL;
	char *sessiondump;
	char *profile;
	int port;
	GGZLoginType type = GGZ_LOGIN_GUEST;

	/* Clearout the room, table and player lists */
	/* FIXME: this should be done as a function in client.c */
	clear_room_list();
	clear_table_list();
	clear_player_list();

	/* FIXME: perhaps this should be done elsewhere? 
	   tmp = g_object_get_data(G_OBJECT(login_dialog), "connect_button");
	   gtk_widget_set_sensitive(tmp, FALSE); */

	/* Get connection and login data */
	tmp = ggz_lookup_widget(login.dialog, "host_entry");
	host = gtk_entry_get_text(GTK_ENTRY(tmp));

	tmp = ggz_lookup_widget(login.dialog, "port_entry");
	port = atoi(gtk_entry_get_text(GTK_ENTRY(tmp)));

	tmp = ggz_lookup_widget(login.dialog, "name_entry");
	username = gtk_entry_get_text(GTK_ENTRY(tmp));

	tmp = ggz_lookup_widget(login.dialog, "normal_radio");
	if (GTK_TOGGLE_BUTTON(tmp)->active) 
		type = GGZ_LOGIN;
	tmp = ggz_lookup_widget(login.dialog, "guest_radio");
	if (GTK_TOGGLE_BUTTON(tmp)->active)
		type = GGZ_LOGIN_GUEST;
	if (!GTK_TOGGLE_BUTTON(tmp)->active) {
		tmp = ggz_lookup_widget(login.dialog,
				      "pass_entry");
		password = gtk_entry_get_text(GTK_ENTRY(tmp));
	}
	tmp = ggz_lookup_widget(login.dialog, "first_radio");
	if (GTK_TOGGLE_BUTTON(tmp)->active)
		type = GGZ_LOGIN_NEW;
	if (GTK_TOGGLE_BUTTON(tmp)->active) {
		tmp = ggz_lookup_widget(login.dialog,
				      "email_entry");
		email = gtk_entry_get_text(GTK_ENTRY(tmp));
	}

	/* Create new server object and set connection/login info */
	server = ggzcore_server_new();
	ggzcore_server_set_hostinfo(server, host, port, 0);
	ggzcore_server_set_logininfo(server, type,
				     username, password, email);

	/* Log server communications to file */
	if (login.option_log) {
		ggzcore_server_log_session(server, login.option_log);
	} else {
		sessiondump =
		    ggzcore_conf_read_string("Debug", "SessionLog", NULL);
		ggzcore_server_log_session(server, sessiondump);
		if (sessiondump)
			ggz_free(sessiondump);
	}

	/* Save as last profile */
	tmp = ggz_lookup_widget(login.dialog, "profile_combo");
	profile = gtk_combo_box_get_active_text(GTK_COMBO_BOX(tmp));
	if (profile && strcmp(profile, "")) {
		ggzcore_conf_write_string("OPTIONS", "LASTPROFILE",
					  profile);
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
	const char *username = NULL, *password = NULL, *email = NULL;
	GGZLoginType type = GGZ_LOGIN_GUEST;

	/* FIXME: perhaps this should be done elsewhere? 
	   tmp = ggz_lookup_widget(login.dialog, "connect_button");
	   gtk_widget_set_sensitive(tmp, FALSE); */

	/* Get login data */
	tmp = ggz_lookup_widget(login.dialog, "name_entry");
	username = gtk_entry_get_text(GTK_ENTRY(tmp));

	tmp = ggz_lookup_widget(login.dialog, "normal_radio");
	if (GTK_TOGGLE_BUTTON(tmp)->active)
		type = GGZ_LOGIN;
	tmp = ggz_lookup_widget(login.dialog, "guest_radio");
	if (GTK_TOGGLE_BUTTON(tmp)->active)
		type = GGZ_LOGIN_GUEST;
	if (!GTK_TOGGLE_BUTTON(tmp)->active) {
		tmp = ggz_lookup_widget(login.dialog, "pass_entry");
		password = gtk_entry_get_text(GTK_ENTRY(tmp));
	}
	tmp = ggz_lookup_widget(login.dialog, "first_radio");
	if (GTK_TOGGLE_BUTTON(tmp)->active)
		type = GGZ_LOGIN_NEW;
	if (GTK_TOGGLE_BUTTON(tmp)->active) {
		tmp = ggz_lookup_widget(login.dialog,
				      "email_entry");
		email = gtk_entry_get_text(GTK_ENTRY(tmp));
	}

	ggzcore_server_set_logininfo(server, type,
				     username, password, email);
	ggzcore_server_login(server);
}


#if 0	/* currently unused */
static GGZHookReturn login_reconnect(GGZServerEvent id, void *event_data,
				     void *user_data)
{
	/* Now that we're disconnected, login to new server */
	/* FIXME: provide a destroy function that frees profile */

	return GGZ_HOOK_REMOVE;
}
#endif


void login_set_entries(Server server)
{
	GtkWidget *tmp;
	gchar *port;

	login.entries_update = TRUE;

	/*
	   tmp = ggz_lookup_widget(login.dialog, "profile_entry");
	   if (server.name)
	   gtk_entry_set_text(GTK_ENTRY(tmp), server.name);
	 */

	tmp = ggz_lookup_widget(login.dialog, "host_entry");
	gtk_entry_set_text(GTK_ENTRY(tmp), server.host);

	tmp = ggz_lookup_widget(login.dialog, "port_entry");
	port = g_strdup_printf("%d", server.port);
	gtk_entry_set_text(GTK_ENTRY(tmp), port);
	g_free(port);

	switch (server.type) {
	case GGZ_LOGIN:
		tmp = ggz_lookup_widget(login.dialog, "normal_radio");
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(tmp), TRUE);
		break;
	case GGZ_LOGIN_GUEST:
		tmp = ggz_lookup_widget(login.dialog, "guest_radio");
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(tmp), TRUE);
		break;
	case GGZ_LOGIN_NEW:
		tmp = ggz_lookup_widget(login.dialog, "first_radio");
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(tmp), TRUE);
		break;
	}

	tmp = ggz_lookup_widget(login.dialog, "name_entry");
	gtk_entry_set_text(GTK_ENTRY(tmp), server.login);

	if (server.type == GGZ_LOGIN && server.password != NULL) {
		tmp = ggz_lookup_widget(login.dialog, "pass_entry");
		gtk_entry_set_text(GTK_ENTRY(tmp), server.password);
	}

	login.entries_update = FALSE;
}

void login_set_sensitive(gboolean sensitive)
{
	GtkWidget *tmp;

	if (!login.dialog) return;

	/* Desensitize the connect button */
	tmp = ggz_lookup_widget(login.dialog, "connect_button");
	gtk_widget_set_sensitive(tmp, sensitive);
}

void login_set_option_log(const char *option_log)
{
	login.option_log = option_log;
}

GtkWidget *create_dlg_login(const char *default_profile)
{
	GtkWidget *dlg_login;
	GtkWidget *profile_frame;
	GtkWidget *entries_box;
	GtkWidget *top_panel;
	GtkWidget *top_box;
	GtkWidget *profile_box;
	GtkWidget *profile_label;
	GtkWidget *profile_combo;
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
	GtkWidget *email_box;
	GtkWidget *email_label;
	GtkWidget *email_entry;
	GtkWidget *radio_box;
	GSList *login_type_group = NULL;
	GtkWidget *normal_radio;
	GtkWidget *guest_radio;
	GtkWidget *first_radio;
	GtkWidget *hbuttonbox;
	GtkWidget *connect_button;
	GtkWidget *cancel_button;

	dlg_login = gtk_vbox_new(FALSE, 0);

	/* Set global value. */
	login.dialog = dlg_login;

	profile_frame = gtk_frame_new(_("Server Profile"));
	g_object_set_data(G_OBJECT(dlg_login), "profile_frame", profile_frame);
	gtk_box_pack_start(GTK_BOX(dlg_login), profile_frame, TRUE,
			   TRUE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(profile_frame), 10);

	entries_box = gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(profile_frame), entries_box);
	gtk_container_set_border_width(GTK_CONTAINER(entries_box), 5);

	top_panel = gtk_notebook_new();
	g_object_set_data(G_OBJECT(dlg_login), "top_panel", top_panel);
	gtk_box_pack_start(GTK_BOX(entries_box), top_panel, TRUE, TRUE, 3);
	GTK_WIDGET_UNSET_FLAGS(top_panel, GTK_CAN_FOCUS);
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(top_panel), FALSE);
	gtk_notebook_set_show_border(GTK_NOTEBOOK(top_panel), FALSE);

	top_box = gtk_vbox_new(FALSE, 3);
	gtk_container_add(GTK_CONTAINER(top_panel), top_box);
	gtk_container_set_border_width(GTK_CONTAINER(top_box), 3);

	profile_box = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(top_box), profile_box, TRUE, TRUE, 0);

	profile_label = gtk_label_new(_("Profile:"));
	gtk_box_pack_start(GTK_BOX(profile_box), profile_label, FALSE,
			   TRUE, 0);
	gtk_misc_set_alignment(GTK_MISC(profile_label), 1, 0.5);

	profile_combo = gtk_combo_box_new_text();
	g_object_set_data(G_OBJECT(dlg_login), "profile_combo", profile_combo);
	gtk_box_pack_start(GTK_BOX(profile_box), profile_combo, FALSE,
			   TRUE, 5);

	profile_button_box = gtk_hbutton_box_new();
	gtk_box_pack_start(GTK_BOX(profile_box), profile_button_box, FALSE,
			   FALSE, 5);

	edit_profiles_button = stockbutton_new(GTK_STOCK_PREFERENCES,
					       _("Edit Profiles..."));
	gtk_container_add(GTK_CONTAINER(profile_button_box),
			  edit_profiles_button);
	GTK_WIDGET_SET_FLAGS(edit_profiles_button, GTK_CAN_DEFAULT);

	server_box = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(top_box), server_box, TRUE, TRUE, 0);

	server_label = gtk_label_new(_("Server:"));
	gtk_box_pack_start(GTK_BOX(server_box), server_label, FALSE, TRUE,
			   0);
	gtk_misc_set_alignment(GTK_MISC(server_label), 1, 0.5);

	host_entry = gtk_entry_new();
	g_object_set_data(G_OBJECT(dlg_login), "host_entry", host_entry);
	gtk_entry_set_max_length(GTK_ENTRY(host_entry), 256);
	gtk_box_pack_start(GTK_BOX(server_box), host_entry, FALSE, FALSE,
			   5);
	gtk_entry_set_text(GTK_ENTRY(host_entry), "localhost");

	port_label = gtk_label_new(_("Port:"));
	gtk_box_pack_start(GTK_BOX(server_box), port_label, FALSE, TRUE,
			   2);
	gtk_misc_set_alignment(GTK_MISC(port_label), 1, 0.5);

	port_entry = gtk_entry_new();
	g_object_set_data(G_OBJECT(dlg_login), "port_entry", port_entry);
	gtk_entry_set_max_length(GTK_ENTRY(port_entry), 5);
	gtk_box_pack_start(GTK_BOX(server_box), port_entry, FALSE, FALSE,
			   0);
	gtk_entry_set_text(GTK_ENTRY(port_entry), "5688");

	msg_label = gtk_label_new("");
	g_object_set_data(G_OBJECT(dlg_login), "msg_label", msg_label);
	gtk_container_add(GTK_CONTAINER(top_panel), msg_label);

	hseparator = gtk_hseparator_new();
	gtk_box_pack_start(GTK_BOX(entries_box), hseparator, TRUE, TRUE,
			   7);

	login_box = gtk_hbox_new(FALSE, 10);
	gtk_box_pack_start(GTK_BOX(entries_box), login_box, FALSE, FALSE,
			   1);

	user_box = gtk_vbox_new(TRUE, 5);
	gtk_box_pack_start(GTK_BOX(login_box), user_box, FALSE, FALSE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(user_box), 10);

	username_box = gtk_hbox_new(FALSE, 5);
	gtk_box_pack_start(GTK_BOX(user_box), username_box, TRUE, TRUE, 0);

	user_label = gtk_label_new(_("Username:"));
	gtk_box_pack_start(GTK_BOX(username_box), user_label, TRUE, TRUE,
			   0);
	gtk_misc_set_alignment(GTK_MISC(user_label), 1, 0.5);

	name_entry = gtk_entry_new();
	g_object_set_data(G_OBJECT(dlg_login), "name_entry", name_entry);
	gtk_entry_set_max_length(GTK_ENTRY(name_entry), 16);
	gtk_box_pack_start(GTK_BOX(username_box), name_entry, FALSE, TRUE,
			   0);

	password_box = gtk_hbox_new(FALSE, 5);
	g_object_set_data(G_OBJECT(dlg_login), "password_box", password_box);
	gtk_box_pack_start(GTK_BOX(user_box), password_box, TRUE, TRUE, 0);

	pass_label = gtk_label_new(_("Password:"));
	gtk_box_pack_start(GTK_BOX(password_box), pass_label, TRUE, TRUE,
			   0);
	gtk_misc_set_alignment(GTK_MISC(pass_label), 1, 0.5);

	pass_entry = gtk_entry_new();
	g_object_set_data(G_OBJECT(dlg_login), "pass_entry", pass_entry);
	gtk_box_pack_start(GTK_BOX(password_box), pass_entry, FALSE, TRUE,
			   0);
	gtk_entry_set_visibility(GTK_ENTRY(pass_entry), FALSE);

	email_box = gtk_hbox_new(FALSE, 5);
	g_object_set_data(G_OBJECT(dlg_login), "email_box", email_box);
	gtk_box_pack_start(GTK_BOX(user_box), email_box, TRUE, TRUE, 0);
	gtk_widget_set_sensitive(email_box, FALSE);

	email_label = gtk_label_new(_("Email:"));
	gtk_box_pack_start(GTK_BOX(email_box), email_label, TRUE, TRUE,
			   0);
	gtk_misc_set_alignment(GTK_MISC(email_label), 1, 0.5);

	email_entry = gtk_entry_new();
	g_object_set_data(G_OBJECT(dlg_login), "email_entry", email_entry);
	gtk_box_pack_start(GTK_BOX(email_box), email_entry, FALSE, TRUE,
			   0);
	gtk_entry_set_visibility(GTK_ENTRY(pass_entry), FALSE);

	radio_box = gtk_vbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(login_box), radio_box, FALSE, FALSE, 0);

	normal_radio =
	    gtk_radio_button_new_with_label(login_type_group,
					    _("Normal Login"));
	g_object_set_data(G_OBJECT(dlg_login), "normal_radio", normal_radio);
	login_type_group =
	    gtk_radio_button_get_group(GTK_RADIO_BUTTON(normal_radio));
	gtk_box_pack_start(GTK_BOX(radio_box), normal_radio, TRUE, TRUE,
			   0);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(normal_radio),
				     TRUE);

	guest_radio =
	    gtk_radio_button_new_with_label(login_type_group,
					    _("Guest Login"));
	g_object_set_data(G_OBJECT(dlg_login), "guest_radio", guest_radio);
	login_type_group =
	    gtk_radio_button_get_group(GTK_RADIO_BUTTON(guest_radio));
	gtk_box_pack_start(GTK_BOX(radio_box), guest_radio, TRUE, TRUE, 0);

	first_radio =
	    gtk_radio_button_new_with_label(login_type_group,
					    _("First-time Login"));
	g_object_set_data(G_OBJECT(dlg_login), "first_radio", first_radio);
	login_type_group =
	    gtk_radio_button_get_group(GTK_RADIO_BUTTON(first_radio));
	gtk_box_pack_start(GTK_BOX(radio_box), first_radio, TRUE, TRUE, 0);

	hbuttonbox = gtk_hbutton_box_new();
	gtk_box_pack_start(GTK_BOX(dlg_login), hbuttonbox, TRUE,
			   TRUE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(hbuttonbox), 5);

	if (ggz_closed_cb) {
		cancel_button = gtk_button_new_from_stock(GTK_STOCK_CANCEL);
	} else {
		cancel_button = gtk_button_new_from_stock(GTK_STOCK_QUIT);
	}
	gtk_container_add(GTK_CONTAINER(hbuttonbox), cancel_button);
	GTK_WIDGET_SET_FLAGS(cancel_button, GTK_CAN_DEFAULT);

	connect_button = stockbutton_new(GTK_STOCK_JUMP_TO, _("Connect"));
	g_object_set_data(G_OBJECT(dlg_login),
			  "connect_button", connect_button);
	gtk_container_add(GTK_CONTAINER(hbuttonbox), connect_button);
	GTK_WIDGET_SET_FLAGS(connect_button, GTK_CAN_DEFAULT);

	g_signal_connect(GTK_OBJECT(dlg_login), "realize",
			 GTK_SIGNAL_FUNC(login_fill_defaults),
			 ggz_strdup(default_profile));
	g_signal_connect(GTK_OBJECT(dlg_login), "destroy",
			 GTK_SIGNAL_FUNC(gtk_widget_destroyed),
			 &login.dialog);
	g_signal_connect(GTK_OBJECT(profile_combo), "changed",
			 GTK_SIGNAL_FUNC(login_profile_changed),
			 dlg_login);
	g_signal_connect(GTK_OBJECT(edit_profiles_button), "clicked",
			 GTK_SIGNAL_FUNC(login_edit_profiles), dlg_login);
	g_signal_connect(GTK_OBJECT(host_entry), "changed",
			 GTK_SIGNAL_FUNC(login_entry_changed), NULL);
	g_signal_connect(GTK_OBJECT(name_entry), "changed",
			 GTK_SIGNAL_FUNC(login_entry_changed), NULL);
	g_signal_connect(GTK_OBJECT(pass_entry), "changed",
			 GTK_SIGNAL_FUNC(login_entry_changed), NULL);
	g_signal_connect(GTK_OBJECT(normal_radio), "toggled",
			 GTK_SIGNAL_FUNC(login_normal_toggled), dlg_login);
	g_signal_connect(GTK_OBJECT(guest_radio), "toggled",
			 GTK_SIGNAL_FUNC(login_guest_toggled), dlg_login);
	g_signal_connect(GTK_OBJECT(first_radio), "toggled",
			 GTK_SIGNAL_FUNC(login_first_toggled), dlg_login);
	g_signal_connect(GTK_OBJECT(connect_button), "clicked",
			 GTK_SIGNAL_FUNC(login_connect_button_clicked),
			 NULL);
	g_signal_connect(GTK_OBJECT(cancel_button), "clicked",
			 GTK_SIGNAL_FUNC(login_cancel_button_clicked),
			 NULL);

	return dlg_login;
}
