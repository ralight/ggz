/*
 * File: game.c
 * Author: Brent Hendricks
 * Project: GGZ Tic-Tac-Toe game module
 * Date: 3/31/00
 * Desc: Game functions
 * $Id: game.c 6892 2005-01-25 04:09:21Z jdorje $
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

/*
 * The Tic-Tac-Toe game server has been designed to let starters easily
 * get to know the features that GGZ provides to game authors.
 * Thus, a lot of #ifdefs are used which are not intended to be undefined,
 * but rather to localize the special features.
 * A simple game can do without them, but their usage is strongly encouraged.
 */

/* Site-specific configuration */
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

/* System header files */
#include <stdlib.h>
#include <string.h>

/* GGZ utility functions */
#include <ggz.h>

/* Header including ggzdmod.h */
#include "game.h"

/* The game supports people not on the table watching the game */
#define GGZSPECTATORS /* do not undefine! */
/* The game supports statistics (e.g. records, ratings, highscores) */
#define GGZSTATISTICS /* do not undefine! */
/* The game supports leave/rejoin of players */
#define GGZGAMERESUME /* do not undefine */
/* The game supports AI players */
#define GGZBOTPLAYERS /* do not undefine */

/* Data structure for Tic-Tac-Toe-Game */
struct ttt_game_t {
	GGZdMod *ggz;
	char board[9];
	char state;
	char turn;
	char move_count;
};

/* Tic-Tac-Toe protocol */
/* Messages from server */
#define TTT_MSG_SEAT     0
#define TTT_MSG_PLAYERS  1
#define TTT_MSG_MOVE     2
#define TTT_MSG_GAMEOVER 3
#define TTT_REQ_MOVE     4
#define TTT_RSP_MOVE     5
#define TTT_SND_SYNC     6

/* Messages from client */
#define TTT_SND_MOVE     0
#define TTT_REQ_SYNC     1

/* Move errors */
#define TTT_ERR_STATE   -1
#define TTT_ERR_TURN    -2
#define TTT_ERR_BOUND   -3
#define TTT_ERR_FULL    -4


/* Global game variables */
static struct ttt_game_t ttt_game;

/* GGZdMod callbacks */
static void game_handle_ggz_state(GGZdMod *ggz,
                                  GGZdModEvent event, const void *data);
static void game_handle_ggz_seat(GGZdMod *ggz,
				 GGZdModEvent event, const void *data);
#ifdef GGZSPECTATORS
static void game_handle_ggz_spectator_join(GGZdMod *ggz,
                                 GGZdModEvent event, const void *data);
static void game_handle_ggz_spectator_leave(GGZdMod *ggz,
                                  GGZdModEvent event, const void *data);
static void game_handle_ggz_spectator(GGZdMod *ggz,
                                   GGZdModEvent event, const void *data);
#endif
static void game_handle_ggz_player(GGZdMod *ggz,
                                   GGZdModEvent event, const void *data);

/* Network IO functions */
static int game_send_seat(int seat);
static int game_send_players(void);
static int game_send_move(int num, int move);
static int game_send_sync(int fd);
static int game_send_gameover(int winner);
static int game_read_move(int num, int* move);

/* TTT-move functions */
static int game_next_move(void);
static int game_req_move(int num);
#ifdef GGZBOTPLAYERS
static int game_bot_move(int num);
#endif
static int game_do_move(int move);

/* Local utility functions */
static void game_rotate_board(char b[9]);
static char game_check_move(int num, int move);
static char game_check_win(void);


/* Setup game state and board */
void game_init(GGZdMod *ggzdmod)
{
	int i;
	
	ttt_game.turn = -1;
	ttt_game.move_count = 0;
	for (i = 0; i < 9; i++)
		ttt_game.board[i] = -1;
		
	/* Setup GGZ game module */
	ttt_game.ggz = ggzdmod;
	ggzdmod_set_handler(ggzdmod, GGZDMOD_EVENT_STATE,
	                    &game_handle_ggz_state);
	ggzdmod_set_handler(ggzdmod, GGZDMOD_EVENT_JOIN,
	                    &game_handle_ggz_seat);
	ggzdmod_set_handler(ggzdmod, GGZDMOD_EVENT_LEAVE,
	                    &game_handle_ggz_seat);
	ggzdmod_set_handler(ggzdmod, GGZDMOD_EVENT_SEAT,
			    &game_handle_ggz_seat);
	ggzdmod_set_handler(ggzdmod, GGZDMOD_EVENT_PLAYER_DATA,
	                    &game_handle_ggz_player);
#ifdef GGZSPECTATORS
	ggzdmod_set_handler(ggzdmod, GGZDMOD_EVENT_SPECTATOR_DATA,
	                    &game_handle_ggz_spectator);
	ggzdmod_set_handler(ggzdmod, GGZDMOD_EVENT_SPECTATOR_JOIN,
	                    &game_handle_ggz_spectator_join);
	ggzdmod_set_handler(ggzdmod, GGZDMOD_EVENT_SPECTATOR_LEAVE,
	                    &game_handle_ggz_spectator_leave);
#endif
}


