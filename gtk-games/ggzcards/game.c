/* 
 * File: game.c
 * Author: Rich Gade, Jason Short
 * Project: GGZCards Client
 * Date: 08/14/2000
 * Desc: Handles user-interaction with game screen
 * $Id: game.c 8259 2006-06-23 06:53:15Z jdorje $
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
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <ggz.h>

#include "ggzintl.h"
#include "menus.h"

#include "client.h"

#include "ai.h"
#include "animation.h"
#include "cb_main.h"
#include "dlg_bid.h"
#include "dlg_main.h"
#include "dlg_options.h"
#include "dlg_players.h"
#include "drawcard.h"
#include "game.h"
#include "layout.h"
#include "main.h"
#include "table.h"

Preferences preferences;

int game_started = FALSE;

card_t table_cards[MAX_NUM_PLAYERS];

static void text_cardlist_message(const char *mark, int *lengths,
				  card_t ** cardlist);

void game_init(void)
{
	ggz_debug(DBG_MAIN, "Entering game_init().");
	statusbar_message(_("Waiting for server..."));
}

gboolean game_handle_ggz(GIOChannel * source, GIOCondition cond,
			 gpointer data)
{
	return (client_handle_ggz() >= 0);
}

gboolean game_handle_io(GIOChannel * source, GIOCondition cond,
			gpointer data)
{
	ggz_debug(DBG_MAIN, "Received data froms server.");
	if (client_handle_server() < 0) {
		gtk_main_quit();
	}
	return TRUE;
}

void game_send_bid(int bid)
{
	int status;

	ggz_debug(DBG_MAIN, "Sending bid %d to server.", bid);

	status = client_send_bid(bid);

	statusbar_message(_("Sending bid to server..."));

	assert(status == 0);
}

void game_send_options(int option_count, int *options_selection)
{
	int status;

	ggz_debug(DBG_MAIN, "Sending options to server.");

	status = client_send_options(option_count, options_selection);

	statusbar_message(_("Sending options to server..."));

	assert(status == 0);
}


/* Move the selected card out toward the playing area.  We don't actually put
   it *on* the table until we hear confirmation from the server that the play
   is valid. */
void game_play_card(int card_num)
{
	int player = ggzcards.play_hand, status;
	card_t card;

	if (preferences.collapse_hand)
		card = ggzcards.players[player].hand.cards[card_num];
	else {
		assert(ggzcards.players[player].u_hand[card_num].is_valid);
		card = ggzcards.players[player].u_hand[card_num].card;
	}

	assert(player >= 0 && player < ggzcards.num_players);

	ggz_debug(DBG_MAIN, "Sending play of card %d to server.",
		  card_num);
	statusbar_message(_("Sending play to server..."));

	status = client_send_play(card);

	/* Setup and trigger the card animation.  Because of the ugly way this 
	   is handled, this function has to be called _before_ we update
	   table_cards[player] (below). */
	if (preferences.animation)
		(void)animation_start(player, card, card_num, -1);

	/* We go ahead and move the card out onto the table, even though we
	   don't yet have validation that it's been played. */
	table_cards[player] = card;

	/* Draw the cards, eliminating the card in play */
	table_display_hand(player, TRUE);

	/* We don't remove the card from our hand until we have validation
	   that it's been played. Graphically, the table_card is skipped over
	   when drawing the hand. */

	assert(status == 0);
}

#ifdef DEBUG
static void game_play_card2(card_t card)
{
	int i, hand_size;
	int p = ggzcards.play_hand;

	if (preferences.collapse_hand)
		hand_size = ggzcards.players[p].hand.hand_size;
	else
		hand_size = ggzcards.players[p].u_hand_size;

	for (i = 0; i < hand_size; i++) {
		card_t hand_card;
		if (preferences.collapse_hand)
			hand_card = ggzcards.players[p].hand.cards[i];
		else {
			if (!ggzcards.players[p].u_hand[i].is_valid)
				continue;
			hand_card = ggzcards.players[p].u_hand[i].card;
		}
		if (are_cards_equal(card, hand_card)) {
			game_play_card(i);
			return;
		}
	}
	assert(0);
	game_play_card(0);
}
#endif /* DEBUG */

