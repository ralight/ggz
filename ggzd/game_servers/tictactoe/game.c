/*
 * GGZ Tic-Tac-Toe Server
 * Copyright (C) 2000 Brent Hendricks.
 * Copyright (C) 2001 - 2007 GGZ Development Team
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

/* Please read README.tictactoe first! */

/* === Include files === */

/* Site-specific configuration */
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

/* System header files */
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <stdarg.h>

/* GGZ utility functions */
#include <ggz.h>

/* TTT header including ggzdmod.h */
#include "game.h"

/* TTT auto-generated networking layer */
#include "net.h"

/* === Configuration of features === */

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
/* The game supports distinguishable named bots */
#define GGZBOTHASNAME /* do not undefine */
/* The game supports to play one more time; FIXME: not used yet */
#define GGZAGAINQUERY /* do not undefine */
/* The game can be restored from a savegame */
#define GGZRESTORGAME /* do not undefine */

/* TTT artificial intelligence */
#ifdef GGZBOTPLAYERS
#include "ttt-ai.h"
#endif

/* === Tic-Tac-Toe protocol (otherwise now defined in net.h) === */

/* Messages from client */
#define TTT_REQ_SYNC     1

/* Move results */
#define TTT_OK           0
#define TTT_ERR_STATE   -1
#define TTT_ERR_TURN    -2
#define TTT_ERR_BOUND   -3
#define TTT_ERR_FULL    -4

/* Unoccupied square on the board */
#define TTT_EMPTY       -1

/* Value for a tie game */
#define TTT_NO_WINNER    2

/* Whether or not DIO is used (FIXME: until ggzcomm supports spectators) */
#define TTT_USE_DIO      0

/* === Variables, types and functions === */

/* Data structure for Tic-Tac-Toe-Game */
/* The game board and the turn are managed by the network protocol */
struct ttt_game_t {
	GGZdMod *ggz;		/* Pointer to global ggzdmod object */
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
static void game_handle_ggz_spectator_data(GGZdMod *ggz,
					   GGZdModEvent event,
					   const void *data);
static void game_handle_ggz_spectator_seat(GGZdMod *ggz,
					   GGZdModEvent event,
					   const void *data);
#endif
static void game_handle_ggz_player(GGZdMod *ggz,
                                   GGZdModEvent event, const void *data);
#ifdef GGZRESTORGAME
static void game_handle_savedgame(GGZdMod *ggz,
                                  GGZdModEvent event, const void *data);
#endif

/* Network IO main functions */
static void game_network_data(int opcode);
static void game_network_error(void);

/* Network IO function prototypes */
static int game_send_move(int num, int move);
static int game_send_sync(GGZCommIO *io);
static int game_send_gameover(int winner);
static int game_read_move(int num, int* move);

/* TTT-move function prototypes */
static int game_next_move(void);
static int game_req_move(int num);
static int game_do_move(int move);

/* Local utility function prototypes */
static char game_check_move(int num, int move);
static char game_check_win(void);
#ifdef GGZSAVEDGAMES
static void game_save(char *fmt, ...);
#endif

/* === Implementation part === */

/* Setup game state and board */
void game_init(GGZdMod *ggzdmod)
{
	int i;

	/* Initialize all variables */
	variables.turn = -1;
	for (i = 0; i < 9; i++)
		variables.space[i] = TTT_EMPTY;

	ttt_game.move_count = 0;
	ttt_game.savegame = NULL;
	ttt_game.ggz = ggzdmod;

	/* Setup GGZ game module callbacks */
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
	                    &game_handle_ggz_spectator_data);
	ggzdmod_set_handler(ggzdmod, GGZDMOD_EVENT_SPECTATOR_JOIN,
	                    &game_handle_ggz_spectator_seat);
	ggzdmod_set_handler(ggzdmod, GGZDMOD_EVENT_SPECTATOR_LEAVE,
	                    &game_handle_ggz_spectator_seat);
	ggzdmod_set_handler(ggzdmod, GGZDMOD_EVENT_SPECTATOR_SEAT,
	                    &game_handle_ggz_spectator_seat);
#endif
#ifdef GGZRESTORGAME
	ggzdmod_set_handler(ggzdmod, GGZDMOD_EVENT_SAVEDGAME,
	                    &game_handle_savedgame);
#endif

	/* Setup the network callbacks for GGZComm */
	ggzcomm_set_notifier_callback(game_network_data);
	ggzcomm_set_error_callback(game_network_error);
}