/* Callback for GGZDMOD_EVENT_STATE */
static void game_handle_ggz_state(GGZdMod *ggz, GGZdModEvent event,
				  const void *data)
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


static int seats_full(void)
{
	/* This calculation is a bit inefficient, but that's OK */
	return ggzdmod_count_seats(ttt_game.ggz, GGZ_SEAT_OPEN) == 0
		&& ggzdmod_count_seats(ttt_game.ggz, GGZ_SEAT_RESERVED) == 0;
}

static int seats_empty(void)
{
	/* This calculation is a bit inefficient, but that's OK */
	return ggzdmod_count_seats(ttt_game.ggz, GGZ_SEAT_PLAYER) == 0
		&& ggzdmod_count_spectators(ttt_game.ggz) == 0;
}

#ifdef GGZSPECTATORS
static void game_handle_ggz_spectator_join(GGZdMod *ggz, GGZdModEvent event,
					   const void *data)
{
	int i, fd;
	const GGZSpectator *old_spectator = data;
	GGZSpectator spectator;

	spectator = ggzdmod_get_spectator(ggz, old_spectator->num);
	fd = spectator.fd;

	if (ggz_write_int(fd, TTT_MSG_PLAYERS) < 0)
		return;
	for (i = 0; i < 2; i++) {
		GGZSeat seat = ggzdmod_get_seat(ttt_game.ggz, i);
		if (ggz_write_int(fd, seat.type) < 0)
			return;
		if (seat.type != GGZ_SEAT_OPEN
		    && ggz_write_string(fd, seat.name) < 0)
			return;
	}

	if (ggz_write_int(fd, TTT_SND_SYNC) < 0
	|| ggz_write_char(fd, ttt_game.turn) < 0)
		return;
	
	for (i = 0; i < 9; i++)
		if (ggz_write_char(fd, ttt_game.board[i]) < 0)
			return;
}

static void game_handle_ggz_spectator_leave(GGZdMod *ggz, GGZdModEvent event,
					    const void *data)
{
	if (seats_empty())
		ggzdmod_set_state(ttt_game.ggz, GGZDMOD_STATE_DONE);
}
#endif

/* Callback for ggzdmod JOIN, LEAVE, and SEAT events */
static void game_handle_ggz_seat(GGZdMod *ggz, GGZdModEvent event,
				 const void *data)
{
	GGZdModState new_state;
	const GGZSeat *old_seat = data;
	GGZSeat new_seat = ggzdmod_get_seat(ggz, old_seat->num);

	if (seats_full())
		new_state = GGZDMOD_STATE_PLAYING;
	else if (seats_empty())
		new_state = GGZDMOD_STATE_DONE;
	else
#ifdef GGZGAMERESUME
		new_state = GGZDMOD_STATE_WAITING;
#else
		new_state = GGZDMOD_STATE_DONE;
#endif

	game_send_players();
	if (new_seat.type == GGZ_SEAT_PLAYER) {
		game_send_seat(new_seat.num);
#ifdef GGZGAMERESUME
		/* If we're continuing a game, send sync to new player */
		if (ttt_game.turn != -1)
			game_send_sync(new_seat.fd);
#endif
	}

	ggzdmod_set_state(ttt_game.ggz, new_state);
}


