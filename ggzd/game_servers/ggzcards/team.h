/*
 * File: team.h
 * Author: Jason Short
 * Project: GGZCards Server
 * Date: 02/27/2002
 * Desc: Functions and data for tracking teams
 * $Id: team.h 8456 2006-08-02 06:00:35Z jdorje $
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

#ifndef __TEAM_H__
#define __TEAM_H__

#include "types.h"

/** A player with this as their "team" is not on a team. */
#define NO_TEAM -1

/** @brief Set the number of teams in the game.
 *
 *  If called by the game's init_game() function (or later, potentially),
 *  this will set the number of teams in the game.  If it is called, then
 *  assign_team must be called to assign each team, although a single
 *  player may still be teamless (this is untested, though).  If it is
 *  not called, then game.num_teams will remain 0 and every player's team
 *  will stay at NO_TEAM.
 *  @param num_teams The number of teams; 0 <= num_teams <= game.num_players
 */
void set_num_teams(int num_teams);

/** @brief Put player p on team t.
 *  @param t The ID number of the team; 0 <= t < game.num_teams
 *  @param p The player to assign
 */
void assign_team(team_t t, player_t p);

/** @brief Call the given function for every player on the team.
 *
 *  This is just a convenience function.  For every player on the
 *  given team, the given function is called.
 *  @param t The ID number of the team; 0 <= t < game.num_teams
 *  @param func The function to call for all players on that team.
 */
void map_func_to_team(team_t t, void (func)(player_t));

/** @brief Return the number of tricks the team has taken. */
int get_team_tricks(team_t t);

/** @brief Return the player's team, or NO_TEAM. */
team_t get_player_team(player_t p);

/** @brief Return the seat's team, or NO_TEAM. */
team_t get_seat_team(seat_t s);

#endif /* __TEAM_H__ */
