/*
 * File: client_main.c
 * Author: Brent Hendricks
 * Project: NetSpades
 * Date: 7/31/97
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


#include <config.h>		/* Site data config */

#include <signal.h>		/* for signal */
#include <stdlib.h>
#include <gtk/gtk.h>
#include <stdio.h>

#include <card.h>
#include <client.h>
#include <display.h>
#include <gtk_dlg_options.h>

#ifdef DEBUG_MEM
# include <dmalloc.h>
#endif


/* Global state of game variable */
gameState_t gameState;


int main(int argc, char *argv[])
{
	int i;

	/* Arrange interrupts to terminate */
	signal(SIGINT, die);
	signal(SIGPIPE, die);

	AppInit();
	
	if (ParseOptions(argc, argv) < 0)
		return -1;
	
	gtk_init(&argc, &argv);

	OptionsDialog(NULL, NULL);
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

	return (0);

}


