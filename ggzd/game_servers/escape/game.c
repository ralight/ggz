/*
 * File: game.c
 * Author: Rich Gade (Modified for Escape by Roger Light)
 * Project: GGZ Escape game module
 * Date: 27th June 2001
 * Desc: Game functions
 * $Id: game.c 8780 2007-01-02 12:15:46Z josef $
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

/* Data structure for Escape game */
struct escape_game_t {
	GGZdMod *ggz; /* GGZ data object */
	char boardheight;
	char goalwidth;
	char wallwidth;
	int board[MAX_WALLWIDTH*2 + MAX_GOALWIDTH+3][MAX_BOARDHEIGHT+3][10];
	int x;
	int y;
	char state;
	char turn;
	unsigned char options[2];
	unsigned char play_again;
	int optionsseat;
};

/* Escape protocol */
/* Messages from server */
#define ESCAPE_MSG_SEAT     0
#define ESCAPE_MSG_PLAYERS  1
#define ESCAPE_MSG_MOVE     2
#define ESCAPE_MSG_GAMEOVER 3
#define ESCAPE_REQ_MOVE     4
#define ESCAPE_RSP_MOVE     5
#define ESCAPE_SND_SYNC     6
#define ESCAPE_MSG_OPTIONS  7
#define ESCAPE_REQ_OPTIONS  8
#define ESCAPE_MSG_CHAT     9
#define ESCAPE_RSP_CHAT    10

/* Messages from client */
#define ESCAPE_SND_MOVE     0
#define ESCAPE_REQ_SYNC     1
#define ESCAPE_SND_OPTIONS  2
#define ESCAPE_REQ_NEWGAME  3

/* Move errors */
#define ESCAPE_ERR_STATE   -1
#define ESCAPE_ERR_TURN    -2
#define ESCAPE_ERR_BOUND   -3
#define ESCAPE_ERR_FULL    -4

/* Escape game states */
#define ESCAPE_STATE_INIT        0 /* Table has been started, no players joined */
#define ESCAPE_STATE_OPTIONS     1 /* Options have been requested */
#define ESCAPE_STATE_WAITING     2 /* Waiting for all players to join, options have been received */
#define ESCAPE_STATE_PLAYING     3 /* Playing */
#define ESCAPE_STATE_DONE        4

/* Global game variables */
struct escape_game_t escape_game;

/* Network IO functions */
static int game_send_seat(int seat);
static int game_send_players(void);
static int game_send_move(int num, int move);
static int game_send_sync(int fd);
static int game_send_gameover(int winner);
static int game_read_move(int num, int* move);

static int seats_full(void);
static int seats_empty(void);

static int game_next_move(void);
static int game_req_move(int num);
static int game_do_move(int move);
static int game_bot_move(int me);
static int game_bot_count_moves(int move);
static char game_check_move(int num, int move);
static int game_send_options_request(int fd);
static int game_send_options(int fd);
static int game_read_options(int seat);
//static int game_handle_newgame(int seat);
static int revdir(int direction);


static char game_check_win(void);


/* Setup game state and board */
void game_init(GGZdMod *ggz)
{
	ggzdmod_log(escape_game.ggz, "game_init()");
	escape_game.turn = -1;
	escape_game.state = ESCAPE_STATE_INIT;
	escape_game.ggz = ggz;
	/* Board is set up after the options have been collected */
}

void game_handle_ggz_state(GGZdMod *ggz, GGZdModEvent event, const void *data)
{
	switch(ggzdmod_get_state(ggz)) {
	case GGZDMOD_STATE_PLAYING:
		if(escape_game.turn == -1){
			escape_game.turn = 0;
			game_next_move();
		}
	default:
		break;
	}
}

static int seats_full(void)
{
	return ggzdmod_count_seats(escape_game.ggz, GGZ_SEAT_OPEN) == 0
	  && ggzdmod_count_seats(escape_game.ggz, GGZ_SEAT_RESERVED) == 0
	  && ggzdmod_count_seats(escape_game.ggz, GGZ_SEAT_ABANDONED) == 0;
}

static int seats_empty(void){
	return ggzdmod_count_seats(escape_game.ggz, GGZ_SEAT_OPEN) == 0
		&& ggzdmod_count_spectators(escape_game.ggz) == 0;
}

