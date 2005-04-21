/*
 * File: game.c
 * Author: Brent Hendricks
 * Project: GGZ Tic-Tac-Toe game module
 * Date: 3/31/00
 * Desc: Game functions
 * $Id: game.c 7119 2005-04-21 18:00:51Z josef $
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
#include <stdio.h>
#include <time.h>
#include <stdarg.h>

/* GGZ utility functions */
#include <ggz.h>

/* Header including ggzdmod.h */
#include "game.h"
#include "net.h"

/* The game supports people not on the table watching the game */
#define GGZSPECTATORS /* do not undefine! */
/* The game supports statistics (e.g. records, ratings, highscores) */
#define GGZSTATISTICS /* do not undefine! */
/* The game supports leave/rejoin of players */
#define GGZGAMERESUME /* do not undefine */
/* The game supports AI players */
#define GGZBOTPLAYERS /* do not undefine */
/* The game supports savegames */
#define GGZSAVEDGAMES /* do not undefine */

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

/* Data structure for Tic-Tac-Toe-Game */
struct ttt_game_t {
	GGZdMod *ggz;		/* Pointer to global ggzdmod object */
	/*char board[9];*/	/* Single-dimension representation of game board (unused) */
	/*char state;*/		/* Unused */
	/*char turn;*/		/* Player turn (-1 before start, 0/1 for players) */
	char move_count;	/* Number of moves already executed */
	FILE *savegame;		/* Continuous game logfile */
};

/* Global game variables */
static struct ttt_game_t ttt_game;
static int save_num;

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

/* Network IO main functions */
static void game_network_data(int opcode);
static void game_network_error(void);

/* Network IO function prototypes */
static int game_send_seat(int seat);
static int game_send_players(void);
static int game_send_move(int num, int move);
static int game_send_sync(int fd);
static int game_send_gameover(int winner);
static int game_read_move(int num, int* move);

/* TTT-move function prototypes */
static int game_next_move(void);
static int game_req_move(int num);
#ifdef GGZBOTPLAYERS
static int game_bot_move(int num);
#endif
static int game_do_move(int move);

/* Local utility function prototypes */
#ifdef GGZBOTPLAYERS
static void game_rotate_board(char b[9]);
#endif
static char game_check_move(int num, int move);
static char game_check_win(void);
#ifdef GGZSAVEDGAMES
static void game_save(char *fmt, ...);
#endif

/* Setup game state and board */
void game_init(GGZdMod *ggzdmod)
{
	int i;
	
	variables.turn = -1;
	for (i = 0; i < 9; i++)
		variables.space[i] = -1;

	ttt_game.move_count = 0;
	ttt_game.savegame = NULL;
		
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

	ggzcomm_set_notifier_callback(game_network_data);
	ggzcomm_set_error_callback(game_network_error);
}


/* Callback for GGZDMOD_EVENT_STATE */
static void game_handle_ggz_state(GGZdMod *ggz, GGZdModEvent event,
				  const void *data)
{

	switch(ggzdmod_get_state(ggz)) {
	case GGZDMOD_STATE_PLAYING:
		/* If we're just starting, set for first players turn*/
		if (variables.turn == -1)
			variables.turn = 0;
		
#ifdef GGZSAVEDGAMES
		if(variables.turn == 0) {
			game_save("players options 3 3");
			game_save("players start %i", time(NULL));
		} else {
			game_save("players continue %i", time(NULL));
		}
#endif
		game_next_move();
	default:
		break;
	}
}


static int seats_full(void)
{
	/* This calculation is a bit inefficient, but that's OK */
	return ggzdmod_count_seats(ttt_game.ggz, GGZ_SEAT_OPEN) == 0
	  && ggzdmod_count_seats(ttt_game.ggz, GGZ_SEAT_RESERVED) == 0
	  && ggzdmod_count_seats(ttt_game.ggz, GGZ_SEAT_ABANDONED) == 0;
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
	int i;
	const GGZSpectator *old_spectator = data;
	GGZSpectator spectator;
	GGZSeat tmpseat;

	spectator = ggzdmod_get_spectator(ggz, old_spectator->num);

	for (i = 0; i < 2; i++) {
		tmpseat = ggzdmod_get_seat(ttt_game.ggz, i);
		variables.seat[i] = tmpseat.type;
		variables.name[i] = (char*)tmpseat.name;
	}

	ggzcomm_set_fd(spectator.fd);
	ggzcomm_msgplayers();

	game_send_sync(spectator.fd);
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
		if (variables.turn != -1)
			game_send_sync(new_seat.fd);
#endif
	}

