/* 
 * File: game.c
 * Author: Rich Gade
 * Project: GGZCards Client
 * Date: 08/14/2000
 * Desc: Handles user-interaction with game screen
 * $Id: game.c 2941 2001-12-18 22:29:24Z jdorje $
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

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <ggz.h>
#include "common.h"

#include "dlg_bid.h"
#include "dlg_options.h"
#include "game.h"
#include "layout.h"
#include "main.h"
#include "table.h"

int table_max_hand_size = 0;


void game_init(void)
{
	ggz_debug("table", "Entering game_init().");
	statusbar_message(_("Waiting for server"));
}

void game_send_bid(int bid)
{
	int status = client_send_bid(bid);

	statusbar_message(_("Sending bid to server"));

	assert(status == 0);
}


void game_play_card(card_t card)
{
	int status;

	status = client_send_play(card);

	statusbar_message(_("Sending play to server"));

	assert(status == 0);
}

void game_handle_io(gpointer data, gint source, GdkInputCondition cond)
{
	client_handle_server();
}








void game_get_newgame(void)
{
	GtkWidget *menu =
		gtk_object_get_data(GTK_OBJECT(dlg_main), "mnu_startgame");
	gtk_widget_set_sensitive(menu, TRUE);

	statusbar_message(_("Select \"Start Game\" to begin the game."));
}

void game_alert_newgame(void)
{
	/* do nothing... */
}

void game_handle_gameover(int num_winners, int *winners)
{
	char msg[4096] = "";

	/* handle different cases */
	if (num_winners == 0)
		snprintf(msg, sizeof(msg), _("There was no winner"));
	else {
		int i;
		for (i = 0; i < num_winners; i++) {
			char *fmt;
			if (i == num_winners - 1)
				fmt = "%s ";
			else if (i == num_winners - 2)
				fmt = "%s and ";	/* not quite perfect
							   grammar... */
			else
				fmt = "%s, ";
			snprintf(msg + strlen(msg), sizeof(msg) - strlen(msg),
				 fmt, ggzcards.players[winners[i]].name);
		}
		snprintf(msg + strlen(msg), sizeof(msg) - strlen(msg),
			 _("won the game."));
	}

	/* This hack places this message in place of the global game message.
	   It prevents it from being overwritten by the upcoming newgame
	   request. */
	messagebar_message(msg);
}

void game_alert_player(int player, GGZSeatType status, const char *name)
{
	char *temp = NULL;
	switch (status) {
	case GGZ_SEAT_PLAYER:
		/* This assumes we can't have a smooth transition from one
		   human player to another.  Could be a problem... */
		if (ggzcards.players[player].status != GGZ_SEAT_PLAYER)
			temp = g_strdup_printf(_("%s joined the table"),
					       name);
		break;
	case GGZ_SEAT_OPEN:
		name = _("Empty Seat");
		if (ggzcards.players[player].status == GGZ_SEAT_PLAYER)
			temp = g_strdup_printf(_("%s left the table"),
					       ggzcards.players[player].name);
		break;
	default:
		/* any other handling? */
		break;
	}
	if (temp) {
		statusbar_message(temp);
		g_free(temp);
	}

	table_set_name(player, name);
}

void game_setup_table(void)
{
	table_setup();
}

void game_alert_hand_size(int max_hand_size)
{
	table_max_hand_size = max_hand_size;

	do {
		/* the inner table must be at least large enough. So, if it's
		   not we make the hand sizes larger. */
		int x, y, w, h, w1, h1;
		get_table_dim(&x, &y, &w, &h);
		get_fulltable_size(&w1, &h1);
		if (w1 > w && h1 > h)
			break;
		table_max_hand_size++;
	} while (1);
}

void game_display_hand(int player)
{
	table_display_hand(player);
}

void game_get_bid(int possible_bids, char **bid_choices)
{
	dlg_bid_display(possible_bids, bid_choices);

	/* This is a hack since sometimes the table would get overdrawn at
	   this point. */
	table_redraw();

	statusbar_message(_("Your turn to bid"));
}

void game_get_play(int hand)
{
#ifdef ANIMATION
	animation_zip(TRUE);
#endif /* ANIMATION */

	if (hand == 0)
		statusbar_message(_("Your turn to play a card"));
	else {
		char buf[100];
		snprintf(buf, sizeof(buf),
			 _("Your turn to play a card from %s's hand."),
			 ggzcards.players[hand].name);
		statusbar_message(buf);
	}
}

