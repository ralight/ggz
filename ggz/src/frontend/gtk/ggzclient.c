/*
 * File: ggzclient.c
 * Author: Justin Zaun
 * Project: GGZ GTK Client
 * $Id: ggzclient.c 3201 2002-02-01 04:55:51Z jdorje $
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

gint numrooms;
gint numtables;

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
static GGZHookReturn ggz_chat_announce(GGZRoomEvent id, void* event_data, void* user_data);
static GGZHookReturn ggz_list_players(GGZRoomEvent id, void* event_data, void* user_data);

static GGZHookReturn ggz_room_enter(GGZRoomEvent id, void* event_data, void* user_data);
static GGZHookReturn ggz_room_leave(GGZRoomEvent id, void* event_data, void* user_data);
static GGZHookReturn ggz_list_tables(GGZRoomEvent id, void* event_data, void* user_data);
static GGZHookReturn ggz_table_update(GGZRoomEvent id, void* event_data, void* user_data);
static GGZHookReturn ggz_state_change(GGZServerEvent id, void* event_data, void* user_data);
static GGZHookReturn ggz_state_sensitivity(GGZServerEvent id, void* event_data, void* user_data);
static void ggz_input_removed(gpointer data);

/* Table functions */
static GGZHookReturn ggz_table_launched(GGZRoomEvent id, void*, void*);
static GGZHookReturn ggz_table_launch_fail(GGZRoomEvent id, void*, void*);
static GGZHookReturn ggz_table_joined(GGZRoomEvent id, void*, void*);
static GGZHookReturn ggz_table_join_fail(GGZRoomEvent id, void*, void*);
static GGZHookReturn ggz_table_left(GGZRoomEvent id, void*, void*);
static GGZHookReturn ggz_table_leave_fail(GGZRoomEvent id, void*, void*);
static GGZHookReturn ggz_table_data(GGZRoomEvent id, void*, void*);

/* One Time Functions */
static GGZHookReturn ggz_auto_join(GGZServerEvent id, void*, void*);


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
	ggzcore_server_add_event_hook(server, GGZ_STATE_CHANGE, ggz_state_sensitivity);
	ggzcore_server_add_event_hook(server, GGZ_TABLE_LEFT, ggz_table_left);
	ggzcore_server_add_event_hook(server, GGZ_PROTOCOL_ERROR, ggz_server_error);
	ggzcore_server_add_event_hook(server, GGZ_NET_ERROR, ggz_net_error);
}


static GGZHookReturn ggz_connected(GGZServerEvent id, void* event_data, void* user_data)
{
	int fd;

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
	gint i;
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
		ggzcore_room_add_event_hook(room, GGZ_ANNOUNCE, ggz_chat_announce);
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
		ggzcore_room_add_event_hook(room, GGZ_PLAYER_LAG, ggz_list_players);
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
	GtkArg arg[1];

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
	chat_display_message(CHAT_LOCAL_NORMAL, NULL, message);
	g_free(message);
	chat_display_message(CHAT_LOCAL_NORMAL, NULL,  ggzcore_room_get_desc(ggzcore_server_get_cur_room(server)));

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
	}else{
		arg[0].name = "user_data";
		tmp = lookup_widget(win_main, "table_vpaned");
		gtk_object_getv(GTK_OBJECT(tmp), 1, arg);
		gtk_paned_set_position(GTK_PANED(tmp), GTK_VALUE_UINT(arg[0]));
		tmp = lookup_widget(win_main, "launch_button");
		gtk_widget_set_sensitive(tmp, TRUE);
		tmp = lookup_widget(win_main, "join_button");
		gtk_widget_set_sensitive(tmp, TRUE);
	}

	return GGZ_HOOK_OK;
}


static GGZHookReturn ggz_entered_fail(GGZServerEvent id, void* event_data, void* user_data)
{
	gchar* msg;

	msg = g_strdup_printf("Error joining room: %s", (char*)event_data);
	msgbox(msg, "Error", MSGBOX_OKONLY, MSGBOX_STOP, MSGBOX_NORMAL);
	g_free(msg);
	
	return GGZ_HOOK_OK;
}


