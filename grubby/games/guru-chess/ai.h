/*
 * Chess game module for Guru
 * Copyright (C) 2004 Josef Spillner, josef@ggzgamingzone.org
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef AI_H
#define AI_H

/* Figure colors */
#define C_NONE 0
#define C_WHITE 1
#define C_BLACK 2

/* Sets up a chess board in an (0-64) array */
/* The color is the one the player is using */
void chess_ai_init(int color, int depth);

/* Move a figure on the chess board */
/* Returns 1 on success, 0 on invalid move */
int chess_ai_move(int from, int to);

/* Finds a move */
/* Returns 1 if found, 0 if not found */
int chess_ai_find(int color, int *from, int *to);

/* Outputs the board to the console */
void chess_ai_output(void);

#endif

