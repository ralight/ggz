/*
 * File: main.h
 * Author: Rich Gade
 * Project: GGZ Connect the Dots Client
 * Date: 08/14/2000
 * Desc: Main loop and supporting logic
 *
 * Copyright (C) 2000, 2001 Brent Hendricks.
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

/* Maximums */
#define MAX_BOARD_WIDTH  25
#define MAX_BOARD_HEIGHT 25

/* Exposed globals */
extern GtkWidget *main_win, *opt_dialog;
extern char game_state;
extern int conf_handle;

struct game_t {
	/* Basic info about connection */
	int fd;
	int me;
	int opponent;
	int seats[2];
	int score[2];
	char names[2][17];
	char state;
	char move;
	char got_players;
};
extern struct game_t game;

/* Exposed functions */
extern int send_options(void);
extern void game_init(void);
extern void handle_req_newgame(void);
