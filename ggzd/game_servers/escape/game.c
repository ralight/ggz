/*
 * File: game.c
 * Author: Rich Gade (Modified for Escape by Roger Light)
 * Project: GGZ Escape game module
 * Date: 27th June 2001
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
struct escape_game_t escape_game;

/* Private functions */
static int game_get_options(int);
static int game_handle_newgame(int);


/* Setup game state and board */
void game_init(void)
{
	ggz_debug("game_init()\n");
	escape_game.turn = -1;
	escape_game.state = ESCAPE_STATE_INIT;
}

/* Handle message from GGZ server */
int game_handle_ggz(int ggz_fd, int* p_fd)
{
	int op, seat, status = -1;

	ggz_debug("game_handle_ggz()\n");

	if(es_read_int(ggz_fd, &op) < 0)
	{
		ggz_debug("\tes_read_int(ggz_fd, &op) < 0, premature exit\n");
		return -1;
	}

	switch(op) {
		case REQ_GAME_LAUNCH:
			ggz_debug("\top == REQ_GAME_LAUNCH\n");
			if(ggz_game_launch() == 0)
			{
				ggz_debug("\tGame launch ok\n");
				status = game_update(ESCAPE_EVENT_LAUNCH, NULL);
			}else{
				ggz_debug("\tGame launch failed\n");
			}
			break;
		case REQ_GAME_JOIN:
			ggz_debug("op == REQ_GAME_JOIN\n");
			if(ggz_player_join(&seat, p_fd) == 0) {
				ggz_debug("\tggz_player_join(%d, p_fd) ok\n",seat);
				status = game_update(ESCAPE_EVENT_JOIN, &seat);
				status = 1;
			}else{
				ggz_debug("\tggz_player_join(%d, p_fd) failed\n",seat);
			}
			break;
		case REQ_GAME_LEAVE:
			ggz_debug("\top == REQ_GAME_LEAVE\n");
			if((status = ggz_player_leave(&seat, p_fd)) == 0) {
				ggz_debug("\tggz_player_leave(%d, p_fd) ok\n",seat);
				game_update(ESCAPE_EVENT_LEAVE, &seat);
				status = 2;
			}else{
				ggz_debug("\tggz_player_leave(%d, p_fd) failed\n",seat);
			}
			break;
		case RSP_GAME_OVER:
			ggz_debug("\top == REQ_GAME_OVER\n");
			status = 3; /* Signal safe to exit */
			break;
		default:
			ggz_debug("\top was unrecognised\n");
			/* Unrecognized opcode */
			status = -1;
			break;
	}
	ggz_debug("\tgame_handle_ggz() is returning %d\n",status);
	return status;
}

//FIXME
/* Handle message from player */
int game_handle_player(int num)
{
	int fd, op, status;
	unsigned char direction;

	ggz_debug("game_handle_player(%d) called\n",num);

	fd = ggz_seats[num].fd;
	
	if(es_read_int(fd, &op) < 0)
	{
		ggz_debug("\tPremature exit due to es_read_int(fd, &op) failure\n");
		return -1;
	}

	ggz_debug("\top = %d\n",op);

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
	ggz_debug("\tgame_handle_player() is returning with status = %d\n",status);
	return status;
}


