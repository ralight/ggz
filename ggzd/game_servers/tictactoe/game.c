/*
 * File: game.c
 * Author: Brent Hendricks
 * Project: GGZ Tic-Tac-Toe game module
 * Date: 3/31/00
 * Desc: Game functions
 * $Id: game.c 2839 2001-12-09 23:55:02Z bmh $
 *
 * Copyright (C) 2000 Brent Hendricks.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope thta it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */

#include <stdlib.h>
#include <string.h>

#include <easysock.h>

#include "game.h"

/* Global game variables */
struct ttt_game_t ttt_game;

/* Local utility functions */
static void game_rotate_board(char b[9]);

/* Setup game state and board */
void game_init(GGZdMod *ggzdmod)
{
	int i;
	
	ttt_game.turn = -1;
	ttt_game.move_count = 0;
	for (i = 0; i < 9; i++)
		ttt_game.board[i] = -1;
		
	ttt_game.ggz = ggzdmod;
}


/* Callback for GGZDMOD_EVENT_STATE */
void game_handle_state_event(GGZdMod *ggz, GGZdModEvent event, void *data)
{

	switch(ggzdmod_get_state(ggz)) {
	case GGZDMOD_STATE_PLAYING:
		/* If we're just starting, set for first players turn*/
		if (ttt_game.turn == -1)
			ttt_game.turn = 0;
		
		game_next_move();
	default:
		break;
	}
}


/* Callback for GGZDMOD_EVENT_JOIN */
void game_handle_join_event(GGZdMod *ggz, GGZdModEvent event, void *data)
{
	int seat;

	seat = *(int*)data;

	/* Send the info to our players */
	game_send_seat(seat);
	game_send_players();
	
	/* We start playing only when there are no open seats. */
	if (!ggzdmod_count_seats(ttt_game.ggz, GGZ_SEAT_OPEN)) {

		/* If we're continuing a game, send sync to new player */
		if (ttt_game.turn != -1)
			game_send_sync(seat);
		
		ggzdmod_set_state(ttt_game.ggz, GGZDMOD_STATE_PLAYING);
	}
}


/* Callback for GGZDMOD_EVENT_LEAVE */
void game_handle_leave_event(GGZdMod *ggz, GGZdModEvent event, void *data)
{
	if (ggzdmod_get_state(ggz) == GGZDMOD_STATE_PLAYING) {
		ggzdmod_set_state(ggz, GGZDMOD_STATE_WAITING);
		game_send_players();
	}

	/* If there are no players left, we'll end the game */
        if (ggzdmod_count_seats(ggz, GGZ_SEAT_PLAYER) == 0) {
		ggzdmod_log(ggz, "No players left: ending game");
		ggzdmod_set_state(ggz, GGZDMOD_STATE_DONE);
	}
}


/* Handle message from player */
void game_handle_player(GGZdMod *ggz, GGZdModEvent event, void *data)
{
	int num = *(int*)data;
	int fd, op, move;
	GGZSeat seat;

	seat = ggzdmod_get_seat(ggz, num);
	fd = seat.fd;
	
	if (es_read_int(fd, &op) < 0)
		return;

	switch (op) {

	case TTT_SND_MOVE:
		if (game_handle_move(num, &move) == 0)
			game_do_move(move);
		break;
	case TTT_REQ_SYNC:
		game_send_sync(num);
		break;
	default:
		ggzdmod_log(ggz, "Unrecognized player opcode %d.", op);
	}
}


/* Send out seat assignment */
int game_send_seat(int num)
{
	GGZSeat seat;

	seat = ggzdmod_get_seat(ttt_game.ggz, num);
	ggzdmod_log(ttt_game.ggz, "Sending player %d's seat num", num);

	if (es_write_int(seat.fd, TTT_MSG_SEAT) < 0
	    || es_write_int(seat.fd, num) < 0)
		return -1;

	return 0;
}


