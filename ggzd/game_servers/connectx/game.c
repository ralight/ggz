/*
 * File: game.c
 * Author: Rich Gade
 * Project: GGZ ConnectX game module
 * Date: 27th June 2001
 * Desc: Game functions
 * $Id: game.c 5299 2002-12-18 11:49:40Z oojah $
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

#ifdef HAVE_CONFIG_H
#  include <config.h>			/* Site-specific config */
#endif

#include <stdlib.h>
#include <string.h>
#include <ggz.h>

#include "game.h"

#define GGZSTATISTICS
#define GGZSPECTATORS

#ifdef GGZSTATISTICS
#include "ggz_stats.h"
#endif

/* Data structure for ConnectX game */
struct connectx_game_t {
	GGZdMod *ggz; /* GGZ data object */
	char boardheight;
	char boardwidth;
	char connectlength;
	int board[MAX_BOARDWIDTH][MAX_BOARDHEIGHT];
	char state;
	char turn;
	int optionsseat;
};

/* Escape protocol */
/* Messages from server */
#define CONNECTX_MSG_SEAT     0
#define CONNECTX_MSG_PLAYERS  1
#define CONNECTX_MSG_MOVE     2
#define CONNECTX_MSG_GAMEOVER 3
#define CONNECTX_REQ_MOVE     4
#define CONNECTX_RSP_MOVE     5
#define CONNECTX_SND_SYNC     6
#define CONNECTX_MSG_OPTIONS  7
#define CONNECTX_REQ_OPTIONS  8
#define CONNECTX_MSG_CHAT     9
#define CONNECTX_RSP_CHAT    10
#ifdef GGZSTATISTICS
#define CONNECTX_SND_STATS    11
#endif

/* Messages from client */
#define CONNECTX_SND_MOVE     0
#define CONNECTX_REQ_SYNC     1
#define CONNECTX_SND_OPTIONS  2
#define CONNECTX_REQ_NEWGAME  3
#ifdef GGZSTATISTICS
#define CONNECTX_REQ_STATS    4
#endif

/* Move errors */
#define CONNECTX_ERR_STATE   -1
#define CONNECTX_ERR_TURN    -2
#define CONNECTX_ERR_BOUND   -3
#define CONNECTX_ERR_FULL    -4

/* Escape game states */
#define CONNECTX_STATE_INIT        0 /* Table has been started, no players joined */
#define CONNECTX_STATE_OPTIONS     1 /* Options have been requested */
#define CONNECTX_STATE_WAITING     2 /* Waiting for all players to join, options have been received */
#define CONNECTX_STATE_PLAYING     3 /* Playing */
#define CONNECTX_STATE_DONE        4

/* Global game variables */
struct connectx_game_t connectx_game;

/* Network IO functions */
static int game_send_seat(int seat);
static int game_send_players(void);
static int game_send_move(int num, int move);
static int game_send_sync(int fd);
#ifdef GGZSTATISTICS
static int game_send_stats(int num);
#endif
static int game_send_gameover(int winner);
static int game_read_move(int num, int* move);

static int seats_full(void);
static int seats_empty(void);

static int game_next_move(void);
static int game_req_move(int num);
static int game_do_move(int move);
static int game_bot_move(int me);
static char game_check_move(int num, int move);
static int game_send_options_request(int fd);
static int game_send_options(int fd);
static int game_read_options(int seat);
//static int game_handle_newgame(int seat);


static char game_check_win(void);


/* Setup game state and board */
void game_init(GGZdMod *ggz)
{
	ggzdmod_log(connectx_game.ggz, "game_init()");
	connectx_game.turn = -1;
	connectx_game.state = CONNECTX_STATE_INIT;
	connectx_game.ggz = ggz;
	/* Board is set up after the options have been collected */
}

void game_handle_ggz_state(GGZdMod *ggz, GGZdModEvent event, void *data)
{
	switch(ggzdmod_get_state(ggz)) {
	case GGZDMOD_STATE_PLAYING:
		if(connectx_game.turn == -1){
			connectx_game.turn = 0;
			game_next_move();
		}
	default:
		break;
	}
}

