/*
 * File: ggzclient.c
 * Author: Justin Zaun
 * Project: GGZ GTK Client
 * $Id: ggzclient.c 6184 2004-10-15 08:35:42Z josef $
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
#  include <config.h>			/* Site-specific config */
#endif

#include <assert.h>
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include <ggz.h>
#include <ggzcore.h>

#include "chat.h"
#include "client.h"
#include "game.h"
#include "ggzclient.h"
#include "launch.h"
#include "login.h"
#include "msgbox.h"
#include "motd.h"
#include "playerlist.h"
#include "server.h"
#include "support.h"
#include "tablelist.h"
#include "xtext.h"


static gint server_handle = -1;
static gint channel_handle = -1;

gint numrooms;

static GGZHookReturn ggz_connected(GGZServerEvent id, void* event_data, void* user_data);
static GGZHookReturn ggz_connect_fail(GGZServerEvent id, void* event_data, void* user_data);
static GGZHookReturn ggz_negotiated(GGZServerEvent id, void* event_data, void* user_data);
static GGZHookReturn ggz_logged_in(GGZServerEvent id, void* event_data, void* user_data);
static GGZHookReturn ggz_channel_ready(GGZGameEvent id, void* event_data, void* user_data);
static GGZHookReturn ggz_login_fail(GGZServerEvent id, void* event_data, void* user_data);
static GGZHookReturn ggz_room_list(GGZServerEvent id, void* event_data, void* user_data);
static GGZHookReturn ggz_entered(GGZServerEvent id, void* event_data, void* user_data);
static GGZHookReturn ggz_entered_fail(GGZServerEvent id, void* event_data, void* user_data);
static GGZHookReturn ggz_logout(GGZServerEvent id, void* event_data, void* user_data);
static GGZHookReturn ggz_motd_loaded(GGZServerEvent id, void* event_data, void* user_data);
static GGZHookReturn ggz_server_error(GGZServerEvent id, void* event_data, void* user_data);
static GGZHookReturn ggz_net_error(GGZServerEvent id, void* event_data, void* user_data);

static GGZHookReturn ggz_chat(GGZRoomEvent id, void* event_data, void* user_data);
static GGZHookReturn ggz_chat_fail(GGZRoomEvent id, void* event_data, void* user_data);
static GGZHookReturn ggz_list_players(GGZRoomEvent id, void* event_data, void* user_data);
static GGZHookReturn ggz_player_count(GGZRoomEvent id,
				      void* event_data, void* user_data);

static GGZHookReturn ggz_room_enter(GGZRoomEvent id, void* event_data, void* user_data);
static GGZHookReturn ggz_room_leave(GGZRoomEvent id, void* event_data, void* user_data);
static GGZHookReturn ggz_list_tables(GGZRoomEvent id, void* event_data, void* user_data);
static GGZHookReturn ggz_table_update(GGZRoomEvent id, void* event_data, void* user_data);
static GGZHookReturn ggz_state_change(GGZServerEvent id, void* event_data, void* user_data);
static GGZHookReturn ggz_state_sensitivity(GGZServerEvent id, void* event_data, void* user_data);

/* Gdk input callbacks */
static void ggz_input_removed(gpointer data);
static void ggz_check_fd(gpointer server, gint fd, GdkInputCondition cond);

/* Table functions */
static GGZHookReturn ggz_table_launched(GGZRoomEvent id, void*, void*);
static GGZHookReturn ggz_table_launch_fail(GGZRoomEvent id, void*, void*);
static GGZHookReturn ggz_table_joined(GGZRoomEvent id, void*, void*);
static GGZHookReturn ggz_table_join_fail(GGZRoomEvent id, void*, void*);
static GGZHookReturn ggz_table_left(GGZRoomEvent id, void*, void*);
static GGZHookReturn ggz_table_leave_fail(GGZRoomEvent id, void*, void*);


/* One Time Functions */
static GGZHookReturn ggz_auto_join(GGZServerEvent id, void*, void*);





