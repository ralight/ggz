/* 
 * File: games.c
 * Author: Jason Short
 * Project: GGZCards Server
 * Date: 06/20/2001
 * Desc: multi-game code
 * $Id: games.c 10225 2008-07-08 18:19:56Z jdorje $
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

#include "net_common.h"

#include "common.h"
#include "games.h"
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
extern game_data_t fortytwo_data;

/* This holds the actual info about the different game modules.  It
   need not correspond with the enumeration in games.h. */
static game_data_t *game_data[] = {
	&bridge_data,
	&euchre_data,
	&fortytwo_data,
	&hearts_data,
	&lapocha_data,
	&spades_data,
	&suaro_data,
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
	ggz_debug(DBG_MISC, "Unknown game for '%s'.", game_name);
	return NULL;
}

void games_handle_gametype(int choice)
{
	choice = game_types[choice];
	if (choice < 0 || choice >= NUM_GAMES) {
		ggz_debug(DBG_MISC,
			    "SERVER/CLIENT error: bad game type %d selected.",
			    choice);
		choice = 0;
	}
	game.data = game_data[choice];
}

/* This function requests the game type from the host client.  It's only
   called if this information isn't determined automatically (i.e. via
   --game=<game> parameter).  The reply is sent to games_handle_gametype,
   below. */
void request_client_gametype(void)
{
	int cnt = 0, i;

	for (i = 0; i < NUM_GAMES; i++) {
		if (game_data[i]->is_valid_game()) {
			game_types[cnt] = i;
			cnt++;
		}
	}

	if (cnt <= 0)
		fatal_error("BUG: games_req_gametype: no valid games.");
	else if (cnt == 1) {
		ggz_debug(DBG_MISC, "Just one valid game: choosing %d.",
			    game_types[0]);
		game.data = game_data[game_types[0]];
		init_game();
		broadcast_sync();
	} else {
		char* option_types[1];
		char* option_descs[1];
		int num_choices[1];
		int option_defaults[1];
		char** option_choices[1];
		char* the_option_choices[cnt];

		option_types[0] = "Game Choice";
		option_descs[0] = "What do you want to play today?";
		num_choices[0] = cnt;
		option_defaults[0] = 0;
		option_choices[0] = the_option_choices;
		for (i = 0; i < cnt; i++)
			option_choices[0][i] = game_data[game_types[i]]->full_name;

		net_send_options_request(game.host,
		                         1, /* 1 option */
					 option_types,
		                         option_descs,
		                         num_choices,
		                         option_defaults,
		                         option_choices);
	}
}
