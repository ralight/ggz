/*
 * File: game.c
 * Author: Rich Gade
 * Project: GGZ Connect the Dots game module
 * Date: 04/27/2000
 * Desc: Game functions
 * $Id: game.c 2812 2001-12-09 01:52:00Z jdorje $
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

#include <easysock.h>

#include "game.h"
#include "ai.h"

/* Global game variables */
struct dots_game_t dots_game;
int s_x[2], s_y[2];
int score;

/* Private functions */
static int game_get_options(int);
static int game_handle_newgame(int);


/* Setup game state and board */
void game_init(GGZdMod *ggz)
{
	int i, j;
	
	dots_game.turn = -1;
	dots_game.state = DOTS_STATE_INIT;
	for(i=0; i<MAX_BOARD_WIDTH; i++)
		for(j=0; j<MAX_BOARD_HEIGHT-1; j++)
			dots_game.vert_board[i][j] = 0;
	for(i=0; i<MAX_BOARD_WIDTH-1; i++)
		for(j=0; j<MAX_BOARD_HEIGHT; j++)
			dots_game.horz_board[i][j] = 0;
	for(i=0; i<MAX_BOARD_WIDTH-1; i++)
		for(j=0; j<MAX_BOARD_HEIGHT-1; j++)
			dots_game.owners_board[i][j] = -1;
	dots_game.score[0] = dots_game.score[1] = 0;
	dots_game.ggz = ggz;
}


/* Handle message from GGZ server */
void game_handle_ggz_state(GGZdMod *ggz, GGZdModEvent event, void *data)
{
	GGZdModState old_state = *(GGZdModState*)data;
	
	if (old_state == GGZDMOD_STATE_CREATED)
		game_update(DOTS_EVENT_LAUNCH, NULL, NULL);
}


void game_handle_ggz_join(GGZdMod *ggz, GGZdModEvent event, void *data)
{
	game_update(DOTS_EVENT_JOIN, (int*)data, NULL);
}

void game_handle_ggz_leave(GGZdMod *ggz, GGZdModEvent event, void *data)
{
	game_update(DOTS_EVENT_LEAVE, (int*)data, NULL);
}


/* Handle message from player */
void game_handle_player_data(GGZdMod *ggz, GGZdModEvent event, void *data)
{
	int num = *(int*)data;
	int fd, op, status;
	unsigned char x, y;

	fd = ggzdmod_get_seat(dots_game.ggz, num).fd;
	
	if(es_read_int(fd, &op) < 0)
		return; /* FIXME: error --JDS */

	switch(op) {
		case DOTS_SND_MOVE_V:
			if((status = game_handle_move(num, 0, &x, &y)) == 0)
				game_update(DOTS_EVENT_MOVE_V, &x, &y);
			break;
		case DOTS_SND_MOVE_H:
			if((status = game_handle_move(num, 1, &x, &y)) == 0)
				game_update(DOTS_EVENT_MOVE_H, &x, &y);
			break;
		case DOTS_REQ_SYNC:
			status = game_send_sync(num);
			break;
		case DOTS_SND_OPTIONS:
			status = game_get_options(num);
			break;
		case DOTS_REQ_NEWGAME:
			status = game_handle_newgame(num);
			break;
		default:
			/* Unrecognized opcode */
			status = -1;
			break;
	}
	/* FIXME: handle status --JDS */
}


/* Get options from client */
static int game_get_options(int seat)
{
	int fd = ggzdmod_get_seat(dots_game.ggz, seat).fd;

	if(es_read_char(fd, &dots_game.board_width) < 0
	   || es_read_char(fd, &dots_game.board_height) < 0)
		return -1;

	return game_update(DOTS_EVENT_OPTIONS, NULL, NULL);
}


/* Send out options */
int game_send_options(int seat)
{
	int fd = ggzdmod_get_seat(dots_game.ggz, seat).fd;

	if(es_write_int(fd, DOTS_MSG_OPTIONS) < 0
	   || es_write_char(fd, dots_game.board_width) < 0
	   || es_write_char(fd, dots_game.board_height) < 0)
		return -1;
	return 0;
}


