/*
 * File: main.c
 * Author: Rich Gade
 * Project: GGZ La Pocha Client
 * Date: 08/14/2000
 * Desc: Main loop and core logic
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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <stdio.h>
#include <unistd.h>

#include <easysock.h>

#include "support.h"
#include "main.h"
#include "dlg_main.h"
#include "dlg_bid.h"
#include "table.h"
#include "game.h"
#include "hand.h"

GtkWidget *dlg_main = NULL;

/* Private functions */
static void ggz_connect(void);
static void game_handle_io(gpointer data, gint source, GdkInputCondition cond);
static void game_init(void);
static int get_seat(void);
static int get_players(void);
static int get_gameover_status(void);
static int get_sync_info(void);
static int get_bid_status(void);
static int get_player_bid(void);
static int get_trump_suit(void);
static int get_play_status(void);
static int get_opponent_play(void);
static int get_trick_winner(void);
static int get_current_scores(void);
static int get_trump_status(void);
static int get_dealer(void);


int main(int argc, char *argv[])
{
	gtk_init(&argc, &argv);

	ggz_connect();
	gdk_input_add(game.fd, GDK_INPUT_READ, game_handle_io, NULL);

	dlg_main = create_dlg_main();

	gtk_widget_show(dlg_main);

	table_initialize();
	game_init();

	gtk_main();
	return 0;
}


