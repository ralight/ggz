/*
 * File: game.c
 * Author: Rich Gade
 * Project: GGZ Connect the Dots Client
 * Date: 08/14/2000
 * Desc: Routines to manipulate the CtD board
 * $Id: game.c 7273 2005-06-10 12:54:26Z josef $
 *
 * Copyright (C) 2000, 2001 Brent Hendricks.
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

#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>

#include <ggz.h>

#include "dlg_main.h"
#include "support.h"
#include "main.h"
#include "game.h"
#include "ggzintl.h"

guint8 vert_board[MAX_BOARD_WIDTH][MAX_BOARD_HEIGHT - 1];
guint8 horz_board[MAX_BOARD_WIDTH - 1][MAX_BOARD_HEIGHT];
gint8 owners_board[MAX_BOARD_WIDTH - 1][MAX_BOARD_HEIGHT - 1];
guint8 board_height, board_width;
GdkColor fg_color, bg_color, p1_color, p2_color;

/* Private variables */
static GdkPixmap *board_pixmap;
static gfloat dot_width, dot_height;
static GdkGC *gc_fg, *gc_bg, *gc_p1, *gc_p2, *gc_p1b, *gc_p2b;
static GtkWidget *board, *statusbar;
static guint sb_context = 0;

/* Private functions */
static gint8 board_move(guint8, guint8, guint8);
static void board_fill_square(guint8, guint8);


void board_init(guint8 width, guint8 height)
{
	guint8 i, j;
	guint16 x, y;
	GdkColormap *sys_colormap;
	GtkWidget *p1b, *p2b;

	board = g_object_get_data(G_OBJECT(main_win), "board");
	p1b = g_object_get_data(G_OBJECT(main_win), "p1b");
	p2b = g_object_get_data(G_OBJECT(main_win), "p2b");

	if (board_pixmap)
		g_object_unref(board_pixmap);

	board_pixmap = gdk_pixmap_new(board->window,
				      board->allocation.width,
				      board->allocation.height, -1);

	if (gc_fg == NULL) {
		gc_fg = gdk_gc_new(board->window);
		gc_bg = gdk_gc_new(board->window);
		gc_p1 = gdk_gc_new(board->window);
		gc_p2 = gdk_gc_new(board->window);
		gc_p1b = gdk_gc_new(board->window);
		gc_p2b = gdk_gc_new(board->window);
	}

	sys_colormap = gdk_colormap_get_system();
	bg_color.red = ggz_conf_read_int(conf_handle,
					 "BackgroundColor", "Red", 0);
	bg_color.green = ggz_conf_read_int(conf_handle,
					   "BackgroundColor",
					   "Green", 32768);
	bg_color.blue = ggz_conf_read_int(conf_handle,
					  "BackgroundColor", "Blue", 0);
	fg_color.red = ggz_conf_read_int(conf_handle,
					 "ForegroundColor", "Red", 65535);
	fg_color.green = ggz_conf_read_int(conf_handle,
					   "ForegroundColor",
					   "Green", 65535);
	fg_color.blue = ggz_conf_read_int(conf_handle,
					  "ForegroundColor", "Blue", 0);
	p1_color.red = ggz_conf_read_int(conf_handle,
					 "PlayerOneColor", "Red", 65535);
	p1_color.green = ggz_conf_read_int(conf_handle,
					   "PlayerOneColor", "Green", 0);
	p1_color.blue = ggz_conf_read_int(conf_handle,
					  "PlayerOneColor", "Blue", 0);
	p2_color.red = ggz_conf_read_int(conf_handle,
					 "PlayerTwoColor", "Red", 0);
	p2_color.green = ggz_conf_read_int(conf_handle,
					   "PlayerTwoColor", "Green", 0);
	p2_color.blue = ggz_conf_read_int(conf_handle,
					  "PlayerTwoColor", "Blue", 65535);
	gdk_colormap_alloc_color(sys_colormap, &bg_color, FALSE, TRUE);
	gdk_gc_set_background(gc_fg, &bg_color);
	gdk_gc_set_background(gc_bg, &bg_color);
	gdk_gc_set_background(gc_p1, &bg_color);
	gdk_gc_set_background(gc_p2, &bg_color);
	gdk_gc_set_foreground(gc_bg, &bg_color);
	gdk_colormap_alloc_color(sys_colormap, &fg_color, FALSE, TRUE);
	gdk_gc_set_foreground(gc_fg, &fg_color);
	gdk_colormap_alloc_color(sys_colormap, &p1_color, FALSE, TRUE);
	gdk_gc_set_foreground(gc_p1, &p1_color);
	gdk_gc_set_foreground(gc_p1b, &p1_color);
	gdk_colormap_alloc_color(sys_colormap, &p2_color, FALSE, TRUE);
	gdk_gc_set_foreground(gc_p2, &p2_color);
	gdk_gc_set_foreground(gc_p2b, &p2_color);

	gdk_draw_rectangle(board_pixmap,
			   gc_bg,
			   TRUE,
			   0, 0,
			   board->allocation.width,
			   board->allocation.height);

	if (width == 0 || height == 0)
		return;

	if (width > MAX_BOARD_WIDTH)
		width = MAX_BOARD_WIDTH;
	if (height > MAX_BOARD_HEIGHT)
		height = MAX_BOARD_HEIGHT;
	board_width = width;
	board_height = height;

	dot_width = board->allocation.width / (float)(board_width + 1);
	dot_height = board->allocation.height / (float)(board_height + 1);
	for (i = 0; i < board_width; i++)
		for (j = 0; j < board_height; j++) {
			x = (i + 1) * dot_width;
			y = (j + 1) * dot_height;
			gdk_draw_point(board_pixmap, gc_fg, x, y);
		}

	gtk_widget_queue_draw(board);

	game.move = -1;


	statusbar_message(_("Waiting for server"));
}


