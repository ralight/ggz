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

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <gtk/gtk.h>
#include <ggz.h>	/* libggz */

#include "display.h"
#include "main_dlg.h"
#include "game.h"


GtkWidget *dlg_main = NULL;
static GtkWidget *draw_area = NULL;
static GtkWidget *statusbar;
static guint sb_context;
static GtkStyle *draw_area_style;
static GdkPixmap *board_buf;

static GdkPixbuf *board_img;
static GdkPixbuf *hole_img;
static GdkPixbuf *marble_img[6];

static GdkGC *gc_line;
static GtkWidget *label[6];
static char *label_color[6] = {
	/* Labels are in the color of their piece, but 50% darker so they'll be
	   legible. */
	"#800000",
	"#000080",
	"#008000",
	"#808000",
	"#008080",
	"#800080"
};

static gchar *colors[6] = { "red", "blue", "green",
	"yellow", "cyan", "purple"
};

static int BOARD_SIZE = 400;

/* Integer value. */
#define HOLE_SIZE ((int)(BOARD_SIZE * 12.5 / 400))

/* Floating-point values. */
#define OFFSET_X ((float)BOARD_SIZE * 11.0 / 400.0)
#define ORIGIN_X ((float)BOARD_SIZE * 61.0 / 400.0)
#define OFFSET_Y ((float)BOARD_SIZE * 19.4 / 400.0)
#define ORIGIN_Y ((float)BOARD_SIZE * 38.0 / 400.0)

#define GET_X(i) ((float)i * OFFSET_X + ORIGIN_X)
#define GET_Y(j) ((float)j * OFFSET_Y + ORIGIN_Y)

static GdkPixbuf *display_load_pixmap(const char *name, int size)
{
	char *fullpath;
	GdkPixbuf *image;
	GError *error = NULL;

	fullpath = g_strdup_printf("%s/%s.svg", get_theme_dir(), name);
	image = gdk_pixbuf_new_from_file_at_size(fullpath,
						 size, size, &error);
	free(fullpath);
	if (image)
		return image;

	fullpath = g_strdup_printf("%s/%s.png", get_theme_dir(), name);
	error = NULL;
	image = gdk_pixbuf_new_from_file(fullpath, &error);
	if (image == NULL) {
		ggz_error_msg("Can't load pixmap %s", fullpath);
		/* non-fatal error */
	}
	g_free(fullpath);

	return image;
}

static void draw(GdkPixbuf * image, int x, int y, int w, int h)
{
	GdkGC *gc = draw_area_style->fg_gc[GTK_WIDGET_STATE(draw_area)];

	gdk_draw_pixbuf(board_buf, gc, image,
			0, 0, x, y, w, h, GDK_RGB_DITHER_NONE, 0, 0);
}

/* Initialize the game display (dlg_main and board) */
int display_init(void)
{
	GdkColormap *sys_colormap;
	GdkColor color;
	int i;

	/* Create and display the main dialog */
	if (dlg_main == NULL) {
		dlg_main = create_dlg_main();
		gtk_widget_show(dlg_main);
	}

	/* Get and store a pointer to our drawingarea and statusbar */
	draw_area = g_object_get_data(G_OBJECT(dlg_main), "draw_area");
	draw_area_style = gtk_widget_get_style(draw_area);
	statusbar = g_object_get_data(G_OBJECT(dlg_main), "statusbar1");
	sb_context = gtk_statusbar_get_context_id(GTK_STATUSBAR(statusbar),
						  "Game Messages");

	/* Create a pixmap buffer from our xpm */
	board_img = display_load_pixmap("board", BOARD_SIZE);
	if (!board_img)
		return -1;

	board_buf = gdk_pixmap_new(draw_area->window,
				   draw_area->allocation.width,
				   draw_area->allocation.height, -1);
	gdk_draw_rectangle(board_buf,
			   draw_area->style->black_gc, TRUE, 0, 0,
			   draw_area->allocation.width,
			   draw_area->allocation.height);

	/* Convert the rest of our xpms to masked pixmaps */
	hole_img = display_load_pixmap("hole", HOLE_SIZE);
	if (!hole_img)
		return -1;

	for (i = 0; i < 6; i++) {
		marble_img[i] = display_load_pixmap(colors[i], HOLE_SIZE);
		if (marble_img[i] == NULL)
			return -1;
	}

	/* Setup the gc for our line drawing */
	gc_line = gdk_gc_new(draw_area->window);
	sys_colormap = gdk_colormap_get_system();
	gdk_color_parse("White", &color);
	gdk_colormap_alloc_color(sys_colormap, &color, FALSE, TRUE);
	gdk_gc_set_foreground(gc_line, &color);
	gdk_gc_set_function(gc_line, GDK_XOR);
	gdk_gc_set_line_attributes(gc_line, 2, GDK_LINE_SOLID,
				   GDK_CAP_BUTT, GDK_JOIN_ROUND);

	display_resized();

	return 0;
}