/* Send a request for client to set options */
int game_send_options_request(int seat)
{
	int fd = ggzdmod_get_seat(dots_game.ggz, seat).fd;

	if(es_write_int(fd, DOTS_REQ_OPTIONS) < 0)
		return -1;
	return 0;
}


/* Send out seat assignment */
int game_send_seat(int seat)
{
	int fd = ggzdmod_get_seat(dots_game.ggz, seat).fd;

	ggzdmod_log(dots_game.ggz, "Sending player %d's seat num", seat);

	if(es_write_int(fd, DOTS_MSG_SEAT) < 0
	   || es_write_int(fd, seat) < 0)
		return -1;

	return 0;
}


/* Send out player list to everybody */
int game_send_players(void)
{
	int i, j, fd;
	
	for(j=0; j<2; j++) {
		if((fd = ggzdmod_get_seat(dots_game.ggz, j).fd) == -1)
			continue;

		ggzdmod_log(dots_game.ggz, "Sending player list to player %d", j);

		if(es_write_int(fd, DOTS_MSG_PLAYERS) < 0)
			return -1;
	
		for(i=0; i<2; i++) {
			GGZSeat seat = ggzdmod_get_seat(dots_game.ggz, i);
			if(es_write_int(fd, seat.type) < 0)
				return -1;
			if(seat.type != GGZ_SEAT_OPEN
			    /* FIXME: seat.name can be NULL! */
			    && es_write_string(fd, seat.name) < 0)
				return -1;
		}
	}
	return 0;
}


/* Send out move for player: num */
int game_send_move(int num, int event, char x, char y)
{
	int fd = ggzdmod_get_seat(dots_game.ggz, dots_game.opponent).fd;
	int i;
	int msg;
	
	/* If player is a computer, don't need to send */
	if(fd == -1)
		return 0;

	ggzdmod_log(dots_game.ggz, "Sending player %d's move to player %d",
		   num, dots_game.opponent);

	if(event == DOTS_EVENT_MOVE_H)
		msg = DOTS_MSG_MOVE_H;
	else
		msg = DOTS_MSG_MOVE_V;

	if(es_write_int(fd, msg) < 0
	   || es_write_char(fd, x) < 0
	   || es_write_char(fd, y) < 0
	   || es_write_char(fd, score) < 0)
		return -1;
	for(i=0; i<score; i++) {
		if(es_write_char(fd, s_x[i]) < 0
		   || es_write_char(fd, s_y[i]) < 0)
			return -1;
	}
	
	return 0;
}


/* Send out board layout */
int game_send_sync(int num)
{	
	int i, j, fd = ggzdmod_get_seat(dots_game.ggz, num).fd;

	ggzdmod_log(dots_game.ggz, "Handling sync for player %d", num);

	if(es_write_int(fd, DOTS_SND_SYNC) < 0
	   || es_write_char(fd, dots_game.turn) < 0
	   || es_write_int(fd, dots_game.score[0]) < 0
	   || es_write_int(fd, dots_game.score[1]) < 0)
		return -1;

	for(i=0; i<dots_game.board_width; i++)
		for(j=0; j<dots_game.board_height-1; j++)
			if(es_write_char(fd, dots_game.vert_board[i][j]) < 0)
				return -1;
	for(i=0; i<dots_game.board_width-1; i++)
		for(j=0; j<dots_game.board_height; j++)
			if(es_write_char(fd, dots_game.horz_board[i][j]) < 0)
				return -1;
	for(i=0; i<dots_game.board_width-1; i++)
		for(j=0; j<dots_game.board_height-1; j++)
			if(es_write_char(fd, dots_game.owners_board[i][j]) < 0)
				return -1;

	return 0;
}
	

/* Send out game-over message */
int game_send_gameover(char winner)
{
	int i, fd;
	
	for(i=0; i<2; i++) {
		if((fd = ggzdmod_get_seat(dots_game.ggz, i).fd) == -1)
			continue;

		ggzdmod_log(dots_game.ggz, "Sending game-over to player %d", i);

		if(es_write_int(fd, DOTS_MSG_GAMEOVER) < 0
		    || es_write_char(fd, winner) < 0)
			return -1;
	}
	return 0;
}


