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
#include <stdlib.h>

#include <ggz.h>	/* libggz */

#include "display.h"
#include "main_dlg.h"
#include "game.h"


GtkWidget *dlg_main = NULL;
static GtkWidget *draw_area = NULL;
static GtkWidget *statusbar;
static guint sb_context;
static GtkStyle *draw_area_style;
static GdkPixmap *board_img;
static GdkPixmap *board_buf;
static GdkPixmap *hole_pixmap;
static GdkBitmap *hole_mask;
static GdkGC *hole_gc;
static GdkPixmap *marble_pixmap[6];
static GdkBitmap *marble_mask[6];
static GdkGC *marble_gc[6];
static GdkGC *gc_line;
static GtkWidget *label[6];
static char *label_color[6] = {
	"RGB:FF/00/00",
	"RGB:00/00/FF",
	"RGB:00/FF/00",
	"RGB:FF/FF/00",
	"RGB:00/FF/FF",
	"RGB:FF/00/FF"
};


static void display_draw_holes(void);


static GdkPixmap *display_load_pixmap(GdkWindow *window, GdkBitmap **mask,
				   GdkColor *trans, const char *name)
{
	char *fullpath;
	GdkPixmap *pixmap;

	fullpath = g_strdup_printf("%s/%s", get_theme_dir(), name);
	pixmap = gdk_pixmap_create_from_xpm(window, mask, trans, fullpath);
	if(pixmap == NULL)
		ggz_error_msg_exit("Can't load pixmap %s", fullpath);
	g_free(fullpath);

	return pixmap;
}


/* Initialize the game display (dlg_main and board) */
int display_init(void)
{
	GdkColormap *sys_colormap;
	GdkColor color;

	/* Create and display the main dialog */
	if(dlg_main == NULL) {
		dlg_main = create_dlg_main();
		gtk_widget_show(dlg_main);
	}

	/* Get and store a pointer to our drawingarea and statusbar */
	draw_area = gtk_object_get_data(GTK_OBJECT(dlg_main), "draw_area");
	draw_area_style = gtk_widget_get_style(draw_area);
	statusbar = gtk_object_get_data(GTK_OBJECT(dlg_main), "statusbar1");
	sb_context = gtk_statusbar_get_context_id(GTK_STATUSBAR(statusbar),
						  "Game Messages");

	/* Create a pixmap buffer from our xpm */
	board_img = display_load_pixmap(draw_area->window, NULL,
					NULL,
					"board.xpm");
	if(board_img == NULL)
		return -1;
	board_buf = gdk_pixmap_new(draw_area->window,
				   draw_area->allocation.width,
				   draw_area->allocation.height,
				   -1);
	gdk_draw_pixmap(board_buf,
			draw_area_style->fg_gc[GTK_WIDGET_STATE(draw_area)],
			board_img,
			0, 0,
			0, 0,
			400, 400);

	/* Convert the rest of our xpms to masked pixmaps */
	hole_gc = gdk_gc_new(draw_area->window);
	gdk_gc_set_fill(hole_gc, GDK_TILED);
	hole_pixmap = display_load_pixmap(draw_area->window,
					&hole_mask,
					NULL,
					"hole.xpm");
	if(hole_pixmap == NULL)
		return -1;
	gdk_gc_set_tile(hole_gc, hole_pixmap);
	marble_gc[0] = gdk_gc_new(draw_area->window);
	gdk_gc_set_fill(marble_gc[0], GDK_TILED);
	marble_pixmap[0] = display_load_pixmap(draw_area->window,
						&marble_mask[0],
						NULL,
						"red.xpm");
	if(marble_pixmap[0] == NULL)
		return -1;
	gdk_gc_set_tile(marble_gc[0], marble_pixmap[0]);
	marble_gc[1] = gdk_gc_new(draw_area->window);
	gdk_gc_set_fill(marble_gc[1], GDK_TILED);
	marble_pixmap[1] = display_load_pixmap(draw_area->window,
						&marble_mask[1],
						NULL,
						"blue.xpm");
	if(marble_pixmap[1] == NULL)
		return -1;
	gdk_gc_set_tile(marble_gc[1], marble_pixmap[1]);
	marble_gc[2] = gdk_gc_new(draw_area->window);
	gdk_gc_set_fill(marble_gc[2], GDK_TILED);
	marble_pixmap[2] = display_load_pixmap(draw_area->window,
						&marble_mask[2],
						NULL,
						"green.xpm");
	if(marble_pixmap[2] == NULL)
		return -1;
	gdk_gc_set_tile(marble_gc[2], marble_pixmap[2]);
	marble_gc[3] = gdk_gc_new(draw_area->window);
	gdk_gc_set_fill(marble_gc[3], GDK_TILED);
	marble_pixmap[3] = display_load_pixmap(draw_area->window,
						&marble_mask[3],
						NULL,
						"yellow.xpm");
	if(marble_pixmap[3] == NULL)
		return -1;
	gdk_gc_set_tile(marble_gc[3], marble_pixmap[3]);
	marble_gc[4] = gdk_gc_new(draw_area->window);
	gdk_gc_set_fill(marble_gc[4], GDK_TILED);
	marble_pixmap[4] = display_load_pixmap(draw_area->window,
						&marble_mask[4],
						NULL,
						"cyan.xpm");
	if(marble_pixmap[4] == NULL)
		return -1;
	gdk_gc_set_tile(marble_gc[4], marble_pixmap[4]);
	marble_gc[5] = gdk_gc_new(draw_area->window);
	gdk_gc_set_fill(marble_gc[5], GDK_TILED);
	marble_pixmap[5] = display_load_pixmap(draw_area->window,
						&marble_mask[5],
						NULL,
						"purple.xpm");
	if(marble_pixmap[5] == NULL)
		return -1;
	gdk_gc_set_tile(marble_gc[5], marble_pixmap[5]);

	/* Setup the gc for our line drawing */
	gc_line = gdk_gc_new(draw_area->window);
	sys_colormap = gdk_colormap_get_system();
	gdk_color_parse("White", &color);
	gdk_colormap_alloc_color(sys_colormap, &color, FALSE, TRUE);
	gdk_gc_set_foreground(gc_line, &color);
	gdk_gc_set_function(gc_line, GDK_XOR);
	gdk_gc_set_line_attributes(gc_line, 2, GDK_LINE_SOLID,
				   GDK_CAP_BUTT, GDK_JOIN_ROUND);

	/* Add the holes to our pixmap buffer */
	display_draw_holes();

	/* Draw our pixmap buffer */
	gdk_draw_pixmap(draw_area->window,
			draw_area_style->fg_gc[GTK_WIDGET_STATE(draw_area)],
			board_buf,
			0, 0,
			0, 0,
			400, 400);

	return 0;
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

			gdk_draw_pixmap(board_buf,
				draw_area_style->fg_gc[GTK_WIDGET_STATE(draw_area)],
				board_img,
				x, y,
				x, y,
				12, 12);
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

	/* Clear a path if one is showing */
	display_show_path(NULL);

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

	/* ggz_debug("main", "[%d][%d]", row, col); */

	game_handle_click_event(row, col);
}


