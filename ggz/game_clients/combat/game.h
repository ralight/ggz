/*
 * File: game.h
 * Author: Ismael Orenstein
 * Project: GGZ Combat game module
 * Date: 09/17/2000
 * Desc: Game header
 *
 * Copyright (C) 2000 Ismael Orenstein.
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

// FIXME: Do that on the status bar
#define game_status(STR...) printf(STR);

// Size of images
#define PIXSIZE 48

// Max number of players
#define MAX_PLAYERS 2


// Game information
struct game_info_t {
	int fd;
	int seat;
	int number;
	int version;
};

// Handle IO
void game_handle_io(gpointer, gint, GdkInputCondition);

// Get info from the server
int game_get_seat();
int game_get_options();

// Get info from the player
int game_ask_options();

// Init variables
void game_init();
void game_init_board();

// Draw stuff
void game_draw_bg();
void game_draw_board();
void game_draw_tile(int, int, int, int);
void game_add_player_info(int);
void game_update_unit_list(int);

// Player info widget
GtkWidget *gtk_player_info_new(GtkWidget *, char *);