#ifdef GGZRESTORGAME
/* Callback for GGZDMOD_EVENT_SAVEDGAME */
static void game_handle_savedgame(GGZdMod *ggz, GGZdModEvent event,
				  const void *data)
{
	const char *savegame = data;
	char *savegamepath;
	FILE *f;
	char line[1024];

	int w, h;
	int x, y;

	savegamepath = ggz_strbuild(GGZDSTATEDIR "/gamedata/TicTacToe/%s", savegame);
	ggzdmod_log(ttt_game.ggz, "Game restoration: savegame=%s", savegamepath);

	variables.turn = -1;

	f = fopen(savegamepath, "r");
	if(f) {
		while(1) {
			if(!fgets(line, sizeof(line), f))
				break;
			line[strlen(line) - 1] = '\0';
			ggzdmod_log(ttt_game.ggz, "Game restoration: line=%s", line);
			if(sscanf(line, "players options %i %i", &w, &h) == 2) {
				ggzdmod_log(ttt_game.ggz, "Game restoration: options=%i/%i", w, h);
				/* unused width/height */
			} else if(sscanf(line, "player1 move %i %i", &x, &y) == 2) {
				ggzdmod_log(ttt_game.ggz, "Game restoration: player1=%i/%i", x, y);
				variables.space[x * 3 + y] = 0;
				variables.turn = 1;
				ttt_game.move_count++;
			} else if(sscanf(line, "player2 move %i %i", &x, &y) == 2) {
				ggzdmod_log(ttt_game.ggz, "Game restoration: player2=%i/%i", x, y);
				variables.space[x * 3 + y] = 1;
				variables.turn = 0;
				ttt_game.move_count++;
			} else {
				ggzdmod_log(ttt_game.ggz, "Game restoration: dismiss");
			}
		}
		fclose(f);
	}

	if (game_check_win() >= 0) {
		ggzdmod_set_state(ttt_game.ggz, GGZDMOD_STATE_DONE);
	}

	ttt_game.savegame = fopen(savegamepath, "a");
	ggz_free(savegamepath);
}
#endif


/* Callback for GGZDMOD_EVENT_STATE */
static void game_handle_ggz_state(GGZdMod *ggz, GGZdModEvent event,
				  const void *data)
{
	int i;

	switch(ggzdmod_get_state(ggz)) {
	case GGZDMOD_STATE_CREATED:
		/* Game server just started without any connections yet */
		for (i = 0; i < 2; i++) {
			assert(ggzdmod_get_seat(ggz, i).playerdata == NULL);
			assert(ggzdmod_get_seat(ggz, i).name == NULL);
			assert(ggzdmod_get_seat(ggz, i).fd == -1);
		}
		break;
	case GGZDMOD_STATE_WAITING:
		/* Game server is connected to GGZ */
		/* Alternatively (later on) not playing anymore */
		break;
	case GGZDMOD_STATE_PLAYING:
		/* If we're starting to play, set for first players turn */
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
		break;
	case GGZDMOD_STATE_DONE:
		/* The game is over and will be destroyed */
		break;
	case GGZDMOD_STATE_RESTORED:
		/* The game is being restored. */
		/* FIXME: we need to read in a savegame here. */
		break;
	}
}


/* Convenience function: returns whether all seats are full */
static int seats_full(void)
{
	/* This calculation is a bit inefficient, but that's OK */
	return ggzdmod_count_seats(ttt_game.ggz, GGZ_SEAT_OPEN) == 0
	  && ggzdmod_count_seats(ttt_game.ggz, GGZ_SEAT_RESERVED) == 0
	  && ggzdmod_count_seats(ttt_game.ggz, GGZ_SEAT_ABANDONED) == 0;
}


/* Convenience function: returns whether all seats are empty */
static int seats_empty(void)
{
	/* This calculation is a bit inefficient, but that's OK */
	return ggzdmod_count_seats(ttt_game.ggz, GGZ_SEAT_PLAYER) == 0
		&& ggzdmod_count_spectators(ttt_game.ggz) == 0;
}


