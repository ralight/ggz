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
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>

#define GGZ_SEAT_OPEN -1

extern struct chess_info game_info;

void net_handle_input(gpointer data, int fd, GdkInputCondition cond) {
  char op;
  char args[2];
  int a;
  char *names;

  /* Get the opcode */
  if (es_read_char(fd, &op) < 0)
    return;

  switch (op) {
    case CHESS_MSG_SEAT:
      printf("Received MSG_SEAT\n");
      /* args[0] holds the seat */
      es_read_char(fd, &args[0]);
      /* args[1] holds version */
      es_read_char(fd, &args[1]);
      /* Issue a EVENT_SEAT */
      game_update(CHESS_EVENT_SEAT, args);
      break;
    case CHESS_MSG_PLAYERS:
      printf("Got an MSG_PLAYERS\n");
      names = (char *)malloc(40 * sizeof(char));
      bzero(names, 40);
      /* Get first code */
      es_read_char(fd, &names[0]);
      if (names[0] != GGZ_SEAT_OPEN)
        es_read_string(fd, names+1, 17);
      /* Get second code */
      es_read_char(fd, &names[20]);
      if (names[20] != GGZ_SEAT_OPEN)
        es_read_string(fd, names+21, 17);
      /* Issue an event */
      game_update(CHESS_EVENT_PLAYERS, names);
      break;
    case CHESS_REQ_TIME:
      printf("Got an REQ_TIME\n");
      /* The server is requesting my time option */
      game_update(CHESS_EVENT_TIME_REQUEST, NULL);
      break;
    case CHESS_RSP_TIME:
      printf("Got an RSP_TIME\n");
      /* The server is sending the time option */
      es_read_int(fd, &a);
      game_update(CHESS_EVENT_TIME_OPTION, &a);
      break;
    case CHESS_MSG_START:
      printf("Got an MSG_START\n");
      /* We should start the game now */
      game_update(CHESS_EVENT_START, NULL);
      break;
    case CHESS_MSG_MOVE:
      printf("Got an MSG_MOVE\n");
      /* We got an move! */
      es_read_char(fd, &args[0]);
      if (args[0] == -1) 
        /* Invalid move */
        game_update(CHESS_EVENT_MOVE, NULL);
      else {
        es_read_char(fd, &args[1]);
        game_update(CHESS_EVENT_MOVE, args);
      }
      break;
    case CHESS_MSG_GAMEOVER:
      /* The game is over */
      printf("Got a MSG_GAMEOVER\n");
      es_read_char(fd, &args[0]);
      game_update(CHESS_EVENT_GAMEOVER, args);
      break;
    case CHESS_REQ_DRAW:
      /* Do you want to draw the game ? */
      printf("Got a REQ_DRAW\n");
      game_update(CHESS_EVENT_DRAW, NULL);
      break;
    default:
      game_message("Unknown opcode: %d\n", op);
      break;
  }

}

void net_send_time(int time_option) {
  es_write_char(game_info.fd, CHESS_RSP_TIME);
  es_write_int(game_info.fd, time_option);
}

void net_send_move(char from, char to) {
  es_write_char(game_info.fd, CHESS_REQ_MOVE);
  es_write_char(game_info.fd, from);
  es_write_char(game_info.fd, to);
}

void net_send_draw() {
  es_write_char(game_info.fd, CHESS_REQ_DRAW);
}