void display_resized(void)
{
	int w, h, i;

	/* HACK: must be realized before used. */
	if (!draw_area) {
		return;
	}

	if (board_buf) {
		g_object_unref(board_buf);
	}

	if (board_img) {
		g_object_unref(board_img);
		g_object_unref(hole_img);
		for (i = 0; i < 6; i++) {
			g_object_unref(marble_img[i]);
		}
	}

	w = draw_area->allocation.width;
	h = draw_area->allocation.height;

	board_buf = gdk_pixmap_new(draw_area->window, w, h, -1);

	board_img = display_load_pixmap("board", MIN(w, h));

	/* If the above load resulted in a smaller board, adapt to the
	 * change. */
	BOARD_SIZE = gdk_pixbuf_get_width(board_img);

	hole_img = display_load_pixmap("hole", HOLE_SIZE);
	assert(hole_img != NULL);
	for (i = 0; i < 6; i++) {
		marble_img[i] = display_load_pixmap(colors[i], HOLE_SIZE);
		assert(marble_img[i] != NULL);
	}

	display_refresh_board();
}


/* Redrsaw board areas that just got exposed */
void display_handle_expose_event(GdkEventExpose * event)
{
	if (draw_area)
		gdk_draw_drawable(draw_area->window,
				  draw_area_style->
				  fg_gc[GTK_WIDGET_STATE(draw_area)],
				  board_buf, event->area.x, event->area.y,
				  event->area.x, event->area.y,
				  event->area.width, event->area.height);
}


/* Refresh the board completely from the game.board image */
void display_refresh_board(void)
{
	int i, j;
	int w = draw_area->allocation.width;
	int h = draw_area->allocation.height;

	/* Clear a path if one is showing */
	display_show_path(NULL);

	gdk_draw_rectangle(board_buf,
			   draw_area->style->black_gc, TRUE, 0, 0, w, h);
	draw(board_img, 0, 0, BOARD_SIZE, BOARD_SIZE);

	/* Refresh the marbles */
	assert(game.board[0][0] == BOARD_NONE);
	for (i = 0; i < ROWS; i++)
		for (j = 0; j < COLS; j++) {
			GdkPixbuf *img;

			/* Hole or invalid */
			if (game.board[i][j] == BOARD_NONE) {
				continue;
			} else if (game.board[i][j] == BOARD_EMPTY) {
				img = hole_img;
			} else {
				img = marble_img[game.board[i][j] - 1];
			}

			/* Plop a marble */
			draw(img, GET_X(j), GET_Y(i), HOLE_SIZE,
			     HOLE_SIZE);
		}

	/* Draw our pixmap buffer */
	gdk_draw_drawable(draw_area->window,
			  draw_area_style->
			  fg_gc[GTK_WIDGET_STATE(draw_area)], board_buf, 0,
			  0, 0, 0, w, h);
}


