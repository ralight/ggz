/*
 * File: game.c
 * Author: Brent Hendricks
 * Project: GGZ Text Client 
 * Date: 3/1/01
 * $Id: game.c 3413 2002-02-18 12:42:40Z jdorje $
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

#include <ggz.h>		/* libggz */

#include <gtk/gtk.h>
#include <ggzcore.h>
#include <stdlib.h>
#include <string.h>

/* Hooks for game events */
static void game_register(GGZGame *game);
static void game_process(void);
static GGZHookReturn game_launched(GGZGameEvent, void*, void*);
static GGZHookReturn game_launch_fail(GGZGameEvent, void*, void*);
static GGZHookReturn game_negotiated(GGZGameEvent, void*, void*);
static GGZHookReturn game_negotiate_fail(GGZGameEvent, void*, void*);
static GGZHookReturn game_data(GGZGameEvent, void *, void*);
static GGZHookReturn game_over(GGZGameEvent, void *, void*);
static GGZHookReturn game_delayed_leave(GGZServerEvent, void *, void *);
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
	gchar *engine, *version;
	gchar *frontend;
	guint num, i;
	GGZRoom *room;
	GGZGameType *gt;
	GGZModule *module = NULL;
	

	/* Make sure we aren't already in a game */
	if (game) {
		msgbox(_("You can only play one game at a time."),
		       _("Game Error"), MSGBOX_OKONLY, MSGBOX_INFO, MSGBOX_NORMAL);
		return -1;
	}
	
	/* Make sure we're actually in a room and not already at a table */
	if (ggzcore_server_get_state(server) != GGZ_STATE_IN_ROOM) {
		msgbox(_("You're still at a table."),
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
	engine = ggzcore_gametype_get_prot_engine(gt);
	version = ggzcore_gametype_get_prot_version(gt);

	/* Check how many modules are registered for this game type */
	num = ggzcore_module_get_num_by_type(name, engine, version);
	if (num == 0) {
		message = g_strdup_printf(_("You don't have this game installed. You can download\nit from %s."), ggzcore_gametype_get_url(gt));
	
		msgbox(message, _("Launch Error"), MSGBOX_OKONLY, MSGBOX_STOP, 
		       MSGBOX_NORMAL);
		       
		g_free(message);
		return -1;
	}

	/* If there's only one choice, use it regardless of frontend */
	if (num == 1)
		module = ggzcore_module_get_nth_by_type(name, engine, version, 0);


	/* FIXME: if num > 1, popup a dialog and let the user choose */
	if (num > 1) {
		ggz_debug("modules", "%s v %s %s had %d modules\n",
		          name, engine, version, num);
		for (i = 0; i < num; i++) {
			module = ggzcore_module_get_nth_by_type(name, engine, version, i);
			frontend = ggzcore_module_get_frontend(module);
			ggz_debug("modules", "Module %d by %s frontend %s..",
			          i, ggzcore_module_get_author(module),
			          frontend);

			if (strcmp("gtk", frontend) == 0) {
				g_print("match\n");
				break;
			}
			else {
				g_print("not ours\n");
				module = NULL;
			}
		}
	}
			

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
	/*gchar *message;*/

	/* Launch game */
	status = ggzcore_game_launch(game);
	if (status < 0) {
		msgbox(_("Failed to execute game module.\n Launch aborted."), _("Launch Error"), MSGBOX_OKONLY, MSGBOX_STOP, MSGBOX_NORMAL);
		game_destroy();
		return -1;
	}

	chat_display_message(CHAT_LOCAL_NORMAL, NULL, _("Launching game"));

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
	chat_display_message(CHAT_LOCAL_NORMAL, NULL, _("Launched game"));
	
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
	chat_display_message(CHAT_LOCAL_HIGH, NULL, _("Launch failed"));

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
	GGZRoom *room;

	chat_display_message(CHAT_LOCAL_NORMAL, NULL, _("Game Over"));

	game_quit();
	room = ggzcore_server_get_cur_room(server);

	/* If we haven't actually made it to the table yet, register a 
	   callback for when we do so that we can leave :) */
	if (ggzcore_server_get_state(server) == GGZ_STATE_LAUNCHING_TABLE ||
	    ggzcore_server_get_state(server) == GGZ_STATE_JOINING_TABLE)
		ggzcore_server_add_event_hook(server, GGZ_STATE_CHANGE, 
					      game_delayed_leave);
	else if (ggzcore_room_leave_table(room) < 0)
		msgbox(_("Error leaving table"),
		       _("Game Error"), MSGBOX_OKONLY, MSGBOX_INFO, MSGBOX_NORMAL);

	return GGZ_HOOK_OK;
}


static GGZHookReturn game_delayed_leave(GGZServerEvent event, void *event_data, void *user_data)
{
	GGZStateID state_id;
	GGZRoom *room;

	state_id = ggzcore_server_get_state(server);
	
	switch (state_id) {
	case GGZ_STATE_AT_TABLE:
		/* We finally made it to the table, so leave it */
		room = ggzcore_server_get_cur_room(server);
		if (ggzcore_room_leave_table(room) < 0)
			msgbox(_("Error leaving table"), _("Game Error"), 
			       MSGBOX_OKONLY, MSGBOX_INFO, MSGBOX_NORMAL);
		return GGZ_HOOK_REMOVE;
		break;
		
	case GGZ_STATE_JOINING_TABLE:
	case GGZ_STATE_LAUNCHING_TABLE:
	case GGZ_STATE_LEAVING_TABLE:
		/* Either We're not there yet, so hang on...  or we're
		   in the cllback a second time because of the above
		   ggzcore_room_leave_table() command (yuck!!) */
		return GGZ_HOOK_OK;
		break;
	default:
		/* Something else bizarre happened, so do nothing and
                   get rid of callback */
		return GGZ_HOOK_REMOVE;
		break;
	}
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

