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
#include "chat.h"
#include "msgbox.h"
#include "support.h"

#include <gtk/gtk.h>
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


int game_init(void)
{
	gchar *message;
	gchar *name;
	gchar *protocol;
	gint num;
	GGZRoom *room;
	GGZGameType *gt;
	GGZModule *module;

	/* Make sure we aren't already in a game */
	if (game) {
		msgbox(_("You can only play one game at a time."),
		       _("Game Error"), MSGBOX_OKONLY, MSGBOX_INFO, MSGBOX_NORMAL);
		return -1;
	}

	/* Make sure we're in a room */
	room = ggzcore_server_get_cur_room(server);
	if (!room) {
		msgbox(_("You must be in a room to launch a game.\nLaunch aborted"), _("Launch Error"), MSGBOX_OKONLY, MSGBOX_STOP, MSGBOX_NORMAL);
		return -1;
	}

	/* Get game type for this room */
	gt = ggzcore_room_get_gametype(room);
	if (!gt) {
		msgbox(_("No game types defined for this server.\nLaunch aborted."), _("Launch Error"), MSGBOX_OKONLY, MSGBOX_STOP, MSGBOX_NORMAL);
		return -1;
	}

	name = ggzcore_gametype_get_name(gt);
	protocol = ggzcore_gametype_get_protocol(gt);

	/* Check how many modules are registered for this game type */
	num = ggzcore_module_get_num_by_type(name, protocol);
	if (num == 0) {
		message = g_strdup_printf(_("You don't have this game installed. You can download\nit from %s."), ggzcore_gametype_get_url(gt));
	
		msgbox(message, _("Launch Error"), MSGBOX_OKONLY, MSGBOX_STOP, 
		       MSGBOX_NORMAL);
		       
		g_free(message);
		return -1;
	}

	/* FIXME: if num > 1, popup a dialog and let the user choose */
	module = ggzcore_module_get_nth_by_type(name, protocol, 0);
	if (!module) {
		msgbox(_("No modules defined for this game.\nLaunch aborted."), _("Launch Error"), MSGBOX_OKONLY, MSGBOX_STOP, MSGBOX_NORMAL);
		return -1;
	}
		
	/* Create new game using this module */
	game = ggzcore_game_new();
	ggzcore_game_init(game, module);

	/* Register callbacks */
	game_register(game);

	return 0;
}


int game_launch(void)
{
	gint status;
	gchar *message;

	/* Launch game */
	status = ggzcore_game_launch(game);
	if (status < 0) {
		msgbox(_("Failed to execute game module.\n Launch aborted."), _("Launch Error"), MSGBOX_OKONLY, MSGBOX_STOP, MSGBOX_NORMAL);
		game_destroy();
		return -1;
	}

	message = g_strdup_printf("Launching Game");
	chat_display_message(CHAT_BEEP, "---", message);
	g_free(message);

	return 0;
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