void display_handle_click_event(GdkEventButton * event)
{
	int row, col;

	/* Convert to an array row/col and pass to game module */
	col = ((float)event->x - ORIGIN_X) / OFFSET_X;
	row = ((float)event->y - ORIGIN_Y) / OFFSET_Y;

	/* ggz_debug("main", "[%d][%d]", row, col); */

	game_handle_click_event(row, col);
}


void display_statusbar(char *msg)
{
	static int firsttime = 1;

	if (firsttime)
		firsttime = 0;
	else
		gtk_statusbar_pop(GTK_STATUSBAR(statusbar), sb_context);

	gtk_statusbar_push(GTK_STATUSBAR(statusbar), sb_context, msg);
}


/* Not really animation, just draws a line from src to dest */
void display_show_path(GSList * path_list)
{
	struct node_t *node;
	int x1, y1, x2, y2;
	GSList *tmp;
	static GSList *old_path_list = NULL;

	if (old_path_list) {
		tmp = old_path_list;
		while (tmp) {
			node = tmp->data;
			x1 = GET_X(node->co) + HOLE_SIZE / 2;
			y1 = GET_Y(node->ro) + HOLE_SIZE / 2;
			x2 = GET_X(node->cd) + HOLE_SIZE / 2;
			y2 = GET_Y(node->rd) + HOLE_SIZE / 2;
			g_free(node);

			gdk_draw_line(board_buf, gc_line, x1, y1, x2, y2);
			gtk_widget_queue_draw_area(draw_area,
						   MIN(x1, x2) - 1,
						   MIN(y1, y2) - 1,
						   abs(x1 - x2) + 2,
						   abs(y1 - y2) + 1);

			tmp = g_slist_next(tmp);
		}

		g_slist_free(old_path_list);
	}

	tmp = path_list;
	while (tmp) {
		node = tmp->data;
		x1 = GET_X(node->co) + HOLE_SIZE / 2;
		y1 = GET_Y(node->ro) + HOLE_SIZE / 2;
		x2 = GET_X(node->cd) + HOLE_SIZE / 2;
		y2 = GET_Y(node->rd) + HOLE_SIZE / 2;

		gdk_draw_line(board_buf, gc_line, x1, y1, x2, y2);
		gtk_widget_queue_draw_area(draw_area,
					   MIN(x1, x2),
					   MIN(y1, y2),
					   abs(x1 - x2) + 1,
					   abs(y1 - y2) + 1);

		tmp = g_slist_next(tmp);
	}

	old_path_list = path_list;
}


void display_set_name(int p, char *p_name)
{
	int pos;

	pos = homes[game.players - 1][p];
	gtk_label_set_text(GTK_LABEL(label[pos]), p_name);
}


void display_set_label_colors(void)
{
	int i, j, l_index;
	GdkColormap *sys_colormap;
	GdkColor color;
	GtkStyle *label_style;

	/* Setup the styles for our name labels */
	label[0] = g_object_get_data(G_OBJECT(dlg_main), "p1_label");
	label[1] = g_object_get_data(G_OBJECT(dlg_main), "p2_label");
	label[2] = g_object_get_data(G_OBJECT(dlg_main), "p3_label");
	label[3] = g_object_get_data(G_OBJECT(dlg_main), "p4_label");
	label[4] = g_object_get_data(G_OBJECT(dlg_main), "p5_label");
	label[5] = g_object_get_data(G_OBJECT(dlg_main), "p6_label");

	sys_colormap = gdk_colormap_get_system();
	for (i = 0; i < 6; i++) {
		l_index = homes[game.players - 1][i];
		if (l_index == -1)
			break;
		gdk_color_parse(label_color[i], &color);
		gdk_colormap_alloc_color(sys_colormap, &color, FALSE,
					 TRUE);
		label_style = gtk_style_new();
		for (j = 0; j < 5; j++)
			label_style->fg[j] = color;
		gtk_widget_set_style(label[l_index], label_style);
	}
}