/* Get options from client */
static int game_get_options(int seat)
{
	int fd = ggz_seats[seat].fd;
	int p, q, d;

	ggz_debug("game_get_options(%d)\n", seat);

	if(es_read_char(fd, &escape_game.boardheight) < 0
	   || es_read_char(fd, &escape_game.wallwidth) < 0
	   || es_read_char(fd, &escape_game.goalwidth) < 0)
		return -1;

	ggz_debug("\tOptions recieved ok\n\tboardheight=%d\n\twallwidth=%d\n\tgoalwidth=%d\n",escape_game.boardheight, escape_game.wallwidth, escape_game.goalwidth);

	for(p = 0;p<escape_game.wallwidth * 2 + escape_game.goalwidth;p++){
		for(q = 0;q<escape_game.boardheight;q++){
			for(d = 0;d<10;d++){
				escape_game.board[p][q][d] = dtEmpty;
            	}
		}
	}
    
    	for(p = 0;p<escape_game.wallwidth * 2 + escape_game.goalwidth;p++){
      	escape_game.board[p][0][4] = dtBlocked;
        	escape_game.board[p][0][7] = dtBlocked;
        	escape_game.board[p][0][8] = dtBlocked;
        	escape_game.board[p][0][9] = dtBlocked;
        	escape_game.board[p][0][6] = dtBlocked;
        
        	escape_game.board[p][escape_game.boardheight][4] = dtBlocked;
        	escape_game.board[p][escape_game.boardheight][1] = dtBlocked;
        	escape_game.board[p][escape_game.boardheight][2] = dtBlocked;
        	escape_game.board[p][escape_game.boardheight][3] = dtBlocked;
        	escape_game.board[p][escape_game.boardheight][6] = dtBlocked;
	}
    
	for(q = 0;q<escape_game.boardheight;q++){
        	escape_game.board[0][q][1] = dtBlocked;
        	escape_game.board[0][q][4] = dtBlocked;
        	escape_game.board[0][q][7] = dtBlocked;
        	escape_game.board[0][q][8] = dtBlocked;
        	escape_game.board[0][q][2] = dtBlocked;
        
        	escape_game.board[escape_game.wallwidth * 2 + escape_game.goalwidth][q][8] = dtBlocked;
        	escape_game.board[escape_game.wallwidth * 2 + escape_game.goalwidth][q][9] = dtBlocked;
        	escape_game.board[escape_game.wallwidth * 2 + escape_game.goalwidth][q][6] = dtBlocked;
        	escape_game.board[escape_game.wallwidth * 2 + escape_game.goalwidth][q][3] = dtBlocked;
        	escape_game.board[escape_game.wallwidth * 2 + escape_game.goalwidth][q][2] = dtBlocked;
	}
    
	escape_game.board[1][1][7] = dtBlocked;
    	escape_game.board[1][escape_game.boardheight - 1][1] = dtBlocked;
    	escape_game.board[escape_game.goalwidth + 2 * escape_game.wallwidth - 1][1][9] = dtBlocked;
    	escape_game.board[escape_game.goalwidth + 2 * escape_game.wallwidth - 1][escape_game.boardheight - 1][3] = dtBlocked;

	escape_game.x = (escape_game.goalwidth + 2 * escape_game.wallwidth - 1)/2 +1;
	escape_game.y = escape_game.boardheight/2;
	return game_update(ESCAPE_EVENT_OPTIONS, NULL);
}


/* Send out options */
int game_send_options(int seat)
{
	int fd = ggz_seats[seat].fd;

	ggz_debug("game_send_options(%d)\n",seat);

	if(es_write_int(fd, ESCAPE_MSG_OPTIONS) < 0
	   || es_write_char(fd, escape_game.boardheight) < 0
	   || es_write_char(fd, escape_game.goalwidth) < 0
	   || es_write_char(fd, escape_game.wallwidth) < 0)
		return -1;

	ggz_debug("\tOptions sent ok with\n\tboardheight=%d\n\tgoalwidth=%d\n\twallwidth=%d\n",escape_game.boardheight, escape_game.goalwidth, escape_game.wallwidth);

	return 0;
}


/* Send a request for client to set options */
int game_send_options_request(int seat)
{
	int fd = ggz_seats[seat].fd;
	
	ggz_debug("game_send_options_request(%d)\n",seat);

	if(es_write_int(fd, ESCAPE_REQ_OPTIONS) < 0)
		return -1;

	ggz_debug("\tESCAPE_REQ_OPTIONS sent ok\n");
	return 0;
}


/* Send out seat assignment */
int game_send_seat(int seat)
{
	int fd = ggz_seats[seat].fd;

	ggz_debug("Sending player %d's seat num", seat);

	if(es_write_int(fd, ESCAPE_MSG_SEAT) < 0
	   || es_write_int(fd, seat) < 0)
		return -1;

	ggz_debug("\tESCAPE_ESCAPE_MSG_SEAT sent ok with seat=%d\n", seat);

	return 0;
}


