/*
 * File: board.h
 * Author: Ismael Orenstein
 * Project: GGZ Chess game module
 * Date: 09/17/2000
 * Desc: Header for board.c
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

/* definitions used here */
#define BISHOP_B 0
#define BISHOP_W 1
#define KING_B 2
#define KING_W 3
#define KNIGHT_B 4
#define KNIGHT_W 5
#define PAWN_B 6
#define PAWN_W 7
#define QUEEN_B 8
#define QUEEN_W 9
#define ROOK_B 10
#define ROOK_W 11
#define PIXSIZE 64

#define LOAD_BITMAP(bitmap, data) ( pieces[bitmap] = gdk_bitmap_create_from_data(main_win->window, data, PIXSIZE, PIXSIZE) )

/* Load the bitmaps */
void board_init();
void board_draw_bg();