/* Send out player list to everybody */
int game_send_players(void)
{
	int i, j, fd;
	GGZSeat seat;
	
	for (j = 0; j < 2; j++) {
		seat = ggzdmod_get_seat(ttt_game.ggz, j);
		fd = seat.fd;

		if (fd != -1) {
			ggzdmod_log(ttt_game.ggz, "Sending playerlist to player %d", j);

			if (es_write_int(fd, TTT_MSG_PLAYERS) < 0)
				return -1;
			
			for (i = 0; i < 2; i++) {
				seat = ggzdmod_get_seat(ttt_game.ggz, i);
				if (es_write_int(fd, seat.type) < 0)
					return -1;
				if (seat.type != GGZ_SEAT_OPEN
				    && es_write_string(fd, seat.name) < 0)
					return -1;
			}
		}
	}
	return 0;
}


/* Send out move for player: num */
int game_send_move(int num, int move)
{
	int opponent = (num + 1) % 2;
	GGZSeat seat = ggzdmod_get_seat(ttt_game.ggz, opponent);

	/* If player is a computer, don't need to send */
	if (seat.fd != -1) {
		
		ggzdmod_log(ttt_game.ggz, "Sending player %d's move to player %d", num,
			    opponent);

		if (es_write_int(seat.fd, TTT_MSG_MOVE) < 0
		    || es_write_int(seat.fd, move) < 0)
			return -1;
	}
	return 0;
}


/* Send out board layout */
int game_send_sync(int num)
{	
	int i;
	GGZSeat seat = ggzdmod_get_seat(ttt_game.ggz, num);

	ggzdmod_log(ttt_game.ggz, "Handling sync for player %d", num);

	if (es_write_int(seat.fd, TTT_SND_SYNC) < 0
	    || es_write_char(seat.fd, ttt_game.turn) < 0)
		return -1;
	
	for (i = 0; i < 9; i++)
		if (es_write_char(seat.fd, ttt_game.board[i]) < 0)
			return -1;
		
	return 0;
}
	

/* Send out game-over message */
int game_send_gameover(char winner)
{
	int i;
	GGZSeat seat;
	
	for (i = 0; i < 2; i++) {
		seat = ggzdmod_get_seat(ttt_game.ggz, i);
		if (seat.fd != -1) {
			ggzdmod_log(ttt_game.ggz, "Sending game-over to player %d", i);
			
			if (es_write_int(seat.fd, TTT_MSG_GAMEOVER) < 0
			    || es_write_char(seat.fd, winner) < 0)
				return -1;
		}
	}

	return 0;
}


/* Do the next move*/
int game_next_move(void)
{
	int move, num = ttt_game.turn;
	GGZSeat seat = ggzdmod_get_seat(ttt_game.ggz, num);
	
	if (seat.type == GGZ_SEAT_BOT) {
		move = game_bot_move(num);
		game_do_move(move);
	}
	else
		game_req_move(num);

	return 0;
}


/* Request move from current player */
int game_req_move(int num)
{
	GGZSeat seat = ggzdmod_get_seat(ttt_game.ggz, num);
	
	ggzdmod_log(ttt_game.ggz, "Requesting move from player %d on %d", num, seat.fd);
	
	if (es_write_int(seat.fd, TTT_REQ_MOVE) < 0) {
		ggzdmod_log(ttt_game.ggz, "Error requesting move from player %d", num);
		return -1;
	}

	return 0;
}


/* Handle incoming move from player */
int game_handle_move(int num, int* move)
{
	GGZSeat seat = ggzdmod_get_seat(ttt_game.ggz, num);
	char status;
	
	ggzdmod_log(ttt_game.ggz, "Handling move for player %d", num);
	if (es_read_int(seat.fd, move) < 0)
		return -1;

	/* Check validity of move */
	status = game_check_move(num, *move);

	/* Send back move status */
	if (es_write_int(seat.fd, TTT_RSP_MOVE) < 0
	    || es_write_char(seat.fd, status))
		return -1;

	/* If move simply invalid, ask for resubmit */
	if ( (status == -3 || status == -4)
	     && game_req_move(num) < 0)
		return -1;

	if (status != 0)
		return 1;
	
	return 0;
}


