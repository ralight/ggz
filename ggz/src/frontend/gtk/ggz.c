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

#include "chat.h"
#include "ggz.h"
#include "login.h"
#include "motd.h"
#include "support.h"
#include "xtext.h"

extern GtkWidget *login_dialog;
extern GtkWidget *win_main;

static GGZHookReturn ggz_connect_fail(GGZEventID id, void* event_data, void* user_data);
static GGZHookReturn ggz_motd(GGZEventID id, void* event_data, void* user_data);
static GGZHookReturn ggz_login_ok(GGZEventID id, void* event_data, void* user_data);
static GGZHookReturn ggz_login_fail(GGZEventID id, void* event_data, void* user_data);
static GGZHookReturn ggz_list_rooms(GGZEventID id, void* event_data, void* user_data);
static GGZHookReturn ggz_room_join(GGZEventID id, void* event_data, void* user_data);
static GGZHookReturn ggz_list_players(GGZEventID id, void* event_data, void* user_data);
static GGZHookReturn ggz_chat_msg(GGZEventID id, void* event_data, void* user_data);
static GGZHookReturn ggz_chat_prvmsg(GGZEventID id, void* event_data, void* user_data);
static GGZHookReturn ggz_chat_beep(GGZEventID id, void* event_data, void* user_data);
static GGZHookReturn ggz_logout(GGZEventID id, void* event_data, void* user_data);
static GGZHookReturn ggz_room_enter(GGZEventID id, void* event_data, void* user_data);
static GGZHookReturn ggz_room_part(GGZEventID id, void* event_data, void* user_data);
static GGZHookReturn ggz_table_update(GGZEventID id, void* event_data, void* user_data);
static GGZHookReturn ggz_state_change(GGZStateID id, void* event_data, void* user_data);

void ggz_event_init(void)
{
	ggzcore_event_add_hook(GGZ_SERVER_CONNECT_FAIL,	ggz_connect_fail);
	ggzcore_event_add_hook(GGZ_SERVER_MOTD,		ggz_motd);
	ggzcore_event_add_hook(GGZ_SERVER_LOGIN,	ggz_login_ok);
	ggzcore_event_add_hook(GGZ_SERVER_LOGIN_FAIL,	ggz_login_fail);
	ggzcore_event_add_hook(GGZ_SERVER_LIST_ROOMS,	ggz_list_rooms);
	ggzcore_event_add_hook(GGZ_SERVER_ROOM_JOIN,	ggz_room_join);
	ggzcore_event_add_hook(GGZ_SERVER_LIST_PLAYERS,	ggz_list_players);
	ggzcore_event_add_hook(GGZ_SERVER_CHAT_MSG,	ggz_chat_msg);
	ggzcore_event_add_hook(GGZ_SERVER_CHAT_PRVMSG,	ggz_chat_prvmsg);
	ggzcore_event_add_hook(GGZ_SERVER_CHAT_BEEP,	ggz_chat_beep);
	ggzcore_event_add_hook(GGZ_SERVER_LOGOUT,	ggz_logout);
	ggzcore_event_add_hook(GGZ_SERVER_ERROR,	NULL);
	ggzcore_event_add_hook(GGZ_SERVER_ROOM_ENTER,	ggz_room_enter);
	ggzcore_event_add_hook(GGZ_SERVER_ROOM_LEAVE,	ggz_room_part);
	ggzcore_event_add_hook(GGZ_SERVER_TABLE_UPDATE,	ggz_table_update);
	ggzcore_event_add_hook(GGZ_NET_ERROR,		NULL);
}


