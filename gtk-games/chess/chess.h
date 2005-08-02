/*
 * File: chess.h
 * Author: Ismael Orenstein
 * Project: GGZ Chess game module
 * Date: 03/01/01
 * Desc: Header file with game description / events / states
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

#define PROTOCOL_VERSION 6
/* Chess module design
 *
 * We should have 5 game states:
 * CHESS_STATE_INIT
 * CHESS_STATE_WAIT
 * CHESS_STATE_PLAYING
 * CHESS_STATE_DONE
 *
 * And we should have the following events
 * CHESS_EVENT_INIT
 * CHESS_EVENT_SEAT
 * CHESS_EVENT_PLAYERS
 * CHESS_EVENT_QUIT
 * CHESS_EVENT_MOVE_END
 * CHESS_EVENT_MOVE
 * CHESS_EVENT_TIME_REQUEST
 * CHESS_EVENT_TIME_OPTION
 * CHESS_EVENT_GAMEOVER
 * CHESS_EVENT_START
 * CHESS_EVENT_UPDATE_TIME
 * CHESS_EVENT_REQUEST_UPDATE
 * CHESS_EVENT_FLAG
 * CHESS_EVENT_DRAW
 *
 * The events and states work like this:
 *
 * CHESS_STATE_INIT
 * 
 *  Load the bitmaps, configure the pixmap buffers, connect to the game server.
 *  Issue an CHESS_EVENT_INIT and go to WAIT state
 *
 * CHESS_STATE_WAIT
 *
 *  Get the seat, players and time option. Wait until a EVENT_START
 *
 * CHESS_STATE_PLAYING
 *
 *  Play the game. Receive UPDATE_TIME, REQUEST_UPDATE, GAMOEVER, etc events.
 *
 * CHESS_STATE_DONE
 *
 *  The game is over, there is nothing left now.
 *
 * The events are hooked up this way:
 *
 * CHESS_EVENT_SEAT           -> When we receive a MSG_SEAT
 * CHESS_EVENT_PLAYERS        -> When we receive a MSG_PLAYERS
 * CHESS_EVENT_QUIT           -> When the user exits the game
 * CHESS_EVENT_MOVE_END       -> When the user finishes dragging a piece
 * CHESS_EVENT_MOVE           -> When the server sends us a move
 * CHESS_EVENT_TIME_REQUEST   -> When we receive a REQ_TIME (ie, the server is
 *                               asking our time option)
 * CHESS_EVENT_TIME_OPTION    -> When we receive a RSP_TIME (ie, we now know the
 *                               time option)
 * CHESS_EVENT_GAMEOVER       -> When we receive a MSG_GAMEOVER
 * CHESS_EVENT_START          -> When we receive a MSG_START
 * CHESS_EVENT_UPDATE_TIME    -> When we receive a RSP_UPDATE
 * CHESS_EVENT_REQUEST_UPDATE -> When the user requests a time update
 * CHESS_EVENT_FLAG           -> When the user calls a flag
 * CHESS_EVENT_DRAW           -> When the server asks if we want to draw the
 *                               game
 */

/* Definition of states */
#define CHESS_STATE_INIT 0
#define CHESS_STATE_WAIT 1
#define CHESS_STATE_PLAYING 2
#define CHESS_STATE_DONE 3

/* Definition of events */
#define CHESS_EVENT_SEAT 0
#define CHESS_EVENT_PLAYERS 1
#define CHESS_EVENT_QUIT 2
#define CHESS_EVENT_INIT 3
#define CHESS_EVENT_MOVE_END 4
#define CHESS_EVENT_MOVE 5
#define CHESS_EVENT_TIME_REQUEST 6
#define CHESS_EVENT_TIME_OPTION 7
#define CHESS_EVENT_GAMEOVER 8
#define CHESS_EVENT_START 9
#define CHESS_EVENT_UPDATE_TIME 10
#define CHESS_EVENT_REQUEST_UPDATE 11
#define CHESS_EVENT_FLAG 12
#define CHESS_EVENT_DRAW 13

/* Definition of messages */
#define CHESS_MSG_SEAT 1
#define CHESS_MSG_PLAYERS 2
#define CHESS_REQ_TIME 3
#define CHESS_RSP_TIME 4
#define CHESS_MSG_START 5
#define CHESS_REQ_MOVE 6
#define CHESS_MSG_MOVE 7
#define CHESS_MSG_GAMEOVER 8
#define CHESS_REQ_UPDATE 9
#define CHESS_RSP_UPDATE 10
#define CHESS_MSG_UPDATE 11
#define CHESS_REQ_FLAG 12
#define CHESS_REQ_DRAW 13

/* Clock types */
#define CHESS_CLOCK_NOCLOCK 0
#define CHESS_CLOCK_CLIENT 1
#define CHESS_CLOCK_SERVERLAG 2
#define CHESS_CLOCK_SERVER 3

/* Game over types */
#define CHESS_GAMEOVER_DRAW_AGREEMENT 1
#define CHESS_GAMEOVER_DRAW_STALEMATE 2
#define CHESS_GAMEOVER_DRAW_POSREP 3
#define CHESS_GAMEOVER_DRAW_MATERIAL 4
#define CHESS_GAMEOVER_DRAW_MOVECOUNT 5
#define CHESS_GAMEOVER_DRAW_TIMEMATERIAL 6
#define CHESS_GAMEOVER_WIN_1_MATE 7
#define CHESS_GAMEOVER_WIN_1_RESIGN 8
#define CHESS_GAMEOVER_WIN_1_FLAG 9
#define CHESS_GAMEOVER_WIN_2_MATE 10
#define CHESS_GAMEOVER_WIN_2_RESIGN 11
#define CHESS_GAMEOVER_WIN_2_FLAG 12

#include <glib.h>

/* Game info structure */
struct chess_info {
	/* Clock type 
	 * 0 -> No clock
	 * 1 -> Client
	 * 2 -> Server + Lag
	 * 3 -> Server*/
	char clock_type;
	/* Number of seconds for each player */
	int seconds[2];
	int t_seconds[2];	/* Only for the timer */
	/* Current state */
	char state;
	/* Current turn */
	int turn;
	/* Game fd */
	int fd;
	/* Current seat */
	char seat;
	/* Server's protocol version */
	char version;
	/* Hold information about the players */
	char assign[2];
	char name[2][18];
	/* Last moves */
	char dest_x, dest_y;
	char src_x, src_y;
	/* Am I in check? */
	char check;
	/* Timer for client clock */
	GTimer *timer;
};
