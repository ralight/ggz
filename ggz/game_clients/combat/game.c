/*
 * File: game.c
 * Author: Ismael Orenstein
 * Project: GGZ Combat game module
 * Date: 09/17/2000
 * Desc: Game functions
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

#include <easysock.h>
#include <stdlib.h>
#include <stdio.h>
#include <gtk/gtk.h>
#include "game.h"
#include "combat.h"

GtkWidget *main_win;
GtkWidget **player_list = NULL;
struct game_info_t cbt_info;
combat_game cbt_game;

// Name of units
static char unitname[12][36] = {"Flag", "Bomb", "Spy", "Scout", "Miner", "Sergeant", "Lieutenant", "Captain", "Major", "Colonel", "General", "Marshall"};

// Graphics stuff
GdkPixmap* cbt_buf;

void game_handle_io(gpointer data, gint fd, GdkInputCondition cond) {
	int op = -1;

	// Read the fd
	if (es_read_int(fd, &op) < 0) {
		printf("Couldn't read the game fd\n");
		return;
	}

	// FIXME: Erase this
	printf("Got message from the server!\n");

	switch (op) {
		case CBT_MSG_SEAT:
			game_get_seat();
			break;
		case CBT_REQ_OPTIONS:
			game_ask_options();
			break;
		case CBT_MSG_OPTIONS:
			game_get_options();
			break;
		default:
			game_status("Ops! Wrong message: %d", op);
			break;
	}
}

int game_get_seat() {
	if (es_read_int(cbt_info.fd, &cbt_info.seat) < 0)
		return -1;

	if (es_read_int(cbt_info.fd, &cbt_info.number) < 0)
		return -1;

	if (es_read_int(cbt_info.fd, &cbt_info.version) < 0)
		return -1;

	game_status("Getting init information\nSeat: %d\tPlayers: %d\tVersion: %d\n", cbt_info.seat, cbt_info.number, cbt_info.version);

	cbt_game.state = CBT_STATE_WAIT;

	game_add_player_info(cbt_info.number);

	return 0;
}

int game_ask_options() {
	combat_game _game;
	char *game_str = NULL;
	int a;
	// Default game
	game_status("Using default game options\n");
	_game.width = 10;
	_game.height = 10;
	_game.map = (tile *)calloc(_game.width * _game.height, sizeof(tile));
	_game.army = (char **)calloc(1, sizeof(char *));
	_game.army[0] = (char *)calloc(12, sizeof(char));
	for (a = 0; a < _game.width*4; a++) {
		_game.map[a].type = OWNER(0) + T_OPEN;
		_game.map[a+60].type = OWNER(1) + T_OPEN;
	}

	for (a = 40; a < _game.width*6; a++) {
		_game.map[a].type = T_OPEN;
	}

	_game.map[CART(3,5,10)].type = T_LAKE;
	_game.map[CART(4,5,10)].type  = T_LAKE;
	_game.map[CART(3,6,10)].type  = T_LAKE;
	_game.map[CART(4,6,10)].type  = T_LAKE;

	_game.map[CART(7,5,10)].type  = T_LAKE;
	_game.map[CART(8,5,10)].type  = T_LAKE;
	_game.map[CART(7,6,10)].type  = T_LAKE;
	_game.map[CART(8,6,10)].type  = T_LAKE;

	// FIXME: Delete this
	printf("Writing army data\n");

	_game.army[0][U_FLAG] = 1;
	_game.army[0][U_BOMB] = 6;
	_game.army[0][U_SPY] = 1;
	_game.army[0][U_SCOUT] = 8;
	_game.army[0][U_MINER] = 5;
	_game.army[0][U_SERGEANT] = 4;
	_game.army[0][U_LIEUTENANT] = 4;
	_game.army[0][U_CAPTAIN] = 4;
	_game.army[0][U_MAJOR] = 3;
	_game.army[0][U_COLONEL] = 2;
	_game.army[0][U_GENERAL] = 1;
	_game.army[0][U_MARSHALL] = 1;
	
	game_status("Sending options string to server\n");

	game_str = combat_options_string_write(game_str, &_game);

	if (es_write_int(cbt_info.fd, CBT_MSG_OPTIONS) < 0 || es_write_string(cbt_info.fd, game_str) < 0)
		return -1;
	return 0;
}

int game_get_options() {
	char *optstr = NULL;
	int a;

	if (es_read_string_alloc(cbt_info.fd, &optstr) < 0)
		return -1;

	combat_options_string_read(optstr, &cbt_game, cbt_info.number);

	for (a = 0; a < cbt_info.number; a++)
		game_update_unit_list(a);

	return 0;
}

void game_init() {
	cbt_game.map = NULL;
	cbt_game.width = 10;
	cbt_game.height = 10;
	cbt_game.army = NULL;
	cbt_game.state = CBT_STATE_INIT;
}

void game_draw_bg() {
	int i;

	gdk_draw_rectangle( cbt_buf,
			main_win->style->mid_gc[GTK_WIDGET_STATE(main_win)], TRUE, 0, 0, PIXSIZE*cbt_game.width, PIXSIZE*cbt_game.height);

	for (i = 1; i < cbt_game.width; i++) {
		gdk_draw_line(cbt_buf, main_win->style->black_gc,
				i*PIXSIZE, 0, i*PIXSIZE, PIXSIZE*cbt_game.height);
	}
	for (i = 1; i < cbt_game.height; i++) {
		gdk_draw_line(cbt_buf, main_win->style->black_gc,
				0, i*PIXSIZE, PIXSIZE*cbt_game.width, i*PIXSIZE);;
	}
}
	

void game_draw_board() {
}

void game_add_player_info(int number) {
	int a;
	GtkWidget *box;
	char name_str[32];

	// Allocs memory
	player_list = (GtkWidget **)calloc(number, sizeof(GtkWidget *));

	// Finds box
	box = gtk_object_get_data(GTK_OBJECT(main_win), "player_box");

	// Creates and puts on the right place
	for (a = 0; a < number; a++) {
		sprintf(name_str, "player_list[%d]", a);
		player_list[a] = (GtkWidget *)gtk_player_info_new(main_win, name_str);
		gtk_box_pack_start (GTK_BOX(box), player_list[a], TRUE, TRUE, 0);
		gtk_widget_show(player_list[a]);
		gtk_widget_ref(player_list[a]);
	}
}

void game_update_unit_list(int seat) {
	GtkWidget *unit_list;
	GtkWidget *player_info;
	char widget_name[32];
	char **info;
	int a;

	sprintf(widget_name, "player_list[%d]", seat);
	player_info = gtk_object_get_data(GTK_OBJECT(main_win), widget_name);
	unit_list = gtk_object_get_data(GTK_OBJECT(player_info), "unit_list");
	info = (char **)calloc(3,sizeof(char *));
	info[0] = (char*)malloc(32*sizeof(char)); 
	info[1] = (char*)malloc(32*sizeof(char)); 
	info[2] = (char*)malloc(32*sizeof(char)); 

	for (a = 0; a < 12; a++) {
		strcpy(info[0], unitname[a]);
		sprintf(info[1], "%d/%d", cbt_game.army[seat][a],
				cbt_game.army[cbt_info.number][a]);
		if (a <= 1)
			sprintf(info[2], "-");
		else
			sprintf(info[2], "%d", a-1);
		gtk_clist_insert(GTK_CLIST(unit_list), a, info);
	}
}
