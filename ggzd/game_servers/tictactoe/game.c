/*
 * File: game.c
 * Author: Brent Hendricks
 * Project: GGZ Tic-Tac-Toe game module
 * Date: 3/31/00
 * Desc: Game functions
 *
 * Copyright (C) 2000 Brent Hendricks.
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

#include <stdlib.h>

#include <ggz.h>
#include <game.h>
#include <protocols.h>
#include <easysock.h>

/* Global game variables */
struct ttt_game_t ttt_game;


/* Setup game state and board */
void game_init(void)
{
	int i;
	
	ttt_game.turn = -1;
	ttt_game.move_count = 0;
	ttt_game.state = TTT_STATE_INIT;
	for (i = 0; i < 9; i++)
		ttt_game.board[i] = -1;
}


/* Handle message from GGZ server */
int game_handle_ggz(int ggz_fd, int* p_fd)
{
	int op, seat, status = -1;

	if (es_read_int(ggz_fd, &op) < 0)
		return -1;

	switch (op) {

	case REQ_GAME_LAUNCH:
		if (ggz_game_launch() == 0)
			status = game_update(TTT_EVENT_LAUNCH, NULL);
		break;
		
	case REQ_GAME_JOIN:
		if (ggz_player_join(&seat, p_fd) == 0) {
			status = game_update(TTT_EVENT_JOIN, &seat);
			status = 1;
		}
		break;

	case REQ_GAME_LEAVE:
		if ( (status = ggz_player_leave(&seat, p_fd)) == 0) {
			game_update(TTT_EVENT_LEAVE, &seat);
			status = 2;
		}
		break;
		
	case RSP_GAME_OVER:
		status = 3; /* Signal safe to exit */
		break;

	default:
		/* Unrecognized opcode */
		status = -1;
		break;
	}

	return status;
}


/* Handle message from player */
int game_handle_player(int num)
{
	int fd, op, status, move;

	fd = ggz_seats[num].fd;
	
	if (es_read_int(fd, &op) < 0)
		return -1;

	switch (op) {

	case TTT_SND_MOVE:
		if ( (status = game_handle_move(num, &move)) == 0)
			game_update(TTT_EVENT_MOVE, &move);
		break;
		
	case TTT_REQ_SYNC:
		status = game_send_sync(num);
		break;

	default:
		/* Unrecognized opcode */
		status = -1;
		break;
	}

	return status;
}


/* Send out seat assignment */
int game_send_seat(int seat)
{
	int fd = ggz_seats[seat].fd;

	ggz_debug("Sending player %d's seat num", seat);

	if (es_write_int(fd, TTT_MSG_SEAT) < 0
	    || es_write_int(fd, seat) < 0)
		return -1;

	return 0;
}


/* Send out player list to everybody */
int game_send_players(void)
{
	int i, j, fd;
	
	for (j = 0; j < 2; j++) {
		if ( (fd = ggz_seats[j].fd) == -1)
			continue;

		ggz_debug("Sending player list to player %d", j);

		if (es_write_int(fd, TTT_MSG_PLAYERS) < 0)
			return -1;
	
		for (i = 0; i < 2; i++) {
			if (es_write_int(fd, ggz_seats[i].assign) < 0)
				return -1;
			if (ggz_seats[i].assign != GGZ_SEAT_OPEN
			    && es_write_string(fd, ggz_seats[i].name) < 0) 
				return -1;
		}
	}
	return 0;
}


/* Send out move for player: num */
int game_send_move(int num, int move)
{
	int opponent = (num + 1) % 2;
	int fd = ggz_seats[opponent].fd;
	
	/* If player is a computer, don't need to send */
	if (fd == -1)
		return 0;

	ggz_debug("Sending player %d's move to player %d", num, opponent);

	if (es_write_int(fd, TTT_MSG_MOVE) < 0
	    || es_write_int(fd, move) < 0)
		return -1;
	
	return 0;
}


/* Send out board layout */
int game_send_sync(int num)
{	
	int i, fd = ggz_seats[num].fd;

	ggz_debug("Handling sync for player %d", num);

	if (es_write_int(fd, TTT_SND_SYNC) < 0
	    || es_write_char(fd, ttt_game.turn) < 0)
		return -1;
	
	for (i = 0; i < 9; i++)
		if (es_write_char(fd, ttt_game.board[i]) < 0)
			return -1;
		
	return 0;
}
	

