/*
 * File: main.c
 * Author: Ismael Orenstein
 * Project: GGZ Chess game module
 * Date: 09/17/2000
 * Desc: Chess client main game loop
 * $Id: main.c 6293 2004-11-07 05:51:47Z jdorje $
 *
 * Copyright (C) 2001 Ismael Orenstein.
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

#include <gtk/gtk.h>
#include <stdlib.h>

#include <ggz.h>
#include <ggz_common.h>
#include <ggzmod.h>

#include "dlg_about.h"
#include "dlg_players.h"
#include "ggzintl.h"

#include "main_win.h"
#include "support.h"
#include "chess.h"
#include "board.h"
#include "game.h"
#include "net.h"

/* main window widget */
GtkWidget *main_win;

/* Game info */
struct chess_info game_info;

static void initialize_debugging(void);
static void cleanup_debugging(void);
static void initialize_about_dialog(void);

static GGZMod *mod;

static gboolean handle_ggz(GIOChannel * source, GIOCondition condition,
			   gpointer data)
{
	ggzmod_dispatch(mod);
	return TRUE;
}

static void handle_ggzmod_server(GGZMod * mod, GGZModEvent e, void *data)
{
	int fd = *(int *)data;
	GIOChannel *channel = g_io_channel_unix_new(fd);

	ggzmod_set_state(mod, GGZMOD_STATE_PLAYING);
	game_info.fd = fd;
	g_io_add_watch(channel, G_IO_IN, net_handle_input, NULL);
}

int main(int argc, char *argv[])
{
	int ret;

	initialize_debugging();

	ggz_intl_init("chess");

	gtk_init(&argc, &argv);
	initialize_about_dialog();

	main_win = create_main_win();
	gtk_widget_realize(main_win);
	/* HACK: we have to call gtk_widget_realize (above) before calling
	 * board_init because board_init needs to access main_win->window.
	 * However we have to call board_init before gtk_widget_show (below)
	 * because gtk_widget_show requires access to the graphics contexts
	 * created in board_init. */

	game_info.state = CHESS_STATE_INIT;
	game_info.fd = -1;

	board_init();
	gtk_widget_show(main_win);
	game_update(CHESS_EVENT_INIT, NULL);

	mod = ggzmod_new(GGZMOD_GAME);
	ggzmod_set_handler(mod, GGZMOD_EVENT_SERVER,
			   &handle_ggzmod_server);
	init_player_list(mod);

	ret = ggzmod_connect(mod);
	if (ret != 0)
		return -1;

	g_io_add_watch(g_io_channel_unix_new(ggzmod_get_fd(mod)),
		       G_IO_IN, handle_ggz, NULL);

	gtk_main();

	if (ggzmod_disconnect(mod) < 0)
		return -2;
	ggzmod_free(mod);

	cleanup_debugging();

	return 0;
}


static void initialize_debugging(void)
{
	/* Our debugging code uses libggz's ggz_debug() function, so we
	   just initialize the _types_ of debugging we want. */
#ifdef DEBUG
	const char *debugging_types[] = { "main", NULL };
#else
	const char *debugging_types[] = { NULL };
#endif
	/* Debugging goes to ~/.ggz/chess-gtk.debug */
	char *file_name =
	    g_strdup_printf("%s/.ggz/chess-gtk.debug", getenv("HOME"));
	ggz_debug_init(debugging_types, file_name);
	g_free(file_name);

	ggz_debug("main", "Starting chess client.");
}


/* This function should be called at the end of the program to clean up
 * debugging, as necessary. */
static void cleanup_debugging(void)
{
	/* ggz_cleanup_debug writes the data out to the file and does a
	   memory check at the same time. */
	ggz_debug("main", "Shutting down chess client.");
#ifdef DEBUG
	ggz_debug_cleanup(GGZ_CHECK_MEM);
#else
	ggz_debug_cleanup(GGZ_CHECK_NONE);
#endif
}

static void initialize_about_dialog(void)
{
	char *header;
	header = g_strdup_printf(_("GGZ Gaming Zone\n"
				   "Chess Version %s"), VERSION);
	init_dlg_about(_("About Chess"), header,
		       _("Copyright (C) 2001 Ismael Orenstein.\n"
			 "\n"
			 "Website: http://www.ggzgamingzone.org/games/chess/"));
	g_free(header);
}