#ifdef GGZSPECTATORS
void game_handle_ggz_spectator_join(GGZdMod *ggz,
				    GGZdModEvent event, const void *data)
{
	int i, fd;
	const GGZSpectator *old_spectator = data;
	GGZSpectator spectator;
	GGZSeat seat;

	spectator = ggzdmod_get_spectator(ggz, old_spectator->num);
	fd = spectator.fd;

	if(ggz_write_int(fd, ESCAPE_MSG_PLAYERS) < 0)
		return;
	for(i = 0; i < 2; i++){
		seat = ggzdmod_get_seat(escape_game.ggz, i);
		if(ggz_write_int(fd, seat.type) < 0)
			return;
		if(seat.type != GGZ_SEAT_OPEN
				&& ggz_write_string(fd, seat.name) < 0)
			return;
		if(game_send_sync(seat.fd) < 0)
			return;
	}
}

void game_handle_ggz_spectator_leave(GGZdMod *ggz,
				     GGZdModEvent event, const void *data)
{
	if(seats_empty())
		ggzdmod_set_state(escape_game.ggz, GGZDMOD_STATE_DONE);
}
#endif

/* Callback for ggzdmod JOIN, LEAVE and SEAT events */
void game_handle_ggz_seat(GGZdMod *ggz, GGZdModEvent event, const void *data)
{
	GGZdModState new_state;
	const GGZSeat *old_seat = data;
	GGZSeat new_seat = ggzdmod_get_seat(ggz, old_seat->num);

	if(seats_full()){
		if(escape_game.state == ESCAPE_STATE_WAITING){
			escape_game.state = ESCAPE_STATE_PLAYING;
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
		if(escape_game.turn != -1){
			game_send_options(new_seat.fd);
			game_send_sync(new_seat.fd);
		}else if(escape_game.state == ESCAPE_STATE_INIT && event == GGZDMOD_EVENT_JOIN){
			game_send_options_request(new_seat.fd);
			escape_game.state = ESCAPE_STATE_OPTIONS;
		}else{
			if(escape_game.state == ESCAPE_STATE_WAITING){
				game_send_options(new_seat.fd);
			}
		}
	}

	ggzdmod_set_state(escape_game.ggz, new_state);
}

/* Handle message from player */
void game_handle_ggz_player(GGZdMod *ggz,
			    GGZdModEvent event, const void *data)
{
	int num = *(int*)data;
	int fd, op, move;
	GGZSeat seat;

	seat = ggzdmod_get_seat(ggz, num);
	fd = seat.fd;

	if(ggz_read_int(fd, &op) < 0)
		return;

	switch(op){
	
	case ESCAPE_SND_MOVE:
		if (game_read_move(num, &move) == 0)
			game_do_move(move);
		break;

	case ESCAPE_REQ_SYNC:
		game_send_sync(fd);
		break;

	case ESCAPE_SND_OPTIONS:
		game_read_options(num);
		break;

	case ESCAPE_REQ_NEWGAME:
//		game_handle_newgame(num);
		break;

	default:
		ggzdmod_log(ggz, "Unrecognised player opcode %d.", op);
	}
}

#ifdef GGZSPECTATORS
/* handle message from spectator */
void game_handle_ggz_spectator(GGZdMod *ggz,
			       GGZdModEvent event, const void *data)
{
	int num = *(int*)data;
	int fd, op;
	GGZSeat seat;

	seat = ggzdmod_get_seat(ggz, num);
	fd = seat.fd;

	if(ggz_read_int(fd, &op) < 0)
		return;

	switch(op){
	case ESCAPE_REQ_SYNC:
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
	
	seat = ggzdmod_get_seat(escape_game.ggz, num);
	ggzdmod_log(escape_game.ggz, "Sending player %d's seat num", num);

	if(ggz_write_int(seat.fd, ESCAPE_MSG_SEAT) < 0
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
		seat = ggzdmod_get_seat(escape_game.ggz, j);
		fd = seat.fd;
		
		if (fd != -1) {
			ggzdmod_log(escape_game.ggz, "Sending playerlist to player %d", j);

			if(ggz_write_int(fd, ESCAPE_MSG_PLAYERS) < 0)
				return -1;

			for(i = 0; i < 2; i++){
				seat = ggzdmod_get_seat(escape_game.ggz, i);
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
static int game_send_move(int num, int direction)
{
	int opponent = (num + 1) % 2;
	GGZSeat seat = ggzdmod_get_seat(escape_game.ggz, opponent);

	/* If player is a computer we don't need to send */
	if (seat.fd != -1){
		ggzdmod_log(escape_game.ggz, "Sending player %d's move to player %d", num, opponent);
		
		if(ggz_write_int(seat.fd, ESCAPE_MSG_MOVE) < 0
		   || ggz_write_int(seat.fd, direction) < 0)
			return -1;
	}
	return 0;
}

/* Send out board layout */
static int game_send_sync(int fd)
{
	int p, q, d;

	ggzdmod_log(escape_game.ggz, "Handling sync for fd %d", fd);

	if(ggz_write_int(fd, ESCAPE_SND_SYNC) < 0
	   || ggz_write_char(fd, escape_game.boardheight) < 0
	   || ggz_write_char(fd, escape_game.goalwidth) < 0
	   || ggz_write_char(fd, escape_game.wallwidth) < 0
	   || ggz_write_char(fd, escape_game.turn) < 0
	   || ggz_write_int(fd, escape_game.x) < 0
	   || ggz_write_int(fd, escape_game.y) < 0)
		return -1;

	for(p=0; p<escape_game.goalwidth+escape_game.wallwidth*2; p++)
		for(q=0; q<escape_game.boardheight; p++)
			for(d=1; d<10; d++)
				if(ggz_write_char(fd, escape_game.board[p][q][d]) < 0)
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
	GGZGameResult results[2];

	for(i = 0; i < 2; i++){
		seat = ggzdmod_get_seat(escape_game.ggz, i);
		if (seat.fd != -1){
			ggzdmod_log(escape_game.ggz, "sending game-over to player %d", i);

			if(ggz_write_int(seat.fd, ESCAPE_MSG_GAMEOVER) < 0
			   || ggz_write_char(seat.fd, winner) < 0)
				return -1;
		}
	}

#ifdef GGZSPECTATORS
	for(i = 0; i < ggzdmod_get_max_num_spectators(escape_game.ggz); i++)
	{
		spectator = ggzdmod_get_spectator(escape_game.ggz, i);
		if(spectator.fd < 0) continue;
		ggz_write_int(spectator.fd, ESCAPE_MSG_GAMEOVER);
		ggz_write_char(spectator.fd, winner);
	}
#endif

	/* Report the results to GGZ. */
	if (winner < 2) {
		/* One player won. */
		results[winner] = GGZ_GAME_WIN;
		results[1 - winner] = GGZ_GAME_LOSS;
	} else if (winner == 2) {
		/* Draw. */
		results[0] = results[1] = GGZ_GAME_TIE;
	} else {
		/* One player forfeited. */
		results[winner - 3] = GGZ_GAME_WIN;
		results[4 - winner] = GGZ_GAME_FORFEIT;
	}
	ggzdmod_report_game(escape_game.ggz, NULL, results, NULL);

	return 0;
}

/* Read incoming move from player */
static int game_read_move(int num, int* move)
{
	GGZSeat seat = ggzdmod_get_seat(escape_game.ggz, num);
	char status;

	ggzdmod_log(escape_game.ggz, "Handling move for player %d", num);
	if (ggz_read_int(seat.fd, move) < 0)
		return -1;

	/* Check validity of move */
	status = game_check_move(num, *move);

	/* Send back move status */
	if (ggz_write_int(seat.fd, ESCAPE_RSP_MOVE) < 0
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
	int move, num = escape_game.turn;
	GGZSeat seat = ggzdmod_get_seat(escape_game.ggz, num);

	if(seat.type == GGZ_SEAT_BOT){
		move = game_bot_move(num);
		game_do_move(move);
	}else
		game_req_move(num);

	return 0;
}

/* FIXME - maybe make client thinner by sending valid moves with ESCAPE_REQ_MOVE */
/* Request move from current player */
static int game_req_move(int num)
{
	GGZSeat seat = ggzdmod_get_seat(escape_game.ggz, num);

	ggzdmod_log(escape_game.ggz, "Requesting move from player %d on %d", num, seat.fd);

	if(ggz_write_int(seat.fd, ESCAPE_REQ_MOVE) < 0){
		ggzdmod_log(escape_game.ggz, "Error requesting move from player %d", num);
		return -1;
	}

	return 0;
}

static int game_do_move(int move)
{
	/* Only called if move is valid */
	char victor;
	int i;
	int newx, newy;
	int repeatmove = 0;
#ifdef GGZSPECTATORS
	int fd;
#endif

	/* FIXME: we should not return on a network error within this
		function - that will fubar the whole game since the turn won't
		be incremented. --JDS */

	if (ggzdmod_get_state(escape_game.ggz) != GGZDMOD_STATE_PLAYING)
		return -1;

	ggzdmod_log(escape_game.ggz, "Player %d moving in direction %d", escape_game.turn, move);

	switch(move){

	case 7: //northwest
		newx = escape_game.x - 1;
		newy = escape_game.y - 1;
		break;
	case 8: //north
		newx = escape_game.x;
		newy = escape_game.y - 1;
		break;
	case 9: //northeast
       	newx = escape_game.x + 1;
       	newy = escape_game.y - 1;
		break;
      case 6: //east
      	newx = escape_game.x + 1;
		newy = escape_game.y;
		break;
      case 3: //southeast
      	newx = escape_game.x + 1;
            newy = escape_game.y + 1;
		break;
	case 2: //south
		newx = escape_game.x;
		newy = escape_game.y + 1;
		break;
	case 1: //southwest
		newx = escape_game.x - 1;
		newy = escape_game.y + 1;
		break;
	case 4: //west
		newx = escape_game.x - 1;
		newy = escape_game.y;
		break;
	default:
		ggzdmod_log(escape_game.ggz, "ERROR: game_handle_move: wrong direction %d.", move);
		newx = escape_game.x;
		newy = escape_game.y;
	}

	if((newx<0)||(newy<0)||(newx>escape_game.wallwidth*2+escape_game.goalwidth)||(newy>escape_game.boardheight)||(newx==escape_game.x && newy==escape_game.y)){
//		status = ESCAPE_ERR_BOUND;
	}else {
		for(i=1; i<10; i++){
			if((escape_game.board[newx][newy][i]!=dtEmpty)&&(escape_game.board[newx][newy][i]!=dtCorner))
			{
				// give new move
				repeatmove=1;
			}
		}		

		if(escape_game.turn % 2){
			escape_game.board[escape_game.x][escape_game.y][move]=dtPlayer1;
			escape_game.board[newx][newy][revdir(move)]=dtPlayer1;
		}else{
			escape_game.board[escape_game.x][escape_game.y][move]=dtPlayer2;
			escape_game.board[newx][newy][revdir(move)]=dtPlayer2;
		}
		escape_game.x = newx;
		escape_game.y = newy;
	}


	game_send_move(escape_game.turn, move);

#ifdef GGZSPECTATORS
	for(i=0; i < ggzdmod_get_max_num_spectators(escape_game.ggz); i++){
		fd = (ggzdmod_get_spectator(escape_game.ggz, i)).fd;
		if(fd < 0) continue;
		ggz_write_int(fd, ESCAPE_MSG_MOVE);
		ggz_write_int(fd, move);
	}
#endif

	if((victor = game_check_win()) < 0){
		if(repeatmove == 0)
			escape_game.turn = (escape_game.turn + 1) % 2;
		
		game_next_move();
	}else{
		game_send_gameover(victor);
		/* Notify GGZ server of game over */
		ggzdmod_set_state(escape_game.ggz, GGZDMOD_STATE_DONE);
	}

	return 0;
}

/* Count the number of available if bot moves in a particular direction */
static int game_bot_count_moves(int move)
{
	int count = 0;
	int i;
	int newx, newy;

	switch(move){

	case 7: //northwest
		newx = escape_game.x - 1;
		newy = escape_game.y - 1;
		break;
	case 8: //north
		newx = escape_game.x;
		newy = escape_game.y - 1;
		break;
	case 9: //northeast
       		newx = escape_game.x + 1;
       		newy = escape_game.y - 1;
		break;
      	case 6: //east
      		newx = escape_game.x + 1;
		newy = escape_game.y;
		break;
      	case 3: //southeast
      		newx = escape_game.x + 1;
            	newy = escape_game.y + 1;
		break;
	case 2: //south
		newx = escape_game.x;
		newy = escape_game.y + 1;
		break;
	case 1: //southwest
		newx = escape_game.x - 1;
		newy = escape_game.y + 1;
		break;
	case 4: //west
		newx = escape_game.x - 1;
		newy = escape_game.y;
		break;
	default:
		return 0;
	}

	for(i=1; i < 10; i++){
		if(escape_game.board[newx][newy][i] == dtEmpty && i != 5)
			count++;
	}
	return count;
}

/* FIXME - write proper AI! */
/* Do bot moves */
static int game_bot_move(int me)
{
	int moves[10];
	int i;
	int tiemove=-1;
	int freecount=0;
	int botmove;

	for(i=1; i < 10; i++){
		moves[i] = escape_game.board[escape_game.x][escape_game.y][i];
		if(game_bot_count_moves(i)==0){
			moves[i]=dtTieMove; /* If we move here then the game will be tied */
			tiemove = i; /* Set tiemove so we can use it if there is no other choice */
		}
		if(moves[i]==dtEmpty && i!=5)
			freecount++;
	}
	moves[5] = dtBlocked;
	if(!freecount)
		return tiemove;


	if(random() % 3){ /* Do a proper type move */
		if(me == 0){ /* Player 1 - moving up */
			if(moves[8]==dtEmpty)
				return 8;

			if(moves[9]==dtEmpty)
				return 9;

			if(moves[7]==dtEmpty)
				return 7;

			if(moves[4]==dtEmpty)
				return 4;

			if(moves[6]==dtEmpty)
				return 6;

			do{
				botmove = (random() % 9) +1;
			}while(moves[botmove]!=dtEmpty);
		}else{
			if(moves[2]==dtEmpty)
				return 2;

			if(moves[1]==dtEmpty)
				return 1;

			if(moves[3]==dtEmpty)
				return 3;
	
			if(moves[4]==dtEmpty)
				return 4;
	
			if(moves[6]==dtEmpty)
				return 6;
	
			do{
				botmove = (random() % 9) + 1;
			}while(moves[botmove]!=dtEmpty);
		}
	}else{
		do{
			botmove = (random() % 9) + 1;
		}while(moves[botmove]!=dtEmpty);
	}
	return botmove;
}

/* Check for valid move */
static char game_check_move(int num, int move)
{
	int newx, newy;

	/* Check for correct state */
	if(ggzdmod_get_state(escape_game.ggz) != GGZDMOD_STATE_PLAYING)
		return ESCAPE_ERR_STATE;

	/* Check for correct turn */
	if(num != escape_game.turn)
		return ESCAPE_ERR_TURN;

	/* Check for out of bounds move */
	switch(move){

	case 7: //northwest
		newx = escape_game.x - 1;
		newy = escape_game.y - 1;
		break;
	case 8: //north
		newx = escape_game.x;
		newy = escape_game.y - 1;
		break;
	case 9: //northeast
		newx = escape_game.x + 1;
     		newy = escape_game.y - 1;
		break;
	case 6: //east
     		newx = escape_game.x + 1;
		newy = escape_game.y;
		break;
      	case 3: //southeast
      		newx = escape_game.x + 1;
      		newy = escape_game.y + 1;
		break;
      	case 2: //south
		newx = escape_game.x;
      		newy = escape_game.y + 1;
		break;
      	case 1: //southwest
     	 	newx = escape_game.x - 1;
      		newy = escape_game.y + 1;
		break;
	case 4: //west
      		newx = escape_game.x - 1;
		newy = escape_game.y;
		break;
	default:
		ggzdmod_log(escape_game.ggz, "ERROR: game_handle_move: wrong direction %d.", move);
		newx = escape_game.x;
		newy = escape_game.y;
	}
	if((newx<0)||(newy<0)||(newx>escape_game.wallwidth*2+escape_game.goalwidth)||(newy>escape_game.boardheight)||(newx==escape_game.x && newy==escape_game.y))
		return ESCAPE_ERR_BOUND;

	/* Check for duplicated or blocked move */
	if(escape_game.board[escape_game.x][escape_game.y][move] != dtEmpty)
		return ESCAPE_ERR_FULL;

	return 0;
}

/* FIXME - add "suicide" type of win with winner = 3, 4 */
/* Check for a win */
static char game_check_win(void)
{
	int i;
	int count=0;

	if((escape_game.x>=escape_game.wallwidth) && (escape_game.x<=escape_game.wallwidth+escape_game.goalwidth))
	{
		if(!escape_game.y)
			return 0;
		else if(escape_game.y==escape_game.boardheight)
			return 1;
	}

	for(i=1; i<10; i++)
		if((escape_game.board[escape_game.x][escape_game.y][i]!=dtEmpty) && (i!=5))
			count++;

	if(count==8){
		ggzdmod_log(escape_game.ggz, "game_checkwin() count = 8");
		ggzdmod_log(escape_game.ggz, "\tescape_game.boardheight = %d",escape_game.boardheight);
		ggzdmod_log(escape_game.ggz, "\escape_game.wallwidth+escape_game.goalwidth = %d",escape_game.wallwidth+escape_game.goalwidth);
		ggzdmod_log(escape_game.ggz, "\tescape_game.x = %d",escape_game.x);
		ggzdmod_log(escape_game.ggz, "\tescape_game.y = %d",escape_game.y);
		for(i=1; i<10; i++){
			if(i!=5){
				ggzdmod_log(escape_game.ggz, "\tescape_game.board[][][%d] = %d",i, escape_game.board[escape_game.x][escape_game.y][i]);
			}
		}	
		return 2;
	}

	/* Game not over yet */
	return -1;
}

/* Send a request for client to set options */
static int game_send_options_request(int fd)
{
	ggzdmod_log(escape_game.ggz, "Sending options request to fd %d",fd);

	if(ggz_write_int(fd, ESCAPE_REQ_OPTIONS) < 0)
		return -1;

	escape_game.state = ESCAPE_STATE_OPTIONS;
	return 0;
}

/* Send out options */
static int game_send_options(int fd)
{
	ggzdmod_log(escape_game.ggz, "game_send_options(%d)",fd);

	if(fd < 0)
		return -1;
	
	if(ggz_write_int(fd, ESCAPE_MSG_OPTIONS) < 0
	   || ggz_write_char(fd, escape_game.boardheight) < 0
	   || ggz_write_char(fd, escape_game.goalwidth) < 0
	   || ggz_write_char(fd, escape_game.wallwidth) < 0)
		return -1;

	ggzdmod_log(escape_game.ggz, "Options sent ok with");
	ggzdmod_log(escape_game.ggz, "\tboardheight=%d", escape_game.boardheight);
	ggzdmod_log(escape_game.ggz, "\tgoalwidth=%d", escape_game.goalwidth);
	ggzdmod_log(escape_game.ggz, "\twallwidth=%d", escape_game.wallwidth);

	return 0;
}

/* Get options from client */
static int game_read_options(int seat)
{
	int fd = ggzdmod_get_seat(escape_game.ggz, seat).fd;
	int p, q, d;

	ggzdmod_log(escape_game.ggz, "game_get_options(%d)", seat);

	if(ggz_read_char(fd, &escape_game.boardheight) < 0
	   || ggz_read_char(fd, &escape_game.goalwidth) < 0
	   || ggz_read_char(fd, &escape_game.wallwidth) < 0)
		return -1;

	// FIXME - add bounds checking to ensure eg. escape_game.boardheight isn't larger than MAXBOARDHEIGHT

	ggzdmod_log(escape_game.ggz, "Options recieved ok");
	ggzdmod_log(escape_game.ggz, "\tboardheight=%d", escape_game.boardheight);
	ggzdmod_log(escape_game.ggz, "\tgoalwidth=%d", escape_game.goalwidth);
	ggzdmod_log(escape_game.ggz, "\twallwidth=%d", escape_game.wallwidth);

	/* Mark everything as empty */
	for(p = 0;p<=escape_game.wallwidth * 2 + escape_game.goalwidth+1;p++){
		for(q = 0;q<=escape_game.boardheight+1;q++){
			for(d = 0;d<10;d++){
				escape_game.board[p][q][d] = dtEmpty;
        	    	}
		}
	}

    	for(p = 0;p<=escape_game.wallwidth * 2 + escape_game.goalwidth+1;p++){
		/* Mark top wall as blocked */
      		escape_game.board[p][0][4] = dtBlocked;
        	escape_game.board[p][0][7] = dtBlocked;
        	escape_game.board[p][0][8] = dtBlocked;
        	escape_game.board[p][0][9] = dtBlocked;
        	escape_game.board[p][0][6] = dtBlocked;

		/* Mark bottom wall as blocked */
        	escape_game.board[p][(int)escape_game.boardheight][4] = dtBlocked;
        	escape_game.board[p][(int)escape_game.boardheight][1] = dtBlocked;
        	escape_game.board[p][(int)escape_game.boardheight][2] = dtBlocked;
        	escape_game.board[p][(int)escape_game.boardheight][3] = dtBlocked;
        	escape_game.board[p][(int)escape_game.boardheight][6] = dtBlocked;
	}

	for(q = 0;q<=escape_game.boardheight+1;q++){
		/* Mark left hand side wall as blocked */
        	escape_game.board[0][q][1] = dtBlocked;
        	escape_game.board[0][q][4] = dtBlocked;
        	escape_game.board[0][q][7] = dtBlocked;
        	escape_game.board[0][q][8] = dtBlocked;
        	escape_game.board[0][q][2] = dtBlocked;

		/* Mark right hand side wall as blocked */
        	escape_game.board[escape_game.wallwidth * 2 + escape_game.goalwidth][q][8] = dtBlocked;
        	escape_game.board[escape_game.wallwidth * 2 + escape_game.goalwidth][q][9] = dtBlocked;
        	escape_game.board[escape_game.wallwidth * 2 + escape_game.goalwidth][q][6] = dtBlocked;
        	escape_game.board[escape_game.wallwidth * 2 + escape_game.goalwidth][q][3] = dtBlocked;
        	escape_game.board[escape_game.wallwidth * 2 + escape_game.goalwidth][q][2] = dtBlocked;
	}

	escape_game.board[1][1][7] = dtCorner;
	escape_game.board[1][escape_game.boardheight - 1][1] = dtCorner;
	escape_game.board[escape_game.goalwidth + 2 * escape_game.wallwidth - 1][1][9] = dtCorner;
	escape_game.board[escape_game.goalwidth + 2 * escape_game.wallwidth - 1][escape_game.boardheight - 1][3] = dtCorner;

	escape_game.x = (escape_game.goalwidth + 2 * escape_game.wallwidth - 1)/2 + 1;
	escape_game.y = escape_game.boardheight/2;

	//game_send_options(0);
	game_send_options(ggzdmod_get_seat(escape_game.ggz, 1-seat).fd);
	
	if(seats_full()){
		escape_game.state = ESCAPE_STATE_PLAYING;
		ggzdmod_set_state(escape_game.ggz, GGZDMOD_STATE_PLAYING);
	}else{
		escape_game.state = ESCAPE_STATE_WAITING;
		ggzdmod_set_state(escape_game.ggz, GGZDMOD_STATE_WAITING);
	}
	return 0;
}
#if 0
static int game_handle_newgame(int seat)
{
	int status = 0;

	/* The first person to say Yes gets to choose the options */
	if(escape_game.play_again == 0) {
		escape_game.state = ESCAPE_STATE_OPTIONS;
		status = game_send_options_request(seat);
	}

	escape_game.play_again++;

	/* Simulate that the bot wants to play another game */
	if(ggzdmod_get_seat(escape_game.ggz, (seat+1)%2).type == GGZ_SEAT_BOT)
		escape_game.play_again++;

	/* Issue the game start if second answer comes */
	/* and options are already setup to go */
	if(escape_game.state == ESCAPE_STATE_WAIT
	   && escape_game.play_again == 2
	   && seats_full()) {
		escape_game.turn = 0;
		escape_game.state = ESCAPE_STATE_PLAYING;
		repeatmove = 0;
		game_move();
	}

	return status;
}
#endif
static int revdir(int direction)
{
	switch(direction)
	{
       	case 1:
			return 9;
			break;
        	case 2:
			return 8;
			break;
        	case 3:
			return 7;
			break;
        	case 4:
			return 6;
			break;
        	case 6:
			return 4;
			break;
        	case 7:
			return 3;
			break;
        	case 8:
			return 2;
			break;
        	case 9:
            	return 1;
			break;
	}
	ggzdmod_log(escape_game.ggz, "ERROR: revdir: invalid direction %d.", direction);
	return 1;
}

