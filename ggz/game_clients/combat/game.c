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
struct game_info_t cbt_info;
combat_game cbt_game;

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

	if (es_read_string_alloc(cbt_info.fd, &optstr) < 0)
		return -1;

	combat_options_string_read(optstr, &cbt_game, cbt_info.number);

	printf("Width: %d\nHeight: %d\n", cbt_game.width, cbt_game.height);
	printf("X:5, Y:6 -> %d\n", cbt_game.map[CART(5,6,cbt_game.width)].type);
	printf("Number of Majors: %d\n", cbt_game.army[1][U_MAJOR]);

	return 0;
}
