/*
 * File: main.c
 * Author: Ismael Orenstein
 * Project: GGZ Combat game module
 * Date: 09/17/2000
 * Desc: Combat client main loop
 * $Id: main.c 3392 2002-02-17 09:29:11Z jdorje $
 *
 * Copyright (C) 2000 Ismael Orenstein.
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


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <gtk/gtk.h>

#include <ggz.h>
#include <ggzmod.h>

#include "combat.h"
#include "game.h"
#include "interface.h"
#include "callbacks.h"
#include "support.h"
#include "config.h"

#define DEFAULTINSTALLEDDIR GGZDATADIR "/combat/pixmaps/default/"
#define DEFAULTSOURCEDIR PACKAGE_SOURCE_DIR "/combat/pixmaps/default"

/* main window widget */
extern GtkWidget *main_win;

/* Game file descriptior */
extern struct game_info_t cbt_info;

/* Global game variables */
combat_game cbt_game;

static void initialize_debugging(void);
static void cleanup_debugging(void);

int main(int argc, char *argv[]) {
	
	initialize_debugging();
	gtk_init(&argc, &argv);

	game_init();

	// TODO: Check for directory the user wants
	add_pixmap_directory("tiles/default");
	add_pixmap_directory(DEFAULTINSTALLEDDIR);
	add_pixmap_directory(DEFAULTSOURCEDIR);

	main_win = create_main_window();
	gtk_widget_show(main_win);

	cbt_info.fd = ggzmod_connect();
	if (cbt_info.fd < 0) return -1;

	gdk_input_add(cbt_info.fd, GDK_INPUT_READ, game_handle_io, NULL);

	gtk_main();

	if (ggzmod_disconnect() < 0)
		return -2;
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
	/* Debugging goes to ~/.ggz/combat-gtk.debug */
	char *file_name =
		g_strdup_printf("%s/.ggz/combat-gtk.debug", getenv("HOME"));
	ggz_debug_init(debugging_types, file_name);
	g_free(file_name);

	ggz_debug("main", "Starting combat client.");	
}


/* This function should be called at the end of the program to clean up
 * debugging, as necessary. */
static void cleanup_debugging(void)
{
	/* ggz_cleanup_debug writes the data out to the file and does a
	   memory check at the same time. */
	ggz_debug("main", "Shutting down combat client.");
#ifdef DEBUG
	ggz_debug_cleanup(GGZ_CHECK_MEM);
#else
	ggz_debug_cleanup(GGZ_CHECK_NONE);
#endif
}
