/* 
 * File: game.c
 * Author: Rich Gade, Jason Short
 * Project: GGZCards Client
 * Date: 08/14/2000
 * Desc: Handles user-interaction with game screen
 * $Id: game.c 3400 2002-02-17 13:10:57Z jdorje $
 *
 * Copyright (C) 2000-2002 Brent Hendricks.
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

#include "animation.h"
#include "dlg_bid.h"
#include "dlg_options.h"
#include "game.h"
#include "layout.h"
#include "main.h"
#include "table.h"

Preferences preferences;

int table_max_hand_size = 0;
int game_started = FALSE;

card_t table_cards[MAX_NUM_PLAYERS];

static void text_cardlist_message(const char *mark, int *lengths,
				  card_t ** cardlist);

void game_init(void)
{
	ggz_debug("main", "Entering game_init().");
	statusbar_message(_("Waiting for server"));
}

void game_handle_io(gpointer data, gint source, GdkInputCondition cond)
{
	ggz_debug("main", "Received data froms server.");
	client_handle_server();
}

void game_send_bid(int bid)
{
	int status;

	ggz_debug("main", "Sending bid %d to server.", bid);

	status = client_send_bid(bid);

	statusbar_message(_("Sending bid to server"));

	assert(status == 0);
}

void game_send_options(int option_count, int *options_selection)
{
	int status;

	ggz_debug("main", "Sending options to server.");

	status = client_send_options(option_count, options_selection);

	statusbar_message(_("Sending options to server"));

	assert(status == 0);
}


/* Move the selected card out toward the playing area.  We don't actually put
   it *on* the table until we hear confirmation from the server that the play
   is valid. */
void game_play_card(int card_num)
{
	int player = ggzcards.play_hand, status;
	card_t card = ggzcards.players[player].hand.card[card_num];
	
	assert(player >= 0 && player < ggzcards.num_players);

	ggz_debug("main", "Sending play of card %d to server.", card_num);
	statusbar_message(_("Sending play to server"));

	status = client_send_play(card);

	/* Setup and trigger the card animation.  Because of the ugly way
	   this is handled, this function has to be called _before_ we
	   update table_cards[player] (below). */
	if (preferences.animation)
		(void) animation_start(player, card, card_num, -1);

	/* We go ahead and move the card out onto the table, even though
	   we don't yet have validation that it's been played. */
	table_cards[player] = card;

	/* Draw the cards, eliminating the card in play */
	table_display_hand(player, TRUE);

	/* We don't remove the card from our hand until we have validation
	   that it's been played. Graphically, the table_card is skipped over 
	   when drawing the hand. */

	assert(status == 0);
}

void game_send_newgame(void)
{
	GtkWidget *menu;
	int status;

	ggz_debug("main", "Sending newgame to server.");

	status = client_send_newgame();

	menu = gtk_object_get_data(GTK_OBJECT(dlg_main), "mnu_startgame");
	assert(menu);
	gtk_widget_set_sensitive(menu, FALSE);

	statusbar_message(_("Waiting for the other players..."));

	assert(status == 0);
}

void game_request_sync(void)
{
	ggz_debug("main", "Requesting sync from server.");

	animation_stop(TRUE);

	(void) client_send_sync_request();
}






void game_get_newgame(void)
{
	if (preferences.autostart) {
		(void) game_send_newgame();
	} else {
		GtkWidget *menu = gtk_object_get_data(GTK_OBJECT(dlg_main),
						      "mnu_startgame");
		gtk_widget_set_sensitive(menu, TRUE);

		ggz_debug("main", "Handling newgame request from server.");

		statusbar_message(_
				  ("Select \"Start Game\" "
				   "to begin the game."));
	}
}

void game_alert_newgame(void)
{
	int p;
	
	ggz_debug("main", "Received newgame alert from server.");
	
	/* Initialize table_cards to unknown. */
	for (p = 0; p < MAX_NUM_PLAYERS; p++)
		table_cards[p] = UNKNOWN_CARD;
		
	game_started = TRUE;
	table_setup();
	/* do nothing... */
}

