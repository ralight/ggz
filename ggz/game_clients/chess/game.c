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
#include "support.h"
#include "game.h"
#include "board.h"
#include "chess.h"
#include "popup.h"
#include "net.h"

/* Game info struct */
extern struct chess_info game_info;

/* Main window */
extern GtkWidget *main_win;

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
  game_info.check = FALSE;
	strcpy(game_info.name[0], "White");
	strcpy(game_info.name[1], "Black");
}

/* FIXME: Create a game_popup
void game_popup(const char *format, ...) {
  game_message(format);
}
*/

#define game_popup game_message

void game_message(const char *format, ...) {
  int id;
  va_list ap;
  char *message;
  GtkWidget *status;

  va_start( ap, format );
  message = g_strdup_vprintf(format, ap);
  va_end(ap);

  status = lookup_widget(main_win, "statusbar");
  id = gtk_statusbar_get_context_id( GTK_STATUSBAR(status), "Main" );
  gtk_statusbar_pop( GTK_STATUSBAR(status), id );
  gtk_statusbar_push( GTK_STATUSBAR(status), id, message );

  g_free( message );
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
 * CHESS_EVENT_MOVE_END -> arg = (str)MOVE
 * CHESS_EVENT_MOVE -> arg = (str)MOVE
 * CHESS_EVENT_GAMEOVER -> arg[0] = (char)OVER_CODE
 *
 */
void game_update(int event, void *arg) {
  char move[6];
  int retval = 0;
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
			if (game_info.assign[0] != GGZ_SEAT_OPEN)
				strcpy(game_info.name[0], (char *)(arg+1));
			if (game_info.assign[1] != GGZ_SEAT_OPEN)
				strcpy(game_info.name[1], (char *)(arg+21));
			board_info_update();
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
      game_info.clock_type = (*((int*)arg)) >> 24;
      game_info.seconds[0] = (*((int*)arg)) & 0xFFFFFF;
      game_info.seconds[1] = (*((int*)arg)) & 0xFFFFFF;
      game_popup("Clock type is %d and time is %d", game_info.clock_type, game_info.seconds[0]);
      break;
    case CHESS_EVENT_START:
      if (game_info.state != CHESS_STATE_WAIT)
        break;
      game_info.state = CHESS_STATE_PLAYING;
      game_message("The game has started!");
      break;
    case CHESS_EVENT_MOVE_END:
      if (game_info.state != CHESS_STATE_PLAYING)
        break;
      net_send_move(arg);
      //net_send_move( *(int*)arg+(8**((int*)arg+1)), *((int*)arg+2)+(8**((int*)arg+3)));
      game_message("Sending move to server...");
      break;
    case CHESS_EVENT_MOVE:
      if (game_info.state != CHESS_STATE_PLAYING)
        break;
      if (!arg) {
        game_message("Invalid move!");
        break;
      }
      /*
      move[0] = 65 + ((*(char*)arg)%8);
      move[1] = 49 + ( (*(char*)arg)/8 );
      move[2] = 65 + ((*((char*)arg+1))%8);
      move[3] = 49 + ((*((char*)arg+1))/8);
      move[4] = 0;
      */
      strncpy(move, arg, 6);
      game_message("Making move %s", move);
      game_info.check = FALSE;
      retval = cgc_make_move(game, move);
      if (retval == CHECK || retval == MATE) {
        if ((game_info.turn%2!=game_info.seat)) {
          game_info.check = TRUE;
          game_message("You are in check!");
        } else {
          game_message("Your opponent is in check!");
        }
      }
      game_info.turn++;
			board_info_update();
      board_info_add_move(move);
      board_draw();
      break;
    case CHESS_EVENT_GAMEOVER:
      switch ( *(char*)arg ) {
        case CHESS_GAMEOVER_DRAW_AGREEMENT:
          game_message("Game is over! Both players agred on a draw");
          break;
        case CHESS_GAMEOVER_DRAW_STALEMATE:
          game_message("Game is over! We have a stalemate");
          break;
        case CHESS_GAMEOVER_DRAW_POSREP:
          game_message("Game is over! Too much repetition of positions");
          break;
        case CHESS_GAMEOVER_DRAW_MATERIAL:
          game_message("Game is over! Neither of the players has material for a mate");
          break;
        case CHESS_GAMEOVER_DRAW_MOVECOUNT:
          game_message("Game is over! The maximum movecount was reached");
          break;
        case CHESS_GAMEOVER_DRAW_TIMEMATERIAL:
          game_message("Gams is over! Time is out and the remaining player doesn't have enough material for a mate");
          break;
        case CHESS_GAMEOVER_WIN_1_MATE:
          game_message("Game is over! Player 1 wins by a mate");
          break;
        case CHESS_GAMEOVER_WIN_1_RESIGN:
          game_message("Game is over! Player 1 wins, Player 2 has resigned");
          break;
        case CHESS_GAMEOVER_WIN_1_FLAG:
          game_message("Game is over! Player 1 wins, Player 2 has run out of time");
          break;
        case CHESS_GAMEOVER_WIN_2_MATE:
          game_message("Game is over! Player 2 wins by a mate");
          break;
        case CHESS_GAMEOVER_WIN_2_RESIGN:
          game_message("Game is over! Player 2 wins, Player 1 has resigned");
          break;
        case CHESS_GAMEOVER_WIN_2_FLAG:
          game_message("Game is over! Player 2 wins, Player 1 has run out of time");
          break;
        default:
          game_message("The game should be over, I don't know why");
      }
      game_info.state = CHESS_STATE_DONE;
      break;
    case CHESS_EVENT_DRAW:
      gtk_widget_show(create_draw_dialog());
      break;
    default:
      game_message("Unknown event! %d", event);
      break;
  }
}
