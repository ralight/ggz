/*
 * File: board.c
 * Author: Ismael Orenstein
 * Project: GGZ Chess game module
 * Date: 09/17/2000
 * Desc: Graphical functions handling the game board and filters for user input
 * (sending the events to game.c)
 * $Id: board.c 9338 2007-11-08 19:38:21Z jdorje $
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <gtk/gtk.h>
#include <ggz.h>

#include "board.h"
#include "game.h"
#include "net.h"
#include "libcgc/cgc.h"
#include "support.h"
#include "chess.h"

/* Graphics declarations */
static GdkPixbuf *pieces[12];
static GdkGC *piece_gc = NULL;
static GdkGC *light_gc = NULL;
static GdkGC *dark_gc = NULL;
static GdkGC *red_gc = NULL;
static GdkColor bg_color[3];
GdkPixmap *board_buf = NULL;

/* Main window */
extern GtkWidget *main_win;

/* Game */
extern game_t *game;

extern struct chess_info game_info;

/* dnd stuff */
GtkTargetEntry *target;

int PIXSIZE = 64;

static GdkPixbuf *load_pixmap(const char *name)
{
	char *fullpath = g_strdup_printf("%s/chess/pixmaps/%s.svg",
					 GGZDATADIR, name);
	GdkPixbuf *image;
	GError *error = NULL;

	image = gdk_pixbuf_new_from_file_at_size(fullpath, PIXSIZE, PIXSIZE,
						 &error);
	if (image == NULL)
		ggz_error_msg_exit("Can't load pixmap %s", fullpath);
	g_free(fullpath);

	return image;
}

void board_resized(int width, int height)
{
	int i;

	PIXSIZE = MIN(width / 8, height / 8);

	/* Unload images. */
	for (i = 0; i < 12; i++) {
		if (pieces[i]) {
			g_object_unref(pieces[i]);
		}
	}

	/* Load images */
	pieces[BISHOP_B] = load_pixmap("bishop_b");
	pieces[BISHOP_W] = load_pixmap("bishop_w");
	pieces[KING_B] = load_pixmap("king_b");
	pieces[KING_W] = load_pixmap("king_w");
	pieces[KNIGHT_B] = load_pixmap("knight_b");
	pieces[KNIGHT_W] = load_pixmap("knight_w");
	pieces[PAWN_B] = load_pixmap("pawn_b");
	pieces[PAWN_W] = load_pixmap("pawn_w");
	pieces[QUEEN_B] = load_pixmap("queen_b");
	pieces[QUEEN_W] = load_pixmap("queen_w");
	pieces[ROOK_B] = load_pixmap("rook_b");
	pieces[ROOK_W] = load_pixmap("rook_w");

	/* No highlights */
	game_info.src_x = -1;
	game_info.src_y = -1;
	game_info.dest_x = -1;
	game_info.dest_y = -1;
}

void board_init(void)
{
	if (pieces[0]) {
		return;
	}

	board_resized(PIXSIZE * 8, PIXSIZE * 8);

	/* Init the GC */
	piece_gc = gdk_gc_new(main_win->window);
	gdk_gc_set_fill(piece_gc, GDK_TILED);
	/* Colors */
	gdk_color_parse("Tan", &bg_color[0]);
	gdk_colormap_alloc_color(gtk_widget_get_colormap(main_win),
				 &bg_color[0], TRUE, TRUE);
	gdk_color_parse("DarkSlateGray", &bg_color[1]);
	gdk_colormap_alloc_color(gtk_widget_get_colormap(main_win),
				 &bg_color[1], TRUE, TRUE);
	gdk_color_parse("Red", &bg_color[2]);
	gdk_colormap_alloc_color(gtk_widget_get_colormap(main_win),
				 &bg_color[2], TRUE, TRUE);
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

	/* Setup the player info */
	//board_info_init();
}

void board_info_init(void)
{
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
	for (j = 0; j < 5; j++) {
		style->fg[j] = gtk_widget_get_style(main_win)->black;
		style->bg[j] = gtk_widget_get_style(main_win)->black;
	}
	pango_font_description_set_weight(style->font_desc,
					  PANGO_WEIGHT_BOLD);
	pango_font_description_set_size(style->font_desc, 14);

	gtk_widget_set_style(black, style);
	/* Arrows */
	gtk_widget_set_style(black_arrow, style);
	gtk_widget_set_style(white_arrow, style);

	/* White label */
	style = gtk_style_copy(gtk_widget_get_style(main_win));
	gdk_color_parse("rgb:88/88/88", &color);
	gdk_colormap_alloc_color(gtk_widget_get_colormap(main_win), &color,
				 FALSE, TRUE);
	for (j = 0; j < 5; j++) {
		/*
		   style->fg[j] = gtk_widget_get_style(main_win)->white;
		   style->bg[j] = gtk_widget_get_style(main_win)->white;
		 */
		style->fg[j] = color;
		style->bg[j] = color;
	}
	pango_font_description_set_weight(style->font_desc,
					  PANGO_WEIGHT_BOLD);
	pango_font_description_set_size(style->font_desc, 14);
	gtk_widget_set_style(white, style);

}

