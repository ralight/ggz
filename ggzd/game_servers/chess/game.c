/*
 * File: game.c
 * Author: Ismael Orenstein
 * Project: GGZ Chess game module
 * Date: 03/01/01
 * Desc: Game main functions
 * $Id: game.c 6892 2005-01-25 04:09:21Z jdorje $
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

#include <libcgc/cgc.h>
#include <sys/time.h>
#include <stdlib.h>
#include <unistd.h>
#include <ggz.h>

#include <stdio.h> // test

#include "chess.h"
#include "game.h"
#include "ai.h"

/* Game from cgc */
game_t *game;
/* Game info from ggz */
struct chess_info game_info;
/* Chronometer
 * Use to store the time of the current play */
struct timeval chronometer;

static void game_restart_chronometer(void);
static int game_read_chronometer(void);

/* All the important stuff happens here */
static int game_update(int event_id, const void *data);

/* Send MSG_SEAT to the player */
static void game_send_seat(int seat);

/* Send MSG_PLAYERS to all the players */
static void game_send_players(void);

/* Send REQ_TIME */
static void game_request_time(int seat);

/* Send RSP_TIME */
static void game_send_time(int seat);

/* Send MSG_START to everyone */
static void game_send_start(void);

/* Send RSP_TIME to players */
static void game_send_update(void);

/* Send MSG_MOVE to everyone */
static void game_send_move(char *move, int time);

/* Send MSG_GAMEOVER to everyone */
static void game_send_gameover(char code);

/* Send REQ_DRAW */
static void game_send_draw(int seat);


/* Lazy way of doing things */
#define OUT_OF_TIME(a) (game_info.seconds[a] <= 0)

/* Translates a GGZ state event into a chess game event. */
void game_handle_ggz_state(GGZdMod *ggz,
			   GGZdModEvent event, const void *data)
{
  const GGZdModState *old_state = data;
  GGZdModState new_state = ggzdmod_get_state(ggz);

  if (*old_state == GGZDMOD_STATE_CREATED)
    game_update(CHESS_EVENT_LAUNCH, NULL);

  if (new_state == GGZDMOD_STATE_DONE);
    game_update(CHESS_EVENT_QUIT, NULL);
}

/* Translates a GGZ join event into a chess game event. */
void game_handle_ggz_join(GGZdMod *ggz,
			  GGZdModEvent event, const void *data) {
  const GGZSeat *old_seat = data;

  game_update(CHESS_EVENT_JOIN, &old_seat->num);
}

/* Translates a GGZ leave event into a chess game event. */
void game_handle_ggz_leave(GGZdMod *ggz,
			   GGZdModEvent event, const void *data) {
  const GGZSeat *old_seat = data;

  game_update(CHESS_EVENT_LEAVE, &old_seat->num);
}

static int seats_full(void)
{
	return ggzdmod_count_seats(game_info.ggz, GGZ_SEAT_OPEN)
		+ ggzdmod_count_seats(game_info.ggz, GGZ_SEAT_RESERVED) == 0;
}

/* game_update(int event_id, void *data) 
 * event_id is a id of the CHESS_EVENT_* family
 * data should be;
 *  CHESS_EVENT_LAUNCH: NULL
 *  CHESS_EVENT_JOIN: (int)Seat of the joining player
 *  CHESS_EVENT_LEAVE: (int)Seat of the leaving player
 *  CHESS_EVENT_QUIT: NULL
 *  CHESS_EVENT_MOVE: 4 chars string with the move (cgc format) + padding to
 *                    complete int size + TIME the move took (if it's the case)
 *  CHESS_EVENT_TIME: the TIME_OPTION integer
 *  CHESS_EVENT_GAMEOVER: An char with the WINNER code
 *  CHESS_EVENT_UPDATE_TIME: Two ints: the seat number and the time
 *  CHESS_EVENT_REQUES_UPDATE: (int)seat of the requesting player
 *  CHESS_EVENT_FLAG: (int)time until now for move
 *  CHESS_EVENT_DRAW: (int)What to add to the draw char
 *  CHESS_EVENT_START: NULL */
