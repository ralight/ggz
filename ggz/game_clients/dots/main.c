/*
 * File: main.c
 * Author: Rich Gade
 * Project: GGZ Connect the Dots Client
 * Date: 08/14/2000
 * Desc: Main loop and supporting logic
 * $Id: main.c 3174 2002-01-21 08:09:42Z jdorje $
 *
 * Copyright (C) 2000, 2001 Brent Hendricks.
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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include <ggz.h>
#include <ggzmod.h>

#include "dlg_main.h"
#include "dlg_opt.h"
#include "dlg_new.h"
#include "support.h"
#include "main.h"
#include "game.h"

GtkWidget *main_win = NULL;
GtkWidget *opt_dialog;
GtkWidget *new_dialog;
struct game_t game;
int conf_handle;

static void game_handle_io(gpointer, gint, GdkInputCondition);
static int get_seat(void);
static int get_players(void);
static int get_options(void);
static int get_move_status(void);
static int get_gameover_status(void);
static int get_sync_info(void);

int main(int argc, char *argv[])
{
	char *filename;

	gtk_init(&argc, &argv);

	game.fd = ggzmod_connect();
	if (game.fd < 0) return -1;

	gdk_input_add(game.fd, GDK_INPUT_READ, game_handle_io, NULL);

	filename = g_strdup_printf("%s/.ggz/dots-gtk.rc", getenv("HOME"));
	conf_handle = ggz_conf_parse(filename,
					   GGZ_CONF_RDWR |
					   GGZ_CONF_CREATE);
	g_free(filename);

	game.state = DOTS_STATE_INIT;
	game_init();

	gtk_main();

	if (ggzmod_disconnect() < 0)
		return -2;

	return 0;
}


#ifdef DEBUG
char *opstr[] = { "DOTS_MSG_SEAT",   "DOTS_MSG_PLAYERS",  "DOTS_MSG_MOVE_H",
		  "DOTS_MSG_MOVE_V", "DOTS_MSG_GAMEOVER", "DOTS_REQ_MOVE",
		  "DOTS_RSP_MOVE",   "DOTS_SND_SYNC",     "DOTS_MSG_OPTIONS" };
#endif

static void game_handle_io(gpointer data, gint source, GdkInputCondition cond)
{
	int op, status;

	if(ggz_read_int(game.fd, &op) < 0) {
		/* FIXME: do something here... */
		return;
	}

	status = 0;
#ifdef DEBUG
	fprintf(stderr, "%s\n", opstr[op]);
#endif
	switch(op) {
		case DOTS_MSG_SEAT:
			status = get_seat();
			break;
		case DOTS_MSG_PLAYERS:
			status = get_players();
			if(game.state != DOTS_STATE_CHOOSE)
				game.state = DOTS_STATE_WAIT;
			break;
		case DOTS_MSG_OPTIONS:
			if((status = get_options()) == 0)
				board_init(board_width, board_height);
			break;
		case DOTS_REQ_MOVE:
			/* We ignore this unless we've seen an opponents move */
			/*if(game.state != DOTS_STATE_OPPONENT) {*/
				game.state = DOTS_STATE_MOVE;
				statusbar_message("Your turn to move");
				game.move = game.me;
				break;
			/*}*/
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
		case DOTS_SND_SYNC:
			status = get_sync_info();
			break;
		case DOTS_REQ_OPTIONS:
			opt_dialog = create_dlg_opt();
			gtk_widget_show(opt_dialog);
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
	GtkWidget *l1, *l2;

	for(i=0; i<MAX_BOARD_WIDTH; i++)
		for(j=0; j<MAX_BOARD_HEIGHT-1; j++)
			vert_board[i][j] = 0;
	for(i=0; i<MAX_BOARD_WIDTH-1; i++)
		for(j=0; j<MAX_BOARD_HEIGHT; j++)
			horz_board[i][j] = 0;
	for(i=0; i<MAX_BOARD_WIDTH-1; i++)
		for(j=0; j<MAX_BOARD_HEIGHT-1; j++)
			owners_board[i][j] = -1;
	game.score[0] = 0;
	game.score[1] = 0;
	game.got_players = 0;

	/* Setup the main board now */
	if(main_win == NULL)
		main_win = create_dlg_main();
	l1 = gtk_object_get_data(GTK_OBJECT(main_win), "lbl_score0");
	l2 = gtk_object_get_data(GTK_OBJECT(main_win), "lbl_score1");
	gtk_label_set_text(GTK_LABEL(l1), "No Score");
	gtk_label_set_text(GTK_LABEL(l2), "No Score");
	gtk_widget_show(main_win);
	board_init(0, 0);

	game.state = DOTS_STATE_INIT;
}


int send_options(void)
{
	if(ggz_write_int(game.fd, DOTS_SND_OPTIONS) < 0
	   || ggz_write_char(game.fd, board_width) < 0
	   || ggz_write_char(game.fd, board_height) < 0)
		return -1;
	return 0;
}


static int get_options(void)
{
	if(ggz_read_char(game.fd, &board_width) < 0
	   || ggz_read_char(game.fd, &board_height) < 0)
		return -1;
	return 0;
}


static int get_seat(void)
{
	if(ggz_read_int(game.fd, &game.me) < 0)
		return -1;
	game.opponent = (game.me+1)%2;
	return 0;
}


