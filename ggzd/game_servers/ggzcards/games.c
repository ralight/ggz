/*
 * File: games.c
 * Author: Jason Short
 * Project: GGZCards game module
 * Date: 06/20/2001
 * Desc: Data and functions related to all games
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

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include <easysock.h>

#include "common.h"
#include "game.h"
#include "games.h"
#include "ggz.h"

/* an extern should be listed here for each game function set you create
 * this is preferable to including the whole <game>.h file. */
extern struct game_function_pointers suaro_funcs;
extern struct game_function_pointers spades_funcs;
extern struct game_function_pointers hearts_funcs;

/* These names are sent to the client when options are requested.  They're different
 * from what's sent to the client as the game name later.  They MUST
 * correspond in ordering to the enumeration defined in games.h.  Finally, the
 * text name should be all lower-case and without any whitespace. */
struct game_info game_data[] = {
		{"suaro", &suaro_funcs},
		{"spades", &spades_funcs},
		{"hearts", &hearts_funcs},
		{"bridge", &game_funcs},
		{"lapocha", &game_funcs},
		{"euchre", &game_funcs},
		{"rook", &game_funcs} };




const int num_games = sizeof(game_data) / sizeof(struct game_info);

/* these aren't *quite* worthy of being in the game struct */
/* static int game_type_cnt; */
static int game_types[sizeof(game_data) / sizeof(struct game_info)];	/* possible types of games; used for option requests */

/* games_get_gametype
 *   determines which game the text corresponds to.  If the --game=<game> parameter
 *   is passed to the server on startup, <game> is passed here to determine the
 *   type of game. */
int games_get_gametype(char* text)
{
	int i;

	for (i=0; i < strlen(text); i++)
		text[i] = tolower( text[i] );

	for (i = 0; i < num_games; i++)
		if (!strcmp(text, game_data[i].name))
			return i;

	/* NOTE: we may not yet be connected to the ggz server, in which case this won't work. */
	ggz_debug("Unknown game for '%s'.", text);
	return GGZ_GAME_UNKNOWN;
}

void games_handle_gametype(int option)
{
	game.which_game = game_types[option];

	if (game.which_game < 0 || game.which_game >= num_games) {
		ggz_debug("SERVER/CLIENT error: bad game type %d selected; using %d instead.", game.which_game, game_types[0]);
		game.which_game = game_types[0];
	}
}



/* game_valid_game
 *   returns a boolean, TRUE if the game is valid in the current setup and false otherwise.
 *   currently, the "current setup" is just the number of players (which is set automatically
 *   by ggz)
 */
int games_valid_game(int which_game)
{
	switch (which_game) {
		case GGZ_GAME_SUARO:
			return (game.num_players == 2);
		case GGZ_GAME_ROOK:
		case GGZ_GAME_SKAT:
			return 0;	/* not yet supported */
		case GGZ_GAME_BRIDGE:
		case GGZ_GAME_LAPOCHA:
		case GGZ_GAME_SPADES:
		case GGZ_GAME_EUCHRE:
			return (game.num_players == 4);
		case GGZ_GAME_HEARTS:
			return (game.num_players > 2 && game.num_players <= 7);	/* 3-7 players */
		default:
			ggz_debug("SERVER BUG: game_valid_game: unknown game %d.", which_game);
			return 0;
	}
}

/* games_req_gametype
 *   this function requests the game type from the host client.  It's only called if
 *   this information isn't determined automatically (i.e. via --game=spades parameter).
 *   the reply is sent to games_handle_gametype, below. */
int games_req_gametype()
{
	int fd = ggz_seats[game.host].fd;
	int cnt = 0, i;
	int status = 0;
	if (fd == -1) {
		ggz_debug("SERVER BUG: nonexistent host.");
		return -1;
	}

	for (i=0; i < num_games; i++) {
		if (games_valid_game(i)) {
			game_types[cnt] = i;
			cnt++;
		}
	}

	if (cnt == 0) {
		ggz_debug("SERVER BUG: no valid games in games_req_gametype.");
		exit(-1);
	}

	if (cnt == 1) {
		ggz_debug("Just one valid game: choosing %d.", game_types[0]);
		game.which_game = game_types[0];
		init_game();
		send_sync_all();
		return 0;
	}

	if (es_write_int(fd, WH_REQ_OPTIONS) < 0 ||
	    es_write_int(fd, 1) < 0 || /* 1 option */
	    es_write_int(fd, cnt) < 0 || /* cnt choices */
	    es_write_int(fd, 0) < 0) /* default is 0 */
		status = -1;
	for (i=0; i<cnt; i++)
		if (es_write_string(fd, game_data[game_types[i]].name) < 0)
			status = -1;

	if (status != 0)
		ggz_debug("ERROR: games_req_gametype: status is %d.", status);
	return status;
}
