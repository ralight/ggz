/*
 * File: game.c
 * Author: Ismael Orenstein
 * Project: GGZ Chess game module
 * Date: 09/17/2000
 * Desc: Game functions
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

#include <stdlib.h>
#include <stdio.h>
#include <libcgc/cgc.h>
#include "game.h"
#include "chess.h"

/* Game info struct */
extern struct chess_info game_info;

game_t *game = NULL;

void game_init() {
  /* Init the libcgc structures */
  game = cgc_create_game();
  cgc_join_game(game, WHITE);
  cgc_join_game(game, BLACK);
}

void game_message(char *msg) {
  printf(msg);
}

/* Events:
 * CHESS_EVENT_SEAT -> arg[0] = (char)SEAT
 *                     arg[1] = (char)VERSION
 */
void game_update(int event, void *arg) {
  switch (event) {
    case CHESS_EVENT_SEAT:
      /* Update the game info structure */
      game_info.seat = ((char *)arg)[0];
      game_info.version = ((char *)arg)[1];
      if (game_info.version != PROTOCOL_VERSION)
        game_message("Incompatible version. The game may not run as expected");
      break;
  }
}
