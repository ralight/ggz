#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

#include "callbacks.h"
#include "main_win.h"
#include "support.h"
#include "board.h"

extern GdkPixmap *board_buf;


void
on_exit_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


gboolean
on_board_configure_event               (GtkWidget       *widget,
                                        GdkEventConfigure *event,
                                        gpointer         user_data)
{
  if (!board_buf)
    board_buf = gdk_pixmap_new( widget->window, PIXSIZE*8, PIXSIZE*8, -1);

  board_draw_bg();

  return TRUE;
}


gboolean
on_board_expose_event                  (GtkWidget       *widget,
                                        GdkEventExpose  *event,
                                        gpointer         user_data)
{
  gdk_draw_pixmap( widget->window,
      widget->style->fg_gc[GTK_WIDGET_STATE(widget)],
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

  return FALSE;
}

