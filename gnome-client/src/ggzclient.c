/*
 * File: ggzclient.c
 * Author: Justin Zaun
 * Project: GGZ GTK Client
 * $Id: ggzclient.c 5097 2002-10-29 00:59:19Z jzaun $
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
#include <gconf/gconf-client.h>

#include "ggz.h"
#include "login.h"
#include "support.h"
#include "xtext.h"
#include "interface.h"
#include "chat.h"
#include "msgboxi.h"
#include "ggzclient.h"
#include "callbacks.h"
#include "game.h"

static gint server_handle = -1;

extern GGZServer *server;
extern GtkWidget *interface;
extern GConfClient *config;

gint numrooms;
gint numtables;

void display_tables(void);
void display_players(void);

static GGZHookReturn ggz_connected(GGZServerEvent id, void* event_data, void* user_data);
static GGZHookReturn ggz_connect_fail(GGZServerEvent id, void* event_data, void* user_data);
static GGZHookReturn ggz_negotiated(GGZServerEvent id, void* event_data, void* user_data);
static GGZHookReturn ggz_logged_in(GGZServerEvent id, void* event_data, void* user_data);
static GGZHookReturn ggz_login_fail(GGZServerEvent id, void* event_data, void* user_data);
static GGZHookReturn ggz_channel_connected(GGZServerEvent id, void *event_data, void *user_data);
static GGZHookReturn ggz_channel_ready(GGZGameEvent id, void* event_data, void* user_data);
static GGZHookReturn ggz_room_list(GGZServerEvent id, void* event_data, void* user_data);
static GGZHookReturn ggz_entered(GGZServerEvent id, void* event_data, void* user_data);
static GGZHookReturn ggz_entered_fail(GGZServerEvent id, void* event_data, void* user_data);
static GGZHookReturn ggz_logout(GGZServerEvent id, void* event_data, void* user_data);
static GGZHookReturn ggz_motd_loaded(GGZServerEvent id, void* event_data, void* user_data);
static GGZHookReturn ggz_table_left(GGZServerEvent id, void* event_data, void* user_data);
static GGZHookReturn ggz_server_error(GGZServerEvent id, void* event_data, void* user_data);
static GGZHookReturn ggz_net_error(GGZServerEvent id, void* event_data, void* user_data);

static GGZHookReturn ggz_chat_event(GGZRoomEvent id, void* event_data, void* user_data);
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

/* One Time Functions */
static GGZHookReturn ggz_auto_join(GGZServerEvent id, void*, void*);

/* Helper functions */
static void server_disconnect(void);


GdkInputFunction ggz_check_fd(gpointer server, gint fd, GdkInputCondition cond);

void ggz_event_init(GGZServer *Server)
{
	ggzcore_server_add_event_hook(Server, GGZ_CONNECTED, ggz_connected);
	ggzcore_server_add_event_hook(Server, GGZ_CONNECT_FAIL,	ggz_connect_fail);
	ggzcore_server_add_event_hook(Server, GGZ_NEGOTIATED, ggz_negotiated);
	ggzcore_server_add_event_hook(Server, GGZ_NEGOTIATE_FAIL, ggz_connect_fail);
	ggzcore_server_add_event_hook(Server, GGZ_LOGGED_IN, ggz_logged_in);
	ggzcore_server_add_event_hook(Server, GGZ_LOGIN_FAIL, ggz_login_fail);
	ggzcore_server_add_event_hook(server, GGZ_CHANNEL_CONNECTED, ggz_channel_connected);
	ggzcore_server_add_event_hook(server, GGZ_CHANNEL_READY, ggz_channel_ready);
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
	
	ggz_debug("connection", "We're connected.");

	
	if(id == GGZ_CONNECTED) 
	{
		/* Add the fd to the ggtk main loop */
		fd = ggzcore_server_get_fd(server);
		server_handle = gdk_input_add_full(fd, GDK_INPUT_READ, 
						   (GdkInputFunction)ggz_check_fd, 
						   (gpointer)server,
						   ggz_input_removed);
	}
	
	return GGZ_HOOK_OK;
}


