/*
 * File: game.h
 * Author: Rich Gade
 * Project: GGZ Chinese Checkers Client
 * Date: 01/01/2001
 * Desc: Core game structures and logic
 *
 * Copyright (C) 2001 Richard Gade.
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


#define GGZ_SEAT_OPEN	-1


struct game_t {
	int fd;
	int me;
	int players;
	int seats[6];
	char names[6][17];
	char state;
	char my_turn;
	char got_players;
	char board[17][25];
};

struct node_t {
	int ro, co;
	int rd, cd;
};

extern struct game_t game;

extern void game_init(void);
extern void game_init_board(void);
extern void game_handle_click_event(int, int);
extern void game_notify_our_turn(void);
extern void game_opponent_move(int, int, int, int, int);