void ggz_event_init(GGZServer *Server)
{
	ggzcore_server_add_event_hook(Server, GGZ_CONNECTED, ggz_connected);
	ggzcore_server_add_event_hook(Server, GGZ_CONNECT_FAIL,	ggz_connect_fail);
	ggzcore_server_add_event_hook(Server, GGZ_NEGOTIATED, ggz_negotiated);
	ggzcore_server_add_event_hook(Server, GGZ_NEGOTIATE_FAIL, ggz_connect_fail);
	ggzcore_server_add_event_hook(Server, GGZ_LOGGED_IN, ggz_logged_in);
	ggzcore_server_add_event_hook(Server, GGZ_LOGIN_FAIL, ggz_login_fail);
	ggzcore_server_add_event_hook(server, GGZ_MOTD_LOADED, ggz_motd_loaded);
	ggzcore_server_add_event_hook(server, GGZ_ROOM_LIST, ggz_room_list);
	/* Unused: GGZ_TYPE_LIST */
	ggzcore_server_add_event_hook(server, GGZ_ENTERED, ggz_entered);
	ggzcore_server_add_event_hook(server, GGZ_ENTER_FAIL, ggz_entered_fail);
	ggzcore_server_add_event_hook(server, GGZ_LOGOUT, ggz_logout);
	ggzcore_server_add_event_hook(server, GGZ_NET_ERROR, ggz_net_error);
	ggzcore_server_add_event_hook(server, GGZ_PROTOCOL_ERROR, ggz_server_error);
	ggzcore_server_add_event_hook(server, GGZ_CHAT_FAIL, ggz_chat_fail);
	ggzcore_server_add_event_hook(server, GGZ_STATE_CHANGE, ggz_state_change);
	ggzcore_server_add_event_hook(server, GGZ_STATE_CHANGE, ggz_state_sensitivity);
	ggzcore_server_add_event_hook(Server, GGZ_CHANNEL_CONNECTED, ggz_connected);
	ggzcore_server_add_event_hook(server, GGZ_CHANNEL_READY, ggz_channel_ready);
	ggzcore_server_add_event_hook(Server, GGZ_CHANNEL_FAIL,	ggz_connect_fail);
}


static GGZHookReturn ggz_connected(GGZServerEvent id, void* event_data, void* user_data)
{
	int fd;
	
	if (id == GGZ_CONNECTED) {
		ggz_debug("connection", "We're connected.");
		
		/* Add the fd to the ggtk main loop */
		fd = ggzcore_server_get_fd(server);
		assert(server_handle == -1);
		server_handle = gdk_input_add_full(fd, GDK_INPUT_READ, 
						   ggz_check_fd, 
						   (gpointer)server,
						   ggz_input_removed);
	}
	else if (id == GGZ_CHANNEL_CONNECTED) {
		ggz_debug("connection", "Direct game channel connected.");

		/* Add the fd to the ggtk main loop */
		fd = ggzcore_server_get_channel(server);
		assert(channel_handle == -1);
		channel_handle = gdk_input_add(fd, GDK_INPUT_READ, 
					       ggz_check_fd, 
					       (gpointer)server);
	}

	return GGZ_HOOK_OK;
}


static GGZHookReturn ggz_connect_fail(GGZServerEvent id, void* event_data, void* user_data)
{
	gchar* msg;

	msg = g_strdup_printf(_("Error connecting to server: %s"), (char*)event_data);
	msgbox(msg, _("Error"), MSGBOX_OKONLY, MSGBOX_STOP, MSGBOX_NORMAL);
	g_free(msg);

	login_connect_failed();

	return GGZ_HOOK_OK;
}


static GGZHookReturn ggz_negotiated(GGZServerEvent id, void* event_data, void* user_data)
{
	ggzcore_server_login(server);

	return GGZ_HOOK_OK;
}


static GGZHookReturn ggz_channel_ready(GGZGameEvent id, void* event_data, void* user_data)
{
	ggz_debug("connection", "Direct game channel ready for game");

	/* Remove channel from gdk input list */
	gdk_input_remove(channel_handle);
	channel_handle = -1;
	
	game_channel_ready();
	
	return GGZ_HOOK_OK;
}


