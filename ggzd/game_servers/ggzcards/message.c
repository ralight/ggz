/*
 * File: messages.h
 * Author: Jason Short
 * Project: GGZCards GGZ Server
 * Date: 06/20/2001
 * Desc: Functions and data for messaging system
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

#include <easysock.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "game.h"
#include "ggz.h"
#include "message.h"

/* NOTE ON "MESSAGES"
 *   - Each seat has a message.  The client should display this near to the player.
 *     It may contain information such as bid, etc.
 *   - There is a global message.  The client should display this prominently.
 *     It may contain information such as contract, trump, etc.
 *   - There are additional "tagged" global messages.  These are not implemented.
 */


/* init_messages
 *   inits all data needed for messages
 */
void init_messages()
{
	/* nothing yet */
}

/* send_player_message()
 *   sends seat s's message to player p
 *   fails silently...
 */
void send_player_message(seat_t s, player_t p)
{
	int fd = ggz_seats[p].fd;
	if (ggz_seats[p].assign == GGZ_SEAT_BOT ||
	    game.seats[s].pmessage == NULL)
		return;
	ggz_debug("Sending seat %d/%s's message to player %d/%s: %s", s,
		  game.seats[s].ggz->name, p, ggz_seats[p].name,
		  game.seats[s].pmessage);
	if (es_write_int(fd, WH_MESSAGE_PLAYER) < 0 ||
	    es_write_int(fd, CONVERT_SEAT(s, p)) < 0 ||
	    es_write_string(fd, game.seats[s].pmessage) < 0)
		ggz_debug("ERROR: failed to send player message.");
}