void ggz_state_init(void)
{
	ggzcore_state_add_hook(GGZ_STATE_OFFLINE, ggz_state_change);
	ggzcore_state_add_hook(GGZ_STATE_CONNECTING, ggz_state_change);
	ggzcore_state_add_hook(GGZ_STATE_ONLINE, ggz_state_change);
	ggzcore_state_add_hook(GGZ_STATE_LOGGING_IN, ggz_state_change);
	ggzcore_state_add_hook(GGZ_STATE_LOGGED_IN, ggz_state_change);
	ggzcore_state_add_hook(GGZ_STATE_BETWEEN_ROOMS, ggz_state_change);
	ggzcore_state_add_hook(GGZ_STATE_ENTERING_ROOM, ggz_state_change);
	ggzcore_state_add_hook(GGZ_STATE_IN_ROOM, ggz_state_change);
	ggzcore_state_add_hook(GGZ_STATE_JOINING_TABLE, ggz_state_change);
	ggzcore_state_add_hook(GGZ_STATE_AT_TABLE, ggz_state_change);
	ggzcore_state_add_hook(GGZ_STATE_LEAVING_TABLE, ggz_state_change);
	ggzcore_state_add_hook(GGZ_STATE_LOGGING_OUT, ggz_state_change);
}


static GGZHookReturn ggz_chat_msg(GGZEventID id, void* event_data, void* user_data)
{
	gchar *player;
	gchar *message;

	player = ((char**)(event_data))[0];
	message = ((char**)(event_data))[1];

	chat_display_message(CHAT_MSG, player, message);

	return GGZ_HOOK_OK;
}

static GGZHookReturn ggz_chat_prvmsg(GGZEventID id, void* event_data, void* user_data)
{
	gchar *message;
	gchar *player;

	player = ((char**)(event_data))[0];
	message = ((char**)(event_data))[1];
	chat_display_message(CHAT_PRVMSG, player, message);

	return GGZ_HOOK_OK;
}

static GGZHookReturn ggz_chat_beep(GGZEventID id, void* event_data, void* user_data)
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

static GGZHookReturn ggz_logout(GGZEventID id, void* event_data, void* user_data)
{
	chat_display_message(CHAT_BEEP, "---", _("Disconnected from Server."));

	return GGZ_HOOK_OK;
}


static GGZHookReturn ggz_list_players(GGZEventID id, void* event_data, void* user_data)
{
	GtkWidget *tmp;
	gint i;
	gchar **names;
	gchar *player[4];

	/* Clear current list of players */
	tmp = lookup_widget(win_main, "player_clist");
	gtk_clist_clear(GTK_CLIST(tmp));

	/* Display current list of players */
	if (!(names = ggzcore_player_get_names()))
		return GGZ_HOOK_OK;

	for (i = 0; names[i]; i++)
	{
		player[0] = g_strdup(" ");
		if(ggzcore_player_get_table(names[i]) == -1)
			player[1] = g_strdup("--");
		else if(ggzcore_player_get_table(names[i]) == -2)
			player[1] = g_strdup("??");
		else
			player[1] = g_strdup_printf("%d", ggzcore_player_get_table(names[i]));
		player[2] = g_strdup(names[i]);
		gtk_clist_append(GTK_CLIST(tmp), player);
		g_free(player[0]);
		g_free(player[1]);
		g_free(player[2]);
	}
	free(names);

	return GGZ_HOOK_OK;
}

static GGZHookReturn ggz_list_rooms(GGZEventID id, void* event_data, void* user_data)
{
	GtkWidget *tmp;
	gint i;
	gchar **names;

	/* Clear current list of rooms */
	tmp = lookup_widget(win_main, "room_clist");
	gtk_clist_clear(GTK_CLIST(tmp));

	/* Display current list of rooms */
	if (!(names = ggzcore_room_get_names()))
		return GGZ_HOOK_OK;

	for (i = 0; names[i]; i++)
		gtk_clist_insert(GTK_CLIST(tmp), i, &names[i]);

	free(names);

	return GGZ_HOOK_OK;
}

