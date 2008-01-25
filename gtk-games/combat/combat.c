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

#ifdef HAVE_CONFIG_H
#  include <config.h>	/* Site-specific config */
#endif

#include "combat.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include <ggz.h>

unsigned char *combat_options_string_write(combat_game * _game,
					   int for_hash)
{
	unsigned char *ptr;
	unsigned char *optstr;
	int a;
	int len = (2 + _game->width * _game->height + 12);
	// Should we add space for the name option?
	if (!for_hash && _game->name && (strcmp(_game->name, "") != 0))
		len += 1 + strlen(_game->name) + 1 + 1;
	/* Binary options */
	// O_BIN1
	if (_game->options & 255)
		len += 1 + 1 + 1;
	// O_BIN2
	if (_game->options & (255 << 8))
		len += 1 + 1 + 1;
	optstr = (unsigned char *)ggz_malloc(sizeof(char) * (len + 1));
	strcpy((char*)optstr, "");
	ptr = optstr;
	/* Width * Height */
	*ptr = _game->width;
	*(++ptr) = _game->height;
	/* Terrain data */
	for (a = 0; a < _game->width * _game->height; a++)
		*(++ptr) = _game->map[a].type;
	/* Army data */
	for (a = 0; a < 12; a++)
		*(++ptr) = ARMY(_game, a);
	/* Options */
	// Map name
	if (!for_hash && _game->name && (strcmp(_game->name, "") != 0)) {
		*(++ptr) = O_NAME;	// It's a name option
		for (a = 0; a < strlen(_game->name); a++)
			*(++ptr) = _game->name[a];
		*(++ptr) = 0;	// Closing \0 for the name
		*(++ptr) = 0;	// Closing \0 for the option packet
	}
	if (_game->options & 255) {
		*(++ptr) = O_BIN1;	// Binary pack 1
		*(++ptr) = (_game->options & 255) ^ 255;	// first byte
		*(++ptr) = 0;	// Closing \0 for the option packet
	}
	if (_game->options & (255 << 8)) {
		*(++ptr) = O_BIN2;	// Binary pack 2
		*(++ptr) = ((_game->options >> 8) & 255) ^ 255;	//  seoond byte
		*(++ptr) = 0;	// Closing \0
	}
	/* Close */
	*(++ptr) = 0;
	// Adds one to all the string, to avoid having zeros between them
	for (a = 0; a < len; a++)
		optstr[a]++;
	return optstr;
}

int combat_options_string_read(unsigned char *optstr, combat_game * _game)
{
	int a, b;
	int len = strlen((char*)optstr);
	int retval = 0;
	// Copy the string from the parameter
	// Removes one from all the string, to return the zeroes
	for (a = 0; a < len; a++)
		optstr[a]--;
	/* Width and Height */
	_game->width = optstr[0];
	_game->height = optstr[1];
	/* Terrain data */
	optstr += 2;
	_game->map =
	    ggz_malloc(_game->width * _game->height * sizeof(tile));
	for (a = 0; a < _game->width * _game->height; a++) {
		_game->map[a].type = optstr[a];
		_game->map[a].unit = U_EMPTY;
	}
	/* Army Data */
	optstr += _game->width * _game->height;
	_game->army = (char **)calloc(_game->number + 1, sizeof(char *));
	for (a = 0; a < _game->number + 1; a++) {
		_game->army[a] = (char *)calloc(12, sizeof(char));
		for (b = 0; b < 12; b++)
			_game->army[a][b] = optstr[b];
	}
	/* Options */
	optstr += 12;
	while (*optstr != 0) {
		// Got a option packet! Check what it is
		switch (*optstr) {
		case O_NAME:
			optstr++;
			// optstr now points to the name of the map
			_game->name =
			    (char *)ggz_malloc(strlen((char*)optstr) + 1);
			if (_game->name)
				strcpy(_game->name, (char*)optstr);
			// Go until the last character in the string
			while (*optstr != 0)
				optstr++;
			optstr++;	// optstr now points to the closing /0 for the packet
			break;
		case O_BIN1:
			optstr++;
			// optstr now points to the first byte of options
			_game->options = *(optstr++) ^ 255;	// First byte
			break;
		case O_BIN2:
			optstr++;
			// optstr now points to the first byte
			_game->options += (*(optstr++) ^ 255) << 8;	// Second byte
			break;
		default:
			retval++;
			break;
		}
		// Now go until the end of the packet
		while (*optstr != 0)
			optstr++;
		// Next byte in the stream
		optstr++;
	}
	return retval;
}

