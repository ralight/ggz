/*
 * File: connect.c
 * Author: Brent Hendricks
 * Project: GGZ Client
 * Date: 1/23/99
 *
 * This fils contains functions for connecting with the server
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


#include <config.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <gtk/gtk.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "chat.h"
#include "connect.h"
#include "datatypes.h"
#include "ggzrc.h"
#include "dlg_error.h"
#include "dlg_login.h"
#include "dlg_main.h"
#include "dlg_msgbox.h"
#include "dlg_motd.h"
#include "easysock.h"
#include "err_func.h"
#include "game.h"
#include "player.h"
#include "protocols.h"
#include "seats.h"
#include "xtext.h"
#include "support.h"
#include "table.h"

/* Global state of game variable */
extern struct ConnectInfo client;
extern struct Game game;
extern struct GameTypes game_types;
extern GtkWidget *dlg_details;
extern GtkWidget *main_win;
extern GtkWidget *mnu_tables;
extern GtkWidget *dlg_login;
extern GtkWidget *dlg_motd;
extern gint selected_table;
extern gint selected_type;
extern GdkColor colors[];
extern struct Rooms room_info;
extern gint new_type;

/* Various local handles */
static guint sock_handle;
static void connect_msg(const gchar *, ...);
static void handle_server_fd(gpointer, gint, GdkInputCondition);
static void handle_list_types(gint op, gint fd);
static void handle_list_tables(gint op, gint fd);
static void handle_list_players(gint op, gint fd);
static void handle_update_tables(gint op, gint fd);
static void handle_update_players(gint op, gint fd);
static void motd_print_line(gchar *line);
void display_rooms();

gchar *opcode_str[] = {
        "MSG_SERVER_ID", 
        "MSG_SERVER_FULL",
        "MSG_MOTD",
        "MSG_CHAT",  
        "MSG_UPDATE_PLAYERS",
        "MSG_UPDATE_TYPES",
        "MSG_UPDATE_TABLES",
        "MSG_UPDATE_ROOMS",
        "MSG_ERROR",
        
        "RSP_LOGIN_NEW", 
        "RSP_LOGIN",
        "RSP_LOGIN_ANON",
        "RSP_LOGOUT",
        "RSP_PREF_CHANGE",   
        "RSP_REMOVE_USER", 

        "RSP_LIST_PLAYERS",
        "RSP_LIST_TYPES",
        "RSP_LIST_TABLES",
        "RSP_LIST_ROOMS",
        "RSP_TABLE_OPTIONS",
        "RSP_USER_STAT", 

        "RSP_TABLE_LAUNCH",  
        "RSP_TABLE_JOIN",  
        "RSP_TABLE_LEAVE",

        "RSP_GAME",
        "RSP_CHAT",
        "RSP_MOTD",
        
        "RSP_ROOM_JOIN"  

};


/**
 * Begin client to server.  
 */
gint connect_to_server(void)
{
	GtkWidget *tmp;

	if (FAIL(client.sock = es_make_socket(ES_CLIENT, client.server.port,
						  client.server.host))) 
	{
		tmp = gtk_object_get_data(GTK_OBJECT(dlg_login), "connect_button");
        	gtk_widget_set_sensitive(GTK_WIDGET(tmp),TRUE); 
		return -1;
	}
	sock_handle = gdk_input_add_full(client.sock, GDK_INPUT_READ,
					 handle_server_fd, NULL, NULL);

	ggz_update_title();
	return 0;
}


/*
 * disconnect from server, close sockets, etc.
 */
void disconnect(GtkWidget * widget, gpointer data)
{
	if (sock_handle)
		gdk_input_remove(sock_handle);
	sock_handle = 0;

	close(client.sock);
	client.connected = FALSE;
	client.cur_room=-3;
	client.new_room=-1;
}


/**
 * Read data from the Spades server 
 */
