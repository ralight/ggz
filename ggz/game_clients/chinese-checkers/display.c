/*
 * File: display.c
 * Author: Rich Gade
 * Project: GGZ Chinese Checkers Client
 * Date: 01/01/2001
 * Desc: Game display support structures and functions
 *
 * Copyright (C) 2001 Richard Gade.
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
#  include <config.h>
#endif

#include <gtk/gtk.h>
#include <stdio.h>

#include "display.h"
#include "main_dlg.h"
#include "game.h"
#include "board.xpm"
#include "hole.xpm"
#include "red.xpm"
#include "blue.xpm"
#include "green.xpm"
#include "yellow.xpm"
#include "cyan.xpm"
#include "purple.xpm"


static GtkWidget *dlg_main;
static GtkWidget *draw_area = NULL;
static GtkWidget *statusbar;
static guint sb_context;
static GtkStyle *draw_area_style;
static GdkPixmap *board_buf;
static GdkPixmap *hole_pixmap;
static GdkBitmap *hole_mask;
static GdkGC *hole_gc;
static GdkPixmap *marble_pixmap[6];
static GdkBitmap *marble_mask[6];
static GdkGC *marble_gc[6];


static void display_draw_holes(void);


/* Initialize the game display (dlg_main and board) */
void display_init(void)
{
	/* Create and display the main dialog */
	dlg_main = create_dlg_main();
	gtk_widget_show(dlg_main);

	/* Get and store a pointer to our drawingarea and statusbar */
	draw_area = gtk_object_get_data(GTK_OBJECT(dlg_main), "draw_area");
	draw_area_style = gtk_widget_get_style(draw_area);
	statusbar = gtk_object_get_data(GTK_OBJECT(dlg_main), "statusbar1");
	sb_context = gtk_statusbar_get_context_id(GTK_STATUSBAR(statusbar),
						  "Game Messages");
	display_statusbar("Chinese Checkers for GGZ, version 0.0.1");

	/* Create a pixmap buffer from our xpm */
	board_buf = gdk_pixmap_create_from_xpm_d(draw_area->window, NULL,
					NULL,
					(gchar **) board_xpm);

	/* Convert the rest of our xpms to masked pixmaps */
	hole_gc = gdk_gc_new(draw_area->window);
	gdk_gc_set_fill(hole_gc, GDK_TILED);
	hole_pixmap = gdk_pixmap_create_from_xpm_d(draw_area->window,
					&hole_mask,
					NULL,
					(gchar **) hole_xpm);
	gdk_gc_set_tile(hole_gc, hole_pixmap);
	marble_gc[0] = gdk_gc_new(draw_area->window);
	gdk_gc_set_fill(marble_gc[0], GDK_TILED);
	marble_pixmap[0] = gdk_pixmap_create_from_xpm_d(draw_area->window,
							&marble_mask[0],
							NULL,
							(gchar **) red_xpm);
	gdk_gc_set_tile(marble_gc[0], marble_pixmap[0]);
	marble_gc[1] = gdk_gc_new(draw_area->window);
	gdk_gc_set_fill(marble_gc[1], GDK_TILED);
	marble_pixmap[1] = gdk_pixmap_create_from_xpm_d(draw_area->window,
							&marble_mask[1],
							NULL,
							(gchar **) blue_xpm);
	gdk_gc_set_tile(marble_gc[1], marble_pixmap[1]);
	marble_gc[2] = gdk_gc_new(draw_area->window);
	gdk_gc_set_fill(marble_gc[2], GDK_TILED);
	marble_pixmap[2] = gdk_pixmap_create_from_xpm_d(draw_area->window,
							&marble_mask[2],
							NULL,
							(gchar **) green_xpm);
	gdk_gc_set_tile(marble_gc[2], marble_pixmap[2]);
	marble_gc[3] = gdk_gc_new(draw_area->window);
	gdk_gc_set_fill(marble_gc[3], GDK_TILED);
	marble_pixmap[3] = gdk_pixmap_create_from_xpm_d(draw_area->window,
							&marble_mask[3],
							NULL,
							(gchar **) yellow_xpm);
	gdk_gc_set_tile(marble_gc[3], marble_pixmap[3]);
	marble_gc[4] = gdk_gc_new(draw_area->window);
	gdk_gc_set_fill(marble_gc[4], GDK_TILED);
	marble_pixmap[4] = gdk_pixmap_create_from_xpm_d(draw_area->window,
							&marble_mask[4],
							NULL,
							(gchar **) cyan_xpm);
	gdk_gc_set_tile(marble_gc[4], marble_pixmap[4]);
	marble_gc[5] = gdk_gc_new(draw_area->window);
	gdk_gc_set_fill(marble_gc[5], GDK_TILED);
	marble_pixmap[5] = gdk_pixmap_create_from_xpm_d(draw_area->window,
							&marble_mask[5],
							NULL,
							(gchar **) purple_xpm);
	gdk_gc_set_tile(marble_gc[5], marble_pixmap[5]);

	/* Add the holes to our pixmap buffer */
	display_draw_holes();

	/* Draw our pixmap buffer */
	gdk_draw_pixmap(draw_area->window,
			draw_area_style->fg_gc[GTK_WIDGET_STATE(draw_area)],
			board_buf,
			0, 0,
			0, 0,
			400, 400);
}


