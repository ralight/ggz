#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>

#include "dlg_main.h"
#include "support.h"
#include "easysock.h"
#include "main.h"
#include "game.h"

guint8 vert_board[MAX_BOARD_WIDTH][MAX_BOARD_HEIGHT-1];
guint8 horz_board[MAX_BOARD_WIDTH-1][MAX_BOARD_HEIGHT];
gint8 owners_board[MAX_BOARD_WIDTH-1][MAX_BOARD_HEIGHT-1];
guint8 board_height, board_width;

/* Private variables */
static GdkPixmap *board_pixmap;
static gfloat dot_width, dot_height;
static GdkGC *gc_fg, *gc_bg, *gc_p1, *gc_p2, *gc_p1b, *gc_p2b;
static GtkWidget *board, *statusbar;
static guint sb_context;

/* Private functions */
static gint8 board_move(guint8, guint8, guint8);
static void board_fill_square(guint8, guint8);


void board_init(guint8 width, guint8 height)
{
	guint8 i, j;
	guint16 x, y;
	GdkColormap *sys_colormap;
	GdkColor color;
	GtkWidget *p1b, *p2b;
	GdkRectangle update_rect;

	board = gtk_object_get_data(GTK_OBJECT(main_win), "board");
	statusbar = gtk_object_get_data(GTK_OBJECT(main_win), "statusbar");
	p1b = gtk_object_get_data(GTK_OBJECT(main_win), "p1b");
	p2b = gtk_object_get_data(GTK_OBJECT(main_win), "p2b");

	if(board_pixmap)
		gdk_pixmap_unref(board_pixmap);

	board_pixmap = gdk_pixmap_new(board->window,
				      board->allocation.width,
				      board->allocation.height,
				      -1);

	if(gc_fg == NULL) {
		gc_fg = gdk_gc_new(board->window);
		gc_bg = gdk_gc_new(board->window);
		gc_p1 = gdk_gc_new(board->window);
		gc_p2 = gdk_gc_new(board->window);
		gc_p1b = gdk_gc_new(board->window);
		gc_p2b = gdk_gc_new(board->window);
	}

	sys_colormap = gdk_colormap_get_system();
	gdk_color_parse("RGB:00/50/00", &color);
	gdk_colormap_alloc_color(sys_colormap, &color, FALSE, TRUE);
	gdk_gc_set_background(gc_fg, &color);
	gdk_gc_set_background(gc_bg, &color);
	gdk_gc_set_background(gc_p1, &color);
	gdk_gc_set_background(gc_p2, &color);
	gdk_gc_set_foreground(gc_bg, &color);
	gdk_color_parse("RGB:FF/FF/00", &color);
	gdk_colormap_alloc_color(sys_colormap, &color, FALSE, TRUE);
	gdk_gc_set_foreground(gc_fg, &color);
	gdk_color_parse("RGB:FF/00/00", &color);
	gdk_colormap_alloc_color(sys_colormap, &color, FALSE, TRUE);
	gdk_gc_set_foreground(gc_p1, &color);
	gdk_gc_set_foreground(gc_p1b, &color);
	gdk_color_parse("RGB:00/00/FF", &color);
	gdk_colormap_alloc_color(sys_colormap, &color, FALSE, TRUE);
	gdk_gc_set_foreground(gc_p2, &color);
	gdk_gc_set_foreground(gc_p2b, &color);

	gdk_draw_rectangle(board_pixmap,
			   gc_bg,
			   TRUE,
			   0, 0,
			   board->allocation.width, board->allocation.height);

	if(width == 0 || height == 0)
		return;

	if(width > MAX_BOARD_WIDTH)
		width = MAX_BOARD_WIDTH;
	if(height > MAX_BOARD_HEIGHT)
		height = MAX_BOARD_HEIGHT;
	board_width = width;
	board_height = height;

	dot_width = board->allocation.width / (float)(board_width+1);
	dot_height = board->allocation.height / (float)(board_height+1);
	for(i=0; i<board_width; i++)
		for(j=0; j<board_height; j++) {
			x = (i+1) * dot_width;
			y = (j+1) * dot_height;
			gdk_draw_point(board_pixmap,
				       gc_fg,
				       x, y);
		}

	update_rect.x = 0;
	update_rect.y = 0;
	update_rect.width = board->allocation.width;
	update_rect.height = board->allocation.height;
	gtk_widget_draw(board, &update_rect);

	game.move = -1;

	sb_context = gtk_statusbar_get_context_id(GTK_STATUSBAR(statusbar),
						  "Game Messages");
	gtk_statusbar_push(GTK_STATUSBAR(statusbar),
			   sb_context,
			   "Waiting for server");
}


