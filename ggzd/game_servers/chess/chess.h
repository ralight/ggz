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

#include "ggzdmod.h"

#define PROTOCOL_VERSION 6
/* Chess module design
 *
 * We should have 4 game states:
 * CHESS_STATE_INIT
 * CHESS_STATE_WAIT
 * CHESS_STATE_PLAYING
 * CHESS_STATE_DONE
 *
 * And we should have the following events
 * CHESS_EVENT_LAUNCH
 * CHESS_EVENT_JOIN
 * CHESS_EVENT_LEAVE
 * CHESS_EVENT_QUIT
 * CHESS_EVENT_MOVE
 * CHESS_EVENT_TIME
 * CHESS_EVENT_GAMEOVER
 * CHESS_EVENT_START
 * CHESS_EVENT_UPDATE_TIME
 * CHESS_EVENT_REQUEST_UPDATE
 * CHESS_EVENT_FLAG
 * CHESS_EVENT_DRAW
 *
 * Besides the GGZ events
 * GGZ_EVENT_LAUNCH
 * GGZ_EVENT_JOIN
 * GGZ_EVENT_LEAVE
 * GGZ_EVENT_QUIT
 * GGZ_EVENT_PLAYER
 *
 * The events, states and protocol messages work like this.
 *
 * First, we hook up the following events:
 * GGZ_EVENT_LAUNCH --> CHESS_EVENT_LAUNCH
 * GGZ_EVENT_JOIN   --> CHESS_EVENT_JOIN
 * GGZ_EVENT_LEAVE  --> CHESS_EVENT_LEAVE
 * GGZ_EVENT_QUIT   --> CHESS_EVENT_QUIT
 *
 * The GGZ_EVENT_PLAYER is hooked up to a a filter function, that can result
 * in one of the other events.
 * GGZ_EVENT_PLAYER --> CHESS_EVENT_MOVE
 *                      CHESS_EVENT_TIME
 *                      CHESS_EVENT_UPDATE_TIME
 *                      CHESS_EVENT_REQUEST_UPDATE
 *                      CHESS_EVENT_FLAG
 *                      CHESS_EVENT_DRAW
 *
 * Thats the way the states and events should work
 * CHESS_STATE_INIT:
 *    CHESS_EVENT_LAUNCH:
 *      Create a new game from cgc
 *      Go to WAIT state
 *  
 *
 * CHESS_STATE_WAIT:
 *    CHESS_EVENT_JOIN:
 *      Send CHESS_MSG_SEAT
 *      Send CHESS_MSG_PLAYERS
 *      Add the player to the cgc game
 *      If it's the first player to join, send
 *        CHESS_REQ_TIME
 *      If he is the second player and the time option is set, send
 *        CHESS_RSP_TIME
 *      If both players have joined and the time option is set, triggers
 *        CHESS_EVENT_START
 *  
 *    CHESS_EVENT_TIME:
 *      If the option is invalid, send
 *        CHESS_REQ_TIME
 *      If the option is valid and both players have joined, trigger
 *        CHESS_EVENT_START
 *      Update the time structs w/ the appropriated values
 *    
 *    CHESS_EVENT_START:
 *      Send CHESS_MSG_START
 *      If SERVER clock, stop the cronometer
 *      (ie, update the time struct with the current time)
 *      Go to PLAYING state
 * 
 *
 * CHESS_STATE_PLAYING:
 *    CHESS_EVENT_MOVE:
 *      Check if the move is valid.
 *      If it isn't, send
 *        CHESS_MSG_MOVE (str)NULL
 *        and return
 *      Make the move via cgc
 *      If the time is CLIENT
 *        Get the time
 *        Update the time structures
 *      If the time is SERVER
 *        Check how much time it took
 *        Update the time structures
 *        Stop the cronometer
 *      Send CHESS_MSG_MOVE
 *      Check for gameover
 *
 *    CHESS_EVENT_UPDATE_TIME:
 *      Check if it's client clock
 *      Updates the time structure for this player
 *      Next move, the time must be sent relative to this new time
 *      Also triggers a CHESS_EVENT_REQUEST_UPDATE to the other player
 *
 *    CHESS_EVENT_REQUEST_UPDATE:
 *      Send a CHESS_RSP_UPDATE
 *
 *    CHESS_EVENT_FLAG:
 *      Check if time is over.
 *      If it is, triggers EVENT_GAMEOVER
 *
 *    CHESS_EVENT_DRAW:
 *      Add to the draw char one of the values:
 *        Player 1 -> 3
 *        Player 2 -> 4
 *        POSREP   -> 5
 *        MOVCNT   -> 6
 *      If the draw char is > 6, draw the game.
 *      Else, send a message to the players asking if they want to
 *      draw it
 *
 *    CHESS_EVENT_GAMEOVER:
 *      Go to DONE state
 *
 *
 * The following is a description of the protocol messages:
 *
 *
 * CHESS_MSG_SEAT (char)SEAT (char)VERSION
 *
 * Sends the player his seat number and the protocol version
 *
 *
 * CHESS_MSG_PLAYERS (char)PLAYER1_TYPE [(str)PLAYER1_NAME] (char)PLAYER2_TYPE
 * [(str)PLAYER2_NAME]
 *
 * Sends the type of each player (AI or Human) and, if it's human, the players
 * name
 *
 * CHESS_REQ_TIME
 * CHESS_RSP_TIME (int)TIME_OPTION
 *
 * This messages are used to define what is goint to be the clock used. We
 * should have 4 clocks:
 *  Client clock                  (1)
 *  Server clock with lag meter   (2)
 *  Server clock without lag      (3)
 *  No clock                      (0)
 *
 * The TIME_OPTION is defined as CLOCK_TYPE<<24 + CLOCK_VALUE
 * CLOCK_TYPE is the type of the clock (like 1, 2, 3, 0)
 * CLOCK_VALUE is the number of seconds that each player will have
 *
 * CHESS_MSG_START
 *
 * Tells the players that the game has started
 *
 * CHESS_REQ_MOVE (str)MOVE [(int)TIME]
 *
 * Requests a move using cgc's notation.
 * If we are using the client clock, TIME is the number of seconds that it 
 * took to make that move.
 *
 * CHESS_MSG_MOVE (str)MOVE [(int)TIME]
 *
 * Sends a move. MOVE is in cgc's notation. If it's a invalid move, just send
 * MOVE = NULL. If we are using some type of
 * clock, TIME is the number of seconds that it took to make that move
 *
 * CHESS_REQ_UPDATE
 * CHESS_RSP_UPDATE (int)PLAYER_1_TIME (int)PLAYER_2_TIME
 *
 * Asks the server to update his time structure, and then sends it to both
 * players. If we are using SERVER_CLOCK, the server will see how much time it
 * take until now to make the move and will decrease the player's clock as
 * well. In the next move, the TIME part will be relative to the times that
 * were sent now.
 *
 * CHESS_REQ_FLAG
 *
 * Triggers EVENT_FLAG. Used by the clients to tell the server that the game
 * must end because of time (flag falling)
 *
 * CHESS_MSG_UPDATE (int)TIME
 *
 * Used by the client when using CLOCK_CLIENT, to send how much time it already
 * take to do that move. A client should send it when it's time is over, so
 * that the server can also notify the other player, so that he will take the 
 * appropriated action.
 *
 * CHESS_REQ_DRAW
 *
 * Used by the server and the client to ask for a draw
 *
 * CHESS_MSG_GAMEOVER (char)WINNER
 *
 * The game is over. The WINNER is one of the winning codes below */

