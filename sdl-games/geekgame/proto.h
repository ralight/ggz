/*
 * Geekgame - a game which only real geeks understand
 * Copyright (C) 2002, 2003 Josef Spillner, josef@ggzgamingzone.org

 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef GEEKGAME_PROTO_H
#define GEEKGAME_PROTO_H

#define OP_GREETING 100
#define OP_NEWPLAYER 101
#define OP_GAMESTART 102
#define OP_GAMESTOP 103
#define OP_GAMEEND 104
#define OP_MOVERESULT 105
#define OP_MODERESULT 106
#define OP_NUMPLAYERS 150

#define OP_PRESENTATION 200
#define OP_RULESET 201
#define OP_MOVE 202

#define MODE_RESERVED 1000
#define MODE_EASY 1001
#define MODE_MATRIX 1002
#define MODE_HAVOC 1003
#define MODE_HAX0R 1004

#endif

