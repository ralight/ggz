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


extern void init_messages();

extern void send_player_message(seat_t, player_t);
extern void send_player_message_toall(seat_t);
extern void set_player_message(player_t p);
extern void set_all_player_messages();

extern void send_global_message(char*, player_t);
extern void send_global_message_toall(char*);
extern void send_all_global_messages(player_t p);
extern void set_global_message(char*, char*, ...);
extern char* get_global_message(char*);

/* the following are automated messages */
extern void send_last_hand();
extern void send_last_trick();
extern void init_cumulative_scores();
extern void update_cumulative_scores();
extern void send_bid_history();