static int seats_full(void){
	return ggzdmod_count_seats(connectx_game.ggz, GGZ_SEAT_OPEN) == 0
		&& ggzdmod_count_seats(connectx_game.ggz, GGZ_SEAT_RESERVED) == 0;
}

static int seats_empty(void){
	return ggzdmod_count_seats(connectx_game.ggz, GGZ_SEAT_OPEN) == 0
		&& ggzdmod_count_spectators(connectx_game.ggz) == 0;
}

#ifdef GGZSPECTATORS
void game_handle_ggz_spectator_join(GGZdMod *ggz, GGZdModEvent event, void *data)
{
	int i, fd;
	GGZSpectator *old_spectator = data;
	GGZSpectator spectator;
	GGZSeat seat;

	spectator = ggzdmod_get_spectator(ggz, old_spectator->num);
	fd = spectator.fd;

	if(ggz_write_int(fd, CONNECTX_MSG_PLAYERS) < 0)
		return;
	for(i = 0; i < 2; i++){
		seat = ggzdmod_get_seat(connectx_game.ggz, i);
		if(ggz_write_int(fd, seat.type) < 0)
			return;
		if(seat.type != GGZ_SEAT_OPEN
				&& ggz_write_string(fd, seat.name) < 0)
			return;
		if(game_send_sync(seat.fd) < 0)
			return;
	}
}

void game_handle_ggz_spectator_leave(GGZdMod *ggz, GGZdModEvent event, void *data)
{
	if(seats_empty())
		ggzdmod_set_state(connectx_game.ggz, GGZDMOD_STATE_DONE);
}
#endif

/* Callback for ggzdmod JOIN, LEAVE and SEAT events */
void game_handle_ggz_seat(GGZdMod *ggz, GGZdModEvent event, void *data)
{
	GGZdModState new_state;
	GGZSeat *old_seat = data;
	GGZSeat new_seat = ggzdmod_get_seat(ggz, old_seat->num);

	if(seats_full()){
		if(connectx_game.state == CONNECTX_STATE_WAITING){
			connectx_game.state = CONNECTX_STATE_PLAYING;
			new_state = GGZDMOD_STATE_PLAYING;
		}else{
			new_state = GGZDMOD_STATE_WAITING;
		}
	}else if (seats_empty())
		new_state = GGZDMOD_STATE_DONE;
	else
		new_state = GGZDMOD_STATE_WAITING;

	if(new_seat.type == GGZ_SEAT_PLAYER) {
		game_send_players();
		game_send_seat(new_seat.num);
		/* If we're continuing a game, send sync to new player */
		if(connectx_game.turn != -1){
			game_send_options(new_seat.fd);
			game_send_sync(new_seat.fd);
		}else if(connectx_game.state == CONNECTX_STATE_INIT && event == GGZDMOD_EVENT_JOIN){
			game_send_options_request(new_seat.fd);
			connectx_game.state = CONNECTX_STATE_OPTIONS;
		}else{
			if(connectx_game.state == CONNECTX_STATE_WAITING){
				game_send_options(new_seat.fd);
			}
		}
	}

	ggzdmod_set_state(connectx_game.ggz, new_state);
}

/* Handle message from player */
void game_handle_ggz_player(GGZdMod *ggz, GGZdModEvent event, void *data)
{
	int num = *(int*)data;
	int fd, op, move;
	GGZSeat seat;

	seat = ggzdmod_get_seat(ggz, num);
	fd = seat.fd;

	if(ggz_read_int(fd, &op) < 0)
		return;

	switch(op){
	
	case CONNECTX_SND_MOVE:
		if (game_read_move(num, &move) == 0)
			game_do_move(move);
		break;

	case CONNECTX_REQ_SYNC:
		game_send_sync(fd);
		break;

	case CONNECTX_SND_OPTIONS:
		game_read_options(num);
		break;

	case CONNECTX_REQ_NEWGAME:
//		game_handle_newgame(num);
		break;

#ifdef GGZSTATISTICS
	case CONNECTX_REQ_STATS:
		game_send_stats(num);
		break;
#endif
	default:
		ggzdmod_log(ggz, "Unrecognised player opcode %d.", op);
	}
}

