/*
 * File: ggz.c
 * Author: Justin Zaun
 * Project: GGZ GTK Client
 *
 * This is the main program body for the GGZ client
 *
 * Copyright (C) 2000 Justin Zaun.
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

#include <gtk/gtk.h>
#include <ggzcore.h>
#include <stdlib.h>
#include <sys/time.h>

#include "chat.h"
#include "ggz.h"
#include "login.h"
#include "msgbox.h"
#include "motd.h"
#include "game.h"
#include "server.h"
#include "support.h"
#include "xtext.h"


static gint server_handle;

extern GtkWidget *login_dialog;
extern GtkWidget *win_main;
extern GGZServer *server;
extern GGZGame *game;

void display_tables(void);
void display_players(void);

static GGZHookReturn ggz_connected(GGZServerEvent id, void* event_data, void* user_data);
static GGZHookReturn ggz_connect_fail(GGZServerEvent id, void* event_data, void* user_data);
static GGZHookReturn ggz_negotiated(GGZServerEvent id, void* event_data, void* user_data);
static GGZHookReturn ggz_logged_in(GGZServerEvent id, void* event_data, void* user_data);
static GGZHookReturn ggz_login_fail(GGZServerEvent id, void* event_data, void* user_data);
static GGZHookReturn ggz_room_list(GGZServerEvent id, void* event_data, void* user_data);
static GGZHookReturn ggz_entered(GGZServerEvent id, void* event_data, void* user_data);
static GGZHookReturn ggz_entered_fail(GGZServerEvent id, void* event_data, void* user_data);
static GGZHookReturn ggz_logout(GGZServerEvent id, void* event_data, void* user_data);
static GGZHookReturn ggz_motd_loaded(GGZServerEvent id, void* event_data, void* user_data);
static GGZHookReturn ggz_table_left(GGZServerEvent id, void* event_data, void* user_data);
static GGZHookReturn ggz_server_error(GGZServerEvent id, void* event_data, void* user_data);
static GGZHookReturn ggz_net_error(GGZServerEvent id, void* event_data, void* user_data);

static GGZHookReturn ggz_chat_msg(GGZRoomEvent id, void* event_data, void* user_data);
static GGZHookReturn ggz_chat_prvmsg(GGZRoomEvent id, void* event_data, void* user_data);
static GGZHookReturn ggz_chat_beep(GGZRoomEvent id, void* event_data, void* user_data);
static GGZHookReturn ggz_list_players(GGZRoomEvent id, void* event_data, void* user_data);

static GGZHookReturn ggz_room_enter(GGZRoomEvent id, void* event_data, void* user_data);
static GGZHookReturn ggz_room_leave(GGZRoomEvent id, void* event_data, void* user_data);
static GGZHookReturn ggz_list_tables(GGZRoomEvent id, void* event_data, void* user_data);
static GGZHookReturn ggz_table_update(GGZRoomEvent id, void* event_data, void* user_data);
static GGZHookReturn ggz_state_change(GGZServerEvent id, void* event_data, void* user_data);
static void ggz_input_removed(gpointer data);

/* Table functions */
static GGZHookReturn ggz_table_launched(GGZRoomEvent id, void*, void*);
static GGZHookReturn ggz_table_launch_fail(GGZRoomEvent id, void*, void*);
static GGZHookReturn ggz_table_joined(GGZRoomEvent id, void*, void*);
static GGZHookReturn ggz_table_join_fail(GGZRoomEvent id, void*, void*);
static GGZHookReturn ggz_table_left(GGZRoomEvent id, void*, void*);
static GGZHookReturn ggz_table_leave_fail(GGZRoomEvent id, void*, void*);
static GGZHookReturn ggz_table_data(GGZRoomEvent id, void*, void*);


GdkInputFunction ggz_check_fd(gpointer server, gint fd, GdkInputCondition cond);