void game_handle_gameover(int num_winners, int *winners)
{
	char msg[4096] = "";

	ggz_debug("main", "Handling gameover from server.");

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

	ggz_debug("main", "Handling player update for player %d.", player);

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

void game_alert_num_players(int new, int old)
{
	/* We ignore new and old; ggzcards.num_players contains the new value 
	   anyway. */
	if (game_started) {
		assert(new > 0);
		ggz_debug("main", "Changing number of players.");
		table_setup();
	}
}

void game_alert_hand_size(int max_hand_size)
{
	ggz_debug("main", "Table max hand size upped to %d.", max_hand_size);

	table_max_hand_size = max_hand_size;

	do {
		/* the inner table must be at least large enough. So, if it's
		   not we make the hand sizes larger. */
		/* NOTE: get_table_dim/get_fulltable_size depends on
		   table_max_hand_size, so we must increment it directly in
		   this loop. */
		int x, y, w, h, w1, h1, d;
		get_table_dim(&x, &y, &w, &h);
		get_fulltable_dim(&d, &d, &w1, &h1);
		if (w1 > w && h1 > h)
			break;
		table_max_hand_size++;
	} while (1);

	table_setup();
}

void game_display_hand(int player)
{
	ggz_debug("main", "Hand display for player %d needed.", player);

	table_display_hand(player, TRUE);
}

void game_get_bid(int possible_bids, char **bid_choices)
{
	ggz_debug("main", "Handling bid request; %d choices.", possible_bids);

	dlg_bid_display(possible_bids, bid_choices);

#if 0
	/* This is a hack since sometimes the table would get overdrawn at
	   this point. */
	table_redraw();
#endif

	statusbar_message(_("Your turn to bid"));
}

void game_get_play(int hand)
{
	ggz_debug("main", "Handle play request.");

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
	ggz_debug("main", "Handling badplay alert.");
	
	/* We may have previously placed the card up; now we need
	   to take it back down. */
	assert(ggzcards.play_hand >= 0
	       && ggzcards.play_hand < ggzcards.num_players);
	table_cards[ggzcards.play_hand] = UNKNOWN_CARD;

	animation_stop(FALSE);

	/* When we first play the card, we'll move it out of the hand, so
	   it's necessary to re-draw the hand with the card back in it if
	   the play fails. */
	table_display_hand(ggzcards.play_hand, TRUE);
	
	/* When we first play the card, we also start animation, and if
	   the animation plays through to completion before the badplay
	   message comes the card will be left up on the table and will
	   need to be cleared off. */
	table_show_cards(TRUE);

	statusbar_message(err_msg);
}

void game_alert_play(int player, card_t card, int pos)
{
	ggz_debug("main", "Handling play alert for player %d.", player);
	
	assert(player >= 0 && player < ggzcards.num_players);

	if (preferences.animation) {
		/* If this is a card _we_ played, then we'll already be
		   animating, and we really don't want to stop just to start
		   over.  But we leave that up to animation_start. */
		(void) animation_start(player, card, pos, -1);
	}
	
	/* This probably isn't necessary at this point, but it's consistent
	   to keep it current. */
	table_cards[player] = card;
	
	if (!preferences.animation) {
		/* We only show the card on the table if we're not animating
		   - if we're animating then we wait for it to get there
		   naturally. */
		table_show_card(player, card, TRUE);
	}			/* if (pref_animation) */

	/* Note, even for cards we played we don't actually remove the card
	   from our hand until we hear confirmation.  So we need to redraw
	   the hand in any case. */
	table_display_hand(player, TRUE);
}

void game_alert_table(void)
{
	int p;
	for (p = 0; p < ggzcards.num_players; p++)
		table_cards[p] = ggzcards.players[p].table_card;
		
	if (game_started) {
		ggz_debug("main", "Handling table update alert.");
		table_show_cards(TRUE);
	}
}

void game_alert_trick(int winner)
{
	char *t_str;

	ggz_debug("main", "Handling trick alert; player %d won.", winner);
	
	/* This is a bit of a hack - the code to move cards off the table
	   is a bit mixed between animating and non-animating versions,
	   since we want to wait 1 second before doing so.  My solution
	   is to have the animation code take care of all of it, whether
	   or not use_animation is actually set.  But maybe this makes
	   sense, if we consider this delay to be animation even when
	   !use_animation.  Hmm. */
	animate_cards_off_table(winner);

	t_str = g_strdup_printf(_("%s won the trick"),
				ggzcards.players[winner].name);
	statusbar_message(t_str);
	g_free(t_str);
}

int game_get_options(int option_cnt, int *choice_cnt, int *defaults,
		     char ***option_choices)
{
	ggz_debug("main", "Handling option request.");

	if (preferences.use_default_options) {
		return -1;
	}

	dlg_option_display(option_cnt, choice_cnt, defaults, option_choices);

	statusbar_message(_("Please select options."));

	return 0;
}

/* Displays a global message (any global message). */
void game_set_text_message(const char *mark, const char *message)
{
	ggz_debug("main", "Received text message for '%s'.", mark);
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
}

static void text_cardlist_message(const char *mark, int *lengths,
				  card_t ** cardlist)
{
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
}

void game_set_cardlist_message(const char *mark, int *lengths,
			       card_t ** cardlist)
{
	static int use_cardlists = -1;
	
	ggz_debug("main", "Received cardlist message for '%s'.", mark);
	
	/* We can't change the cardlists from graphical to text once
	   they've been created, so instead we just don't have the
	   preference take effect until the _first_ cardlist appears.
	   The user should still be able to change the pref after this
	   point, but it won't take effect until they restart. */
	if (use_cardlists == -1)
		use_cardlists = preferences.cardlists;
	
	if (use_cardlists) {
		/* if pref_cardlists is set, then we make a graphical
		   cardlist popup dialog. */
		menubar_cardlist_message(mark, lengths, cardlist);
	} else {
		/* if pref_cardlists is _not_ set, then we translate the
		   cardlist message into a text list. */
		text_cardlist_message(mark, lengths, cardlist);
	}
}

/* Displays a player's message on the table. */
void game_set_player_message(int player, const char *message)
{
	ggz_debug("main", "Received player message for %d.", player);
	table_set_player_message(player, message);
}

int game_handle_game_message(int fd, int game, int size)
{
	ggz_debug("main", "Received game message for game %d.", game);
	return 0;
}
