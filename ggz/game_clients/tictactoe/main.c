/*
 * File: main.c
 * Author: Brent Hendricks
 * Project: GGZ Tic-Tac-Toe game module
 * Date: 3/31/00
 * Desc: Main loop
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

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <stdio.h>
#include <unistd.h>

#include <easysock.h>

/* Tic-Tac-Toe protocol */
/* Messages from server */
#define TTT_MSG_SEAT     0
#define TTT_MSG_PLAYERS  1
#define TTT_MSG_MOVE     2
#define TTT_MSG_GAMEOVER 3
#define TTT_REQ_MOVE     4
#define TTT_RSP_MOVE     5
#define TTT_SND_SYNC     6

/* Move errors */
#define TTT_ERR_STATE   -1
#define TTT_ERR_TURN    -2
#define TTT_ERR_BOUND   -3
#define TTT_ERR_FULL    -4

/* Messages from client */
#define TTT_SND_MOVE     0
#define TTT_REQ_SYNC     1


#define GGZ_SEAT_OPEN   -1


int fd;
char board[3][3];

int num;
int seat[2];
char name[2][9];

void ggz_connect(void);
void init_board(void);
void display_board(void);
int get_seat(int* num);
int get_players(void);
int get_my_move(void);
int send_my_move(int fd, int move);
int make_my_move(int move);
int get_opponent_move(int fd);
int get_sync(void);
int get_gameover(void);


int main(int argc, char* argv[])
{
	int move, op;
	char game_over = 0;

	ggz_connect();

	/* Assume a second argument is the -o */
	if (argc > 1) {
		fprintf(stderr, "Sending NULL options\n");
		es_write_int(fd, 0);
	}

	init_board();
	display_board();
	while(!game_over) {

		if (es_read_int(fd, &op) < 0)
			return -1;

		switch(op) {

		case TTT_MSG_SEAT:
			get_seat(&num);
			break;
			
		case TTT_MSG_PLAYERS:
			get_players();
			break;

		case TTT_REQ_MOVE:
			move = get_my_move();
			send_my_move(fd, move);
			break;

		case TTT_RSP_MOVE:
			make_my_move(move);
			display_board();
			break;

		case TTT_MSG_MOVE:
			get_opponent_move(fd);
			display_board();
			break;

		case TTT_SND_SYNC:
			get_sync();
			display_board();
			break;

		case TTT_MSG_GAMEOVER:
			get_gameover();
			game_over = 1;
			break;
		}
	}
	
	return 0;
}


int get_seat(int* num)
{
	fprintf(stderr, "Getting seat number\n");

	if (es_read_int(fd, num) < 0)
		return -1;
	
	return 0;
}


int get_players(void)
{
	int i;

	fprintf(stderr, "Getting player names\n");
	for (i = 0; i < 2; i++) {
		if (es_read_int(fd, &seat[i]) < 0)
			return -1;
		/*fprintf(stderr, "Player %d is %d\n", i, seat[i]);*/
		
		if (seat[i] != GGZ_SEAT_OPEN) {
			if (es_read_string(fd, name[i], 9) < 0)
				return -1;
			fprintf(stderr, "Player %d named: %s\n", i, name[i]);
		}
	}

	return 0;
}


int get_my_move(void)
{
	int move;
	
	printf("Your move [0-8]:\n");
	scanf("%d", &move);
	
	return move;
}


int get_opponent_move(int fd)
{
	int move;
	
	fprintf(stderr, "Getting opponent's move\n");
	
	if (es_read_int(fd, &move) < 0)
		return -1;

	board[(move / 3)][(move % 3)] = (num == 0 ? 'o' : 'x');	

	return 0;
}


int get_sync(void)
{
	int i, j;
	char turn;
	char space;
	
	fprintf(stderr, "Getting re-sync\n");
	
	if (es_read_char(fd, &turn) < 0)
		return -1;

	fprintf(stderr, "Player %d's turn\n", turn);

        for (i = 0; i < 3; i++)
		for (j = 0; j < 3; j++) {
			if (es_read_char(fd, &space) < 0)
				return -1;
			if (space >= 0)
				board[i][j] = (space == 0 ? 'x' : 'o');
		}

	return 0;
}


int get_gameover(void)
{
	char winner;
	
	fprintf(stderr, "Game over\n");
	
	if (es_read_char(fd, &winner) < 0)
		return -1;
	
	switch (winner) {
	case 0:
	case 1:
		fprintf(stderr, "Player %d won\n", winner);
		break;
	case 2:
		fprintf(stderr, "Tie game!\n");
		break;
	}

	return 0;
}


void display_board(void)
{
	printf("  %c | %c | %c  \n", board[0][0], board[0][1], board[0][2]);
	printf("-------------\n");
	printf("  %c | %c | %c  \n", board[1][0], board[1][1], board[1][2]);
	printf("-------------\n");
	printf("  %c | %c | %c  \n", board[2][0], board[2][1], board[2][2]);
	printf("\n");
}


void init_board(void)
{
	int i,j;

	for (i = 0; i < 3; i++)
		for (j = 0; j < 3; j++)
			board[i][j] = ' ';
}


int send_my_move(int fd, int move)
{
	if (es_write_int(fd, TTT_SND_MOVE) < 0
	    || es_write_int(fd, move) < 0)
		return -1;
	
	return 0;
}


int make_my_move(int move)
{
	char status;

	if (es_read_char(fd, &status) < 0)
		return -1;

	switch(status) {
	case TTT_ERR_STATE:
		fprintf(stderr, "Server not ready!!\n");
		break;
	case TTT_ERR_TURN:
		fprintf(stderr, "Not my turn !!\n");
		break;
	case TTT_ERR_BOUND:
		fprintf(stderr, "Move out of bounds\n");
		break;
	case TTT_ERR_FULL:
		fprintf(stderr, "Space already occupied\n");
		break;
	case 0:
		fprintf(stderr, "Move OK\n");
		board[(move / 3)][(move % 3)] = (num == 0 ? 'x' : 'o');
		break;
	}

	return 0;
}


void ggz_connect(void)
{
	char fd_name[30];
        struct sockaddr_un addr;
	
	/* Connect to Unix domain socket */
	sprintf(fd_name, "/tmp/TicTacToe.%d", getpid());

	if ( (fd = socket(PF_LOCAL, SOCK_STREAM, 0)) < 0)
		exit(-1);

	bzero(&addr, sizeof(addr));
	addr.sun_family = AF_LOCAL;
	strcpy(addr.sun_path, fd_name);

	if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
		exit(-1);
}