#ifdef GGZSPECTATORS
/* Callback for joining and leaving spectators */
static void game_handle_ggz_spectator_seat(GGZdMod *ggz, GGZdModEvent event,
					   const void *data)
{
	const GGZSpectator *old_spectator = data;
	GGZSpectator spectator;

	spectator = ggzdmod_get_spectator(ggz, old_spectator->num);

	if (event == GGZDMOD_EVENT_SPECTATOR_JOIN) {
		GGZCommIO *io = ggzcomm_io_allocate(spectator.fd);

		assert(spectator.playerdata == NULL);

		ggzdmod_set_playerdata(ggz, true, spectator.num, io);
		spectator = ggzdmod_get_spectator(ggz, old_spectator->num);
		assert(spectator.playerdata == io);
	}

	if (spectator.name) {
		game_send_sync(spectator.playerdata);
	}

	if (event == GGZDMOD_EVENT_SPECTATOR_LEAVE) {
		ggzcomm_io_free(old_spectator->playerdata);
	}

#if 0
	if (seats_empty())
		ggzdmod_set_state(ttt_game.ggz, GGZDMOD_STATE_DONE);
#endif
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
	else {
#ifdef GGZGAMERESUME
		new_state = GGZDMOD_STATE_WAITING;
#else
		new_state = GGZDMOD_STATE_DONE;
#endif
	}

	if (event == GGZDMOD_EVENT_LEAVE) {
		ggzcomm_io_free(old_seat->playerdata);
	} else if (event == GGZDMOD_EVENT_JOIN) {
		GGZCommIO *io = ggzcomm_io_allocate(new_seat.fd);

		assert(new_seat.playerdata == NULL);
		ggzdmod_set_playerdata(ggz, false, new_seat.num, io);
		new_seat = ggzdmod_get_seat(ggz, old_seat->num);
		assert(new_seat.playerdata == io);
	}

	if (new_seat.type == GGZ_SEAT_PLAYER) {
#ifdef GGZGAMERESUME
		/* If we're continuing a game, send sync to new player */
		if (variables.turn != -1)
			game_send_sync(new_seat.playerdata);
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


/* Handle message from player (via ggzcomm) */
static void game_handle_ggz_player(GGZdMod *ggz, GGZdModEvent event,
				   const void *data)
{
	GGZSeat seat;

	save_num = *(int*)data;
	seat = ggzdmod_get_seat(ggz, save_num);

	ggzcomm_network_main(seat.playerdata);
}

/* Callback for network messages (via ggzcomm) */
static void game_network_data(int opcode)
{
	int num;
	GGZSeat seat = ggzdmod_get_seat(ttt_game.ggz, save_num);
	GGZCommIO *io = seat.playerdata;

	ggzdmod_log(ttt_game.ggz, "Network data: opcode=%i", opcode);

	num = save_num;

	switch (opcode) {

	case sndmove:
		if (game_read_move(num, &variables.move_c) == 0)
			game_do_move(variables.move_c);
		break;
	case reqsync:
		game_send_sync(io);
		break;
	}
}

/* Callback for network errors (via ggzcomm) */
static void game_network_error(void)	
{
	ggzdmod_log(ttt_game.ggz, "Network error!");
}

#ifdef GGZSPECTATORS
/* Handle message from spectator */
/* FIXME: doesn't use ggzcomm yet */
static void game_handle_ggz_spectator_data(GGZdMod *ggz, GGZdModEvent event,
					   const void *data)
{
	const int *num_ptr = data;
	const int num = *num_ptr;
	int op;
	GGZSpectator spectator = ggzdmod_get_spectator(ggz, num);
	GGZCommIO *io = spectator.playerdata;

#if TTT_USE_DIO
	ggz_dio_get_int(io->dio, &op);
#else
	ggz_read_int(io->fd, &op);
#endif

	switch (op) {
	case TTT_REQ_SYNC:
		game_send_sync(io);
		break;
	default:
		ggzdmod_log(ggz, "Unrecognized spectator opcode %d.", op);
	}
}
#endif

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

		ggzcomm_msgmove(seat.playerdata);
	}
	return 0;
}


/* Send out board layout */
static int game_send_sync(GGZCommIO *io)
{	
#if TTT_USE_DIO
	ggzdmod_log(ttt_game.ggz, "Handling sync for fd %d",
		    ggz_dio_get_socket(io->dio));
#else
	ggzdmod_log(ttt_game.ggz, "Handling sync for fd %d",
		    io->fd);
#endif

	ggzcomm_sndsync(io);

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

			ggzcomm_msggameover(seat.playerdata);
		}
	}

#ifdef GGZSPECTATORS
	for (i = 0; i < ggzdmod_get_max_num_spectators(ttt_game.ggz); i++)
	{
		spectator = ggzdmod_get_spectator(ttt_game.ggz, i);
		if (spectator.fd < 0) continue;

		variables.winner = winner;

		ggzcomm_msggameover(spectator.playerdata);
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
	
	ggzdmod_log(ttt_game.ggz, "Handling move for player %d: %i", num, *move);

	/* Check validity of move */
	status = game_check_move(num, *move);

	/* Send back move status */
	ggzdmod_log(ttt_game.ggz, "Send back result: %i", status);

	variables.status = status;
	ggzcomm_rspmove(seat.playerdata);

	/* If move simply invalid, ask for resubmit */
	if ( (status == TTT_ERR_BOUND || status == TTT_ERR_FULL)
	     && game_req_move(num) < 0)
		return -1;

	if (status != TTT_OK)
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
#ifdef GGZBOTHASNAME
		GGZSeat seat;
		int difficulty;
		char *botclass;

		difficulty = 1;
		seat = ggzdmod_get_seat(ttt_game.ggz, variables.turn);
		botclass = ggzdmod_get_bot_class(ttt_game.ggz, seat.name);
		if (!ggz_strcmp(botclass, "easy"))
			difficulty = 0;
		if (!ggz_strcmp(botclass, "hard"))
			difficulty = 1;
		if (botclass)
			ggz_free(botclass);
#endif
		move = ai_findmove(variables.turn, difficulty, variables.space);
		game_do_move(move);
	} else {
		game_req_move(variables.turn);
	}