static GGZHookReturn ggz_logged_in(GGZServerEvent id, void* event_data, void* user_data)
{
	gchar *password;
	gchar *message;
	gchar *title;


	/* Set title */
	title = g_strdup_printf("GGZ Gaming Zone - [%s:%d]",
				ggzcore_server_get_host(server),
				ggzcore_server_get_port(server));
	gtk_window_set_title (GTK_WINDOW (win_main), title);
	g_free(title);

	login_destroy();
	ggzcore_server_add_event_hook(server, GGZ_ROOM_LIST, ggz_auto_join);
	ggzcore_server_list_rooms(server, -1, 1);
	ggzcore_server_list_gametypes(server, 1);


	/* If this was a first-time login, get the password from the server */
	if (ggzcore_server_get_type(server) == GGZ_LOGIN_NEW) {
		password = ggzcore_server_get_password(server);
		message = g_strdup_printf(_("Your new password is %s"), password);
		msgbox(message, _("New password"), MSGBOX_OKONLY, MSGBOX_INFO, 
		       MSGBOX_NORMAL);
		g_free(message);
	}
		

	return GGZ_HOOK_OK;
}


static GGZHookReturn ggz_login_fail(GGZServerEvent id,
				    void* event_data, void* user_data)
{
	GGZErrorEventData *error = event_data;

	login_failed(error);

	return GGZ_HOOK_OK;
}

static void update_room_list(void)
{
	GtkWidget *tmp = lookup_widget(win_main, "room_clist");
	int i;
	const int numrooms = ggzcore_server_get_num_rooms(server);

	/* Clear current list of rooms */
	gtk_clist_freeze(GTK_CLIST(tmp));
	gtk_clist_clear(GTK_CLIST(tmp));

	for (i = 0; i < numrooms; i++) {
		gchar *table[3];
		GGZRoom *room = ggzcore_server_get_nth_room(server, i);
		gchar *name = ggzcore_room_get_name(room);
		int players = ggzcore_room_get_num_players(room);

		table[0] = g_strdup_printf("%s", name);
		if (players >= 0) {
			table[1] = g_strdup_printf("%d", players);
		} else {
			table[1] = NULL;
		}
		table[2] = NULL;

		gtk_clist_append(GTK_CLIST(tmp), table);
	}

	gtk_clist_thaw(GTK_CLIST(tmp));
}

static void update_one_room(int room)
{
	GtkWidget *tmp;
	gchar *name;
	int players;
	gchar *table[2];
	GGZRoom *roomptr = ggzcore_server_get_nth_room(server, room);

	if (!roomptr) return;

	tmp = lookup_widget(win_main, "room_clist");
	name = ggzcore_room_get_name(roomptr);
	players = ggzcore_room_get_num_players(roomptr);

	table[0] = g_strdup_printf("%s", name);
	if (players >= 0) {
		table[1] = g_strdup_printf("%d", players);
	} else {
		table[1] = NULL;
	}

	gtk_clist_freeze(GTK_CLIST(tmp));
	gtk_clist_set_text(GTK_CLIST(tmp), room, 0, table[0]);
	gtk_clist_set_text(GTK_CLIST(tmp), room, 1, table[1]);
	gtk_clist_thaw(GTK_CLIST(tmp));
}

static GGZHookReturn ggz_room_list(GGZServerEvent id, void* event_data, void* user_data)
{
	GGZRoom *room;
	gint i;

	/* Display current list of rooms */
	numrooms = ggzcore_server_get_num_rooms(server);

	update_room_list();

	for (i = 0; i < numrooms; i++)
	{
		room = ggzcore_server_get_nth_room(server, i);

		/* Hookup the chat functions to the new room */
		ggzcore_room_add_event_hook(room, GGZ_CHAT_EVENT, ggz_chat);
		ggzcore_room_add_event_hook(room, GGZ_PLAYER_LIST, ggz_list_players);
		ggzcore_room_add_event_hook(room, GGZ_PLAYER_COUNT, ggz_player_count);
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
		ggzcore_room_add_event_hook(room, GGZ_PLAYER_LAG, ggz_list_players);
		ggzcore_room_add_event_hook(room, GGZ_PLAYER_STATS, ggz_list_players);
	}

	return GGZ_HOOK_OK;
}

