/*
 * File: game.c
 * Author: Ismael Orenstein
 * Project: GGZ Chess game module
 * Date: 09/17/2000
 * Desc: Game functions
 * $Id: game.c 5201 2002-11-04 02:47:56Z jdorje $
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

#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <libcgc/cgc.h>

#include <ggzmod.h>
#include <ggz.h>	/* libggz, for ggz_debug */
#include <ggz_common.h>

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

/* Timeout id */
gint timeout_id;

game_t *game = NULL;

void game_init(void) {
  /* Init the libcgc structures */
  game = cgc_create_game();
  cgc_join_game(game, WHITE);
  cgc_join_game(game, BLACK);
  /* Init the game_info structure */
  game_info.clock_type = 0;
  game_info.seconds[0] = 0;
  game_info.seconds[1] = 0;
  game_info.t_seconds[0] = 0;
  game_info.t_seconds[1] = 0;
  game_info.turn = 0;
  game_info.check = FALSE;
	strcpy(game_info.name[0], "White");
	strcpy(game_info.name[1], "Black");
	game_info.timer = g_timer_new();
}

void game_popup(const char *format, ...) {
	va_list ap;
	char *message;
	GtkWidget *dialog;
	GtkWidget *ok;
	GtkWidget *label;

	/* Create the message */
	va_start( ap, format );
	message = g_strdup_vprintf(format, ap);
	va_end(ap);

	/* Create the widgets */
	dialog = gtk_dialog_new();
	gtk_window_set_transient_for(GTK_WINDOW(dialog),
				     GTK_WINDOW(main_win));
	label = gtk_label_new(message);
	ok = gtk_button_new_with_label("OK");
	gtk_label_set_line_wrap( GTK_LABEL(label), TRUE );

  /* Ensure that the dialog box is destroyed when
   * the user clicks ok. */
 
  gtk_signal_connect_object (GTK_OBJECT (ok), "clicked",
                             GTK_SIGNAL_FUNC (gtk_widget_destroy), GTK_OBJECT(dialog));

  gtk_container_add (GTK_CONTAINER (GTK_DIALOG(dialog)->action_area),
                     ok);

  /* Add the label, and show everything
   * we've added to the dialog. */

   gtk_container_add (GTK_CONTAINER (GTK_DIALOG(dialog)->vbox),
                      label);
   gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
   gtk_widget_show_all (dialog);

	g_free(message);
}

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
 * CHESS_EVENT_UPDATE_TIME -> arg[0] = (int)TIME_1
 *                            arg[1] = (int)TIME_2
 *
 */