static int game_update(int event_id, const void *data)
{
  int time, st;
  int from, to;
  int ret;
  char botmove[6];
  int color, seatnum;

  switch (event_id) {
    case CHESS_EVENT_LAUNCH:
      /* Check for current state */
      if (game_info.state != CHESS_STATE_INIT)
        break;
      /* Create a new game */
      game = cgc_create_game();
      /* Go to wait state */
      game_info.state = CHESS_STATE_WAIT;

      if (ggzdmod_count_seats(game_info.ggz, GGZ_SEAT_BOT) == 1) {
        seatnum = ((ggzdmod_get_seat(game_info.ggz, 0).type == GGZ_SEAT_BOT) ? 0 : 1);
        color = ((seatnum == 0) ? WHITE : BLACK);
        if (cgc_join_game(game, color) < 0)
          return -1;
        chess_ai_init((color == BLACK ? C_BLACK : C_WHITE), 2);
      }
      break;
    case CHESS_EVENT_JOIN:
      /* Check for current state */
      if (game_info.state != CHESS_STATE_WAIT)
        /* Doesn't this prevent leaving/returning during a game?  --JDS */
        break;
      ggzdmod_log(game_info.ggz, "New player!");
      /* Add to cgc */
      if (cgc_join_game(game, *(int*)data == 0 ? WHITE : BLACK) < 0)
        /* Boy, that's bad... */
        return -1;
      ggzdmod_log(game_info.ggz, "Protocol stuff");
      /* Send seat */
      game_send_seat(*(int *)data);
      /* Send players */
      game_send_players();
      /* Handle time request */
      if (game_info.host < 0) {
        /* Congratulations: You are the host! */
        game_info.host = *(int *)data;
        game_request_time(*(int *)data);
      } else if (game_info.clock_type != CHESS_CLOCK_UNSET) {
        /* We already have set the time */
        game_send_time(*(int *)data);
      }
      /* Should we start the game? */
      if (game_info.clock_type != CHESS_CLOCK_UNSET && seats_full())
        game_update(CHESS_EVENT_START, NULL);
      break;
    case CHESS_EVENT_TIME:
      /* Check for current state */
      if (game_info.state != CHESS_STATE_WAIT)
        break;
      /* Check if the time is valid */
      if (!data) {
        game_request_time(game_info.host);
        break;
      }
      /* Update the time structs */
      game_info.clock_type = *(int*)data >> 24;
      game_info.seconds[0] = *(int*)data & 0xFFFFFF;
      game_info.seconds[1] = *(int*)data & 0xFFFFFF;
      /* Send the time to everyone */
      if (ggzdmod_get_seat(game_info.ggz, 0).type == GGZ_SEAT_PLAYER)
        game_send_time(0);
      if (ggzdmod_get_seat(game_info.ggz, 1).type == GGZ_SEAT_PLAYER)
        game_send_time(1);
      /* Should we start the game? */
      if (game_info.clock_type != CHESS_CLOCK_UNSET && seats_full())
        game_update(CHESS_EVENT_START, NULL);
      break;
    case CHESS_EVENT_START:
      /* Check for current state */
      if (game_info.state != CHESS_STATE_WAIT)
        break;
      ggzdmod_set_state(game_info.ggz, GGZDMOD_STATE_PLAYING);
      /* Send start message! */
      game_send_start();
      /* Stop the chronometer if it's the case */
      if (game_info.clock_type == CHESS_CLOCK_SERVERLAG ||
          game_info.clock_type == CHESS_CLOCK_SERVER)
        game_restart_chronometer();
      /* Update state */
      game_info.state = CHESS_STATE_PLAYING;
      /* If bot is white (seat 1), start to move */
      seatnum = ((ggzdmod_get_seat(game_info.ggz, 0).type == GGZ_SEAT_BOT) ? 0 : 1);
      if ((ggzdmod_count_seats(game_info.ggz, GGZ_SEAT_BOT) == 1)
      && (game_info.turn % 2 == seatnum)) {
        ret = chess_ai_find((seatnum == 0 ? C_WHITE : C_BLACK), &from, &to);
        botmove[0] = from % 8 + 'A';
        botmove[1] = from / 8 + '1';
        botmove[2] = to % 8 + 'A';
        botmove[3] = to / 8 + '1';
        botmove[4] = 0;
        botmove[5] = 0;
        data = &botmove;
        game_update(CHESS_EVENT_MOVE, data);
      }
      break;
    case CHESS_EVENT_MOVE:
      ggz_debug(DEBUG_GAME, "** got move!\n");
      /* Check for state */
      if (game_info.state != CHESS_STATE_PLAYING)
        break;
      ggz_debug(DEBUG_GAME, "** handle move!\n");
      ggzdmod_log(game_info.ggz, "Move: %s", (char *)data);
      /* Try to make the move via cgc */
      if ( !data || (st = cgc_make_move(game, (char *)data)) < 0) {
        ggz_debug(DEBUG_GAME, "** cgc: not valid (%p): %d!\n", data, st);
        ggzdmod_log(game_info.ggz, "CGC status: %d", st);
        game_send_move(NULL, 0);
        break;
      }
      ggz_debug(DEBUG_GAME, "** move is valid!\n");
      from = ((char*)data)[0] - 65 + (((char*)data)[1] - 49) * 8;
      to = ((char*)data)[2] - 65 + (((char*)data)[3] - 49) * 8;
      ggz_debug(DEBUG_GAME, "** ai: execute %i->%i\n", from, to);
      ret = chess_ai_move(from, to, 1);
      ggz_debug(DEBUG_GAME, "** ai: executed with result %i\n", ret);
      chess_ai_output();

      /* Move was valid */
      if (game_info.clock_type == CHESS_CLOCK_NOCLOCK)
          game_send_move((char *)data, 0);
      else {
        /* Worry about time */
        time = *((int *)data + (6/sizeof(int)) + 1);
        /* Update the structures */
        if (!OUT_OF_TIME(game_info.turn %2))
          game_info.seconds[game_info.turn % 2] -= time;
        else
          game_info.seconds[game_info.turn % 2] = 0;
        /* If server, stop the chronometer */
        if (game_info.clock_type != CHESS_CLOCK_CLIENT)
          game_restart_chronometer();
        /* Send the move */
        game_send_move((char *)data, time);
      }
      /* Check if the game is over */
      switch (st) {
        case MATE:
          /* MATE! */
          if (game_info.turn % 2 == 0)
            st = CHESS_GAMEOVER_WIN_1_MATE;
          else
            st = CHESS_GAMEOVER_WIN_2_MATE;
          game_update(CHESS_EVENT_GAMEOVER, &st);
          break;
        case DRAW_STALE:
          st = CHESS_GAMEOVER_DRAW_STALEMATE;
          game_update(CHESS_EVENT_GAMEOVER, &st);
          break;
        case DRAW_MOVECOUNT:
          game_info.movecount++;
          game_info.posrep = 0;
          break;
        case DRAW_POSREP:
          game_info.posrep++;
          game_info.movecount = 0;
          break;
        default:
          game_info.movecount = 0;
          game_info.posrep = 0;
          break;
      }
      /* Check if we could draw the game */
      game_info.draw = 0;
      if (game_info.movecount >= 50) {
        st = 6;
        game_update(CHESS_EVENT_DRAW, &st);
      } else if (game_info.posrep >= 3) {
        st = 5;
        game_update(CHESS_EVENT_DRAW, &st);
      }
      game_info.turn++;
      ggz_debug(DEBUG_GAME, "** move was: %i/%i %i/%i\n",
        ((char*)data)[0] - 65, ((char*)data)[1] - 49,
        ((char*)data)[2] - 65, ((char*)data)[3] - 49);
      ggz_debug(DEBUG_GAME, "** next turn!\n");

      seatnum = ((ggzdmod_get_seat(game_info.ggz, 0).type == GGZ_SEAT_BOT) ? 0 : 1);
      if ((ggzdmod_count_seats(game_info.ggz, GGZ_SEAT_BOT) == 1)
      && (game_info.turn % 2 == seatnum)) {
        ggz_debug(DEBUG_GAME, "** now move chess bot!\n");
        ret = chess_ai_find((seatnum == 0 ? C_WHITE : C_BLACK), &from, &to);
        ggz_debug(DEBUG_GAME, "** would move from %i to %i! (valid: %i)\n", from, to, ret);
        botmove[0] = from % 8 + 'A';
        botmove[1] = from / 8 + '1';
        botmove[2] = to % 8 + 'A';
        botmove[3] = to / 8 + '1';
        botmove[4] = 0;
        botmove[5] = 0;
        data = &botmove;
        ggz_debug(DEBUG_GAME, "** that would be: %i/%i %i/%i\n",
          ((char*)data)[0] - 65, ((char*)data)[1] - 49,
          ((char*)data)[2] - 65, ((char*)data)[3] - 49);
        game_update(CHESS_EVENT_MOVE, data);
      }
      break;
    case CHESS_EVENT_UPDATE_TIME:
      if (game_info.state != CHESS_STATE_PLAYING)
        break;
      /* Change time struct, if we aren't out of time */
      if (!OUT_OF_TIME(*(int*)data))
        game_info.seconds[*(int*)data] -= *((unsigned int*)data+1);
      else
        game_info.seconds[*(int*)data] = 0;
      /* Trigger a REQUEST_UPDATE event */
      *(int*)data = (*(int*)data + 1) % 2;
      game_update(CHESS_EVENT_REQUEST_UPDATE, data);
      break;
    case CHESS_EVENT_REQUEST_UPDATE:
      if (game_info.state != CHESS_STATE_PLAYING)
        break;
      /* If we have server clock, then update the time */
      if (game_info.clock_type == CHESS_CLOCK_SERVER) {
	time = game_read_chronometer();
        game_restart_chronometer();
        game_info.seconds[game_info.turn%2] -= time;
      }
      if (OUT_OF_TIME(0))
        game_info.seconds[0] = 0;
      if (OUT_OF_TIME(1))
        game_info.seconds[1] = 0;
      /* Send the info to both players */
      game_send_update();
      break;
    case CHESS_EVENT_FLAG:
      if (game_info.state != CHESS_STATE_PLAYING)
        break;
      /* I know that the claiming player isn't out of time
       * ie, there is only one player out of time */
      if (game_info.seconds[0] - (((game_info.turn+1)%2)*(*(int*)data)) <= 0)
        st = CHESS_GAMEOVER_WIN_2_FLAG;
      else if (game_info.seconds[1] - ((game_info.turn%2)*(*(int*)data)) <= 0)
        st = CHESS_GAMEOVER_WIN_1_FLAG;
      else
        break;
      game_update(CHESS_EVENT_GAMEOVER, &st);
      break;
    case CHESS_EVENT_DRAW:
      /* Check state */
      if (game_info.state != CHESS_STATE_PLAYING)
        break;
      /* Should the game end? */
      if (game_info.draw + *(int*)data > 6) {
        if (game_info.draw + *(int*)data == 7) {
          /* Both players want to draw! */
          st = CHESS_GAMEOVER_DRAW_AGREEMENT;
          game_update(CHESS_EVENT_GAMEOVER, &st);
        } else if (game_info.draw == 5) {
          /* Repetition of position */
          st = CHESS_GAMEOVER_DRAW_POSREP;
          game_update(CHESS_EVENT_GAMEOVER, &st);
        } else if (game_info.draw == 6) {
          /* Move count */
          st = CHESS_GAMEOVER_DRAW_MOVECOUNT;
          game_update(CHESS_EVENT_GAMEOVER, &st);
        }
        break;
      }
      game_info.draw = *(int*)data;
      if (*(int*)data == 3)
        /* Ask player 2 */
        game_send_draw(1);
      else if (*(int*)data == 4)
        /* Ask player 1 */
        game_send_draw(0);
      else {
        /* Ask both players */
        game_send_draw(0);
        game_send_draw(1);
      }
      break;
    case CHESS_EVENT_GAMEOVER:
      game_info.state = CHESS_STATE_DONE;
      ggzdmod_log(game_info.ggz, "Game over!");
      game_send_gameover(*(char*)data);
      break;
    default:
      ggzdmod_log(game_info.ggz, "Unknown event!!!");
      break;
  }
  return 0;
}

