/*
 * File: game.c
 * Author: Rich Gade (Modified for Escape by Roger Light)
 * Project: GGZ Escape game module
 * Date: 27th June 2001
 * Desc: Game functions
 * $Id: game.c 2273 2001-08-27 06:48:01Z jdorje $
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
#include "../libggzmod/ggz_server.h"

#include "game.h"

/* Global game variables */
struct escape_game_t escape_game;

/* Private functions */
static int game_get_options(int);
static int game_handle_newgame(int);


/* Setup game state and board */
void game_init(void)
{
	ggzd_debug("game_init()");
	escape_game.turn = -1;
	escape_game.state = ESCAPE_STATE_INIT;
}

//FIXME
/* Handle message from player */
int game_handle_player(ggzd_event_t event, void *data)
{
	int num = *(int*)data; /* player number */
	int fd, op, status;
	unsigned char direction;

	ggzd_debug("game_handle_player(%d) called",num);
	ggzd_debug("\tnum = %d, escape_game.turn = %d",num, escape_game.turn);

	fd = ggzd_get_player_socket(num);
	
	if(es_read_int(fd, &op) < 0)
	{
		ggzd_debug("\tPremature exit due to es_read_int(fd, &op) failure");
		return -1;
	}

	ggzd_debug("\top = %d",op);

	switch(op) {
		case ESCAPE_SND_MOVE:
			if((status = game_handle_move(num, &direction)) == 0)
				game_update(ESCAPE_EVENT_MOVE, &direction);
			break;
		case ESCAPE_REQ_SYNC:
			status = game_send_sync(num);
			break;
		case ESCAPE_SND_OPTIONS:
			status = game_get_options(num);
			break;
		case ESCAPE_REQ_NEWGAME:
			status = game_handle_newgame(num);
			break;
		default:
			/* Unrecognized opcode */
			status = -1;
			break;
	}
	ggzd_debug("\tgame_handle_player() is returning with status = %d",status);
	return status;
}


/* Get options from client */
static int game_get_options(int seat)
{
	int fd = ggzd_get_player_socket(seat);
	int p, q, d;

	ggzd_debug("game_get_options(%d)", seat);

	if(es_read_char(fd, &escape_game.boardheight) < 0
	   || es_read_char(fd, &escape_game.wallwidth) < 0
	   || es_read_char(fd, &escape_game.goalwidth) < 0)
		return -1;

	// FIXME - add bounds checking to ensure eg. escape_game.boardheight isn't larger than MAXBOARDHEIGHT

	ggzd_debug("\tOptions recieved ok\n\tboardheight=%d\n\twallwidth=%d\n\tgoalwidth=%d",escape_game.boardheight, escape_game.wallwidth, escape_game.goalwidth);

	for(p = 0;p<=escape_game.wallwidth * 2 + escape_game.goalwidth+1;p++){
		for(q = 0;q<=escape_game.boardheight+1;q++){
			for(d = 0;d<10;d++){
				escape_game.board[p][q][d] = dtEmpty;
            	}
		}
	}

    	for(p = 0;p<=escape_game.wallwidth * 2 + escape_game.goalwidth+1;p++){
      	escape_game.board[p][0][4] = dtBlocked;
        	escape_game.board[p][0][7] = dtBlocked;
        	escape_game.board[p][0][8] = dtBlocked;
        	escape_game.board[p][0][9] = dtBlocked;
        	escape_game.board[p][0][6] = dtBlocked;

        	escape_game.board[p][escape_game.boardheight+1][4] = dtBlocked;
        	escape_game.board[p][escape_game.boardheight+1][1] = dtBlocked;
        	escape_game.board[p][escape_game.boardheight+1][2] = dtBlocked;
        	escape_game.board[p][escape_game.boardheight+1][3] = dtBlocked;
        	escape_game.board[p][escape_game.boardheight+1][6] = dtBlocked;
	}

	for(q = 0;q<=escape_game.boardheight+1;q++){
        	escape_game.board[0][q][1] = dtBlocked;
        	escape_game.board[0][q][4] = dtBlocked;
        	escape_game.board[0][q][7] = dtBlocked;
        	escape_game.board[0][q][8] = dtBlocked;
        	escape_game.board[0][q][2] = dtBlocked;

        	escape_game.board[escape_game.wallwidth * 2 + escape_game.goalwidth+1][q][8] = dtBlocked;
        	escape_game.board[escape_game.wallwidth * 2 + escape_game.goalwidth+1][q][9] = dtBlocked;
        	escape_game.board[escape_game.wallwidth * 2 + escape_game.goalwidth+1][q][6] = dtBlocked;
        	escape_game.board[escape_game.wallwidth * 2 + escape_game.goalwidth+1][q][3] = dtBlocked;
        	escape_game.board[escape_game.wallwidth * 2 + escape_game.goalwidth+1][q][2] = dtBlocked;
	}

	escape_game.x = (escape_game.goalwidth + 2 * escape_game.wallwidth - 1)/2 +1;
	escape_game.y = escape_game.boardheight/2;
	return game_update(ESCAPE_EVENT_OPTIONS, NULL);
}