void board_handle_expose_event(GtkWidget *widget, GdkEventExpose *event)
{
	gdk_draw_pixmap(widget->window,
			widget->style->fg_gc[GTK_WIDGET_STATE(widget)],
			board_pixmap,
			event->area.x, event->area.y,
			event->area.x, event->area.y,
			event->area.width, event->area.height);
}


void board_handle_click(GtkWidget *widget, GdkEventButton *event)
{
	gint8 line_x, line_y, dist_x, dist_y;
	gint8 left, top;
	gint8 result;
	guint16 x1, y1, x2, y2;
	GdkRectangle update_rect;
	gchar *tstr;

	if(game.state != DOTS_STATE_MOVE) {
		statusbar_message("Wait for your turn to move");
		return;
	}

	/* Figure out what lines we are closest to horiz and vert */
	line_x = (int) (event->x / dot_width +.5) - 1;
	dist_x = event->x - (line_x+1)*dot_width;
	line_y = (int) (event->y / dot_height +.5) - 1;
	dist_y = event->y - (line_y+1)*dot_height;

	/* Just quit if we can't choose one */
	if(line_x < 0 || line_x >= board_width
	   || line_y < 0 || line_y >= board_height
	   || abs(dist_x) == abs(dist_y))
		return;

	if(abs(dist_x) < abs(dist_y)) {
		/* Vertical Line */
		if(dist_y < 0)
			top = line_y-1;
		else
			top = line_y;
		if(top < 0 || top+2 > board_height)
			return;

		if((result = board_move(0, line_x, top)) == -1) {
			gdk_beep();
			return;
		}

		x1 = x2 = (line_x+1) * dot_width;
		y1 = (top+1) * dot_height;
		y2 = (top+2) * dot_height;
		update_rect.x = x1;
		update_rect.y = y1;
		update_rect.width = 1;
		update_rect.height = dot_height+1;
		if(es_write_int(game.fd, DOTS_SND_MOVE_V) < 0
		   || es_write_char(game.fd, line_x) < 0
		   || es_write_char(game.fd, top) < 0) {
			fprintf(stderr, "Lost server connection\n");
			exit(1);
		}
	} else {
		/* Horizontal Line */
		if(dist_x < 0)
			left = line_x-1;
		else
			left = line_x;
		if(left < 0 || left+2 > board_width)
			return;

		if((result = board_move(1, left, line_y)) == -1) {
			gdk_beep();
			return;
		}

		y1 = y2 = (line_y+1) * dot_height;
		x1 = (left+1) * dot_width;
		x2 = (left+2) * dot_width;
		update_rect.x = x1;
		update_rect.y = y1;
		update_rect.width = dot_width+1;
		update_rect.height = 1;
		if(es_write_int(game.fd, DOTS_SND_MOVE_H) < 0
		   || es_write_char(game.fd, left) < 0
		   || es_write_char(game.fd, line_y) < 0) {
			fprintf(stderr, "Lost server connection\n");
			exit(1);
		}
	}

	gdk_draw_line(board_pixmap,
		      gc_fg,
		      x1, y1,
		      x2, y2);
	gtk_widget_draw(widget, &update_rect);

	if(result <= 0) {
		tstr = g_strconcat("Waiting for ",
				   game.names[game.opponent], "...", NULL);
		statusbar_message(tstr);
		g_free(tstr);
		game.move = game.opponent;
		game.state = DOTS_STATE_OPPONENT;
	} else {
		statusbar_message("Waiting for server response");
		game.state = DOTS_STATE_WAIT;
	}
}