/* game_handle_player(int id, int *seat)
 * Filters the data from the player
 * id -> GGZ_EVENT_PLAYER
 * seat -> Pointer holding the seat of the player that sent the data
 * Data should be filtered this way:
 *  CHESS_REQ_TIME   -> Check for validity, then CHESS_EVENT_TIME
 *  CHESS_REQ_MOVE   -> Check for validity, then CHESS_EVENT_MOVE
 *  CHESS_REQ_UPDATE -> Trigger CHESS_EVENT_REQUEST_UPDATE
 *  CHESS_MSG_UPDATE -> Check for the right clock, then trigger UPDATE_TIME
 *  CHESS_REQ_FLAG   -> Check for the right clock, then trigger EVENT_FLAG
 *  CHESS_REQ_DRAW   -> Check if he hasn't done it yet, then trigger EVENT_DRAW
 *  */
void game_handle_player_data(GGZdMod *ggz, GGZdModEvent id,
			     const void *seat_data)
{
  const int *seat = seat_data;
  int fd, time;
  char op;
  void *data;

  /* Is the seat valid? */
  fd = ggzdmod_get_seat(ggz, *seat).fd;
  if (fd < 0)
    return;

  ggzdmod_log(game_info.ggz, "Handling player");

  /* Get the opcode */
  if (ggz_read_char(fd, &op) < 0)
    return;

  ggzdmod_log(game_info.ggz, "Received opcode %d", op);

  /* What to do? */
  switch (op) {
    case CHESS_RSP_TIME:
      ggzdmod_log(game_info.ggz, "Player sent a RSP_TIME");
      if (ggz_read_int(fd, &time) < 0)
        return;
      /* Check if this player is the host */
      if (*seat != game_info.host)
        return;
      ggzdmod_log(game_info.ggz, "Player sent the following time: %d", time);
      /* Is it a valid time ? */
      if ((time >> 24) > 3) {
        game_update(CHESS_EVENT_TIME, NULL);
        return;
      }
      if ((time >> 24) > 0 && (time & 0xFFFFFF) <= 0) {
        game_update(CHESS_EVENT_TIME, NULL);
        return;
      }
      /* Ok, then trigger the event */
      game_update(CHESS_EVENT_TIME, &time);
      break;
    case CHESS_REQ_MOVE:
      ggzdmod_log(game_info.ggz, "Player sent a REQ_MOVE");
      if (game_info.clock_type == CHESS_CLOCK_NOCLOCK) {
        /* We don't use clocks! */
        data = malloc(sizeof(char) * 6);
        ggz_read_string(fd, data, 6);
        /*
        ggz_read_char(fd, &move[0]);
        ggz_read_char(fd, &move[1]);
        *(char *)data = (move[0]%8) + 65;
        *(char *)(data+1) = 49 + (move[0]/8);
        *(char *)(data+2) = (move[1]%8) + 65;
        *(char *)(data+3) = 49 + (move[1]/8);
        *(char *)(data+4) = 0;
        */
      } else {
        data = malloc(sizeof(int) * (2 + (6/sizeof(int))));
        ggz_read_string(fd, data, 6);
        /*
        ggz_read_char(fd, &move[0]);
        ggz_read_char(fd, &move[1]);
        *(char *)data = (move[0]%8) + 65;
        *(char *)(data+1) = 49 + (move[0]/8);
        *(char *)(data+2) = (move[1]%8) + 65;
        *(char *)(data+3) = 49 + (move[1]/8);
        *(char *)(data+4) = 0;
        */
        if (game_info.clock_type == CHESS_CLOCK_CLIENT) {
          /* Get the time */
          ggz_read_int(fd, &time);
          /* Now put it on the data buffer */
        } else {
          /* How much type did that player take ? */
          time = game_read_chronometer();
        }
        /* If first turn, there is no time! */
        if (game_info.turn == 0 || game_info.turn == 1)
          time = 0;
        ggzdmod_log(game_info.ggz, "Move: %s\tTime: %d", (char*)data, time);
        *((int *)data + (6/sizeof(int)) + 1) = time;
      }
      /* Check if correct turn */
      if (*seat == game_info.turn % 2)
        game_update(CHESS_EVENT_MOVE, data);
      else
        game_update(CHESS_EVENT_MOVE, NULL);
      break;
    case CHESS_REQ_UPDATE:
      game_update(CHESS_EVENT_REQUEST_UPDATE, seat);
      break;
    case CHESS_MSG_UPDATE:
      data = (int *)malloc(sizeof(int) * 2);
      *(int*)data = *seat;
      ggz_read_int(fd, (int*)data+1);
      /* Check if right clock type */
      if (game_info.clock_type != CHESS_CLOCK_CLIENT)
        break;
      /* Check if it's my turn */
      if (game_info.turn % 2 != *seat)
        break;
      game_update(CHESS_EVENT_UPDATE_TIME, data);
      break;
    case CHESS_REQ_FLAG:
      /* Check for the right clock */
      if (game_info.clock_type == CHESS_CLOCK_NOCLOCK)
        break;
      time = 0;
      /* If server clock, get the time until now*/
      if (game_info.clock_type == CHESS_CLOCK_SERVER ||
          game_info.clock_type == CHESS_CLOCK_SERVERLAG) {
        time = game_read_chronometer();
      }
      /* Check if this player can claim */
      if (game_info.turn % 2 == *seat && game_info.seconds[*seat] - time <= 0)
        break;
      /* Ok, so update it */
      game_update(CHESS_EVENT_FLAG, &time);
      break;
    case CHESS_REQ_DRAW:
      if (*seat == 0 && game_info.draw == 3)
        break;
      if (*seat == 1 && game_info.draw == 4)
        break;
      id = *seat+3;
      game_update(CHESS_EVENT_DRAW, &id);
      break;
  }
  return;
}

