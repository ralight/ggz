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
#define KING_B 1
#define KNIGHT_B 2
#define PAWN_B 3
#define QUEEN_B 4
#define ROOK_B 5
#define BISHOP_W 6
#define KING_W 7
#define KNIGHT_W 8
#define PAWN_W 9
#define QUEEN_W 10
#define ROOK_W 11
#define PIXSIZE 64

#define LOAD_BITMAP(bitmap, data) ( pieces[bitmap] = gdk_pixmap_create_from_xpm_d(main_win->window, &pieces_mask[bitmap], NULL, data) )

/* Load the bitmaps */
void board_init();

/* translate between the cgc piece value and the game piece value */
int board_translate();

/* dnd stuff */
void board_dnd_init();
void board_dnd_highlight( int x, int y, GdkDragContext *drag_context );

/* info (labels and move list) */
void board_info_init();
void board_info_update();
void board_info_add_move(char *);

/* Draw stuff */
void board_draw();
void board_draw_bg();
void board_draw_pieces();
void board_draw_highlights();
void board_draw_outline(int, int, GdkGC *);
void board_draw_piece(int piece, int x, int y);

/* UI */
void board_request_draw();

/* Promote hack */
void
promote_piece                          (GtkButton       *button,
                                        gpointer         user_data);