static GGZHookReturn ggz_logout(GGZServerEvent id, void* event_data, void* user_data)
{
	gdk_input_remove(server_handle);
	server_handle = -1;
	chat_display_message(CHAT_LOCAL_HIGH, NULL, _("Disconnected from Server."));

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
	chat_display_message(CHAT_LOCAL_NORMAL, NULL, message);
	if( ggzcore_conf_read_int("CHAT", "SOUND", TRUE) )
		gdk_beep();

	g_free(message);

	return GGZ_HOOK_OK;
}

static GGZHookReturn ggz_chat_announce(GGZRoomEvent id, void* event_data, void* user_data)
{
	gchar *message;
	gchar *player;

	player = ((char**)(event_data))[0];
	message = ((char**)(event_data))[1];
	chat_display_message(CHAT_ANNOUNCE, player, message);

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

	msg = g_strdup_printf("Server error: %s", (char*)event_data);
	msgbox(msg, "Error", MSGBOX_OKONLY, MSGBOX_STOP, MSGBOX_NORMAL);
	g_free(msg);

	return GGZ_HOOK_OK;
}

static GGZHookReturn ggz_net_error(GGZServerEvent id, void* event_data, void* user_data)
{
	GtkWidget *tmp;

	gdk_input_remove(server_handle);
	server_handle = -1;
	/*ggzcore_server_free(server);*/
	chat_display_message(CHAT_LOCAL_HIGH, NULL, _("Disconnected from Server."));

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
	gchar *table[4] = {NULL, NULL, NULL, NULL}, *desc;
	gint i, num, avail, seats;
	GGZRoom *room;
	GGZTable *t = NULL;

	/* Retrieve the list. */
	tmp = lookup_widget(win_main, "table_clist");
	
	/* "Freeze" the clist.  This prevents any graphical updating
	 * until we "thaw" it later. */
	gtk_clist_freeze(GTK_CLIST(tmp));
	
	/* clear the list */
	gtk_clist_clear(GTK_CLIST(tmp));

	room = ggzcore_server_get_cur_room(server);

	/* Display current list of players
	if (!(numbers = ggzcore_room_get_numbers()))
		return GGZ_HOOK_OK;*/
	
	numtables = ggzcore_room_get_num_tables(room);
	for (i = 0; i < numtables; i++) {
	
		t = ggzcore_room_get_nth_table(room, i);
		num   = ggzcore_table_get_id(t);
		avail = ggzcore_table_get_seat_count(t, GGZ_SEAT_OPEN)
			+ ggzcore_table_get_seat_count(t, GGZ_SEAT_RESERVED);
		seats = ggzcore_table_get_num_seats(t);
		desc = ggzcore_table_get_desc(t);
		
		/* FIXME: we have a significant problem here.  Do we
		   display the number of open seats, the number of
		   available-to-us seats, or the total number of
		   unfilled seats?  Any way we do it we'll have
		   problems.  Right now I just show the total
		   number of unfilled seats. */
			
		table[0] = g_strdup_printf("%d", num);
		table[1] = g_strdup_printf("%d/%d", avail, seats);
		table[2] = g_strdup_printf("%s", desc);
		gtk_clist_append(GTK_CLIST(tmp), table);
		g_free(table[0]);
		g_free(table[1]);
		g_free(table[2]);
	}
	
	/* "Thaw" the clist (it was "frozen" up above). */
	gtk_clist_thaw(GTK_CLIST(tmp));
}


