/*
 * File: main.c
 * Author: Ismael Orenstein
 * Project: GGZ Chess game module
 * Date: 03/01/01
 * Desc: Main loop
 * $Id: main.c 2649 2001-11-04 17:33:57Z jdorje $
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


#include "../../ggzdmod/ggz_server.h"

#include "chess.h"
#include "game.h"

extern struct chess_info game_info;


int main(void)
{

  /* Init the game info */
  game_info.clock_type = -1;
  game_info.seconds[0] = -1;
  game_info.seconds[1] = -1;
  game_info.state = CHESS_STATE_INIT;
  game_info.host = -1;
  game_info.movecount = 0;
  game_info.posrep = 0;
  game_info.draw = 0;
  /* White player starts ! */
  game_info.turn = 0;

  /* Set the handlers */
  ggzd_set_handler(GGZ_EVENT_LAUNCH, &ggz_update);
  ggzd_set_handler(GGZ_EVENT_JOIN, &ggz_update);
  ggzd_set_handler(GGZ_EVENT_LEAVE, &ggz_update);
  ggzd_set_handler(GGZ_EVENT_QUIT, &ggz_update);
  ggzd_set_handler(GGZ_EVENT_PLAYER, &game_handle_player);

  (void)ggzd_main_loop();
  return 0;
}




