/*
 * File: net.c
 * Author: Ismael Orenstein
 * Project: GGZ Chess game module
 * Date: 09/17/2000
 * Desc: Functions to filter input and send the events to game.c and send stuff
 * out to the server
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

#include "game.h"
#include "chess.h"
#include <easysock.h>
#include <stdio.h>
#include <gtk/gtk.h>

void net_handle_input(gpointer data, int fd, GdkInputCondition cond) {
  char op;
  char args[2];

  /* Get the opcode */
  if (es_read_char(fd, &op) < 0)
    return;

  printf("Received opcode %d\n", op);

  switch (op) {
    case CHESS_MSG_SEAT:
      /* args[0] holds the seat */
      es_read_char(fd, &args[0]);
      /* args[1] holds version */
      es_read_char(fd, &args[1]);
      /* Issue a EVENT_SEAT */
      game_update(CHESS_EVENT_SEAT, args);
      break;
  }

}