int combat_check_move(combat_game * _game, int from, int to)
{
	int f_s, f_u, t_s, t_u, t_t;
	int x1, x2, y1, y2, dx, dy, dir = 0;
	int a;
	int is_rushing = 0;

	// Check range
	if (from < 0 || from >= _game->width * _game->height || to < 0
	    || to >= _game->width * _game->height)
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
	if (f_u <= U_BOMB) {
		// The only scape is that we are
		// using MOVING_BOMB/FLAG and t_u is EMPTY!
		if ((f_u == U_BOMB && !((_game->options & OPT_MOVING_BOMB)
					&& t_u == U_EMPTY))
		    || (f_u == U_FLAG
			&& !((_game->options & OPT_MOVING_FLAG)
			     && t_u == U_EMPTY)))
			return CBT_ERROR_NOTMOVING;
	}

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
	if (abs(dx) > 0 && abs(dy) > 0
	    && !(_game->options & OPT_ALLOW_DIAGONAL))
		return CBT_ERROR_MOVE_DIAGONAL;

	// Checks if it has distance 1
	if (f_u != U_SCOUT
	    && (f_u != U_SERGEANT || !(_game->options & OPT_SF_SERGEANT))
	    && !(_game->options & OPT_RUSH_ATTACK) && (abs(dx) > 1
						       || abs(dy) > 1))
		return CBT_ERROR_MOVE_BIGMOVE;
	else if (f_u == U_SCOUT) {
		// Normalizes the vectors
		dir = 0;
		if (dx != 0)
			dir += dx / abs(dx);
		if (dy != 0)
			dir += dy / abs(dy) * _game->width;
		// Checks what exists between the FROM and the TO
		for (a = from + dir; a != to; a += dir) {
			if (LAST(_game->map[a].type) != T_OPEN
			    || LAST(_game->map[a].unit) != U_EMPTY)
				return CBT_ERROR_MOVE_SCOUT;
		}
		// Check if he is moving >1 and attacking at the same time
		if ((abs(dx) > 1 || abs(dy) > 1) && t_u != U_EMPTY
		    && !(_game->options & OPT_SUPER_SCOUT)
		    && !(_game->options & OPT_RUSH_ATTACK))
			return CBT_ERROR_MOVE_SCOUT;

		if ((abs(dx) > 1 || abs(dy) > 1)
		    && (_game->options & OPT_RUSH_ATTACK)
		    && !(_game->options & OPT_SUPER_SCOUT))
			is_rushing = 1;
	} else if (_game->options & OPT_SF_SERGEANT && f_u == U_SERGEANT) {
		// Ok! Very similar to the scout one, although we must have
		// only lakes betwen from and to
		dir = 0;
		if (dx != 0)
			dir += dx / abs(dx);
		if (dy != 0)
			dir += dy / abs(dy) * _game->width;
		for (a = from + dir; a != to; a += dir) {
			if (LAST(_game->map[a].type) != T_LAKE
			    || LAST(_game->map[a].unit) != U_EMPTY)
				return CBT_ERROR_NOTOPEN;
		}
		// We won't check for move + attack
		// He is a SF guy! He should be able to do that!
	} else if (_game->options & OPT_RUSH_ATTACK) {
		// Rush attack!
		dir = 0;
		if (dx != 0)
			dir += dx / abs(dx);
		if (dy != 0)
			dir += dy / abs(dy) * _game->width;
		for (a = from + dir; a != to; a += dir) {
			if (LAST(_game->map[a].type) != T_OPEN
			    || LAST(_game->map[a].unit) != U_EMPTY)
				return CBT_ERROR_NOTOPEN;
		}
		// We must also be attacking!
		if ((abs(dx) > 1 || abs(dy) > 1) && t_u == U_EMPTY)
			return CBT_ERROR_MOVE_SCOUT;
		is_rushing = 1;
	}
	// No error until now! Then its ok!

	if (t_u == U_EMPTY)
		return CBT_CHECK_MOVE;

	if (t_u != U_EMPTY && !is_rushing)
		return CBT_CHECK_ATTACK;
	else
		return CBT_CHECK_ATTACK_RUSH;

}

