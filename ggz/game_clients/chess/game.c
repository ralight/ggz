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

#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <libcgc/cgc.h>
#include "game.h"
#include "board.h"
#include "chess.h"
#include "net.h"

/* Game info struct */
extern struct chess_info game_info;

game_t *game = NULL;

void game_init() {
  /* Init the libcgc structures */
  game = cgc_create_game();
  cgc_join_game(game, WHITE);
  cgc_join_game(game, BLACK);
  /* Init the game_info structure */
  game_info.clock_type = 0;
  game_info.seconds[0] = 0;
  game_info.seconds[1] = 0;
  game_info.turn = 0;
  bzero(game_info.name[0], 18);
  bzero(game_info.name[1], 18);
}

void game_popup(char *msg) {
  game_message(msg);
}

void game_message(char *msg) {
  printf(msg);
}

/* Events:
 * CHESS_EVENT_INIT -> NULL
 * CHESS_EVENT_SEAT -> arg[0] = (char)SEAT
 *                     arg[1] = (char)VERSION
 * CHESS_EVENT_PLAYERS arg[0] = (char)ASSIGN1
 *                     arg[1-17] = (string)NAME1
 *                     arg[20] = (char)ASSIGN2
 *                     arg[21-37] = (string)NAME2
 * CHESS_EVENT_TIME_REQUEST -> NULL
 * CHESS_EVENT_TIME_OPTION -> time option
 * CHESS_EVENT_START -> NULL
 * CHESS_EVENT_MOVE_END -> arg[0] = (int)FROM_X
 *                         arg[1] = (int)FROM_Y
 *                         arg[2] = (int)TO_X
 *                         arg[3] = (int)TO_Y
 * CHESS_EVENT_MOVE -> arg[0] = (char)FROM
 *                     arg[1] = (char)TO
 * CHESS_EVENT_GAMEOVER -> arg[0] = (char)OVER_CODE
 *
 */
void game_update(int event, void *arg) {
  char move[5];
  switch (event) {
    case CHESS_EVENT_INIT:
      if (game_info.state != CHESS_STATE_INIT)
        break;
      game_init();
      game_info.state = CHESS_STATE_WAIT;
      break;
    case CHESS_EVENT_SEAT:
      if (game_info.state != CHESS_STATE_WAIT)
        break;
      /* Update the game info structure */
      game_info.seat = ((char *)arg)[0];
      game_info.version = ((char *)arg)[1];
      if (game_info.version != PROTOCOL_VERSION)
        game_popup("Incompatible version. The game may not run as expected");
      break;
    case CHESS_EVENT_PLAYERS:
      if (game_info.state != CHESS_STATE_WAIT)
        break;
      /* Update the game info structure */
      game_info.assign[0] = *(char *)arg;
      game_info.assign[1] = *(char *)(arg+20);
      strcpy(game_info.name[0], (char *)(arg+1));
      strcpy(game_info.name[1], (char *)(arg+21));
      /* FIXME: Should be game_message */
      printf("Player 1 is %s and Player 2 is %s\n", game_info.name[0], game_info.name[1]);
      free(arg);
      break;
    case CHESS_EVENT_TIME_REQUEST:
      if (game_info.state != CHESS_STATE_WAIT)
        break;
      /* FIXME: Should ask the user for the time */
      net_send_time(0);
      break;
    case CHESS_EVENT_TIME_OPTION:
      if (game_info.state != CHESS_STATE_WAIT)
        break;
      game_info.clock_type = *(int*)arg >> 24;
      game_info.seconds[0] = *(int*)arg & 0xFFFFFF;
      game_info.seconds[1] = *(int*)arg & 0xFFFFFF;
      /* FIXME: Should be game_popup */
      printf("Clock type is %d and time is %d\n", game_info.clock_type, game_info.seconds[0]);
      break;
    case CHESS_EVENT_START:
      if (game_info.state != CHESS_STATE_WAIT)
        break;
      game_info.state = CHESS_STATE_PLAYING;
      game_message("The game has started!\n");
      break;
    case CHESS_EVENT_MOVE_END:
      if (game_info.state != CHESS_STATE_PLAYING)
        break;
      printf("Move from %d/%d to %d/%d\n", *(int*)arg, *((int*)arg+1), *((int*)arg+2), *((int*)arg+3));
      net_send_move( *(int*)arg+(8**((int*)arg+1)), *((int*)arg+2)+(8**((int*)arg+3)));
      break;
    case CHESS_EVENT_MOVE:
      if (game_info.state != CHESS_STATE_PLAYING)
        break;
      if (!arg) {
        game_message("Invalid move!\n");
        break;
      }
      move[0] = 65 + ((*(char*)arg)%8);
      move[1] = 49 + ( (*(char*)arg)/8 );
      move[2] = 65 + ((*((char*)arg+1))%8);
      move[3] = 49 + ((*((char*)arg+1))/8);
      move[4] = 0;
      cgc_make_move(game, move);
      /* FIXME: game_message */
      printf("Making move %s\n", move);
      board_draw();
      break;
    case CHESS_EVENT_GAMEOVER:
      switch ( *(char*)arg ) {
        case CHESS_GAMEOVER_DRAW_AGREEMENT:
          game_message("Game is over! Both players agred on a draw\n");
          break;
        case CHESS_GAMEOVER_DRAW_STALEMATE:
          game_message("Game is over! We have a stalemate\n");
          break;
        case CHESS_GAMEOVER_DRAW_POSREP:
          game_message("Game is over! Too much repetition of positions\n");
          break;
        case CHESS_GAMEOVER_DRAW_MATERIAL:
          game_message("Game is over! Neither of the players has material for a mate\n");
          break;
        case CHESS_GAMEOVER_DRAW_MOVECOUNT:
          game_message("Game is over! The maximum movecount was reached\n");
          break;
        case CHESS_GAMEOVER_DRAW_TIMEMATERIAL:
          game_message("Gams is over! Time is out and the remaining player doesn't have enough material for a mate\n");
          break;
        case CHESS_GAMEOVER_WIN_1_MATE:
          game_message("Game is over! Player 1 wins by a mate\n");
          break;
        case CHESS_GAMEOVER_WIN_1_RESIGN:
          game_message("Game is over! Player 1 wins, Player 2 has resigned\n");
          break;
        case CHESS_GAMEOVER_WIN_1_FLAG:
          game_message("Game is over! Player 1 wins, Player 2 has run out of time\n");
          break;
        case CHESS_GAMEOVER_WIN_2_MATE:
          game_message("Game is over! Player 2 wins by a mate\n");
          break;
        case CHESS_GAMEOVER_WIN_2_RESIGN:
          game_message("Game is over! Player 2 wins, Player 1 has resigned\n");
          break;
        case CHESS_GAMEOVER_WIN_2_FLAG:
          game_message("Game is over! Player 2 wins, Player 1 has run out of time\n");
          break;
        default:
          game_message("The game should be over, I don't know why\n");
      }
      game_info.state = CHESS_STATE_DONE;
      break;
    default:
      /* FIXME: Should be game_message */
      printf("Unknown event! %d\n", event);
      break;
  }
}
