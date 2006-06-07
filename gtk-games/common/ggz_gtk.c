/* 
 * File: ggz_gtk.c
 * Author: GGZ Development Team
 * Project: GGZ GTK games
 * Date: 11/11/2004
 * Desc: GGZ Handlers for GTK games
 * $Id: ggz_gtk.c 8121 2006-06-07 07:25:36Z jdorje $
 *
 * Copyright (C) 2004 GGZ Development Team
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
#  include <config.h>
#endif

#include <stdlib.h>

#include <gtk/gtk.h>
#include <ggzmod.h>

#include "dlg_players.h"
#include "ggz_gtk.h"

GtkWindow *ggz_game_main_window;

static gboolean(*game_server_handler) (GGZMod * mod);

static gboolean handle_ggz(GIOChannel * channel, GIOCondition cond,
			   gpointer data)
{
	GGZMod *mod = data;

	return (ggzmod_dispatch(mod) >= 0);
}

static gboolean handle_game_server(GIOChannel * channel, GIOCondition cond,
				   gpointer data)
{
	GGZMod *mod = data;

	return (game_server_handler) (mod);
}

static void handle_ggzmod_server(GGZMod * mod, GGZModEvent e,
				 const void *data)
{
	const int *fd = data;
	GIOChannel *channel;

	ggzmod_set_state(mod, GGZMOD_STATE_PLAYING);
	channel = g_io_channel_unix_new(*fd);
	g_io_add_watch(channel, G_IO_IN, handle_game_server, mod);
}

GGZMod *init_ggz_gtk(GtkWindow * main_window,
		     gboolean(*game_handler) (GGZMod * mod))
{
	GIOChannel *channel;
	GGZMod *mod;

	if (!ggzmod_is_ggz_mode()) {
		printf(_("This program should only be run from within GGZ."));
		printf("\n");
		exit(1);
	}

	ggz_game_main_window = main_window;

	/* Connect to GGZ */
	mod = ggzmod_new(GGZMOD_GAME);
	game_server_handler = game_handler;
	ggzmod_set_handler(mod, GGZMOD_EVENT_SERVER, handle_ggzmod_server);
	ggzmod_connect(mod);

	channel = g_io_channel_unix_new(ggzmod_get_fd(mod));
	g_io_add_watch(channel, G_IO_IN, handle_ggz, mod);

	init_player_list(mod);

	return mod;
}