/* Send out options */
int game_send_options(int seat)
{
	int fd = ggzd_get_player_socket(seat);

	ggzd_debug("game_send_options(%d)",seat);

	if(es_write_int(fd, ESCAPE_MSG_OPTIONS) < 0
	   || es_write_char(fd, escape_game.boardheight) < 0
	   || es_write_char(fd, escape_game.goalwidth) < 0
	   || es_write_char(fd, escape_game.wallwidth) < 0)
		return -1;

	ggzd_debug("\tOptions sent ok with\n\tboardheight=%d\n\tgoalwidth=%d\n\twallwidth=%d\n",escape_game.boardheight, escape_game.goalwidth, escape_game.wallwidth);

	return 0;
}


/* Send a request for client to set options */
int game_send_options_request(int seat)
{
	int fd = ggzd_get_player_socket(seat);
	
	ggzd_debug("game_send_options_request(%d)",seat);

	if(es_write_int(fd, ESCAPE_REQ_OPTIONS) < 0)
		return -1;

	ggzd_debug("\tESCAPE_REQ_OPTIONS sent ok");
	return 0;
}


/* Send out seat assignment */
int game_send_seat(int seat)
{
	int fd = ggzd_get_player_socket(seat);

	ggzd_debug("Sending player %d`s seat num", seat);

	if(es_write_int(fd, ESCAPE_MSG_SEAT) < 0
	   || es_write_int(fd, seat) < 0)
		return -1;

	ggzd_debug("\tESCAPE_ESCAPE_MSG_SEAT sent ok with seat=%d", seat);

	return 0;
}


/* Send out player list to everybody */
int game_send_players(void)
{
	int i, j, fd;
	
	ggzd_debug("game_send_players()");

	for(j=0; j<2; j++) {
		if((fd = ggzd_get_player_socket(j)) == -1)
			continue;

		ggzd_debug("\tSending player list to player %d", j);

		if(es_write_int(fd, ESCAPE_MSG_PLAYERS) < 0)
			return -1;
	
		for(i=0; i<2; i++) {
			if(es_write_int(fd, ggzd_get_seat_status(i)) < 0)
				return -1;
			if(ggzd_get_seat_status(i) != GGZ_SEAT_OPEN
			    && es_write_string(fd, ggzd_get_player_name(i)) < 0)
				return -1;
		}
	}
	ggzd_debug("\tgame_send_players() returning ok");
	return 0;
}