gint8 board_move(guint8 dir, guint8 x, guint8 y)
{
	gint8 result=0;
	GtkWidget *l1, *l2;
	char *text;

	if(dir == 0) {
		/* A vertical move */
		if(vert_board[x][y])
			return -1;
		vert_board[x][y] = 1;
		if(x != 0)
			if(vert_board[x-1][y]
			   && horz_board[x-1][y]
			   && horz_board[x-1][y+1]) {
				/* Winning square */
				board_fill_square(x-1, y);
				result++;
			}
		if(x != board_width-1)
			if(vert_board[x+1][y]
			   && horz_board[x][y]
			   && horz_board[x][y+1]) {
				/* Winning square */
				board_fill_square(x, y);
				result++;
			}
	} else {
		if(horz_board[x][y])
			return -1;
		horz_board[x][y] = 1;
		if(y != 0)
			if(horz_board[x][y-1]
			   && vert_board[x][y-1]
			   && vert_board[x+1][y-1]) {
				/* Winning square */
				board_fill_square(x, y-1);
				result++;
			}
		if(y != board_height-1)
			if(horz_board[x][y+1]
			   && vert_board[x][y]
			   && vert_board[x+1][y]) {
				/* Winning square */
				board_fill_square(x, y);
				result++;
			}
	}

	if(result > 0) {
                game.score[(int)game.move] += result;
                l1 = gtk_object_get_data(GTK_OBJECT(main_win), "lbl_score0");
                l2 = gtk_object_get_data(GTK_OBJECT(main_win), "lbl_score1");
                text = g_strdup_printf("Score = %d", game.score[0]);
                gtk_label_set_text(GTK_LABEL(l1), text);
                g_free(text);
                text = g_strdup_printf("Score = %d", game.score[1]);
                gtk_label_set_text(GTK_LABEL(l2), text);
                g_free(text);
	}

	return result;
}


gint8 board_opponent_move(guint8 dir)
{
	gint8 result=0;
	guchar x, y;
	guint16 x1, y1, x2, y2;
	GdkRectangle update_rect;
	GtkWidget *l1, *l2;
	char *text;
	char t_s, t_x, t_y;
	int i;

	if(es_read_char(game.fd, &x) < 0
	   || es_read_char(game.fd, &y) < 0
	   || es_read_char(game.fd, &t_s) < 0)
		return -1;
	for(i=0; i<t_s; i++) {
		if(es_read_char(game.fd, &t_x) < 0
		   || es_read_char(game.fd, &t_y) < 0)
			return -1;
	}

	/* Future DOTS_REQ_MOVE's now make sense */
	if(game.state == DOTS_STATE_OPPONENT)
		game.state = DOTS_STATE_WAIT;

	if(dir == 0) {
		/* A vertical move */
		x1 = x2 = (x+1) * dot_width;
		y1 = (y+1) * dot_height;
		y2 = (y+2) * dot_height;
		update_rect.x = x1;
		update_rect.y = y1;
		update_rect.width = 1;
		update_rect.height = dot_height+1;
		gdk_draw_line(board_pixmap,
			      gc_fg,
			      x1, y1,
			      x2, y2);
		gtk_widget_draw(board, &update_rect);

		vert_board[x][y] = 1;
		if(x != 0)
			if(vert_board[x-1][y]
			   && horz_board[x-1][y]
			   && horz_board[x-1][y+1]) {
				/* Winning square */
				board_fill_square(x-1, y);
				result++;
			}
		if(x != board_width-1)
			if(vert_board[x+1][y]
			   && horz_board[x][y]
			   && horz_board[x][y+1]) {
				/* Winning square */
				board_fill_square(x, y);
				result++;
			}
	} else {
		y1 = y2 = (y+1) * dot_height;
		x1 = (x+1) * dot_width;
		x2 = (x+2) * dot_width;
		update_rect.x = x1;
		update_rect.y = y1;
		update_rect.width = dot_width+1;
		update_rect.height = 1;
		gdk_draw_line(board_pixmap,
			      gc_fg,
			      x1, y1,
			      x2, y2);
		gtk_widget_draw(board, &update_rect);

		horz_board[x][y] = 1;
		if(y != 0)
			if(horz_board[x][y-1]
			   && vert_board[x][y-1]
			   && vert_board[x+1][y-1]) {
				/* Winning square */
				board_fill_square(x, y-1);
				result++;
			}
		if(y != board_height-1)
			if(horz_board[x][y+1]
			   && vert_board[x][y]
			   && vert_board[x+1][y]) {
				/* Winning square */
				board_fill_square(x, y);
				result++;
			}
	}

	if(result > 0) {
                game.score[(int)game.move] += result;
                l1 = gtk_object_get_data(GTK_OBJECT(main_win), "lbl_score0");
                l2 = gtk_object_get_data(GTK_OBJECT(main_win), "lbl_score1");
                text = g_strdup_printf("Score = %d", game.score[0]);
                gtk_label_set_text(GTK_LABEL(l1), text);
                g_free(text);
                text = g_strdup_printf("Score = %d", game.score[1]);
                gtk_label_set_text(GTK_LABEL(l2), text);
                g_free(text);
	}

	return 0;
}