/* Send out game-over message */
int game_send_gameover(char winner)
{
	int i, fd;
	
	for (i = 0; i < 2; i++) {
		if ( (fd = ggz_seats[i].fd) == -1)
			continue;

		ggz_debug("Sending game-over to player %d", i);

		if (es_write_int(fd, TTT_MSG_GAMEOVER) < 0
		    || es_write_char(fd, winner) < 0)
			return -1;
	}

	return 0;
}


/* Do the next move*/
int game_move(void)
{
	int move, num = ttt_game.turn;
	
	if (ggz_seats[num].assign == GGZ_SEAT_BOT) {
		move = game_bot_move();
		game_update(TTT_EVENT_MOVE, &move);
	}
	else
		game_req_move(num);

	return 0;
}


/* Request move from current player */
int game_req_move(int num)
{
	int fd = ggz_seats[num].fd;

	if (es_write_int(fd, TTT_REQ_MOVE) < 0)
		return -1;

	return 0;
}


/* Handle incoming move from player */
int game_handle_move(int num, int* move)
{
	int fd = ggz_seats[num].fd;
	char status;
	
	ggz_debug("Handling move for player %d", num);
	if (es_read_int(fd, move) < 0)
		return -1;

	/* Check validity of move */
	status = game_check_move(num, *move);

	/* Send back move status */
	if (es_write_int(fd, TTT_RSP_MOVE) < 0
	    || es_write_char(fd, status))
		return -1;

	/* If move simply invalid, ask for resubmit */
	if ( (status == -3 || status == -4)
	     && game_req_move(num) < 0)
		return -1;

	if (status != 0)
		return 1;
	
	return 0;
}


/* Do bot moves */
int game_bot_move(void)
{
	int i;
	
	for (i = 0; i < 9; i++)
		if (ttt_game.board[i] == -1)
			return i;

	return -1;
}


/* Check for valid move */
char game_check_move(int num, int move)
{
	/* Check for correct state */
	if (ttt_game.state != TTT_STATE_PLAYING)
		return -1;

	/* Check for correct turn */
	if (num != ttt_game.turn)
		return -2;
	
	/* Check for out of bounds move */
	if (move < 0 || move > 8)
		return -3;

	/* Check for duplicated move */
	if (ttt_game.board[move] != -1)
		return -4;

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


/* Update game state */
int game_update(int event, void* data)
{
	int seat, move;
	char victor;
	
	switch(event) {

	case TTT_EVENT_LAUNCH:
		if (ttt_game.state != TTT_STATE_INIT)
			return -1;
		ttt_game.state = TTT_STATE_WAIT;
		break;

	case TTT_EVENT_JOIN:
		if (ttt_game.state != TTT_STATE_WAIT)
			return -1;
		
		seat = *(int*)data;
		game_send_seat(seat);
		game_send_players();

		if (!ggz_seats_open()) {
			if (ttt_game.turn == -1)
				ttt_game.turn = 0;
			else 
				game_send_sync(seat);
			
			ttt_game.state = TTT_STATE_PLAYING;
			game_move();
		}
		break;

	case TTT_EVENT_LEAVE:
		if (ttt_game.state == TTT_STATE_PLAYING) {
			ttt_game.state = TTT_STATE_WAIT;
			game_send_players();
		}
		break;

	case TTT_EVENT_MOVE:
		if (ttt_game.state != TTT_STATE_PLAYING)
			return -1;
		
		move = *(int*)data;

		ggz_debug("Player %d in square %d", ttt_game.turn, move);
		ttt_game.board[move] = ttt_game.turn;
		ttt_game.move_count++;
		game_send_move(ttt_game.turn, move);
		
		if ( (victor = game_check_win()) < 0) {
			ttt_game.turn = (ttt_game.turn + 1) % 2;
			game_move();
		}
		else {
			ttt_game.state = TTT_STATE_DONE;
			game_send_gameover(victor);
			/* Notify GGZ server of game over */
			ggz_done();
		}
		break;
	}
	
	return 0;
}