static GGZHookReturn ggz_entered(GGZServerEvent id, void* event_data, void* user_data)
{
	GtkWidget *tmp;
	gchar *name;
	gchar *message;
	GGZRoom *room;
	GGZGameType *gt;

	/* Clear the player list */
	/* We do this here so that on slower links people
	 * don't think all the old people are in the old room */
	/* FIXME: this should be move to client_clear_players() */
	tmp = lookup_widget(win_main, "player_clist");
	gtk_clist_clear(GTK_CLIST(tmp));

	/* Clear table list */
	clear_tables();

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
	chat_display_local(CHAT_LOCAL_NORMAL, NULL, message);
	g_free(message);
	chat_display_local(CHAT_LOCAL_NORMAL, NULL,
			   ggzcore_room_get_desc(ggzcore_server_get_cur_room(server)));

	/* Check what the current game type is */
	room = ggzcore_server_get_cur_room(server);
	gt = ggzcore_room_get_gametype(room);

	if(ggzcore_gametype_get_name(gt) == NULL)
	{
		tmp = lookup_widget(win_main, "table_vpaned");
		if(GTK_PANED(tmp)->child1_size != 0)
			gtk_object_set(GTK_OBJECT(tmp), "user_data", GTK_PANED(tmp)->child1_size, NULL);
		gtk_paned_set_position(GTK_PANED(tmp), 0);
		tmp = lookup_widget(win_main, "launch_button");
		gtk_widget_set_sensitive(tmp, FALSE);
		tmp = lookup_widget(win_main, "join_button");
		gtk_widget_set_sensitive(tmp, FALSE);
		tmp = lookup_widget(win_main, "watch_button");
		gtk_widget_set_sensitive(tmp, FALSE);
	}else{
		int height;
		tmp = lookup_widget(win_main, "table_vpaned");
		gtk_object_get(GTK_OBJECT(tmp), "user_data", &height, NULL);
		gtk_paned_set_position(GTK_PANED(tmp), height);
		tmp = lookup_widget(win_main, "launch_button");
		gtk_widget_set_sensitive(tmp, TRUE);
		tmp = lookup_widget(win_main, "join_button");
		gtk_widget_set_sensitive(tmp, TRUE);
		tmp = lookup_widget(win_main, "watch_button");
		gtk_widget_set_sensitive(tmp, TRUE);
	}

	return GGZ_HOOK_OK;
}


static GGZHookReturn ggz_entered_fail(GGZServerEvent id, void* event_data, void* user_data)
{
	gchar* msg;

	msg = g_strdup_printf(_("Error joining room: %s"), (char*)event_data);
	msgbox(msg, _("Error"), MSGBOX_OKONLY, MSGBOX_STOP, MSGBOX_NORMAL);
	g_free(msg);
	
	return GGZ_HOOK_OK;
}


static GGZHookReturn ggz_logout(GGZServerEvent id, void* event_data, void* user_data)
{
	ggz_debug("connection", "Logged out.");
	
	server_disconnect();

	/* set title */
	gtk_window_set_title (GTK_WINDOW (win_main), "GGZ Gaming Zone");

	return GGZ_HOOK_OK;
}


static GGZHookReturn ggz_chat(GGZRoomEvent id, void* event_data, void* user_data)
{
	GGZChatEventData *chat_data = event_data;

	chat_display_server(chat_data->type,
			    chat_data->sender, chat_data->message);

	return GGZ_HOOK_OK;
}


