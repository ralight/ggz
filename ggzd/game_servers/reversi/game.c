/*
 * File: game.c
 * Author: Ismael Orenstein
 * Project: GGZ Reversi game module
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

#include "ggz.h"
#include "game.h"
#include "protocols.h"
#include <easysock.h>

#include <time.h>
#include <stdlib.h>

// Global game variables
struct rvr_game_t rvr_game;

// Initializes everything
void game_init() {
	int i;
	rvr_game.black = 2;
	rvr_game.white = 2;
	rvr_game.state = RVR_STATE_INIT;
	rvr_game.turn = EMPTY;
	for (i = 0; i < 64; i++) {
		rvr_game.board[i] = EMPTY;
	}
	// Initial positions
	rvr_game.board[CART(4,4)] = WHITE;
	rvr_game.board[CART(5,5)] = WHITE;
	rvr_game.board[CART(5,4)] = BLACK;
	rvr_game.board[CART(4,5)] = BLACK;
	// Inits random number generator
	srand(time(0));
}

// Handle server messages
int game_handle_ggz(int ggz_fd, int *p_fd) {
	int op, seat, status = RVR_SERVER_ERROR;

	if (es_read_int(ggz_fd, &op) < 0)
		return RVR_SERVER_ERROR;

	switch (op) {
		case REQ_GAME_LAUNCH:
			// Check if it's the right time to launch the game and if ggz could do taht
			if (rvr_game.state == RVR_STATE_INIT && ggz_game_launch() == 0) {
				// That's great! Update the state
				// Now waiting for people to join
				rvr_game.state = RVR_STATE_WAIT;
				ggz_debug("Waiting for players");
			}
			status = RVR_SERVER_OK;
			break;
		case REQ_GAME_JOIN:
			// Check if it's the right time to join the game and if ggz could do that
			if (rvr_game.state == RVR_STATE_WAIT && ggz_player_join(&seat, p_fd) == 0) {
				// That's great!! Do stuff
				
				game_send_seat(seat);
				game_send_players();

				// Waiting for anyone?
				if (!ggz_seats_open()) {
					// Game already going on?
					if (rvr_game.turn == EMPTY)
						rvr_game.turn = BLACK;
					else
						game_send_sync(seat);
					// No! Let's start the game!
					game_start();
					game_play();
				}
			}
			status = RVR_SERVER_JOIN;
			break;
		case REQ_GAME_LEAVE:
			// User has left
			if (ggz_player_leave(&seat, p_fd) == 0) {
				if (rvr_game.state == RVR_STATE_PLAYING) {
					rvr_game.state = RVR_STATE_WAIT;
					game_send_players();
				}
			}
			status = RVR_SERVER_LEFT;
			break;
		case RSP_GAME_OVER:
			// This game is over
			status = RVR_SERVER_QUIT;
			break;
		default:
			// Problems!
			status = RVR_SERVER_ERROR;
			break;
	}

	return status;

}

int game_send_seat(int seat) {
	int fd = ggz_seats[seat].fd;

	ggz_debug("Sending player %d's his seat number\n", seat);

	if (es_write_int(fd, RVR_MSG_SEAT) < 0 || es_write_int(fd, seat) < 0) {
		ggz_debug("Couldn't send seat!\n");
		return -1;
	}

	return 0;
}

int game_send_players() {
	int i, j, fd;

	for (j = 0; j < ggz_seats_num(); j++) {
		if ( (fd = ggz_seats[j].fd) == -1 ) {
			ggz_debug("Bot seat\n");
			continue;
		}
		ggz_debug("Sending player list to player %d", j);

		if (es_write_int(fd, RVR_MSG_PLAYERS) < 0) {
			ggz_debug("Can't send player list!\n");
			return -1;
		}
	
		for (i = 0; i < ggz_seats_num(); i++) {
			if (es_write_int(fd, ggz_seats[i].assign) < 0)
				return -1;
			if (ggz_seats[i].assign != GGZ_SEAT_OPEN && es_write_string(fd, ggz_seats[i].name) < 0) {
				ggz_debug("Can't send player name!\n");
				return -1;
			}
		}
	}

	return 0;
}

int game_send_sync(int seat) {
	int i, fd = ggz_seats[seat].fd;

	ggz_debug("Handling sync for player %d", seat);

	// Send SYNC message and current turn
	
	if (es_write_int(fd, RVR_MSG_SYNC) < 0 || es_write_char(fd, rvr_game.turn) < 0)
		return -1;

	// Send current board state
	
	for (i = 0; i < 64; i++) {
		if (es_write_char(fd, rvr_game.board[i]) < 0)
			return -1;
	}

	// That's fine
	
	return 0;

}

int game_start() {

	int i, fd;
	
	// Start game variables
	rvr_game.state = RVR_STATE_PLAYING;

	// Sends out start message
	for (i = 0; i < ggz_seats_num(); i++) {
		fd = ggz_seats[i].fd;
		// Don't send anything if the player is a computer!
		if (fd == -1)
			continue;
		if (es_write_int(fd, RVR_MSG_START) < 0)
			return -1;
	}

	ggz_debug("Game has started!\n");

	return 0;
}

int game_handle_player(int seat) {
	int fd, op, move, status;

	fd = ggz_seats[seat].fd;

	if (es_read_int(fd, &op) < 0)
		return -1;

	switch (op) {

		case RVR_REQ_MOVE:
			status = game_handle_move(seat, &move);
			break;

		case RVR_REQ_SYNC:
			status = game_send_sync(seat);
			break;

		case RVR_REQ_AGAIN:
			status = game_play_again(seat);
			break;

		default:
			status = RVR_SERVER_ERROR;
			break;

	}

	return status;

}

int game_handle_move(int seat, int *move) {
	int fd = ggz_seats[seat].fd, status;

	ggz_debug("Handling move %d,%dfor player %d\n", X(*move), Y(*move), seat);
	
	// Get the move from the message
	if (es_read_int(fd, move) < 0)
		return -1;


	// Check if it's his turn
	if (SEAT2PLAYER(seat) != rvr_game.turn) {
		status = RVR_ERROR_WRONGTURN;
	} else {
		// Check if it's valid (status return a negative value if error, the move on sucess)
		status = game_check_move(SEAT2PLAYER(seat), *move);
	}

	// Make move if it's valid (also sends it to everyone);
	game_make_move(SEAT2PLAYER(seat), status);

	return status;

}

void game_play() {
	int move;
	int seat = PLAYER2SEAT(rvr_game.turn);

	// Is the AI?
	if (ggz_seats[seat].assign == GGZ_SEAT_BOT) {
		// Yes!
		move = game_bot_move(rvr_game.turn);
		game_make_move(rvr_game.turn, move);
	}

	// It's not! Wait until the player sends the message...
	
	return;
}

int game_bot_move(int player) {
	int i;

	// Returns a random move between 0 and 63
	i=(int) (64.0*rand()/(RAND_MAX+1.0));
	if (game_check_move(player, i) >= 0)
		return i;
	else
		return game_bot_move(player);
}

int game_check_move(int player, int move) {

	int x = X(move), y = Y(move), status = 0;
	
	// Check if it's right time
	if (rvr_game.state != RVR_STATE_PLAYING)
		return RVR_ERROR_WRONGTURN;

	// Check if out of bounds
	if (move < 0 || move > 63)
		return RVR_ERROR_INVALIDMOVE;

	// Check if duplicated
	if (rvr_game.board[move] != EMPTY)
		return RVR_ERROR_INVALIDMOVE;

	/* CHECK IF THE MOVE IS VALID */

	// Check if it's valid up
	status += game_check_direction(player, 0,-1, x, y);
	status += game_check_direction(player, 1,-1, x, y);
	status += game_check_direction(player, 1, 0, x, y);
	status += game_check_direction(player, 1, 1, x, y);
	status += game_check_direction(player, 0, 1, x, y);
	status += game_check_direction(player,-1, 1, x, y);
	status += game_check_direction(player,-1, 0, x, y);
	status += game_check_direction(player,-1,-1, x, y);
	
	if (status > 0)
		return move;
	else
		return RVR_ERROR_INVALIDMOVE;

}
	
