/* 
 * File: messages.h
 * Author: Jason Short
 * Project: GGZCards Server
 * Date: 06/20/2001
 * Desc: Functions and data for messaging system
 * $Id: message.c 3570 2002-03-16 06:36:32Z jdorje $
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
 * Copyright (C) 2001-2002 Brent Hendricks.
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

#ifdef HAVE_CONFIG_H
#  include <config.h>			/* Site-specific config */
#endif

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ggz.h>

#include "common.h"
#include "net.h"

#ifdef USE_GGZ_STATS		/* defined in common.h */
# include "../../ggzdmod/ggz_stats.h"
#endif /* USE_GGZ_STATS */

#include "message.h"

struct global_message_list_t {
	char *mark;
	char *message;
	struct global_message_list_t *next;
};

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
	int fd = get_player_socket(p);
	if (game.seats[s].pmessage == NULL)
		return;
	ggzdmod_log(game.ggz,
		    "Sending seat %d/%s's message to player %d/%s: %s", s,
		    get_seat_name(s), p, get_player_name(p),
		    game.seats[s].pmessage);
	if (write_opcode(fd, MESSAGE_GAME) < 0 ||
	    write_opcode(fd, GAME_MESSAGE_PLAYER) < 0 ||
	    write_seat(fd, CONVERT_SEAT(s, p)) < 0 ||
	    ggz_write_string(fd, game.seats[s].pmessage) < 0)
		ggzdmod_log(game.ggz,
			    "ERROR: failed to send player message.");
}

static void doput_player_message(seat_t s, char *msg)
{
	if (game.seats[s].pmessage != NULL)
		ggz_free(game.seats[s].pmessage);
	game.seats[s].pmessage = ggz_strdup(msg);
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
		ggzdmod_log(game.ggz, "ERROR: SERVER BUG: "
			    "send_player_message_toall(%d) called.", s);
	ggzdmod_log(game.ggz, "Sending seat %d/%s's message to all.", s,
		    get_seat_name(s));
	for (p = 0; p < game.num_players; p++)
		send_player_message(s, p);
}

