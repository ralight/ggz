/* 
 * File: games.c
 * Author: Jason Short
 * Project: GGZCards Server
 * Date: 06/20/2001
 * Desc: multi-game code
 * $Id: games.c 3992 2002-04-15 09:36:11Z jdorje $
 *
 * This file contains the data and functions that allow the game type to
 * be picked and the right functions for that game to be set up.  It's
 * pretty messy.
 *
 * Copyright (C) 2001-2002 Brent Hendricks.
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
#  include <config.h>			/* Site-specific config */
#endif

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <ggz.h>

#include "common.h"
#include "net.h"

/* BEGINNING of game data.  To add a new game, you'll need to add information 
   about it here in the form of an extern declaration of its function pointer 
   set, and an entry into the game_data array */

/* an extern should be listed here for each game function set you create this 
   is preferable to including the whole <game>.h file. */
extern game_data_t suaro_data;
extern game_data_t spades_data;
extern game_data_t hearts_data;
extern game_data_t bridge_data;
extern game_data_t lapocha_data;
extern game_data_t euchre_data;
extern game_data_t sueca_data;
extern game_data_t whist_data;

/* This holds the actual info about the different game modules.  It
   need not correspond with the enumeration in games.h. */
game_data_t *game_data[] = {
	&suaro_data,
	&spades_data,
	&hearts_data,
	&bridge_data,
	&lapocha_data,
	&euchre_data,
	&sueca_data,
	&whist_data
};


/* END of game data */


#define NUM_GAMES ARRAY_LEN(game_data)

/* these aren't *quite* worthy of being in the game struct */
/* static int game_type_cnt; */
static int game_types[NUM_GAMES];	/* possible types of games; used for
					   option requests */

/* games_get_gametype determines which game the text corresponds to.  If the
   --game=<game> parameter is passed to the server on startup, <game> is
   passed here to determine the type of game. */
game_data_t * games_get_gametype(char *game_name)
{
	int i;

	for (i = 0; i < NUM_GAMES; i++)
		if (!strcasecmp(game_name, game_data[i]->name))
			return game_data[i];

	/* NOTE: we may not yet be connected to the ggz server, in which case 
	   this won't work. */
	ggzdmod_log(game.ggz, "Unknown game for '%s'.", game_name);
	return NULL;
}

void games_handle_gametype(int choice)
{
	choice = game_types[choice];
	if (choice < 0 || choice >= NUM_GAMES) {
		ggzdmod_log(game.ggz,
			    "SERVER/CLIENT error: bad game type %d selected.",
			    choice);
		choice = 0;
	}
	game.data = game_data[choice];
}



/* game_valid_game returns a boolean, TRUE if the game is valid in the
   current setup and false otherwise. */
int games_get_game_id(char* game_name)
{
	int i;
	
	for (i = 0; i < NUM_GAMES; i++) {
		if (strcmp(game_data[i]->name, game_name) == 0)
			return i;
	}
	
	assert(FALSE);
	return -1;
}

/* This function requests the game type from the host client.  It's only
   called if this information isn't determined automatically (i.e. via
   --game=<game> parameter).  The reply is sent to games_handle_gametype,
   below. */
int games_req_gametype()
{
	int fd = get_player_socket(game.host);
	int cnt = 0, i;
	int status = 0;
	const char *desc = "What do you want to play today?";
	
	assert(fd >= 0);

	for (i = 0; i < NUM_GAMES; i++) {
		if (game_data[i]->is_valid_game()) {
			game_types[cnt] = i;
			cnt++;
		}
	}

	if (cnt == 0)
		fatal_error("BUG: games_req_gametype: no valid games.");

	if (cnt == 1) {
		ggzdmod_log(game.ggz, "Just one valid game: choosing %d.",
			    game_types[0]);
		game.data = game_data[game_types[0]];
		init_game();
		broadcast_sync();
		return 0;
	}

	if (write_opcode(fd, REQ_OPTIONS) < 0 ||
	    ggz_write_int(fd, 1) < 0 ||	/* 1 option */
	    ggz_write_string(fd, desc) < 0 || /* description */
	    ggz_write_int(fd, cnt) < 0 ||	/* cnt choices */
	    ggz_write_int(fd, 0) < 0)	/* default is 0 */
		status = -1;
	for (i = 0; i < cnt; i++) {
		game_data_t *data = game_data[game_types[i]];
		if (ggz_write_string(fd, data->full_name) < 0)
			status = -1;
	}

	if (status != 0)
		ggzdmod_log(game.ggz,
			    "ERROR: games_req_gametype: status is %d.",
			    status);
	return status;
}