/* Send out player list to everybody */
int game_send_players(void)
{
	int i, j, fd;
	
	ggz_debug("game_send_players()\n");

	for(j=0; j<2; j++) {
		if((fd = ggz_seats[j].fd) == -1)
			continue;

		ggz_debug("\tSending player list to player %d", j);

		if(es_write_int(fd, ESCAPE_MSG_PLAYERS) < 0)
			return -1;
	
		for(i=0; i<2; i++) {
			if(es_write_int(fd, ggz_seats[i].assign) < 0)
				return -1;
			if(ggz_seats[i].assign != GGZ_SEAT_OPEN
			    && es_write_string(fd, ggz_seats[i].name) < 0) 
				return -1;
		}
	}
	ggz_debug("\tgame_send_players() returning ok\n");
	return 0;
}


/* Send out move for player: num */
int game_send_move(int num, int event, char direction)
{
	int fd = ggz_seats[(num+1)%2].fd;
	int i;

	ggz_debug("game_send_move(%d, %d, %d)\n",num, event, direction);	

	/* If player is a computer, don't need to send */
	if(fd == -1)
		return 0;

	ggz_debug("\tSending player %d's move to player %d",
		   num, (num+1)%2);

	if(es_write_int(fd, ESCAPE_MSG_MOVE) < 0
	   || es_write_char(fd, direction) < 0){
		return -1;
	}
	
	return 0;
}

/* Send out board layout */
int game_send_sync(int num)
{	
	int fd = ggz_seats[num].fd;
	int p, q, d;

	ggz_debug("Handling sync for player %d", num);

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
		if((fd = ggz_seats[i].fd) == -1)
			continue;

		ggz_debug("Sending game-over to player %d", i);

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
	unsigned char dir, x, y;

//	if(ggz_seats[num].assign == GGZ_SEAT_BOT) {
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
	int fd = ggz_seats[num].fd;

	ggz_debug("game_req_move()\n");

	if(es_write_int(fd, ESCAPE_REQ_MOVE) < 0)
		return -1;

	ggz_debug("\tgame_req_move() returning 0\n");
	return 0;
}


