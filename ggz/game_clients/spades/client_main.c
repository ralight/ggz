/*
 * File: client_main.c
 * Author: Brent Hendricks
 * Project: NetSpades
 * Date: 7/31/97
 * $Id: client_main.c 6670 2005-01-14 03:48:51Z jdorje $
 *
 * This file contains the client function which is responsible for
 * handling the particulars of playing spades on the client side.  I
 * tried to keep this file at as high a level as possible, so the code
 * could be largely algorithmic.  This makes for ease of change and
 * aids in possible future games.
 *
 * Copyright (C) 1998 Brent Hendricks.
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


#include <config.h>	/* Site data config */

#include <signal.h>	/* for signal */
#include <stdlib.h>
#include <gtk/gtk.h>
#include <stdio.h>

#include <ggz.h>	/* libggz */

#include "dlg_about.h"
#include "ggzintl.h"

#include <card.h>
#include <client.h>
#include <display.h>
#include <gtk_dlg_options.h>

#ifdef DEBUG_MEM
# include <dmalloc.h>
#endif


/* Global state of game variable */
gameState_t gameState;

static void initialize_debugging(void);
static void cleanup_debugging(void);
static void initialize_about_dialog(void);

int main(int argc, char *argv[])
{
	int i;

	initialize_debugging();

	ggz_intl_init("spades");

	/* Arrange interrupts to terminate */
	signal(SIGINT, die);
#ifdef SIGPIPE
	signal(SIGPIPE, die);
#endif

	AppInit();

	if (ParseOptions(argc, argv) < 0)
		return -1;

	gtk_init(&argc, &argv);
	initialize_about_dialog();


	/* 
	 * Display option window if this is a new table,
	 * main window otherwise
	 */
	if (gameState.get_opt)
		OptionsDialog(NULL, NULL);
	else
		DisplayInit();


	/*DisplayInit();
	   DisplayStatusLine( "Welcome to Gnu Gaming Zone Spades %s", VERSION );
	 */

	gtk_main();

	DisplayStatusLine("Session Over");
	DisplayCleanup();
	NetClose();

	/* Clean up after ourselves */
	if (gameState.record != NULL) {
		free(gameState.record);
	}
	gameState.record = NULL;

	for (i = 0; i < 4; i++) {
		if (gameState.players[i] != NULL) {
			free(gameState.players[i]);
			gameState.players[i] = NULL;
		}
	}

	cleanup_debugging();

	return (0);

}


static void initialize_debugging(void)
{
	/* Our debugging code uses libggz's ggz_debug() function, so we
	   just initialize the _types_ of debugging we want. */
#ifdef DEBUG
	const char *debugging_types[] = { "main", "socket", NULL };
#else
	const char *debugging_types[] = { NULL };
#endif
	/* Debugging goes to ~/.ggz/netspades-gtk.debug */
	char *file_name =
	    g_strdup_printf("%s/.ggz/netspades-gtk.debug", getenv("HOME"));
	ggz_debug_init(debugging_types, file_name);
	g_free(file_name);

	ggz_debug("main", "Starting NetSpades client.");
}


/* This function should be called at the end of the program to clean up
 * debugging, as necessary. */
static void cleanup_debugging(void)
{
	/* ggz_cleanup_debug writes the data out to the file and does a
	   memory check at the same time. */
	ggz_debug("main", "Shutting down NetSpades client.");
#ifdef DEBUG
	ggz_debug_cleanup(GGZ_CHECK_MEM);
#else
	ggz_debug_cleanup(GGZ_CHECK_NONE);
#endif
}

static void initialize_about_dialog(void)
{
	char *header;
	header =
	    g_strdup_printf(_("GtkSpades Client Version %s\n"), VERSION);
	init_dlg_about(_("About GtkSpades"), header,
		       _("Copyright 1999: Brent Hendricks"));
	g_free(header);
}
