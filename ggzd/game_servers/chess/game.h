/*
 * File: game.h
 * Author: Ismael Orenstein
 * Project: GGZ Chess game module
 * Date: 03/01/01
 * Desc: Header file for game functions
 * $Id: game.h 6892 2005-01-25 04:09:21Z jdorje $
 *
 * Copyright (C) 2000 Ismael Orenstein.
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

#include "ggzdmod.h"

#define DEBUG_GAME "chess"
#define DEBUG_AI "ai"

/* Translate a GGZ event into a chess event --JDS */
void game_handle_ggz_state(GGZdMod *ggz,
			   GGZdModEvent event, const void *data);
void game_handle_ggz_join(GGZdMod *ggz,
			  GGZdModEvent event, const void *data);
void game_handle_ggz_leave(GGZdMod *ggz,
			   GGZdModEvent event, const void *data);

/* Filter the msg from the player into the
 * appropriated event */
void game_handle_player_data(GGZdMod *ggz,
			     GGZdModEvent event, const void *seat_data);