void game_send_newgame(void)
{
	ggz_debug(DBG_MAIN, "Sending newgame to server.");

	if (client_send_newgame() < 0)
		assert(FALSE);

	set_menu_sensitive(_("<main>/Game/Start game"), FALSE);

	statusbar_message(_("Waiting for the other players..."));
}

void game_resync(void)
{
	ggz_debug(DBG_MAIN, "Requesting sync from server.");

	animation_stop(TRUE);

	(void)client_send_sync_request();
}




void game_alert_server(int server_socket_fd)
{
	/* Start listening on the server socket.  We may stop later, if
	   necessary. */
	listen_for_server(TRUE);
}

void game_get_newgame(void)
{
	if (preferences.autostart) {
		(void)game_send_newgame();
	} else {
		set_menu_sensitive(_("<main>/Game/Start game"), TRUE);

		ggz_debug(DBG_MAIN,
			  "Handling newgame request from server.");

		statusbar_message(_("Select \"Start Game\" "
				    "to begin the game."));
	}
}

void game_alert_newgame(cardset_type_t cardset_type)
{
	int p;

	ggz_debug(DBG_MAIN, "Received newgame alert from server.");

#ifdef DEBUG
	if (preferences.use_ai)
		start_hand();
#endif /* DEBUG */

	/* Initialize table_cards to unknown. */
	for (p = 0; p < MAX_NUM_PLAYERS; p++)
		table_cards[p] = UNKNOWN_CARD;

	table_hide_player_list();

	load_card_data(cardset_type);

	game_started = TRUE;
	table_setup();
	/* do nothing... */
}

void game_alert_newhand(void)
{
#ifdef DEBUG
	if (preferences.use_ai)
		start_hand();
#endif
	/* nothing */
}

void game_handle_gameover(int num_winners, int *winners)
{
	char msg[1024] = "";

	ggz_debug(DBG_MAIN, "Handling gameover from server.");

	/* handle different cases */
	if (num_winners == 0)
		snprintf(msg, sizeof(msg), _("There was no winner."));
	else {
		int i;
		for (i = 0; i < num_winners; i++) {
			char *fmt;
			if (i == num_winners - 1)
				fmt = "%s ";
			else if (i == 0 && num_winners == 2)
				fmt = "%s and ";
			else if (i == num_winners - 2)
				fmt = "%s, and ";
			else
				fmt = "%s, ";
			snprintf(msg + strlen(msg),
				 sizeof(msg) - strlen(msg), fmt,
				 ggzcards.players[winners[i]].name);
		}
		snprintf(msg + strlen(msg), sizeof(msg) - strlen(msg),
			 _("won the game."));
	}

	table_show_player_list();

	/* This hack places this message in place of the global game message.
	   It prevents it from being overwritten by the upcoming newgame
	   request. */
	messagebar_message(msg);
}

void game_alert_player(int player,
		       GGZSeatType old_status, const char *old_name)
{
	char *message = NULL;
	GGZSeatType new_status = ggzcards.players[player].status;
	char *new_name = ggzcards.players[player].name;

	ggz_debug(DBG_MAIN, "Handling player update for player %d.",
		  player);

	switch (new_status) {
	case GGZ_SEAT_PLAYER:
		/* This assumes we can't have a smooth transition from one
		   human player to another.  Could be a problem... */
		if (old_status != GGZ_SEAT_PLAYER)
			message =
			    g_strdup_printf(_("%s joined the table."),
					    new_name);
		break;
	case GGZ_SEAT_OPEN:
		new_name = _("Empty Seat");
		if (old_status == GGZ_SEAT_PLAYER)
			message = g_strdup_printf(_("%s left the table."),
						  old_name);
		break;
	default:
		/* any other handling? */
		break;
	}
	if (message) {
		statusbar_message(message);
		g_free(message);
	}

	table_set_name(player, new_name);
}

void game_alert_num_players(int new, int old)
{
	/* We ignore new and old; ggzcards.num_players contains the new value
	   anyway. */
	if (game_started) {
		assert(new > 0);
		ggz_debug(DBG_MAIN, "Changing number of players.");
		table_setup();
	}
}

void game_alert_hand_size(int max_hand_size)
{
	ggz_debug(DBG_MAIN, "Table max hand size upped to %d.",
		  max_hand_size);

	set_max_hand_size(max_hand_size);

	table_setup();
}