/* Handle message from player */
static void game_handle_ggz_player(GGZdMod *ggz, GGZdModEvent event,
				   const void *data)
{
	const int *num_ptr = data;
	const int num = *num_ptr;
	int fd, op, move;
	GGZSeat seat;

	seat = ggzdmod_get_seat(ggz, num);
	fd = seat.fd;
	
	if (ggz_read_int(fd, &op) < 0)
		return;

	switch (op) {

	case TTT_SND_MOVE:
		if (game_read_move(num, &move) == 0)
			game_do_move(move);
		break;
	case TTT_REQ_SYNC:
		game_send_sync(fd);
		break;
	default:
		ggzdmod_log(ggz, "Unrecognized player opcode %d.", op);
	}
}

#ifdef GGZSPECTATORS
/* Handle message from spectator */
static void game_handle_ggz_spectator(GGZdMod *ggz, GGZdModEvent event,
				      const void *data)
{
	const int *num_ptr = data;
	const int num = *num_ptr;
	int fd, op;
	GGZSpectator spectator;

	spectator = ggzdmod_get_spectator(ggz, num);
	fd = spectator.fd;
	
	if (ggz_read_int(fd, &op) < 0)
		return;

	switch (op) {
	case TTT_REQ_SYNC:
		game_send_sync(fd);
		break;
	default:
		ggzdmod_log(ggz, "Unrecognized spectator opcode %d.", op);
	}
}
#endif

/* Send out seat assignment */
static int game_send_seat(int num)
{
	GGZSeat seat;

	seat = ggzdmod_get_seat(ttt_game.ggz, num);
	ggzdmod_log(ttt_game.ggz, "Sending player %d's seat num", num);

	if (ggz_write_int(seat.fd, TTT_MSG_SEAT) < 0
	    || ggz_write_int(seat.fd, num) < 0)
		return -1;

	return 0;
}


/* Send out player list to everybody */
static int game_send_players(void)
{
	int i, j, fd;
	GGZSeat seat;
	
	for (j = 0; j < 2; j++) {
		seat = ggzdmod_get_seat(ttt_game.ggz, j);
		fd = seat.fd;

		if (fd != -1) {
			ggzdmod_log(ttt_game.ggz, "Sending playerlist to player %d", j);

			if (ggz_write_int(fd, TTT_MSG_PLAYERS) < 0)
				return -1;
			
			for (i = 0; i < 2; i++) {
				seat = ggzdmod_get_seat(ttt_game.ggz, i);
				if (ggz_write_int(fd, seat.type) < 0)
					return -1;
				if (seat.type != GGZ_SEAT_OPEN
				    && ggz_write_string(fd, seat.name) < 0)
					return -1;
			}
		}
	}
	return 0;
}


/* Send out move for player: num */
static int game_send_move(int num, int move)
{
	int opponent = (num + 1) % 2;
	GGZSeat seat = ggzdmod_get_seat(ttt_game.ggz, opponent);

	/* If player is a computer, don't need to send */
	if (seat.fd != -1) {
		
		ggzdmod_log(ttt_game.ggz, "Sending player %d's move to player %d", num,
			    opponent);

		if (ggz_write_int(seat.fd, TTT_MSG_MOVE) < 0
			|| ggz_write_int(seat.fd, num) < 0
		    || ggz_write_int(seat.fd, move) < 0)
			return -1;
	}
	return 0;
}


/* Send out board layout */
static int game_send_sync(int fd)
{	
	int i;

	ggzdmod_log(ttt_game.ggz, "Handling sync for fd %d", fd);

	if (ggz_write_int(fd, TTT_SND_SYNC) < 0
	    || ggz_write_char(fd, ttt_game.turn) < 0)
		return -1;
	
	for (i = 0; i < 9; i++)
		if (ggz_write_char(fd, ttt_game.board[i]) < 0)
			return -1;
		
	return 0;
}
	

/* Send out game-over message */
static int game_send_gameover(int winner)
{
	int i;
	GGZSeat seat;
#ifdef GGZSPECTATORS
	GGZSpectator spectator;
#endif

#ifdef GGZSTATISTICS
	GGZGameResult results[2];
	if (winner < 2) {
		results[winner] = GGZ_GAME_WIN;
		results[1 - winner] = GGZ_GAME_LOSS;
	} else {
		results[0] = results[1] = GGZ_GAME_TIE;
	}

	ggzdmod_report_game(ttt_game.ggz, NULL, results, NULL);
#endif
	
	for (i = 0; i < 2; i++) {
		seat = ggzdmod_get_seat(ttt_game.ggz, i);
		if (seat.fd != -1) {
			ggzdmod_log(ttt_game.ggz, "Sending game-over to player %d", i);
			
			if (ggz_write_int(seat.fd, TTT_MSG_GAMEOVER) < 0
			    || ggz_write_char(seat.fd, winner) < 0)
				return -1;
		}
	}

#ifdef GGZSPECTATORS
	for (i = 0; i < ggzdmod_get_max_num_spectators(ttt_game.ggz); i++)
	{
		spectator = ggzdmod_get_spectator(ttt_game.ggz, i);
		if (spectator.fd < 0) continue;
		ggz_write_int(spectator.fd, TTT_MSG_GAMEOVER);
		ggz_write_char(spectator.fd, winner);
	}
#endif

	return 0;
}