void display_statusbar(char *msg)
{
	static int firsttime=1;

	if(firsttime)
		firsttime = 0;
	else
		gtk_statusbar_pop(GTK_STATUSBAR(statusbar), sb_context);

	gtk_statusbar_push(GTK_STATUSBAR(statusbar), sb_context, msg);
}


/* Not really animation, just draws a line from src to dest */
void display_show_path(GSList *path_list)
{
	GdkRectangle update_rect;
	struct node_t *node;
	int x1, y1, x2, y2;
	GSList *tmp;
	static GSList *old_path_list=NULL;

	if(old_path_list) {
		tmp = old_path_list;
		while(tmp) {
			node = tmp->data;
			x1 = (node->co*11)+67;
			y1 = (node->ro*19.4)+44;
			x2 = (node->cd*11)+67;
			y2 = (node->rd*19.4)+44;
			g_free(node);

			if(x1 <= x2)
				update_rect.x = x1-1;
			else
				update_rect.x = x2-1;
			if(y1 <= y2)
				update_rect.y = y1-1;
			else
				update_rect.y = y2-1;

			update_rect.height = abs(y1-y2) + 2;
			update_rect.width = abs(x1-x2) + 2;

			gdk_draw_line(board_buf,
				      gc_line,
				      x1, y1,
				      x2, y2);

			gtk_widget_draw(draw_area, &update_rect);

			tmp = g_slist_next(tmp);
		}

		g_slist_free(old_path_list);
	}

	tmp = path_list;
	while(tmp) {
		node = tmp->data;
		x1 = (node->co*11)+67;
		y1 = (node->ro*19.4)+44;
		x2 = (node->cd*11)+67;
		y2 = (node->rd*19.4)+44;

		if(x1 <= x2)
			update_rect.x = x1;
		else
			update_rect.x = x2;
		if(y1 <= y2)
			update_rect.y = y1;
		else
			update_rect.y = y2;

		update_rect.height = abs(y1-y2) + 1;
		update_rect.width = abs(x1-x2) + 1;

		gdk_draw_line(board_buf,
			      gc_line,
			      x1, y1,
			      x2, y2);

		gtk_widget_draw(draw_area, &update_rect);

		tmp = g_slist_next(tmp);
	}

	old_path_list = path_list;
}


void display_set_name(int p, char *p_name)
{
	int pos;

	pos = homes[game.players-1][p];
	gtk_label_set_text(GTK_LABEL(label[pos]), p_name);
}


void display_set_label_colors(void)
{
	int i, j, l_index;
	GdkColormap *sys_colormap;
	GdkColor color;
	GtkStyle *label_style;

	/* Setup the styles for our name labels */
	label[0] = gtk_object_get_data(GTK_OBJECT(dlg_main), "p1_label");
	label[1] = gtk_object_get_data(GTK_OBJECT(dlg_main), "p2_label");
	label[2] = gtk_object_get_data(GTK_OBJECT(dlg_main), "p3_label");
	label[3] = gtk_object_get_data(GTK_OBJECT(dlg_main), "p4_label");
	label[4] = gtk_object_get_data(GTK_OBJECT(dlg_main), "p5_label");
	label[5] = gtk_object_get_data(GTK_OBJECT(dlg_main), "p6_label");

	sys_colormap = gdk_colormap_get_system();
	for(i=0; i<6; i++) {
		GdkFont *font;
		l_index = homes[game.players-1][i];
		if(l_index == -1)
			break;
		gdk_color_parse(label_color[i], &color);
		gdk_colormap_alloc_color(sys_colormap, &color, FALSE, TRUE);
		label_style = gtk_style_new();
		font = gdk_font_load("fixed");
#ifdef GTK2
		gtk_style_set_font(label_style, font);
#else
		label_style->font = font;
#endif
		for(j=0; j<5; j++)
			label_style->fg[j] = color;
		gtk_widget_set_style(label[l_index], label_style);
	}
}
