/* $Id: table.h 2070 2001-07-23 00:03:11Z jdorje $ */
/*
 * File: table.h
 * Author: Rich Gade
 * Project: GGZCards Client
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

#include "hand.h"
#include "game.h" /* for ANIMATION */

extern GtkRcStyle *fixed_font_style;

extern void table_initialize(void);
extern void table_setup(void);
extern void table_handle_expose_event(GdkEventExpose *);
extern void table_handle_click_event(GdkEventButton *);
extern void table_set_name(int, char *);
extern void table_set_dealer(void);
extern void table_set_bidder(int);
extern void table_display_hand(int);
extern void table_display_all_hands(void);
extern void table_set_bid(int, int);
extern void table_set_tricks(int, int);
extern void table_set_score(int, int);
extern void table_set_trump(void);
#ifdef ANIMATION
extern void table_animation_abort(void);
extern void table_animation_zip(gboolean);
extern void table_animation(int, card_t);
#endif /* ANIMATION */
extern void table_clear_table(void);
extern void table_show_cards();
extern void table_style_change(void);

extern void table_play_card(int, card_t);

extern void table_set_player_message(int, char*);
extern void table_set_message(char*, char*);