static void game_send_seat(int seat)
{
  int fd = ggzdmod_get_seat(game_info.ggz, seat).fd;
  if (fd < 0)
    return;
  if (ggz_write_char(fd, CHESS_MSG_SEAT) < 0 ||
      ggz_write_char(fd, (char)seat) < 0 ||
      ggz_write_char(fd, PROTOCOL_VERSION) < 0)
    return;
}

static void game_send_players(void)
{
	int i, j, fd;
	
	for (j = 0; j < 2; j++) {
		if ( (fd = ggzdmod_get_seat(game_info.ggz, j).fd) == -1)
			continue;

		ggzdmod_log(game_info.ggz, "Sending player list to player %d", j);

		if (ggz_write_char(fd, CHESS_MSG_PLAYERS) < 0)
			return;
	
		for (i = 0; i < 2; i++) {
			GGZSeat seat = ggzdmod_get_seat(game_info.ggz, i);
			if (ggz_write_char(fd, seat.type) < 0)
				return;
			if (seat.type != GGZ_SEAT_OPEN
			    /* FIXME: seat.name can be NULL! */
			    && ggz_write_string(fd, seat.name) < 0)
				return;
		}
	}
}

static void game_restart_chronometer(void)
{
  gettimeofday(&chronometer, NULL);
}

