/*
 * File: main.c
 * Author: Rich Gade
 * Project: GGZCards Client
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

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
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
static int get_players(void);
static int get_gameover_status(void);
static void handle_badplay(void);
static int handle_play(void);
static int handle_msg_table(void);
static int get_trick_winner(void);


int main(int argc, char *argv[])
{

	game.num_players = 4; /* TODO: temporary measure; eventually this should be 0 */
	ggz_debug("Launching.");

	gtk_init(&argc, &argv);
	ggz_debug("gtk_init completed.");

	ggz_connect();
	ggz_debug("ggz_connect completed.");
	gdk_input_add(game.fd, GDK_INPUT_READ, game_handle_io, NULL);
	ggz_debug("ggz_input_add completed.");

	dlg_main = create_dlg_main();
	ggz_debug("dlg_main completed.");

	gtk_widget_show(dlg_main);
	ggz_debug("gtk_widget_show completed.");

	table_initialize();
	ggz_debug("table_initialize completed.");
	game_init();
	ggz_debug("game_init completed.");

	gtk_main();
	ggz_debug("Exiting normally.");
	return 0;
}


static void ggz_connect(void)
{
	char fd_name[30];
	struct sockaddr_un addr;

	/* Connect to Unix domain socket */
	ggz_snprintf(fd_name, sizeof(fd_name), "/tmp/GGZCards.%d", getpid());

	if((game.fd = socket(PF_LOCAL, SOCK_STREAM, 0)) < 0) {
		ggz_debug("ggz_connect(): no socket.  exit(-1).");
		exit(-1);
	}

	bzero(&addr, sizeof(addr));
	addr.sun_family = AF_LOCAL;
	strcpy(addr.sun_path, fd_name);

	if(connect(game.fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		ggz_debug("ggz_connect: no connection.  exit(-1).");
		exit(-1);
	}
}

static int handle_message_global()
{
 	char mark[100];
	char message[100];

	if (es_read_string(game.fd, mark, sizeof(mark)) < 0)
		return -1;
	if (es_read_string(game.fd, message, sizeof(message)) < 0)
		return -1;

	ggz_debug("     Global message received, marked as '%s':%s", mark, message);
	/* TODO: show in interface */

	table_set_message(mark, message);

	return 0;
}

static int handle_message_player()
{
	int player;
	char message[100];
	if (es_read_int(game.fd, &player) < 0)
		return -1;
	if (es_read_string(game.fd, message, sizeof(message)) < 0)
		return -1;

	table_set_player_message(player, message);

	return 0;
}

static int handle_req_play()
{
	if (es_read_int(game.fd, &game.play_hand) < 0)
		return -1;

#ifdef ANIMATION
	if(game.state == WH_STATE_ANIM)
		table_animation_zip(TRUE);
#endif /* ANIMATION */

	set_game_state( WH_STATE_PLAY );
	if (game.play_hand == 0)
		statusbar_message( _("Your turn to play a card") );
	else {
		char buf[100];
		ggz_snprintf(buf, sizeof(buf), _("Your turn to play a card from %s's hand."), game.players[game.play_hand].name);
		statusbar_message( buf );
	}
	return 0;	
}

char *opstr[] = { "WH_REQ_NEWGAME", "WH_MSG_NEWGAME",  "WH_MSG_GAMEOVER",
		  "WH_MSG_PLAYERS", "WH_MSG_HAND",
		  "WH_REQ_BID",	    "WH_REQ_PLAY",     "WH_MSG_BADPLAY",
		  "WH_MSG_PLAY",    "WH_MSG_TRICK",    "WH_MESSAGE_GLOBAL",
		  "WH_MESSAGE_PLAYER", "WH_REQ_OPTIONS", "WH_MSG_TABLE" };

static void game_handle_io(gpointer data, gint source, GdkInputCondition cond)
{
	int op, status;

	if(es_read_int(game.fd, &op) < 0) {
		/* FIXME: do something here... */
		return;
	}

	status = 0;

	if (op >= WH_REQ_NEWGAME && op <= WH_REQ_OPTIONS)
		ggz_debug("Received opcode: %s", opstr[op]);
	else
		ggz_debug("Received opcode %d", op);

	switch(op) {
		case WH_REQ_NEWGAME:
			/* TODO: ask player about this */
			sleep(1);
			status = es_write_int(game.fd, WH_RSP_NEWGAME);
			ggz_debug("     Handled WH_REQ_NEWGAME: status is %d.", status);
			break;
		case WH_MSG_NEWGAME:
			/* TODO: don't make "new game" until here */
			break;
		case WH_MSG_GAMEOVER:
			status = get_gameover_status();
			break;
		case WH_MSG_PLAYERS:
			status = get_players();
			break;
		case WH_MSG_HAND:
			status = hand_read_hand();
			break;
		case WH_REQ_BID:
			if (handle_bid_request() < 0)
				ggz_debug("Error or bug: -1 returned by handle_bid_request."); /* TODO */
			break;
		case WH_REQ_PLAY:
			status = handle_req_play();
			break;
		case WH_MSG_BADPLAY:
			handle_badplay();
			break;
		case WH_MSG_PLAY:
			status = handle_play();
			break;
		case WH_MSG_TABLE:
			status = handle_msg_table();
			break;
		case WH_MSG_TRICK:
			status = get_trick_winner();
			break;
		case WH_MESSAGE_GLOBAL:
			status = handle_message_global();
			break;
		case WH_MESSAGE_PLAYER:
			status = handle_message_player();
			break;
		case WH_REQ_OPTIONS:
			status = handle_option_request();
			break;
		default:
			ggz_debug("SERVER/CLIENT bug: unknown opcode received %d", op);
			status = -1;
			break;
	}

	if(status == -1) {
		ggz_debug("Lost connection to server?!");
		close(game.fd);
		exit(-1);
	}
}


static void game_init(void)
{
	ggz_debug("Entering game_init().");
	statusbar_message( _("Waiting for server") );
	game.state = WH_STATE_INIT;
}


static int get_players(void)
{
	int i, left=0;
	char *temp;
	char t_name[17];
	int numplayers;
	static int initted = 0; /* TODO: temporary measure */

	if (es_read_int(game.fd, &numplayers) < 0)
		return -1;

	/* TODO: support for changing the number of players */

	/* we may need to allocate memory for the players */
	if (!initted || game.num_players != numplayers) {
		/* TODO: free if necessary */
		initted = 1;
		ggz_debug("get_players: (re)allocating game.players.");
  		game.players = (struct seat_t *)g_malloc(numplayers * sizeof(struct seat_t));
		bzero(game.players, numplayers * sizeof(struct seat_t));
	}

	for(i = 0; i < numplayers; i++) {
		if (es_read_int(game.fd, &game.players[i].seat) < 0)
			return -1;
		if (es_read_string(game.fd, (char*)&t_name, 17) < 0)
			return -1;
		if (i != 0 && game.num_players
				&& strcmp(t_name, game.players[i].name)) {
			temp = g_strdup_printf( _("%s joined the table"), t_name);
			statusbar_message(temp);
			g_free(temp);
		}
		strcpy(game.players[i].name, t_name);
		table_set_name(i, game.players[i].name);
		/* TODO: check for leaving the table, etc. */
	}

	game.num_players = numplayers;

	if(left && game.state == WH_STATE_BID) {
		/* TODO: cancel bid (I think????) */
	}
	if(left)
		set_game_state( WH_STATE_WAIT );

	return 0;
}


static int get_gameover_status(void)
{
	int num_winners;
	int winner;
	char msg[100] = "";
	int mlen = 0;

	if (es_read_int(game.fd, &num_winners) < 0)
		return -1;

	/* handle different cases */	
	if (num_winners == 0)
		ggz_snprintf(msg, sizeof(msg), _("There was no winner") );
	else {
   	for(; num_winners; num_winners--) {
			if (es_read_int(game.fd, &winner) < 0)
				return -1;
			/* TODO: better grammar */
			mlen += ggz_snprintf(msg+mlen, 100-mlen, "%s ", game.players[winner].name);		
		}
		ggz_snprintf(msg+mlen, 100-mlen, _("won the game") );
	}
	
	statusbar_message(msg);
	set_game_state( WH_STATE_DONE );

	return 0;
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
	ggz_debug("     Put up statusbar message: '%s'", msg);
}

static void handle_badplay(void)
{
	char err_msg[100];
	int p = game.play_hand;

	if(es_read_string(game.fd, err_msg, sizeof(err_msg)) < 0)
		ggz_snprintf(err_msg, sizeof(err_msg), _("Bad play: unknown reason.") );

	/* Restore the cards the way they should be */
	game.players[p].table_card = UNKNOWN_CARD;
#ifdef ANIMATION
	table_animation_abort();
#endif /* ANIMATION */

	/* redraw cards */
	table_display_hand(p);

	set_game_state( WH_STATE_PLAY );
	
	statusbar_message(err_msg);
	sleep(1); /* just a delay? */
}


static int handle_play(void)
{
	int p, c, tc;
	card_t card;
	struct hand_t *hand;

	/* TODO: handle plays by self */

	if(es_read_int(game.fd, &p) < 0
	   || es_read_card(game.fd, &card) < 0)
		return -1;

	ggz_debug("     Received play from player %d: %i %i %i.", p, card.face, card.suit, card.deck);

#ifdef ANIMATION
	if(game.state == WH_STATE_ANIM)
		table_animation_zip(TRUE);
#endif /* ANIMATION */

	/* remove the card from the hand */
	hand = &game.players[p].hand;

	/* first, find a matching card to remove.
	 * Anything "unknown" will match, as will teh card itself*/
	for (tc=hand->hand_size-1; tc>=0; tc--) {
		card_t hcard = hand->card[tc];
		if (hcard.deck != -1 && hcard.deck != card.deck) continue;
		if (hcard.suit != -1 && hcard.suit != card.suit) continue;
		if (hcard.face != -1 && hcard.face != card.face) continue;
		break;
	}
	if (tc == -1) {
		ggz_debug("SERVER/CLIENT BUG: unknown card played.  Hand is:");
		for (tc = 0; tc < hand->hand_size; tc++) {
			card_t hcard = hand->card[tc];
			ggz_debug("     Card %d is (%d %d %d).", tc, hcard.face, hcard.suit, hcard.deck);
		}
		return -1;
	}

	/* now, remove the card */
	for (c=tc; c<hand->hand_size; c++)
		hand->card[c] = hand->card[c+1];
	hand->card[hand->hand_size] = UNKNOWN_CARD;
	hand->hand_size--;

	/* now update the graphics */
	table_display_hand(p);
	table_play_card(p, card);

	return 0;
}

static int handle_msg_table(void)
{
	int p, status=0;

	for (p=0; p<game.num_players; p++)
		if (es_read_card(game.fd, &game.players[p].table_card) < 0)
			status = -1;

	/* TODO: verify that the table cards have been removed from the hands */

	table_show_cards();
}



/* get_trick_winner
 *   handles the end of a trick
 */
static int get_trick_winner(void)
{
	int p_num;
	char *t_str;

#ifdef ANIMATION
	if(game.state == WH_STATE_ANIM)
		table_animation_zip(TRUE);
#endif /* ANIMATION */

	if(es_read_int(game.fd, &p_num) < 0)
		return -1;

	t_str = g_strdup_printf(_("%s won the trick"), game.players[(int)p_num].name);
	statusbar_message(t_str);
	g_free(t_str);

	table_clear_table();

	return 0;
}