void display_players(void)
{
	GtkWidget *tmp;
	gint i, num;
	/* Some of the fields of the clist receive pixmaps
	 * instead, and are therefore set to NULL. */
	gchar *player[4] = {NULL, NULL, NULL, NULL} ;
	gchar *path = NULL;
	GGZPlayer *p;
	GGZTable *table;
	GGZRoom *room = ggzcore_server_get_cur_room(server);
	GdkPixmap *pixmap1 = NULL, *pixmap2 = NULL;
	GdkBitmap *mask1, *mask2;
	
	/* Retrieve the player list (CList) widget. */
	tmp = lookup_widget(win_main, "player_clist");
	
	/* "Freeze" the clist.  This prevents any graphical updating
	 * until we "thaw" it later. */
	gtk_clist_freeze(GTK_CLIST(tmp));
	
	/* Clear current list of players */
	gtk_clist_clear(GTK_CLIST(tmp));

	/* Display current list of players */
	num = ggzcore_room_get_num_players(room);

	for (i = 0; i < num; i++) {
		p = ggzcore_room_get_nth_player(room, i);

		table = ggzcore_player_get_table(p);
		if(!table)
			player[1] = g_strdup("--");
		/*else if (ggzcore_player_get_table(names[i]) == -2)
		  player[1] = g_strdup("??");*/
		else
			player[1] = g_strdup_printf("%d", ggzcore_table_get_id(table));

		player[2] = g_strdup(ggzcore_player_get_name(p));
		gtk_clist_append(GTK_CLIST(tmp), player);

		if(ggzcore_player_get_lag(p) == -1 || ggzcore_player_get_lag(p) ==0)
			path = g_strjoin(G_DIR_SEPARATOR_S, GGZDATADIR, 
					 "ggz_gtk_lag0.xpm", NULL);
		else if(ggzcore_player_get_lag(p) == 1)
			path = g_strjoin(G_DIR_SEPARATOR_S, GGZDATADIR, 
					 "ggz_gtk_lag1.xpm", NULL);
		else if(ggzcore_player_get_lag(p) == 2)
			path = g_strjoin(G_DIR_SEPARATOR_S, GGZDATADIR, 
					 "ggz_gtk_lag2.xpm", NULL);
		else if(ggzcore_player_get_lag(p) == 3)
			path = g_strjoin(G_DIR_SEPARATOR_S, GGZDATADIR, 
					 "ggz_gtk_lag3.xpm", NULL);
		else if(ggzcore_player_get_lag(p) == 4)
			path = g_strjoin(G_DIR_SEPARATOR_S, GGZDATADIR, 
					 "ggz_gtk_lag4.xpm", NULL);
		else if(ggzcore_player_get_lag(p) == 5)
			path = g_strjoin(G_DIR_SEPARATOR_S, GGZDATADIR, 
					 "ggz_gtk_lag5.xpm", NULL);
		
		if (path) {
			pixmap1 = gdk_pixmap_create_from_xpm(tmp->window, &mask1,
							    NULL, path);
			if (pixmap1)
				gtk_clist_set_pixmap(GTK_CLIST(tmp), i, 0, pixmap1, mask1);
			g_free(path);
		}


		if(ggzcore_player_get_type(p) == GGZ_PLAYER_GUEST)
			path = g_strjoin(G_DIR_SEPARATOR_S, GGZDATADIR, 
					 "ggz_gtk_guest.xpm", NULL);
		else if(ggzcore_player_get_type(p) == GGZ_PLAYER_NORMAL)
			path = g_strjoin(G_DIR_SEPARATOR_S, GGZDATADIR, 
					 "ggz_gtk_registered.xpm", NULL);
		else if(ggzcore_player_get_type(p) == GGZ_PLAYER_ADMIN)
			path = g_strjoin(G_DIR_SEPARATOR_S, GGZDATADIR, 
					 "ggz_gtk_admin.xpm", NULL);
		
		if (path) {
			pixmap2 = gdk_pixmap_create_from_xpm(tmp->window, &mask2,
							    NULL, path);
			if (pixmap2)
				gtk_clist_set_pixtext(GTK_CLIST(tmp), i, 2, player[2], 5, pixmap2, mask2);
			g_free(path);
		}
		
//		g_free(player[0]);
		g_free(player[1]);
		g_free(player[2]);
	}
	
	/* "Thaw" the clist (it was "frozen" up above). */
	gtk_clist_thaw(GTK_CLIST(tmp));
}


static GGZHookReturn ggz_auto_join(GGZServerEvent id, void* event_data, void* user_data)
{
	GtkWidget *tmp;

	ggzcore_server_join_room(server, 0);
	tmp =  lookup_widget(win_main, "room_clist");
	gtk_clist_select_row(GTK_CLIST(tmp), 0, 0);

	return GGZ_HOOK_REMOVE;
}