void game_alert_badplay(char *err_msg)
{
#ifdef ANIMATION
	animation_abort();
#endif /* ANIMATION */

	/* redraw cards */
	table_display_hand(ggzcards.play_hand);

	statusbar_message(err_msg);
	sleep(1);		/* just a delay? */
}

void game_alert_play(int player, card_t card, int pos)
{
#ifdef ANIMATION
	animation_zip(TRUE);
#endif /* ANIMATION */

	table_display_hand(player);
	table_play_card(player, card, pos);
}

void game_alert_table(void)
{
	ggz_debug("table", "Handling table update alert.");
	table_show_cards();
}

void game_alert_trick(int player)
{
	char *t_str;

#ifdef ANIMATION
	animation_zip(TRUE);
#endif /* ANIMATION */

	t_str = g_strdup_printf(_("%s won the trick"),
				ggzcards.players[player].name);
	statusbar_message(t_str);
	g_free(t_str);

	/* TODO: make this sleep optional (a preference) */
	sleep(1);

	table_clear_table();
}

int game_get_options(int option_cnt, int *choice_cnt, int *defaults,
		     char ***option_choices)
{
	dlg_option_display(option_cnt, choice_cnt, defaults, option_choices);

	statusbar_message(_("Please select options."));

	return 0;
}

/* Displays a global message (any global message). */
void game_set_text_message(const char *mark, const char *message)
{
	ggz_debug("table", "Received message '%s'/'%s'.", mark, message);
#if 0
	if (!table_initialized)
		return;
#endif
	if (!*mark)
		/* This is the "global" global message that has mark "". it
		   gets displayed on the messagebar. */
		messagebar_message(message);
	else if (!strcmp(mark, "game")) {
		/* This is the game's name; we just adjust the title bar */
		char title[50];
		snprintf(title, sizeof(title), _("GGZ Gaming Zone - %s"),
			 message);
		gtk_window_set_title(GTK_WINDOW(dlg_main), title);
	} else {
		/* Other messages get displayed in a special window with menu
		   access. */
		menubar_text_message(mark, message);
	}
	ggz_debug("table", "Handled message.");
}

void game_set_cardlist_message(const char *mark, int *lengths,
			       card_t ** cardlist)
{
#if 0
	int p, i;
	char buf[4096] = "";
	int maxlen = 0, namewidth = 0;

	/* FIXME: translations */
	char *suit_names[4] = { "clubs", "diamonds", "hearts", "spades" };
	char *short_suit_names[4] = { "C", "D", "H", "S" };
	char *face_names[15] =
		{ NULL, "ace", "two", "three", "four", "five", "six", "seven",
		"eight", "nine", "ten", "jack", "queen", "king", "ace"
	};
	char *short_face_names[15] =
		{ NULL, "A", "2", "3", "4", "5", "6", "7", "8", "9", "10",
		"J", "Q", "K", "A"
	};

	for (p = 0; p < ggzcards.num_players; p++) {
		if (lengths[p] > maxlen)
			maxlen = lengths[p];
		assert(ggzcards.players[p].name);
		if (strlen(ggzcards.players[p].name) > namewidth)
			namewidth = strlen(ggzcards.players[p].name);
	}

	assert(maxlen > 0);
	assert(namewidth > 0);

	for (p = 0; p < ggzcards.num_players; p++) {
		snprintf(buf + strlen(buf), sizeof(buf) - strlen(buf),
			 "%*s: ", namewidth, ggzcards.players[p].name);
		if (maxlen == 1) {
			if (lengths[p]) {
				card_t card = cardlist[p][0];
				snprintf(buf + strlen(buf),
					 sizeof(buf) - strlen(buf),
					 _("%s of %s"),
					 face_names[(int) card.face],
					 suit_names[(int) card.suit]);
			}
		} else {
			for (i = 0; i < lengths[p]; i++) {
				card_t card = cardlist[p][i];
				snprintf(buf + strlen(buf),
					 sizeof(buf) - strlen(buf), "%2s%s ",
					 short_face_names[(int) card.face],
					 short_suit_names[(int) card.suit]);
			}
		}
		snprintf(buf + strlen(buf), sizeof(buf) - strlen(buf), "\n");
	}

	game_set_text_message(mark, buf);
#else
	menubar_cardlist_message(mark, lengths, cardlist);
#endif
}

/* Displays a player's message on the table. */
void game_set_player_message(int player, const char *message)
{
	table_set_player_message(player, message);
}

int game_handle_game_message(int fd, int game, int size)
{
	return 0;
}