/* Definition of states */
#define CHESS_STATE_INIT 0
#define CHESS_STATE_WAIT 1
#define CHESS_STATE_PLAYING 2
#define CHESS_STATE_DONE 3

/* Definition of events */
/* NOTE: These used to be defined as GGZ events, but that seemed
   quite unsafe so I changed them to just be regular integers. --JDS */
#define CHESS_EVENT_LAUNCH 0
#define CHESS_EVENT_JOIN 1
#define CHESS_EVENT_LEAVE 2
#define CHESS_EVENT_QUIT 3
#define CHESS_EVENT_MOVE 4
#define CHESS_EVENT_TIME 5
#define CHESS_EVENT_GAMEOVER 6
#define CHESS_EVENT_START 7
#define CHESS_EVENT_UPDATE_TIME 8
#define CHESS_EVENT_REQUEST_UPDATE 9
#define CHESS_EVENT_FLAG 10
#define CHESS_EVENT_DRAW 11

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
enum Chess_Clock_Type {
  CHESS_CLOCK_UNSET = -1,
  CHESS_CLOCK_NOCLOCK = 0,
  CHESS_CLOCK_CLIENT,
  CHESS_CLOCK_SERVERLAG,
  CHESS_CLOCK_SERVER
};

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

/* Game info structure */
struct chess_info {
  /* GGZ data object */
  GGZdMod *ggz;

  /* Clock type.  See the enumeration. */
  enum Chess_Clock_Type clock_type;

  /* Number of seconds for each player */
  int seconds[2];

  /* Current state */
  char state;

  /* Host player */
  char host;

  /* Current turn */
  int turn;

  /* For keeping care of draws */
  char movecount;
  char posrep;
  char draw;
};