void board_handle_expose_event(GtkWidget * widget, GdkEventExpose * event)
{
	gdk_draw_drawable(widget->window,
			  widget->style->fg_gc[GTK_WIDGET_STATE(widget)],
			  board_pixmap,
			  event->area.x, event->area.y,
			  event->area.x, event->area.y,
			  event->area.width, event->area.height);
}


void board_handle_click(GtkWidget * widget, GdkEventButton * event)
{
	gint8 line_x, line_y, dist_x, dist_y;
	gint8 left, top;
	gint8 result;
	guint16 x1, y1, x2, y2;
	gchar *tstr;

	if (game.state != DOTS_STATE_MOVE) {
		statusbar_message(_("Wait for your turn to move"));
		return;
	}

	/* Figure out what lines we are closest to horiz and vert */
	line_x = (int)(event->x / dot_width + .5) - 1;
	dist_x = event->x - (line_x + 1) * dot_width;
	line_y = (int)(event->y / dot_height + .5) - 1;
	dist_y = event->y - (line_y + 1) * dot_height;

	/* Just quit if we can't choose one */
	if (line_x < 0 || line_x >= board_width
	    || line_y < 0 || line_y >= board_height
	    || abs(dist_x) == abs(dist_y))
		return;

	if (abs(dist_x) < abs(dist_y)) {
		/* Vertical Line */
		if (dist_y < 0)
			top = line_y - 1;
		else
			top = line_y;
		if (top < 0 || top + 2 > board_height)
			return;

		if ((result = board_move(0, line_x, top)) == -1) {
			gdk_beep();
			return;
		}

		x1 = x2 = (line_x + 1) * dot_width;
		y1 = (top + 1) * dot_height;
		y2 = (top + 2) * dot_height;
		gtk_widget_queue_draw_area(widget, x1, y1, 1,
					   dot_height + 1);
		if (ggz_write_int(game.fd, DOTS_SND_MOVE_V) < 0
		    || ggz_write_char(game.fd, line_x) < 0
		    || ggz_write_char(game.fd, top) < 0) {
			ggz_error_msg(_("Lost server connection"));
			exit(1);
		}
	} else {
		/* Horizontal Line */
		if (dist_x < 0)
			left = line_x - 1;
		else
			left = line_x;
		if (left < 0 || left + 2 > board_width)
			return;

		if ((result = board_move(1, left, line_y)) == -1) {
			gdk_beep();
			return;
		}

		y1 = y2 = (line_y + 1) * dot_height;
		x1 = (left + 1) * dot_width;
		x2 = (left + 2) * dot_width;
		gtk_widget_queue_draw_area(widget, x1, y1, dot_width + 1,
					   1);
		if (ggz_write_int(game.fd, DOTS_SND_MOVE_H) < 0
		    || ggz_write_char(game.fd, left) < 0
		    || ggz_write_char(game.fd, line_y) < 0) {
			ggz_error_msg(_("Lost server connection"));
			exit(1);
		}
	}

	gdk_draw_line(board_pixmap, gc_fg, x1, y1, x2, y2);

	if (result <= 0) {
		tstr = g_strconcat(_("Waiting for "),
				   game.names[game.opponent], "...", NULL);
		statusbar_message(tstr);
		g_free(tstr);
		game.move = game.opponent;
		game.state = DOTS_STATE_OPPONENT;
	} else {
		statusbar_message(_("Waiting for server response"));
		game.state = DOTS_STATE_WAIT;
	}
}


