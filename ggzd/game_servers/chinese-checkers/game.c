/*
 * File: game.c
 * Author: Rich Gade
 * Project: GGZ Chinese Checkers game module
 * Date: 01/01/2001
 * Desc: Game functions
 *
 * Copyright (C) 2001 Richard Gade.
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
#include <ai.h>

/* Global game variables */
struct game_t game;

/* Private functions */
static void game_handle_newgame(void);
static void game_setup_board(void);
static int game_find_path(int, int, int, int, int);
static void game_print_board(void);


/* Setup game state and board */
void game_init(void)
{
	game.turn = -1;
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
	unsigned char xo, yo, xd, yd;

	fd = ggz_seats[num].fd;
	
	if(es_read_int(fd, &op) < 0)
		return -1;

	switch(op) {
		case CC_SND_MOVE:
			if((status = game_handle_move(num, &xo, &yo, &xd, &yd)) == 0)
				game_update(CC_EVENT_MOVE, &xo, &yo, &xd, &yd);
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
int game_send_move(int num, char xo, char yo, char xd, char yd)
{
	int fd;
	int i;
	
	for(i=0; i<seats; i++) {
		if((i == num) || ((fd = ggz_seats[i].fd) == -1))
			continue;

		ggz_debug("Sending player %d's move to player %d", num, i);

		if(es_write_int(fd, CC_MSG_MOVE) < 0
		   || es_write_int(fd, num) < 0
		   || es_write_char(fd, xo) < 0
		   || es_write_char(fd, yo) < 0
		   || es_write_char(fd, xd) < 0
		   || es_write_char(fd, yd) < 0)
			return -1;
	}

	return 0;
}


/* Send out board layout */
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
	unsigned char xo, yo, xd, yd;

	if(ggz_seats[num].assign == GGZ_SEAT_BOT) {
		ai_move(&xo, &yo, &xd, &yd);
		game_update(CC_EVENT_MOVE, &xo, &yo, &xd, &yd);
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
int game_handle_move(int num, unsigned char *xo, unsigned char *yo,
			      unsigned char *xd, unsigned char *yd)
{
	int fd = ggz_seats[num].fd;
	char status;

	if(num != game.turn)
		if(es_write_int(fd, CC_RSP_MOVE) < 0
		    || es_write_char(fd, CC_ERR_TURN) < 0)
			return -1;

	ggz_debug("Handling move for player %d", num);
	if(es_read_char(fd, xo) < 0)
		return -1;
	if(es_read_char(fd, yo) < 0)
		return -1;
	if(es_read_char(fd, xd) < 0)
		return -1;
	if(es_read_char(fd, yd) < 0)
		return -1;

	if(*xo >= 17 || *yo >= 25 || *xd >=17 || *yd >= 25
	   || game.board[*xo][*yo] == -1 || game.board[*xd][*yd] == -1)
		status = CC_ERR_BOUND;
	else if(game.board[*xo][*yo] != num+1)
		status = CC_ERR_INVALID;
	else if(game.board[*xd][*yd] != 0)
		status = CC_ERR_FULL;
	else if(!game_find_path(0, *xo, *yo, *xd, *yd))
		status = CC_ERR_INVALID;
	else {
		status = 0;
		game.board[*xo][*yo] = 0;
		game.board[*xd][*yd] = num+1;
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


/* Check for a win */
char game_check_win(void)
{

	/* Game not over yet */
	return -1;
}


/* Update game state */
int game_update(int event, void *d1, void *d2, void *d3, void *d4)
{
	int seat;
	char xo, yo, xd, yd;
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
		
			xo = *(char*)d1;
			yo = *(char*)d2;
			xd = *(char*)d3;
			yd = *(char*)d4;

			game_send_move(game.turn, xo, yo, xd, yd);

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
	{ 0,  3,  1,  4, -1, -1 },
	{ 0,  1,  2,  3,  4, -1 },	/* Five player game also filler */
	{ 0,  3,  1,  4,  2,  5 }
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

	game_print_board();
}


/* Display the game board in text format */
static void game_print_board(void)
{
        int i, j;

        printf("Current game board:\n");
        for(i=0; i<17; i++) {
                for(j=0; j<25; j++)
                        switch(game.board[i][j]) {
                                case -1:
				        printf(" ");
                                        break;
                                case 0:
				case 1:
				case 2:
				case 3:
				case 4:
				case 5:
				case 6:
					printf("%c", '0'+game.board[i][j]);
					break;
                                default:
				        printf("?");
                        }
                printf("\n");
        }
}


static int game_find_path(int from, int xo, int yo, int xd, int yd)
{
	/* FIXME: This code is not guaranteed to terminate! */

	if(from == 0) {
		/* Check each of the six immediate move directions for dest */
		/* We don't have to check if the cell is occupied, as we    */
		/* know the destination cell to be open */
		if((xo == xd && yo-2 == yd)
		   || (xo-1 == xd && yo-1 == yd)
		   || (xo-1 == xd && yo+1 == yd)
		   || (xo == xd && yo+2 == yd)
		   || (xo+1 == xd && yo+1 == yd)
		   || (xo+1 == xd && yo-1 == yd) )
			return 1;

		/* Check all six possible jump directions recursively */
		if((yo-4 >= 0)
		   && (game.board[xo][yo-2] > 0)
		   && (game.board[xo][yo-4] == 0)
		   && game_find_path(1, xo, yo-4, xd, yd) )
			return 1;
		if((yo-2 >= 0 && xo-2 >= 0)
		   && (game.board[xo-1][yo-1] > 0)
		   && (game.board[xo-2][yo-2] == 0)
		   && game_find_path(2, xo-2, yo-2, xd, yd) )
			return 1;
		if((yo+2 < 25 && xo-2 >= 0)
		   && (game.board[xo-1][yo+1] > 0)
		   && (game.board[xo-2][yo+2] == 0)
		   && game_find_path(3, xo-2, yo+2, xd, yd) )
			return 1;
		if((yo+4 < 25)
		   && (game.board[xo][yo+2] > 0)
		   && (game.board[xo][yo+4] == 0)
		   && game_find_path(4, xo, yo+4, xd, yd) )
			return 1;
		if((yo+2 < 25 && xo+2 < 17)
		   && (game.board[xo+1][yo+1] > 0)
		   && (game.board[xo+2][yo+2] == 0)
		   && game_find_path(5, xo+2, yo+2, xd, yd) )
			return 1;
		if((yo-2 >= 0 && xo+2 < 17)
		   && (game.board[xo+1][yo-1] > 0)
		   && (game.board[xo+2][yo-2] == 0)
		   && game_find_path(6, xo+2, yo-2, xd, yd) )
			return 1;

		/* Failed in all six directions, not a valid move */
		return 0;
	}

	/* Is it soup yet? */
	if(xo == xd && yo == yd)
		return 1;

	/* Check all six possible jump directions recursively */
	/* EXCEPT the one we came from */
	if((from != 4)
	   && (yo-4 >= 0)
	   && (game.board[xo][yo-2] > 0)
	   && (game.board[xo][yo-4] == 0)
	   && game_find_path(1, xo, yo-4, xd, yd) )
		return 1;
	if((from != 5)
	   && (yo-2 >= 0 && xo-2 >= 0)
	   && (game.board[xo-1][yo-1] > 0)
	   && (game.board[xo-2][yo-2] == 0)
	   && game_find_path(2, xo-2, yo-2, xd, yd) )
		return 1;
	if((from != 6)
	   && (yo+2 < 25 && xo-2 >= 0)
	   && (game.board[xo-1][yo+1] > 0)
	   && (game.board[xo-2][yo+2] == 0)
	   && game_find_path(3, xo-2, yo+2, xd, yd) )
		return 1;
	if((from != 1)
	   && (yo+4 < 25)
	   && (game.board[xo][yo+2] > 0)
	   && (game.board[xo][yo+4] == 0)
	   && game_find_path(4, xo, yo+4, xd, yd) )
		return 1;
	if((from != 2)
	   && (yo+2 < 25 && xo+2 < 17)
	   && (game.board[xo+1][yo+1] > 0)
	   && (game.board[xo+2][yo+2] == 0)
	   && game_find_path(5, xo+2, yo+2, xd, yd) )
		return 1;
	if((from != 3)
	   && (yo-2 >= 0 && xo+2 < 17)
	   && (game.board[xo+1][yo-1] > 0)
	   && (game.board[xo+2][yo-2] == 0)
	   && game_find_path(6, xo+2, yo-2, xd, yd) )
		return 1;

	return 0;
}
