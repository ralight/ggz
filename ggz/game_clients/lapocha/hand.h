/*
 * File: hand.h
 * Author: Rich Gade
 * Project: GGZ La Pocha Client
 * Date: 08/14/2000
 * Desc: Functions to get cards from server
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

struct hand_t {
	char hand_size;
	char card[13];
	char selected_card;
	char in_play_card_num;
	char in_play_card_val;
};

extern struct hand_t hand;

extern int hand_read_hand(void);