#ifdef GGZSPECTATORS
/* handle message from spectator */
void game_handle_ggz_spectator(GGZdMod *ggz, GGZdModEvent event, void *data)
{
	int num = *(int*)data;
	int fd, op;
	GGZSeat seat;

	seat = ggzdmod_get_seat(ggz, num);
	fd = seat.fd;

	if(ggz_read_int(fd, &op) < 0)
		return;

	switch(op){
	case CONNECTX_REQ_SYNC:
		game_send_sync(fd);
		break;
	default:
		ggzdmod_log(ggz, "Unrecognised spectator opcode %d.", op);
	}
}
#endif

/* Send out seat assignment */
static int game_send_seat(int num)
{
	GGZSeat seat;
	
	seat = ggzdmod_get_seat(connectx_game.ggz, num);
	ggzdmod_log(connectx_game.ggz, "Sending player %d's seat num", num);

	if(ggz_write_int(seat.fd, CONNECTX_MSG_SEAT) < 0
		|| ggz_write_int(seat.fd, num) < 0)
			return -1;

	return 0;
}

/* Send out player list to everybody */
static int game_send_players(void)
{
	int i, j, fd;
	GGZSeat seat;

	for(j = 0; j < 2; j++){
		seat = ggzdmod_get_seat(connectx_game.ggz, j);
		fd = seat.fd;
		
		if (fd != -1) {
			ggzdmod_log(connectx_game.ggz, "Sending playerlist to player %d", j);

			if(ggz_write_int(fd, CONNECTX_MSG_PLAYERS) < 0)
				return -1;

			for(i = 0; i < 2; i++){
				seat = ggzdmod_get_seat(connectx_game.ggz, i);
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

/* Send outmove for player: num */
static int game_send_move(int num, int column)
{
	int opponent = (num + 1) % 2;
	GGZSeat seat = ggzdmod_get_seat(connectx_game.ggz, opponent);

	/* If player is a computer we don't need to send */
	if (seat.fd != -1){
		ggzdmod_log(connectx_game.ggz, "Sending player %d's move to player %d", num, opponent);
		
		if(ggz_write_int(seat.fd, CONNECTX_MSG_MOVE) < 0
		   || ggz_write_int(seat.fd, column) < 0)
			return -1;
	}
	return 0;
}

#ifdef GGZSTATISTICS
/* Send game statistics */
static int game_send_stats(int num)
{
	int wins, losses, ties;
	GGZStats *stats;
	GGZSeat seat;

	seat = ggzdmod_get_seat(connectx_game.ggz, num);

	ggzdmod_log(connectx_game.ggz, "Handling statistics for player %d", num);

	stats = ggzstats_new(connectx_game.ggz);
	ggzstats_get_record(stats, num, &wins, &losses, &ties);
	ggzstats_free(stats);

	if(ggz_write_int(seat.fd, CONNECTX_SND_STATS) < 0
		|| ggz_write_int(seat.fd, wins) < 0
		|| ggz_write_int(seat.fd, losses) < 0
	   || ggz_write_int(seat.fd, ties) < 0)
		return -1;
	return 0;
}
#endif

/* Send out board layout */
static int game_send_sync(int fd)
{
	int p, q;

	ggzdmod_log(connectx_game.ggz, "Handling sync for fd %d", fd);

	if(ggz_write_int(fd, CONNECTX_SND_SYNC) < 0
	   || ggz_write_char(fd, connectx_game.boardheight) < 0
	   || ggz_write_char(fd, connectx_game.boardwidth) < 0
	   || ggz_write_char(fd, connectx_game.connectlength) < 0
	   || ggz_write_char(fd, connectx_game.turn) < 0)
		return -1;

	for(p=0; p<connectx_game.boardwidth; p++)
		for(q=0; q<connectx_game.boardheight; q++)
			if(ggz_write_char(fd, connectx_game.board[p][q]) < 0)
				return -1;

	return 0;		
}

/* Send out game over message */
static int game_send_gameover(int winner)
{
	int i;
	GGZSeat seat;
#ifdef GGZSPECTATORS
	GGZSpectator spectator;
#endif

#ifdef GGZSTATISTICS
	GGZStats *stats;

	stats = ggzstats_new(connectx_game.ggz);
	if(winner < 2) {
		ggzstats_set_game_winner(stats, winner, 1.0);
		ggzstats_set_game_winner(stats, (winner + 1) % 2, 0.0);
	} else{
		/* Draw */
		ggzstats_set_game_winner(stats, 0, 0.5);
		ggzstats_set_game_winner(stats, 1, 0.5);
	}
	ggzstats_recalculate_ratings(stats);
	ggzstats_free(stats);
#endif

	for(i = 0; i < 2; i++){
		seat = ggzdmod_get_seat(connectx_game.ggz, i);
		if (seat.fd != -1){
			ggzdmod_log(connectx_game.ggz, "sending game-over to player %d", i);

			if(ggz_write_int(seat.fd, CONNECTX_MSG_GAMEOVER) < 0
			   || ggz_write_char(seat.fd, winner) < 0)
				return -1;
		}
	}

#ifdef GGZSPECTATORS
	for(i = 0; i < ggzdmod_get_max_num_spectators(connectx_game.ggz); i++)
	{
		spectator = ggzdmod_get_spectator(connectx_game.ggz, i);
		if(spectator.fd < 0) continue;
		ggz_write_int(spectator.fd, CONNECTX_MSG_GAMEOVER);
		ggz_write_char(spectator.fd, winner);
	}
#endif

	return 0;
}

/* Read incoming move from player */
static int game_read_move(int num, int* move)
{
	GGZSeat seat = ggzdmod_get_seat(connectx_game.ggz, num);
	char status;

	ggzdmod_log(connectx_game.ggz, "Handling move for player %d", num);
	if (ggz_read_int(seat.fd, move) < 0)
		return -1;

	/* Check validity of move */
	status = game_check_move(num, *move);

	/* Send back move status */
	if (ggz_write_int(seat.fd, CONNECTX_RSP_MOVE) < 0
	   || ggz_write_char(seat.fd, status) < 0
	   || ggz_write_int(seat.fd, *move) < 0)
		return -1;

	if( (status == -3 || status == -4)
	   && game_req_move(num) < 0)
		return -1;

	if(status != 0)
		return 1;

	return 0;
}

/* Do the next move */
static int game_next_move(void)
{
	int move, num = connectx_game.turn;
	GGZSeat seat = ggzdmod_get_seat(connectx_game.ggz, num);

	if(seat.type == GGZ_SEAT_BOT){
		move = game_bot_move(num);
		game_do_move(move);
	}else
		game_req_move(num);

	return 0;
}

/* Request move from current player */
static int game_req_move(int num)
{
	GGZSeat seat = ggzdmod_get_seat(connectx_game.ggz, num);

	ggzdmod_log(connectx_game.ggz, "Requesting move from player %d on %d", num, seat.fd);

	if(ggz_write_int(seat.fd, CONNECTX_REQ_MOVE) < 0){
		ggzdmod_log(connectx_game.ggz, "Error requesting move from player %d", num);
		return -1;
	}

	return 0;
}

static int game_do_move(int move)
{
	/* Only called if move is valid */
	char victor;
	int i;
#ifdef GGZSPECTATORS
	int fd;
#endif

	/* FIXME: we should not return on a network error within this
		function - that will fubar the whole game since the turn won't
		be incremented. --JDS */

	if (ggzdmod_get_state(connectx_game.ggz) != GGZDMOD_STATE_PLAYING)
		return -1;

	ggzdmod_log(connectx_game.ggz, "Player %d moving in column %d", connectx_game.turn, move);

	for(i=0; i<connectx_game.boardheight; i++){
		if(connectx_game.board[move][i]==dtEmpty){
			if(connectx_game.turn % 2){
				connectx_game.board[move][i]=dtPlayer1;
			}else{
				connectx_game.board[move][i]=dtPlayer2;
			}
			break;
		}
	}

	game_send_move(connectx_game.turn, move);

#ifdef GGZSPECTATORS
	for(i=0; i < ggzdmod_get_max_num_spectators(connectx_game.ggz); i++){
		fd = (ggzdmod_get_spectator(connectx_game.ggz, i)).fd;
		if(fd < 0) continue;
		ggz_write_int(fd, CONNECTX_MSG_MOVE);
		ggz_write_int(fd, move);
	}
#endif

	if((victor = game_check_win()) < 0){
		connectx_game.turn = (connectx_game.turn + 1) % 2;
		
		game_next_move();
	}else{
		game_send_gameover(victor);
		/* Notify GGZ server of game over */
		ggzdmod_set_state(connectx_game.ggz, GGZDMOD_STATE_DONE);
	}

	return 0;
}

/* FIXME - write proper AI! */
/* Do bot moves */
static int game_bot_move(int me)
{
	int botmove;

	do{
		botmove = (random() % connectx_game.boardwidth);
	}while(game_check_move(me, botmove)!=0);
	ggzdmod_log(connectx_game.ggz, "boardwidth = %d", connectx_game.boardwidth);
	ggzdmod_log(connectx_game.ggz, "botmove = %d", botmove);
	return botmove;
}

/* Check for valid move */
static char game_check_move(int num, int move)
{
	/* Check for correct state */
	if(ggzdmod_get_state(connectx_game.ggz) != GGZDMOD_STATE_PLAYING)
		return CONNECTX_ERR_STATE;

	/* Check for correct turn */
	if(num != connectx_game.turn)
		return CONNECTX_ERR_TURN;

	/* Check for out of bounds move */
	if(move<0 || move >= connectx_game.boardwidth)
		return CONNECTX_ERR_BOUND;

	if(connectx_game.board[move][connectx_game.boardheight-1]!=dtEmpty)
		return CONNECTX_ERR_FULL;

	return 0;
}

/* Check for a win */
static char game_check_win(void)
{
	//int i;
	//int count=0;

/* FIXME - write this function
	if((connectx_game.x>=connectx_game.wallwidth) && (connectx_game.x<=connectx_game.wallwidth+connectx_game.goalwidth))
	{
		if(!connectx_game.y)
			return 0;
		else if(connectx_game.y==connectx_game.boardheight)
			return 1;
	}

	for(i=1; i<10; i++)
		if((connectx_game.board[connectx_game.x][connectx_game.y][i]!=dtEmpty) && (i!=5))
			count++;

	if(count==8){
		ggzdmod_log(connectx_game.ggz, "game_checkwin() count = 8");
		ggzdmod_log(connectx_game.ggz, "\tconnectx_game.boardheight = %d",connectx_game.boardheight);
		ggzdmod_log(connectx_game.ggz, "\connectx_game.wallwidth+connectx_game.goalwidth = %d",connectx_game.wallwidth+connectx_game.goalwidth);
		ggzdmod_log(connectx_game.ggz, "\tconnectx_game.x = %d",connectx_game.x);
		ggzdmod_log(connectx_game.ggz, "\tconnectx_game.y = %d",connectx_game.y);
		for(i=1; i<10; i++){
			if(i!=5){
				ggzdmod_log(connectx_game.ggz, "\tconnectx_game.board[][][%d] = %d",i, connectx_game.board[connectx_game.x][connectx_game.y][i]);
			}
		}	
		return 2;
	}
*/
	/* Game not over yet */
	return -1;
}

/* Send a request for client to set options */
static int game_send_options_request(int fd)
{
	ggzdmod_log(connectx_game.ggz, "Sending options request to fd %d",fd);

	if(ggz_write_int(fd, CONNECTX_REQ_OPTIONS) < 0
	   || ggz_write_char(fd, (char)MIN_BOARDWIDTH)
	   || ggz_write_char(fd, (char)MAX_BOARDWIDTH)
	   || ggz_write_char(fd, (char)MIN_BOARDHEIGHT)
	   || ggz_write_char(fd, (char)MAX_BOARDHEIGHT)
	   || ggz_write_char(fd, (char)MIN_CONNECTLENGTH)
	   || ggz_write_char(fd, (char)MAX_CONNECTLENGTH))
		return -1;

	connectx_game.state = CONNECTX_STATE_OPTIONS;
	return 0;
}

/* Send out options */
static int game_send_options(int fd)
{
	ggzdmod_log(connectx_game.ggz, "game_send_options(%d)",fd);

	if(fd < 0)
		return -1;
	
	if(ggz_write_int(fd, CONNECTX_MSG_OPTIONS) < 0
	   || ggz_write_char(fd, connectx_game.boardheight) < 0
	   || ggz_write_char(fd, connectx_game.boardwidth) < 0
	   || ggz_write_char(fd, connectx_game.connectlength) < 0)
		return -1;

	ggzdmod_log(connectx_game.ggz, "Options sent ok with");
	ggzdmod_log(connectx_game.ggz, "\tboardheight=%d", connectx_game.boardheight);
	ggzdmod_log(connectx_game.ggz, "\tboardwidth=%d", connectx_game.boardwidth);
	ggzdmod_log(connectx_game.ggz, "\tconectlength=%d", connectx_game.connectlength);

	return 0;
}

/* Get options from client */
static int game_read_options(int seat)
{
	int fd = ggzdmod_get_seat(connectx_game.ggz, seat).fd;
	int p, q;

	ggzdmod_log(connectx_game.ggz, "game_get_options(%d)", seat);

	if(ggz_read_char(fd, &connectx_game.boardheight) < 0
	   || ggz_read_char(fd, &connectx_game.boardwidth) < 0
	   || ggz_read_char(fd, &connectx_game.connectlength) < 0)
		return -1;

	// FIXME - add bounds checking to ensure eg. connectx_game.boardheight isn't larger than MAXBOARDHEIGHT

	ggzdmod_log(connectx_game.ggz, "Options recieved ok");
	ggzdmod_log(connectx_game.ggz, "\tboardheight=%d", connectx_game.boardheight);
	ggzdmod_log(connectx_game.ggz, "\tboardwidth=%d", connectx_game.boardwidth);
	ggzdmod_log(connectx_game.ggz, "\tconnectlength=%d", connectx_game.connectlength);

	/* Mark everything as empty */
	for(p = 0;p<=connectx_game.boardwidth;p++){
		for(q = 0;q<=connectx_game.boardheight+1;q++){
			connectx_game.board[p][q] = dtEmpty;
      	}
	}

	//game_send_options(0);
	game_send_options(ggzdmod_get_seat(connectx_game.ggz, 1-seat).fd);
	
	if(seats_full()){
		connectx_game.state = CONNECTX_STATE_PLAYING;
		ggzdmod_set_state(connectx_game.ggz, GGZDMOD_STATE_PLAYING);
	}else{
		connectx_game.state = CONNECTX_STATE_WAITING;
		ggzdmod_set_state(connectx_game.ggz, GGZDMOD_STATE_WAITING);
	}
	return 0;
}
#if 0
static int game_handle_newgame(int seat)
{
	int status = 0;

	/* The first person to say Yes gets to choose the options */
	if(connectx_game.play_again == 0) {
		connectx_game.state = CONNECTX_STATE_OPTIONS;
		status = game_send_options_request(seat);
	}

	connectx_game.play_again++;

	/* Simulate that the bot wants to play another game */
	if(ggzdmod_get_seat(connectx_game.ggz, (seat+1)%2).type == GGZ_SEAT_BOT)
		connectx_game.play_again++;

	/* Issue the game start if second answer comes */
	/* and options are already setup to go */
	if(connectx_game.state == CONNECTX_STATE_WAIT
	   && connectx_game.play_again == 2
	   && seats_full()) {
		connectx_game.turn = 0;
		connectx_game.state = CONNECTX_STATE_PLAYING;
		repeatmove = 0;
		game_move();
	}

	return status;
}
#endif

