/* 
 * File: messages.h
 * Author: Jason Short
 * Project: GGZCards Server
 * Date: 06/20/2001
 * Desc: Functions and data for messaging system
 * $Id: message.c 2647 2001-11-04 03:50:54Z jdorje $
 *
 * Right now GGZCards uses a super-generic "messaging" system.  Text
 * messages are generated by the server and sent to the client for
 * direct display.  This makes i18n basically impossible.  It also
 * makes really cool client displays basically impossible.  However,
 * it makes it really easy to add new games - you don't even have to
 * change the client!
 *
 * A more complicated alternate messaging system is in planning.
 *
 * Copyright (C) 2001 Brent Hendricks.
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-130
 */

#include <config.h>		/* Site-specific config */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <easysock.h>
#include "../../ggzdmod/ggz_stats.h"

#include "common.h"
#include "message.h"

/* NOTE ON "MESSAGES" - Each seat has a message.  The client should display
   this near to the player.  It may contain information such as bid, etc.  -
   There is a global message.  The client should display this prominently. It 
   may contain information such as contract, trump, etc.  - There are
   additional "tagged" global messages.  These are not implemented. */


/* 
 * PLAYER MESSAGES
 */

/* send_player_message() sends seat s's message to player p fails silently... 
 */
void send_player_message(seat_t s, player_t p)
{
	int fd = ggzd_get_player_socket(p);
	if (ggzd_get_seat_status(p) == GGZ_SEAT_BOT ||
	    game.seats[s].pmessage == NULL)
		return;
	ggzd_debug("Sending seat %d/%s's message to player %d/%s: %s", s,
		   get_seat_name(s), p, ggzd_get_player_name(p),
		   game.seats[s].pmessage);
	if (write_opcode(fd, WH_MESSAGE_PLAYER) < 0
	    || write_seat(fd, CONVERT_SEAT(s, p)) < 0
	    || es_write_string(fd, game.seats[s].pmessage) < 0)
		ggzd_debug("ERROR: failed to send player message.");
}

static void doput_player_message(seat_t s, char *msg)
{
	if (game.seats[s].pmessage != NULL)
		free(game.seats[s].pmessage);
	game.seats[s].pmessage = strdup(msg);
	if (game.seats[s].pmessage == NULL) {
		ggzd_debug("ERROR: " "bad strdup.");
		exit(-1);
	}
}

void clear_player_message(seat_t s)
{
	doput_player_message(s, "");
}

void put_player_message(seat_t s, char *fmt, ...)
{
	char buf[4096];

	va_list ap;
	va_start(ap, fmt);
	vsnprintf(buf, sizeof(buf), fmt, ap);
	va_end(ap);

	doput_player_message(s, buf);
}

void add_player_message(seat_t s, char *fmt, ...)
{
	va_list ap;
	char buf[4096] = "";
	int len;

	if (game.seats[s].pmessage != NULL)
		strcpy(buf, game.seats[s].pmessage);
	len = strlen(buf);

	va_start(ap, fmt);
	vsnprintf(buf + len, sizeof(buf) - len, fmt, ap);
	va_end(ap);

	doput_player_message(s, buf);
}

/* send_player_message_toall sends seat s's message to all players fails
   silently... */
void send_player_message_toall(seat_t s)
{
	player_t p;
	if (s < 0 || s >= game.num_seats)
		ggzd_debug("ERROR: SERVER BUG: "
			   "send_player_message_toall(%d) called.", s);
	ggzd_debug("Sending seat %d/%s's message to all.", s,
		   get_seat_name(s));
	for (p = 0; p < game.num_players; p++)
		send_player_message(s, p);
}

void set_player_message(player_t p)
{
	if (game.which_game == GGZ_GAME_UNKNOWN)
		/* silently fail; it's easier to check here than elsewhere */
		return;
	ggzd_debug("Setting player %d/%s's message.", p,
		   ggzd_get_player_name(p));
	if (p < 0 || p >= game.num_players)
		ggzd_debug("ERROR: SERVER BUG: "
			   "set_player_message(%d) called.", p);
	game.funcs->set_player_message(p);
	send_player_message_toall(game.players[p].seat);
}