static int game_read_chronometer(void)
{
  struct timeval now;

  gettimeofday(&now, NULL);
  return now.tv_sec - chronometer.tv_sec;
}

static void game_request_time(int seat)
{
  int fd = ggzdmod_get_seat(game_info.ggz, seat).fd;

  if (fd < 0)
    return;

  ggz_write_char(fd, CHESS_REQ_TIME);
}

static void game_send_time(int seat)
{
  int fd = ggzdmod_get_seat(game_info.ggz, seat).fd;
  int clock_type = game_info.clock_type;
  int sec = game_info.seconds[0];

  if (fd < 0)
    return;

  if (ggz_write_char(fd, CHESS_RSP_TIME) < 0
      || ggz_write_int(fd, (clock_type << 24) + (sec & 0xFFFFFF)))
    return;

}

static void game_send_start(void)
{
  int fd, a;

  for (a = 0; a < 2; a++) {
    fd = ggzdmod_get_seat(game_info.ggz, a).fd;

    if (fd < 0)
      continue;

    ggz_write_char(fd, CHESS_MSG_START);
  }
}

static void game_send_move(char *move, int time)
{
  int fd, a;

  for (a = 0; a < 2; a++) {
    fd = ggzdmod_get_seat(game_info.ggz, a).fd;
    if (fd < 0)
      continue;

    /* Send MSG_MOVE */
    ggz_write_char(fd, CHESS_MSG_MOVE);

    /* Send MOVE */
    if (move)
      ggz_write_string(fd, move);
    else
      ggz_write_int(fd, 0);
    /*
    if (move) {
      ggz_write_char(fd, move[0]-65+(8*(move[1]-49)));
      ggz_write_char(fd, move[2]-65+(8*(move[3]-49)));
    }
    else
      ggz_write_char(fd, -1);
      */

    /* Send time, if not error */
    if (game_info.clock_type && move)
      ggz_write_int(fd, time);

  }


}