void handle_server_fd(gpointer data, gint source, GdkInputCondition cond)
{
	gpointer tmp;
	gchar *message;
	gchar name[MAX_USER_NAME_LEN + 1];
	gchar password[17];
	gchar reservation_flag;
	gchar status;
	gint i_status;
	guchar subop;
	gint num, op, size, checksum, count, i;
	gchar buf[4096];
	gchar *tmpstr;

        if (FAIL(es_read_int(source, &op))) {
	        disconnect(NULL, NULL);
	        return;
	}
	
	switch (op) {
	case MSG_SERVER_ID:
		es_read_string_alloc(source, &message);
		es_read_int(source, &num);
		es_read_int(source, &size);
		connect_msg("[%s] %s\n", opcode_str[op], message);
		connect_msg("[%s] Protocol version %d\n", opcode_str[op], num);
		connect_msg("[%s] Max chat length %d\n", opcode_str[op], size);
		if (num != GGZ_CS_PROTO_VERSION) {
			err_dlg("Protocol version mistamch!");
			login_disconnect();
		}
		else {
			switch (client.server.type) {
			case GGZ_LOGIN:	/*Normal login */
				normal_login();
				break;
			case GGZ_LOGIN_GUEST:	/*Guest login */
				anon_login();
				break;
			case GGZ_LOGIN_NEW:	/*First time login */
				new_login();
				break;
			}
		}
		break;
		
	case MSG_SERVER_FULL:
		break;

	case RSP_LOGIN_NEW:
		es_read_char(source, &status);
		connect_msg("[%s] %d\n", opcode_str[op], status);
		if(status < 0) {
			login_bad_name("Sorry!", "That username is already in usage,\nor not permitted on this server.\n\nPlease choose a different name");
			return;
		}
		
		/* Read in our server assigned password */
		es_read_string(source, password, 16);
		note_dlg("The server has assigned the password\n'%s' to you. Please write this down\nand change it as soon as possible.\n\nYou will need it to login in the future.", password);
		
		es_read_int(source, &checksum);
		login_ok();
		login_online();
		connect_msg("[%s] Checksum = %d\n", opcode_str[op], checksum);

		/* Get Room and type lists */
                es_write_int(client.sock, REQ_LIST_ROOMS);
                es_write_int(client.sock, -1);
                es_write_char(client.sock, 1);
		es_write_int(client.sock, REQ_LIST_TYPES);
		es_write_char(client.sock, 1);
		break;
			
	case RSP_LOGIN:
		es_read_char(source, &status);
		connect_msg("[%s] %d\n", opcode_str[op], status);
		if(status < 0) {
			login_bad_name("Denied", "Login denied, please check your username\nand password before trying again.");
			
			return;
		}
		es_read_int(source, &checksum);
		es_read_char(source, &reservation_flag);
		login_ok();
		login_online();
		connect_msg("[%s] Checksum = %d\n", opcode_str[op], checksum);
		connect_msg("[%s] Reservation_Flag = %d\n", opcode_str[op],
			    reservation_flag);

		/* Get Room and type lists */
                es_write_int(client.sock, REQ_LIST_ROOMS);
                es_write_int(client.sock, -1);
                es_write_char(client.sock, 1);
		es_write_int(client.sock, REQ_LIST_TYPES);
		es_write_char(client.sock, 1);
		break;

	case RSP_LOGIN_ANON:
		es_read_char(source, &status);
		connect_msg("[%s] %d\n", opcode_str[op], status);
		if (status < 0) {
			login_bad_name("Sorry!", "That username is already in usage,\n or not permitted on this server.\n\nPlease choose a different name");
			
			return;
		}
		es_read_int(source, &checksum);
		login_ok();
		login_online();
		connect_msg("[%s] Checksum = %d\n", opcode_str[op], checksum);

		/* Get Room and type lists */
                es_write_int(client.sock, REQ_LIST_ROOMS);
                es_write_int(client.sock, -1);
                es_write_char(client.sock, 1);
		es_write_int(client.sock, REQ_LIST_TYPES);
		es_write_char(client.sock, 1);
		break;

	case RSP_TABLE_LAUNCH:
		es_read_char(source, &status);
		connect_msg("[%s] %d\n", opcode_str[op], status);
		switch (status) {
		case E_NOT_IN_ROOM:
			warn_dlg("Must be in room to launch table");
			break;
		case E_BAD_OPTIONS:
			warn_dlg("Your table launch was rejected due to invalid seat assignments");
			break;
		case 0:
			tmp = lookup_widget(main_win, "launch");
			gtk_widget_set_sensitive(GTK_WIDGET(tmp),FALSE);
			tmp = lookup_widget(main_win, "join");
			gtk_widget_set_sensitive(GTK_WIDGET(tmp),FALSE);
			tmp = lookup_widget(main_win, "join");
			gtk_widget_set_sensitive(GTK_WIDGET(tmp),FALSE);
			tmp = lookup_widget(main_win, "launch_button");
			gtk_widget_set_sensitive(GTK_WIDGET(tmp),FALSE);
			tmp = lookup_widget(main_win, "join_button");
			gtk_widget_set_sensitive(GTK_WIDGET(tmp),FALSE);
			tmp = lookup_widget(mnu_tables, "launch");
			gtk_widget_set_sensitive(GTK_WIDGET(tmp),FALSE);
			tmp = lookup_widget(mnu_tables, "join1");
			gtk_widget_set_sensitive(GTK_WIDGET(tmp),FALSE);
			tmp = lookup_widget(main_win, "room_combo");
			gtk_widget_set_sensitive(GTK_WIDGET(tmp),FALSE);
			
			client.playing = TRUE;
			launch_game(new_type, TRUE);
			break;
		}
		break;

	case RSP_TABLE_JOIN:
		es_read_char(source, &status);
		connect_msg("[%s] %d\n", opcode_str[op], status);
		switch (status) {
		case 0:
			/* break if already in a game */
			if (client.playing)
				break;

			tmp = lookup_widget(main_win, "launch");
			gtk_widget_set_sensitive(GTK_WIDGET(tmp),FALSE);
			tmp = lookup_widget(main_win, "join");
			gtk_widget_set_sensitive(GTK_WIDGET(tmp),FALSE);
			tmp = lookup_widget(main_win, "join");
			gtk_widget_set_sensitive(GTK_WIDGET(tmp),FALSE);
			tmp = lookup_widget(main_win, "launch_button");
			gtk_widget_set_sensitive(GTK_WIDGET(tmp),FALSE);
			tmp = lookup_widget(main_win, "join_button");
			gtk_widget_set_sensitive(GTK_WIDGET(tmp),FALSE);
			tmp = lookup_widget(mnu_tables, "launch");
			gtk_widget_set_sensitive(GTK_WIDGET(tmp),FALSE);
			tmp = lookup_widget(mnu_tables, "join1");
			gtk_widget_set_sensitive(GTK_WIDGET(tmp),FALSE);
			tmp = lookup_widget(main_win, "room_combo");
			gtk_widget_set_sensitive(GTK_WIDGET(tmp),FALSE);
			
			client.playing = TRUE;
 			launch_game(selected_type, FALSE);
			break;
		case E_NOT_IN_ROOM:
			warn_dlg("Must be in room to join table");
			break;
		case E_TABLE_EMPTY: 
			warn_dlg("No table selected to join.");
			break;
		case E_TABLE_FULL:
			warn_dlg("Sorry, The table is full.");
			break;
		}
		
		break;
		
	case RSP_TABLE_LEAVE:
		es_read_char(source, &status);
		connect_msg("[%s] %d\n", opcode_str[op], status);
		switch (status) {
		case 0:
			game_over();
			break;
		case E_NO_TABLE:
			warn_dlg("You are not at a table");
			break;
			
		case E_LEAVE_FAIL:
			warn_dlg("Can't leave table");
			break;
			
		case E_LEAVE_FORBIDDEN:
			warn_dlg("Can't leave table during game");
			break;
		}

		/* Should put in function call which checks state */
		if (status == 0) {
			tmp = gtk_object_get_data(GTK_OBJECT(main_win), "launch");
			gtk_widget_set_sensitive(GTK_WIDGET(tmp),TRUE);
			tmp = gtk_object_get_data(GTK_OBJECT(main_win), "join");
			gtk_widget_set_sensitive(GTK_WIDGET(tmp),TRUE);
			tmp = gtk_object_get_data(GTK_OBJECT(main_win), "join");
			gtk_widget_set_sensitive(GTK_WIDGET(tmp),TRUE);
			tmp = gtk_object_get_data(GTK_OBJECT(main_win), "launch_button");
			gtk_widget_set_sensitive(GTK_WIDGET(tmp),TRUE);
			tmp = gtk_object_get_data(GTK_OBJECT(main_win), "join_button");
			gtk_widget_set_sensitive(GTK_WIDGET(tmp),TRUE);
			tmp = gtk_object_get_data(GTK_OBJECT(mnu_tables), "launch");
			gtk_widget_set_sensitive(GTK_WIDGET(tmp),TRUE);
			tmp = gtk_object_get_data(GTK_OBJECT(mnu_tables), "join1");
			gtk_widget_set_sensitive(GTK_WIDGET(tmp),TRUE);
		}

		break;

	case RSP_LOGOUT:
		es_read_char(source, &status);
		connect_msg("[%s] %d\n", opcode_str[op], status);
		login_disconnect();
		break;

	case RSP_LIST_TYPES:
		handle_list_types(op, source);
		break;

	case RSP_LIST_TABLES:
		handle_list_tables(op, source);
		break;

	case RSP_LIST_ROOMS:
		es_read_int(source, &count);
		room_info.count = count;
		connect_msg("[%s] Room List Count %d\n", opcode_str[op], count);
		if (count>0)
		{
			room_info.info = calloc(count, sizeof(RoomInfo));
			for (i = 0; i < count; i++) {
				es_read_int(source, &room_info.info[i].num);
				es_read_string_alloc(source, &room_info.info[i].name);
				es_read_int(source, &room_info.info[i].game_type);
				es_read_string_alloc(source, &room_info.info[i].desc);
				connect_msg("[%s] Room Num. %d\n", opcode_str[op], room_info.info[i].num);
				connect_msg("[%s] Room Name %s\n", opcode_str[op], room_info.info[i].name);
				connect_msg("[%s] Room Game %d\n", opcode_str[op], room_info.info[i].game_type);
				connect_msg("[%s] Room Desc %s\n", opcode_str[op], room_info.info[i].desc);
			}
		}
		display_rooms();
		break;

	case RSP_LIST_PLAYERS:
		handle_list_players(op, source);
		break;

	case RSP_GAME:
		es_read_int(source, &size);
		connect_msg("[%s] %d bytes\n", opcode_str[op], size);
		es_readn(source, buf, size);
		i_status = es_writen(game.fd, buf, size);
		if (i_status <= 0) {	/* Game over */
			dbg_msg("Game is over");
			client.playing = 0;
			close(game.fd);
		}
		break;

	case RSP_CHAT:
		es_read_char(source, &status);
		connect_msg("[%s] Chat Send Status: %d\n", opcode_str[op],
							   status);
		switch(status) {
			case 0:
				tmpstr = NULL;
				break;
			case E_USR_LOOKUP:
				tmpstr = "User not found.";
				break;
			case E_NOT_IN_ROOM:
				tmpstr = "You must be in a room to chat.";
				break;
			case E_AT_TABLE:
				tmpstr = "Unable to send (player is at table)";
				break;
			default:
				tmpstr = "Unknown chat error.";
				break;
		}
		if(tmpstr)
			chat_print(CHAT_COLOR_SERVER, "---", tmpstr);
		break;

	case MSG_CHAT:
		es_read_char(source, &subop);
		es_read_string(source, name, MAX_USER_NAME_LEN+1);
		connect_msg("[%s] msg from %s\n", opcode_str[op], name);
		if (subop & GGZ_CHAT_M_MESSAGE)
			es_read_string_alloc(source, &message);
		else
			message = g_strdup("");		
		connect_msg("[%s] Message: %s\n", opcode_str[op], message);
		display_chat(subop, name, message);
		g_free(message);
		break;

	case MSG_UPDATE_PLAYERS:
		handle_update_players(op, source);
		break;

	case MSG_UPDATE_TABLES:
		handle_update_tables(op, source);
		break;

	case MSG_UPDATE_TYPES:
		connect_msg("[%s]\n", opcode_str[op]);
		ggz_get_types(NULL, NULL);
		break;

	case MSG_UPDATE_ROOMS:
		connect_msg("[%s]\n", opcode_str[op]);
		/*ggz_get_rooms(NULL, NULL);*/
		break;

	case RSP_MOTD:
		if (dlg_motd == NULL)
			dlg_motd = create_dlgMOTD();
        	tmp = gtk_object_get_data(GTK_OBJECT(main_win), "motd");
	        gtk_widget_set_sensitive(GTK_WIDGET(tmp),FALSE);

		es_read_int(source, &count);
		connect_msg("[%s] MOTD line count %d\n", opcode_str[op], count);
		for (i = 0; i < count; i++) {
			es_read_string_alloc(source, &message);
			connect_msg("[%s] %s",opcode_str[op], message);
			motd_print_line(message);
		}
		gtk_widget_show(dlg_motd);
		break;
	case MSG_MOTD:
		if (dlg_motd == NULL)
			dlg_motd = create_dlgMOTD();
        	tmp = gtk_object_get_data(GTK_OBJECT(main_win), "motd");
	        gtk_widget_set_sensitive(GTK_WIDGET(tmp),FALSE);

		es_read_int(source, &count);
		connect_msg("[%s] MOTD line count %d\n", opcode_str[op], count);
		for (i = 0; i < count; i++) {
			es_read_string_alloc(source, &message);
			connect_msg("[%s] %s",opcode_str[op], message);
			motd_print_line(message);
		}
		gtk_widget_show(dlg_motd);
		break;
	case RSP_PREF_CHANGE:
	case RSP_REMOVE_USER:
	case RSP_TABLE_OPTIONS:
	case RSP_USER_STAT:
		break;

	case RSP_ROOM_JOIN:
		es_read_char(source, &status);
		connect_msg("[%s] Room join %d\n", opcode_str[op], status);

		tmp = gtk_object_get_data(GTK_OBJECT(main_win), "room_entry");
		
		switch (status) {
		case E_ROOM_FULL:
			gtk_entry_set_text(GTK_ENTRY(tmp), room_info.info[client.cur_room].name);
			break;
		case E_BAD_OPTIONS:
			gtk_entry_set_text(GTK_ENTRY(tmp), room_info.info[client.cur_room].name);
			break;
		case -1:
			gtk_entry_set_text(GTK_ENTRY(tmp), room_info.info[client.cur_room].name);
			break;
		case 0:
			client.cur_room = client.new_room;
			tmpstr = g_strdup_printf("Joined room: %s",
				      room_info.info[client.cur_room].name);
			chat_print(CHAT_COLOR_SERVER, "---", tmpstr);
			g_free(tmpstr);
			ggz_get_players(NULL, NULL);
			ggz_get_tables(NULL, NULL);
			break;
		}

		break;
	}
}


