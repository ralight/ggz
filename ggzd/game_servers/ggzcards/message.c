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
	if (fd == -1) return;
	ggz_debug("Sending seat %d/%s's message to player %d/%s: %s", s, game.seats[s].ggz->name, p, ggz_seats[p].name, game.seats[s].message);
	es_write_int(fd, WH_MESSAGE_PLAYER);
	es_write_int(fd, CONVERT_SEAT(s, p));
	es_write_string(fd, game.seats[s].message);
}

/* send_player_message_toall
 *   sends seat s's message to all players
 *   fails silently...
 */
void send_player_message_toall(seat_t s)
{
	player_t p;
	ggz_debug("Sending seat %d/%s's message to all.", s, game.seats[s].ggz->name);
	for (p = 0; p < game.num_players; p++)
		send_player_message(s, p);
}

void set_all_player_messages()
{
	player_t p;
	for (p=0; p<game.num_players; p++)
		game_set_player_message(p);	
}

/* send_global_message
 *   sends the truly global message to player p
 *   fails silently...
 */
void send_global_message(char* mark, player_t p)
{
	/* the "mark" is a tag on the message that the client uses to identify it */
	int fd = ggz_seats[p].fd;
	char* message = get_global_message(mark);
	if (mark == NULL) {
		ggz_debug("SERVER BUG: send_global_message: NULL mark.");
		return;
	}
	if (fd == -1) return;
	if (message == NULL) message = ""; /* this happens sometimes */
	ggz_debug("Sending global message to player %d/%s: %s", p, ggz_seats[p].name, message);
	es_write_int(fd, WH_MESSAGE_GLOBAL);
	es_write_string(fd, mark);
	es_write_string(fd, message);
}

/* send_global_message_toall
 *   sends the truly global message to all players
 *   fails silently...
 */
void send_global_message_toall(char* mark)
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
}

/* set_global_message
 */
void set_global_message(char* mark, char* message, ...)
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

	ggz_debug("Setting global message for '%s'.  Length is %d.", mark, strlen(buf));

	/* we re-duplicate it each time; this is a little slower but saves memory.
	 * we also check to see if it's identical; this saves bandwidth and user time overall */
	if (game.messages[hash]) {
		if (!strcmp(game.messages[hash], buf))
			different = 0;
		else
			free (game.messages[hash]);
	}
	if (different) {
		game.messages[hash] = strdup(buf);
		if (game.messages[hash] == NULL) {
			ggz_debug("ERROR: set_global_message: strdup returned NULL");
			/* it's NULL, so we should just be able to keep going... */
		}
		send_global_message_toall(mark);
	}
}

char* get_global_message(char* mark)
{
	int hash = *mark;
	return game.messages[hash];
}