int combat_options_check(combat_game * _game)
{
	int a, b, size = 0, cur_size = 0;

	if (ARMY(_game, U_FLAG) <= 0)
		return CBT_ERROR_OPTIONS_FLAGS;

	for (a = 2; a < 13; a++) {
		if (a == 12)
			return CBT_ERROR_OPTIONS_MOVING;
		if (ARMY(_game, a) > 0)
			break;
	}

	for (a = 0; a < 12; a++)
		size += ARMY(_game, a);

	// Checks for number of starting positions
	for (a = 0; a < _game->number; a++) {
		for (b = 0; b < _game->width * _game->height; b++) {
			if (GET_OWNER(_game->map[b].type) == a)
				cur_size++;
		}
		if (cur_size < size)
			return CBT_ERROR_OPTIONS_SIZE;
		cur_size = 0;
	}

	return 0;
}

#define DESC(TXT) do { \
                    strcat(retstr, TXT); \
                    if(short_desc) \
                      strcat(retstr, ", "); \
                    else \
                      strcat(retstr, "\n"); \
                  } while (0)

char *combat_options_describe(combat_game * _game, int short_desc)
{
	char *retstr;
	char temp[32];
	int a, tot = 0;
	retstr = (char *)ggz_malloc(sizeof(char) * 1024);
	strcpy(retstr, "");
	if (!short_desc) {
		if (_game->name) {
			strcat(retstr, "Map: ");
			strcat(retstr, _game->name);
			strcat(retstr, "\n");
		}
		snprintf(temp, sizeof(temp), "Size: %d x %d\n", _game->width,
			_game->height);
		strcat(retstr, temp);
		for (a = 0; a < 12; a++)
			tot += ARMY(_game, a);
		snprintf(temp, sizeof(temp), "%d units\n", tot);
		strcat(retstr, temp);
		if (_game->options)
			strcat(retstr, "Options:\n");
	}
	if (_game->options & OPT_OPEN_MAP)
		DESC("Open map");
	if (_game->options & OPT_ONE_TIME_BOMB)
		DESC("One time bomb");
	if (_game->options & OPT_TERRORIST_SPY)
		DESC("Terrorist spy");
	if (_game->options & OPT_MOVING_BOMB)
		DESC("Moving bombs");
	if (_game->options & OPT_SUPER_SCOUT)
		DESC("Super scout");
	if (_game->options & OPT_MOVING_FLAG)
		DESC("Moving flags");
	if (_game->options & OPT_RANDOM_OUTCOME)
		DESC("Random outcome of attacks");
	if (_game->options & OPT_ALLOW_DIAGONAL)
		DESC("Allow diagonal moves");
	if (_game->options & OPT_UNKNOWN_VICTOR)
		DESC("Unknown victor");
	if (_game->options & OPT_SILENT_DEFENSE)
		DESC("Silent defense");
	if (_game->options & OPT_SILENT_OFFENSE)
		DESC("Silent offense");
	if (_game->options & OPT_RANDOM_SETUP)
		DESC("Random setup");
	if (_game->options & OPT_SF_SERGEANT)
		DESC("Special forces sergeant");
	if (_game->options & OPT_RUSH_ATTACK)
		DESC("Rush attack");
	if (_game->options & OPT_HIDE_UNIT_LIST)
		DESC("Hide enemy unit list");
	if (_game->options & OPT_SHOW_ENEMY_UNITS)
		DESC("Remember enemy units");
	if (short_desc)
		retstr[strlen(retstr) - 2] = 0;

	return retstr;
}

void combat_random_setup(combat_game * _game, int seat)
{
	int total = 0;
	int a, b, c, pos;
	for (a = 0; a < _game->width * _game->height; a++) {
		if (GET_OWNER(_game->map[a].type) == seat
		    && LAST(_game->map[a].unit) == U_EMPTY)
			total++;
	}
	for (a = 0; a < 12; a++) {
		b = _game->army[seat][a];
		while (b-- > 0) {
			// Sanity check
			pos = RANDOM(total);
			while (pos > total)
				pos = RANDOM(total);
			pos--;
			// Ok, so I'll put army 'a' on position 'pos'
			for (c = 0; c < _game->width * _game->height; c++) {
				if (GET_OWNER(_game->map[c].type) == seat
				    && LAST(_game->map[c].unit) == U_EMPTY)
					pos--;
				if (pos < 0)
					break;
			}
			// That's the place!
			_game->map[c].unit = OWNER(seat) + a;
			total--;
		}
	}
}