void board_info_update(void)
{
	GtkWidget *black, *white;
	char text[32];

	black = lookup_widget(main_win, "black_time");
	white = lookup_widget(main_win, "white_time");

	/* Set the white text */
	sprintf(text, "%s -> %02d:%02d", game_info.name[0],
		game_info.t_seconds[0] / 60, game_info.t_seconds[0] % 60);
	gtk_label_set_text(GTK_LABEL(white), text);

	/* Set the black text */
	sprintf(text, "%s -> %02d:%02d", game_info.name[1],
		game_info.t_seconds[1] / 60, game_info.t_seconds[1] % 60);
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




void board_dnd_init(void)
{
	GtkWidget *board;
	target = malloc(sizeof(GtkTargetEntry));

	board = lookup_widget(main_win, "board");

	target->target = malloc(sizeof(char) * 6);
	strcpy(target->target, "board");
	target->flags = GTK_TARGET_SAME_WIDGET;
	target->info = 233;

	gtk_drag_dest_set(board,
			  GTK_DEST_DEFAULT_DROP | GTK_DEST_DEFAULT_MOTION,
			  target, 1, GDK_ACTION_MOVE);


}

void board_draw(void)
{
	board_draw_bg();
	board_draw_pieces();
	board_draw_highlights();

	gtk_widget_queue_draw(lookup_widget(main_win, "board"));
}

void board_draw_highlights(void)
{
	int x, y;
	char output[80];
	cgc_getboard(output, game->board);
	if (game_info.src_x >= 0 && game_info.src_y >= 0)
		board_draw_outline(game_info.src_x, game_info.src_y,
				   gtk_widget_get_style(main_win)->
				   white_gc);
	if (game_info.dest_x >= 0 && game_info.dest_y >= 0)
		board_draw_outline(game_info.dest_x, game_info.dest_y,
				   gtk_widget_get_style(main_win)->
				   white_gc);
	if (game_info.check) {
		/* Find the king ! */
		for (x = 0; x < 8; x++) {
			for (y = 0; y < 8; y++) {
				if ((game_info.seat == 0
				     && output[x + (y * 9)] == 'K')
				    || (game_info.seat == 1
					&& output[x + (y * 9)] == 'k'))
					board_draw_outline(x, y, red_gc);
			}
		}
	}
}

void board_draw_outline(int x, int y, GdkGC * gc)
{
	/* Outside outline */
	gdk_draw_rectangle(board_buf,
			   gc,
			   FALSE,
			   x * PIXSIZE - 1, y * PIXSIZE - 1,
			   PIXSIZE + 2, PIXSIZE + 2);
	/* Inside outline */
	gdk_draw_rectangle(board_buf,
			   gc,
			   FALSE,
			   x * PIXSIZE, y * PIXSIZE, PIXSIZE, PIXSIZE);
}





void board_draw_bg(void)
{
	int i, j;
	if (!pieces[0])
		board_init();
	if (!board_buf)
		return;
	/* Draw squares */
	for (i = 0; i < 8; i++) {
		for (j = 0; j < 8; j++) {
			/* i+j even -> dark
			 * i+j odd  -> light */
			gdk_draw_rectangle(board_buf,
					   ((i + j) % 2 ==
					    0 ? light_gc : dark_gc), TRUE,
					   i * PIXSIZE, j * PIXSIZE,
					   PIXSIZE, PIXSIZE);
		}
	}

	/* Draw lines */
	for (i = 1; i < 8; i++) {
		gdk_draw_line(board_buf,
			      main_win->style->black_gc,
			      i * PIXSIZE, 0, i * PIXSIZE, PIXSIZE * 8);

		gdk_draw_line(board_buf,
			      main_win->style->black_gc,
			      0, i * PIXSIZE, 8 * PIXSIZE, i * PIXSIZE);
	}

}

void board_draw_pieces(void)
{
	int x, y;
	char output[80];

	if (!game)
		game_update(CHESS_EVENT_INIT, NULL);

	cgc_getboard(output, game->board);

	for (x = 0; x < 8; x++) {
		for (y = 0; y < 8; y++) {
			board_draw_piece(board_translate
					 (output[x + (y * 9)]), x, y);
		}
	}

}

int board_translate(int cgc_val)
{
	switch (cgc_val) {
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

void board_draw_piece(int piece, int x, int y)
{
	if (piece < 0)
		return;

	gdk_draw_pixbuf(board_buf, piece_gc, pieces[piece],
			0, 0, x * PIXSIZE, y * PIXSIZE, PIXSIZE, PIXSIZE,
			GDK_RGB_DITHER_NONE, 0, 0);
}

void board_dnd_highlight(int x, int y, GdkDragContext * drag_context)
{
	char output[80];
	int piece;
	cgc_getboard(output, game->board);
	piece = board_translate(output[x + (y * 9)]);
	gtk_drag_set_icon_pixbuf(drag_context, pieces[piece], 5, 5);
	game_info.src_x = x;
	game_info.src_y = y;
}

void board_request_draw(void)
{
	game_message("Requesting draw!");
	net_send_draw();
}

void board_call_flag(void)
{
	game_message("Calling flag");
	net_call_flag();
}

int board_auto_call(void)
{
	GtkWidget *auto_call;
	auto_call = lookup_widget(main_win, "auto_call_flag");
	return GTK_CHECK_MENU_ITEM(auto_call)->active;
}

void board_info_add_move(char *move)
{
	GtkWidget *move_list = lookup_widget(main_win, "last_moves");
	char *text = g_strdup_printf("%s -> %s\n",
				     game_info.turn %
				     2 ? "White" : "Black",
				     move);
	/* FIXME: formatting? */
	GtkTextBuffer *buf =
	    gtk_text_view_get_buffer(GTK_TEXT_VIEW(move_list));
	GtkTextIter iter;
	gtk_text_buffer_get_end_iter(buf, &iter);
	gtk_text_buffer_insert(buf, &iter, text, strlen(text));
	free(text);
}

void promote_piece(GtkButton * button, gpointer user_data)
{
	GtkWidget *widget;
	char promote = 0;
	char *move = g_object_get_data(G_OBJECT(main_win), "promote");

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

void board_request_update(void)
{
	net_request_update();
}