void ggz_event_init(GGZServer *Server)
{
	ggzcore_server_add_event_hook(Server, GGZ_CONNECTED, ggz_connected);
	ggzcore_server_add_event_hook(Server, GGZ_CONNECT_FAIL,	ggz_connect_fail);
	ggzcore_server_add_event_hook(Server, GGZ_NEGOTIATED, ggz_negotiated);
	ggzcore_server_add_event_hook(Server, GGZ_NEGOTIATE_FAIL, ggz_connect_fail);
	ggzcore_server_add_event_hook(Server, GGZ_LOGGED_IN, ggz_logged_in);
	ggzcore_server_add_event_hook(Server, GGZ_LOGIN_FAIL, ggz_login_fail);
	ggzcore_server_add_event_hook(server, GGZ_ROOM_LIST, ggz_room_list);
	ggzcore_server_add_event_hook(server, GGZ_ENTERED, ggz_entered);
	ggzcore_server_add_event_hook(server, GGZ_ENTER_FAIL, ggz_entered_fail);
	ggzcore_server_add_event_hook(server, GGZ_LOGOUT, ggz_logout);
	ggzcore_server_add_event_hook(server, GGZ_MOTD_LOADED, ggz_motd_loaded);
	ggzcore_server_add_event_hook(server, GGZ_STATE_CHANGE, ggz_state_change);
	ggzcore_server_add_event_hook(server, GGZ_TABLE_LEFT, ggz_table_left);
	ggzcore_server_add_event_hook(server, GGZ_PROTOCOL_ERROR, ggz_server_error);
	ggzcore_server_add_event_hook(server, GGZ_NET_ERROR, ggz_net_error);
}


static GGZHookReturn ggz_connected(GGZServerEvent id, void* event_data, void* user_data)
{
	GtkWidget *tmp;
	int fd;

	/* Desensitize the connect button */
	/* Should be done elsewhere? */
	tmp = gtk_object_get_data(GTK_OBJECT(login_dialog), "connect_button");
	gtk_widget_set_sensitive(tmp, FALSE);

	/* Add the fd to the ggtk main loop */
	fd = ggzcore_server_get_fd(server);
	server_handle = gdk_input_add_full(fd, GDK_INPUT_READ, 
					   (GdkInputFunction)ggz_check_fd, 
					   (gpointer)server,
					   ggz_input_removed);

	return GGZ_HOOK_OK;
}


static GGZHookReturn ggz_connect_fail(GGZServerEvent id, void* event_data, void* user_data)
{
	gchar* msg;

	msg = g_strdup_printf("Error connecting to server: %s", (char*)event_data);
	msgbox(msg, "Error", MSGBOX_OKONLY, MSGBOX_STOP, MSGBOX_NORMAL);
	g_free(msg);

	login_connect_failed();

	return GGZ_HOOK_OK;
}


static GGZHookReturn ggz_negotiated(GGZServerEvent id, void* event_data, void* user_data)
{
	ggzcore_server_login(server);

	return GGZ_HOOK_OK;
}


static GGZHookReturn ggz_logged_in(GGZServerEvent id, void* event_data, void* user_data)
{
	gchar *password;
	gchar *message;
	gchar *title;
	GtkWidget *tmp;


	/* Set title */
	title = g_strdup_printf("GGZ Gaming Zone - [%s:%d]",
				ggzcore_server_get_host(server),
				ggzcore_server_get_port(server));
	gtk_window_set_title (GTK_WINDOW (win_main), title);
	g_free(title);

	login_destroy();
	ggzcore_server_list_rooms(server, -1, 1);
	ggzcore_server_list_gametypes(server, 1);

	/* set senditivity */
	/* Menu bar */
	tmp = lookup_widget(win_main, "connect");
	gtk_widget_set_sensitive(tmp, FALSE);
	tmp = lookup_widget(win_main, "disconnect");
	gtk_widget_set_sensitive(tmp, TRUE);
	tmp = lookup_widget(win_main, "game");
	gtk_widget_set_sensitive(tmp, FALSE);
	tmp = lookup_widget(win_main, "edit");
	gtk_widget_set_sensitive(tmp, TRUE);
	tmp = lookup_widget(win_main, "view");
	gtk_widget_set_sensitive(tmp, TRUE);

	/* Tool bar */
	tmp = lookup_widget(win_main, "connect_button");
	gtk_widget_set_sensitive(tmp, FALSE);
	tmp = lookup_widget(win_main, "disconnect_button");
	gtk_widget_set_sensitive(tmp, TRUE);
	tmp = lookup_widget(win_main, "launch_button");
	gtk_widget_set_sensitive(tmp, FALSE);
	tmp = lookup_widget(win_main, "join_button");
	gtk_widget_set_sensitive(tmp, FALSE);
	tmp = lookup_widget(win_main, "props_button");
	gtk_widget_set_sensitive(tmp, TRUE);
	tmp = lookup_widget(win_main, "stats_button");
	gtk_widget_set_sensitive(tmp, TRUE);

	/* Client area */
	tmp = lookup_widget(win_main, "room_clist");
	gtk_widget_set_sensitive(tmp, TRUE);
	tmp = lookup_widget(win_main, "player_clist");
	gtk_widget_set_sensitive(tmp, TRUE);
	tmp = lookup_widget(win_main, "table_clist");
	gtk_widget_set_sensitive(tmp, FALSE);
	tmp = lookup_widget(win_main, "chat_entry");
	gtk_widget_set_sensitive(tmp, FALSE);
	tmp = lookup_widget(win_main, "send_button");
	gtk_widget_set_sensitive(tmp, FALSE);

	/* If this was a first-time login, get the password from the server */
	if (ggzcore_server_get_type(server) == GGZ_LOGIN_NEW) {
		password = ggzcore_server_get_password(server);
		message = g_strdup_printf("Your new password is %s", password);
		msgbox(message, "New password", MSGBOX_OKONLY, MSGBOX_INFO, 
		       MSGBOX_NORMAL);
		free(message);
	}
		

	return GGZ_HOOK_OK;
}


