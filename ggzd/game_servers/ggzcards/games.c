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

#include "games.h"


/* these aren't *quite* worthy of being in the game struct */
int game_type_cnt;
int game_types[GGZ_NUM_GAMES];	/* possible types of games; used for option requests */
