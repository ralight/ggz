/*
 * GGZ Gaming Zone TicTacToe 3D
 * Copyright (C) 2002 Josef Spillner, dr_maux@users.sourceforge.net
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

#ifndef DISPLAY_H
#define DISPLAY_H

#include <GL/gl.h>

extern int cursormove;
extern int cursor;
extern int usetexturesmove;
extern int movemove;

extern GLfloat view_rotx, view_roty;
extern GLfloat angle;

extern char board[9];

void draw(void);
void idle(void);
void reshape(int width, int height);
void init(void);

#endif