static GGZHookReturn ggz_login_fail(GGZServerEvent id, void* event_data, void* user_data)
{
	login_failed();

	return GGZ_HOOK_OK;
}

static GGZHookReturn ggz_room_list(GGZServerEvent id, void* event_data, void* user_data)
{
	GtkWidget *tmp;
	GGZRoom *room;
	gint i, numrooms;
	gchar *name;

	/* Clear current list of rooms */
	tmp = lookup_widget(win_main, "room_clist");
	gtk_clist_clear(GTK_CLIST(tmp));

	/* Display current list of rooms */
	numrooms = ggzcore_server_get_num_rooms(server);
	if (numrooms == 0)
		return GGZ_HOOK_OK;

	for (i = 0; i < numrooms; i++)
	{
		room = ggzcore_server_get_nth_room(server, i);
		name = ggzcore_room_get_name(room);
		gtk_clist_insert(GTK_CLIST(tmp), i, &name);
		/* Hookup the chat functions to the new room */
		ggzcore_room_add_event_hook(room, GGZ_CHAT, ggz_chat_msg);
		ggzcore_room_add_event_hook(room, GGZ_PRVMSG, ggz_chat_prvmsg);
		ggzcore_room_add_event_hook(room, GGZ_BEEP, ggz_chat_beep);
		ggzcore_room_add_event_hook(room, GGZ_PLAYER_LIST, ggz_list_players);
		ggzcore_room_add_event_hook(room, GGZ_TABLE_LIST, ggz_list_tables);
		ggzcore_room_add_event_hook(room, GGZ_ROOM_ENTER, ggz_room_enter);
		ggzcore_room_add_event_hook(room, GGZ_ROOM_LEAVE, ggz_room_leave);
		ggzcore_room_add_event_hook(room, GGZ_TABLE_LAUNCHED, ggz_table_launched);
		ggzcore_room_add_event_hook(room, GGZ_TABLE_LAUNCH_FAIL, ggz_table_launch_fail);
		ggzcore_room_add_event_hook(room, GGZ_TABLE_JOINED, ggz_table_joined);
		ggzcore_room_add_event_hook(room, GGZ_TABLE_JOIN_FAIL, ggz_table_join_fail);
		ggzcore_room_add_event_hook(room, GGZ_TABLE_LEFT, ggz_table_left);
		ggzcore_room_add_event_hook(room, GGZ_TABLE_LEAVE_FAIL, ggz_table_leave_fail);
		ggzcore_room_add_event_hook(room, GGZ_TABLE_UPDATE, ggz_table_update);
		ggzcore_room_add_event_hook(room, GGZ_TABLE_DATA, ggz_table_data);
	}

	return GGZ_HOOK_OK;
}

