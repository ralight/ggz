/*
 * File: main.c
 * Author: Ismael Orenstein
 * Project: GGZ Reversi game module
 * Date: 09/17/2000
 * Desc: Reversi client main game loop
 *
 * Copyright (C) 2000 Ismael Orenstein.
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


#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <easysock.h>
#include <gtk/gtk.h>

#include "game.h"
#include "support.h"

/* main window widget */
extern GtkWidget *main_win;

/* Global game variables */
struct game_state_t game;

int main(int argc, char *argv[]) {
	
	gtk_init(&argc, &argv);
  add_pixmap_directory(".");

	main_win = create_main_win();
	gtk_widget_show(main_win);

	game_init();

	strcpy(game.names[0], "empty");
	strcpy(game.names[1], "empty");

	display_board();

	ggz_connect();

	gdk_input_add(game.fd, GDK_INPUT_READ, game_handle_io, NULL);

	gtk_main();
	
	return 0;
}

void game_handle_io(gpointer data, gint fd, GdkInputCondition cond) {
	int op = -1;

	// Read the fd
	if (es_read_int(game.fd, &op) < 0) {
		printf("Couldn't read the game fd\n");
		return;
	}

	switch (op) {
		case RVR_MSG_SEAT:
			get_seat();
			break;
		case RVR_MSG_PLAYERS:
			get_players();
			game.state = RVR_STATE_WAIT;
			break;
		case RVR_MSG_SYNC:
			get_sync();
			display_board();
			break;
		case RVR_MSG_START:
			game.state = RVR_STATE_PLAYING;
			printf("Game has started\n");
			display_board();
			break;
		case RVR_MSG_MOVE:
			get_move();
			display_board();
			break;
		case RVR_MSG_GAMEOVER:
			get_gameover();
			game.state = RVR_STATE_DONE;
			break;
		default:
			game_status("Ops! Wrong message: %d", op);
			break;
	}
}

		


void game_init() {
	int i;

	for (i = 0; i < 64; i++) {
		game.board[i] = EMPTY;
	}

	game.board[CART(4,4)] = WHITE;
	game.board[CART(5,5)] = WHITE;
	game.board[CART(4,5)] = BLACK;
	game.board[CART(5,4)] = BLACK;

	game.black = 2;
	game.white = 2;
	game.turn = BLACK;
	game.state = RVR_STATE_INIT;
	game.last_move = -1;


}

void ggz_connect(void) {
	char fd_name[30];
	struct sockaddr_un addr;
						 
	/* Connect to Unix domain socket */
	sprintf(fd_name, "/tmp/Reversi.%d", getpid());
		 
	if ( (game.fd = socket(PF_LOCAL, SOCK_STREAM, 0)) < 0)
		exit(-1);
		 
	bzero(&addr, sizeof(addr));
	addr.sun_family = AF_LOCAL;
	strcpy(addr.sun_path, fd_name);
					 
	if (connect(game.fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
		exit(-1);

}    

int get_seat() {

	if (es_read_int(game.fd, &game.num) < 0) 
		return -1;

	game.num = SEAT2PLAYER(game.num);

	game_status("Getting seat number: %d\n", game.num);

	return 0;
}

int get_players() {
  int i;
	 
	for (i = 0; i < 2; i++) {
		if (es_read_int(game.fd, &game.seats[i]) < 0)
			return -1;
	 
		if (game.seats[i] != GGZ_SEAT_OPEN) {
			if (es_read_string(game.fd, (char*)&game.names[i], 17) < 0)
				return -1;
			game_status("Player %d named: %s", i, game.names[i]);
		}
	}

	return 0; 

}

// Get the move from the server and makes it (don't check for validity!!)
int get_move() {
	int move;

	if (es_read_int(game.fd, &move) < 0)
		return -1;

	if (move == RVR_ERROR_CANTMOVE) {
		// Skip move
		game.turn*=-1;
		return 1;
	}

	if (move < 0) {
		return -1;
	}

	game_make_move(game.turn, move);

	return 0;

}

// make the move
void game_make_move(int player, int move) {
	int x = X(move), y = Y(move);

	// Make the move
	game.board[move] = player;

	// Now goes through all directions, marking the board
	
	game_mark_board(player, 0,-1, x, y);
	game_mark_board(player, 1,-1, x, y);
	game_mark_board(player, 1, 0, x, y);
	game_mark_board(player, 1, 1, x, y);
	game_mark_board(player, 0, 1, x, y);
	game_mark_board(player,-1, 1, x, y);
	game_mark_board(player,-1, 0, x, y);
	game_mark_board(player,-1,-1, x, y);

	// Change turn
	game.turn*=-1;

	// Update scores
	game_update_scores();
	
	// Update last move
	game.last_move = move;

	return;

}


void game_mark_board(int player, int vx, int vy, int x, int y) {
	int i, j;
	if (!game_check_direction(player, vx, vy, x, y))
		return;
	for (i = x+vx, j = y+vy; GET(i,j) == -player; i+=vx, j+=vy) {
		game.board[CART(i,j)]*=-1;
	}
	return;
}


int game_check_direction(int player, int vx, int vy, int x, int y) {
	int i, j;
	for (i = x+vx, j = y+vy; GET(i,j) == -player; i+=vx, j+=vy) {
		if (GET(i+vx,j+vy) == player)
			return 1;
	}
	return 0;
}

void send_my_move(int move) {
	if (es_write_int(game.fd, RVR_REQ_MOVE) < 0 || es_write_int(game.fd, move) < 0) {
		game_status("Can't send move!");
		return;
	}
	printf("Sent move: %d\n", move);
}

int request_sync() {
	if (es_write_int(game.fd, RVR_REQ_SYNC) < 0) {
		// Not that someone would check this return value, but...
		return -1;
	} else {
		game_status("Requesting sync from the server");
	}
	return 0;
}
		
int get_sync() {
	char fboard[64];
	char fturn;
	int i;
	if (es_read_char(game.fd, &fturn) < 0)
		return -1;
	for (i = 0; i < 64; i++) {
		if (es_read_char(game.fd, &fboard[i]) < 0)
			return -1;
	}
	// Ok, everything worked well!
	// Some sanity tests
	if (fturn > -2 && fturn < 2) {
		if (game.turn != fturn)
			game_status("You have been synchronized");
		game.turn = fturn;
	}
	for (i = 0; i < 64; i++) {
		if (fboard[i] != game.board[i])
			game_status("You have been synchoronized");
	 	if (fboard[i] == BLACK || fboard[i] == WHITE || fboard[i] == EMPTY)
			game.board[i] = fboard[i];
	} 
	return 0;
}

void game_update_scores() {
	int i;
	game.white = 0;
	game.black = 0;
	for (i = 0; i < 64; i++) {
		if (game.board[i] == WHITE)
			game.white++;
		if (game.board[i] == BLACK)
			game.black++;
	}
}