static GGZHookReturn ggz_connect_fail(GGZServerEvent id, void* event_data, void* user_data)
{
	gchar* msg;

	msg = g_strdup_printf("Error connecting to server: %s", (char*)event_data);
	msgbox_error(msg);
	g_free(msg);

	return GGZ_HOOK_OK;
}


static GGZHookReturn ggz_negotiated(GGZServerEvent id, void* event_data, void* user_data)
{
	ggzcore_server_login(server);
	
	return GGZ_HOOK_OK;
}


static GGZHookReturn ggz_logged_in(GGZServerEvent id, void* event_data, void* user_data)
{
	GtkWidget *tmp;
	GSList *slProfileNames, *slProfileUsernames, *slProfileServers, *slProfilePWDs;
	gint intProfiles;
	
	login_set_chat(interface);
	ggzcore_server_add_event_hook(server, GGZ_ROOM_LIST, ggz_auto_join);
	ggzcore_server_list_rooms(server, -1, 1);
	ggzcore_server_list_gametypes(server, 1);


	/* If this was a first-time login, get the password from the server */
	if (ggzcore_server_get_type(server) == GGZ_LOGIN_NEW) {
		/*Get current information*/
		slProfileNames = gconf_client_get_list (config, "/schemas/apps/ggz-gnome/profiles/profiles", GCONF_VALUE_STRING, NULL);
		slProfileUsernames = gconf_client_get_list (config, "/schemas/apps/ggz-gnome/profiles/usernames", GCONF_VALUE_STRING, NULL);
		slProfileServers = gconf_client_get_list (config, "/schemas/apps/ggz-gnome/profiles/servers", GCONF_VALUE_STRING, NULL);
		slProfilePWDs = gconf_client_get_list (config, "/schemas/apps/ggz-gnome/profiles/passwords", GCONF_VALUE_STRING, NULL);
		intProfiles = gconf_client_get_int (config, "/schemas/apps/ggz-gnome/profiles/total", NULL);

		/*Update the lists*/
		tmp = lookup_widget (interface, "entNewProfileName");
		slProfileNames = g_slist_append (slProfileNames,
			(char*)gtk_entry_get_text (GTK_ENTRY(tmp)));
		slProfileUsernames = g_slist_append (slProfileUsernames, ggzcore_server_get_handle (server));
		slProfileServers = g_slist_append (slProfileServers, g_strdup_printf ("%s:%d", ggzcore_server_get_host (server), ggzcore_server_get_port (server)));
		slProfilePWDs = g_slist_append (slProfilePWDs, ggzcore_server_get_password(server));
		intProfiles++;

		/*Store the information back*/
		gconf_client_set_list (config, "/schemas/apps/ggz-gnome/profiles/profiles", GCONF_VALUE_STRING, slProfileNames, NULL);
		gconf_client_set_list (config, "/schemas/apps/ggz-gnome/profiles/usernames", GCONF_VALUE_STRING, slProfileUsernames, NULL);
		gconf_client_set_list (config, "/schemas/apps/ggz-gnome/profiles/servers", GCONF_VALUE_STRING, slProfileServers, NULL);
		gconf_client_set_list (config, "/schemas/apps/ggz-gnome/profiles/passwords", GCONF_VALUE_STRING, slProfilePWDs, NULL);
		gconf_client_set_int (config, "/schemas/apps/ggz-gnome/profiles/total", intProfiles, NULL);
	}
		

	return GGZ_HOOK_OK;
}


static GGZHookReturn ggz_login_fail(GGZServerEvent id, void* event_data, void* user_data)
{
	gchar *msg;
	GtkWidget *tmp;
	
	msg = g_strdup_printf("%s", (gchar*)event_data);
	msgbox_error(msg);
	g_free(msg);

	tmp = lookup_widget(interface, "btnLogin");
	gtk_widget_set_sensitive(tmp, TRUE);
	tmp = lookup_widget(interface, "btnNew");
	gtk_widget_set_sensitive(tmp, TRUE);

	return GGZ_HOOK_OK;
}