/* Send out move for player: num */
int game_send_move(int num, int event, char direction)
{
	int fd = ggzd_get_player_socket(escape_game.opponent);
	/* int i; */

	ggzd_debug("game_send_move(%d, %d, %d)",num, event, direction);	

	/* If player is a computer, don't need to send */
	if(fd == -1)
		return 0;

	ggzd_debug("\tSending player %d`s move to player %d",
		   num, escape_game.opponent);

	if(es_write_int(fd, ESCAPE_MSG_MOVE) < 0
	   || es_write_char(fd, direction) < 0){
		ggzd_debug("\tgame_send_move() returning -1");
		return -1;
	}
	
	ggzd_debug("\tgame_send_move() returning 0");
	return 0;
}

// FIXME - incomplete (should send x, y)
/* Send out board layout */
int game_send_sync(int num)
{	
	int fd = ggzd_get_player_socket(num);
	int p, q, d;

	ggzd_debug("Handling sync for player %d", num);

	if(es_write_int(fd, ESCAPE_SND_SYNC) < 0
	   || es_write_char(fd, escape_game.boardheight) < 0
	   || es_write_char(fd, escape_game.goalwidth) < 0
	   || es_write_char(fd, escape_game.wallwidth) < 0
	   || es_write_char(fd, escape_game.turn) < 0)
		return -1;

	for(p=0; p<escape_game.goalwidth+escape_game.wallwidth*2; p++)
		for(q=0; q<escape_game.boardheight; p++)
			for(d=1; d<10; d++)
				if(es_write_char(fd, escape_game.board[p][q][d]) < 0)
					return -1;
	return 0;
}
	

/* Send out game-over message */
int game_send_gameover(char winner)
{
	int i, fd;
	
	for(i=0; i<2; i++) {
		if((fd = ggzd_get_player_socket(i)) == -1)
			continue;

		ggzd_debug("Sending game-over to player %d", i);

		if(es_write_int(fd, ESCAPE_MSG_GAMEOVER) < 0
		    || es_write_char(fd, winner) < 0)
			return -1;
	}
	return 0;
}

//FIXME - ai
/* Do the next move*/
int game_move(void)
{
	int num = escape_game.turn;
	/* unsigned char dir, x, y; */

//	if(ggzd_get_seat_status(num) == GGZ_SEAT_BOT) {
//		dir = ai_move(&x, &y);
//		if(dir == 0)
//			/* FIXME: These will cause recursion on score */
//			game_update(ESCAPE_EVENT_MOVE_V, &x, &y);
//		else
//			game_update(DOTS_EVENT_MOVE_H, &x, &y);
//	} else
		game_req_move(num);

	return 0;
}


/* Request move from current player */
int game_req_move(int num)
{
	int fd = ggzd_get_player_socket(num);

	ggzd_debug("game_req_move(num = %d)", num);

	if(es_write_int(fd, ESCAPE_REQ_MOVE) < 0){
		ggzd_debug("\tgame_req_move() returning -1");
		return -1;
	}

	ggzd_debug("\tgame_req_move() returning 0");
	return 0;
}


