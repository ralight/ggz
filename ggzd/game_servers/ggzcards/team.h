/*
 * File: team.h
 * Author: Jason Short
 * Project: GGZCards Server
 * Date: 02/27/2002
 * Desc: Functions and data for tracking teams
 * $Id: team.h 3495 2002-02-27 13:02:23Z jdorje $
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

void set_num_teams(int num_teams);

void assign_team(team_t t, player_t p);

void map_func_to_team(team_t t, void (func)(player_t));

#endif /* __TEAM_H__ */
