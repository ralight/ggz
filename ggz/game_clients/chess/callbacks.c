#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

#include <stdio.h>
#include "callbacks.h"
#include "chess.h"
#include "board.h"
#include "game.h"
#include "libcgc/cgc.h"
#include "main_win.h"
#include "support.h"

extern game_t *game;

extern GdkPixmap *board_buf;

extern GtkTargetEntry *target;

void
on_exit_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  gtk_main_quit();

}


gboolean
on_board_configure_event               (GtkWidget       *widget,
                                        GdkEventConfigure *event,
                                        gpointer         user_data)
{
  if (!board_buf)
    board_buf = gdk_pixmap_new( widget->window,
                  PIXSIZE*8,
                  PIXSIZE*8,
                  -1);

  board_draw();


  return FALSE;
}


gboolean
on_board_expose_event                  (GtkWidget       *widget,
                                        GdkEventExpose  *event,
                                        gpointer         user_data)
{
	gdk_draw_pixmap( widget->window,
			 widget->style->fg_gc[GTK_WIDGET_STATE (widget)],
			 board_buf,
			 event->area.x, event->area.y,
			 event->area.x, event->area.y,
			 event->area.width, event->area.height);

  return FALSE;
}


gboolean
on_board_button_press_event            (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
  int x, y;
  x = event->x / PIXSIZE;
  y = event->y / PIXSIZE;
  /* Set the last pressed data */
  gtk_object_set_data(GTK_OBJECT(widget), "last_from_x", GINT_TO_POINTER(x));
  gtk_object_set_data(GTK_OBJECT(widget), "last_from_y", GINT_TO_POINTER(y));

  if (game->board[x][y] != EMPTY)
    gtk_drag_source_set(widget, GDK_BUTTON1_MASK, target, 1, GDK_ACTION_MOVE);


  return FALSE;
}

void
on_board_drag_begin                    (GtkWidget       *widget,
                                        GdkDragContext  *drag_context,
                                        gpointer         user_data)
{
  int x, y;
  x = GPOINTER_TO_INT(gtk_object_get_data(GTK_OBJECT(widget), "last_from_x"));
  y = GPOINTER_TO_INT(gtk_object_get_data(GTK_OBJECT(widget), "last_from_y"));
  board_dnd_highlight(x, y, drag_context);

}

gboolean
on_board_drag_motion                   (GtkWidget       *widget,
                                        GdkDragContext  *drag_context,
                                        gint             x,
                                        gint             y,
                                        guint            time,
                                        gpointer         user_data)
{
  /* FIXME: Highlight possible moves */

  return FALSE;
}


gboolean
on_board_drag_drop                     (GtkWidget       *widget,
                                        GdkDragContext  *drag_context,
                                        gint             x,
                                        gint             y,
                                        guint            time,
                                        gpointer         user_data)
{
  int arg[4];

  arg[0] = GPOINTER_TO_INT(gtk_object_get_data(GTK_OBJECT(widget), "last_from_x"));
  arg[1] = GPOINTER_TO_INT(gtk_object_get_data(GTK_OBJECT(widget), "last_from_y"));

  arg[2] = x / PIXSIZE;
  arg[3] = y / PIXSIZE;

  game_update(CHESS_EVENT_MOVE_END, arg);

  gtk_drag_source_unset(widget);


  return FALSE;
}

