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
#include "popup.h"

extern game_t *game;

extern struct chess_info game_info;

extern GtkWidget *main_win;

extern GdkPixmap *board_buf;

extern GtkTargetEntry *target;


gboolean
ExitDialog                             (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
  ggz_show_exit_dialog(0);
  return TRUE;
}

void
on_exit_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  ggz_show_exit_dialog(1);
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
	char output[80];
	cgc_getboard(output, game->board);
  x = event->x / PIXSIZE;
  y = event->y / PIXSIZE;
  /* Set the last pressed data */
  gtk_object_set_data(GTK_OBJECT(widget), "from_x", GINT_TO_POINTER(x));
  gtk_object_set_data(GTK_OBJECT(widget), "from_y", GINT_TO_POINTER(y));

  if (output[x+(y*9)] == '-' || (game_info.seat == 1 && output[x+(y*9)] < 'Z' ) || (game_info.seat == 0 && !(output[x+(y*9)] < 'Z')))
    gtk_drag_source_unset(widget);
  else
    gtk_drag_source_set(widget, GDK_BUTTON1_MASK, target, 1, GDK_ACTION_MOVE);


  return FALSE;
}

void
on_board_drag_begin                    (GtkWidget       *widget,
                                        GdkDragContext  *drag_context,
                                        gpointer         user_data)
{
  int x, y;
  x = GPOINTER_TO_INT(gtk_object_get_data(GTK_OBJECT(widget), "from_x"));
  y = GPOINTER_TO_INT(gtk_object_get_data(GTK_OBJECT(widget), "from_y"));
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
  int f_x, f_y, t_x, t_y, retval;
  f_x = GPOINTER_TO_INT(gtk_object_get_data(GTK_OBJECT(widget), "from_x"));
  f_y = GPOINTER_TO_INT(gtk_object_get_data(GTK_OBJECT(widget), "from_y"));
  t_x = x / PIXSIZE;
  t_y = y / PIXSIZE;
  retval = cgc_valid_move(game, f_x, 7-f_y, t_x, 7-t_y, 0);
  if (retval == VALID) {
    game_info.dest_x = t_x;
    game_info.dest_y = t_y;
  } else {
    game_info.dest_x = -1;
    game_info.dest_y = -1;
  }
  board_draw();

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
  char move[6];
	char output[80];
  int promote = 0;
	cgc_getboard(output, game->board);

  arg[0] = GPOINTER_TO_INT(gtk_object_get_data(GTK_OBJECT(widget), "from_x"));
  arg[1] = GPOINTER_TO_INT(gtk_object_get_data(GTK_OBJECT(widget), "from_y"));

  arg[2] = x / PIXSIZE;
  arg[3] = y / PIXSIZE;

  /* Should we promote ? */
  if (game_info.seat == 0) {
    /* We are white ! So we must go from 6 to 7 */
    if (arg[1] == 6 && arg[3] == 7 && output[arg[0]+(arg[1]*10)] == 'P')
      promote = 1;
  } else {
    /* We are black ! So we must go from 1 to 0 */
    if (arg[1] == 1 && arg[3] == 0 && output[arg[0]*(arg[1]*10)] == 'p')
      promote = 1;
  }

  move[0] = 65 + arg[0];
  move[1] = 49 + (7-arg[1]);
  move[2] = 65 + arg[2];
  move[3] = 49 + (7-arg[3]);
  move[4] = promote;
  move[5] = 0;

  if (!promote && cgc_valid_move(game, arg[0], 7-arg[1], arg[2], 7-arg[3], 0) == VALID) {
    game_update(CHESS_EVENT_MOVE_END, move);
  } else if (promote) {
    gtk_object_set_data(GTK_OBJECT(main_win), "promote", g_strdup(move));
    gtk_widget_show(create_promote_dialog());
  }

  gtk_drag_source_unset(widget);

  game_info.src_x = -1;
  game_info.src_y = -1;
  game_info.dest_x = -1;
  game_info.dest_y = -1;

  board_draw();


  return FALSE;
}

void
on_auto_call_flag_activate             (GtkMenuItem     *menuitem,
                                        gpointer         user_data) {
}