void game_display_hand(int player)
{
	ggz_debug(DBG_MAIN, "Hand display for player %d needed.", player);
	table_display_hand(player, TRUE);
}

void game_get_bid(int possible_bids,
		  bid_t * bid_choices, char **bid_texts, char **bid_descs)
{
	ggz_debug(DBG_MAIN, "Handling bid request; %d choices.",
		  possible_bids);

	if (BID_ON_TABLE) {
		statusbar_message(_
				  ("It's your turn to bid.  Please choose "
				   "a bid from the selection above."));
	} else {
		statusbar_message(_
				  ("It's your turn to bid.  Please choose "
				   "a bid from the bid window."));
	}

	dlg_bid_destroy();

#ifdef DEBUG
	if (preferences.use_ai) {
		/* We ignore bid_texts and bid_descs */
		bid_t bid = get_bid(bid_choices, possible_bids);
		int i;

		for (i = 0; i < possible_bids; i++) {
			if (bid.bid == bid_choices[i].bid) {
				game_send_bid(i);
				return;
			}
		}

		assert(0);
		game_send_bid(rand() % possible_bids);
		return;
	}
#endif /* DEBUG */

	/* We ignore the bid_choices themselves. */
	dlg_bid_display(possible_bids, bid_texts, bid_descs);

#if 0
	/* This is a hack since sometimes the table would get overdrawn at
	   this point. */
	table_redraw();
#endif
}

void game_alert_bid(int bidder, bid_t bid)
{
#ifdef DEBUG
	if (preferences.use_ai)
		alert_bid(bidder, bid);
#endif /* DEBUG */

	/* otherwise nothing */
}

void game_get_play(int play_hand, int num_valid_cards,
		   card_t * valid_cards)
{
	ggz_debug(DBG_MAIN, "Handle play request.");

	if (play_hand == 0)
		statusbar_message(_("It's your turn to play a card."));
	else {
		char buf[100];
		snprintf(buf, sizeof(buf),
			 _
			 ("It's your turn to play a card (from %s's hand)."),
			 ggzcards.players[play_hand].name);
		statusbar_message(buf);
	}

#ifdef DEBUG
	if (preferences.use_ai) {
		int play_num, hand_num;
		hand_t *hand = &ggzcards.players[play_hand].hand;
		bool valid_plays[hand->hand_size];
		card_t play_card;

		assert(play_hand == ggzcards.play_hand);

		for (hand_num = 0; hand_num < hand->hand_size; hand_num++)
			valid_plays[hand_num] = FALSE;
		for (play_num = 0; play_num < num_valid_cards; play_num++) {
			card_t play_card = valid_cards[play_num];
			for (hand_num = 0;
			     hand_num < hand->hand_size; hand_num++) {
				card_t hand_card = hand->cards[hand_num];
				if (are_cards_equal(play_card, hand_card)) {
					valid_plays[hand_num] = TRUE;
					break;
				}
			}
			assert(hand_num < hand->hand_size);
		}

		play_card = get_play(ggzcards.play_hand, valid_plays);
		game_play_card2(play_card);
	}
#endif /* DEBUG */
}

void game_alert_badplay(char *err_msg)
{
	ggz_debug(DBG_MAIN, "Handling badplay alert.");

	/* We may have previously placed the card up; now we need to take it
	   back down. */
	assert(ggzcards.play_hand >= 0
	       && ggzcards.play_hand < ggzcards.num_players);
	table_cards[ggzcards.play_hand] = UNKNOWN_CARD;

	animation_stop(FALSE);

	/* When we first play the card, we'll move it out of the hand, so it's 
	   necessary to re-draw the hand with the card back in it if the play 
	   fails. */
	table_display_hand(ggzcards.play_hand, TRUE);

	/* When we first play the card, we also start animation, and if the
	   animation plays through to completion before the badplay message
	   comes the card will be left up on the table and will need to be
	   cleared off. */
	table_show_cards(TRUE);

	statusbar_message(err_msg);

#ifdef DEBUG
	if (preferences.use_ai) {
		assert(FALSE);
		client_send_sync_request();
	}
#endif /* DEBUG */
}