#ifdef GGZSAVEDGAMES
	if ((old_seat->type == GGZ_SEAT_PLAYER) || (old_seat->type == GGZ_SEAT_BOT)) {
		game_save("player%i leave %s", old_seat->num + 1, old_seat->name);
	}
	if ((new_seat.type == GGZ_SEAT_PLAYER) || (new_seat.type == GGZ_SEAT_BOT)) {
		game_save("player%i join %s", new_seat.num + 1, new_seat.name);
	}
#endif

	ggzdmod_set_state(ttt_game.ggz, new_state);
}


/* Handle message from player */
static void game_handle_ggz_player(GGZdMod *ggz, GGZdModEvent event,
				   const void *data)
{
	GGZSeat seat;

	save_num = *(int*)data;
	seat = ggzdmod_get_seat(ggz, save_num);

	ggzcomm_set_fd(seat.fd);
	ggzcomm_network_main();
}

static void game_network_data(int opcode)
{
	int num, fd;

	ggzdmod_log(ttt_game.ggz, "Network data: opcode=%i", opcode);

	fd = ggzcomm_get_fd();
	num = save_num;

	switch (opcode) {

	case sndmove:
		if (game_read_move(num, &variables.move_c) == 0)
			game_do_move(variables.move_c);
		break;
	case reqsync:
		game_send_sync(fd);
		break;
	}
}

static void game_network_error(void)	
{
	ggzdmod_log(ttt_game.ggz, "Network error!");
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

	variables.num = num;

	ggzcomm_set_fd(seat.fd);
	ggzcomm_msgseat();
}


/* Send out player list to everybody */
static int game_send_players(void)
{
	int i, j;
	GGZSeat seat, tmpseat;

	for (j = 0; j < 2; j++) {
		seat = ggzdmod_get_seat(ttt_game.ggz, j);

		if (seat.fd != -1) {
			ggzdmod_log(ttt_game.ggz, "Sending playerlist to player %d", j);

			for (i = 0; i < 2; i++) {
				tmpseat = ggzdmod_get_seat(ttt_game.ggz, i);
				variables.seat[i] = tmpseat.type;
				variables.name[i] = (char*)tmpseat.name;
			}

			ggzcomm_set_fd(seat.fd);
			ggzcomm_msgplayers();
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

			variables.player = num;
			variables.move = move;

			ggzcomm_set_fd(seat.fd);
			ggzcomm_msgmove();
	}
	return 0;
}


/* Send out board layout */
static int game_send_sync(int fd)
{	
	ggzdmod_log(ttt_game.ggz, "Handling sync for fd %d", fd);

	ggzcomm_set_fd(fd);
	ggzcomm_sndsync();

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

			variables.winner = winner;

			ggzcomm_set_fd(seat.fd);
			ggzcomm_msggameover();
		}
	}

#ifdef GGZSPECTATORS
	for (i = 0; i < ggzdmod_get_max_num_spectators(ttt_game.ggz); i++)
	{
		spectator = ggzdmod_get_spectator(ttt_game.ggz, i);
		if (spectator.fd < 0) continue;

		variables.winner = winner;

		ggzcomm_set_fd(spectator.fd);
		ggzcomm_msggameover();
	}
#endif

#ifdef GGZSAVEDGAMES
	game_save("%s %s %i",
		(winner == 2 ? "players" : (winner == 0 ? "player1" : "player2")),
		(winner == 2 ? "tie" : "winner"),
		time(NULL));
#endif

	return 0;
}


/* Read incoming move from player */
int game_read_move(int num, int* move)
{
	GGZSeat seat = ggzdmod_get_seat(ttt_game.ggz, num);
	char status;
	
	ggzdmod_log(ttt_game.ggz, "Handling move for player %d", num);

	/* Check validity of move */
	status = game_check_move(num, *move);

	/* Send back move status */
	variables.status = status;

	ggzcomm_set_fd(seat.fd);
	ggzcomm_rspmove();

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
#ifdef GGZBOTPLAYERS
	int move;
	GGZSeat seat = ggzdmod_get_seat(ttt_game.ggz, variables.turn);
#endif
	
#ifdef GGZBOTPLAYERS
	if (seat.type == GGZ_SEAT_BOT) {
		move = game_bot_move(variables.turn);
		game_do_move(move);
	}
	else
#endif
		game_req_move(variables.turn);

	return 0;
}