gint8 board_move(guint8 dir, guint8 x, guint8 y)
{
	gint8 result = 0;
	GtkWidget *l1, *l2;
	char *text;

	if (dir == 0) {
		/* A vertical move */
		if (vert_board[x][y])
			return -1;
		vert_board[x][y] = 1;
		if (x != 0)
			if (vert_board[x - 1][y]
			    && horz_board[x - 1][y]
			    && horz_board[x - 1][y + 1]) {
				/* Winning square */
				owners_board[x - 1][y] = 0;
				board_fill_square(x - 1, y);
				result++;
			}
		if (x != board_width - 1)
			if (vert_board[x + 1][y]
			    && horz_board[x][y]
			    && horz_board[x][y + 1]) {
				/* Winning square */
				owners_board[x][y] = 0;
				board_fill_square(x, y);
				result++;
			}
	} else {
		if (horz_board[x][y])
			return -1;
		horz_board[x][y] = 1;
		if (y != 0)
			if (horz_board[x][y - 1]
			    && vert_board[x][y - 1]
			    && vert_board[x + 1][y - 1]) {
				/* Winning square */
				owners_board[x][y - 1] = 0;
				board_fill_square(x, y - 1);
				result++;
			}
		if (y != board_height - 1)
			if (horz_board[x][y + 1]
			    && vert_board[x][y]
			    && vert_board[x + 1][y]) {
				/* Winning square */
				owners_board[x][y] = 0;
				board_fill_square(x, y);
				result++;
			}
	}

	if (result > 0) {
		game.score[(int)game.move] += result;
		l1 = g_object_get_data(G_OBJECT(main_win), "lbl_score0");
		l2 = g_object_get_data(G_OBJECT(main_win), "lbl_score1");
		text = g_strdup_printf(_("Score = %d"), game.score[0]);
		gtk_label_set_text(GTK_LABEL(l1), text);
		g_free(text);
		text = g_strdup_printf(_("Score = %d"), game.score[1]);
		gtk_label_set_text(GTK_LABEL(l2), text);
		g_free(text);
	}

	return result;
}