void game_alert_play(int player, card_t card, int pos, int hand_pos)
{
	ggz_debug(DBG_MAIN, "Handling play alert for player %d.", player);

	assert(player >= 0 && player < ggzcards.num_players);

#ifdef DEBUG
	if (preferences.use_ai)
		alert_play(player, card);
#endif /* DEBUG */

	if (preferences.animation) {
		/* If this is a card _we_ played, then we'll already be
		   animating, and we really don't want to stop just to start
		   over.  But we leave that up to animation_start. */
		if (!preferences.collapse_hand)
			pos = hand_pos;
		(void)animation_start(player, card, pos, -1);
	}

	/* This probably isn't necessary at this point, but it's consistent to 
	   keep it current. */
	table_cards[player] = card;

	if (!preferences.animation) {
		/* We only show the card on the table if we're not animating - 
		   if we're animating then we wait for it to get there
		   naturally. */
		table_show_card(player, card, TRUE);
	}

	/* if (pref_animation) */
	/* Note, even for cards we played we don't actually remove the card
	   from our hand until we hear confirmation.  So we need to redraw the 
	   hand in any case. */
	table_display_hand(player, TRUE);
}

void game_alert_table(void)
{
	int p;
	for (p = 0; p < ggzcards.num_players; p++)
		table_cards[p] = ggzcards.players[p].table_card;

	if (game_started) {
		ggz_debug(DBG_MAIN, "Handling table update alert.");
		table_show_cards(TRUE);
	}
}

void game_alert_trick(int winner)
{
	char *t_str;

	ggz_debug(DBG_MAIN, "Handling trick alert; player %d won.",
		  winner);

#ifdef DEBUG
	if (preferences.use_ai)
		alert_trick(winner);
#endif /* DEBUG */

	/* This is a bit of a hack - the code to move cards off the table is a 
	   bit mixed between animating and non-animating versions, since we
	   want to wait 1 second before doing so.  My solution is to have the
	   animation code take care of all of it, whether or not use_animation
	   is actually set.  But maybe this makes sense, if we consider this
	   delay to be animation even when !use_animation. Hmm. */
	animate_cards_off_table(winner);

	t_str = g_strdup_printf(_("%s won the trick"),
				ggzcards.players[winner].name);
	statusbar_message(t_str);
	g_free(t_str);
}

int game_get_options(int option_cnt,
		     char **types,
		     char **descriptions,
		     int *choice_cnt, int *defaults,
		     char ***option_choices)
{
	ggz_debug(DBG_MAIN, "Handling option request.");

	dlg_options_destroy();

	if (preferences.use_default_options) {
		return -1;
	}

	dlg_option_display(option_cnt, types, descriptions,
			   choice_cnt, defaults, option_choices);

	statusbar_message(_("Please select the game's options."));

	return 0;
}

/* Displays a global message (any global message). */
void game_set_text_message(const char *mark, const char *message)
{
	ggz_debug(DBG_MAIN, "Received text message for '%s'.", mark);
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
					 get_face_name(card.face),
					 get_suit_name(card.suit));
			}
		} else {
			for (i = 0; i < lengths[p]; i++) {
				card_t card = cardlist[p][i];
				snprintf(buf + strlen(buf),
					 sizeof(buf) - strlen(buf),
					 "%2s%s ",
					 get_short_face_name(card.face),
					 get_short_suit_name(card.suit));
			}
		}
		snprintf(buf + strlen(buf), sizeof(buf) - strlen(buf),
			 "\n");
	}

	game_set_text_message(mark, buf);
}

void game_set_cardlist_message(const char *mark, int *lengths,
			       card_t ** cardlist)
{
	static int use_cardlists = -1;

	ggz_debug(DBG_MAIN, "Received cardlist message for '%s'.", mark);

	/* We can't change the cardlists from graphical to text once they've
	   been created, so instead we just don't have the preference take
	   effect until the _first_ cardlist appears. The user should still be 
	   able to change the pref after this point, but it won't take effect
	   until they restart. */
	if (use_cardlists == -1)
		use_cardlists = preferences.cardlists;

	if (use_cardlists) {
		/* if pref_cardlists is set, then we make a graphical cardlist 
		   popup dialog. */
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
	ggz_debug(DBG_MAIN, "Received player message for %d.", player);
	table_set_player_message(player, message);
}

int game_handle_game_message(int fd, const char *game, int size)
{
	ggz_debug(DBG_MAIN, "Received game message for game %s.", game);
	return 0;
}
