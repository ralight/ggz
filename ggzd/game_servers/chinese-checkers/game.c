/*
 * File: game.c
 * Author: Rich Gade
 * Project: GGZ Chinese Checkers game module
 * Date: 01/01/2001
 * Desc: Game functions
 *
 * Copyright (C) 2001 Richard Gade.
 *
 * This program is free software; cou can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at cour option) any later version.
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
#include <unistd.h>

#include <ggz.h>
#include <game.h>
#include <protocols.h>
#include <easysock.h>
#include <ai.h>

/* Global game variables */
struct game_t game;

/* Private functions */
static void game_handle_newgame(void);
static void game_setup_board(void);
static int game_find_path(int, int, int, int, int);


/* Setup game state and board */
void game_init(void)
{
	game.turn = -1;
	game.turn_count = 0;
	game.state = CC_STATE_INIT;
}


/* Handle message from GGZ server */
int game_handle_ggz(int ggz_fd, int* p_fd)
{
	int op, seat, status = -1;

	if(es_read_int(ggz_fd, &op) < 0)
		return -1;

	switch(op) {
		case REQ_GAME_LAUNCH:
			if(ggz_game_launch() == 0)
				status = game_update(CC_EVENT_LAUNCH,
						     NULL, NULL, NULL, NULL);
			break;
		case REQ_GAME_JOIN:
			if(ggz_player_join(&seat, p_fd) == 0) {
				status = game_update(CC_EVENT_JOIN, &seat,
						     NULL, NULL, NULL);
				status = 1;
			}
			break;
		case REQ_GAME_LEAVE:
			if((status = ggz_player_leave(&seat, p_fd)) == 0) {
				game_update(CC_EVENT_LEAVE, &seat, NULL,
					    NULL, NULL);
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
	int fd, op, status;
	unsigned char ro, co, rd, cd;

	fd = ggz_seats[num].fd;
	
	if(es_read_int(fd, &op) < 0)
		return -1;

	switch(op) {
		case CC_SND_MOVE:
			if((status = game_handle_move(num, &ro, &co, &rd, &cd)) == 0)
				game_update(CC_EVENT_MOVE, &ro, &co, &rd, &cd);
			break;
		case CC_REQ_SYNC:
			status = game_send_sync(num);
			break;
		case CC_REQ_NEWGAME:
			game_handle_newgame();
			status = 0;
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

	if(es_write_int(fd, CC_MSG_SEAT) < 0
	   || es_write_int(fd, seats) < 0
	   || es_write_int(fd, seat) < 0)
		return -1;

	return 0;
}


/* Send out player list to everybody */
int game_send_players(void)
{
	int i, j, fd;
	
	for(j=0; j<seats; j++) {
		if((fd = ggz_seats[j].fd) == -1)
			continue;

		ggz_debug("Sending player list to player %d", j);

		if(es_write_int(fd, CC_MSG_PLAYERS) < 0)
			return -1;
	
		if(es_write_int(fd, seats) < 0)
			return -1;
		for(i=0; i<seats; i++) {
			if(es_write_int(fd, ggz_seats[i].assign) < 0)
				return -1;
			if(ggz_seats[i].assign != GGZ_SEAT_OPEN
			    && es_write_string(fd, ggz_seats[i].name) < 0) 
				return -1;
		}
	}
	return 0;
}


/* Send out move for player: num */
int game_send_move(int num, char ro, char co, char rd, char cd)
{
	int fd;
	int i;
	
	for(i=0; i<seats; i++) {
		if((i == num) || ((fd = ggz_seats[i].fd) == -1))
			continue;

		ggz_debug("Sending player %d's move to player %d", num, i);

		if(es_write_int(fd, CC_MSG_MOVE) < 0
		   || es_write_int(fd, num) < 0
		   || es_write_char(fd, ro) < 0
		   || es_write_char(fd, co) < 0
		   || es_write_char(fd, rd) < 0
		   || es_write_char(fd, cd) < 0)
			return -1;
	}

	/* Always wait 1 second after sending a player move */
	sleep(1);

	return 0;
}


/* Send out board lacout */
int game_send_sync(int num)
{	
	int i, j, fd = ggz_seats[num].fd;

	ggz_debug("Handling sync for player %d", num);

	if(es_write_int(fd, CC_MSG_SYNC) < 0
	   || es_write_char(fd, game.turn) < 0)
		return -1;

	for(i=0; i<17; i++)
		for(j=0; j<25; j++)
			if(es_write_char(fd, game.board[i][j]) < 0)
				return -1;

	return 0;
}
	

/* Send out game-over message */
int game_send_gameover(char winner)
{
	int i, fd;
	
	for(i=0; i<seats; i++) {
		if((fd = ggz_seats[i].fd) == -1)
			continue;

		ggz_debug("Sending game-over to player %d", i);

		if(es_write_int(fd, CC_MSG_GAMEOVER) < 0
		    || es_write_char(fd, winner) < 0)
			return -1;
	}
	return 0;
}


/* Do the next move*/
int game_move(void)
{
	int num = game.turn;
	unsigned char ro, co, rd, cd;

	if(num == 0)
		game.turn_count++;

	if(ggz_seats[num].assign == GGZ_SEAT_BOT) {
		ai_move(&ro, &co, &rd, &cd);
		game_update(CC_EVENT_MOVE, &ro, &co, &rd, &cd);
	} else
		game_req_move(num);

	return 0;
}


/* Request move from current player */
int game_req_move(int num)
{
	int fd = ggz_seats[num].fd;

	if(es_write_int(fd, CC_REQ_MOVE) < 0)
		return -1;

	return 0;
}


/* Handle incoming move from player */
int game_handle_move(int num, unsigned char *ro, unsigned char *co,
			      unsigned char *rd, unsigned char *cd)
{
	int fd = ggz_seats[num].fd;
	char status;

	if(num != game.turn)
		if(es_write_int(fd, CC_RSP_MOVE) < 0
		    || es_write_char(fd, CC_ERR_TURN) < 0)
			return -1;

	ggz_debug("Handling move for player %d", num);
	if(es_read_char(fd, ro) < 0)
		return -1;
	if(es_read_char(fd, co) < 0)
		return -1;
	if(es_read_char(fd, rd) < 0)
		return -1;
	if(es_read_char(fd, cd) < 0)
		return -1;

	if(*ro >= 17 || *co >= 25 || *rd >=17 || *cd >= 25
	   || game.board[*ro][*co] == -1 || game.board[*rd][*cd] == -1)
		status = CC_ERR_BOUND;
	else if(game.board[*ro][*co] != num+1)
		status = CC_ERR_INVALID;
	else if(game.board[*rd][*cd] != 0)
		status = CC_ERR_FULL;
	else if(!game_find_path(0, *ro, *co, *rd, *cd))
		status = CC_ERR_INVALID;
	else {
		status = 0;
		game.board[*ro][*co] = 0;
		game.board[*rd][*cd] = num+1;
	}

	/* Send back move status */
	if(es_write_int(fd, CC_RSP_MOVE) < 0
	    || es_write_char(fd, status) < 0)
		return -1;

	/* If move simply invalid, ask for resubmit */
	if((status == CC_ERR_BOUND || status == CC_ERR_INVALID
	    || status == CC_ERR_FULL)
	    && game_req_move(num) < 0)
		return -1;

	if(status < 0)
		return 1;
	
	return 0;
}


/* Update game state */
int game_update(int event, void *d1, void *d2, void *d3, void *d4)
{
	int seat;
	char ro, co, rd, cd;
	char victor;
	
	switch(event) {
		case CC_EVENT_LAUNCH:
			if(game.state != CC_STATE_INIT)
				return -1;
			game_setup_board();
			game.state = CC_STATE_WAIT;
			break;
		case CC_EVENT_JOIN:
			if(game.state != CC_STATE_WAIT)
				return -1;

			/* Send out seat assignments and player list */
			seat = *(int*)d1;
			game_send_seat(seat);
			game_send_players();

			if(!ggz_seats_open()) {
				if(game.turn == -1)
					game.turn = 0;
				else 
					game_send_sync(seat);
			
				game.state = CC_STATE_PLAYING;
				game_move();
			}
			break;
		case CC_EVENT_LEAVE:
			game_send_players();
			if(game.state == CC_STATE_PLAYING)
				game.state = CC_STATE_WAIT;
			break;
		case CC_EVENT_MOVE:
			if(game.state != CC_STATE_PLAYING)
				return -1;
		
			ro = *(char*)d1;
			co = *(char*)d2;
			rd = *(char*)d3;
			cd = *(char*)d4;

			game_send_move(game.turn, ro, co, rd, cd);

			if((victor = game_check_win()) < 0) {
				/* Request next move */
				game.turn = (game.turn + 1) % seats;
				game_move();
			} else {
				/* We have a winner */
				game.state = CC_STATE_DONE;
				game_send_gameover(victor);
				game_init();
				game.play_again = 0;
			}
			break;
	}
	
	return 0;
}


static void game_handle_newgame(void)
{
	/* First user to say yes causes the board to reset */
	if(game.play_again == 0)
		game_setup_board();

	game.play_again++;
}


static char init_board[17][25] = {
        "            *            ",
        "           * *           ",
        "          * * *          ",
        "         * * * *         ",
        "* * * * * * * * * * * * *",
        " * * * * * * * * * * * * ",
        "  * * * * * * * * * * *  ",
        "   * * * * * * * * * *   ",
        "    * * * * * * * * *    ",
        "   * * * * * * * * * *   ",
        "  * * * * * * * * * * *  ",
        " * * * * * * * * * * * * ",
        "* * * * * * * * * * * * *",
        "         * * * *         ",
        "          * * *          ",
        "           * *           ",
        "            *            "
};

static int homexy[6][10][2] = {
	/* Position 0 */
	{ {16, 12},
	  {15, 11}, {15, 13},
	  {14, 10}, {14, 12}, {14, 14},
	  {13, 9},  {13, 11}, {13, 13}, {13, 15} },
	/* Position 1 */
	{ {12, 0},
	  {11, 1}, {12, 2},
	  {10, 2}, {11, 3}, {12, 4},
	  {9, 3},  {10, 4}, {11, 5}, {12, 6} },
	/* Position 2 */
	{ {4, 0},
	  {4, 2}, {5, 1},
	  {4, 4}, {5, 3}, {6, 2},
	  {4, 6}, {5, 5}, {6, 4}, {7, 3} },
	/* Position 3 */
	{ {0, 12},
	  {1, 11}, {1, 13},
	  {2, 10}, {2, 12}, {2, 14},
	  {3, 9},  {3, 11}, {3, 13}, {3, 15} },
	/* Position 4 */
	{ {4, 24},
	  {5, 23}, {4, 22},
	  {6, 22}, {5, 21}, {4, 20},
	  {7, 21}, {6, 20}, {5, 19}, {4,18} },
	/* Position 5 */
	{ {12, 24},
	  {12, 22}, {11, 23},
	  {12, 20}, {11, 21}, {10, 22},
	  {12, 18}, {11, 19}, {10, 20}, {9, 21} }
};

static int homes[6][6] = {
	{ 0, -1, -1, -1, -1, -1 },	/* One player game (just filler) */
	{ 0,  3, -1, -1, -1, -1 },
	{ 0,  2,  4, -1, -1, -1 },
	{ 0,  1,  3,  4, -1, -1 },
	{ 0,  1,  2,  3,  4, -1 },	/* Five player game also filler */
	{ 0,  1,  2,  3,  4,  5 }
};

static void game_setup_board(void)
{
	int i, j;
	int x, y, home;

	/* First zero everything out */
        for(i=0; i<17; i++)
                for(j=0; j<25; j++)
                        if(init_board[i][j] == ' ')
                                game.board[i][j] = -1;
                        else
                                game.board[i][j] = 0;

	/* Now place the home positions for folks marbles */
	for(i=0; i<seats; i++) {
		home = homes[seats-1][i];
		for(j=0; j<10; j++) {
			x = homexy[home][j][0];
			y = homexy[home][j][1];
			game.board[x][y] = i+1;
		}
	}
}


static int game_find_path(int from, int ro, int co, int rd, int cd)
{
	int r, c;
	static char visited[17][25];

	if(from == 0) {
		/* Zero out the array of nodes we've visited */
		for(r=0; r<17; r++)
			for(c=0; c<25; c++)
				visited[r][c] = 0;

		/* Check each of the six immediate move directions for dest */
		/* We don't have to check if the cell is occupied, as we    */
		/* know the destination cell to be open */
		if((ro == rd && co-2 == cd)
		   || (ro-1 == rd && co-1 == cd)
		   || (ro-1 == rd && co+1 == cd)
		   || (ro == rd && co+2 == cd)
		   || (ro+1 == rd && co+1 == cd)
		   || (ro+1 == rd && co-1 == cd) )
			return 1;

		visited[ro][co] = 1;

		/* Check all six possible jump directions recursively */
		if((co-4 >= 0)
		   && (game.board[ro][co-2] > 0)
		   && (game.board[ro][co-4] == 0)
		   && game_find_path(1, ro, co-4, rd, cd) )
			return 1;
		if((co-2 >= 0 && ro-2 >= 0)
		   && (game.board[ro-1][co-1] > 0)
		   && (game.board[ro-2][co-2] == 0)
		   && game_find_path(2, ro-2, co-2, rd, cd) )
			return 1;
		if((co+2 < 25 && ro-2 >= 0)
		   && (game.board[ro-1][co+1] > 0)
		   && (game.board[ro-2][co+2] == 0)
		   && game_find_path(3, ro-2, co+2, rd, cd) )
			return 1;
		if((co+4 < 25)
		   && (game.board[ro][co+2] > 0)
		   && (game.board[ro][co+4] == 0)
		   && game_find_path(4, ro, co+4, rd, cd) )
			return 1;
		if((co+2 < 25 && ro+2 < 17)
		   && (game.board[ro+1][co+1] > 0)
		   && (game.board[ro+2][co+2] == 0)
		   && game_find_path(5, ro+2, co+2, rd, cd) )
			return 1;
		if((co-2 >= 0 && ro+2 < 17)
		   && (game.board[ro+1][co-1] > 0)
		   && (game.board[ro+2][co-2] == 0)
		   && game_find_path(6, ro+2, co-2, rd, cd) )
			return 1;

		/* Failed in all six directions, not a valid move */
		return 0;
	}

	/* Check to make sure we haven't visited here before */
	if(visited[ro][co])
		return 0;
	visited[ro][co] = 1;

	/* Is it soup yet? */
	if(ro == rd && co == cd)
		return 1;

	/* Check all six possible jump directions recursively */
	/* EXCEPT the one we came from */
	if((from != 4)
	   && (co-4 >= 0)
	   && (game.board[ro][co-2] > 0)
	   && (game.board[ro][co-4] == 0)
	   && game_find_path(1, ro, co-4, rd, cd) )
		return 1;
	if((from != 5)
	   && (co-2 >= 0 && ro-2 >= 0)
	   && (game.board[ro-1][co-1] > 0)
	   && (game.board[ro-2][co-2] == 0)
	   && game_find_path(2, ro-2, co-2, rd, cd) )
		return 1;
	if((from != 6)
	   && (co+2 < 25 && ro-2 >= 0)
	   && (game.board[ro-1][co+1] > 0)
	   && (game.board[ro-2][co+2] == 0)
	   && game_find_path(3, ro-2, co+2, rd, cd) )
		return 1;
	if((from != 1)
	   && (co+4 < 25)
	   && (game.board[ro][co+2] > 0)
	   && (game.board[ro][co+4] == 0)
	   && game_find_path(4, ro, co+4, rd, cd) )
		return 1;
	if((from != 2)
	   && (co+2 < 25 && ro+2 < 17)
	   && (game.board[ro+1][co+1] > 0)
	   && (game.board[ro+2][co+2] == 0)
	   && game_find_path(5, ro+2, co+2, rd, cd) )
		return 1;
	if((from != 3)
	   && (co-2 >= 0 && ro+2 < 17)
	   && (game.board[ro+1][co-1] > 0)
	   && (game.board[ro+2][co-2] == 0)
	   && game_find_path(6, ro+2, co-2, rd, cd) )
		return 1;

	return 0;
}


/* Check for a win */
char game_check_win(void)
{
	int i, dest, x, y;

	dest = (homes[seats-1][(int)game.turn] + 3) % 6;
	for(i=0; i<10; i++) {
		x = homexy[dest][i][0];
		y = homexy[dest][i][1];
		/* If any of the target holes are not ours, we haven't won */
		if(game.board[x][y] != game.turn + 1)
			return -1;
	}

	/* This game is over, we won! */
	return game.turn;
}