void game_update(int event, void *arg) {
  char move[6];
  /*int diff[2];*/
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
      game_info.assign[1] = *(((char *)arg) + 20);
			if (game_info.assign[0] != GGZ_SEAT_OPEN)
				strcpy(game_info.name[0], (char *)arg + 1);
			if (game_info.assign[1] != GGZ_SEAT_OPEN)
				strcpy(game_info.name[1], (char *)arg + 21);
			board_info_update();
      free(arg);
      break;
    case CHESS_EVENT_TIME_REQUEST:
      if (game_info.state != CHESS_STATE_WAIT)
        break;
      /* FIXME: Should ask the user for the time */
      gtk_widget_show(create_clock_dialog());
      break;
    case CHESS_EVENT_TIME_OPTION:
      if (game_info.state != CHESS_STATE_WAIT)
        break;
      game_info.clock_type = (*((int*)arg)) >> 24;
      game_info.seconds[0] = (*((int*)arg)) & 0xFFFFFF;
      game_info.seconds[1] = (*((int*)arg)) & 0xFFFFFF;
      game_info.t_seconds[0] = (*((int*)arg)) & 0xFFFFFF;
      game_info.t_seconds[1] = (*((int*)arg)) & 0xFFFFFF;
			switch(game_info.clock_type) {
				case CHESS_CLOCK_NOCLOCK:
					game_popup("This game won't have a time limit.");
					break;
				case CHESS_CLOCK_CLIENT:
					game_popup("This game will use a client clock.\nThis option should only be used when playing against people you trust, as it relies much in the client program, that can be cheated.\nSo, if the time behaves very strangely (like your oponnent time never wearing out), he may be running a cheated client.\n\nEach player will have %d min : %d sec to win the game.", game_info.seconds[0]/60, game_info.seconds[0]%60);
					break;
				case CHESS_CLOCK_SERVER:
					game_popup("This game will use a server clock.\nIt is very difficult to cheat when using this type of clock, and you should use it if you suspect your oponnent may have a cheated client or if you don't trust him.\nHowever, if either your connection or your opponent's is deeply lagged, it will have a deep effect on the time count as well.\n\nEach player will have %d min : %d sec to win the game.", game_info.seconds[0]/60, game_info.seconds[0]%60);
					break;
				case CHESS_CLOCK_SERVERLAG:
					game_popup("This game will use a server clock with lag support.\nIn this option, we will use a server clock, but using a lag meter to compensate for any lag due to Internet connection. Although it's possible to cheat with this option, it is much more difficult then cheating with the client clock.\nBesides, the lag of either connect won't have a so deep effect on the time of the players.\n\nEach player will have %d min : %d sec to win the game.", game_info.seconds[0]/60, game_info.seconds[0]%60);
					break;
				default:
					game_popup("Clock type is %d and time is %d", game_info.clock_type, game_info.seconds[0]);
					break;
			}
      board_info_update();
      break;
    case CHESS_EVENT_START:
      if (game_info.state != CHESS_STATE_WAIT)
        break;
      game_info.state = CHESS_STATE_PLAYING;
      if (game_info.clock_type != CHESS_CLOCK_NOCLOCK)
        gtk_timeout_add(1000, game_timer, NULL);
      game_message("The game has started!");
      break;
    case CHESS_EVENT_MOVE_END:
      if (game_info.state != CHESS_STATE_PLAYING)
        break;
			ggz_debug("main", "Sending move... %s",
			          (char *)arg);
			if (game_info.clock_type != CHESS_CLOCK_CLIENT)
				net_send_move(arg, -1);
			else {
				gtk_timeout_remove(timeout_id);
				if (game_info.turn == 0 || game_info.turn == 1)
					net_send_move(arg, 0);
				else
					net_send_move(arg, g_timer_elapsed(game_info.timer, NULL));
				g_timer_stop(game_info.timer);
				g_timer_reset(game_info.timer);
			}
      game_message("Sending move to server...");
      break;
    case CHESS_EVENT_MOVE:
      if (game_info.state != CHESS_STATE_PLAYING)
        break;
      if (!arg) {
        game_message("Invalid move!");
        break;
      }
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
      /* Now update the time, if that's the case */
      if (game_info.clock_type != CHESS_CLOCK_NOCLOCK) {
        game_info.seconds[game_info.turn%2] -= *((gint32*)arg+2);
        /* Ok, adjust the timer clocks */
        game_info.t_seconds[0] = game_info.seconds[0];
        game_info.t_seconds[1] = game_info.seconds[1];
      }
      game_info.turn++;
			/* Check if it's my turn now and we have CLIENT_CLOCK */
			if (game_info.clock_type == CHESS_CLOCK_CLIENT && game_info.turn % 2 == game_info.seat) {
				/* Starts the timer ! */
				g_timer_start(game_info.timer);
				/* Starts the timeout for MSG_UPDATE */
				timeout_id = gtk_timeout_add(15000, game_update_server, NULL);
			}
			board_info_update();
      board_info_add_move(move);
      board_draw();
      break;
		case CHESS_EVENT_UPDATE_TIME:
			game_info.seconds[0] = *((int*)arg);
			game_info.seconds[1] = *((int*)arg+1);
			game_info.t_seconds[0] = game_info.seconds[0];
			game_info.t_seconds[1] = game_info.seconds[1];
			board_info_update();
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
          game_message("Game is over! %s wins by a mate", game_info.name[0]);
          break;
        case CHESS_GAMEOVER_WIN_1_RESIGN:
          game_message("Game is over! %s wins, %s has resigned", game_info.name[0], game_info.name[1]);
          break;
        case CHESS_GAMEOVER_WIN_1_FLAG:
          game_message("Game is over! %s wins, %s has run out of time", game_info.name[0], game_info.name[1]);
          break;
        case CHESS_GAMEOVER_WIN_2_MATE:
          game_message("Game is over! %s wins by a mate", game_info.name[1]);
          break;
        case CHESS_GAMEOVER_WIN_2_RESIGN:
          game_message("Game is over! %s wins, %s has resigned", game_info.name[1], game_info.name[0]);
          break;
        case CHESS_GAMEOVER_WIN_2_FLAG:
          game_message("Game is over! %s wins, %s has run out of time", game_info.name[1], game_info.name[0]);
          break;
        default:
          game_message("The game should be over, but I don't know why");
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

int game_timer(gpointer user_data) {
  if (game_info.state == CHESS_STATE_DONE)
    return FALSE;
  if (game_info.state != CHESS_STATE_PLAYING)
    return TRUE;
  if (game_info.turn == 0 || game_info.turn == 1)
    return TRUE;
  /* Ok, update the timer */
  game_info.t_seconds[game_info.turn % 2]--;
  /* Check if we should call a flag */
  if (board_auto_call() && game_info.t_seconds[(game_info.seat+1)%2] <= 0)
    board_call_flag();
  board_info_update();
  return TRUE;
}

int game_update_server(gpointer user_data) {
	int time;
	time = g_timer_elapsed(game_info.timer, NULL);
	g_timer_reset(game_info.timer);
	net_update_server(time);
	return TRUE;
}
