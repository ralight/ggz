/*
 * File: message.h
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


extern void init_messages();	/* initializes all necessary message data */

/* Note the difference between set_player_message and set_global_message.  They
 * have similar names, but behave quite differently.  This is because global and
 * player messages are handled quite differently.  Global messages are set at
 * specific points in the code, and sent out to the clients as-is.  Player
 * messages are set by a specific function in the code, so the rest of the code
 * just calls set_player_message() to have them automatically set up and sent
 * out. */

/* player messages */
extern void send_player_message(seat_t, player_t);	/* sends the seat's message to one player */
extern void send_player_message_toall(seat_t);	/* sends the seat's message to all players */
extern void set_player_message(player_t p);	/* calls the appropriate game set_player_message function, then sends mesage to everyone */
extern void set_all_player_messages();	/* calls set_player_message for all players */

extern void clear_player_message(seat_t s);	/* clears the player message; NOTE it's by seat not player */
extern void put_player_message(seat_t s, char *, ...);	/* sets the player message for the appropriate seat, printf-style. */
extern void add_player_message(seat_t s, char *fmt, ...);	/* adds to an existing player message */

/* global messages */
extern void send_global_message(char *, player_t);	/* simply sends the current message */
extern void send_global_message_toall(char *);	/* sends the current message to everyone */
extern void send_all_global_messages(player_t p);	/* sends _all_ messages to one player */
extern void set_global_message(char *, char *, ...);	/* sets the global message (sprintf-stype), and sends it to everyone */
extern char *get_global_message(char *);	/* returns the message */

/* the following are automated player messages */
extern void add_player_score_message(player_t);
extern void add_player_bid_message(player_t);
extern void add_player_tricks_message(player_t);
extern void add_player_action_message(player_t);

/* the following are automated global messages */
extern void send_last_hand();	/* sends a listing of the cards in the last hand */
extern void send_last_trick();	/* sends a listing of the cards on the last trick */
extern void init_cumulative_scores();	/* initialized the "cumulative scores" tracking */
extern void update_cumulative_scores();	/* updates the "cumulative scores", and sends new one */
extern void send_bid_history();	/* sends the bid history from the current hand */