gint8 board_opponent_move(guint8 dir)
{
	gint8 result = 0;
	guchar x, y;
	guint16 x1, y1, x2, y2;
	GtkWidget *l1, *l2;
	char *text;
	char t_s, t_x, t_y;
	int i;

	if (ggz_read_char(game.fd, (char*)&x) < 0
	    || ggz_read_char(game.fd, (char*)&y) < 0
	    || ggz_read_char(game.fd, &t_s) < 0)
		return -1;
	for (i = 0; i < t_s; i++) {
		if (ggz_read_char(game.fd, &t_x) < 0
		    || ggz_read_char(game.fd, &t_y) < 0)
			return -1;
	}

	/* Future DOTS_REQ_MOVE's now make sense */
	if (game.state == DOTS_STATE_OPPONENT)
		game.state = DOTS_STATE_WAIT;

	if (dir == 0) {
		/* A vertical move */
		x1 = x2 = (x + 1) * dot_width;
		y1 = (y + 1) * dot_height;
		y2 = (y + 2) * dot_height;
		gdk_draw_line(board_pixmap, gc_fg, x1, y1, x2, y2);
		gtk_widget_queue_draw_area(board, x1, y1, 1,
					   dot_height + 1);

		vert_board[x][y] = 1;
		if (x != 0)
			if (vert_board[x - 1][y]
			    && horz_board[x - 1][y]
			    && horz_board[x - 1][y + 1]) {
				/* Winning square */
				owners_board[x - 1][y] = 1;
				board_fill_square(x - 1, y);
				result++;
			}
		if (x != board_width - 1)
			if (vert_board[x + 1][y]
			    && horz_board[x][y]
			    && horz_board[x][y + 1]) {
				/* Winning square */
				owners_board[x][y] = 1;
				board_fill_square(x, y);
				result++;
			}
	} else {
		y1 = y2 = (y + 1) * dot_height;
		x1 = (x + 1) * dot_width;
		x2 = (x + 2) * dot_width;
		gdk_draw_line(board_pixmap, gc_fg, x1, y1, x2, y2);
		gtk_widget_queue_draw_area(board, x1, y1, dot_width + 1,
					   1);

		horz_board[x][y] = 1;
		if (y != 0)
			if (horz_board[x][y - 1]
			    && vert_board[x][y - 1]
			    && vert_board[x + 1][y - 1]) {
				/* Winning square */
				owners_board[x][y - 1] = 1;
				board_fill_square(x, y - 1);
				result++;
			}
		if (y != board_height - 1)
			if (horz_board[x][y + 1]
			    && vert_board[x][y]
			    && vert_board[x + 1][y]) {
				/* Winning square */
				owners_board[x][y] = 1;
				board_fill_square(x, y);
				result++;
			}
	}

	if (result > 0) {
		game.score[(int)game.move] += result;
		l1 = g_object_get_data(G_OBJECT(main_win), "lbl_score0");
		l2 = g_object_get_data(G_OBJECT(main_win), "lbl_score1");
		text = g_strdup_printf(_("Score = %d"), game.score[0]);
		gtk_label_set_text(GTK_LABEL(l1), text);
		g_free(text);
		text = g_strdup_printf(_("Score = %d"), game.score[1]);
		gtk_label_set_text(GTK_LABEL(l2), text);
		g_free(text);
	}

	return 0;
}


void board_fill_square(guint8 x, guint8 y)
{
	guint16 x1, y1;

	x1 = (x + 1) * dot_width + 1;
	y1 = (y + 1) * dot_height + 1;

	if (game.move == 0)
		gdk_draw_rectangle(board_pixmap,
				   gc_p1,
				   TRUE,
				   x1, y1, dot_width - 1, dot_height - 1);
	else
		gdk_draw_rectangle(board_pixmap,
				   gc_p2,
				   TRUE,
				   x1, y1, dot_width - 1, dot_height - 1);

	gtk_widget_queue_draw_area(board, x1, y1,
				   dot_width - 1, dot_height - 1);
}


void statusbar_message(char *msg)
{
	if (statusbar == NULL) {
		statusbar =
		    g_object_get_data(G_OBJECT(main_win), "statusbar");
		sb_context =
		    gtk_statusbar_get_context_id(GTK_STATUSBAR(statusbar),
						 _("Game Messages"));
	}

	gtk_statusbar_pop(GTK_STATUSBAR(statusbar), sb_context);
	gtk_statusbar_push(GTK_STATUSBAR(statusbar), sb_context, msg);
}


void board_handle_pxb_expose(void)
{
	GtkWidget *p1b, *p2b;

	/* Draw them both even if just one got exposed */
	p1b = g_object_get_data(G_OBJECT(main_win), "p1b");
	p2b = g_object_get_data(G_OBJECT(main_win), "p2b");

	gdk_draw_rectangle(p1b->window,
			   gc_p1b,
			   TRUE,
			   0, 0,
			   p1b->allocation.width, p1b->allocation.height);
	gdk_draw_rectangle(p2b->window,
			   gc_p2b,
			   TRUE,
			   0, 0,
			   p2b->allocation.width, p2b->allocation.height);
}