/* Request move from current player */
static int game_req_move(int num)
{
	GGZSeat seat = ggzdmod_get_seat(ttt_game.ggz, num);

	ggzdmod_log(ttt_game.ggz, "Requesting move from player %d on %d", num, seat.fd);

	ggzcomm_set_fd(seat.fd);
	ggzcomm_reqmove();

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
		
	ggzdmod_log(ttt_game.ggz, "Player %d in square %d", variables.turn, move);
	variables.space[move] = variables.turn;
	ttt_game.move_count++;
	game_send_move(variables.turn, move);

#ifdef GGZSPECTATORS
	for(i = 0; i < ggzdmod_get_max_num_spectators(ttt_game.ggz); i++)
	{
		fd = (ggzdmod_get_spectator(ttt_game.ggz, i)).fd;
		if (fd < 0) continue;

		variables.player = variables.turn;
		variables.move = move;

		ggzcomm_set_fd(fd);
		ggzcomm_msgmove();
	}
#endif

#ifdef GGZSAVEDGAMES
	game_save("player%i move %i %i", variables.turn + 1, move / 3, move % 3);
#endif
	
	if ( (victor = game_check_win()) < 0) {
		variables.turn = (variables.turn + 1) % 2;
		game_next_move();
	}
	else {
		game_send_gameover(victor);
		/* Notify GGZ server of game over */
#ifdef GGZGAMERESUME
		ggzdmod_set_state(ttt_game.ggz, GGZDMOD_STATE_WAITING);
#else
		ggzdmod_set_state(ttt_game.ggz, GGZDMOD_STATE_DONE);
#endif
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
	for (i = 0; i < 9; i++)
		board[i] = variables.space[i];

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
	if (num != variables.turn)
		return TTT_ERR_TURN;
	
	/* Check for out of bounds move */
	if (move < 0 || move > 8)
		return TTT_ERR_BOUND;

	/* Check for duplicated move */
	if (variables.space[move] != -1)
		return TTT_ERR_FULL;

	return 0;
}


/* Check for a win */
static char game_check_win(void)
{
	/* Check horizontals */
	if (variables.space[0] == variables.space[1]
	    && variables.space[1] == variables.space[2]
	    && variables.space[2] != -1)
		return variables.space[0];

	if (variables.space[3] == variables.space[4]
	    && variables.space[4] == variables.space[5]
	    && variables.space[5] != -1)
		return variables.space[3];
			
	if (variables.space[6] == variables.space[7]
	    && variables.space[7] == variables.space[8]
	    && variables.space[8] != -1)
		return variables.space[6];

	/* Check verticals */
	if (variables.space[0] == variables.space[3]
	    && variables.space[3] == variables.space[6]
	    && variables.space[6] != -1)
		return variables.space[0];

	if (variables.space[1] == variables.space[4]
	    && variables.space[4] == variables.space[7]
	    && variables.space[7] != -1)
		return variables.space[1];

	if (variables.space[2] == variables.space[5]
	    && variables.space[5] == variables.space[8]
	    && variables.space[8] != -1)
		return variables.space[2];
	
	/* Check diagonals */
	if (variables.space[0] == variables.space[4]
	    && variables.space[4] == variables.space[8]
	    && variables.space[8] != -1)
		return variables.space[0];

	if (variables.space[2] == variables.space[4]
	    && variables.space[4] == variables.space[6]
	    && variables.space[6] != -1)
		return variables.space[2];

	/* No one won yet */
	if (ttt_game.move_count == 9)
		return 2;  /* Cat's game */

	return -1;
}


#ifdef GGZBOTPLAYERS
static void game_rotate_board(char b[9])
{
	int i, j;
	char tmp[9];
	
	for (i = 0; i < 9; i++)
		tmp[i] = b[i];
	for (i = 0; i < 3; i++)
		for (j = 0; j < 3; j++)
			b[3*i+j] = tmp[3*(2-j)+i];
}
#endif


#ifdef GGZSAVEDGAMES
#define TEMPLATE "savegame.XXXXXX"

static void game_save(char *fmt, ...)
{
	int fd;
	char *savegamepath, *savegamename;
	char buffer[1024];

	if(!ttt_game.savegame) {
		savegamepath = strdup(DATADIR "/gamedata/TicTacToe/" TEMPLATE);
		fd = mkstemp(savegamepath);
		savegamename = strdup(savegamepath + strlen(savegamepath) - strlen(TEMPLATE));
		free(savegamepath);
		if(fd < 0) return;
		ttt_game.savegame = fdopen(fd, "w");
		if(!ttt_game.savegame) return;

		ggzdmod_report_savegame(ttt_game.ggz, savegamename);
		free(savegamename);
	}

	va_list ap;
	va_start(ap, fmt);
	vsnprintf(buffer, sizeof(buffer), fmt, ap);
	va_end(ap);

	fprintf(ttt_game.savegame, "%s\n", buffer);
	fflush(ttt_game.savegame);
}
#endif