void set_all_player_messages()
{
	player_t p;
	for (p = 0; p < game.num_players; p++)
		set_player_message(p);
}


/* 
 * GLOBAL MESSAGES
 */

/* send_global_message sends a global message to player p fails silently... */
void send_global_message(const char *mark, player_t p)
{
	send_global_text_message(p, mark, get_global_message(mark));
}

/* send_global_message_toall sends the truly global message to all players */
void send_global_message_toall(const char *mark)
{
	player_t p;
	const char *message = get_global_message(mark);
	for (p = 0; p < game.num_players; p++)
		send_global_text_message(p, mark, message);
}

void send_all_global_messages(player_t p)
{
	global_message_list_t *gml;

	for (gml = game.message_head; gml != NULL; gml = gml->next)
		send_global_text_message(p, gml->mark, gml->message);
}

const char *get_global_message(const char *mark)
{
	global_message_list_t *gml;

	for (gml = game.message_head; gml != NULL; gml = gml->next)
		if (!strcmp(mark, gml->mark))
			return gml->message;
	return "";
}

static void put_global_message(char *mark, char *msg)
{
	global_message_list_t *gml;
	ggzd_debug("Setting global message for '%s'.  Length is %d.", mark,
		   strlen(msg));

	if (!mark || !msg)
		ggzd_debug("ERROR: SERVER BUG: "
			   "put_global_message called on NULL string.");

	for (gml = game.message_head; gml != NULL; gml = gml->next) {
		if (!strcmp(mark, gml->mark)) {
			free(gml->message);
			gml->message = strdup(msg);
			if (gml->message == NULL) {
				ggzd_debug("ERROR: " "bad strdup.");
				exit(-1);
			}
		}
	}

	gml = alloc(sizeof(global_message_list_t));
	/* TODO: xstrdup */
	gml->mark = strdup(mark);
	gml->message = strdup(msg);
	if (game.message_tail)
		game.message_tail->next = gml;
	game.message_tail = gml;
	if (!game.message_head)
		game.message_head = gml;
}

/* set_global_message */
void set_global_message(char *mark, char *message, ...)
{
	char buf[4096];

	va_list ap;
	va_start(ap, message);
	vsnprintf(buf, sizeof(buf), message, ap);
	va_end(ap);

	put_global_message(mark, buf);
	send_global_message_toall(mark);
}


/* 
 * AUTOMATED PLAYER MESSAGES
 */

void add_player_score_message(player_t p)
{
	seat_t s = game.players[p].seat;
	add_player_message(s, "Score: %d\n", game.players[p].score);
}

void add_player_bid_message(player_t p)
{
	seat_t s = game.players[p].seat;
	if ((game.state == WH_STATE_NEXT_BID
	     || game.state == WH_STATE_WAIT_FOR_BID)
	    && game.players[p].bid_count > 0) {
		char bid_text[512];
		game.funcs->get_bid_text(bid_text, sizeof(bid_text),
					 game.players[p].bid);
		if (*bid_text)
			add_player_message(s, "Bid: %s\n", bid_text);
	}
}

void add_player_tricks_message(player_t p)
{
	seat_t s = game.players[p].seat;
	if (game.state == WH_STATE_WAIT_FOR_PLAY
	    || game.state == WH_STATE_NEXT_TRICK
	    || game.state == WH_STATE_NEXT_PLAY)
		add_player_message(s, "Tricks: %d\n", game.players[p].tricks);
}

void add_player_action_message(player_t p)
{
	seat_t s = game.players[p].seat;
	if (game.state == WH_STATE_WAIT_FOR_BID && p == game.next_bid)
		add_player_message(s, "Bidding...");
	if (game.state == WH_STATE_WAIT_FOR_PLAY && p == game.curr_play)
		add_player_message(s, "Playing...");
}

