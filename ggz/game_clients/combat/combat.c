/*
 * File: combat.c
 * Author: Ismael Orenstein
 * Project: GGZ Combat game module
 * Date: 09/17/2000
 * Desc: Commom (client/server) combat functions
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

#include "combat.h"
#include "stdlib.h"
#include "stdio.h"

char *combat_options_string_write(char *optstr, combat_game *_game) {
	char *ptr;
	int a;
	int len = (2 + _game->width*_game->height + 12 + 1);
	optstr = (char *)malloc(sizeof(char) * len);
	ptr = optstr;
	*ptr = _game->width;
	*(++ptr) = _game->height;
	for (a = 0; a < _game->width * _game->height; a++)
		*(++ptr) = _game->map[a].type;
	for (a = 0; a < 12; a++)
		*(++ptr) = _game->army[0][a];
	*(++ptr) = 0;
	// Adds one to all the string, to avoid having zeros between them
	for (a = 0; a < len; a++)
		optstr[a]++;
	printf("Len: %d\n", strlen(optstr));
	return optstr;
}

void combat_options_string_read(char *optstr, combat_game *_game, int num_players) {
	int a, b;
	int len = strlen(optstr);
	// Removes one from all the string, to return the zeroes
	for (a = 0; a < len; a++)
		optstr[a]--;
	_game->width = optstr[0];
	_game->height = optstr[1];
	optstr+=2;
	_game->map = calloc(_game->width*_game->height, sizeof(tile));
	for (a = 0; a < _game->width*_game->height; a++) {
			_game->map[a].type = optstr[a];
			_game->map[a].unit = U_EMPTY;
	}
	optstr+=_game->width*_game->height;
	_game->army = (char **)calloc(num_players, sizeof(char*));
	for (a = 0 ; a < num_players; a++) {
		_game->army[a] = (char *)calloc(12, sizeof(char));
		for (b = 0; b < 12; b++)
			_game->army[a][b] = optstr[b];
	}
	optstr+=12;
	while (*(optstr++) != 0)
		printf("Unsuported option! (%d)\n", *optstr);
}