static GGZHookReturn ggz_chat_fail(GGZRoomEvent id, void* event_data,
				   void* user_data)
{
	GGZErrorEventData *error = event_data;
	char buf[512];

	switch (error->status) {
	case E_NOT_IN_ROOM:
		chat_display_local(CHAT_LOCAL_NORMAL, NULL,
				   _("You can't chat while not in a room."));
		break;
	case E_NO_PERMISSION:
		chat_display_local(CHAT_LOCAL_NORMAL, NULL,
				   _("You don't have permission to chat here."));
		break;
	case E_AT_TABLE:
		chat_display_local(CHAT_LOCAL_NORMAL, NULL,
				   _("No private chatting at a table!."));
		break;
	case E_USR_LOOKUP:
		chat_display_local(CHAT_LOCAL_NORMAL, NULL,
				   _("That player isn't in the room!"));
		break;
	case E_BAD_OPTIONS:
		chat_display_local(CHAT_LOCAL_NORMAL, NULL,
				   _("There was an error sending the chat."));
		break;
	case E_NO_TABLE:
		chat_display_local(CHAT_LOCAL_NORMAL, NULL,
				   _("You're not at a table."));
		break;
	default:
		snprintf(buf, sizeof(buf),
			 _("Chat failed: %s."),
			 error->message);
		chat_display_local(CHAT_LOCAL_NORMAL, NULL, buf);
		break;
	}

	return GGZ_HOOK_OK;
}


static GGZHookReturn ggz_list_players(GGZRoomEvent id, void* event_data, void* user_data)
{
	int *room = event_data;

	display_players();
	if(room) update_one_room(*room);
	return GGZ_HOOK_OK;
}

static GGZHookReturn ggz_player_count(GGZRoomEvent id, void* event_data, void* user_data)
{
	int *room = (int *)event_data;
	update_one_room(*room);
	return GGZ_HOOK_OK;
}

static GGZHookReturn ggz_room_enter(GGZRoomEvent id,\
				    void* event_data, void* user_data)
{
	GGZRoomChangeEventData *data = event_data;

	update_one_room(data->to_room);
	update_one_room(data->from_room);
	display_players();
	chat_enter(data->player_name, data->from_room);

	return GGZ_HOOK_OK;
}


static GGZHookReturn ggz_room_leave(GGZRoomEvent id,
				    void* event_data, void* user_data)
{
	GGZRoomChangeEventData *data = event_data;

	update_one_room(data->to_room);
	update_one_room(data->from_room);
	display_players();
	chat_part(data->player_name, data->to_room);

	return GGZ_HOOK_OK;
}