//FIXME
/* Handle incoming move from player */
int game_handle_move(int num, unsigned char *direction)
{
	int fd = ggzd_get_player_socket(num);
	int i;
	int newx, newy;
	char status=0;
	/* int count=0; */
	
	ggzd_debug("Handling move for player %d", num);
	if(es_read_char(fd, direction) < 0)
		return -1;


	/* We make a note who our opponent is, easier on the update func */
	escape_game.opponent = 1 - num;
	ggzd_debug("\tescape_game.opponent = %d", escape_game.opponent);
	escape_game.repeatmove=0;

	if(escape_game.board[escape_game.x][escape_game.y][*direction]!=dtEmpty)
		status = ESCAPE_ERR_FULL;
	else {
		switch(*direction){
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
				ggzd_debug("ERROR: game_handle_move: wrong direction %d.", *direction);
				newx = escape_game.x;
				newy = escape_game.y;
		}
		if((newx<0)||(newy<0)||(newx>escape_game.wallwidth*2+escape_game.goalwidth)||(newy>escape_game.boardheight))
			status = ESCAPE_ERR_BOUND;
		else {
			for(i=1; i<10; i++)
			{
				if((escape_game.board[newx][newy][i]!=dtEmpty)&&
						(!((newx==1)&&(newy==1)&&(i==7))||
						((newx==1)&&(newy==escape_game.boardheight)&&(i==1))||
						((newx==escape_game.goalwidth+escape_game.wallwidth*2)&&(newy==1)&&(i==9))||
						((newx==escape_game.goalwidth+escape_game.wallwidth*2)&&(newy==escape_game.boardheight)&&(i==3))))
				{
					// give new move
					escape_game.repeatmove=1;
				}
			}		
			if(num%2){
				escape_game.board[escape_game.x][escape_game.y][*direction]=dtPlayer1;
				escape_game.board[newx][newy][revdir(*direction)]=dtPlayer1;
			}else{
				escape_game.board[escape_game.x][escape_game.y][*direction]=dtPlayer2;
				escape_game.board[newx][newy][revdir(*direction)]=dtPlayer2;
			}
			escape_game.x = newx;
			escape_game.y = newy;
			status = 0;
		}
	}


	/* Send back move status */
	if(es_write_int(fd, ESCAPE_RSP_MOVE) < 0
	    || es_write_char(fd, status) < 0
	    || es_write_char(fd, *direction) < 0)
		return -1;

	/* If move simply invalid, ask for resubmit */
	if((status == -3 || status == -4)
	     && game_req_move(num) < 0)
		return -1;

	if(status < 0)
		return 1;
	
	if(!escape_game.repeatmove){ // move on to next player
		ggzd_debug("\tmove on to next player");
		escape_game.turn = 1 - escape_game.turn;
	}else{
		ggzd_debug("\tdon`t move on to next player");
	}
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


//FIXME
/* Check for a win */
char game_check_win(void)
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
		ggzd_debug("game_checkwin() count = 8");
		ggzd_debug("\tescape_game.boardheight = %d",escape_game.boardheight);
		ggzd_debug("\escape_game.wallwidth+escape_game.goalwidth = %d",escape_game.wallwidth+escape_game.goalwidth);
		ggzd_debug("\tescape_game.x = %d",escape_game.x);
		ggzd_debug("\tescape_game.y = %d",escape_game.y);
		for(i=1; i<10; i++){
			if(i!=5){
				ggzd_debug("\tescape_game.board[][][%d] = %d",escape_game.board[escape_game.x][escape_game.y][i]);
			}
		}	
		return 2;
	}

	/* Game not over yet */
	return -1;
}

/* FIXME: there's no reason this should be separate from game_update */
int ggz_update(ggzd_event_t event, void *data)
{
	switch (event) {
		case GGZ_EVENT_LAUNCH:
			return game_update(ESCAPE_EVENT_LAUNCH, data);
		case GGZ_EVENT_JOIN:
			return game_update(ESCAPE_EVENT_JOIN, data);
		case GGZ_EVENT_LEAVE:
			return game_update(ESCAPE_EVENT_LEAVE, data);
		default:
			ggzd_debug("ggz_update: bad event.");
	}
}

