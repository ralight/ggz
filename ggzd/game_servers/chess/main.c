/*
 * File: main.c
 * Author: Ismael Orenstein
 * Project: GGZ Chess game module
 * Date: 03/01/01
 * Desc: Main loop
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


#include <ggz.h>

#include <chess.h>
#include <game.h>

extern struct chess_info game_info;


int main(void)
{
  ggz_init("Chess");

  /* Set the handlers */
  ggz_set_handler(GGZ_EVENT_LAUNCH, (GGZHandler)game_update);
  ggz_set_handler(GGZ_EVENT_JOIN, (GGZHandler)game_update);
  ggz_set_handler(GGZ_EVENT_LEAVE, (GGZHandler)game_update);
  ggz_set_handler(GGZ_EVENT_QUIT, (GGZHandler)game_update);
  ggz_set_handler(GGZ_EVENT_PLAYER, (GGZHandler)game_handle_player);

  /* Init the game info */
  game_info.clock_type = -1;
  game_info.seconds[0] = -1;
  game_info.seconds[1] = -1;
  game_info.state = CHESS_STATE_INIT;
  game_info.host = -1;
  /* White player starts ! */
  game_info.turn = 0;

  ggz_main();
	return 0;
}




