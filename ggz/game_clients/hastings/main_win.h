/*
 * File: main.c
 * Author: Josef Spillner
 * Original Author: Brent Hendricks
 * Project: GGZ Hastings1066 game module
 * Date: 09/13/00
 * Desc: Main window creation and utility funcs
 *
 * Copyright (C) 2000 - 2002 Josef Spillner
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

#ifndef HASTINGS_MAIN_WIN_H
#define HASTINGS_MAIN_WIN_H

extern const char *player_colors[];
extern const char *team_colors[];

/* Create the main window */
GtkWidget *create_main_win(void);

/* Display the game status */
void game_status(const char *format, ...);

/* Paint the game board */
void display_board(void);

#endif
