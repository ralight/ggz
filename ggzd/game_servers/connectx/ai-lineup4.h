/*
 * Interface to the LINeup4 Four-in-a-row AI
 * Copyright (C) 2008 Josef Spillner, josef@ggzgamingzone.org
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

#ifndef AI_LINEUP4_H
#define AI_LINEUP4_H

/* Strength levels */
#define LINEUP4_EASY 1
#define LINEUP4_NORMAL 2

/* Sets up the board and initialises the AI engine */
/* NOTE: LINeup4 command line AI only works for 7x6 boards! */
void lineup4_ai_init(int rows, int columns, int level);

/* Puts a piece into one of the columns */
/* Returns 1 if legal, 0 if not */
int lineup4_ai_move(int column);

/* Finds a column for the player */
/* Returns 1 if found, 0 if not found */
int lineup4_ai_find(int *column);

/* Outputs the board to the console */
void lineup4_ai_output(void);

/* Game over detection */
/* Returns 1 on game over, 0 for game continuation */
int lineup4_ai_gameover(void);

/* Velena engine interface */
int lineup4_launch(void);
int lineup4_check(void);
void lineup4_shutdown(void);

#endif

