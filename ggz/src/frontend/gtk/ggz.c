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

void ggz_chat_msg(GGZEventID id, void* event_data, void* user_data);
void ggz_chat_prvmsg(GGZEventID id, void* event_data, void* user_data);
void ggz_chat_beep(GGZEventID id, void* event_data, void* user_data);
void ggz_list_rooms(GGZEventID id, void* event_data, void* user_data);
void ggz_list_players(GGZEventID id, void* event_data, void* user_data);
void ggz_room_join(GGZEventID id, void* event_data, void* user_data);
void ggz_room_enter(GGZEventID id, void* event_data, void* user_data);
void ggz_room_part(GGZEventID id, void* event_data, void* user_data);
void ggz_login_ok(GGZEventID id, void* event_data, void* user_data);
void ggz_login_fail(GGZEventID id, void* event_data, void* user_data);
void ggz_connect_fail(GGZEventID id, void* event_data, void* user_data);
void ggz_motd(GGZEventID id, void* event_data, void* user_data);


void ggz_event_init(void)
{
	ggzcore_event_connect(GGZ_SERVER_CHAT_MSG, ggz_chat_msg);
	ggzcore_event_connect(GGZ_SERVER_CHAT_PRVMSG, ggz_chat_prvmsg);
	ggzcore_event_connect(GGZ_SERVER_CHAT_BEEP, ggz_chat_beep);
	ggzcore_event_connect(GGZ_SERVER_LOGIN, ggz_login_ok);
	ggzcore_event_connect(GGZ_SERVER_CONNECT_FAIL, ggz_connect_fail);
	ggzcore_event_connect(GGZ_SERVER_ROOM_JOIN, ggz_room_join);
	ggzcore_event_connect(GGZ_SERVER_ROOM_ENTER, ggz_room_enter);
	ggzcore_event_connect(GGZ_SERVER_ROOM_LEAVE, ggz_room_part);
	ggzcore_event_connect(GGZ_SERVER_LOGIN_FAIL, ggz_login_fail);
	ggzcore_event_connect(GGZ_SERVER_LIST_ROOMS, ggz_list_rooms);
	ggzcore_event_connect(GGZ_SERVER_LIST_PLAYERS, ggz_list_players);
	ggzcore_event_connect(GGZ_SERVER_MOTD, ggz_motd);
}


/* Event functions */
void ggz_chat_msg(GGZEventID id, void* event_data, void* user_data)
{
	gchar *player;
	gchar *message;

	player = ((char**)(event_data))[0];
	message = ((char**)(event_data))[1];

	chat_display_message(CHAT_MSG, player, message);
}

void ggz_chat_prvmsg(GGZEventID id, void* event_data, void* user_data)
{
	gchar *message;
	gchar *player;

	player = ((char**)(event_data))[0];
	message = ((char**)(event_data))[1];
	chat_display_message(CHAT_PRVMSG, player, message);

}

void ggz_chat_beep(GGZEventID id, void* event_data, void* user_data)
{
	gchar *message;
	gchar *player;

	player = ((char**)(event_data))[0];
	message = g_strdup_printf("You've been beeped by %s.", player);
	chat_display_message(CHAT_BEEP, "---", message);
	gdk_beep();

	g_free(message);
}

void ggz_list_players(GGZEventID id, void* event_data, void* user_data)
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
		return;

	for (i = 0; names[i]; i++)
	{
		player[0] = g_strdup(" ");
		player[1] = g_strdup(" ");
		player[2] = g_strdup(names[i]);
		gtk_clist_append(GTK_CLIST(tmp), player);
		g_free(player[0]);
		g_free(player[1]);
		g_free(player[2]);
	}
	free(names);
}

void ggz_list_rooms(GGZEventID id, void* event_data, void* user_data)
{
	GtkWidget *tmp;
	gint i;
	gchar **names;

	/* Clear current list of rooms */
	tmp = lookup_widget(win_main, "room_clist");
	gtk_clist_clear(GTK_CLIST(tmp));

	/* Display current list of rooms */
	if (!(names = ggzcore_room_get_names()))
		return;

	for (i = 0; names[i]; i++)
		gtk_clist_insert(GTK_CLIST(tmp), i, &names[i]);

	free(names);
}

void ggz_room_join(GGZEventID id, void* event_data, void* user_data)
{
	GtkWidget *tmp;
	gchar *name;

	/* Get player list */
	/* FIXME: Player list should use the ggz update system*/
	ggzcore_event_trigger(GGZ_USER_LIST_PLAYERS, NULL, NULL);

	/* Set the room label to current room */
	tmp = lookup_widget(win_main, "Current_room_label");
	name = g_strdup_printf("Current Room: %s", ggzcore_room_get_name(ggzcore_state_get_room()));
	gtk_label_set_text(GTK_LABEL(tmp), name);
	g_free(name);

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

}

void ggz_room_enter(GGZEventID id, void* event_data, void* user_data)
{
	gchar *player;

	player = g_strdup(event_data);
	chat_enter(event_data);
	g_free(player);

	/* Get player list */
	/* FIXME: Player list should use the ggz update system*/
	ggzcore_event_trigger(GGZ_USER_LIST_PLAYERS, NULL, NULL);
}

void ggz_room_part(GGZEventID id, void* event_data, void* user_data)
{
	gchar *player;

	player = g_strdup(event_data);
	chat_part(event_data);
	g_free(player);

	/* Get player list */
	/* FIXME: Player list should use the ggz update system*/
	ggzcore_event_trigger(GGZ_USER_LIST_PLAYERS, NULL, NULL);
}

void ggz_login_ok(GGZEventID id, void* event_data, void* user_data)
{
	GtkWidget *tmp;

	/* Close the login dialog */
	login_destroy();

	/* Get list of rooms */
	ggzcore_event_trigger(GGZ_USER_LIST_ROOMS, NULL, NULL);

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

}

void ggz_connect_fail(GGZEventID id, void* event_data, void* user_data)
{

}

void ggz_login_fail(GGZEventID id, void* event_data, void* user_data)
{
	GtkWidget *tmp;

	tmp = lookup_widget(login_dialog, "connect_button");
	gtk_label_set_text(GTK_LABEL(GTK_BIN(tmp)->child),"Login");
	gtk_widget_set_sensitive(tmp, TRUE);

	tmp = lookup_widget(login_dialog, "top_panel");
	gtk_notebook_set_page(GTK_NOTEBOOK(tmp), 1);

	tmp = lookup_widget(login_dialog, "profile_frame");
	gtk_frame_set_label(GTK_FRAME(tmp), "Sorry!");

	tmp = lookup_widget(login_dialog, "msg_label");
	gtk_label_set_text(GTK_LABEL(tmp),
		"That username is already in usage,\nor not permitted on this server.\n\nPlease choose a different name");
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

void ggz_motd(GGZEventID id, void* event_data, void* user_data)
{
	gint i;
	gchar **motd;

	motd = event_data;

	motd_create_or_raise();
	for(i = 0; motd[i] != NULL; i++)
		motd_print_line(motd[i]);
}