void board_redraw(void)
{
	guint8 i, j;
	guint16 x1, y1, x2, y2;
	GdkGC *gc_ptr;

	gdk_draw_rectangle(board_pixmap,
			   gc_bg,
			   TRUE,
			   0, 0,
			   board->allocation.width,
			   board->allocation.height);
	for (i = 0; i < board_width; i++)
		for (j = 0; j < board_height; j++) {
			x1 = (i + 1) * dot_width;
			y1 = (j + 1) * dot_height;
			gdk_draw_point(board_pixmap, gc_fg, x1, y1);
		}

	/* Draw all known vertical lines */
	for (i = 0; i < board_width; i++)
		for (j = 0; j < board_height - 1; j++)
			if (vert_board[i][j]) {
				x1 = x2 = (i + 1) * dot_width;
				y1 = (j + 1) * dot_height;
				y2 = (j + 2) * dot_height;
				gdk_draw_line(board_pixmap,
					      gc_fg, x1, y1, x2, y2);
			}

	/* Draw all known horizontal lines */
	for (i = 0; i < board_width - 1; i++)
		for (j = 0; j < board_height; j++)
			if (horz_board[i][j]) {
				y1 = y2 = (j + 1) * dot_height;
				x1 = (i + 1) * dot_width;
				x2 = (i + 2) * dot_width;
				gdk_draw_line(board_pixmap,
					      gc_fg, x1, y1, x2, y2);
			}

	/* Draw all the squares in their proper colors */
	for (i = 0; i < board_width - 1; i++)
		for (j = 0; j < board_height - 1; j++) {
			switch (owners_board[i][j]) {
			case 0:
				gc_ptr = gc_p1b;
				break;
			case 1:
				gc_ptr = gc_p2b;
				break;
			default:
				gc_ptr = NULL;
				break;
			}

			if (gc_ptr) {
				x1 = (i + 1) * dot_width + 1;
				y1 = (j + 1) * dot_height + 1;
				gdk_draw_rectangle(board_pixmap,
						   gc_ptr,
						   TRUE,
						   x1, y1,
						   dot_width - 1,
						   dot_height - 1);
			}
		}

	/* Finally, update the on-screen pixmap */
	gtk_widget_queue_draw(board);
}


void game_apply_colors(GdkColor new_fg, GdkColor new_bg,
		       GdkColor new_p1, GdkColor new_p2)
{
	GdkColormap *sys_colormap;

	sys_colormap = gdk_colormap_get_system();

	gdk_colormap_alloc_color(sys_colormap, &new_bg, FALSE, TRUE);
	gdk_gc_set_background(gc_fg, &new_bg);
	gdk_gc_set_background(gc_bg, &new_bg);
	gdk_gc_set_background(gc_p1, &new_bg);
	gdk_gc_set_background(gc_p2, &new_bg);
	gdk_gc_set_foreground(gc_bg, &new_bg);
	gdk_colormap_alloc_color(sys_colormap, &new_fg, FALSE, TRUE);
	gdk_gc_set_foreground(gc_fg, &new_fg);
	gdk_colormap_alloc_color(sys_colormap, &new_p1, FALSE, TRUE);
	gdk_gc_set_foreground(gc_p1, &new_p1);
	gdk_gc_set_foreground(gc_p1b, &new_p1);
	gdk_colormap_alloc_color(sys_colormap, &new_p2, FALSE, TRUE);
	gdk_gc_set_foreground(gc_p2, &new_p2);
	gdk_gc_set_foreground(gc_p2b, &new_p2);

	board_redraw();
	board_handle_pxb_expose();
}


void game_write_colors(void)
{
	ggz_conf_write_int(conf_handle,
			   "BackgroundColor", "Red", bg_color.red);
	ggz_conf_write_int(conf_handle,
			   "BackgroundColor", "Green", bg_color.green);
	ggz_conf_write_int(conf_handle,
			   "BackgroundColor", "Blue", bg_color.blue);
	ggz_conf_write_int(conf_handle,
			   "ForegroundColor", "Red", fg_color.red);
	ggz_conf_write_int(conf_handle,
			   "ForegroundColor", "Green", fg_color.green);
	ggz_conf_write_int(conf_handle,
			   "ForegroundColor", "Blue", fg_color.blue);
	ggz_conf_write_int(conf_handle,
			   "PlayerOneColor", "Red", p1_color.red);
	ggz_conf_write_int(conf_handle,
			   "PlayerOneColor", "Green", p1_color.green);
	ggz_conf_write_int(conf_handle,
			   "PlayerOneColor", "Blue", p1_color.blue);
	ggz_conf_write_int(conf_handle,
			   "PlayerTwoColor", "Red", p2_color.red);
	ggz_conf_write_int(conf_handle,
			   "PlayerTwoColor", "Green", p2_color.green);
	ggz_conf_write_int(conf_handle,
			   "PlayerTwoColor", "Blue", p2_color.blue);

	ggz_conf_commit(conf_handle);
}