void ggz_sensitivity_init(void)
{
	GtkWidget *tmp;

	/* set senditivity */
	/* Menu bar */
	
	/* Set the room label to current room */
	tmp = lookup_widget(win_main, "Current_room_label");
	gtk_label_set_text(GTK_LABEL(tmp), _("Current Room:"));
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
	case GGZ_STATE_LAUNCHING_TABLE:
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


static GGZHookReturn ggz_state_sensitivity(GGZServerEvent id, void* event_data, void* user_data)
{
	GtkWidget *tmp;
	GGZStateID state_id;

	state_id = ggzcore_server_get_state(server);
	
	switch (state_id) {
	case GGZ_STATE_OFFLINE:
		/* Re-enable connect button */
		if (login_dialog) {
			tmp = lookup_widget(login_dialog, "connect_button");
			gtk_widget_set_sensitive(tmp, TRUE);
		}

		/* Menu bar */
		tmp = lookup_widget(win_main, "connect");
		gtk_widget_set_sensitive(tmp, TRUE);
		
		tmp = lookup_widget(win_main, "disconnect");
		gtk_widget_set_sensitive(tmp, FALSE);

		tmp = lookup_widget(win_main, "game");
		gtk_widget_set_sensitive(tmp, FALSE);

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

		tmp = lookup_widget(win_main, "watch_button");
		gtk_widget_set_sensitive(tmp, FALSE);

		tmp = lookup_widget(win_main, "leave_button");
		gtk_widget_set_sensitive(tmp, FALSE);
		
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
		break;

	case GGZ_STATE_CONNECTING:
		if (login_dialog) {
			/* Desensitize the connect button */
			tmp = lookup_widget(login_dialog, "connect_button");
			gtk_widget_set_sensitive(tmp, FALSE);
		}
		
		/* Menu bar */
		tmp = lookup_widget(win_main, "connect");
		gtk_widget_set_sensitive(tmp, FALSE);

		tmp = lookup_widget(win_main, "disconnect");
		gtk_widget_set_sensitive(tmp, TRUE);

		/* Tool bar */
		tmp = lookup_widget(win_main, "connect_button");
		gtk_widget_set_sensitive(tmp, FALSE);

		tmp = lookup_widget(win_main, "disconnect_button");
		gtk_widget_set_sensitive(tmp, TRUE);
		break;

	case GGZ_STATE_ONLINE:
		break;

	case GGZ_STATE_LOGGING_IN:
		if (login_dialog) {
			/* Desensitize the connect button */
			tmp = lookup_widget(login_dialog, "connect_button");
			gtk_widget_set_sensitive(tmp, FALSE);
		}

		break;

	case GGZ_STATE_LOGGED_IN:
		/* set senditivity */
		/* Menu bar */
		tmp = lookup_widget(win_main, "view");
		gtk_widget_set_sensitive(tmp, TRUE);
		
		/* Tool bar */
		tmp = lookup_widget(win_main, "stats_button");
		gtk_widget_set_sensitive(tmp, TRUE);
		
		/* Client area */
		tmp = lookup_widget(win_main, "room_clist");
		gtk_widget_set_sensitive(tmp, TRUE);
		break;

	case GGZ_STATE_BETWEEN_ROOMS:
	case GGZ_STATE_ENTERING_ROOM:
		tmp = lookup_widget(win_main, "chat_entry");
		gtk_widget_set_sensitive(tmp, FALSE);

		tmp = lookup_widget(win_main, "send_button");
		gtk_widget_set_sensitive(tmp, FALSE);
		break;

	case GGZ_STATE_IN_ROOM:
		/* Menu bar */
		tmp = lookup_widget(win_main, "game");
		gtk_widget_set_sensitive(tmp, TRUE);

		tmp = lookup_widget(win_main, "launch");
		gtk_widget_set_sensitive(tmp, TRUE);

		tmp = lookup_widget(win_main, "join");
		gtk_widget_set_sensitive(tmp, TRUE);
		
		/* Tool bar */
		tmp = lookup_widget(win_main, "launch_button");
		gtk_widget_set_sensitive(tmp, TRUE);
		
		tmp = lookup_widget(win_main, "join_button");
		gtk_widget_set_sensitive(tmp, TRUE);

		tmp = lookup_widget(win_main, "watch_button");
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
		break;

	case GGZ_STATE_LAUNCHING_TABLE:
		/* Tool bar */
		tmp = lookup_widget(win_main, "launch_button");
		gtk_widget_set_sensitive(tmp, FALSE);

		tmp = lookup_widget(win_main, "join_button");
		gtk_widget_set_sensitive(tmp, FALSE);

		tmp = lookup_widget(win_main, "watch_button");
		gtk_widget_set_sensitive(tmp, FALSE);

		/* Menu bar */
		tmp = lookup_widget(win_main, "launch");
		gtk_widget_set_sensitive(tmp, FALSE);

		tmp = lookup_widget(win_main, "join");
		gtk_widget_set_sensitive(tmp, FALSE);
		break;

	case GGZ_STATE_JOINING_TABLE:
		/* Tool bar */
		tmp = lookup_widget(win_main, "launch_button");
		gtk_widget_set_sensitive(tmp, FALSE);

		tmp = lookup_widget(win_main, "join_button");
		gtk_widget_set_sensitive(tmp, FALSE);

		tmp = lookup_widget(win_main, "watch_button");
		gtk_widget_set_sensitive(tmp, FALSE);

		/* Menu bar */
		tmp = lookup_widget(win_main, "launch");
		gtk_widget_set_sensitive(tmp, FALSE);

		tmp = lookup_widget(win_main, "join");
		gtk_widget_set_sensitive(tmp, FALSE);
		break;
		
	case GGZ_STATE_AT_TABLE:
		/* Tool bar */
		tmp = lookup_widget(win_main, "leave_button");
		gtk_widget_set_sensitive(tmp, TRUE);

		/* Menu bar */
		tmp = lookup_widget(win_main, "leave");
		gtk_widget_set_sensitive(tmp, TRUE);
		break;

	case GGZ_STATE_LEAVING_TABLE:
		/* Tool bar */
		tmp = lookup_widget(win_main, "leave_button");
		gtk_widget_set_sensitive(tmp, FALSE);

		/* Menu bar */
		tmp = lookup_widget(win_main, "leave");
		gtk_widget_set_sensitive(tmp, FALSE);
		break;

	case GGZ_STATE_LOGGING_OUT:
		/* Menu bar */
		tmp = lookup_widget(win_main, "disconnect");
		gtk_widget_set_sensitive(tmp, FALSE);

		tmp = lookup_widget(win_main, "game");
		gtk_widget_set_sensitive(tmp, FALSE);

		tmp = lookup_widget(win_main, "view");
		gtk_widget_set_sensitive(tmp, FALSE);
		
		/* Tool bar */
		tmp = lookup_widget(win_main, "disconnect_button");
		gtk_widget_set_sensitive(tmp, FALSE);

		tmp = lookup_widget(win_main, "launch_button");
		gtk_widget_set_sensitive(tmp, FALSE);
		
		tmp = lookup_widget(win_main, "join_button");
		gtk_widget_set_sensitive(tmp, FALSE);

		tmp = lookup_widget(win_main, "watch_button");
		gtk_widget_set_sensitive(tmp, FALSE);

		tmp = lookup_widget(win_main, "leave_button");
		gtk_widget_set_sensitive(tmp, FALSE);
		
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
		break;
	default:
		break;
		
	}

	return GGZ_HOOK_OK;
}


static GGZHookReturn ggz_server_error(GGZServerEvent id, void* event_data, void* user_data)
{
	gchar* msg;
	
	ggz_debug("connection", "Server error.");
	
	server_disconnect();
	
	/* SHould we clear the list of rooms/players/tables? */

	msg = g_strdup_printf(_("Server error: %s"), (char*)event_data);
	msgbox(msg, _("Error"), MSGBOX_OKONLY, MSGBOX_STOP, MSGBOX_NORMAL);
	g_free(msg);

	return GGZ_HOOK_OK;
}

static GGZHookReturn ggz_net_error(GGZServerEvent id, void* event_data, void* user_data)
{
	GtkWidget *tmp;
	
	ggz_debug("connection", "Net error.");

	server_disconnect();

        /* Clear current list of rooms */
        tmp = gtk_object_get_data(GTK_OBJECT(win_main), "room_clist");
        gtk_clist_clear(GTK_CLIST(tmp));
 
        /* Clear current list of players */
        tmp = gtk_object_get_data(GTK_OBJECT(win_main), "player_clist");
        gtk_clist_clear(GTK_CLIST(tmp));

        /* Clear current list of tables */
        tmp = gtk_object_get_data(GTK_OBJECT(win_main), "table_clist");
        gtk_clist_clear(GTK_CLIST(tmp));

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

	msg = g_strdup_printf(_("Error launching table: %s"), (char*)event_data);
	msgbox(msg, _("Error"), MSGBOX_OKONLY, MSGBOX_STOP, MSGBOX_NORMAL);
	g_free(msg);

	game_quit();

	return GGZ_HOOK_OK;
}


static GGZHookReturn ggz_table_joined(GGZRoomEvent id, void* event_data,
				      void* user_data)
{
	int table_id = *(int*)event_data;

	char message[1024];
	snprintf(message, sizeof(message),
		 _("You have joined table %d."), table_id);
	chat_display_local(CHAT_LOCAL_NORMAL, NULL, message);

	return GGZ_HOOK_OK;
}


static GGZHookReturn ggz_table_join_fail(GGZRoomEvent id, void* event_data, void* user_data)
{
	gchar* msg;

	msg = g_strdup_printf(_("Error joining table: %s"), (char*)event_data);
	msgbox(msg, _("Error"), MSGBOX_OKONLY, MSGBOX_STOP, MSGBOX_NORMAL);
	g_free(msg);

	game_quit();

	return GGZ_HOOK_OK;
}


static GGZHookReturn ggz_table_left(GGZRoomEvent id, void* event_data,
				    void* user_data)
{
	GGZTableLeaveEventData *data = event_data;
	char message[1024] = "???";

	switch (data->reason) {
	case GGZ_LEAVE_BOOT:
		snprintf(message, sizeof(message),
			 _("You have been booted from the table by %s."),
			 data->player);
		break;
	case GGZ_LEAVE_NORMAL:
		snprintf(message, sizeof(message),
			 _("You have left the table."));
		break;
	case GGZ_LEAVE_GAMEOVER:
		snprintf(message, sizeof(message),
			 _("The game is over."));
		break;
	case GGZ_LEAVE_GAMEERROR:
		snprintf(message, sizeof(message),
			 _("There was an error with the game server."));
		break;
	}
	chat_display_local(CHAT_LOCAL_NORMAL, NULL, message);

	game_quit();

	return GGZ_HOOK_OK;
}


static GGZHookReturn ggz_table_leave_fail(GGZRoomEvent id, void* event_data, void* user_data)
{
	gchar* msg;

	msg = g_strdup_printf(_("Error leaving table: %s"), (char*)event_data);
	msgbox(msg, _("Error"), MSGBOX_OKONLY, MSGBOX_STOP, MSGBOX_NORMAL);
	g_free(msg);

	return GGZ_HOOK_OK;
}
	

/* The below function are used to
 * add and remove servers to the 
 * main loop
 */

static void ggz_check_fd(gpointer my_server, gint fd, GdkInputCondition cond)
{
	assert(server == my_server);
	ggzcore_server_read_data(my_server, fd);
}


/* GdkDestroyNotify function for server fd */
static void ggz_input_removed(gpointer data)
{
	if (!server) {
		/* This function should only be called once when we
		   disconnect; calling it more than once would attempt to
		   free the server twice.  See server_disconnect().  It's
		   possible this could cause some errors since the
		   ggzcore_server_free() doesn't get called immediately.
		   The alternative is to put this code into
		   server_disconnect(), and drop this function
		   entirely.

		   Actually, this function can be called after the server
		   data has already been deallocated: if ggzd spontaneously
		   shuts down, for instance.  Just one more but of ugliness...

		   --JDS */
		return;
	}

	assert(server == data);

	ggzcore_server_free((GGZServer*)data);
	server = NULL;
}


static GGZHookReturn ggz_auto_join(GGZServerEvent id, void* event_data, void* user_data)
{
	GtkWidget *tmp;

	ggzcore_server_join_room(server, 0);
	tmp =  lookup_widget(win_main, "room_clist");
	gtk_clist_select_row(GTK_CLIST(tmp), 0, 0);

	return GGZ_HOOK_REMOVE;
}


int ggz_connection_query(void)
{
	if(server_handle < 0)
		return 0;
	return 1;
}

/* Handle the ggz-gtk end of disconnecting. */
void server_disconnect(void)
{
	assert(server);

	/* This can happen if we get disconnected from a server that we never
	   actually connected to (for instance if there's no server listening
	   on the host/port we connected to).  We still have to free the
	   server variable, but not remove the input handler.  This is ugly.
	   See ggz_input_removed(). */
	if (server_handle == -1 && server) {
		ggzcore_server_free(server);
		server = NULL;
		return;
	}

	/* Removing the input handler prompts the calling of 
	   ggz_input_removed, which frees the server data.   This is
	   a bit kludgy, but... */
	gdk_input_remove(server_handle);
	server_handle = -1;

	chat_display_local(CHAT_LOCAL_HIGH, NULL,
			   _("Disconnected from server."));
}