#else
	game_req_move(variables.turn);
#endif

	return 0;
}


/* Request move from current player */
static int game_req_move(int num)
{
	GGZSeat seat = ggzdmod_get_seat(ttt_game.ggz, num);

	ggzdmod_log(ttt_game.ggz, "Requesting move from player %d on fd %d", num, seat.fd);

	ggzcomm_reqmove(seat.playerdata);

	return 0;
}


/* Applying a move */
static int game_do_move(int move)
{
	char victor;
#ifdef GGZSPECTATORS
	int i;
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
	for(i = 0; i < ggzdmod_get_max_num_spectators(ttt_game.ggz); i++) {
		GGZSpectator spectator
		  = ggzdmod_get_spectator(ttt_game.ggz, i);

		if (spectator.fd < 0) continue;

		variables.player = variables.turn;
		variables.move = move;

		ggzcomm_msgmove(spectator.playerdata);
	}
#endif

#ifdef GGZSAVEDGAMES
	game_save("player%i move %i %i", variables.turn + 1, move / 3, move % 3);
#endif

	if ( (victor = game_check_win()) < 0) {
		variables.turn = (variables.turn + 1) % 2;
		game_next_move();
	} else {
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
	if (variables.space[move] != TTT_EMPTY)
		return TTT_ERR_FULL;

	return 0;
}


/* Check for a win */
static char game_check_win(void)
{
	/* Check horizontals */
	if (variables.space[0] == variables.space[1]
	    && variables.space[1] == variables.space[2]
	    && variables.space[2] != TTT_EMPTY)
		return variables.space[0];

	if (variables.space[3] == variables.space[4]
	    && variables.space[4] == variables.space[5]
	    && variables.space[5] != TTT_EMPTY)
		return variables.space[3];
			
	if (variables.space[6] == variables.space[7]
	    && variables.space[7] == variables.space[8]
	    && variables.space[8] != TTT_EMPTY)
		return variables.space[6];

	/* Check verticals */
	if (variables.space[0] == variables.space[3]
	    && variables.space[3] == variables.space[6]
	    && variables.space[6] != TTT_EMPTY)
		return variables.space[0];

	if (variables.space[1] == variables.space[4]
	    && variables.space[4] == variables.space[7]
	    && variables.space[7] != TTT_EMPTY)
		return variables.space[1];

	if (variables.space[2] == variables.space[5]
	    && variables.space[5] == variables.space[8]
	    && variables.space[8] != TTT_EMPTY)
		return variables.space[2];
	
	/* Check diagonals */
	if (variables.space[0] == variables.space[4]
	    && variables.space[4] == variables.space[8]
	    && variables.space[8] != TTT_EMPTY)
		return variables.space[0];

	if (variables.space[2] == variables.space[4]
	    && variables.space[4] == variables.space[6]
	    && variables.space[6] != TTT_EMPTY)
		return variables.space[2];

	/* No one won yet */
	if (ttt_game.move_count == 9)
		return TTT_NO_WINNER;  /* Cat's game */

	return -1; /* Error */
}

#ifdef GGZSAVEDGAMES
#define TEMPLATE "savegame.XXXXXX"

/* Saves the progress of a game */
static void game_save(char *fmt, ...)
{
	int fd;
	char *savegamepath, *savegamename;
	char buffer[1024];
	va_list ap;

	if(!ttt_game.savegame) {
		savegamepath = strdup(GGZDSTATEDIR "/gamedata/TicTacToe/" TEMPLATE);
		fd = mkstemp(savegamepath);
		savegamename = strdup(savegamepath + strlen(savegamepath) - strlen(TEMPLATE));
		free(savegamepath);
		if(fd < 0) return;
		ttt_game.savegame = fdopen(fd, "w");
		if(!ttt_game.savegame) return;

		ggzdmod_report_savegame(ttt_game.ggz, savegamename);
		free(savegamename);
	}

	va_start(ap, fmt);
	vsnprintf(buffer, sizeof(buffer), fmt, ap);
	va_end(ap);

	fprintf(ttt_game.savegame, "%s\n", buffer);
	fflush(ttt_game.savegame);
}
#endif