static GGZHookReturn ggz_channel_connected(GGZServerEvent id, void *event_data, void *user_data)
{
	game_channel_connected(ggzcore_server_get_channel(server));
	return GGZ_HOOK_OK;
}

static GGZHookReturn ggz_channel_ready(GGZGameEvent id, void* event_data, void* user_data)
{
	game_channel_ready(ggzcore_server_get_channel(server));

	return GGZ_HOOK_OK;
}

static GGZHookReturn ggz_room_list(GGZServerEvent id, void* event_data, void* user_data)
{
	GtkWidget *vpRooms, *vbRooms, *hbRoom, *vbRoom, *pmRoom, *lblRoomName, *lblRoomDesc, *hsRoom, *ebRoom;
	GGZRoom *room;
	gint i;
	gchar *name, *desc, *strRoomNum;

	/* Clear current list of rooms */
	vpRooms = lookup_widget(interface, "vpRooms");
	vbRooms = lookup_widget(interface, "vbRooms");
	gtk_widget_destroy(vbRooms);

	vbRooms = gtk_vbox_new (FALSE, 0);
	gtk_widget_ref (vbRooms);
	gtk_object_set_data_full (GTK_OBJECT (interface), "vbRooms", vbRooms,
				  (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (vbRooms);
	gtk_container_add (GTK_CONTAINER (vpRooms), vbRooms);
 
	/* Display current list of rooms */
	numrooms = ggzcore_server_get_num_rooms(server);
	if (numrooms == 0)
		return GGZ_HOOK_OK;

	for (i = 0; i < numrooms; i++)
	{
		strRoomNum = g_strdup_printf("%d", i);
		room = ggzcore_server_get_nth_room(server, i);
		
		ebRoom = gtk_event_box_new();
		gtk_widget_ref (ebRoom);
		gtk_widget_show (ebRoom);
		gtk_widget_set_name(ebRoom, strRoomNum);
		gtk_signal_connect (GTK_OBJECT (ebRoom), "event",
				GTK_SIGNAL_FUNC (on_room_click), NULL);
		
		hbRoom = gtk_hbox_new (FALSE, 5);
		gtk_widget_ref (hbRoom);
		gtk_widget_show (hbRoom);
		gtk_container_add (GTK_CONTAINER (ebRoom), hbRoom);

		pmRoom = create_pixmap (interface, NULL);
		gtk_widget_ref (pmRoom);
		gtk_widget_show (pmRoom);
		gtk_box_pack_start (GTK_BOX (hbRoom), pmRoom, FALSE, FALSE, 0);
		gtk_widget_set_usize (pmRoom, 64, 64);
		  
		vbRoom = gtk_vbox_new (FALSE, 0);
		gtk_widget_ref (vbRoom);
		gtk_widget_show (vbRoom);
		gtk_box_pack_start (GTK_BOX (hbRoom), vbRoom, TRUE, TRUE, 0);
		
		name = ggzcore_room_get_name(room);
		lblRoomName = gtk_label_new (name);
		gtk_widget_ref (lblRoomName);
		gtk_widget_show (lblRoomName);
		gtk_box_pack_start (GTK_BOX (vbRoom), lblRoomName, TRUE, FALSE, 0);
		gtk_label_set_justify (GTK_LABEL (lblRoomName), GTK_JUSTIFY_LEFT);
		gtk_misc_set_alignment (GTK_MISC (lblRoomName), 0, 0.5);
		
		desc = ggzcore_room_get_desc(room);
		lblRoomDesc = gtk_label_new (desc);
		gtk_widget_ref (lblRoomDesc);
		gtk_widget_show (lblRoomDesc);
		gtk_box_pack_start (GTK_BOX (vbRoom), lblRoomDesc, TRUE, FALSE, 0);
		gtk_label_set_justify (GTK_LABEL (lblRoomDesc), GTK_JUSTIFY_LEFT);
		gtk_label_set_line_wrap (GTK_LABEL (lblRoomDesc), TRUE);
		gtk_misc_set_alignment (GTK_MISC (lblRoomDesc), 0, 0.5);
		
		gtk_box_pack_start (GTK_BOX (vbRooms), ebRoom, TRUE, TRUE, 0);
		g_free (strRoomNum);

		if (i != numrooms-1)
		{
			hsRoom = gtk_hseparator_new ();
			gtk_widget_ref (hsRoom);
			gtk_widget_show (hsRoom);
			gtk_container_add (GTK_CONTAINER (vbRooms), hsRoom);
		}
		
		ggzcore_room_add_event_hook(room, GGZ_CHAT_EVENT, ggz_chat_event);
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
		ggzcore_room_add_event_hook(room, GGZ_PLAYER_LAG, ggz_list_players);
	}

	return GGZ_HOOK_OK;
}

static GGZHookReturn ggz_entered(GGZServerEvent id, void* event_data, void* user_data)
{
	gchar *message;

	/* Get player list */
	/* FIXME: Player list should use the ggz update system*/
	ggzcore_room_list_tables(ggzcore_server_get_cur_room(server), -1, 0);
	ggzcore_room_list_players(ggzcore_server_get_cur_room(server));


	/* Display a messgae in the chat widget */
	message = g_strdup_printf(_("You've joined room \"%s\"."), ggzcore_room_get_name(ggzcore_server_get_cur_room(server)));
	chat_display_message(CHAT_LOCAL_NORMAL, NULL, message);
	g_free(message);
	chat_display_message(CHAT_LOCAL_NORMAL, NULL,  ggzcore_room_get_desc(ggzcore_server_get_cur_room(server)));


	return GGZ_HOOK_OK;
}


static GGZHookReturn ggz_entered_fail(GGZServerEvent id, void* event_data, void* user_data)
{
	gchar* msg;

	msg = g_strdup_printf("Error joining room: %s", (char*)event_data);

	g_free(msg);
	
	return GGZ_HOOK_OK;
}


static GGZHookReturn ggz_logout(GGZServerEvent id, void* event_data, void* user_data)
{
	ggz_debug("connection", "Logged out.");

	server_disconnect();
	login_set_login(interface);
	
	return GGZ_HOOK_OK;
}


static GGZHookReturn ggz_chat_event(GGZRoomEvent id, void* event_data, void* user_data)
{
	GGZChatEventData *msg;
	gchar *message;

	msg = (GGZChatEventData*)event_data;
	switch(msg->type)
	{
		case GGZ_CHAT_TABLE:
		case GGZ_CHAT_NORMAL:
			chat_display_message(CHAT_MSG, (char*)msg->sender, (char*)msg->message);
			break;
		case GGZ_CHAT_PERSONAL:
			chat_display_message(CHAT_PRVMSG, (char*)msg->sender, (char*)msg->message);
			break;
		case GGZ_CHAT_BEEP:
			message = g_strdup_printf(_("You've been beeped by %s."), (char*)msg->sender);
			chat_display_message(CHAT_LOCAL_NORMAL, NULL, message);
			g_free(message);
			gnome_sound_init ("localhost");
			gnome_sound_play ("/usr/share/sounds/info.wav");
			gnome_sound_shutdown ();
			break;
		case GGZ_CHAT_ANNOUNCE:
			chat_display_message(CHAT_ANNOUNCE, (char*)msg->sender, (char*)msg->message);
			break;
		case GGZ_CHAT_UNKNOWN:
			break;
	}

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


static GGZHookReturn ggz_motd_loaded(GGZServerEvent id, void* event_data, void* user_data)
{
	gint i;
	gchar **motd;

	motd = event_data;

	for(i = 0; motd[i] != NULL; i++)
		chat_display_message(CHAT_ANNOUNCE, "***", motd[i]);

	return GGZ_HOOK_OK;
}


static GGZHookReturn ggz_state_change(GGZServerEvent id, void* event_data, void* user_data)
{
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

	return GGZ_HOOK_OK;
}


static GGZHookReturn ggz_state_sensitivity(GGZServerEvent id, void* event_data, void* user_data)
{
	GtkWidget *tmp;
	GGZStateID state_id;

	state_id = ggzcore_server_get_state(server);
	
	switch (state_id) {
	case GGZ_STATE_OFFLINE:
		tmp = lookup_widget(interface, "btnLogin");
		gtk_widget_set_sensitive(tmp, TRUE);
		tmp = lookup_widget(interface, "btnNew");
		gtk_widget_set_sensitive(tmp, TRUE);
		break;

	case GGZ_STATE_CONNECTING:
		tmp = lookup_widget(interface, "btnLogin");
		gtk_widget_set_sensitive(tmp, FALSE);
		tmp = lookup_widget(interface, "btnNew");
		gtk_widget_set_sensitive(tmp, FALSE);
	break;
		
	case GGZ_STATE_ONLINE:
		break;

	case GGZ_STATE_LOGGING_IN:
		tmp = lookup_widget(interface, "btnChatChange");
		gtk_widget_set_sensitive(tmp, FALSE);
		tmp = lookup_widget(interface, "btnChatGame");
		gtk_widget_set_sensitive(tmp, FALSE);
		break;

	case GGZ_STATE_LOGGED_IN:
		tmp = lookup_widget(interface, "btnChatChange");
		gtk_widget_set_sensitive(tmp, TRUE);
		tmp = lookup_widget(interface, "btnChatGame");
		gtk_widget_set_sensitive(tmp, TRUE);
		break;

	case GGZ_STATE_BETWEEN_ROOMS:
	case GGZ_STATE_ENTERING_ROOM:
		tmp = lookup_widget(interface, "btnChatChange");
		gtk_widget_set_sensitive(tmp, FALSE);
		tmp = lookup_widget(interface, "btnChatGame");
		gtk_widget_set_sensitive(tmp, FALSE);
		break;
	case GGZ_STATE_IN_ROOM:
		tmp = lookup_widget(interface, "btnChatChange");
		gtk_widget_set_sensitive(tmp, TRUE);
		tmp = lookup_widget(interface, "btnChatGame");
		gtk_widget_set_sensitive(tmp, TRUE);
		break;

	case GGZ_STATE_LAUNCHING_TABLE:
		break;

	case GGZ_STATE_JOINING_TABLE:
		break;
	case GGZ_STATE_AT_TABLE:
		break;
	case GGZ_STATE_LEAVING_TABLE:
		break;

	case GGZ_STATE_LOGGING_OUT:
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
	msg = g_strdup_printf("Server error: %s", (char*)event_data);
	msgbox_error(msg);
	g_free(msg);

	return GGZ_HOOK_OK;
}

static GGZHookReturn ggz_net_error(GGZServerEvent id, void* event_data, void* user_data)
{
	ggz_debug("connection", "Net error.");

	server_disconnect();

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
	return GGZ_HOOK_OK;
}


static GGZHookReturn ggz_table_launch_fail(GGZRoomEvent id, void* event_data, void* user_data)
{
	gchar* msg;

	msg = g_strdup_printf("Error launching table: %s", (char*)event_data);
//	msgbox(msg, "Error", MSGBOX_OKONLY, MSGBOX_STOP, MSGBOX_NORMAL);
	g_print("%s\n", msg);
	g_free(msg);

//	game_quit();

	return GGZ_HOOK_OK;
}


static GGZHookReturn ggz_table_joined(GGZRoomEvent id, void* event_data, void* user_data)
{

	return GGZ_HOOK_OK;
}


static GGZHookReturn ggz_table_join_fail(GGZRoomEvent id, void* event_data, void* user_data)
{
	gchar* msg;

	msg = g_strdup_printf("Error joining table: %s", (char*)event_data);
//	msgbox(msg, "Error", MSGBOX_OKONLY, MSGBOX_STOP, MSGBOX_NORMAL);
	g_print ("%s\n", msg);
	g_free(msg);

//	game_quit();

	return GGZ_HOOK_OK;
}


static GGZHookReturn ggz_table_left(GGZRoomEvent id, void* event_data, void* user_data)
{
	/*output_text("-- Left table");*/

//	game_quit();

	return GGZ_HOOK_OK;
}


static GGZHookReturn ggz_table_leave_fail(GGZRoomEvent id, void* event_data, void* user_data)
{
	gchar* msg;

	msg = g_strdup_printf("Error leaving table: %s", (char*)event_data);
//	msgbox(msg, "Error", MSGBOX_OKONLY, MSGBOX_STOP, MSGBOX_NORMAL);
	g_free(msg);

	return GGZ_HOOK_OK;
}
	


/* The below function are used to
 * add and remove servers to the 
 * main loop
 */

GdkInputFunction ggz_check_fd(gpointer server, gint fd, GdkInputCondition cond)
{
	ggzcore_server_read_data(server, fd);
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
	GtkListStore *stoTables;
	GtkTreeIter iter;
	gint i, num;
	GGZTable *table;
	GGZRoom *room = ggzcore_server_get_cur_room(server);
	
	/* Clear current list of tables */
        tmp = lookup_widget(interface, "treTables");
	stoTables = GTK_LIST_STORE(gtk_tree_view_get_model (GTK_TREE_VIEW (tmp)));
	gtk_list_store_clear (GTK_LIST_STORE (stoTables));
			
	/* Display current list of tables */
	num = ggzcore_room_get_num_tables(room);

	for (i = 0; i < num; i++) {
		table = ggzcore_room_get_nth_table(room, i);
		gtk_list_store_append (stoTables, &iter);
		gtk_list_store_set (stoTables, &iter,
				0, ggzcore_table_get_id(table),
				1, ggzcore_table_get_seat_count(table, GGZ_SEAT_OPEN)+
				   ggzcore_table_get_seat_count(table, GGZ_SEAT_RESERVED),
				2, ggzcore_table_get_num_seats(table),
				3, ggzcore_table_get_desc(table),
				-1);
	}
}


void display_players(void)
{
	GtkWidget *tmp;
	GtkListStore *stoPlayers;
	GtkTreeIter iter;
	gint i, num;
	GGZPlayer *p;
	GGZTable *table;
	GGZRoom *room = ggzcore_server_get_cur_room(server);
	gchar *strTable;
	
	/* Clear current list of players */
        tmp = lookup_widget(interface, "trePlayers");
	stoPlayers = GTK_LIST_STORE(gtk_tree_view_get_model (GTK_TREE_VIEW (tmp)));
	gtk_list_store_clear (GTK_LIST_STORE (stoPlayers));
			
	/* Display current list of players */
	num = ggzcore_room_get_num_players(room);

	for (i = 0; i < num; i++) {
		p = ggzcore_room_get_nth_player(room, i);

		table = ggzcore_player_get_table(p);
		
		if(!table)
			strTable = g_strdup("--");
		else
			strTable = g_strdup_printf("%d", ggzcore_table_get_id(table));
		gtk_list_store_append (stoPlayers, &iter);
		gtk_list_store_set (stoPlayers, &iter,
				0, ggzcore_player_get_lag(p),
				1, strTable,
				2, ggzcore_player_get_name(p),
				-1);
		g_free(strTable);	
	}
}


static GGZHookReturn ggz_auto_join(GGZServerEvent id, void* event_data, void* user_data)
{
	ggzcore_server_join_room(server, 0);
	
	return GGZ_HOOK_REMOVE;
}


int ggz_connection_query(void)
{
	if(server_handle < 0)
		return 0;
	return 1;
}

static void server_disconnect(void)
{
	gdk_input_remove(server_handle);
	server_handle = -1;
}
