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
	_game->map = malloc(_game->width*_game->height * sizeof(tile));
	for (a = 0; a < _game->width*_game->height; a++) {
			_game->map[a].type = optstr[a];
			_game->map[a].unit = U_EMPTY;
	}
	optstr+=_game->width*_game->height;
	_game->army = (char **)calloc(num_players+1, sizeof(char*));
	for (a = 0 ; a < num_players+1; a++) {
		_game->army[a] = (char *)calloc(12, sizeof(char));
		for (b = 0; b < 12; b++)
			_game->army[a][b] = optstr[b];
	}
	optstr+=12;
	while (*(optstr++) != 0)
		printf("Unsuported option! (%d)\n", *optstr);
}

int combat_check_move(combat_game *_game, int from, int to) {
	int f_s, f_u, t_s, t_u, t_t;
	int x1, x2, y1, y2, dx, dy, dir = 0;
	int a;

	// Check range
	if (from < 0 || from >= _game->width*_game->height || to < 0 || to >= _game->width*_game->height)
		return CBT_ERROR_OUTRANGE;

	// Gets seat and unit
	f_s = GET_OWNER(_game->map[from].unit);
	f_u = LAST(_game->map[from].unit);
	t_s = GET_OWNER(_game->map[to].unit);
	t_u = LAST(_game->map[to].unit);
	t_t = LAST(_game->map[to].type);

	/* Checks ownership */

	// Checks if the f_s is of the current player
	if (f_s != _game->turn)
		return CBT_ERROR_WRONGTURN;

	// Checks if the TO units belongs to the current player
	if (t_s == _game->turn)
		return CBT_ERROR_SUICIDAL;

	/* Checks units */

	// Checks if the FROM unit is a moving one 
	// (!= BOMB, != FLAG)
	if (f_u <= U_BOMB)
		return CBT_ERROR_NOTMOVING;

	/* Checks terrain */

	// Checks if the TO terrain is empty
	if (t_t != T_OPEN)
		return CBT_ERROR_NOTOPEN;

	/* Now checks the validity of the movement */

	// Gets positions
	x1 = X(from, _game->width);
	x2 = X(to, _game->width);
	y1 = Y(from, _game->width);
	y2 = Y(to, _game->width);

	dx = x2 - x1;
	dy = y2 - y1;

	// Checks if it is a move
	if (from == to)
		return CBT_ERROR_MOVE_NOMOVE;

	// Checks if its not a diagonal move
	if (abs(dx) > 0 && abs(dy) > 0)
		return CBT_ERROR_MOVE_DIAGONAL;

	// Checks if it has distance 1
	if (f_u != U_SCOUT && abs(dx + dy) != 1)
		return CBT_ERROR_MOVE_BIGMOVE;
	else if (f_u == U_SCOUT) {
		// Normalizes the vectors
		if (dx != 0)
			dir = dx/abs(dx);
		if (dy != 0)
			dir = dy/abs(dy) * _game->width;
		// Checks what exists between the FROM and the TO
		for (a = from + dir; a != to; a += dir) {
			if (LAST(_game->map[a].type) != T_OPEN || LAST(_game->map[a].unit) != U_EMPTY)
				return CBT_ERROR_MOVE_SCOUT;
		}
		// Check if he is moving >1 and attacking at the same time
		if (abs(dx+dy) != 1 && t_u != U_EMPTY)
			return CBT_ERROR_MOVE_SCOUT;
	}

	// Now error until now! Then its ok!
	
	if (t_u == U_EMPTY)
		return CBT_CHECK_MOVE;		

	if (t_u != U_EMPTY)
		return CBT_CHECK_ATTACK;

	// This really shouldn't happen!!!!
	return CBT_ERROR_CRAZY;

}
	