void connect_msg(const gchar *format, ...)
{
	va_list ap;
	gchar *message;
	gpointer tmp;

	/* If we're not displaying details then don't bother */
	if (dlg_details == NULL)
		return;
	
	va_start(ap, format);
	message = g_strdup_vprintf(format, ap);
	va_end(ap);


	tmp = gtk_object_get_data(GTK_OBJECT(dlg_details), "text");
	gtk_text_insert(GTK_TEXT(tmp), NULL, NULL, NULL, message, -1);
	
	g_free(message);
}


gint normal_login(void)
{
	es_write_int(client.sock, REQ_LOGIN);
	es_write_string(client.sock, client.server.login);
	es_write_string(client.sock, client.server.password);

	return 0;
}


gint anon_login(void)
{
	es_write_int(client.sock, REQ_LOGIN_ANON);
	es_write_string(client.sock, client.server.login);

	return 0;
}


gint new_login(void)
{
	es_write_int(client.sock, REQ_LOGIN_NEW);
	es_write_string(client.sock, client.server.login);

	return 0;
}


static void handle_list_types(gint op, gint fd)
{
	gint count, i;

	es_read_int(fd, &count);
	game_types.count = count;
	connect_msg("[%s] List Count %d\n", opcode_str[op], count);

	for (i = 0; i < count; i++) {
		es_read_int(fd, &game_types.info[i].index);
		es_read_string(fd, game_types.info[i].name,
			       MAX_GAME_NAME_LEN);
		es_read_string(fd, game_types.info[i].version,
			       MAX_GAME_VER_LEN);
		es_read_char(fd, &game_types.info[i].num_play_allow);
		es_read_char(fd, &game_types.info[i].num_bot_allow);
		es_read_string(fd, game_types.info[i].desc,
			       MAX_GAME_DESC_LEN);
		es_read_string(fd, game_types.info[i].author,
			       MAX_GAME_AUTH_LEN);
		es_read_string(fd, game_types.info[i].web,
			       MAX_GAME_WEB_LEN);
		
		connect_msg("[%s] Game type %d\n", opcode_str[op], 
			    game_types.info[i].index);
		connect_msg("[%s] %s\n", opcode_str[op], 
			    game_types.info[i].name);
		connect_msg("[%s] %s\n", opcode_str[op], 
			    game_types.info[i].version);
		connect_msg("[%s] %d\n", opcode_str[op], 
			    game_types.info[i].num_play_allow);
		connect_msg("[%s] %s\n", opcode_str[op], 
			    game_types.info[i].desc);
		connect_msg("[%s] %s\n", opcode_str[op], 
			    game_types.info[i].author);
		connect_msg("[%s] %s\n", opcode_str[op], 
			    game_types.info[i].web);
	}
}


