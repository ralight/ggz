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

#include <stdlib.h>
#include <stdio.h>
#include <gtk/gtk.h>
#include "board.h"
#include "game.h"
#include "net.h"
#include "libcgc/cgc.h"
#include "support.h"
#include "chess.h"

/* Include images */
#include "bitmaps/bishop_b.xpm"
#include "bitmaps/bishop_w.xpm"
#include "bitmaps/king_b.xpm"
#include "bitmaps/king_w.xpm"
#include "bitmaps/knight_b.xpm"
#include "bitmaps/knight_w.xpm"
#include "bitmaps/pawn_b.xpm"
#include "bitmaps/pawn_w.xpm"
#include "bitmaps/queen_b.xpm"
#include "bitmaps/queen_w.xpm"
#include "bitmaps/rook_b.xpm"
#include "bitmaps/rook_w.xpm"

/* Graphics declarations */
GdkPixmap **pieces = NULL;
GdkBitmap **pieces_mask = NULL;
GdkGC *piece_gc = NULL;
GdkGC *light_gc = NULL;
GdkGC *dark_gc = NULL;
GdkGC *red_gc = NULL;
GdkColor bg_color[3];
GdkPixmap *board_buf = NULL;

/* Main window */
extern GtkWidget *main_win;

/* Game */
extern game_t *game;

extern struct chess_info game_info;

/* dnd stuff */
GtkTargetEntry *target;

void board_init() {
  if (pieces)
    return;
  /* Load images */
  pieces = (GdkPixmap **)malloc(12 * sizeof(GdkPixmap *));
  pieces_mask = (GdkBitmap **)malloc(12 * sizeof(GdkBitmap *));
  LOAD_BITMAP(BISHOP_B, bishop_b_xpm);
  LOAD_BITMAP(BISHOP_W, bishop_w_xpm);
  LOAD_BITMAP(KING_B, king_b_xpm);
  LOAD_BITMAP(KING_W, king_w_xpm);
  LOAD_BITMAP(KNIGHT_B, knight_b_xpm);
  LOAD_BITMAP(KNIGHT_W, knight_w_xpm);
  LOAD_BITMAP(PAWN_B, pawn_b_xpm);
  LOAD_BITMAP(PAWN_W, pawn_w_xpm);
  LOAD_BITMAP(QUEEN_B, queen_b_xpm);
  LOAD_BITMAP(QUEEN_W, queen_w_xpm);
  LOAD_BITMAP(ROOK_B, rook_b_xpm);
  LOAD_BITMAP(ROOK_W, rook_w_xpm);
  /* Init the GC */
  piece_gc = gdk_gc_new(main_win->window);
  gdk_gc_set_fill(piece_gc, GDK_TILED);
  /* Colors */
  gdk_color_parse("rgb:C8/B5/72", &bg_color[0]);
  gdk_colormap_alloc_color(gtk_widget_get_colormap(main_win), &bg_color[0], TRUE, TRUE);
  gdk_color_parse("rgb:67/55/3F", &bg_color[1]);
  gdk_colormap_alloc_color(gtk_widget_get_colormap(main_win), &bg_color[1], TRUE, TRUE);
  gdk_color_parse("rgb:FF/00/00", &bg_color[2]);
  gdk_colormap_alloc_color(gtk_widget_get_colormap(main_win), &bg_color[2], TRUE, TRUE);
  /* Background GC */
  light_gc = gdk_gc_new(main_win->window);
  gdk_gc_set_fill(light_gc, GDK_SOLID);
  gdk_gc_set_foreground(light_gc, &bg_color[0]);
  gdk_gc_set_background(light_gc, &bg_color[0]);
  dark_gc = gdk_gc_new(main_win->window);
  gdk_gc_set_fill(dark_gc, GDK_SOLID);
  gdk_gc_set_foreground(dark_gc, &bg_color[1]);
  gdk_gc_set_background(dark_gc, &bg_color[1]);
  red_gc = gdk_gc_new(main_win->window);
  gdk_gc_set_fill(red_gc, GDK_SOLID);
  gdk_gc_set_foreground(red_gc, &bg_color[2]);
  gdk_gc_set_background(red_gc, &bg_color[2]);

  /* Setup the drag and drop */
  board_dnd_init();

  /* No highlights */
  game_info.src_x = -1;
  game_info.src_y = -1;
  game_info.dest_x = -1;
  game_info.dest_y = -1;

	/* Setup the player info */
	//board_info_init();
}