static GGZHookReturn ggz_entered(GGZServerEvent id, void* event_data, void* user_data)
{
	GtkWidget *tmp;
	gchar *name;
	gchar *message;

	 
	/* Clear the player list */
	/* We do this here so that on slower links people
	 * don't think all the old people are in the old room */
	tmp = lookup_widget(win_main, "player_clist");
	gtk_clist_clear(GTK_CLIST(tmp));

	/* Get player list */
	/* FIXME: Player list should use the ggz update system*/
	ggzcore_room_list_tables(ggzcore_server_get_cur_room(server), -1, 0);
	ggzcore_room_list_players(ggzcore_server_get_cur_room(server));


	/* Set the room label to current room */
	tmp = lookup_widget(win_main, "Current_room_label");
	name = g_strdup_printf(_("Current Room: %s"), ggzcore_room_get_name(ggzcore_server_get_cur_room(server)));
	gtk_label_set_text(GTK_LABEL(tmp), name);
	g_free(name);

	/* Display message in chat area */
	message = g_strdup_printf(_("You've joined room \"%s\"."), ggzcore_room_get_name(ggzcore_server_get_cur_room(server)));
	chat_display_message(CHAT_BEEP, "---", message);
	g_free(message);
	chat_display_message(CHAT_BEEP, "---",  ggzcore_room_get_desc(ggzcore_server_get_cur_room(server)));

	/* set senditivity */
	/* Menu bar */
	tmp = lookup_widget(win_main, "connect");
	gtk_widget_set_sensitive(tmp, FALSE);
	tmp = lookup_widget(win_main, "disconnect");
	gtk_widget_set_sensitive(tmp, TRUE);
	tmp = lookup_widget(win_main, "game");
	gtk_widget_set_sensitive(tmp, TRUE);
	tmp = lookup_widget(win_main, "edit");
	gtk_widget_set_sensitive(tmp, TRUE);
	tmp = lookup_widget(win_main, "view");
	gtk_widget_set_sensitive(tmp, TRUE);

	/* Tool bar */
	tmp = lookup_widget(win_main, "connect_button");
	gtk_widget_set_sensitive(tmp, FALSE);
	tmp = lookup_widget(win_main, "disconnect_button");
	gtk_widget_set_sensitive(tmp, TRUE);
	tmp = lookup_widget(win_main, "launch_button");
	gtk_widget_set_sensitive(tmp, TRUE);
	tmp = lookup_widget(win_main, "join_button");
	gtk_widget_set_sensitive(tmp, TRUE);
	tmp = lookup_widget(win_main, "props_button");
	gtk_widget_set_sensitive(tmp, TRUE);
	tmp = lookup_widget(win_main, "stats_button");
	gtk_widget_set_sensitive(tmp, TRUE);

	/* Client area */
	tmp = lookup_widget(win_main, "room_clist");
	gtk_widget_set_sensitive(tmp, TRUE);
	tmp = lookup_widget(win_main, "player_clist");
	gtk_widget_set_sensitive(tmp, TRUE);
	tmp = lookup_widget(win_main, "table_clist");
	gtk_widget_set_sensitive(tmp, TRUE);
	tmp = lookup_widget(win_main, "chat_entry");
	gtk_widget_set_sensitive(tmp, TRUE);
	tmp = lookup_widget(win_main, "send_button");
	gtk_widget_set_sensitive(tmp, TRUE);

	return GGZ_HOOK_OK;
}


static GGZHookReturn ggz_entered_fail(GGZServerEvent id, void* event_data, void* user_data)
{
	gchar* msg;
	GtkWidget *tmp;

	/* Client area */
	tmp = lookup_widget(win_main, "room_clist");
	gtk_widget_set_sensitive(tmp, TRUE);
	tmp = lookup_widget(win_main, "player_clist");
	gtk_widget_set_sensitive(tmp, TRUE);
	tmp = lookup_widget(win_main, "table_clist");
	gtk_widget_set_sensitive(tmp, TRUE);
	tmp = lookup_widget(win_main, "chat_entry");
	gtk_widget_set_sensitive(tmp, TRUE);
	tmp = lookup_widget(win_main, "send_button");
	gtk_widget_set_sensitive(tmp, TRUE);


	msg = g_strdup_printf("Error joining room: %s", (char*)event_data);
	msgbox(msg, "Error", MSGBOX_OKONLY, MSGBOX_STOP, MSGBOX_NORMAL);
	g_free(msg);
	
	return GGZ_HOOK_OK;
}


