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
static GdkPixmap *board_buf;

static GdkPixbuf *board_img;
static GdkPixbuf *hole_img;
static GdkPixbuf *marble_img[6];

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


static GdkPixbuf *display_load_pixmap(const char *name)
{
	char *fullpath;
	GdkPixbuf *image;
	GError *error = NULL;

	fullpath = g_strdup_printf("%s/%s.png", get_theme_dir(), name);
	image = gdk_pixbuf_new_from_file(fullpath, &error);
	if(image == NULL) {
		ggz_error_msg("Can't load pixmap %s", fullpath);
		/* non-fatal error */
	}
	g_free(fullpath);

	return image;
}

static void draw(GdkPixbuf *image, int x, int y, int w, int h)
{
	GdkGC *gc = draw_area_style->fg_gc[GTK_WIDGET_STATE(draw_area)];

	gdk_pixbuf_render_to_drawable(image, board_buf, gc,
				      0, 0, x, y, w, h,
				      GDK_RGB_DITHER_NONE, 0, 0);
}

/* Initialize the game display (dlg_main and board) */
int display_init(void)
{
	GdkColormap *sys_colormap;
	GdkColor color;
	gchar *colors[6] = {"red", "blue", "green",
			    "yellow", "cyan", "purple"};
	int i;

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
	board_img = display_load_pixmap("board");
	if (!board_img)
		return -1;

	board_buf = gdk_pixmap_new(draw_area->window,
				   draw_area->allocation.width,
				   draw_area->allocation.height,
				   -1);
	draw(board_img, 0, 0, 400, 400);

	/* Convert the rest of our xpms to masked pixmaps */
	hole_img = display_load_pixmap("hole");
	if (!hole_img)
		return -1;

	for (i = 0; i < 6; i++) {
		marble_img[i] = display_load_pixmap(colors[i]);
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
			draw(hole_img, x, y, 12, 12);
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
			draw(marble_img[p], x, y, 12, 12);
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
		gtk_style_set_font(label_style, font);
		for(j=0; j<5; j++)
			label_style->fg[j] = color;
		gtk_widget_set_style(label[l_index], label_style);
	}
}