static void handle_list_tables(gint op, gint fd)
{
	gint i, j, count, seats;
	gchar name[MAX_USER_NAME_LEN + 1];
	Table* table;

	/* Clear list of tables since we're getting a new list */
	table_list_clear();

	/* Read in number of tables in list */
	es_read_int(fd, &count);
	connect_msg("[%s] Table List Count %d\n", opcode_str[op], count);

	for (i = 0; i < count; i++) {
		/* Allocate a new table */
		table = (Table*)g_malloc0(sizeof(Table));
		
		es_read_int(fd, &table->id);
		es_read_int(fd, &table->room);
		es_read_int(fd, &table->type);
		es_read_string(fd, table->desc, MAX_GAME_DESC_LEN);
		es_read_char(fd, &table->state);
		es_read_int(fd, &seats);
		
		/* read in seat assignments */
		for (j = 0; j < seats; j++) {
			es_read_int(fd, &table->seats[j]);
			if (table->seats[j] == GGZ_SEAT_PLAYER
			    || table->seats[j] == GGZ_SEAT_RESV) {
				es_read_string(fd, name, MAX_USER_NAME_LEN +1);
				table->names[j] = g_strdup(name);
			}
		}
		for (j = seats; j < MAX_TABLE_SIZE; j++)
			table->seats[j] = GGZ_SEAT_NONE;
		
		connect_msg("[%s] Room: %d\n", opcode_str[op], table->room);
		connect_msg("[%s] Type: %d\n", opcode_str[op], table->type);
		connect_msg("[%s] State: %d\n", opcode_str[op], table->state);
		connect_msg("[%s] Seats: %d\n", opcode_str[op], seats);
		connect_msg("[%s] Desc: %s\n", opcode_str[op], table->desc);
		table_list_add(table);
	}

	/* Display new list */
	ggz_tables_display();
}


