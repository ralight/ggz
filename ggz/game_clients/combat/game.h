/*
 * File: game.h
 * Author: Ismael Orenstein
 * Project: GGZ Combat game module
 * Date: 09/17/2000
 * Desc: Game header
 * $Id: game.h 5122 2002-10-30 22:12:20Z jdorje $
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

// Size of images
#define PIXSIZE 48

// Max number of players
#define MAX_PLAYERS 2

// Where to save the global maps

#define GLOBAL_MAPS GAMEDIR "/combat/maps"


// Game information
struct game_info_t {
	int fd;
	int seat;
	char **names;
	int *seats;
	int version;
	int current;
	gboolean show_enemy;
	char last_unit;
	int last_from;
	int last_to;
};

// Handle IO
void game_handle_io(gpointer, gint, GdkInputCondition);
void game_unit_list_handle (GtkCList *clist, gint row, gint column,
	 													GdkEventButton *event, gpointer user_data);

// Handle setup
void game_handle_setup(int);
void game_setup_add(int, int);
void game_setup_remove(int);

// Get info from the server
int game_get_seat(void);
int game_get_options(void);
int game_get_players(void);
void game_get_move(void);
void game_get_attack(void);
void game_get_gameover(void);
void game_get_sync(void);

// Get info from the player
void game_ask_options(void);
void game_handle_setup(int);
void game_handle_move(int);
void game_send_setup(void);
int game_send_options(GtkWidget *);
void game_resync(void);
gboolean game_refuse_options (GtkWidget       *widget,
                              GdkEvent        *event,
                              gpointer         user_data);

// Init variables
void game_init(void);
void game_init_board(void);
void game_start(void);

// Draw stuff
void game_draw_bg(void);
void game_draw_board(void);
void game_draw_extra(void);
void game_draw_unit(int, int, int, int);
void game_draw_terrain(int, int, int);
void game_add_player_info(int);
void game_update_unit_list(int);
void game_update_player_name(int);
void game_status(const char *format, ... );
void game_message(const char *format, ... );
void game_change_turn(void);

// Player info widget
GtkWidget *gtk_player_info_new(GtkWidget *, char *, int);

// Saves maps
void game_ask_save_map(void);
void game_confirm_save_map(GtkButton *, gpointer );
