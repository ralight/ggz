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

#include "dlg_main.h"
#include "dlg_opt.h"
#include "support.h"
#include "main.h"
#include "game.h"
#include "easysock.h"

GtkWidget *main_win;
GtkWidget *opt_dialog;
struct game_t game;

static int request_options(void);
static void ggz_connect(void);
static void game_handle_io(gpointer, gint, GdkInputCondition);
static int get_seat(void);
static int get_players(void);
static int get_options(void);
static int get_move_status(void);
static int get_gameover_status(void);

int main(int argc, char *argv[])
{
	gtk_init(&argc, &argv);

	ggz_connect();
	gdk_input_add(game.fd, GDK_INPUT_READ, game_handle_io, NULL);

	if(argc > 1) {
		/* Get options from a dialog */
		opt_dialog = create_dlg_opt();
		gtk_widget_show(opt_dialog);
	} else
		game_init();

	gtk_main();
	return 0;
}


char *opstr[] = { "DOTS_MSG_SEAT",   "DOTS_MSG_PLAYERS",  "DOTS_MSG_MOVE_H",
		  "DOTS_MSG_MOVE_V", "DOTS_MSG_GAMEOVER", "DOTS_REQ_MOVE",
		  "DOTS_RSP_MOVE",   "DOTS_SND_SYNC",     "DOTS_RSP_OPTIONS" };

static void game_handle_io(gpointer data, gint source, GdkInputCondition cond)
{
	int op, status;

	if(es_read_int(game.fd, &op) < 0) {
		/* FIXME: do something here... */
		return;
	}

	status = 0;
	fprintf(stderr, "%s\n", opstr[op]);
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
			if(game.move == game.me)
				statusbar_message("You get to move again!");
			else
				statusbar_message("Your turn to move");
			game.move = game.me;
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
		case DOTS_MSG_GAMEOVER:
			status = get_gameover_status();
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

	/* Setup the main board now */
	main_win = create_dlg_main();
	gtk_widget_show(main_win);
	board_init(0, 0);

	/* Get options from the server */
	if(request_options() < 0)
		exit(1);
}


int send_options(void)
{
	if(es_write_int(game.fd, 2) < 0
	   || es_write_char(game.fd, board_width) < 0
	   || es_write_char(game.fd, board_height) < 0)
		return -1;
	return 0;
}


static int get_options(void)
{
	if(es_read_char(game.fd, &board_width) < 0
	   || es_read_char(game.fd, &board_height) < 0)
		return -1;
	return 0;
}


static int request_options(void)
{
	if(es_write_int(game.fd, DOTS_REQ_OPTIONS) < 0)
		return -1;
	return 0;
}


static void ggz_connect(void)
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


static int get_seat(void)
{
	if(es_read_int(game.fd, &game.me) < 0)
		return -1;
	game.opponent = (game.me+1)%2;
	return 0;
}


static int get_players(void)
{
	int i;

	for(i=0; i<2; i++) {
		if(es_read_int(game.fd, &game.seats[i]) < 0)
			return -1;
		if(game.seats[i] != GGZ_SEAT_OPEN)
			if(es_read_string(game.fd, (char*)&game.names[i], 9) <0)
				return -1;
	}
	return 0;
}


static int get_move_status(void)
{
	char status;

	if(es_read_char(game.fd, &status) < 0)
		return -1;
	if(status != 0)
		fprintf(stderr, "Move status = %d, client broken!\n", status);

	return (int)status;
}


static int get_gameover_status(void)
{
	char status;
	gchar *tstr;

	if(es_read_char(game.fd, &status) < 0)
		return -1;
	if(status == game.me) {
		tstr = g_strconcat("Game over, you beat ",
				   game.names[game.opponent], "!", NULL);
		statusbar_message(tstr);
	} else if(status == game.opponent) {
		tstr = g_strconcat("Game over, ",
				   game.names[game.opponent], " won :(", NULL);
		statusbar_message(tstr);
	} else {
		tstr = g_strconcat("Game over, you tied with ",
				   game.names[game.opponent], ".", NULL);
		statusbar_message(tstr);
	}
	g_free(tstr);

	game.state = DOTS_STATE_WAIT;
	return 0;
}