static void handle_list_players(gint op, gint fd)
{
	gint i, count, table;
	gchar name[MAX_USER_NAME_LEN + 1];

	/* Clear list of users since we're getting a new list */
	player_list_clear();
	
	/* Read in number of players in list */
	es_read_int(fd, &count);
	connect_msg("[%s] Player List Count %d\n", opcode_str[op], count);
	
	for (i = 0; i < count; i++) {
		es_read_string(fd, name, MAX_USER_NAME_LEN+1);
		es_read_int(fd, &table);
		connect_msg("[%s] User %s\n", opcode_str[op], name);
		connect_msg("[%s] Table %d\n", opcode_str[op], table);
		player_list_add(name, table, 0);
	}
	
	/* Display new list */
	ggz_players_display();
}


static void handle_update_players(gint op, gint fd)
{
	guchar subop;
	gchar name[MAX_USER_NAME_LEN + 1];
	
	es_read_char(fd, &subop);
	es_read_string(fd, name, MAX_USER_NAME_LEN+1);
	switch (subop) {
	case GGZ_UPDATE_ADD:
		connect_msg("[%s] %s entered room\n", opcode_str[op], 
			    name);
		/* Add player to list with no table */
		player_list_add(name, -1, 0);
		chat_print(CHAT_COLOR_SERVER, "-->", name);
		break;
	case GGZ_UPDATE_DELETE:
		connect_msg("[%s] %s left room\n", opcode_str[op], 
			    name);
		player_list_remove(name);
		chat_print(CHAT_COLOR_SERVER, "<--", name);
		break;
	}	

	/* Display updated list */
	ggz_players_display();
}