void set_player_message(player_t p)
{
	if (game.which_game == NULL) {
		/* silently fail; it's easier to check here than elsewhere */
		return;
	}

	ggzdmod_log(game.ggz, "Setting player %d/%s's message.", p,
		    get_player_name(p));
	if (p < 0 || p >= game.num_players)
		ggzdmod_log(game.ggz, "ERROR: SERVER BUG: "
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
	(void) send_global_text_message(p, mark, get_global_message(mark));
}

void send_all_global_messages(player_t p)
{
	global_message_list_t *gml;

	for (gml = game.message_head; gml != NULL; gml = gml->next)
		(void) send_global_text_message(p, gml->mark, gml->message);
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
	ggzdmod_log(game.ggz,
		    "Setting global message for '%s'.  Length is %d.", mark,
		    strlen(msg));

	if (!mark || !msg)
		ggzdmod_log(game.ggz, "ERROR: SERVER BUG: "
			    "put_global_message called on NULL string.");

	for (gml = game.message_head; gml != NULL; gml = gml->next) {
		if (!strcmp(mark, gml->mark)) {
			ggz_free(gml->message);
			gml->message = ggz_strdup(msg);
		}
	}

	gml = ggz_malloc(sizeof(global_message_list_t));
	gml->mark = ggz_strdup(mark);
	gml->message = ggz_strdup(msg);
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
	
	broadcast_global_text_message(mark, buf);
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
	if (game.players[p].bid_count > 0) {
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
	
	if (game.state != STATE_WAIT_FOR_PLAY
	    && game.state != STATE_NEXT_TRICK
	    && game.state != STATE_NEXT_PLAY)
		return;
	
	if (game.players[p].team >= 0) {
		int tricks = 0;
		player_t p2;
		
		for (p2 = 0; p2 < game.num_players; p2++)
			if (game.players[p2].team == game.players[p].team)
				tricks += game.players[p2].tricks;
				
		add_player_message(s, "Tricks: %d (%d)\n", game.players[p].tricks, tricks);
	} else {
		add_player_message(s, "Tricks: %d\n", game.players[p].tricks);
	}
}

void add_player_action_message(player_t p)
{
	seat_t s = game.players[p].seat;
	if (game.players[p].bid_data.is_bidding)
		add_player_message(s, "Bidding...");
	if (game.state == STATE_WAIT_FOR_PLAY && game.players[p].is_playing)
		add_player_message(s, "Playing...");
}

void add_player_rating_message(player_t p)
{
#ifdef USE_GGZ_STATS		/* defined in common.h */
	seat_t s = game.players[p].seat;
	int rating = ggzd_get_rating(game.ggz, p);
	add_player_message(s, "Rating: %d\n", rating);
#endif /* USE_GGZ_STATS */
}


/* 
 * AUTOMATED GLOBAL MESSAGES
 */

void send_last_hand(void)
{
	int *lengths = ggz_malloc(game.num_seats * sizeof(*lengths));
	card_t **cardlist = ggz_malloc(game.num_seats * sizeof(*cardlist));
	hand_t *hand;
	seat_t s;

	for (s = 0; s < game.num_seats; s++) {
		hand = &game.seats[s].hand;
		hand->hand_size = hand->full_hand_size;
		cards_sort_hand(hand);
		lengths[s] = hand->full_hand_size;
		cardlist[s] = hand->cards;
	}

	broadcast_global_cardlist_message("Previous Hand", lengths, cardlist);

	ggz_free(lengths);
	ggz_free(cardlist);
}

void send_last_trick(void)
{
	int *lengths = ggz_malloc(game.num_seats * sizeof(*lengths));
	card_t **cardlist = ggz_malloc(game.num_seats * sizeof(*cardlist));
	card_t *cardlist2 = ggz_malloc(game.num_seats * sizeof(*cardlist2));
	seat_t s;

	for (s = 0; s < game.num_seats; s++) {
		if (game.seats[s].player >= 0) {
			lengths[s] = 1;
			cardlist[s] = &cardlist2[s];
			cardlist[s][0] = game.seats[s].table;
		} else
			lengths[s] = 0;
	}

	broadcast_global_cardlist_message("Last Trick", lengths, cardlist);

	ggz_free(lengths);
	ggz_free(cardlist);
	ggz_free(cardlist2);
}

static int **cumulative_scores = NULL;
static int c_score_count = 0;
static int c_score_size = 0;

static void send_cumulative_scores(void)
{
	char buf[4096];
	int buf_len = 0, r;
	player_t p;
	int widths[game.num_players];

	if (!game.cumulative_scores)
		return;

	for (p = 0; p < game.num_players; p++) {
		widths[p] = strlen(get_player_name(p));
		if (widths[p] < 4)
			widths[p] = 4;
		buf_len +=
			snprintf(buf + buf_len, sizeof(buf) - buf_len,
				 "%*s%s", widths[p], get_player_name(p),
				 p == game.num_players - 1 ? "" : " ");
	}
	buf_len += snprintf(buf + buf_len, sizeof(buf) - buf_len, "\n");

	for (r = 0; r < c_score_count; r++) {
		for (p = 0; p < game.num_players; p++) {
			/* this overcomplicated bit of hackery is intended to
			   center the score under the name. Unfortunately, it
			   assumes the number isn't longer than 3 characters. */
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
			ggz_realloc(cumulative_scores,
				    c_score_size * sizeof(int *));
	}
	cumulative_scores[c_score_count - 1] =
		ggz_malloc(game.num_players * sizeof(int));

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
		widths[p] = strlen(get_player_name(p));
		/* TODO if (widths[p] < game.max_bid_length) widths[p] =
		   game.max_bid_length; */
		buf_len +=
			snprintf(buf + buf_len, sizeof(buf) - buf_len,
				 "%*s%s", widths[p], get_player_name(p),
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
