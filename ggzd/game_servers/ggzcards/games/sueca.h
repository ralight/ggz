/*
 * File: games/sueca.h
 * Author: Ismael Orenstein
 * Project: GGZCards game module
 * Date: 07/03/2001
 * Desc: Game-dependent game functions for Sueca
 *
 * Copyright (C) 2001 Ismael Orenstein
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


#define GSUECA ( *(sueca_game_t *)(game.specific) )
typedef struct sueca_game_t {
  int points_on_hand[4];
} sueca_game_t;

