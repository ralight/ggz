/*
 * File: main.c
 * Author: Ismael Orenstein
 * Project: GGZ Chess game module
 * Date: 09/17/2000
 * Desc: Chess client main game loop
 * $Id: main.c 9952 2008-04-12 22:50:04Z oojah $
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

#include "ggz_gtk.h"

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

int main(int argc, char *argv[])
{
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

	mod = init_ggz_gtk(GTK_WINDOW(main_win), net_handle_input);
	if (!mod) return EXIT_FAILURE;

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
#ifdef WIN32
	char *file_name =
	    g_strdup_printf("%s\\.ggz\\chess-gtk.debug", getenv("APPDATA"));
#else
	char *file_name =
	    g_strdup_printf("%s/.ggz/chess-gtk.debug", getenv("HOME"));
#endif
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
			 "Website: http://www.ggzgamingzone.org/gameclients/chess/"));
	g_free(header);
}
