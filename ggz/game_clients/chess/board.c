/*
 * File: board.c
 * Author: Ismael Orenstein
 * Project: GGZ Chess game module
 * Date: 09/17/2000
 * Desc: Graphical functions handling the game board and filters for user input
 * (sending the events to game.c)
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

#include "board.h"
#include "support.h"
#include <stdlib.h>
#include <gtk/gtk.h>

/* Include images */
#include "bitmaps/bishop_b.xbm"
#include "bitmaps/bishop_w.xbm"
#include "bitmaps/king_b.xbm"
#include "bitmaps/king_w.xbm"
#include "bitmaps/knight_b.xbm"
#include "bitmaps/knight_w.xbm"
#include "bitmaps/pawn_b.xbm"
#include "bitmaps/pawn_w.xbm"
#include "bitmaps/queen_b.xbm"
#include "bitmaps/queen_w.xbm"
#include "bitmaps/rook_b.xbm"
#include "bitmaps/rook_w.xbm"

/* Graphics declarations */
GdkBitmap **pieces = NULL;
GdkGC *pieces_gc = NULL;
GdkGC *light_gc = NULL;
GdkGC *dark_gc = NULL;
GdkColor bg_color[2];
GdkPixmap *board_buf = NULL;

/* Main window */
extern GtkWidget *main_win;

void board_init() {
  if (pieces)
    return;
  /* Load images */
  pieces = (GdkBitmap **)malloc(12 * sizeof(GdkBitmap *));
  LOAD_BITMAP(BISHOP_B, BISHOP_B_bits);
  LOAD_BITMAP(BISHOP_W, BISHOP_W_bits);
  LOAD_BITMAP(KING_B, KING_B_bits);
  LOAD_BITMAP(KING_W, KING_W_bits);
  LOAD_BITMAP(KNIGHT_B, KNIGHT_B_bits);
  LOAD_BITMAP(KNIGHT_W, KNIGHT_W_bits);
  LOAD_BITMAP(PAWN_B, PAWN_B_bits);
  LOAD_BITMAP(PAWN_W, PAWN_W_bits);
  LOAD_BITMAP(QUEEN_B, QUEEN_B_bits);
  LOAD_BITMAP(QUEEN_W, QUEEN_W_bits);
  LOAD_BITMAP(ROOK_B, ROOK_B_bits);
  LOAD_BITMAP(ROOK_W, ROOK_W_bits);
  /* Init the GC */
  pieces_gc = gdk_gc_new(main_win->window);
  gdk_gc_set_fill(pieces_gc, GDK_STIPPLED);
  gdk_gc_set_foreground(pieces_gc, &gtk_widget_get_style(main_win)->black);
  /* Colors */
  gdk_color_parse("rgb:C8/B5/72", &bg_color[0]);
  gdk_colormap_alloc_color(gtk_widget_get_colormap(main_win), &bg_color[0], TRUE, TRUE);
  gdk_color_parse("rgb:67/55/3F", &bg_color[1]);
  gdk_colormap_alloc_color(gtk_widget_get_colormap(main_win), &bg_color[1], TRUE, TRUE);
  /* Background GC */
  light_gc = gdk_gc_new(main_win->window);
  gdk_gc_set_fill(light_gc, GDK_SOLID);
  gdk_gc_set_foreground(light_gc, &bg_color[0]);
  gdk_gc_set_background(light_gc, &bg_color[0]);
  dark_gc = gdk_gc_new(main_win->window);
  gdk_gc_set_fill(dark_gc, GDK_SOLID);
  gdk_gc_set_foreground(dark_gc, &bg_color[1]);
  gdk_gc_set_background(dark_gc, &bg_color[1]);
}

void board_draw_bg() {
  int i, j;
  if (!pieces)
    board_init();
  if (!board_buf)
    return;
  /* Draw squares */
  for (i = 0; i < 8; i++) {
    for (j = 0; j < 8; j++) {
      /* i+j even -> light
       * i+j odd  -> dark */
      gdk_draw_rectangle(board_buf,
          ( (i+j)%2==0 ? light_gc : dark_gc ),
          TRUE,
          i*PIXSIZE, j*PIXSIZE,
          PIXSIZE, PIXSIZE);
    }
  }

  /* Draw lines */
  for (i = 1; i < 8; i++) {
    gdk_draw_line(board_buf,
        main_win->style->black_gc,
        i*PIXSIZE, 0,
        i*PIXSIZE, PIXSIZE*8);
    
    gdk_draw_line(board_buf,
        main_win->style->black_gc,
        0, i*PIXSIZE,
        8*PIXSIZE, i*PIXSIZE);
  }

  gtk_widget_draw(lookup_widget(main_win, "board"), NULL);
  
}
