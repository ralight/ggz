/*
 * File: game.c
 * Author: Brent Hendricks
 * Project: GGZ Text Client 
 * Date: 3/1/01
 *
 * Functions for handling game events
 *
 * Copyright (C) 2000 Brent Hendricks.
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


#include "game.h"

#include <gtk/gtk.h>
#include <chat.h>
#include <ggzcore.h>
#include <stdlib.h>

/* Hooks for game events */
static void game_register(GGZGame *game);
static void game_process(void);
static GGZHookReturn game_launched(GGZGameEvent, void*, void*);
static GGZHookReturn game_launch_fail(GGZGameEvent, void*, void*);
static GGZHookReturn game_negotiated(GGZGameEvent, void*, void*);
static GGZHookReturn game_negotiate_fail(GGZGameEvent, void*, void*);
static GGZHookReturn game_data(GGZGameEvent, void *, void*);
static GGZHookReturn game_over(GGZGameEvent, void *, void*);
static void game_input_removed(gpointer data);

GGZGame *game;
static int fd = -1;
static gint game_handle;

extern GGZServer *server;
extern GtkWidget *win_main;

void game_init(GGZModule *module)
{
	game = ggzcore_game_new();
	ggzcore_game_init(game, module);
	game_register(game);
	ggzcore_game_launch(game);
}


void game_quit(void)
{
	if (fd != -1)
	{
	        gdk_input_remove(game_handle);
        	game_handle = -1;
		fd = -1;
	}
}


void game_destroy(void)
{
	if (game)
		ggzcore_game_free(game);
	game = NULL;
}


static void game_process(void)
{
	if (game)
		ggzcore_game_read_data(game);
}


static void game_register(GGZGame *game)
{
	ggzcore_game_add_event_hook(game, GGZ_GAME_LAUNCHED, game_launched);
	ggzcore_game_add_event_hook(game, GGZ_GAME_LAUNCH_FAIL, game_launch_fail);
	ggzcore_game_add_event_hook(game, GGZ_GAME_NEGOTIATED, game_negotiated);
	ggzcore_game_add_event_hook(game, GGZ_GAME_NEGOTIATE_FAIL, game_negotiate_fail);
	ggzcore_game_add_event_hook(game, GGZ_GAME_DATA, game_data);
	ggzcore_game_add_event_hook(game, GGZ_GAME_OVER, game_over);
}


static GGZHookReturn game_launched(GGZGameEvent id, void* event_data, 
				   void* user_data)
{
	GtkWidget *tmp;

	chat_display_message(CHAT_BEEP, "---", "Launched game");
	tmp = gtk_object_get_data(GTK_OBJECT(win_main), "leave_button");
	gtk_widget_set_sensitive(tmp, TRUE);
	
	fd = ggzcore_game_get_fd(game);
        game_handle = gdk_input_add_full(fd, GDK_INPUT_READ,
					 (GdkInputFunction)game_process,
					 (gpointer)server,
					 game_input_removed);

	return GGZ_HOOK_OK;
}


static GGZHookReturn game_launch_fail(GGZGameEvent id, void* event_data,
				      void* user_data)
{
	GtkWidget *tmp;

	chat_display_message(CHAT_BEEP, "---", "Launched faild");
	tmp = gtk_object_get_data(GTK_OBJECT(win_main), "leave_button");
	gtk_widget_set_sensitive(tmp, TRUE);

	return GGZ_HOOK_OK;
}


static GGZHookReturn game_negotiated(GGZGameEvent id, void* event_data, 
				     void* user_data)
{
	return GGZ_HOOK_OK;
}


static GGZHookReturn game_negotiate_fail(GGZGameEvent id, void* event_data,
				      void* user_data)
{
	return GGZ_HOOK_OK;
}


static GGZHookReturn game_data(GGZGameEvent id, void* event_data, void* user_data)
{
	GGZRoom *room;

	room = ggzcore_server_get_cur_room(server);
	ggzcore_room_send_game_data(room, event_data);

	return GGZ_HOOK_OK;
}


static GGZHookReturn game_over(GGZGameEvent id, void* event_data, void* user_data)
{
	GtkWidget *tmp;
	GGZRoom *room;

	chat_display_message(CHAT_BEEP, "---", "Game Over");
	tmp = gtk_object_get_data(GTK_OBJECT(win_main), "leave_button");
	gtk_widget_set_sensitive(tmp, FALSE);

	game_quit();
	room = ggzcore_server_get_cur_room(server);
	ggzcore_room_leave_table(room);

	return GGZ_HOOK_OK;
}


/* GdkDestroyNotify function for server fd */
static void game_input_removed(gpointer data)
{
	game_destroy();
}


int game_play(void)
{
	if(fd != -1)
		return TRUE;
	return FALSE;
}

