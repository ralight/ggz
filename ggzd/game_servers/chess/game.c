/*
 * File: game.c
 * Author: Ismael Orenstein
 * Project: GGZ Chess game module
 * Date: 03/01/01
 * Desc: Game main functions
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
#include <libcgc/cgc.h>
#include <chess.h>
#include <game.h>
#include <stdlib.h>
#include <easysock.h>
#include <sys/time.h>
#include <unistd.h>

/* Game from cgc */
game_t *game;
/* Game info from ggz */
struct chess_info game_info;
/* Cronometer
 * Use to store the time of the current play */
struct timeval cronometer;

/* Lazy way of doing things */
#define OUT_OF_TIME (game_info.seconds[0] <= 0 || game_info.seconds[1] <= 0)

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
 *  CHESS_EVENT_FLAG: NULL
 *  CHESS_EVENT_START: NULL */
void game_update(int event_id, void *data) {
  int time, st;
  switch (event_id) {
    case CHESS_EVENT_LAUNCH:
      /* Check for current state */
      if (game_info.state != CHESS_STATE_INIT)
        break;
      /* Create a new game */
      game = cgc_create_game();
      /* Go to wait state */
      game_info.state = CHESS_STATE_WAIT;
      break;
    case CHESS_EVENT_JOIN:
      /* Check for current state */
      if (game_info.state != CHESS_STATE_WAIT)
        break;
      ggz_debug("New player!");
      /* Add to cgc */
      if (cgc_join_game(game, *(int*)data == 0 ? WHITE : BLACK) < 0)
        /* Boy, that's bad... */
        return;
      ggz_debug("Protocol stuff");
      /* Send seat */
      game_send_seat(*(int *)data);
      /* Send players */
      game_send_players();
      /* Handle time request */
      if (game_info.host < 0) {
        /* Congratulations: You are the host! */
        game_info.host = *(int *)data;
        game_request_time(*(int *)data);
      } else if (game_info.clock_type >= 0) {
        /* We already have set the time */
        game_send_time(*(int *)data);
      }
      /* Should we start the game? */
      if (game_info.clock_type >= 0 && ggz_seats_open() == 0)
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
      if (ggz_seats[0].assign == GGZ_SEAT_PLAYER)
        game_send_time(0);
      if (ggz_seats[1].assign == GGZ_SEAT_PLAYER)
        game_send_time(1);
      /* Should we start the game? */
      if (game_info.clock_type >= 0 && ggz_seats_open() == 0)
        game_update(CHESS_EVENT_START, NULL);
      break;
    case CHESS_EVENT_START:
      /* Check for current state */
      if (game_info.state != CHESS_STATE_WAIT)
        break;
      /* Send start message! */
      game_send_start();
      /* Stop the cronometer if it's the case */
      if (game_info.clock_type == CHESS_CLOCK_SERVERLAG ||
          game_info.clock_type == CHESS_CLOCK_SERVER)
        game_stop_cronometer();
      /* Update state */
      game_info.state = CHESS_STATE_PLAYING;
      break;
    case CHESS_EVENT_MOVE:
      /* Check for state */
      if (game_info.state != CHESS_STATE_PLAYING)
        break;
      ggz_debug("Move: %s", (char *)data);
      /* Try to make the move via cgc */
      if ( !data || (st = cgc_make_move(game, (char *)data)) < 0) {
        ggz_debug("CGC status: %d", st);
        game_send_move(NULL, 0);
        break;
      }
      /* Move was valid */
      if (game_info.clock_type == CHESS_CLOCK_NOCLOCK)
          game_send_move((char *)data, 0);
      else {
        /* Worry about time */
        time = *((int *)data + (5/sizeof(int)) + 1);
        /* Update the structures */
        if (!OUT_OF_TIME)
          game_info.seconds[game_info.turn % 2] -= time;
        /* If server, stop the cronometer */
        if (game_info.clock_type != CHESS_CLOCK_CLIENT)
          game_stop_cronometer();
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
      /* Update the turn */
      game_info.turn++;
      break;
    case CHESS_EVENT_UPDATE_TIME:
      if (game_info.state != CHESS_STATE_PLAYING)
        break;
      /* Change time struct, if we aren't out of time */
      if (!OUT_OF_TIME)
        game_info.seconds[*(int*)data] -= *((unsigned int*)data+1);
      /* Trigger a REQUEST_UPDATE event */
      *(int*)data = (*(int*)data + 1) % 2;
      game_update(CHESS_EVENT_REQUEST_UPDATE, data);
      break;
    case CHESS_EVENT_REQUEST_UPDATE:
      if (game_info.state != CHESS_STATE_PLAYING)
        break;
      /* Send the info to the player */
      game_send_update(*(int*)data);
      break;
    case CHESS_EVENT_FLAG:
      if (game_info.state != CHESS_STATE_PLAYING)
        break;
      /* Check if player 1 and 2 are out of time */
      if (game_info.seconds[0] <= 0 && game_info.seconds[1] <= 0) {
        /* The one with more time wins */
        if (game_info.seconds[0] > game_info.seconds[1])
          st = CHESS_GAMEOVER_WIN_1_FLAG;
        else if (game_info.seconds[1] > game_info.seconds[0])
          st = CHESS_GAMEOVER_WIN_2_FLAG;
        game_update(CHESS_EVENT_GAMEOVER, &st);
      } else if (game_info.seconds[0] <= 0) {
        /* Only player 1 is out of time */
        st = CHESS_GAMEOVER_WIN_2_FLAG;
        game_update(CHESS_EVENT_GAMEOVER, &st);
      } else if (game_info.seconds[1] <= 0) {
        /* Only player 2 is out of time */
        st = CHESS_GAMEOVER_WIN_1_FLAG;
        game_update(CHESS_EVENT_GAMEOVER, &st);
      }
      break;
    case CHESS_EVENT_GAMEOVER:
      game_info.state = CHESS_STATE_DONE;
      ggz_debug("Game over!");
      game_send_gameover(*(char*)data);
      break;
    default:
      ggz_debug("Unknown event!!!");
      break;
  }
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
 *  CHESS_REQ_FLAG   -> Check for the right clock, then trigger EVENT_FLAG */
void game_handle_player(int id, int *seat) {
  int fd, time;
  char op;
  char move[2];
  void *data;
  struct timeval now;

  /* Is the seat valid? */
  fd = ggz_seats[*seat].fd;
  if (fd < 0)
    return;

  ggz_debug("Handling player");

  /* Get the opcode */
  if (es_read_char(fd, &op) < 0)
    return;

  ggz_debug("Received opcode %d", op);

  /* What to do? */
  switch (op) {
    case CHESS_RSP_TIME:
      ggz_debug("Player sent a RSP_TIME");
      if (es_read_int(fd, &time) < 0)
        return;
      /* Check if this player is the host */
      if (*seat != game_info.host)
        return;
      ggz_debug("Player sent the following time: %d", time);
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
      ggz_debug("Player sent a REQ_MOVE");
      if (game_info.clock_type == CHESS_CLOCK_NOCLOCK) {
        /* We don't use clocks! */
        data = malloc(sizeof(char) * 5);
        es_read_char(fd, &move[0]);
        es_read_char(fd, &move[1]);
        *(char *)data = (move[0]%8) + 65;
        *(char *)(data+1) = 56 - (move[0]/8);
        *(char *)(data+2) = (move[1]%8) + 65;
        *(char *)(data+3) = 56 - (move[1]/8);
        *(char *)(data+4) = 0;
      } else {
        data = malloc(sizeof(int) * (2 + (5/sizeof(int))));
        es_read_char(fd, &move[0]);
        es_read_char(fd, &move[1]);
        *(char *)data = (move[0]%8) + 65;
        *(char *)(data+1) = 56 - (move[0]/8);
        *(char *)(data+2) = (move[1]%8) + 65;
        *(char *)(data+3) = 56 - (move[1]/8);
        *(char *)(data+4) = 0;
        if (game_info.clock_type == CHESS_CLOCK_CLIENT) {
          /* Get the time */
          es_read_int(fd, &time);
          /* Now put it on the data buffer */
        } else {
          /* How much type did that player take ? */
          gettimeofday(&now, NULL);
          time = now.tv_sec - cronometer.tv_sec;
        }
        ggz_debug("Move: %s\tTime: %d", data, time);
        *((int *)data + (5/sizeof(int)) + 1) = time;
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
      es_read_int(fd, (int*)data+1);
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
      game_update(CHESS_EVENT_FLAG, NULL);
      break;
  }
}

void game_send_seat(int seat) {
  int fd = ggz_seats[seat].fd;
  if (fd < 0)
    return;
  if (es_write_char(fd, CHESS_MSG_SEAT) < 0 ||
      es_write_char(fd, (char)seat) < 0 ||
      es_write_char(fd, PROTOCOL_VERSION) < 0)
    return;
}

void game_send_players() {
	int i, j, fd;
	
	for (j = 0; j < 2; j++) {
		if ( (fd = ggz_seats[j].fd) == -1)
			continue;

		ggz_debug("Sending player list to player %d", j);

		if (es_write_char(fd, CHESS_MSG_PLAYERS) < 0)
			return;
	
		for (i = 0; i < 2; i++) {
			if (es_write_char(fd, ggz_seats[i].assign) < 0)
				return;
			if (ggz_seats[i].assign != GGZ_SEAT_OPEN
			    && es_write_string(fd, ggz_seats[i].name) < 0) 
				return;
		}
	}
}

void game_stop_cronometer() {
  gettimeofday(&cronometer, NULL);
}

void game_request_time(int seat) {
  int fd = ggz_seats[seat].fd;

  if (fd < 0)
    return;

  es_write_char(fd, CHESS_REQ_TIME);
}

void game_send_time(int seat) {
  int fd = ggz_seats[seat].fd;

  if (fd < 0)
    return;

  if (es_write_char(fd, CHESS_RSP_TIME) < 0 ||
      es_write_int(fd, (game_info.clock_type<<24)+(game_info.seconds[0]&0xFFFFFF)))
    return;

}

void game_send_start() {
  int fd, a;

  for (a = 0; a < 2; a++) {
    fd = ggz_seats[a].fd;

    if (fd < 0)
      continue;

    es_write_char(fd, CHESS_MSG_START);
  }
}

void game_send_move(char *move, int time) {
  int fd, a;

  for (a = 0; a < 2; a++) {
    fd = ggz_seats[a].fd;
    if (fd < 0)
      continue;

    /* Send MSG_MOVE */
    es_write_char(fd, CHESS_MSG_MOVE);

    /* Send MOVE */
    if (move) {
      es_write_char(fd, move[0]-'A'+(8*(56-move[1])));
      es_write_char(fd, move[2]-'A'+(8*(56-move[3])));
    }
    else
      es_write_char(fd, -1);
    
    /* Send time */
    if (time)
      es_write_int(fd, time);

  }


}

void game_send_update(int seat) {
  int fd = ggz_seats[seat].fd;
  if (fd < 0)
    return;
  ggz_debug("To player %d: %d and %d sec", seat, game_info.seconds[0], game_info.seconds[1]);
  if (es_write_char(fd, CHESS_RSP_UPDATE) < 0 ||
      es_write_int(fd, game_info.seconds[0]) < 0 ||
      es_write_int(fd, game_info.seconds[1]) < 0)
    return;
}

void game_send_gameover(char code) {
  int fd, a;

  for (a = 0; a < 2; a++) {
    fd = ggz_seats[a].fd;

    if (fd < 0)
      continue;

    es_write_char(fd, CHESS_MSG_GAMEOVER);
    es_write_char(fd, code);
  }
}
