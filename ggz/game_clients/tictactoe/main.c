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
#include <gtk/gtk.h>
#include <stdio.h>
#include <unistd.h>

#include <easysock.h>
#include <main_win.h>

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


extern GdkPixmap* ttt_buf;
GtkWidget *main_win;
char board[3][3];

int fd;
int num;
int move;
int seat[2];
char name[2][9];
char gameover;

void ggz_connect(void);
void game_handle_io(gpointer data, gint fd, GdkInputCondition cond);
void init_board(void);
int get_seat(int* num);
int get_players(void);
int get_my_move(void);
int send_my_move(int move);
int make_my_move(int move);
int get_opponent_move(void);
int get_sync(void);
int get_gameover(void);


int main(int argc, char* argv[])
{
	gtk_init (&argc, &argv);

	main_win = create_main_win();
	gtk_widget_show(main_win);

	init_board();
	display_board();
	
	ggz_connect();
	gdk_input_add(fd, GDK_INPUT_READ, game_handle_io, NULL);
	
	/* Assume a second argument is the -o */
	if (argc > 1) {
		game_status("Sending NULL options");
		es_write_int(fd, 0);
	}
	
	gtk_main();
	
	return 0;
}


void game_handle_io(gpointer data, gint source, GdkInputCondition cond)
{
	int op;

	if (es_read_int(fd, &op) < 0) {
		/* FIXME: do something here...*/
		return;
	}
	
	switch(op) {
		
	case TTT_MSG_SEAT:
		get_seat(&num);
		break;
		
	case TTT_MSG_PLAYERS:
		get_players();
		break;
		
	case TTT_REQ_MOVE:
		get_my_move();
		break;
		
	case TTT_RSP_MOVE:
		make_my_move(move);
		display_board();
		break;
		
	case TTT_MSG_MOVE:
		get_opponent_move();
		display_board();
		break;
		
	case TTT_SND_SYNC:
		get_sync();
		display_board();
		break;
		
	case TTT_MSG_GAMEOVER:
		get_gameover();
		gameover = 1;
		break;
	}

}		


int get_seat(int* num)
{
	game_status("Getting seat number");

	if (es_read_int(fd, num) < 0)
		return -1;
	
	return 0;
}


int get_players(void)
{
	int i;

	game_status("Getting player names");
	for (i = 0; i < 2; i++) {
		if (es_read_int(fd, &seat[i]) < 0)
			return -1;
		
		if (seat[i] != GGZ_SEAT_OPEN) {
			if (es_read_string(fd, name[i], 9) < 0)
				return -1;
			game_status("Player %d named: %s", i, name[i]);
		}
	}

	return 0;
}


int get_my_move(void)
{
	
	game_status("Your move");
	/*scanf("%d", &move);*/
	
	return 0;
}


int get_opponent_move(void)
{
	
	game_status("Getting opponent's move");
	
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
	
	game_status("Getting re-sync");
	
	if (es_read_char(fd, &turn) < 0)
		return -1;

	game_status("Player %d's turn", turn);
	
        for (i = 0; i < 3; i++)
		for (j = 0; j < 3; j++) {
			if (es_read_char(fd, &space) < 0)
				return -1;
			game_status("Read a %d ", space);
			if (space >= 0)
				board[i][j] = (space == 0 ? 'x' : 'o');
		}
	
	game_status("Sync completed");
	return 0;
}


int get_gameover(void)
{
	char winner;
	
	game_status("Game over");
	
	if (es_read_char(fd, &winner) < 0)
		return -1;
	
	switch (winner) {
	case 0:
	case 1:
		game_status("Player %d won", winner);
		break;
	case 2:
		game_status("Tie game!");
		break;
	}

	return 0;
}


void init_board(void)
{
	int i,j;
  
	for (i = 0; i < 3; i++)
		for (j = 0; j < 3; j++)
			board[i][j] = ' ';
}


int send_my_move(int move)
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
		game_status("Server not ready!!");
		break;
	case TTT_ERR_TURN:
		game_status("Not my turn !!");
		break;
	case TTT_ERR_BOUND:
		game_status("Move out of bounds");
		break;
	case TTT_ERR_FULL:
		game_status("Space already occupied");
		break;
	case 0:
		game_status("Move OK");
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
