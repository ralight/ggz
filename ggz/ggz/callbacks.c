/*
 * File: callbacks.c
 * Author: Brent Hendricks
 * Project: GGZ Client
 * Date: 2/18/00
 *
 * This file contains callbacks for GTK windows generated by glade
 *
 * Copyright (C) 1998 Brent Hendricks.
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

#include <gtk/gtk.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include "connect.h"
#include "dlg_error.h"
#include "protocols.h"
#include "datatypes.h"
#include "game.h"
#include "callbacks.h"
#include "interface.h"
#include "support.h"
#include "err_func.h"


/* Global data */
extern GtkWidget *detail_window;
extern GtkWidget *main_win;
extern GtkWidget *dlg_launch;
extern struct ConnectInfo connection;
extern int selected_table;
extern struct GameTypes game_types;


/*				*
 *	Login dialog		*
 *				*/

void anon_toggled(GtkWidget* button, gpointer window) 
{
	gpointer tmp;
	tmp = gtk_object_get_data(GTK_OBJECT(window), "pass_box");
	
	if (GTK_TOGGLE_BUTTON(button)->active)
		gtk_widget_hide(GTK_WIDGET(tmp));
	else 
		gtk_widget_show(GTK_WIDGET(tmp));
}


void fill_defaults(GtkWidget * win, gpointer user_data)
{
	gpointer *tmp;
	char port[5];

	tmp = gtk_object_get_data(GTK_OBJECT(win), "name_entry");
	if (connection.username)
		gtk_entry_set_text(GTK_ENTRY(tmp), connection.username);
	else 
		gtk_entry_set_text(GTK_ENTRY(tmp), getenv("LOGNAME"));
		

	tmp = gtk_object_get_data(GTK_OBJECT(win), "host_entry");
	if (connection.server)
		gtk_entry_set_text(GTK_ENTRY(tmp), connection.server);
	else
		gtk_entry_set_text(GTK_ENTRY(tmp), "localhost");

	tmp = gtk_object_get_data(GTK_OBJECT(win), "port_entry");
	if (connection.port) {
		snprintf(port, 5, "%d", connection.port);
		gtk_entry_set_text(GTK_ENTRY(tmp), port);
	}
	else
		gtk_entry_set_text(GTK_ENTRY(tmp), "7626");

	tmp = gtk_object_get_data(GTK_OBJECT(win), "anon_radio");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(tmp), TRUE);
}


void input_options(GtkButton * button, gpointer window)
{
        gpointer tmp;

	tmp = gtk_object_get_data(GTK_OBJECT(window), "name_entry");
	connection.username = g_strdup(gtk_entry_get_text(GTK_ENTRY(tmp)));

	tmp = gtk_object_get_data(GTK_OBJECT(window), "pass_entry");
	connection.password = g_strdup(gtk_entry_get_text(GTK_ENTRY(tmp)));
	
	tmp = gtk_object_get_data(GTK_OBJECT(window), "host_entry");
	connection.server = g_strdup(gtk_entry_get_text(GTK_ENTRY(tmp)));

	tmp = gtk_object_get_data(GTK_OBJECT(window), "port_entry");
	connection.port = atoi(gtk_entry_get_text(GTK_ENTRY(tmp)));
	
	tmp = gtk_object_get_data(GTK_OBJECT(window), "normal_radio");
	if (GTK_TOGGLE_BUTTON(tmp)->active)
		connection.login_type = GGZ_LOGIN;

	tmp = gtk_object_get_data(GTK_OBJECT(window), "anon_radio");
	if (GTK_TOGGLE_BUTTON(tmp)->active)
		connection.login_type = GGZ_LOGIN_ANON;

	tmp = gtk_object_get_data(GTK_OBJECT(window), "first_radio");
	if (GTK_TOGGLE_BUTTON(tmp)->active)
		connection.login_type = GGZ_LOGIN_NEW;
}


void start_session(GtkButton * button, gpointer window)
{
	if (connection.connected) {
		warn_dlg("Already Connected.");
		return;
	}
	
	/* FIXME: Initialize for new game session */
	
	if (connect_to_server() < 0) {
		err_dlg("Could not connect");
		return;
	}

	/* Close connect dialog if we were successful */
	connection.connected = TRUE;
	gtk_widget_destroy(GTK_WIDGET(window));

	/*FIXME: Other session starting things ? */
}


