/*
 * File: team.c
 * Author: Jason Short
 * Project: GGZCards Server
 * Date: 02/27/2002
 * Desc: Functions and data for tracking teams
 * $Id: team.c 8443 2006-08-01 16:40:16Z jdorje $
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-130
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>			/* Site-specific config */
#endif

#include <assert.h>

#include "common.h"
#include "team.h"

void set_num_teams(int num_teams)
{
	game.num_teams = num_teams;
}

void assign_team(team_t t, player_t p)
{
	assert(t >= 0 && t < game.num_teams);
	assert(p >= 0 && p < game.num_players);
	game.players[p].team = t;
}


void map_func_to_team(team_t t, void (func)(player_t))
{
	int p;
	assert(t >= 0 && t < game.num_teams);
	for (p = 0; p < game.num_players; p++) {
		if (game.players[p].team == t)
			func(p);
	}
}

int get_team_tricks(team_t t)
{
	int tricks = 0;
	player_t p;
	
	for (p = 0; p < game.num_players; p++)
		if (game.players[p].team == t)
			tricks += game.players[p].tricks;
			
	return tricks;
}

int get_team_score(team_t t)
{
	player_t p;
	
	for (p = 0; p < game.num_players; p++)
		if (game.players[p].team == t)
			return game.players[p].score;
			
	assert(FALSE);
	return 0;
}

team_t get_player_team(player_t p)
{
	/* This applies to spectators or empty seats. */
	if (p < 0) return NO_TEAM;

	return game.players[p].team;
}

team_t get_seat_team(seat_t s)
{
	return get_player_team(SEAT_TO_PLAYER(s));
}
