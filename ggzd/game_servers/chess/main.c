/*
 * File: main.c
 * Author: Ismael Orenstein
 * Project: GGZ Chess game module
 * Date: 03/01/01
 * Desc: Main loop
 * $Id: main.c 6748 2005-01-20 00:23:37Z jdorje $
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

#ifdef HAVE_CONFIG_H
#  include <config.h>			/* Site-specific config */
#endif

#include <stdio.h>

#include "chess.h"
#include "game.h"

extern struct chess_info game_info;


int main(void)
{
  /* Initialize GGZ. */
  GGZdMod *ggz = ggzdmod_new(GGZDMOD_GAME);
  ggzdmod_set_handler(ggz, GGZDMOD_EVENT_STATE, &game_handle_ggz_state);
  ggzdmod_set_handler(ggz, GGZDMOD_EVENT_JOIN, &game_handle_ggz_join);
  ggzdmod_set_handler(ggz, GGZDMOD_EVENT_LEAVE, &game_handle_ggz_leave);
  ggzdmod_set_handler(ggz, GGZDMOD_EVENT_PLAYER_DATA, &game_handle_player_data);

  /* Init the game info */
  game_info.ggz = ggz;
  game_info.clock_type = CHESS_CLOCK_UNSET;
  game_info.seconds[0] = -1;
  game_info.seconds[1] = -1;
  game_info.state = CHESS_STATE_INIT;
  game_info.host = -1;
  game_info.movecount = 0;
  game_info.posrep = 0;
  game_info.draw = 0;
  /* White player starts ! */
  game_info.turn = 0;

  /* Play! */
  if (ggzdmod_connect(ggz) < 0)
    return -1;
  (void)ggzdmod_loop(ggz);
  (void)ggzdmod_disconnect(ggz);
  ggzdmod_free(ggz);

  return 0;
}