void board_info_init() {
	GtkWidget *black, *white;
	GtkWidget *black_arrow, *white_arrow;
	GdkColor color;
	GtkStyle *style;
	int j;

	black = lookup_widget(main_win, "black_time");
	white = lookup_widget(main_win, "white_time");
	black_arrow = lookup_widget(main_win, "black_arrow");
	white_arrow = lookup_widget(main_win, "white_arrow");


	/* Black label */
	style = gtk_style_copy(gtk_widget_get_style(main_win));
	for (j=0; j<5; j++) {
		style->fg[j] = gtk_widget_get_style(main_win)->black;
		style->bg[j] = gtk_widget_get_style(main_win)->black;
	}
	style->font = gdk_font_load("-*-*-bold-r-normal-*-14-*");
	gtk_widget_set_style(black, style);
	/* Arrows */
	gtk_widget_set_style(black_arrow, style);
	gtk_widget_set_style(white_arrow, style);

	/* White label */
	style = gtk_style_copy(gtk_widget_get_style(main_win));
	gdk_color_parse("rgb:88/88/88", &color);
	gdk_colormap_alloc_color(gtk_widget_get_colormap(main_win), &color, FALSE, TRUE);
	for (j=0; j<5; j++) {
		/*
		style->fg[j] = gtk_widget_get_style(main_win)->white;
		style->bg[j] = gtk_widget_get_style(main_win)->white;
		*/
		style->fg[j] = color;
		style->bg[j] = color;
	}
	style->font = gdk_font_load("-*-*-bold-r-normal-*-14-*");
	gtk_widget_set_style(white, style);

}

void board_info_update() {
	GtkWidget *black, *white;
	char text[32];

	black = lookup_widget(main_win, "black_time");
	white = lookup_widget(main_win, "white_time");

	/* Set the white text */
	sprintf(text, "%s -> %02d:%02d", game_info.name[0], game_info.t_seconds[0]/60, game_info.t_seconds[0]%60);
	gtk_label_set_text(GTK_LABEL(white), text);

	/* Set the black text */
	sprintf(text, "%s -> %02d:%02d", game_info.name[1], game_info.t_seconds[1]/60, game_info.t_seconds[1]%60);
	gtk_label_set_text(GTK_LABEL(black), text);

	/* Get the arrows to set the current turn */
	black = lookup_widget(main_win, "black_arrow");
	white = lookup_widget(main_win, "white_arrow");
	gtk_widget_hide(black);
	gtk_widget_hide(white);
	if (game_info.turn % 2)
		gtk_widget_show(black);
	else
		gtk_widget_show(white);

}

	


void board_dnd_init() {
  GtkWidget *board;
  target = (GtkTargetEntry *)malloc(sizeof(GtkTargetEntry));

  board = lookup_widget(main_win, "board");

  target->target = (char *)malloc(sizeof(char) * 6);
  strcpy(target->target, "board");
  target->flags = GTK_TARGET_SAME_WIDGET;
  target->info = 233;

  gtk_drag_dest_set(board, GTK_DEST_DEFAULT_DROP | GTK_DEST_DEFAULT_MOTION, target, 1, GDK_ACTION_MOVE);


}

void board_draw() {
  board_draw_bg();
  board_draw_pieces();
  board_draw_highlights();

  gtk_widget_draw(lookup_widget(main_win, "board"), NULL);
}

void board_draw_highlights() {
  int x, y;
	char output[80];
	cgc_getboard(output, game->board);
  if (game_info.src_x >= 0 && game_info.src_y >= 0)
    board_draw_outline(game_info.src_x, game_info.src_y, gtk_widget_get_style(main_win)->white_gc);
  if (game_info.dest_x >= 0 && game_info.dest_y >= 0)
    board_draw_outline(game_info.dest_x, game_info.dest_y, gtk_widget_get_style(main_win)->white_gc);
  if (game_info.check) {
    /* Find the king ! */
    for (x = 0; x < 8; x++) {
      for (y = 0; y < 8; y++) {
        if ((game_info.seat == 0 && output[x+(y*9)] == 'K') || (game_info.seat == 1 && output[x+(y*9)] == 'k'))
          board_draw_outline(x, y, red_gc);
      }
    }
  }
}

