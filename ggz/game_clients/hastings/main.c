/*
 * File: main.c
 * Author: Josef Spillner
 * Original Author: Brent Hendricks
 * Project: GGZ Hastings1066 game module
 * Date: 09/13/00
 * Desc: Main loop
 *
 * Copyright (C) 2000 Josef Spillner
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

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <gtk/gtk.h>
#include <stdio.h>
#include <unistd.h>

#include <easysock.h>
#include <game.h>
#include <main_win.h>


/* main window widget */
extern GtkWidget *main_win;

/* Global game variables */
struct game_state_t game;


int main(int argc, char* argv[])
{
	gtk_init (&argc, &argv);

/*dirtyhack*/
game.playernum = 8;

	game_init();

	main_win = create_main_win();
	gtk_widget_show(main_win);

	display_board();

	ggz_connect();
	gdk_input_add(game.fd, GDK_INPUT_READ, game_handle_io, NULL);

	gtk_main();

	return 0;
}


void game_handle_io(gpointer data, gint source, GdkInputCondition cond)
{
	int op;

	if (es_read_int(game.fd, &op) < 0) {
		/* FIXME: do something here...*/
		return;
	}

	switch(op) {

	case HASTINGS_MSG_SEAT:
		get_seat();
		break;

	case HASTINGS_MSG_PLAYERS:
		get_players();
		game.state = STATE_WAIT;
		break;

	case HASTINGS_REQ_MOVE:
		game.state = STATE_SELECT;
/* improve this? needs to know what number player has */
game.self = game.num;
		game_status("Your move: select a knight");
		break;

	case HASTINGS_RSP_MOVE:
		get_move_status();
		display_board();
		break;

	case HASTINGS_MSG_MOVE:
		get_opponent_move();
		display_board();
		break;

	case HASTINGS_SND_SYNC:
		get_sync();
		display_board();
		break;

	case HASTINGS_MSG_GAMEOVER:
		get_gameover();
		game.state = STATE_DONE;
		break;
	}

}


int get_seat(void)
{
	game_status("Getting seat number");

	if (es_read_int(game.fd, &game.num) < 0)
		return -1;

	return 0;
}


int get_players(void)
{
	int i;

	game_status("Getting player names");
	for (i = 0; i < game.playernum; i++) {
		if (es_read_int(game.fd, &game.seats[i]) < 0)
			return -1;

		if (game.seats[i] != GGZ_SEAT_OPEN) {
			if (es_read_string(game.fd, (char*)&game.knightnames[i], 17) < 0)
				return -1;
			game_status("Player %d named: %s", i, game.knightnames[i]);
		}
	}

	return 0;
}


int get_opponent_move(void)
{
	game_status("Getting opponent's move");

	if ((es_read_int(game.fd, &game.num) < 0)
	|| (es_read_int(game.fd, &game.move_src_x) < 0)
	|| (es_read_int(game.fd, &game.move_src_y) < 0)
	|| (es_read_int(game.fd, &game.move_dst_x) < 0)
	|| (es_read_int(game.fd, &game.move_dst_y) < 0))
		return -1;

	game.board[game.move_src_x][game.move_src_y] = -1;
	game.board[game.move_dst_x][game.move_dst_y] = game.num;

printf("Opponent %i: From %i/%i to %i/%i!\n", game.num, game.move_src_x, game.move_src_y, game.move_dst_x, game.move_dst_y);

/* READY: adapt to hastings board
	game.board[move] = (game.num == 0 ? 'o' : 'x');
*/

	return 0;
}


int get_sync(void)
{
	int i, j;
	char turn;
	char space;

	game_status("Getting re-sync");

	if (es_read_char(game.fd, &turn) < 0)
		return -1;

	game_status("Player %d's turn", turn);

        for (i = 0; i < 6; i++) {
	        for (j = 0; j < 19; j++) {
			if (es_read_char(game.fd, &space) < 0)
				return -1;
			game_status("Unit: Read a %d ", space);
				game.board[i][j] = space;
		}
	}
        for (i = 0; i < 6; i++) {
	        for (j = 0; j < 19; j++) {
			if (es_read_char(game.fd, &space) < 0)
				return -1;
			game_status("Map: Read a %d ", space);
				game.boardmap[i][j] = space;
		}
	}
/* READY: syncing for the whole map
        for (i = 0; i < 9; i++) {
		if (es_read_char(game.fd, &space) < 0)
			return -1;
		game_status("Read a %d ", space);
		if (space >= 0)
			game.board[i] = (space == 0 ? 'x' : 'o');
	}
*/

	game_status("Sync completed");
	return 0;
}