void add_player_rating_message(player_t p)
{
	seat_t s = game.players[p].seat;
	int rating = ggzd_get_rating(p);
	add_player_message(s, "Rating: %d\n", rating);
}


/* 
 * AUTOMATED GLOBAL MESSAGES
 */

static void send_last_hand_cardlist(void)
{
	int *lengths = alloc(game.num_seats * sizeof(*lengths));
	card_t **cardlist = alloc(game.num_seats * sizeof(*cardlist));
	hand_t *hand;
	seat_t s;

	for (s = 0; s < game.num_seats; s++) {
		hand = &game.seats[s].hand;
		hand->hand_size = hand->full_hand_size;
		cards_sort_hand(hand);
		lengths[s] = hand->full_hand_size;
		cardlist[s] = hand->cards;
	}


	broadcast_global_cardlist_message("Previous Hand List", lengths,
					  cardlist);

	free(lengths);
	free(cardlist);
}

void send_last_hand(void)
{
	int c, bsiz = 0, width = 0;
	char buf[4096];
	hand_t *hand;
	player_t p;
	seat_t s;

	if (!game.last_hand)
		return;

	for (p = 0; p < game.num_players; p++)
		if (strlen(ggzd_get_player_name(p)) > width)
			width = strlen(ggzd_get_player_name(p));

	for (s = 0; s < game.num_seats; s++) {
		hand = &game.seats[s].hand;
		hand->hand_size = hand->full_hand_size;
		cards_sort_hand(hand);
		bsiz += snprintf(buf + bsiz, sizeof(buf) - bsiz, "%*s: ",
				 width, get_seat_name(s));
		for (c = 0; c < hand->hand_size; c++) {
			card_t card = hand->cards[c];
			bsiz += snprintf(buf + bsiz, sizeof(buf) - bsiz,
					 "%2s%s ",
					 short_face_names[(int) card.face],
					 short_suit_names[(int) card.suit]);
		}
		bsiz += snprintf(buf + bsiz, sizeof(buf) - bsiz, "\n");
	}

	set_global_message("Previous Hand", "%s", buf);

	send_last_hand_cardlist();	/* experimental */
}

static void send_last_trick_cardlist(void)
{

	int *lengths = alloc(game.num_seats * sizeof(*lengths));
	card_t **cardlist = alloc(game.num_seats * sizeof(*cardlist));
	card_t *cardlist2 = alloc(game.num_seats * sizeof(*cardlist2));
	seat_t s;

	for (s = 0; s < game.num_seats; s++) {
		if (game.seats[s].player >= 0) {
			lengths[s] = 1;
			cardlist[s] = &cardlist2[s];
			cardlist[s][0] = game.seats[s].table;
		} else
			lengths[s] = 0;
	}


	broadcast_global_cardlist_message("Previous Trick List", lengths,
					  cardlist);

	free(lengths);
	free(cardlist);
	free(cardlist2);
}

void send_last_trick(void)
{
	/* this sets up a "last trick" message */
	player_t p, p_r;
	char message[512];
	int msg_len = 0, width = 0;
	card_t card;

	if (!game.last_trick)
		return;

	for (p = 0; p < game.num_players; p++)
		if (strlen(ggzd_get_player_name(p)) > width)
			width = strlen(ggzd_get_player_name(p));

	for (p_r = 0; p_r < game.num_players; p_r++) {
		p = (game.leader + p_r) % game.num_players;
		card = game.seats[game.players[p].seat].table;
		msg_len +=
			snprintf(message + msg_len, sizeof(message) - msg_len,
				 "%*s - %s of %s%s\n", width,
				 ggzd_get_player_name(p),
				 face_names[(int) card.face],
				 suit_names[(int) card.suit],
				 p == game.winner ? " (winner)" : "");
	}

	set_global_message("Last Trick", "%s", message);

	send_last_trick_cardlist();
}

