/*
 * Initial main.c file generated by Glade. Edit as required.
 * Glade will not overwrite this file.
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <stdio.h>
#include <unistd.h>

#include "interface.h"
#include "support.h"
#include "main.h"
#include "game.h"
#include "easysock.h"

GtkWidget *main_win;
struct game_t game;

static void game_init(void);
static int send_options(void);
static int request_options(void);
static void ggz_connect(void);
void game_handle_io(gpointer, gint, GdkInputCondition);
static int get_seat(void);
static int get_players(void);
static int get_options(void);
static int get_move_status(void);

int main(int argc, char *argv[])
{
	gtk_init(&argc, &argv);

	main_win = create_window();
	gtk_widget_show(main_win);

	game_init();
	ggz_connect();
	gdk_input_add(game.fd, GDK_INPUT_READ, game_handle_io, NULL);

	if(argc > 1) {
		/* Get Options (board size) from a dialog */
		board_width = 5;
		board_height = 5;
		if(send_options() < 0)
			exit(1);
	}

	board_init(0, 0);
	if(request_options() < 0)
		exit(1);

	gtk_main();
	return 0;
}


void game_handle_io(gpointer data, gint source, GdkInputCondition cond)
{
	int op, status;

	if(es_read_int(game.fd, &op) < 0) {
		/* FIXME: do something here... */
		return;
	}

	status = 0;
	switch(op) {
		case DOTS_MSG_SEAT:
			status = get_seat();
			break;
		case DOTS_MSG_PLAYERS:
			status = get_players();
			game.state = DOTS_STATE_WAIT;
			break;
		case DOTS_RSP_OPTIONS:
			if((status = get_options()) == 0)
				board_init(board_width, board_height);
			break;
		case DOTS_REQ_MOVE:
			game.state = DOTS_STATE_MOVE;
			statusbar_message("Your turn to move");
			break;
		case DOTS_MSG_MOVE_H:
			status = board_opponent_move(1);
			break;
		case DOTS_MSG_MOVE_V:
			status = board_opponent_move(0);
			break;
		case DOTS_RSP_MOVE:
			status = get_move_status();
			break;
		default:
			fprintf(stderr, "Unknown opcode received %d\n", op);
			status = -1;
			break;
	}

	if(status < 0) {
		fprintf(stderr, "Ouch!\n");
		close(game.fd);
		exit(-1);
	}
}


void game_init(void)
{
	int i, j;

	for(i=0; i<MAX_BOARD_WIDTH; i++)
		for(j=0; j<MAX_BOARD_HEIGHT-1; j++)
			vert_board[i][j] = 0;
	for(i=0; i<MAX_BOARD_WIDTH-1; i++)
		for(j=0; j<MAX_BOARD_HEIGHT; j++)
			horz_board[i][j] = 0;
	game.state = DOTS_STATE_INIT;
}


int send_options(void)
{
	fprintf(stderr, "Sending options\n");

	if(es_write_int(game.fd, 2) < 0
	   || es_write_char(game.fd, board_width) < 0
	   || es_write_char(game.fd, board_height) < 0)
		return -1;
	return 0;
}


int get_options(void)
{
	fprintf(stderr, "Getting options\n");

	if(es_read_char(game.fd, &board_width) < 0
	   || es_read_char(game.fd, &board_height) < 0)
		return -1;
	return 0;
}


int request_options(void)
{
	fprintf(stderr, "Requesting options\n");

	if(es_write_int(game.fd, DOTS_REQ_OPTIONS) < 0)
		return -1;
	return 0;
}


void ggz_connect(void)
{
	char fd_name[30];
	struct sockaddr_un addr;

	/* Connect to Unix domain socket */
	sprintf(fd_name, "/tmp/Dots.%d", getpid());

	if((game.fd = socket(PF_LOCAL, SOCK_STREAM, 0)) < 0)
		exit(-1);

	bzero(&addr, sizeof(addr));
	addr.sun_family = AF_LOCAL;
	strcpy(addr.sun_path, fd_name);

	if(connect(game.fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
		exit(-1);
}


int get_seat(void)
{
	fprintf(stderr, "Getting seat number\n");

	if(es_read_int(game.fd, &game.num) < 0)
		return -1;
	return 0;
}


int get_players(void)
{
	int i;

	fprintf(stderr, "Getting player names\n");

	for(i=0; i<2; i++) {
		if(es_read_int(game.fd, &game.seats[i]) < 0)
			return -1;
		if(game.seats[i] != GGZ_SEAT_OPEN)
			if(es_read_string(game.fd, (char*)&game.names[i], 9) <0)
				return -1;
	}
	return 0;
}


int get_move_status(void)
{
	char status;

	if(es_read_char(game.fd, &status) < 0)
		return -1;
	if(status != 0)
		fprintf(stderr, "Move status = %d, client broken!\n", status);

	return (int)status;
}