/* Do the next move*/
int game_move(void)
{
	int num = dots_game.turn;
	unsigned char dir, x, y;

	if(ggzdmod_get_seat(dots_game.ggz, num).type == GGZ_SEAT_BOT) {
		dir = ai_move(&x, &y);
		if(dir == 0)
			/* FIXME: These will cause recursion on score */
			game_update(DOTS_EVENT_MOVE_V, &x, &y);
		else
			game_update(DOTS_EVENT_MOVE_H, &x, &y);
	} else
		game_req_move(num);

	return 0;
}


/* Request move from current player */
int game_req_move(int num)
{
	int fd = ggzdmod_get_seat(dots_game.ggz, num).fd;

	if(es_write_int(fd, DOTS_REQ_MOVE) < 0)
		return -1;

	return 0;
}


/* Handle incoming move from player */
int game_handle_move(int num, int dir, unsigned char *x, unsigned char *y)
{
	int fd = ggzdmod_get_seat(dots_game.ggz, num).fd;
	int i;
	char status;
	
	ggzdmod_log(dots_game.ggz, "Handling move for player %d", num);
	if(es_read_char(fd, x) < 0)
		return -1;
	if(es_read_char(fd, y) < 0)
		return -1;

	score = 0;

	if(*x >= dots_game.board_width
	   || *y >= dots_game.board_height)
		status = DOTS_ERR_BOUND;
	else {
		status = DOTS_ERR_FULL;
		if(dir == 0) {
			/* A vertical move */
			if(!dots_game.vert_board[*x][*y]) {
				status = 0;
				dots_game.vert_board[*x][*y] = 1;
				if(*x != 0
		   	   	   && dots_game.vert_board[*x-1][*y]
		   	   	   && dots_game.horz_board[*x-1][*y]
		   	   	   && dots_game.horz_board[*x-1][*y+1]) {
					s_x[score] = *x-1;
					s_y[score] = *y;
					score++;
					dots_game.owners_board[*x-1][*y] = num;
				}
				if(*x != dots_game.board_width-1
			   	   && dots_game.vert_board[*x+1][*y]
			   	   && dots_game.horz_board[*x][*y]
			   	   && dots_game.horz_board[*x][*y+1]) {
					s_x[score] = *x;
					s_y[score] = *y;
					score++;
					dots_game.owners_board[*x][*y] = num;
				}
			}
		} else {
			/* Horizontal move */
			if(!dots_game.horz_board[*x][*y]) {
				status = 0;
				dots_game.horz_board[*x][*y] = 1;
				if(*y != 0
			   	   && dots_game.horz_board[*x][*y-1]
			   	   && dots_game.vert_board[*x][*y-1]
			   	   && dots_game.vert_board[*x+1][*y-1]) {
					s_x[score] = *x;
					s_y[score] = *y-1;
					score++;
					dots_game.owners_board[*x][*y-1] = num;
				}
				if(*y != dots_game.board_height-1
			   	   && dots_game.horz_board[*x][*y+1]
			   	   && dots_game.vert_board[*x][*y]
			   	   && dots_game.vert_board[*x+1][*y]) {
					s_x[score] = *x;
					s_y[score] = *y;
					score++;
					dots_game.owners_board[*x][*y] = num;
				}
			}
		}
	}

	/* Send back move status */
	if(es_write_int(fd, DOTS_RSP_MOVE) < 0
	    || es_write_char(fd, status) < 0
	    || es_write_char(fd, score) < 0)
		return -1;
	for(i=0; i<score; i++) {
		if(es_write_char(fd, s_x[i]) < 0
		   || es_write_char(fd, s_y[i]) < 0)
			return -1;
	}

	/* If move simply invalid, ask for resubmit */
	if((status == -3 || status == -4)
	     && game_req_move(num) < 0)
		return -1;

	if(status < 0)
		return 1;
	
	/* We make a note who our opponent is, easier on the update func */
	dots_game.opponent = (num + 1) % 2;
	if(score > 0)
		/* They scored, go again */
		dots_game.score[num] += score;
	else
		/* No score, turn passes */
		dots_game.turn = (dots_game.turn + 1) % 2;

	return 0;
}

#if 0
/* Do bot moves */
int game_bot_move(void)
{
	int i;
	
	for(i = 0; i < 9; i++)
		if (dots_game.board[i] == -1)
			return i;

	return -1;
}
#endif