static void game_send_update(void)
{
  int fd, a;
  for (a = 0; a < 2; a++) {
    fd = ggzdmod_get_seat(game_info.ggz, a).fd;
    if (fd < 0)
      return;
    ggzdmod_log(game_info.ggz, "To player %d: %d and %d sec", a, game_info.seconds[0], game_info.seconds[1]);
    if (ggz_write_char(fd, CHESS_RSP_UPDATE) < 0 ||
        ggz_write_int(fd, game_info.seconds[0]) < 0 ||
        ggz_write_int(fd, game_info.seconds[1]) < 0)
      return;
  }
}

static void game_send_gameover(char code)
{
  int fd, a;
  GGZGameResult results[2];
  int winner = (code == CHESS_GAMEOVER_WIN_1_MATE) ? 0 : 1;

  /* Report results to GGZ. */
  results[winner] = GGZ_GAME_WIN;
  results[1 - winner] = GGZ_GAME_LOSS;
  ggzdmod_report_game(game_info.ggz, NULL, results, NULL);

  for (a = 0; a < 2; a++) {
    fd = ggzdmod_get_seat(game_info.ggz, a).fd;

    if (fd < 0)
      continue;

    ggz_write_char(fd, CHESS_MSG_GAMEOVER);
    ggz_write_char(fd, code);
  }
}

static void game_send_draw(int seat)
{
  int fd = ggzdmod_get_seat(game_info.ggz, seat).fd;

  if (fd < 0)
    return;

  ggz_write_char(fd, CHESS_REQ_DRAW);
}