//FIXME
/* Handle incoming move from player */
int game_handle_move(int num, unsigned char *direction)
{
	int fd = ggz_seats[num].fd;
	int i;
	int newx, newy;
	char status=0;
	int count=0;
	
	ggz_debug("Handling move for player %d", num);
	if(es_read_char(fd, direction) < 0)
		return -1;


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
		}
		if((newx<0)||(newy<0)||(newx>escape_game.wallwidth*2+escape_game.goalwidth)||(newy>escape_game.boardheight))
			status = ESCAPE_ERR_BOUND;
		else {
			/* We make a note who our opponent is, easier on the update func */
			escape_game.opponent = (num + 1) % 2;

			escape_game.repeatmove=0;
			for(i=1; i<10; i++)
			{
				if((escape_game.board[newx][newy][i]!=dtEmpty)&&
						(!((newx==1)&&(newy==1)&&(i==7))||
						((newx==1)&&(newy==escape_game.boardheight-1)&&(i==1))||
						((newx==escape_game.goalwidth+escape_game.wallwidth*2)&&(newy==1)&&(i==9))||
						((newx==escape_game.goalwidth+escape_game.wallwidth*2)&&(newy==escape_game.boardheight-1)&&(i==3))))
				{
					// give new move
					escape_game.repeatmove=1;
//				}else{
					// pass move on
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
		ggz_debug("\tmove on to next player");
		escape_game.turn = (escape_game.turn + 1) % 2;
	}else{
		ggz_debug("\tdon't move on to next player");
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
		if(!escape_game.y)
			return 0;
		else if(escape_game.y==escape_game.boardheight)
			return 1;

	for(i=1; i<10; i++)
		if((escape_game.board[escape_game.x][escape_game.y][i]!=dtEmpty) && (i!=5))
			count++;

	if(count==8)
		return 2;

	/* Game not over yet */
	return -1;
}

/* Update game state */
int game_update(int event, void *d1)
{
	int seat;
	char direction;
	char victor;
	static int first_join=1;
	
	ggz_debug("game_update(%d, ) entered\n",event);

	switch(event) {
		case ESCAPE_EVENT_LAUNCH:
			ggz_debug("\tESCAPE_EVENT_LAUNCH\n");
			if(escape_game.state != ESCAPE_STATE_INIT)
				return -1;
			escape_game.state = ESCAPE_STATE_OPTIONS;
			ggz_debug("\tescape_game.state = ESCAPE_STATE_OPTIONS\n");
			escape_game.repeatmove = 0;
			break;
		case ESCAPE_EVENT_OPTIONS:
			ggz_debug("\tESCAPE_EVENT_OPTIONS\n");
			if(escape_game.state != ESCAPE_STATE_OPTIONS)
				return -1;
			escape_game.state = ESCAPE_STATE_WAIT;
			ggz_debug("\tescape_game.state = ESCAPE_STATE_WAIT\n");
			
			/* Send the options to anyone waiting for them */
			for(seat=0; seat<2; seat++)
				if(ggz_seats[seat].assign == GGZ_SEAT_PLAYER)
					game_send_options(seat);

			ggz_debug("\tOptions sent to players\n");
			/* Start the game if we are ready to */
			if(!ggz_seats_open() && escape_game.play_again != 1) {
				escape_game.turn = 0;
				escape_game.state = ESCAPE_STATE_PLAYING;
				ggz_debug("\tescape_game.state = ESCAPE_STATE_PLAYING\n");
				game_move();
			}
			break;
		case ESCAPE_EVENT_JOIN:
			ggz_debug("\tESCAPE_EVENT_JOIN\n");
			if(escape_game.state != ESCAPE_STATE_WAIT
			   && escape_game.state != ESCAPE_STATE_OPTIONS)
				return -1;

			/* Send out seat assignments and player list */
			seat = *(int*)d1;
			game_send_seat(seat);
			ggz_debug("\tgame_send_seat(%d)\n",seat);
			game_send_players();

			/* The first joining client gets asked to set options */
			if(escape_game.state == ESCAPE_STATE_OPTIONS) {
				if(first_join) {
					if(game_send_options_request(seat) < 0)
						return -1;
					ggz_debug("\tgame_send_options_request(%d)\n",seat);
					first_join = 0;
				}
				return 0;
			}

			/* If options are already set, we can proceed */
			game_send_options(seat);
			ggz_debug("\tgame_send_options(%d)\n",seat);
			if(!ggz_seats_open()) {
				if(escape_game.turn == -1){
					escape_game.turn = 0;
					ggz_debug("\tescape_game.turn = 0\n");
				}else{
					ggz_debug("\tgame_send_sync(%d)\n",seat);
					game_send_sync(seat);
				}
				escape_game.state = ESCAPE_STATE_PLAYING;
				ggz_debug("\tescape_game.state = ESCAPE_STATE_PLAYING\n");
				
				game_move();
			}
			break;
		case ESCAPE_EVENT_LEAVE:
			ggz_debug("\tESCAPE_EVENT_LEAVE\n\tgame_send_players()\n");
			game_send_players();
			if(escape_game.state == ESCAPE_STATE_PLAYING){
				escape_game.state = ESCAPE_STATE_WAIT;
				ggz_debug("\tescape_game.state = ESCAPE_STATE_WAIT\n");
			}
			break;
		case ESCAPE_EVENT_MOVE:
			ggz_debug("\tESCAPE_EVENT_MOVE\n");

			if(escape_game.state != ESCAPE_STATE_PLAYING)
				return -1;
		
			direction = *(char*)d1;
			ggz_debug("\tgame_send_move(%d, %d, %d)\n",escape_game.turn, event, direction);
			game_send_move(escape_game.turn, event, direction);
		
			if((victor = game_check_win()) < 0) {
				/* Request next move */
				ggz_debug("\tgame_move()\n");
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
	ggz_debug("\tgame_update() returning 0\n");
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
	if(ggz_seats[(seat+1)%2].assign == GGZ_SEAT_BOT)
		escape_game.play_again++;

	/* Issue the game start if second answer comes */
	/* and options are already setup to go */
	if(!ggz_seats_open() && escape_game.state == ESCAPE_STATE_WAIT
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
}