static GGZHookReturn ggz_logout(GGZServerEvent id, void* event_data, void* user_data)
{
	gdk_input_remove(server_handle);
	server_handle = -1;
	chat_display_message(CHAT_BEEP, "---", _("Disconnected from Server."));

	/* set title */
	gtk_window_set_title (GTK_WINDOW (win_main), "GGZ Gaming Zone");

	return GGZ_HOOK_OK;
}


static GGZHookReturn ggz_chat_msg(GGZRoomEvent id, void* event_data, void* user_data)
{
	gchar *player;
	gchar *message;

	player = ((char**)(event_data))[0];
	message = ((char**)(event_data))[1];

	chat_display_message(CHAT_MSG, player, message);

	return GGZ_HOOK_OK;
}

static GGZHookReturn ggz_chat_prvmsg(GGZRoomEvent id, void* event_data, void* user_data)
{
	gchar *message;
	gchar *player;

	player = ((char**)(event_data))[0];
	message = ((char**)(event_data))[1];
	chat_display_message(CHAT_PRVMSG, player, message);

	return GGZ_HOOK_OK;
}

static GGZHookReturn ggz_chat_beep(GGZRoomEvent id, void* event_data, void* user_data)
{
	gchar *message;
	gchar *player;

	player = ((char**)(event_data))[0];
	message = g_strdup_printf(_("You've been beeped by %s."), player);
	chat_display_message(CHAT_BEEP, "---", message);
	if( ggzcore_conf_read_int("CHAT", "SOUND", TRUE) )
		gdk_beep();

	g_free(message);

	return GGZ_HOOK_OK;
}

static GGZHookReturn ggz_list_players(GGZRoomEvent id, void* event_data, void* user_data)
{
	display_players();
	return GGZ_HOOK_OK;
}

static GGZHookReturn ggz_room_enter(GGZRoomEvent id, void* event_data, void* user_data)
{
	gchar *player;

	display_players();
	player = g_strdup(event_data);
	chat_enter(event_data);
	g_free(player);

	return GGZ_HOOK_OK;
}


static GGZHookReturn ggz_room_leave(GGZRoomEvent id, void* event_data, void* user_data)
{
	gchar *player;

	display_players();
	player = g_strdup(event_data);
	chat_part(event_data);
	g_free(player);

	return GGZ_HOOK_OK;
}


void ggz_sensitivity_init(void)
{
	GtkWidget *tmp;

	/* set senditivity */
	/* Menu bar */
	tmp = lookup_widget(win_main, "connect");
	gtk_widget_set_sensitive(tmp, TRUE);
	tmp = lookup_widget(win_main, "disconnect");
	gtk_widget_set_sensitive(tmp, FALSE);
	tmp = lookup_widget(win_main, "game");
	gtk_widget_set_sensitive(tmp, FALSE);
	tmp = lookup_widget(win_main, "edit");
	gtk_widget_set_sensitive(tmp, TRUE);
	tmp = lookup_widget(win_main, "view");
	gtk_widget_set_sensitive(tmp, FALSE);

	/* Tool bar */
	tmp = lookup_widget(win_main, "connect_button");
	gtk_widget_set_sensitive(tmp, TRUE);
	tmp = lookup_widget(win_main, "disconnect_button");
	gtk_widget_set_sensitive(tmp, FALSE);
	tmp = lookup_widget(win_main, "launch_button");
	gtk_widget_set_sensitive(tmp, FALSE);
	tmp = lookup_widget(win_main, "join_button");
	gtk_widget_set_sensitive(tmp, FALSE);
	tmp = lookup_widget(win_main, "leave_button");
	gtk_widget_set_sensitive(tmp, FALSE);
	tmp = lookup_widget(win_main, "props_button");
	gtk_widget_set_sensitive(tmp, TRUE);
	tmp = lookup_widget(win_main, "stats_button");
	gtk_widget_set_sensitive(tmp, FALSE);

	/* Client area */
	tmp = lookup_widget(win_main, "room_clist");
	gtk_widget_set_sensitive(tmp, FALSE);
	tmp = lookup_widget(win_main, "player_clist");
	gtk_widget_set_sensitive(tmp, FALSE);
	tmp = lookup_widget(win_main, "table_clist");
	gtk_widget_set_sensitive(tmp, FALSE);
	tmp = lookup_widget(win_main, "chat_entry");
	gtk_widget_set_sensitive(tmp, FALSE);
	tmp = lookup_widget(win_main, "send_button");
	gtk_widget_set_sensitive(tmp, FALSE);

	/* Set the room label to current room */
	tmp = lookup_widget(win_main, "Current_room_label");
	gtk_label_set_text(GTK_LABEL(tmp), "Current Room:");
}