int game_check_direction(int player, int vx, int vy, int x, int y) {
	int i, j;
	for (i = x+vx, j = y+vy; GET(i,j) == -player; i+=vx, j+=vy) {
		if (GET(i+vx,j+vy) == player)
			return 1;
	}
	return 0;
}

int game_make_move(int player, int move) {
	int x = X(move), y = Y(move);
	int status = 0;
	int a, fd;

	// Make the move
	if (move >= 0) {
		rvr_game.board[move] = player;

		// Now goes through all directions, marking the board
	
		status += game_mark_board(player, 0,-1, x, y);
		status += game_mark_board(player, 1,-1, x, y);
		status += game_mark_board(player, 1, 0, x, y);
		status += game_mark_board(player, 1, 1, x, y);
		status += game_mark_board(player, 0, 1, x, y);
		status += game_mark_board(player,-1, 1, x, y);
		status += game_mark_board(player,-1, 0, x, y);
		status += game_mark_board(player,-1,-1, x, y);

	} else
		status = -1;

	for (a = 0; a < ggz_seats_num(); a++) {
		fd = ggz_seats[a].fd;
		if (fd == -1)
			continue;
		if (status > 0) {
			if (es_write_int(fd, RVR_MSG_MOVE) < 0 || es_write_int(fd, move) < 0)
				ggz_debug("Couldn't send message to player\n");
		} else {
			if (es_write_int(fd, RVR_MSG_MOVE) < 0 || es_write_int(fd, RVR_ERROR_INVALIDMOVE)< 0)
				ggz_debug("Couldn't send error message to player\n");
		}
		player*=-1;
	}

	// Change turn
	rvr_game.turn*=-1;

	game_update_scores();

	// Check if game is over
	if (!game_check_over()) {

		// AI play
		game_play();

	}

	// If couldn`t make the move, sends sync to the player
	if (status < 0)
			game_send_sync(PLAYER2SEAT(player));

	return status;

}

