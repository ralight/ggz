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
#include "xtext.h"

extern GtkWidget *dlg_login;
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
void ggz_state(void);


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
	GtkCList *tmp;
	gint i;
	gchar **names;
	gchar *player[4];

	/* Clear current list of players */
	tmp = gtk_object_get_data(GTK_OBJECT(win_main), "player_clist");
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
	GtkCList *tmp;
	gint i;
	gchar **names;

	/* Clear current list of rooms */
	tmp = gtk_object_get_data(GTK_OBJECT(win_main), "room_clist");
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
	GtkLabel *tmp;
	gchar *name;

	/* Get player list */
	/* FIXME: Player list should use the ggz update system*/
	ggzcore_event_trigger(GGZ_USER_LIST_PLAYERS, NULL, NULL);

	/* Set the room label to current room */
	tmp = gtk_object_get_data(GTK_OBJECT(win_main), "Current_room_label");
	name = ggzcore_room_get_name(ggzcore_state_get_room());
	gtk_label_set_text(GTK_LABEL(tmp), name);
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
	/* Close the login dialog */
	if(dlg_login)
		gtk_widget_destroy(dlg_login);

	/* Get list of rooms */
	ggzcore_event_trigger(GGZ_USER_LIST_ROOMS, NULL, NULL);
}

void ggz_connect_fail(GGZEventID id, void* event_data, void* user_data)
{

}

void ggz_login_fail(GGZEventID id, void* event_data, void* user_data)
{
	GtkWidget *tmp;

	tmp = gtk_object_get_data(GTK_OBJECT(dlg_login), "connect_button");
	gtk_label_set_text(GTK_LABEL(GTK_BIN(tmp)->child),"Login");
	gtk_widget_set_sensitive(GTK_WIDGET(tmp),TRUE);

	tmp = gtk_object_get_data(GTK_OBJECT(dlg_login), "top_panel");
	gtk_notebook_set_page(GTK_NOTEBOOK(tmp), 1);

	tmp = gtk_object_get_data(GTK_OBJECT(dlg_login), "profile_frame");
	gtk_frame_set_label(GTK_FRAME(tmp), "Sorry!");

	tmp = gtk_object_get_data(GTK_OBJECT(dlg_login), "msg_label");
	gtk_label_set_text(GTK_LABEL(tmp),
		"That username is already in usage,\nor not permitted on this server.\n\nPlease choose a different name");
}


/* State function 
 *
 * This is the only place we should ever have 
 * manipulate the focus and sensitivity of 
 * widgets
 */
void ggz_state(void)
{
	int state;
	static int oldstate = GGZ_STATE_OFFLINE;

	state = (int)ggzcore_state_get_id();
	if (state != oldstate)
	{
		/* State Change */
		oldstate = state;


		if (state != GGZ_STATE_OFFLINE)
		{
		} else {
		}	
	}
}