static void doput_player_message(seat_t s, char *msg)
{
	if (game.seats[s].pmessage != NULL)
		free(game.seats[s].pmessage);
	game.seats[s].pmessage = strdup(msg);
	if (game.seats[s].pmessage == NULL) {
		ggz_debug("ERROR: " "bad strdup.");
		exit(-1);
	}
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

/* send_player_message_toall
 *   sends seat s's message to all players
 *   fails silently...
 */
void send_player_message_toall(seat_t s)
{
	player_t p;
	if (s < 0 || s >= game.num_seats)
		ggz_debug("ERROR: SERVER BUG: "
			  "send_player_message_toall(%d) called.", s);
	ggz_debug("Sending seat %d/%s's message to all.", s,
		  game.seats[s].ggz->name);
	for (p = 0; p < game.num_players; p++)
		send_player_message(s, p);
}

void set_player_message(player_t p)
{
	if (game.which_game == GGZ_GAME_UNKNOWN)
		/* silently fail; it's easier to check here than elsewhere */
		return;
	ggz_debug("Setting player %d/%s's message.", p, ggz_seats[p].name);
	if (p < 0 || p >= game.num_players)
		ggz_debug("ERROR: SERVER BUG: "
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

/* send_global_message
 *   sends the truly global message to player p
 *   fails silently...
 */
void send_global_message(char *mark, player_t p)
{
	/* the "mark" is a tag on the message that the client uses to identify it */
	int fd = ggz_seats[p].fd;
	char *message = get_global_message(mark);
	if (mark == NULL) {
		ggz_debug("ERROR: SERVER BUG: "
			  "send_global_message: NULL mark.");
		return;
	}
	if (fd == -1)
		return;
	if (message == NULL)
		message = "";	/* this happens sometimes */
	ggz_debug("Sending global message to player %d/%s: %s", p,
		  ggz_seats[p].name, message);
	es_write_int(fd, WH_MESSAGE_GLOBAL);
	es_write_string(fd, mark);
	es_write_string(fd, message);
}

/* send_global_message_toall
 *   sends the truly global message to all players
 *   fails silently...
 */
void send_global_message_toall(char *mark)
{
	player_t p;
	for (p = 0; p < game.num_players; p++)
		send_global_message(mark, p);
}

void send_all_global_messages(player_t p)
{
	/* TODO: this isn't game-independent;
	 * I _really_ need to make the messaging system
	 * more powerful server-side */
	send_global_message("", p);
	send_global_message("game", p);
	send_global_message("Rules", p);
	send_global_message("Options", p);
	if (game.last_trick)
		send_global_message("Last Trick", p);
	if (game.last_hand)
		send_global_message("Previous Hand", p);
	if (game.cumulative_scores)
		send_global_message("Scores", p);
	if (game.bid_history)
		send_global_message("Bid History", p);
}

/* set_global_message
 */
void set_global_message(char *mark, char *message, ...)
{
	/* TODO: we just use the first character of the mark
	 * as an index into an array.  In the long term, we'll need some
	 * sort of hash instead.  Is there a standard hash library? */
	int hash = *mark;
	char buf[4096];
	int different = 1;

	va_list ap;
	va_start(ap, message);
	vsnprintf(buf, sizeof(buf), message, ap);
	va_end(ap);

	ggz_debug("Setting global message for '%s'.  Length is %d.", mark,
		  strlen(buf));

	/* we re-duplicate it each time; this is a little slower but saves memory.
	 * we also check to see if it's identical; this saves bandwidth and user time overall */
	if (game.messages[hash]) {
		if (!strcmp(game.messages[hash], buf))
			different = 0;
		else
			free(game.messages[hash]);
	}
	if (different) {
		game.messages[hash] = strdup(buf);
		if (game.messages[hash] == NULL) {
			ggz_debug
				("ERROR: set_global_message: strdup returned NULL");
			/* it's NULL, so we should just be able to keep going... */
		}
		send_global_message_toall(mark);
	}
}

char *get_global_message(char *mark)
{
	int hash = *mark;
	return game.messages[hash];
}


void send_last_hand()
{
	int s, c, bsiz = 0;
	char buf[4096];
	hand_t *hand;

	if (!game.last_hand)
		return;

	for (s = 0; s < game.num_seats; s++) {
		hand = &game.seats[s].hand;
		hand->hand_size = hand->full_hand_size;
		cards_sort_hand(hand);
		bsiz += snprintf(buf + bsiz, sizeof(buf) - bsiz, "%17s: ",
				 game.seats[s].ggz->name);
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
}

void send_last_trick()
{
	/* this sets up a "last trick" message */
	player_t p, p_r;
	char message[512];
	int msg_len = 0;
	card_t card;

	if (!game.last_trick)
		return;

	for (p_r = 0; p_r < game.num_players; p_r++) {
		p = (game.leader + p_r) % game.num_players;
		card = game.seats[game.players[p].seat].table;
		msg_len +=
			snprintf(message + msg_len, sizeof(message) - msg_len,
				 "%17s - %s of %s\n", ggz_seats[p].name,
				 face_names[(int) card.face],
				 suit_names[(int) card.suit]
				 /*, p == hi_player ? " (winner)" : "" */
			);
	}

	set_global_message("Last Trick", "%s", message);
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
		widths[p] = strlen(ggz_seats[p].name);
		if (widths[p] < 4)
			widths[p] = 4;
		buf_len +=
			snprintf(buf + buf_len, sizeof(buf) - buf_len,
				 "%*s%s", widths[p], ggz_seats[p].name,
				 p == game.num_players - 1 ? "" : " ");
	}
	buf_len += snprintf(buf + buf_len, sizeof(buf) - buf_len, "\n");

	for (r = 0; r < c_score_count; r++) {
		for (p = 0; p < game.num_players; p++) {
			/* this overcomplicated bit of hackery is
			 * intended to center the score under the name.
			 * Unfortunately, it assumes the number isn't longer
			 * than 3 characters. */
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
			ggz_debug
				("ERROR: update_cumulative_scores: NULL realloc.");
			exit(-1);
		}
	}
	cumulative_scores[c_score_count - 1] =
		(int *) alloc(game.num_players * sizeof(int));

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
	char buf2[game.max_bid_length];

	if (!game.bid_history)
		return;

	for (p = 0; p < game.num_players; p++) {
		widths[p] = strlen(ggz_seats[p].name);
		if (widths[p] < game.max_bid_length)
			widths[p] = game.max_bid_length;
		buf_len +=
			snprintf(buf + buf_len, sizeof(buf) - buf_len,
				 "%*s%s", widths[p], ggz_seats[p].name,
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
