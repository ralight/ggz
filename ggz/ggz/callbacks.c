/*
 * File: callbacks.c
 * Author: Brent Hendricks
 * Project: GGZ
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

#include "client.h"
#include "connect.h"
/*#include <gtk_dlg_options.h>*/
#include "dlg_error.h"
#include "protocols.h"
#include "datatypes.h"
#include "game.h"
#include "callbacks.h"
#include "interface.h"
#include "support.h"
#include "err_func.h"


GtkWidget* detail_window = NULL;
extern struct ConnectInfo connection;

void
InputOptions                           (GtkButton       *button,
                                        gpointer         user_data)
{
  
  GtkWidget* tmp;

  tmp = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(user_data),"name_entry"));
  connection.username = g_strdup(gtk_entry_get_text( GTK_ENTRY(tmp)));

  tmp = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(user_data),"pass_entry"));
  connection.password = g_strdup(gtk_entry_get_text( GTK_ENTRY(tmp)));

  tmp = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(user_data),"host_entry"));
  connection.server = g_strdup( gtk_entry_get_text( GTK_ENTRY(tmp) ) );

  tmp = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(user_data),"port_entry"));
  connection.port = atoi( gtk_entry_get_text( GTK_ENTRY(tmp) ) );
  
  tmp = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(user_data),"normal_radio"));
  if( GTK_TOGGLE_BUTTON(tmp)->active )
    connection.login_type = GGZ_LOGIN;

  tmp = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(user_data),"anon_radio"));
  if( GTK_TOGGLE_BUTTON(tmp)->active ) 
    connection.login_type = GGZ_LOGIN_ANON;

  tmp = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(user_data),"first_radio"));
  if( GTK_TOGGLE_BUTTON(tmp)->active ) 
    connection.login_type = GGZ_LOGIN_NEW;
  
  
}

void
StartSession                           (GtkButton       *button,
                                        gpointer         user_data)
{

	if (connection.connected) {
		DisplayWarning("Already Connected.");
		return;
	}
	
	/* FIXME: Initialize for new game session */
	
	if (connect_to_server() < 0) {
		DisplayError("Could not connect");
		return;
	}

	/* Close connect dialog if we were successful*/
	connection.connected = TRUE;
	gtk_widget_destroy(GTK_WIDGET(user_data));
	/*FIXME: Other session starting things ? */
}


void
show_details                           (GtkButton       *button,
                                        gpointer         user_data)
{
  detail_window = create_dlg_details();
  gtk_widget_show(detail_window);
}


void
cancel_details                         (GtkButton       *button,
                                        gpointer         user_data)
{
  gtk_widget_destroy(detail_window);
  detail_window = NULL;
}



void
join_game                              (GtkButton       *button,
                                        gpointer         user_data) 
{

	/* FIXME: Don't hardcode table numnber!*/
	dbg_msg("joining game");
	CheckWriteInt(connection.sock, REQ_JOIN_GAME);
	CheckWriteInt(connection.sock, 0);
	launch_game(0, 0);
}


void
get_game_options                       (GtkButton       *button,
                                        gpointer         user_data) 
{
	int type = 0;	/* FIXME: Input type of game to launch */
	
	if (!connection.connected) 
		DisplayWarning("Not connected!");
	else {
		launch_game(type, 1);
	}
}


void
logout                                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	dbg_msg("Logging out");
	CheckWriteInt(connection.sock, REQ_LOGOUT);
}


void
get_types                              (GtkMenuItem     *menuitem,
                                        gpointer         user_data) 
{
  char verbose = 1;
  
  CheckWriteInt(connection.sock, REQ_GAME_TYPES);
  write(connection.sock, &verbose, 1);
}

void
get_players                            (GtkMenuItem     *menuitem,
                                        gpointer         user_data) 
{
  CheckWriteInt(connection.sock, REQ_USER_LIST);
}

void
get_tables                             (GtkMenuItem     *menuitem,
                                        gpointer         user_data) 
{
  CheckWriteInt(connection.sock, REQ_TABLE_LIST);
  CheckWriteInt(connection.sock, -1);
}




void
fill_defaults                          (GtkWidget       *win,
                                        gpointer         user_data) 
{
	GtkWidget* tmp;
	char port[5];
	
	if (connection.username) {
		tmp = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(win), 
						     "name_entry"));
		gtk_entry_set_text(GTK_ENTRY(tmp), connection.username);
	}
	
	if (connection.server) {
		tmp = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(win), 
						     "host_entry"));
		gtk_entry_set_text(GTK_ENTRY(tmp), connection.server);
	}
	
	tmp = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(win), "port_entry"));
	snprintf(port, 5, "%d", connection.port);
	gtk_entry_set_text(GTK_ENTRY(tmp), port );
}


void
input_chat_msg                         (GtkWidget        *widget,
                                        gpointer         user_data) {
	
	/* FIXME: Check to see if connected */
	if (strcmp(gtk_entry_get_text(GTK_ENTRY(user_data)), "") != 0
	    && CheckWriteInt( connection.sock, REQ_CHAT ) == NET_OK)
		
		CheckWriteString(connection.sock, 
				 gtk_entry_get_text(GTK_ENTRY(user_data)));
	
	gtk_entry_set_text(GTK_ENTRY(user_data), "");
}

void
NormalOption                           (GtkWidget       *widget,
                                        gpointer         user_data) 
{
	GtkWidget* tmpWidget;
	tmpWidget = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(dlg_login),"pass_box"));
	gtk_widget_show (tmpWidget);
}

void
AnonOption                             (GtkWidget       *widget,
                                        gpointer         user_data) 
{
	GtkWidget* tmpWidget;
	tmpWidget = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(dlg_login),"pass_box"));
	gtk_widget_hide (tmpWidget);
}

void
FirstOption                            (GtkWidget       *Widget,
                                        gpointer         user_data) 
{
	GtkWidget* tmpWidget;
	tmpWidget = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(dlg_login),"pass_box"));
	gtk_widget_show (tmpWidget);
}