int get_gameover(void)
{
	char winner;

	game_status("Game over");

	if (es_read_char(game.fd, &winner) < 0)
		return -1;

	switch (winner) {
	case 0:
	case 1:
		game_status("Player %d won", winner);
		break;
/* TODO: should there be a tie game at all? nope!
	case 2:
		game_status("Tie game!");
		break;
*/
	}

	return 0;
}


void game_init(void)
{
	int i, j;

/* READY: set units
	for (i = 0; i < 9; i++)
		game.board[i] = ' ';
*/

	/* set up some knights*/
	memmove(game.board[0], "11            555  ", 19);
	memmove(game.board[1], "113333         5 5 ", 19);
	memmove(game.board[2], " 1 3 4      6 6 6  ", 19);
	memmove(game.board[3], "22  44        66   ", 19);
	memmove(game.board[4], "22   4   0007777   ", 19);
	memmove(game.board[5], " 2   4   0 0 7     ", 19);
	for (i = 0; i < 6; i++)
	 	for (j = 0; j < 19; j++)
		{
			if(game.board[i][j] == 32) game.board[i][j] = -1;
			else game.board[i][j] -= 48;
		}

	/* hexagon fields: this is difficult to understand */
	/* but it works (one line is two columns, and flip it two times to fit) */
	memmove(game.boardmap[0], "xxxxxxxxxxxx xxxx  ", 19);
	memmove(game.boardmap[1], "xxxxxxxxxx     x x ", 19);
	memmove(game.boardmap[2], "xx x xxxxxxxx x x  ", 19);
	memmove(game.boardmap[3], "xx  xxxxxxxxxxxx   ", 19);
	memmove(game.boardmap[4], "xx   xxxxxxxxxxx   ", 19);
	memmove(game.boardmap[5], " x   x x x x x     ", 19);

	game.state = STATE_INIT;
}


int send_my_move(void)
{
	game_status("Sending my move: %d/%d to %d/%d", game.move_src_x, game.move_src_y, game.move_dst_x, game.move_dst_y);
	if ((es_write_int(game.fd, HASTINGS_SND_MOVE) < 0)
	    || (es_write_int(game.fd, game.move_src_x) < 0)
	    || (es_write_int(game.fd, game.move_src_y) < 0)
	    || (es_write_int(game.fd, game.move_dst_x) < 0)
	    || (es_write_int(game.fd, game.move_dst_y) < 0))
		return -1;

	game.state = STATE_WAIT;
	game_status("My status: I'm waiting (what for? AI players!)");
	return 0;
}


int send_options(void)
{
	game_status("Sending NULL options");
	return (es_write_int(game.fd, 0));
}


int get_move_status(void)
{
	char status;

	if (es_read_char(game.fd, &status) < 0)
		return -1;

	if(status != 0) game.state = STATE_SELECT;
	switch(status) {
	case HASTINGS_ERR_STATE:
		game_status("Server not ready!!");
		break;
	case HASTINGS_ERR_TURN:
		game_status("Not my turn !!");
		break;
	case HASTINGS_ERR_BOUND:
		game_status("Move out of bounds");
		break;
	case HASTINGS_ERR_EMPTY:
		game_status("Nothing to move (erm?)");
		break;
	case HASTINGS_ERR_FULL:
		game_status("Space already occupied");
		break;
	case HASTINGS_ERR_DIST:
		game_status("Hey, that is far too far!");
		break;
	case HASTINGS_ERR_MAP:
		game_status("Argh! You should play 'sink the ship' for that purpose!");
		break;
	case 0:
		game_status("Moved knight from %i/%i to %i/%i",
			game.move_src_x, game.move_src_y, game.move_dst_x, game.move_dst_y);
		game.board[game.move_src_x][game.move_src_y] = -1;
		game.board[game.move_dst_x][game.move_dst_y] = game.self;

/* READY: execute move
		game_status("Move OK");
		game.board[game.move] = (game.num == 0 ? 'x' : 'o');
		break;
*/
	}
	return 0;
}


void ggz_connect(void)
{
	char fd_name[30];
        struct sockaddr_un addr;

	/* Connect to Unix domain socket */
	sprintf(fd_name, "/tmp/Hastings.%d", getpid());

	if ( (game.fd = socket(PF_LOCAL, SOCK_STREAM, 0)) < 0)
		exit(-1);

	bzero(&addr, sizeof(addr));
	addr.sun_family = AF_LOCAL;
	strcpy(addr.sun_path, fd_name);

	if (connect(game.fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
		exit(-1);
}
