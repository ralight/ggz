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

#include <stdlib.h>
#include <string.h>

#include <easysock.h>

#include "common.h"
#include "game.h"
#include "games.h"
#include "ggz.h"

/* These names are sent to the client when options are requested.  They're different
 * from what's sent to the client as the game name later */
struct game_info game_data[7] = {
		{"Suaro", &game_funcs},
		{"Spades", &game_funcs},
		{"Hearts", &game_funcs},
		{"Bridge", &game_funcs},
		{"La Pocha", &game_funcs},
		{"Euchre", &game_funcs},
		{"Rook", &game_funcs} };

/* these aren't *quite* worthy of being in the game struct */
/* static int game_type_cnt; */
int game_types[GGZ_NUM_GAMES];	/* possible types of games; used for option requests */

/* games_get_gametype
 *   determines which game the text corresponds to.  If the --game=<game> parameter
 *   is passed to the server on startup, <game> is passed here to determine the
 *   type of game. */
int games_get_gametype(char* text)
{
	if (!strcmp(text, "bridge"))
		return GGZ_GAME_BRIDGE;

	if (!strcmp(text, "suaro"))
		return GGZ_GAME_SUARO;

	if (!strcmp(text, "lapocha"))
		return GGZ_GAME_LAPOCHA;

	if (!strcmp(text, "spades"))
		return GGZ_GAME_SPADES;

	if (!strcmp(text, "hearts"))
		return GGZ_GAME_HEARTS;

	if (!strcmp(text, "euchre"))
		return GGZ_GAME_EUCHRE;

	/* NOTE: we may not yet be connected to the ggz server, in which case this won't work. */
	ggz_debug("Unknown game for '%s'.", text);
	return GGZ_GAME_UNKNOWN;
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

	for (i=0; i < GGZ_NUM_GAMES; i++) {
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