/* Update game state */
int game_update(int event, void *d1)
{
	int seat;
	char direction;
	char victor;
	static int first_join=1;
	
	ggzd_debug("game_update(%d, ) entered",event);

	switch(event) {
		case ESCAPE_EVENT_LAUNCH:
			ggzd_debug("\tESCAPE_EVENT_LAUNCH");
			if(escape_game.state != ESCAPE_STATE_INIT)
				return -1;
			escape_game.state = ESCAPE_STATE_OPTIONS;
			ggzd_debug("\tescape_game.state = ESCAPE_STATE_OPTIONS");
			escape_game.repeatmove = 0;
			break;
		case ESCAPE_EVENT_OPTIONS:
			ggzd_debug("\tESCAPE_EVENT_OPTIONS");
			if(escape_game.state != ESCAPE_STATE_OPTIONS)
				return -1;
			escape_game.state = ESCAPE_STATE_WAIT;
			ggzd_debug("\tescape_game.state = ESCAPE_STATE_WAIT");
			
			/* Send the options to anyone waiting for them */
			for(seat=0; seat<2; seat++)
				if(ggzd_get_seat_status(seat) == GGZ_SEAT_PLAYER)
					game_send_options(seat);

			ggzd_debug("\tOptions sent to players");
			/* Start the game if we are ready to */
			if(!ggzd_seats_open() && escape_game.play_again != 1) {
				escape_game.turn = 0;
				escape_game.state = ESCAPE_STATE_PLAYING;
				ggzd_debug("\tescape_game.state = ESCAPE_STATE_PLAYING");
				game_move();
			}
			break;
		case ESCAPE_EVENT_JOIN:
			ggzd_debug("\tESCAPE_EVENT_JOIN");
			if(escape_game.state != ESCAPE_STATE_WAIT
			   && escape_game.state != ESCAPE_STATE_OPTIONS)
				return -1;

			/* Send out seat assignments and player list */
			seat = *(int*)d1;
			game_send_seat(seat);
			ggzd_debug("\tgame_send_seat(%d)",seat);
			game_send_players();

			/* The first joining client gets asked to set options */
			if(escape_game.state == ESCAPE_STATE_OPTIONS) {
				if(first_join) {
					if(game_send_options_request(seat) < 0)
						return -1;
					ggzd_debug("\tgame_send_options_request(%d)",seat);
					first_join = 0;
				}
				return 0;
			}

			/* If options are already set, we can proceed */
			game_send_options(seat);
			ggzd_debug("\tgame_send_options(%d)",seat);
			if(!ggzd_seats_open()) {
				if(escape_game.turn == -1){
					escape_game.turn = 0;
					ggzd_debug("\tescape_game.turn = 0");
				}else{
					ggzd_debug("\tgame_send_sync(%d)",seat);
					game_send_sync(seat);
				}
				escape_game.state = ESCAPE_STATE_PLAYING;
				ggzd_debug("\tescape_game.state = ESCAPE_STATE_PLAYING");
				
				game_move();
			}
			break;
		case ESCAPE_EVENT_LEAVE:
			ggzd_debug("\tESCAPE_EVENT_LEAVE\n\tgame_send_players()");
			game_send_players();
			if(escape_game.state == ESCAPE_STATE_PLAYING){
				escape_game.state = ESCAPE_STATE_WAIT;
				ggzd_debug("\tescape_game.state = ESCAPE_STATE_WAIT");
			}
			break;
		case ESCAPE_EVENT_MOVE:
			ggzd_debug("\tESCAPE_EVENT_MOVE");

			if(escape_game.state != ESCAPE_STATE_PLAYING)
				return -1;
		
			direction = *(char*)d1;
			ggzd_debug("\tgame_send_move(%d, %d, %d)",escape_game.turn, event, direction);
			game_send_move(escape_game.turn, event, direction);
		
			if((victor = game_check_win()) < 0) {
				/* Request next move */
				ggzd_debug("\tgame_move()");
				game_move();
			} else {
				/* We have a winner */
				escape_game.state = ESCAPE_STATE_DONE;
				game_send_gameover(victor);
				game_init();
				escape_game.play_again = 0;
			}
			break;
	}
	ggzd_debug("\tgame_update() returning 0");
	return 0;
}

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
	if(ggzd_get_seat_status((seat+1)%2) == GGZ_SEAT_BOT)
		escape_game.play_again++;

	/* Issue the game start if second answer comes */
	/* and options are already setup to go */
	if(!ggzd_seats_open() && escape_game.state == ESCAPE_STATE_WAIT
	   && escape_game.play_again == 2) {
		escape_game.turn = 0;
		escape_game.state = ESCAPE_STATE_PLAYING;
		escape_game.repeatmove = 0;
		game_move();
	}

	return status;
}

unsigned char revdir(unsigned char direction)
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
	ggzd_debug("ERROR: revdir: invalid direction %d.", direction);
	return 1;
}