static GGZHookReturn ggz_motd_loaded(GGZServerEvent id, void* event_data, void* user_data)
{
	gint i;
	gchar **motd;

	motd = event_data;

	motd_create_or_raise();
	for(i = 0; motd[i] != NULL; i++)
		motd_print_line(motd[i]);

	return GGZ_HOOK_OK;
}


static GGZHookReturn ggz_state_change(GGZServerEvent id, void* event_data, void* user_data)
{
	int context;
	GtkWidget* statebar;
	char *state;
	GGZStateID state_id;

	state_id = ggzcore_server_get_state(server);
	
	switch (state_id) {
	case GGZ_STATE_OFFLINE:
		state = _("Offline");
		break;
	case GGZ_STATE_CONNECTING:
		state = _("Connecting");
		break;
	case GGZ_STATE_ONLINE:
		state = _("Online");
		break;
	case GGZ_STATE_LOGGING_IN:
		state = _("Logging In");
		break;
	case GGZ_STATE_LOGGED_IN:
		state = _("Logged In");
		break;
	case GGZ_STATE_BETWEEN_ROOMS:
	case GGZ_STATE_ENTERING_ROOM:
		state = _("--> Room");
		break;
	case GGZ_STATE_IN_ROOM:
		state = _("Chatting");
		break;
	case GGZ_STATE_JOINING_TABLE:
		state = _("--> Table");
		break;
	case GGZ_STATE_AT_TABLE:
		state = _("Playing");
		break;
	case GGZ_STATE_LEAVING_TABLE:
		state = _("<-- Table");
		break;
	case GGZ_STATE_LOGGING_OUT:
		state = _("Logging Out");
		break;
	default:
		state = _("**none**");
		
	}

	statebar = lookup_widget(win_main, "statebar");
	context = gtk_statusbar_get_context_id(GTK_STATUSBAR(statebar), "state");
	gtk_statusbar_pop(GTK_STATUSBAR(statebar), context);
	gtk_statusbar_push(GTK_STATUSBAR(statebar), context, state);

	return GGZ_HOOK_OK;
}


static GGZHookReturn ggz_server_error(GGZServerEvent id, void* event_data, void* user_data)
{
	return GGZ_HOOK_OK;
}

static GGZHookReturn ggz_net_error(GGZServerEvent id, void* event_data, void* user_data)
{
	GtkWidget *tmp;

	gdk_input_remove(server_handle);
	server_handle = -1;
	/*ggzcore_server_free(server);*/
	chat_display_message(CHAT_BEEP, "---", _("Disconnected from Server."));

        /* Clear current list of rooms */
        tmp = gtk_object_get_data(GTK_OBJECT(win_main), "room_clist");
        gtk_clist_clear(GTK_CLIST(tmp));
 
        /* Clear current list of players */
        tmp = gtk_object_get_data(GTK_OBJECT(win_main), "player_clist");
        gtk_clist_clear(GTK_CLIST(tmp));

        /* Clear current list of tables */
        tmp = gtk_object_get_data(GTK_OBJECT(win_main), "table_clist");
        gtk_clist_clear(GTK_CLIST(tmp));

        ggz_sensitivity_init();

	return GGZ_HOOK_OK;
}


static GGZHookReturn ggz_list_tables(GGZRoomEvent id, void* event_data, void* user_data)
{
	display_tables();
	return GGZ_HOOK_OK;
}


static GGZHookReturn ggz_table_update(GGZRoomEvent id, void* event_data, void* user_data)
{
	display_tables();
	display_players();
	return GGZ_HOOK_OK;
}


static GGZHookReturn ggz_table_launched(GGZRoomEvent id, void* event_data, void* user_data)
{
	/*output_text("-- Table launched");*/
	return GGZ_HOOK_OK;
}


static GGZHookReturn ggz_table_launch_fail(GGZRoomEvent id, void* event_data, void* user_data)
{
	gchar* msg;

	msg = g_strdup_printf("Error launching table: %s", (char*)event_data);
	msgbox(msg, "Error", MSGBOX_OKONLY, MSGBOX_STOP, MSGBOX_NORMAL);
	g_free(msg);

	game_quit();

	return GGZ_HOOK_OK;
}