static void handle_update_tables(gint op, gint fd)
{
	guchar subop, state;
	guint id, seat, i;
	gchar name[MAX_USER_NAME_LEN + 1];		
	Table* table;

	es_read_char(fd, &subop);
	es_read_int(fd, &id);
	switch (subop) {
		
	case GGZ_UPDATE_DELETE:
		connect_msg("[%s] Table %d deleted\n", opcode_str[op], id);
		table_list_remove(id);
		break;

	case GGZ_UPDATE_STATE:
		es_read_char(fd, &state);
		connect_msg("[%s] Table %d new state %d\n", opcode_str[op], 
			    id, state);
		break;
	case GGZ_UPDATE_JOIN:
		es_read_int(fd, &seat);
		es_read_string(fd, name, MAX_USER_NAME_LEN + 1);
		connect_msg("[%s] %s joined table %d in seat %d\n", 
			    opcode_str[op], name, id, seat);
		player_list_update(name, id, 0);
		table_list_player_join(id, seat, name);
		/* Display new player list */
		ggz_players_display();
		break;
			
	case GGZ_UPDATE_LEAVE:
		es_read_int(fd, &seat);
		es_read_string(fd, name, MAX_USER_NAME_LEN + 1);
		connect_msg("[%s] %s left table %d in seat %d\n", 
			    opcode_str[op], name, id, seat);
		player_list_update(name, -1, 0);
		table_list_player_leave(id, seat);		
		/* Display new list */
		ggz_players_display();
		break;

	case GGZ_UPDATE_ADD:
		table = (Table*)g_malloc0(sizeof(Table));
		table->id = id;
		es_read_int(fd, &table->room);
		es_read_int(fd, &table->type);
		es_read_string(fd, table->desc, MAX_GAME_DESC_LEN);
		es_read_char(fd, &table->state);
		es_read_int(fd, &seat);
		
		for (i = 0; i < seat; i++) {
			es_read_int(fd, &table->seats[i]);
			if (table->seats[i] == GGZ_SEAT_PLAYER
			    || table->seats[i] == GGZ_SEAT_RESV) {
				es_read_string(fd, name, MAX_USER_NAME_LEN +1);
				table->names[i] = g_strdup(name);
			}
		}

		for (i = seat; i < MAX_TABLE_SIZE; i++)
			table->seats[i] = GGZ_SEAT_NONE;
		
		connect_msg("[%s] New table %d added\n", opcode_str[op], id);
		connect_msg("[%s] Room: %d\n", opcode_str[op], table->room);
		connect_msg("[%s] Type: %d\n", opcode_str[op], table->type);
		connect_msg("[%s] State: %d\n", opcode_str[op], table->state);
		connect_msg("[%s] Seats: %d\n", opcode_str[op], seat);
		connect_msg("[%s] Desc: %s\n", opcode_str[op], table->desc);
		table_list_add(table);
		break;
	}
	
	/* Display new list */
	ggz_tables_display();
}