void board_draw_outline(int x, int y, GdkGC *gc) {
  /* Outside outline */
  gdk_draw_rectangle(board_buf,
      gc,
      FALSE,
      x*PIXSIZE-1, y*PIXSIZE-1,
      PIXSIZE+2, PIXSIZE+2);
  /* Inside outline */
  gdk_draw_rectangle(board_buf,
      gc,
      FALSE,
      x*PIXSIZE, y*PIXSIZE,
      PIXSIZE, PIXSIZE);
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
      /* i+j even -> dark
       * i+j odd  -> light */
      gdk_draw_rectangle(board_buf,
          ( (i+j)%2==0 ? dark_gc : light_gc),
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

}

void board_draw_pieces() {
  int x, y;
	char output[80];

  if (!game)
    game_update(CHESS_EVENT_INIT, NULL);

	cgc_getboard(output, game->board);

  for (x = 0; x < 8; x++) {
    for (y = 0; y < 8; y++) {
      board_draw_piece(board_translate(output[x+(y*9)]), x, y);
    }
  }

}

int board_translate(int cgc_val) {
  switch(cgc_val) {
    case 'P':
      return PAWN_W;
    case 'R':
      return ROOK_W;
    case 'N':
      return KNIGHT_W;
    case 'B':
      return BISHOP_W;
    case 'Q':
      return QUEEN_W;
    case 'K':
      return KING_W;
	  case 'p':
      return PAWN_B;
    case 'r':
      return ROOK_B;
    case 'n':
      return KNIGHT_B;
    case 'b':
      return BISHOP_B;
    case 'k':
      return KING_B;
    case 'q':
      return QUEEN_B;
  }
  return -1;
}

void board_draw_piece(int piece, int x, int y) {

  if (piece < 0)
    return;

  gdk_gc_set_tile(piece_gc, pieces[piece]);
  gdk_gc_set_ts_origin(piece_gc, x*PIXSIZE, y*PIXSIZE);
  gdk_gc_set_clip_origin(piece_gc, x*PIXSIZE, y*PIXSIZE);
  gdk_gc_set_clip_mask(piece_gc, pieces_mask[piece]);

  gdk_draw_rectangle(board_buf, piece_gc, TRUE, x*PIXSIZE, y*PIXSIZE, 
                     PIXSIZE, PIXSIZE);
}

void board_dnd_highlight( int x, int y, GdkDragContext *drag_context) {
	char output[80];
  int piece;	
	cgc_getboard(output, game->board);
	piece = board_translate(output[x+(y*9)]);
  gtk_drag_set_icon_pixmap(drag_context, gtk_widget_get_colormap(main_win),
      pieces[piece], pieces_mask[piece], 5, 5);
  game_info.src_x = x;
  game_info.src_y = y;
}

void board_request_draw() {
  game_message("Requesting draw!");
  net_send_draw();
}

void board_call_flag() {
  game_message("Calling flag");
  net_call_flag();
}

int board_auto_call() {
  GtkWidget *auto_call;
  auto_call = lookup_widget(main_win, "auto_call_flag");
  return GTK_CHECK_MENU_ITEM(auto_call)->active;
}

void board_info_add_move(char *move) {
  GtkWidget *move_list = lookup_widget(main_win, "last_moves");
  char *text;
  text = g_strdup_printf("%s -> %s\n", game_info.turn % 2 ? "White" : "Black", move);
  gtk_text_insert(GTK_TEXT(move_list), gtk_widget_get_style(move_list)->font, &gtk_widget_get_style(move_list)->black, &gtk_widget_get_style(move_list)->white, text, strlen(text)+1);
  free(text);
}

void
promote_piece                          (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget *widget;
  char promote = 0;
  char *move = gtk_object_get_data(GTK_OBJECT(main_win), "promote");
  /* It's the queen ? */
  widget = lookup_widget(user_data, "queen");
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
    promote = 'Q';
  /* It's the rook ? */
  widget = lookup_widget(user_data, "rook");
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
    promote = 'R';
  /* It's the bishop ? */
  widget = lookup_widget(user_data, "bishop");
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
    promote = 'B';
  /* It's the knight ? */
  widget = lookup_widget(user_data, "knight");
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
    promote = 'N';

  /* Ok, update the move */
  move[4] = promote;
  move[5] = 0;

  game_update(CHESS_EVENT_MOVE_END, move);
  free(move);

}

void board_request_update() {
	net_request_update();
}
