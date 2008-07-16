/*
 * File: game.h
 * Author: Rich Gade
 * Project: GGZ Chinese Checkers Client
 * Date: 01/01/2001
 * Desc: Core game structures and logic
 * $Id: game.h 6293 2004-11-07 05:51:47Z jdorje $
 *
 * Copyright (C) 2001-2002 Richard Gade.
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

#include <ggzmod.h>

struct game_t {
	int fd;
	int me;
	int players;
	int seats[6];
	char names[6][17];
	char my_turn;
#define ROWS 17
#define COLS 25
#define BOARD_NONE -1
#define BOARD_EMPTY 0
	char board[17][25];
	int conf_handle;
	int beep;
	char *theme;		/* Allocated with ggz_malloc/ggz_strdup */
	char **theme_names;
	int num_themes;
	/* These two are never actually used: */
	/* char state; */
	/* char got_players; */
};

struct node_t {
	int ro, co;
	int rd, cd;
};

extern struct game_t game;
extern int homes[6][6];

extern void game_init(void);
void game_zap_board(void);	/* Clears board - called in several places. */
void game_init_board(void);
extern void game_handle_click_event(int, int);
extern void game_notify_our_turn(void);
extern void game_opponent_move(int, int, int, int, int);
extern void game_handle_sync(int);
extern void game_update_config(char *, int);
extern char *get_theme_dir(void);
extern GGZMod *game_mod(void);