int game_do_move(int move)
{
	char victor;
	
	if (ggzdmod_get_state(ttt_game.ggz) != GGZDMOD_STATE_PLAYING)
		return -1;
		
	ggzdmod_log(ttt_game.ggz, "Player %d in square %d", ttt_game.turn, move);
	ttt_game.board[move] = ttt_game.turn;
	ttt_game.move_count++;
	game_send_move(ttt_game.turn, move);
	
	if ( (victor = game_check_win()) < 0) {
		ttt_game.turn = (ttt_game.turn + 1) % 2;
		game_next_move();
	}
	else {
		game_send_gameover(victor);
		/* Notify GGZ server of game over */
		ggzdmod_set_state(ttt_game.ggz, GGZDMOD_STATE_DONE);
	}
	
	return 0;
}



/* Do bot moves */
int game_bot_move(int me)
{
	int i, move = -1;
	int him = 1 - me;
	char board[9];

	/* Local copy of the boaard to rotate*/
	memcpy(board, ttt_game.board, 9);
	
	/* Checking for win */
	/* Four possible board rotations to check */
	for (i = 3; i >= 0; i--) {
		
		game_rotate_board(board);
		
		/* If three squares of interest filled, try again */
		if (board[0] != -1 && board[1] != -1 && board[3] != -1)
			continue;
		
		/* 5 patterns to check for win */
		if (board[0] == -1 && board[1] == me && board[2] == me) {
			move = 0;
			break;
		}
			
		if (board[0] == -1 && board[3] == me && board[6] == me) {
			move = 0;
			break;
		}

		if (board[0] == -1 && board[4] == me && board[8] == me) {
			move = 0;
			break;
		}

		if (board[0] == me && board[1] == -1 && board[2] == me) {
			move = 1;
			break;
		}

		if (board[1] == -1 && board[4] == me && board[7] == me) {
			move = 1;
			break;
		}
	}
	
	/* We found a move.  Now "unrotate" it */
	if (move != -1) {
		while (i-- > 0)
			move = 2 + 3 * (move % 3) - move / 3;
		return move;
	}
		
	/* Checking for immediate block */
	for (i = 3; i >= 0; i--) {

		game_rotate_board(board);

		/* If three squares of interest filled, try again */
		if (board[0] != -1 && board[1] != -1 && board[3] != -1)
			continue;
		
		/* 5 patterns to check for immediate block */
		if (board[0] == -1 && board[1] == him && board[2] == him) {
			move = 0;
			break;
		}

		if (board[0] == -1 && board[3] == him && board[6] == him) {
			move = 0;
			break;
		}

		if (board[0] == -1 && board[4] == him && board[8] == him) {
			move = 0;
			break;
		}

		if (board[0] == him && board[1] == -1 && board[2] == him) {
			move = 1;
			break;
		}
		
		if (board[1] == -1 && board[4] == him && board[7] == him) {
			move = 1;
			break;
		}
	}

	/* FIXME: If not playing perfect, move here */
	/* We found a move.  Now "unrotate" it */
	if (move != -1) {
		while (i-- > 0)
			move = 2 + 3 * (move % 3) - move / 3;
		return move;
	}
	
	/* Checking for future block */
	for (i = 3; i >= 0; i--) {

		game_rotate_board(board);

		/* If three squares of interest filled, try again */
		if (board[0] != -1 && board[1] != -1 && board[3] != -1)
			continue;
		
		/* 6 patterns to check for future block */
		if (board[0] == -1 && board[1] == -1 && board[2] == him
		    && board[3] == -1 && board[4] == me && board[5] == -1
		    && board[6] == him && board[7] == -1 && board[8] == -1) {
			move = 1;
			break;
		}

		if (board[0] == -1 && board[1] == him && board[2] == -1
		    && board[3] == him && board[4] == me && board[6] == -1) {
			move = 0;
			break;
		}

		if (board[0] == -1 && board[1] == -1 && board[2] == him
		    && board[3] == him && board[4] == me && board[6] == -1) {
			move = 0;
			break;
		}

		if (board[0] == -1 && board[1] == -1 && board[2] == him
		    && board[3] == -1 && board[4] == me && board[6] == him) {
			move = 0;
			break;
		}

		if (board[0] == -1 && board[4] == him && board[8] == -1) {
			move = 0;
			break;
		}

		if (board[1] == -1 && board[4] == him && board[7] == -1) {
			move = 1;
			break;
		}
	}
	
	/* We found a move.  Now "unrotate" it */
	if (move != -1) {
		while (i-- > 0)
			move = 2 + 3 * (move % 3) - move / 3;
	} else {
		/* If we didn't match a pattern, just pick something*/
		if (board[4] == -1) {
			move = 4;
		}
		else
			for (i = 0; i < 9; i++)
				if (board[i] == -1) {
					move = i;
					break;
				}
	}

	return move;
}