/* Check for a win */
char game_check_win(void)
{
	if(dots_game.score[0] + dots_game.score[1] ==
	   (dots_game.board_width-1) * (dots_game.board_height-1)) {
		/* Game is over, return score leader */
		if(dots_game.score[0] > dots_game.score[1])
			return 0;
		else if(dots_game.score[0] < dots_game.score[1])
			return 1;
		else
			return 2;
	}

	/* Game not over yet */
	return -1;
}


/* Update game state */
int game_update(int event, void *d1, void *d2)
{
	int seat;
	char x, y;
	char victor;
	static int first_join=1;
	
	switch(event) {
		case DOTS_EVENT_LAUNCH:
			if(dots_game.state != DOTS_STATE_INIT)
				return -1;
			dots_game.state = DOTS_STATE_OPTIONS;
			break;
		case DOTS_EVENT_OPTIONS:
			if(dots_game.state != DOTS_STATE_OPTIONS)
				return -1;
			dots_game.state = DOTS_STATE_WAIT;

			/* Send the options to anyone waitng for them */
			for(seat=0; seat<2; seat++)
				if(ggzdmod_get_seat(dots_game.ggz, seat).type == GGZ_SEAT_PLAYER)
					game_send_options(seat);

			/* Start the game if we are ready to */
			if(!ggzdmod_count_seats(dots_game.ggz, GGZ_SEAT_OPEN) && dots_game.play_again != 1) {
				dots_game.turn = 0;
				dots_game.state = DOTS_STATE_PLAYING;
				game_move();
			}
			break;
		case DOTS_EVENT_JOIN:
			if(dots_game.state != DOTS_STATE_WAIT
			   && dots_game.state != DOTS_STATE_OPTIONS)
				return -1;

			/* Send out seat assignments and player list */
			seat = *(int*)d1;
			game_send_seat(seat);
			game_send_players();

			/* The first joining client gets asked to set options */
			if(dots_game.state == DOTS_STATE_OPTIONS) {
				if(first_join) {
					if(game_send_options_request(seat) < 0)
						return -1;
					first_join = 0;
				}
				return 0;
			}

			/* If options are already set, we can proceed */
			game_send_options(seat);
			if(!ggzdmod_count_seats(dots_game.ggz, GGZ_SEAT_OPEN)) {
				if(dots_game.turn == -1)
					dots_game.turn = 0;
				else
					game_send_sync(seat);
			
				dots_game.state = DOTS_STATE_PLAYING;
				game_move();
			}
			break;
		case DOTS_EVENT_LEAVE:
			game_send_players();
			if(dots_game.state == DOTS_STATE_PLAYING)
				dots_game.state = DOTS_STATE_WAIT;
			break;
		case DOTS_EVENT_MOVE_H:
		case DOTS_EVENT_MOVE_V:
			if(dots_game.state != DOTS_STATE_PLAYING)
				return -1;
		
			x = *(char*)d1;
			y = *(char*)d2;

			game_send_move(dots_game.turn, event, x, y);
		
			if((victor = game_check_win()) < 0) {
				/* Request next move */
				game_move();
			} else {
				/* We have a winner */
				dots_game.state = DOTS_STATE_DONE;
				game_send_gameover(victor);
				game_init(dots_game.ggz);
				dots_game.play_again = 0;
			}
			break;
	}
	
	return 0;
}


static int game_handle_newgame(int seat)
{
	int status = 0;

	/* The first person to say Yes gets to choose the options */
	if(dots_game.play_again == 0) {
		dots_game.state = DOTS_STATE_OPTIONS;
		status = game_send_options_request(seat);
	}

	dots_game.play_again++;

	/* Simulate that the bot wants to play another game */
	if(ggzdmod_get_seat(dots_game.ggz, (seat+1)%2).type == GGZ_SEAT_BOT)
		dots_game.play_again++;

	/* Issue the game start if second answer comes */
	/* and options are already setup to go */
	if(!ggzdmod_count_seats(dots_game.ggz, GGZ_SEAT_OPEN) && dots_game.state == DOTS_STATE_WAIT
	   && dots_game.play_again == 2) {
		dots_game.turn = 0;
		dots_game.state = DOTS_STATE_PLAYING;
		game_move();
	}

	return status;
}