void show_details(GtkButton * button, gpointer user_data)
{
	detail_window = create_dlg_details();
	gtk_widget_show(detail_window);
}


/*				*
 *	Details dialog		*
 *				*/

void cancel_details(GtkButton * button, gpointer user_data)
{
	gtk_widget_destroy(detail_window);
	detail_window = NULL;
}


/*				*
 *	Launch Game dialog	*
 *				*/

void get_game_options(GtkButton * button, gpointer user_data)
{

	GtkWidget *temp_widget;
	GList *combo_items = NULL;
	int count;

	if (!connection.connected)
		warn_dlg("Not connected!");
	else {
		dlg_launch = create_dlgLaunch();
	        temp_widget = gtk_object_get_data(GTK_OBJECT(dlg_launch), "lblUser");
		gtk_label_set_text (GTK_LABEL (temp_widget), connection.username);
		for (count=0; count<game_types.count; count++){
			combo_items = g_list_append (combo_items, game_types.info[count].name);
		}
	        temp_widget = gtk_object_get_data(GTK_OBJECT(dlg_launch), "combo1");
		gtk_combo_set_popdown_strings (GTK_COMBO (temp_widget), combo_items);
		g_list_free (combo_items);

		gtk_widget_show(dlg_launch);
	}

}

void launch_change_type(GtkCombo *type_combo, gpointer user_data)
{
	/* FIXME: Disable/Enable the player dropdowns
		based on the total number of players
		aloud in the game.
	*/

	/* FIXME: If the game HAS to have a total of n players
		remove the "Closed" option from the drop down.
	*/
}

void launch_start_game(GtkWidget *btn_launch, gpointer user_data)
{
	/*
		BIG FIXME: Currently this just launches like
			it has in the past, but the dialog is
			in place. Things that will happen here are
			1) Get the right game type
			2) Get player settings
			3) Launch the game with right options
	*/

	int type = 0;

	gtk_widget_destroy(dlg_launch);
	dlg_launch = NULL;

	if (!connection.connected)
		warn_dlg("Not connected!");
	else {
		launch_game(type, 1);
	}

}

/*				*
 *	Main GGZ client window	*
 *				*/

void join_game(GtkButton * button, gpointer user_data)
{
	dbg_msg("joining game");
	es_write_int(connection.sock, REQ_TABLE_JOIN);
	es_write_int(connection.sock, selected_table);
}

void logout(GtkMenuItem * menuitem, gpointer user_data)
{
	dbg_msg("Logging out");
	es_write_int(connection.sock, REQ_LOGOUT);
}


void get_types(GtkMenuItem * menuitem, gpointer user_data)
{
	char verbose = 1;

	es_write_int(connection.sock, REQ_LIST_TYPES);
	write(connection.sock, &verbose, 1);
}


void get_players(GtkMenuItem * menuitem, gpointer user_data)
{
	es_write_int(connection.sock, REQ_LIST_PLAYERS);
}


void get_tables(GtkMenuItem * menuitem, gpointer user_data)
{
	es_write_int(connection.sock, REQ_LIST_TABLES);
	es_write_int(connection.sock, -1);
}


void input_chat_msg(GtkWidget * widget, gpointer user_data)
{
	if (!connection.connected) {
		err_dlg("Not Connected");
		return;
	}
	
	if (strcmp(gtk_entry_get_text(GTK_ENTRY(user_data)), "") != 0
	    && es_write_int(connection.sock, REQ_CHAT) > 0)
		
		es_write_string(connection.sock,
				gtk_entry_get_text(GTK_ENTRY(user_data)));
	
	gtk_entry_set_text(GTK_ENTRY(user_data), "");
}


void table_select_row_callback(GtkWidget *widget, gint row, gint column,
			       GdkEventButton *event, gpointer data)
{
	gchar *text;

	gtk_clist_get_text(GTK_CLIST(widget), row, 1, &text);
	selected_table = atoi(text);
}