void motd_print_line(gchar *line)
{
        gchar out[1024];
        gint lindex=0;
        gint oindex=0;
	GtkWidget *temp_widget;
	GdkColormap *cmap;
	GdkFont *fixed_font;
	gint color_index=9; /* Black */
	gint letter;

	/* Make shure the motd window it there */
	if (dlg_motd == NULL)
		return;

	cmap = gdk_colormap_get_system();
	if (!gdk_color_alloc(cmap, &colors[color_index])) {
		g_error("couldn't allocate color");
	}

	temp_widget = gtk_object_get_data(GTK_OBJECT(dlg_motd), "text1");
	fixed_font = gdk_font_load ("-misc-fixed-medium-r-normal--10-100-75-75-c-60-iso8859-1");

        while(line[lindex] != '\0')
        {
                if (line[lindex] == '%')
                {
                        lindex++;
                        if (line[lindex] == 'c')
                        {
                                lindex++;
				letter = atoi(&line[lindex]);
				if ((letter>=0) && (letter<=9))
				{
					out[oindex]='\0';
					gtk_text_insert (GTK_TEXT (temp_widget), fixed_font,
								&colors[color_index], NULL, out, -1);
					color_index=atoi(&line[lindex]);
					cmap = gdk_colormap_get_system();
					if (!gdk_color_alloc(cmap, &colors[color_index])) {
						g_error("couldn't allocate color");
					}
                                        oindex=0;
                                        lindex++;
				}else {
                                        lindex--;
                                        lindex--;
                                }
                        }else{
                                lindex--;
                        }
                }
                out[oindex]=line[lindex];
                lindex++;
                oindex++;
        }
        out[oindex]='\0';
	gtk_text_insert (GTK_TEXT (temp_widget), fixed_font,
			&colors[color_index], NULL, out, -1);
}


void display_rooms()
{
	GtkWidget *tmp;
	GList *items=NULL;
	gint i;

	for(i=0;i<room_info.count;i++)
	{
		items = g_list_append(items, room_info.info[i].name);
	}

	tmp = gtk_object_get_data(GTK_OBJECT(main_win), "room_combo");
	gtk_combo_set_popdown_strings(GTK_COMBO(tmp), items);
	gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(tmp)->entry), "<none>");
}