void board_fill_square(guint8 x, guint8 y)
{
	guint16 x1, y1;
	GdkRectangle update_rect;

	x1 = (x+1) * dot_width + 1;
	y1 = (y+1) * dot_height + 1;
	update_rect.x = x1;
	update_rect.y = y1;
	update_rect.width = dot_width-1;
	update_rect.height = dot_height-1;

	if(game.move == 0)
		gdk_draw_rectangle(board_pixmap,
				   gc_p1,
				   TRUE,
				   x1, y1,
				   dot_width-1, dot_height-1);
	else
		gdk_draw_rectangle(board_pixmap,
				   gc_p2,
				   TRUE,
				   x1, y1,
				   dot_width-1, dot_height-1);

	gtk_widget_draw(board, &update_rect);
}


void statusbar_message(char *msg)
{
	gtk_statusbar_pop(GTK_STATUSBAR(statusbar), sb_context);
	gtk_statusbar_push(GTK_STATUSBAR(statusbar), sb_context, msg);
}


void board_handle_pxb_expose(void)
{
	GtkWidget *p1b, *p2b;

	/* Draw them both even if just one got exposed */
	p1b = gtk_object_get_data(GTK_OBJECT(main_win), "p1b");
	p2b = gtk_object_get_data(GTK_OBJECT(main_win), "p2b");

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
	GdkRectangle update_rect;
	GdkGC *gc_ptr;

	/* Draw all known vertical lines */
	for(i=0; i<board_width; i++)
		for(j=0; j<board_height-1; j++)
			if(vert_board[i][j]) {
				x1 = x2 = (i+1) * dot_width;
				y1 = (j+1) * dot_height;
				y2 = (j+2) * dot_height;
				gdk_draw_line(board_pixmap,
			      		gc_fg,
			      		x1, y1,
			      		x2, y2);
			}

	/* Draw all known horizontal lines */
	for(i=0; i<board_width-1; i++)
		for(j=0; j<board_height; j++)
			if(horz_board[i][j]) {
				y1 = y2 = (j+1) * dot_height;
				x1 = (i+1) * dot_width;
				x2 = (i+2) * dot_width;
				gdk_draw_line(board_pixmap,
			      		gc_fg,
			      		x1, y1,
			      		x2, y2);
			}

	/* Draw all the squares in their proper colors */
	for(i=0; i<board_width-1; i++)
		for(j=0; j<board_height-1; j++) {
			switch(owners_board[i][j]) {
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

			if(gc_ptr) {
				x1 = (i+1) * dot_width + 1;
				y1 = (j+1) * dot_height + 1;
				gdk_draw_rectangle(board_pixmap,
						   gc_ptr,
						   TRUE,
						   x1, y1,
						   dot_width-1, dot_height-1);
			}
		}

	/* Finally, update the on-screen pixmap */
	update_rect.x = 0;
	update_rect.y = 0;
	update_rect.width = board->allocation.width;
	update_rect.height = board->allocation.height;
	gtk_widget_draw(board, &update_rect);
}