static void ggz_connect(void)
{
	char fd_name[30];
	struct sockaddr_un addr;

	/* Connect to Unix domain socket */
	sprintf(fd_name, "/tmp/LaPocha.%d", getpid());

	if((game.fd = socket(PF_LOCAL, SOCK_STREAM, 0)) < 0)
		exit(-1);

	bzero(&addr, sizeof(addr));
	addr.sun_family = AF_LOCAL;
	strcpy(addr.sun_path, fd_name);

	if(connect(game.fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
		exit(-1);
}


#ifdef DEBUG
char *opstr[] = { "LP_MSG_SEAT",    "LP_MSG_PLAYERS",    "LP_MSG_GAMEOVER",
		  "LP_MSG_HAND",    "LP_REQ_BID",        "LP_RSP_BID",
                  "LP_MSG_BID",     "LP_REQ_PLAY",       "LP_RSP_PLAY",
                  "LP_MSG_PLAY",    "LP_SND_SYNC",       "LP_MSG_TRUMP",
		  "LP_REQ_TRUMP",   "LP_MSG_TRICK",      "LP_MSG_SCORES",
		  "LP_RSP_TRUMP" };
#endif

static void game_handle_io(gpointer data, gint source, GdkInputCondition cond)
{
	int op, status;
	int i;

	if(es_read_int(game.fd, &op) < 0) {
		/* FIXME: do something here... */
		return;
	}

	status = 0;
#ifdef DEBUG
	fprintf(stderr, "%s\n", opstr[op]);
#endif
	switch(op) {
		case LP_MSG_SEAT:
			status = get_seat();
			break;
		case LP_MSG_PLAYERS:
			status = get_players();
			if(game.state != LP_STATE_CHOOSE)
				game.state = LP_STATE_WAIT;
			break;
		case LP_MSG_GAMEOVER:
			status = get_gameover_status();
			break;
		case LP_MSG_HAND:
			game.trump_suit = -1;
			table_set_trump();
			status = get_dealer();
			if(status == 0)
				status = hand_read_hand();
			break;
		case LP_REQ_BID:
			for(i=0; i<4; i++)
				game.tricks[i] = 0;
			game.state = LP_STATE_BID;
			dlg_bid_display(hand.hand_size);
			status = 0;
			break;
		case LP_RSP_BID:
			status = get_bid_status();
			break;
		case LP_MSG_BID:
			status = get_player_bid();
			break;
		case LP_REQ_PLAY:
			if(game.state == LP_STATE_ANIM)
				table_animation_zip(TRUE);
			game.state = LP_STATE_PLAY;
			statusbar_message("Your turn to play a card");
			status = 0;
			break;
		case LP_RSP_PLAY:
			status = get_play_status();
			break;
		case LP_MSG_PLAY:
			status = get_opponent_play();
			break;
		case LP_SND_SYNC:
			status = get_sync_info();
			break;
		case LP_MSG_TRUMP:
			status = get_trump_suit();
			break;
		case LP_REQ_TRUMP:
			status = 0;
			table_show_cards(0, 13, 39, 26);
			game.state = LP_STATE_TRUMP;
			statusbar_message("Please choose the trump suit");
			break;
		case LP_MSG_TRICK:
			status = get_trick_winner();
			break;
		case LP_MSG_SCORES:
			status = get_current_scores();
			break;
		case LP_RSP_TRUMP:
			status = get_trump_status();
			break;
		default:
			fprintf(stderr, "Unknown opcode received %d\n", op);
			status = -1;
			break;
	}

	if(status == -1) {
		fprintf(stderr, "Lost connection to server?!\n");
		close(game.fd);
		exit(-1);
	}
}


static void game_init(void)
{
	statusbar_message("Waiting for server");
	game.state = LP_STATE_INIT;
}


static int get_seat(void)
{
	if(es_read_int(game.fd, &game.me) < 0)
		return -1;
	return 0;
}


static int get_players(void)
{
	int i;
	char *temp;
	char t_name[17];

	for(i=0; i<4; i++) {
		if(es_read_int(game.fd, &game.seats[i]) < 0)
			return -1;
		if(game.seats[i] != GGZ_SEAT_OPEN) {
			if(es_read_string(game.fd, (char *)&t_name, 17) < 0)
				return -1;
			if(i != game.me && game.got_players
			   &&  strcmp(t_name, game.names[i])) {
				temp = g_strdup_printf("%s joined the table",
							t_name);
				statusbar_message(temp);
				g_free(temp);
			}
			strcpy(game.names[i], t_name);
			table_set_name(i, game.names[i]);
		} else {
			table_set_name(i, "Empty Seat");
			if(game.names[i][0] != '\0' && game.got_players) {
				temp = g_strdup_printf("%s left the table",
							game.names[i]);
				game.names[i][0] = '\0';
				statusbar_message(temp);
				g_free(temp);
			}
		}
	}

	game.got_players++;

	return 0;
}


static int get_sync_info(void)
{
	int i;
	char state, tosschar;
	char cur_table[4];

	/* Receive game turn */
	if(es_read_char(game.fd, &tosschar) < 0)
		return -1;

	/* Receive scores from the server */
	for(i=0; i<4; i++) {
		if(es_read_int(game.fd, &game.score[i]) < 0)
			return -1;
		table_set_score(i, game.score[i]);
	}

	/* Get our game hand */
	if(hand_read_hand() < 0)
		return -1;

	/* Get the server's game state */
	if(es_read_char(game.fd, &state) < 0)
		return -1;

	/* Get info based on state */
	switch(state) {
		case LP_SERVER_INIT:
			/* Can't occur */
			break;
		case LP_SERVER_WAIT:
			/* Can't occur */
			break;
		case LP_SERVER_NEW_HAND:
			/* Can't occur */
			break;
		case LP_SERVER_GET_TRUMP:
			/* Can't occur */
			break;
		case LP_SERVER_BIDDING:
			/* Can't occur */
			break;
		case LP_SERVER_PLAYING:
			if(es_read_char(game.fd, &game.dealer) < 0
			   || es_read_char(game.fd, &tosschar) < 0
			   || es_read_char(game.fd, &game.trump_suit) < 0
			   || es_read_char(game.fd, &game.lead) < 0)
				return -1;

			table_set_trump();

			/* Get all four bids, trick counts and cards on table */
			for(i=0; i<4; i++) {
				if(es_read_int(game.fd, &game.bid[i]) < 0)
					return -1;
				table_set_bid(i, game.bid[i]);
			}
			for(i=0; i<4; i++) {
				if(es_read_int(game.fd, &game.tricks[i]) < 0)
					return -1;
				table_set_tricks(i, game.tricks[i]);
			}
			for(i=0; i<4; i++)
				if(es_read_char(game.fd, &cur_table[i]) < 0)
					return -1;

			/* Display the cards on the table */
			table_show_cards_pnum(cur_table[0], cur_table[1],
					      cur_table[2], cur_table[3]);

			game.state = LP_STATE_WAIT;
		case LP_SERVER_DONE:
			/* Can't occur */
			break;
	}

	return 0;
}


static int get_gameover_status(void)
{
	char winner;
	char *t_str;

	if(es_read_char(game.fd, &winner) < 0)
		return -1;

	t_str = g_strdup_printf("%s won the game", game.names[(int)winner]);
	statusbar_message(t_str);
	g_free(t_str);
	game.state = LP_STATE_DONE;

	return 0;
}


static void handle_req_newgame(void)
{
	/* Reinitialize the game data and board */
	game_init();

	/* Send a game request to the server */
	es_write_int(game.fd, LP_REQ_NEWGAME);
}


void statusbar_message(char *msg)
{
	static GtkWidget *sb=NULL;
	static guint sb_context;

	if(sb == NULL) {
		sb = gtk_object_get_data(GTK_OBJECT(dlg_main), "statusbar1");
		sb_context = gtk_statusbar_get_context_id(GTK_STATUSBAR(sb),
							  "Game Messages");
	}

	gtk_statusbar_push(GTK_STATUSBAR(sb), sb_context, msg);
}


static int get_bid_status(void)
{
	char status;

	if(es_read_char(game.fd, &status) < 0)
		return -1;

	if(status == 0) {
		table_set_bid(game.me, game.bid[game.me]);
		statusbar_message("Your bid was accepted");
		game.state = LP_STATE_WAIT;
		table_set_bidder(game.me);
	} else if(status == LP_ERR_INVALID) {
		dlg_bid_display(hand.hand_size);
		statusbar_message("Invalid bid, please resubmit");
	}

	return status;
}


static int get_trump_status(void)
{
	char status;

	if(es_read_char(game.fd, &status) < 0)
		return -1;

	if(status == 0) {
		statusbar_message("Your trump was accepted");
		game.state = LP_STATE_WAIT;
		table_clear_table();
	} else if(status == LP_ERR_INVALID)
		statusbar_message("Invalid trump, this shouldn't happen");

	return status;
}


static int get_player_bid(void)
{
	char cnum;
	int num;
	char bid;
	char *t_str;

	if(es_read_char(game.fd, &cnum) < 0
	   || es_read_char(game.fd, &bid) < 0)
		return -1;
	num = cnum;

	game.bid[num] = bid;
	table_set_bid(num, bid);

	t_str = g_strdup_printf("%s bid %d", game.names[num], game.bid[num]);
	statusbar_message(t_str);
	g_free(t_str);

	table_set_bidder(cnum);

	return 0;
}


static int get_trump_suit(void)
{
	if(es_read_char(game.fd, &game.trump_suit) < 0)
		return -1;

	table_set_trump();
	return 0;
}


static int get_play_status(void)
{
	char status;
	char *msg;
	int card;

	if(es_read_char(game.fd, &status) < 0)
		return -1;

	if(status == 0) {
		statusbar_message("Waiting for next play");
		table_set_turn(game.me);
	} else {
		/* Restore the cards the way they should be */
		card = hand.in_play_card_num;
		hand.card[card] = hand.in_play_card_val;
		table_animation_abort();

		switch((int)status) {
			case LP_ERR_FOLLOW_SUIT:
				msg = "You must follow the led suit";
				game.state = LP_STATE_PLAY;
				break;
			case LP_ERR_MUST_TRUMP:
				msg = "You must play a trump card";
				game.state = LP_STATE_PLAY;
				break;
			case LP_ERR_TURN:
				msg = "Wait for your turn to play";
				game.state = LP_STATE_WAIT;
				break;
			case LP_ERR_INVALID:
			default:
				/* Should resynch at this point */
				/* but we return -1 below for now */
				game.state = LP_STATE_WAIT;
				msg = "Internal error, ack puke";
				break;
		}
		statusbar_message(msg);
	}

	if(status == LP_ERR_INVALID)
		return -1;

	return 0;
}


static int get_opponent_play(void)
{
	char p_num, card;

	if(es_read_char(game.fd, &p_num) < 0
	   || es_read_char(game.fd, &card) < 0)
		return -1;

	if(game.state == LP_STATE_ANIM)
		table_animation_zip(TRUE);

	table_animation_opponent(p_num, card);

	table_set_turn(p_num);

	return 0;
}


static int get_trick_winner(void)
{
	char p_num;
	char *t_str;

	if(game.state == LP_STATE_ANIM)
		table_animation_zip(TRUE);

	if(es_read_char(game.fd, &p_num) < 0)
		return -1;

	table_set_tricks(p_num, ++game.tricks[(int)p_num]);

	t_str = g_strdup_printf("%s won the trick", game.names[(int)p_num]);
	statusbar_message(t_str);
	g_free(t_str);

	table_clear_table();

	/* This will set next player, which causes an anomaly at     */
	/* the end of a hand, but one that won't keep me up at night */
	game.lead = p_num + 1;
	table_set_turn((p_num + 3) % 4);
	game.lead = p_num;

	return 0;
}


static int get_current_scores(void)
{
	int i;

	for(i=0; i<4; i++) {
		if(es_read_int(game.fd, &game.score[i]) < 0)
			return -1;
		table_set_score(i, game.score[i]);
	}

	return 0;
}


static int get_dealer(void)
{
	if(es_read_char(game.fd, &game.dealer) < 0)
		return -1;

	table_set_dealer();
	game.lead = (game.dealer + 1) % 4;

	return 0;
}