static int **cumulative_scores = NULL;
static int c_score_count = 0;
static int c_score_size = 0;

static void send_cumulative_scores()
{
	char buf[4096];
	int buf_len = 0, r;
	player_t p;
	int widths[game.num_players];

	if (!game.cumulative_scores)
		return;

	for (p = 0; p < game.num_players; p++) {
		widths[p] = strlen(ggzd_get_player_name(p));
		if (widths[p] < 4)
			widths[p] = 4;
		buf_len +=
			snprintf(buf + buf_len, sizeof(buf) - buf_len,
				 "%*s%s", widths[p], ggzd_get_player_name(p),
				 p == game.num_players - 1 ? "" : " ");
	}
	buf_len += snprintf(buf + buf_len, sizeof(buf) - buf_len, "\n");

	for (r = 0; r < c_score_count; r++) {
		for (p = 0; p < game.num_players; p++) {
			/* this overcomplicated bit of hackery is intended to 
			   center the score under the name. Unfortunately, it 
			   assumes the number isn't longer than 3 characters. 
			 */
			int extra = widths[p] - 3;
			buf_len +=
				snprintf(buf + buf_len, sizeof(buf) - buf_len,
					 "%*s%3d%*s%s", extra / 2, "",
					 cumulative_scores[r][p],
					 (extra + 1) / 2, "",
					 p ==
					 game.num_players - 1 ? "" : " ");
		}
		buf_len +=
			snprintf(buf + buf_len, sizeof(buf) - buf_len, "\n");
	}

	set_global_message("Scores", "%s", buf);
}

void init_cumulative_scores()
{
	if (!game.cumulative_scores)
		return;
	if (cumulative_scores != NULL) {
		/* TODO: free data */
	}
	c_score_count = c_score_size = 0;

	send_cumulative_scores();
}

/* add on this hand's scores */
void update_cumulative_scores()
{
	player_t p;

	if (!game.cumulative_scores)
		return;
	c_score_count++;
	if (c_score_count > c_score_size) {
		c_score_size += 10;
		cumulative_scores =
			realloc(cumulative_scores,
				c_score_size * sizeof(int *));
		if (cumulative_scores == NULL) {
			ggzd_debug
				("ERROR: update_cumulative_scores: NULL realloc.");
			exit(-1);
		}
	}
	cumulative_scores[c_score_count - 1] =
		alloc(game.num_players * sizeof(int));

	for (p = 0; p < game.num_players; p++)
		cumulative_scores[c_score_count - 1][p] =
			game.players[p].score;

	send_cumulative_scores();
}

void send_bid_history()
{
	int r, buf_len = 0, widths[game.num_players];
	player_t p;
	char buf[4096];
	char buf2[512];

	if (!game.bid_history)
		return;

	for (p = 0; p < game.num_players; p++) {
		widths[p] = strlen(ggzd_get_player_name(p));
		/* TODO if (widths[p] < game.max_bid_length) widths[p] =
		   game.max_bid_length; */
		buf_len +=
			snprintf(buf + buf_len, sizeof(buf) - buf_len,
				 "%*s%s", widths[p], ggzd_get_player_name(p),
				 p == game.num_players - 1 ? "" : " ");
	}
	buf_len += snprintf(buf + buf_len, sizeof(buf) - buf_len, "\n");

	for (r = 0; r <= game.bid_rounds; r++) {
		for (p = 0; p < game.num_players; p++) {
			game.funcs->get_bid_text(buf2, sizeof(buf2),
						 game.players[p].allbids[r]);
			buf_len +=
				snprintf(buf + buf_len, sizeof(buf) - buf_len,
					 "%*s%s", widths[p], buf2,
					 p ==
					 game.num_players - 1 ? "" : " ");

		}
		buf_len +=
			snprintf(buf + buf_len, sizeof(buf) - buf_len, "\n");
	}

	set_global_message("Bid History", "%s", buf);
}