/* Check for valid move */
char game_check_move(int num, int move)
{
	/* Check for correct state */
	if (ggzdmod_get_state(ttt_game.ggz) != GGZDMOD_STATE_PLAYING)
		return TTT_ERR_STATE;

	/* Check for correct turn */
	if (num != ttt_game.turn)
		return TTT_ERR_TURN;
	
	/* Check for out of bounds move */
	if (move < 0 || move > 8)
		return TTT_ERR_BOUND;

	/* Check for duplicated move */
	if (ttt_game.board[move] != -1)
		return TTT_ERR_FULL;

	return 0;
}


/* Check for a win */
char game_check_win(void)
{
	/* Check horizontals */
	if (ttt_game.board[0] == ttt_game.board[1]
	    && ttt_game.board[1] == ttt_game.board[2]
	    && ttt_game.board[2] != -1)
		return ttt_game.board[0];

	if (ttt_game.board[3] == ttt_game.board[4]
	    && ttt_game.board[4] == ttt_game.board[5]
	    && ttt_game.board[5] != -1)
		return ttt_game.board[3];
			
	if (ttt_game.board[6] == ttt_game.board[7]
	    && ttt_game.board[7] == ttt_game.board[8]
	    && ttt_game.board[8] != -1)
		return ttt_game.board[6];

	/* Check verticals */
	if (ttt_game.board[0] == ttt_game.board[3]
	    && ttt_game.board[3] == ttt_game.board[6]
	    && ttt_game.board[6] != -1)
		return ttt_game.board[0];

	if (ttt_game.board[1] == ttt_game.board[4]
	    && ttt_game.board[4] == ttt_game.board[7]
	    && ttt_game.board[7] != -1)
		return ttt_game.board[1];

	if (ttt_game.board[2] == ttt_game.board[5]
	    && ttt_game.board[5] == ttt_game.board[8]
	    && ttt_game.board[8] != -1)
		return ttt_game.board[2];
	
	/* Check diagonals */
	if (ttt_game.board[0] == ttt_game.board[4]
	    && ttt_game.board[4] == ttt_game.board[8]
	    && ttt_game.board[8] != -1)
		return ttt_game.board[0];

	if (ttt_game.board[2] == ttt_game.board[4]
	    && ttt_game.board[4] == ttt_game.board[6]
	    && ttt_game.board[6] != -1)
		return ttt_game.board[2];

	/* No one won yet */
	if (ttt_game.move_count == 9)
		return 2;  /* Cat's game */

	return -1;
}


static void game_rotate_board(char b[9])
{
	int i, j;
	char tmp[9];
	
	memcpy(tmp, b, 9);
	for (i = 0; i < 3; i++)
		for (j = 0; j < 3; j++)
			b[3*i+j] = tmp[3*(2-j)+i];
}