/* Redrsaw board areas that just got exposed */
void display_handle_expose_event(GdkEventExpose *event)
{
	if(draw_area)
		gdk_draw_pixmap(draw_area->window,
			draw_area_style->fg_gc[GTK_WIDGET_STATE(draw_area)],
			board_buf,
			event->area.x, event->area.y,
			event->area.x, event->area.y,
			event->area.width, event->area.height);
}


/* Draw all the holes onto the board */
static void display_draw_holes(void)
{
	int i, j, count;
	int x, y;

	for(i = 0; i<17; i++) {
		if(i < 4) {
			x = 193 - (i * 11);
			count = i;
		} else if(i < 9) {
			x = 193 - ((16-i) * 11);
			count = 16 - i;
		} else if(i < 13) {
			x = 193 - (i * 11);
			count = i;
		} else {
			x = 193 - ((16-i) * 11);
			count = 16-i;
		}
		y = (i*19.4) + 38;
		for(j=0; j<=count; j++) {
			gdk_gc_set_ts_origin(hole_gc, x, y);
			gdk_gc_set_clip_origin(hole_gc, x, y);
			gdk_gc_set_clip_mask(hole_gc, hole_mask);

			gdk_draw_rectangle(board_buf,
					   hole_gc,
					   TRUE,
					   x, y,
					   12, 12);
			x += 22;
		}
	}
}


/* Refresh the board completely from the game.board image */
void display_refresh_board(void)
{
	int i, j;
	int x, y;
	int p;

	/* Refresh the holes */
	display_draw_holes();

	/* Refresh the marbles */
	for(i=0; i<17; i++)
		for(j=0; j<25; j++) {
			/* Hole or invalid */
			if(game.board[i][j] < 1)
				continue;

			/* Plop a marble */
			x = j * 11 + 61;
			y = (i*19.4) + 38;
			p = game.board[i][j] - 1;
			gdk_gc_set_ts_origin(marble_gc[p], x, y);
			gdk_gc_set_clip_origin(marble_gc[p], x, y);
			gdk_gc_set_clip_mask(marble_gc[p], marble_mask[p]);

			gdk_draw_rectangle(board_buf,
					   marble_gc[p],
					   TRUE,
					   x, y,
					   12, 12);
		}

	/* Draw our pixmap buffer */
	gdk_draw_pixmap(draw_area->window,
			draw_area_style->fg_gc[GTK_WIDGET_STATE(draw_area)],
			board_buf,
			0, 0,
			0, 0,
			400, 400);
}


void display_handle_click_event(GdkEventButton *event)
{
	int row, col;

	/* Convert to an array row/col and pass to game module */
	col = (event->x - 61) / 11;
	row = (event->y - 38) / 19.4;

	/*printf("[%d][%d]\n", row, col);*/

	game_handle_click_event(row, col);
}


void display_statusbar(char *msg)
{
	gtk_statusbar_push(GTK_STATUSBAR(statusbar), sb_context, msg);
}