static GGZHookReturn ggz_room_join(GGZEventID id, void* event_data, void* user_data)
{
	GtkWidget *tmp;
	gchar *name;
	gchar *message;

	/* Get player list */
	/* FIXME: Player list should use the ggz update system*/
	ggzcore_event_enqueue(GGZ_USER_LIST_PLAYERS, NULL, NULL);

	/* Set the room label to current room */
	tmp = lookup_widget(win_main, "Current_room_label");
	name = g_strdup_printf(_("Current Room: %s"), ggzcore_room_get_name(ggzcore_state_get_room()));
	gtk_label_set_text(GTK_LABEL(tmp), name);
	g_free(name);

	/* Display message in chat area */
	message = g_strdup_printf(_("You've joined room \"%s\"."), ggzcore_room_get_name(ggzcore_state_get_room()));
	chat_display_message(CHAT_BEEP, "---", message);
	g_free(message);
	chat_display_message(CHAT_BEEP, "---",  ggzcore_room_get_desc(ggzcore_state_get_room()));

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

static GGZHookReturn ggz_room_enter(GGZEventID id, void* event_data, void* user_data)
{
	gchar *player;

	player = g_strdup(event_data);
	chat_enter(event_data);
	g_free(player);

	/* Get player list */
	/* FIXME: Player list should use the ggz update system*/
	ggzcore_event_enqueue(GGZ_USER_LIST_PLAYERS, NULL, NULL);

	return GGZ_HOOK_OK;
}

static GGZHookReturn ggz_room_part(GGZEventID id, void* event_data, void* user_data)
{
	gchar *player;

	player = g_strdup(event_data);
	chat_part(event_data);
	g_free(player);

	/* Get player list */
	/* FIXME: Player list should use the ggz update system*/
	ggzcore_event_enqueue(GGZ_USER_LIST_PLAYERS, NULL, NULL);

	return GGZ_HOOK_OK;
}

static GGZHookReturn ggz_login_ok(GGZEventID id, void* event_data, void* user_data)
{
	GtkWidget *tmp;

	/* Close the login dialog */
	login_destroy();

	/* Get list of rooms */
	ggzcore_event_enqueue(GGZ_USER_LIST_ROOMS, NULL, NULL);

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

	return GGZ_HOOK_OK;
}

static GGZHookReturn ggz_connect_fail(GGZEventID id, void* event_data, void* user_data)
{
	GtkWidget *tmp;

	tmp = gtk_object_get_data(GTK_OBJECT(login_dialog), "connect_button");
	gtk_widget_set_sensitive(tmp, TRUE);

	return GGZ_HOOK_OK;
}

static GGZHookReturn ggz_login_fail(GGZEventID id, void* event_data, void* user_data)
{
	login_failed();

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

static GGZHookReturn ggz_motd(GGZEventID id, void* event_data, void* user_data)
{
	gint i;
	gchar **motd;

	motd = event_data;

	motd_create_or_raise();
	for(i = 0; motd[i] != NULL; i++)
		motd_print_line(motd[i]);

	return GGZ_HOOK_OK;
}


static GGZHookReturn ggz_state_change(GGZStateID id, void* event_data, void* user_data)
{
	GtkWidget* statebar;
	char *state;

	switch (id) {
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
	id = gtk_statusbar_get_context_id(GTK_STATUSBAR(statebar), "state");
	gtk_statusbar_pop(GTK_STATUSBAR(statebar), id);
	gtk_statusbar_push(GTK_STATUSBAR(statebar), id, state);


	return GGZ_HOOK_OK;
}

static GGZHookReturn ggz_table_update(GGZEventID id, void* event_data, void* user_data)
{
	GtkWidget *tmp;
	gchar *table[4];
	gint *numbers;
	gint i;

	/* clear the list */
	tmp = lookup_widget(win_main, "table_clist");
	gtk_clist_clear(GTK_CLIST(tmp));


	/* Display current list of players */
	if (!(numbers = ggzcore_table_get_numbers()))
		return GGZ_HOOK_OK;


	for (i = 0; i < ggzcore_table_get_num(); i++)
	{
		table[0] = g_strdup_printf("%d", numbers[i]);
		table[1] = g_strdup_printf("%d/%d", ggzcore_table_get_open(numbers[i]),
						    ggzcore_table_get_seats(numbers[i]));
		table[2] = g_strdup_printf("%s", ggzcore_table_get_desc(numbers[i]));
		gtk_clist_append(GTK_CLIST(tmp), table);
		g_free(table[0]);
		g_free(table[1]);
		g_free(table[2]);
	}
	free(numbers);

	return GGZ_HOOK_OK;
}