/* Read incoming move from player */
int game_read_move(int num, int* move)
{
	GGZSeat seat = ggzdmod_get_seat(ttt_game.ggz, num);
	char status;
	
	ggzdmod_log(ttt_game.ggz, "Handling move for player %d", num);
	if (ggz_read_int(seat.fd, move) < 0)
		return -1;

	/* Check validity of move */
	status = game_check_move(num, *move);

	/* Send back move status */
	if (ggz_write_int(seat.fd, TTT_RSP_MOVE) < 0
	    || ggz_write_char(seat.fd, status))
		return -1;

	/* If move simply invalid, ask for resubmit */
	if ( (status == -3 || status == -4)
	     && game_req_move(num) < 0)
		return -1;

	if (status != 0)
		return 1;
	
	return 0;
}


/* Do the next move*/
static int game_next_move(void)
{
	int move, num = ttt_game.turn;
	GGZSeat seat = ggzdmod_get_seat(ttt_game.ggz, num);
	
#ifdef GGZBOTPLAYERS
	if (seat.type == GGZ_SEAT_BOT) {
		move = game_bot_move(num);
		game_do_move(move);
	}
	else
#endif
		game_req_move(num);

	return 0;
}


/* Request move from current player */
static int game_req_move(int num)
{
	GGZSeat seat = ggzdmod_get_seat(ttt_game.ggz, num);
	
	ggzdmod_log(ttt_game.ggz, "Requesting move from player %d on %d", num, seat.fd);
	
	if (ggz_write_int(seat.fd, TTT_REQ_MOVE) < 0) {
		ggzdmod_log(ttt_game.ggz, "Error requesting move from player %d", num);
		return -1;
	}

	return 0;
}


static int game_do_move(int move)
{
	char victor;
#ifdef GGZSPECTATORS
	int i, fd;
#endif

	/* FIXME: we should not return on a network error within this
	   function - that will fubar the whole game since the turn won't
	   be incremented.  --JDS */
	
	if (ggzdmod_get_state(ttt_game.ggz) != GGZDMOD_STATE_PLAYING)
		return -1;
		
	ggzdmod_log(ttt_game.ggz, "Player %d in square %d", ttt_game.turn, move);
	ttt_game.board[move] = ttt_game.turn;
	ttt_game.move_count++;
	game_send_move(ttt_game.turn, move);

#ifdef GGZSPECTATORS
	for(i = 0; i < ggzdmod_get_max_num_spectators(ttt_game.ggz); i++)
	{
		fd = (ggzdmod_get_spectator(ttt_game.ggz, i)).fd;
		if (fd < 0) continue;
		if (ggz_write_int(fd, TTT_MSG_MOVE) < 0
		    || ggz_write_int(fd, ttt_game.turn) < 0
		    || ggz_write_int(fd, move) < 0)
			return -1;
	}
#endif
	
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


#ifdef GGZBOTPLAYERS
/* Do bot moves */
static int game_bot_move(int me)
{
	int i, move = -1;
	int him = 1 - me;
	char board[9];
	int c;

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

	/* Avoid 'holy cow' weakness */
	c = board[4];
	if(c != -1)
	{
		for (i = 0; i < 9; i += 2) {
			if (i == 4) continue;
			if ((board[i] == c)
			&& (board[8 - i] != c)
			&& (board[8 - i] != -1)) {
				/*printf("holy cow! at: %i\n", i);*/
				if (board[abs(i - 6)] == -1) {
					return abs(i - 6);
				} else if (board[abs(i - 2)] == -1) {
					return abs(i - 2);
				}
			}
		}
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
#endif


/* Check for valid move */
static char game_check_move(int num, int move)
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
static char game_check_win(void)
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