static int get_players(void)
{
	int i;
	GtkWidget *frame[2];
	char *temp;

	frame[0] = gtk_object_get_data(GTK_OBJECT(main_win), "frame_left");
	frame[1] = gtk_object_get_data(GTK_OBJECT(main_win), "frame_right");

	for(i=0; i<2; i++) {
		if(ggz_read_int(game.fd, &game.seats[i]) < 0)
			return -1;
		if(game.seats[i] != GGZ_SEAT_OPEN) {
			if(ggz_read_string(game.fd, (char*)&game.names[i], 17)<0)
				return -1;
			temp = g_strdup_printf("   %s   ", game.names[i]);
			gtk_frame_set_label(GTK_FRAME(frame[i]), game.names[i]);
			g_free(temp);
			if(i != game.me) {
				temp = g_strdup_printf("%s joined the table",
							game.names[i]);
				statusbar_message(temp);
				g_free(temp);
			}
		} else {
			gtk_frame_set_label(GTK_FRAME(frame[i]), "Empty Seat");
			if(game.got_players) {
				temp = g_strdup_printf("%s left the table",
							game.names[i]);
				statusbar_message(temp);
				g_free(temp);
			}
		}
	}

	game.got_players++;

	return 0;
}


static int get_move_status(void)
{
	char status;
	char t_s;
	int i;
	char t_x, t_y;

	if(ggz_read_char(game.fd, &status) < 0
	   || ggz_read_char(game.fd, &t_s) < 0)
		return -1;
	for(i=0; i<t_s; i++) {
		if(ggz_read_char(game.fd, &t_x) < 0
		   || ggz_read_char(game.fd, &t_y) < 0)
			return -1;
	}

	if(status < 0)
		fprintf(stderr, "Client cheater!\n");

	return (int)status;
}


static int get_sync_info(void)
{
	int i,j;
	gchar *text;
	GtkWidget *l1, *l2;

	if(ggz_read_char(game.fd, &game.move) < 0
	   || ggz_read_int(game.fd, &game.score[0]) < 0
	   || ggz_read_int(game.fd, &game.score[1]) < 0)
		return -1;
	for(i=0; i<board_width; i++)
		for(j=0; j<board_height-1; j++)
			if(ggz_read_char(game.fd, &vert_board[i][j]) < 0)
				return -1;
	for(i=0; i<board_width-1; i++)
		for(j=0; j<board_height; j++)
			if(ggz_read_char(game.fd, &horz_board[i][j]) < 0)
				return -1;
	for(i=0; i<board_width-1; i++)
		for(j=0; j<board_height-1; j++)
			if(ggz_read_char(game.fd, &owners_board[i][j]) < 0)
				return -1;

	board_redraw();

	if(game.score[0] != 0 || game.score[1] != 0)
	{
		l1 = gtk_object_get_data(GTK_OBJECT(main_win), "lbl_score0");
		l2 = gtk_object_get_data(GTK_OBJECT(main_win), "lbl_score1");
		text = g_strdup_printf("Score = %d", game.score[0]);
		gtk_label_set_text(GTK_LABEL(l1), text);
		g_free(text);
		text = g_strdup_printf("Score = %d", game.score[1]);
		gtk_label_set_text(GTK_LABEL(l2), text);
		g_free(text);
	}

	return 0;
}


static int get_gameover_status(void)
{
	char status;
	gchar *tstr;
	GtkWidget *lbl_winner, *lbl_score;

	if(ggz_read_char(game.fd, &status) < 0)
		return -1;

	/* Create the New Game dialog */
	new_dialog = create_dlg_new();
	lbl_winner = gtk_object_get_data(GTK_OBJECT(new_dialog), "lbl_winner");
	lbl_score = gtk_object_get_data(GTK_OBJECT(new_dialog), "lbl_score");

	if(status == game.me) {
		tstr = g_strconcat("Game over, you beat ",
				   game.names[game.opponent], "!", NULL);
		statusbar_message(tstr);
		g_free(tstr);
		tstr = g_strconcat("You beat ", game.names[game.opponent],NULL);
	} else if(status == game.opponent) {
		tstr = g_strconcat("Game over, ",
				   game.names[game.opponent], " won :(", NULL);
		statusbar_message(tstr);
		g_free(tstr);
		tstr = g_strconcat("You lost to ", game.names[game.opponent],
				   NULL);
	} else {
		tstr = g_strconcat("Game over, you tied with ",
				   game.names[game.opponent], ".", NULL);
		statusbar_message(tstr);
		g_free(tstr);
		tstr = g_strconcat("You tied with ", game.names[game.opponent],
				   NULL);
	}
	gtk_label_set_text(GTK_LABEL(lbl_winner), tstr);
	g_free(tstr);
	tstr = g_strdup_printf("%d to %d.", game.score[game.me],
					    game.score[game.opponent]);
	gtk_label_set_text(GTK_LABEL(lbl_score), tstr);
	g_free(tstr);

	gtk_widget_show(new_dialog);

	game.state = DOTS_STATE_CHOOSE;
	return 0;
}


void handle_req_newgame(void)
{
	/* Reinitialize the game data and board */
	game_init();
	game.got_players = 1;

	/* Send a game request to the server */
	ggz_write_int(game.fd, DOTS_REQ_NEWGAME);
}
