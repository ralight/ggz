/*
 * File: game.h
 * Author: Ismael Orenstein
 * Project: GGZ Reversi game module
 * Date: 09/17/2000
 * Desc: Description of game variables
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

#define BLACK -1
#define WHITE +1
#define EMPTY  0

/* REVERSI PROTOCOL VERSION 0.0.4
 *
 * The server handles everything, from the AI to the validity of moves
 *
 * RVR_STATE_INIT:
 *
 * First, it fetchs from the ggz_server how much seats there are, and how much are open/closed
 *
 * RVR_STATE_JOIN:
 *
 * Then, it waits until some player join . When he does, it sends:
 *
 * To Joining player:
 * 	RVR_MSG_SEAT (int)SEAT_NUM
 * To All Human players:
 * 	RVR_MSG_PLAYERS PLAYER_IDENTIFER[BLACK] PLAYER_IDENTIFIER[WHITE]
 *  where PLAYER_IDENTIFIER is:
 *  	(int)PLAYER_TYPE
 *  	if PLAYER_TYPE == GGZ_SEAT_OPEN
 *  		(str)PLAYER_NAME
 *
 * If the game was already happening, send the current game state to the new player
 *
 * 	RVR_MSG_SYNC (char)TURN BOARD_STATE
 * 	where TURN is the current turn, and:
 * 		BOARD_STATE = (char)POSITION * 64
 *
 * When all the open seats are filled, it enters playing state (state = RVR_STATE_PLAYING) and sends:
 *
 * To All Human players:
 * 	RVR_MSG_START
 *
 * RVR_STATE_PLAYING:
 *
 * If current player is AI:
 *
 * 	doMove(AI_MOVE());
 *
 * If it isn't AI, then it waits until client sends a RVR_REQ_MOVE
 *
 * When client sends a RVR_REQ_MOVE, the server must check if he is the current player and
 * if the move is valid. (The client should do it too, must the server must recheck to avoid cheating)
 * Then the server executes the move, and sends:
 * 
 * To All Human players:
 *
 *	RVR_MSG_MOVE (int)MOVE
 * 	where MOVE can be a array index (0-63) or one of the following error values:
 * 		RVR_ERROR_WRONGTURN
 * 			The player tried to move when it wasn't his turn
 * 		RVR_ERROR_INVALIDMOVE
 * 			The player tried a invalid move (Cheating sign!)
 *
 * Then the server checks if the game is over (No more valid moves) and who has won. If the game is over,
 * it sends to All Human players:
 *
 * 	RVR_MSG_GAMEOVER (int)WINNER
 * 		where WINNER can be: BLACK, WHITE, EMPTY(draw)
 *
 * If some player left while the game was happening, it sends to all Human players:
 *
 * 	RVR_MSG_PLAYERS
 *
 * And it will wait until someone returns
 *
 * The client:
 *
 * game_init():
 * - Init variables and set the turn to BLACK (first turn) and the state to init
 *
 * RVR_MSG_SEAT
 * - Store his seat on a safe variable
 *
 * RVR_MSG_PLAYERS
 * - Store (and display) the name of the players
 * - Set the state to wait
 *
 * RVR_MSG_START
 * - Set the state to playing
 *
 * RVR_MSG_SYNC
 * - Believes what the server says and updates everything
 *
 * RVR_MSG_MOVE
 * - Do the move
 *
 * RVR_MSG_GAMEOVER
 * - Display ending message
 * 
 * And sends to the server:
 *
 * RVR_REQ_MOVE
 * - Ask to move
 *
 * RVR_REQ_SYNC
 * - Ask for sync
 *
 * Luckily that's all, this protocol is perfect and it shouldn't be modified anymore. Or so I hope. :) */

// Reversi protocol
// The numbers aren't on order, because I used the same constants from TicTacToe - simplify testing
#define RVR_MSG_SEAT 0
#define RVR_MSG_PLAYERS 1
#define RVR_MSG_SYNC 6
#define RVR_REQ_SYNC 7
#define RVR_MSG_START 5
#define RVR_MSG_MOVE 2
#define RVR_MSG_GAMEOVER 3
#define RVR_REQ_MOVE 4

// States
#define RVR_STATE_INIT 0
#define RVR_STATE_WAIT 1
#define RVR_STATE_PLAYING 2
#define RVR_STATE_DONE 3

// GGZ define
#define GGZ_SEAT_OPEN -1

// Responses from server
#define RVR_SERVER_ERROR -1
#define RVR_SERVER_OK 0
#define RVR_SERVER_JOIN 1
#define RVR_SERVER_LEFT 2
#define RVR_SERVER_QUIT 3

// Errors
#define RVR_ERROR_INVALIDMOVE -1
#define RVR_ERROR_WRONGTURN -2
#define RVR_ERROR_CANTMOVE -3

// Takes cartesian coordinates and transform them into a array index
#define CART(X,Y) ( (Y-1)*8+(X-1) )

// Takes a index and transform it in cartesians coordinates
#define X(I) ( (I%8)+1 )
#define Y(I) ( (I/8)+1 )

// See what's the value at that place (like a 10x10 board)
#define GET(X,Y) ( (X==0 || Y==0 || X>8 || Y>8) ? EMPTY : game.board[CART(X,Y)] )

// Takes a seat and transform into a player code
#define SEAT2PLAYER(seat) ( (seat==0)?BLACK:WHITE )

// Takes a player code and transform into a seat index
#define PLAYER2SEAT(player) ( (player==BLACK)?0:1 )


struct game_state_t {
	/* Connection info */
	int fd;
	int num;
	int seats[2];
	char names[2][17];

	/* Game info */
	// Board
	char board[64];
	// Score
	int black;
	int white;
	// Last move
	int last_move;
	// State
	char state;
	// Turn
	char turn;
};

// Setup functions
void game_init();
void ggz_connect();
void game_handle_io(gpointer data, gint fd, GdkInputCondition cond);   

// Get stuff from server
int get_seat();
int get_players();
int get_sync();
int get_move();
int get_gameover();

// Send stuff to server
void send_my_move(int);
int request_sync();

// Game functions
void game_make_move(int, int);
void game_mark_board(int, int, int, int, int);
int game_check_direction(int, int, int, int, int);
int game_check_move(int, int);
void game_update_scores();


// Graphic functions
GtkWidget* create_main_win ();  
void display_board();
void game_status( const char *, ...);
void draw_bg(GtkWidget *);