int game_check_over() {
	int i, status = 0;

	// Check if the current player can play
	for (i = 0; i < 64; i++) {
		if (game_check_move(rvr_game.turn, i) > 0) {
			status++;
			break;
		}
	}

	// If he can play, the game is not over
	if (status > 0)
		return 0;

	// He can't! Check if the opponent can
	status = 0;

	for (i = 0; i < 64; i++) {
		if (game_check_move(-rvr_game.turn, i) > 0) {
			status++;
			break;
		}
	}

	// If he can, then skip the players move.
	// If he can't, end the game
	
	if (status > 0)
		game_skip_move();
	else {
		game_gameover();
		return 1;
	}

	return 0;
}





int game_mark_board(int player, int vx, int vy, int x, int y) {
	int i, j;
	if (!game_check_direction(player, vx, vy, x, y))
		return 0;
	for (i = x+vx, j = y+vy; GET(i,j) == -player; i+=vx, j+=vy) {
		rvr_game.board[CART(i,j)]*=-1;
	}
	return 1;
}

void game_skip_move() {
	int seat;

	for (seat = 0; seat < ggz_seats_num(); seat++) {
		if (ggz_seats[seat].fd != -1) {
			if (es_write_int(ggz_seats[seat].fd, RVR_MSG_MOVE) < 0 || es_write_int(ggz_seats[seat].fd, RVR_ERROR_CANTMOVE) < 0)
				ggz_debug("Can't skip move");
		}
	}

	rvr_game.turn*=-1;

	return;

}

void game_gameover() {
	int seat, fd, winner;
	
	// Ends everything
	rvr_game.turn = EMPTY;
	rvr_game.state = RVR_STATE_DONE;

	if (rvr_game.black > rvr_game.white)
		winner = BLACK;
	else if (rvr_game.white > rvr_game.black)
		winner = WHITE;
	else
		winner = EMPTY;

	// Send message	
	for (seat = 0; seat < ggz_seats_num(); seat++) {
		fd = ggz_seats[seat].fd;
		if (fd == -1)
			continue;
		if (es_write_int(fd, RVR_MSG_GAMEOVER) < 0 || es_write_int(fd, winner) < 0)
			ggz_debug("Can't send gameover message");
	}
	
	// What to do now?
	// Puts human players score = 0
	// When they send a REQ_AGAIN message, put it = 1
	// When everyone equal = 1, then starts it all again
	ggz_debug("Game is over. Waiting to see if we should play again\n");
	rvr_game.white = (ggz_seats[PLAYER2SEAT(WHITE)].assign == GGZ_SEAT_BOT);
	rvr_game.black = (ggz_seats[PLAYER2SEAT(BLACK)].assign == GGZ_SEAT_BOT);
	
	return;

}

int game_play_again(int seat) {
	ggz_debug("%d wants to play again! He is a good fellow.\n", seat);
	// If game is not finished, forget about it
	if (rvr_game.state != RVR_STATE_DONE) {
		ggz_debug("The game wasn`t over yet! Are you crazy?\n");
		return RVR_SERVER_ERROR;
	}
	
	if (SEAT2PLAYER(seat) == WHITE)
		rvr_game.white = 1;
	else if (SEAT2PLAYER(seat) == BLACK)
		rvr_game.black = 1;
	
	if (rvr_game.white && rvr_game.black) {
		// Starts is again
		game_init();
		game_start();
		rvr_game.turn = BLACK;
		game_play();
	}


	return RVR_SERVER_OK;

}

void game_update_scores() {
	int i;
	rvr_game.white = 0;
	rvr_game.black = 0;
	for (i = 0; i < 64; i++) {
		if (rvr_game.board[i] == WHITE)
			rvr_game.white++;
		if (rvr_game.board[i] == BLACK)
			rvr_game.black++;
	}
}