static GGZHookReturn ggz_table_joined(GGZRoomEvent id, void* event_data, void* user_data)
{
	/*output_text("-- Table joined");*/
	return GGZ_HOOK_OK;
}


static GGZHookReturn ggz_table_join_fail(GGZRoomEvent id, void* event_data, void* user_data)
{
	gchar* msg;

	msg = g_strdup_printf("Error joining table: %s", (char*)event_data);
	msgbox(msg, "Error", MSGBOX_OKONLY, MSGBOX_STOP, MSGBOX_NORMAL);
	g_free(msg);

	game_quit();

	return GGZ_HOOK_OK;
}


static GGZHookReturn ggz_table_left(GGZRoomEvent id, void* event_data, void* user_data)
{
	/*output_text("-- Left table");*/

	game_quit();

	return GGZ_HOOK_OK;
}


static GGZHookReturn ggz_table_leave_fail(GGZRoomEvent id, void* event_data, void* user_data)
{
	gchar* msg;

	msg = g_strdup_printf("Error leaving table: %s", (char*)event_data);
	msgbox(msg, "Error", MSGBOX_OKONLY, MSGBOX_STOP, MSGBOX_NORMAL);
	g_free(msg);

	return GGZ_HOOK_OK;
}
	

static GGZHookReturn ggz_table_data(GGZRoomEvent id, void* event_data, void* user_data)
{
        ggzcore_game_send_data(game, event_data);
        return GGZ_HOOK_OK;
}




/* The below function are used to
 * add and remove servers to the 
 * main loop
 */

GdkInputFunction ggz_check_fd(gpointer server, gint fd, GdkInputCondition cond)
{
	ggzcore_server_read_data(server);
	return 0;
}


/* GdkDestroyNotify function for server fd */
static void ggz_input_removed(gpointer data)
{
	ggzcore_server_free((GGZServer*)data);
	server = NULL;
}


void display_tables(void)
{
	GtkWidget *tmp;
	gchar *table[4], *desc;
	gint i, num, open, seats;
	GGZRoom *room;
	GGZTable *t = NULL;

	/* clear the list */
	tmp = lookup_widget(win_main, "table_clist");
	gtk_clist_clear(GTK_CLIST(tmp));


	room = ggzcore_server_get_cur_room(server);

	/* Display current list of players 
	if (!(numbers = ggzcore_room_get_numbers()))
		return GGZ_HOOK_OK;*/

	
	for (i = 0; i < ggzcore_room_get_num_tables(room); i++) {

		t = ggzcore_room_get_nth_table(room, i);
		num   = ggzcore_table_get_id(t);
		open = ggzcore_table_get_num_open(t);
		seats = ggzcore_table_get_num_seats(t);
		desc = ggzcore_table_get_desc(t);
			
		table[0] = g_strdup_printf("%d", num);
		table[1] = g_strdup_printf("%d/%d", open, seats);
		table[2] = g_strdup_printf("%s", desc);
		gtk_clist_append(GTK_CLIST(tmp), table);
		g_free(table[0]);
		g_free(table[1]);
		g_free(table[2]);
	}
}


void display_players(void)
{
	GtkWidget *tmp;
	gint i, num;
	gchar *player[4];
	GGZPlayer *p;
	GGZTable *table;
	GGZRoom *room = ggzcore_server_get_cur_room(server);

	/* Clear current list of players */
	tmp = lookup_widget(win_main, "player_clist");
	gtk_clist_clear(GTK_CLIST(tmp));

	/* Display current list of players */
	if ( (num = ggzcore_room_get_num_players(room)) <= 0)
		return;

	for (i = 0; i < num; i++) {
		
		p = ggzcore_room_get_nth_player(room, i);

		player[0] = " ";
		table = ggzcore_player_get_table(p);
		
		if(!table)
			player[1] = g_strdup("--");
		/*else if (ggzcore_player_get_table(names[i]) == -2)
		  player[1] = g_strdup("??");*/
		else
			player[1] = g_strdup_printf("%d", ggzcore_table_get_id(table));
		player[2] = g_strdup(ggzcore_player_get_name(p));
		gtk_clist_append(GTK_CLIST(tmp), player);
		g_free(player[1]);
		g_free(player[2]);
	}
}


