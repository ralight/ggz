/*
 * File: table.h
 * Author: Rich Gade
 * Project: GGZ La Pocha Client
 * Date: 08/14/2000
 * Desc: Routines to handle the Gtk game table
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

extern void table_initialize(void);
extern void table_handle_expose_event(GdkEventExpose *);
extern void table_handle_click_event(GdkEventButton *);
extern void table_set_name(int, char *);
extern void table_set_dealer(void);
extern void table_set_bidder(int);
extern void table_display_hand(void);
extern void table_set_bid(int, int);
extern void table_set_tricks(int, int);
extern void table_set_score(int, int);
extern void table_set_trump(void);
extern void table_animation_abort(void);
extern void table_animation_zip(gboolean);
extern void table_animation_opponent(char, char);
extern void table_clear_table(void);
extern void table_show_cards(char, char, char, char);
extern void table_style_change(void);
