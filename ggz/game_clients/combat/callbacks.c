#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"
#include "game.h"
#include "combat.h"

extern GdkPixmap *cbt_buf;
extern combat_game cbt_game;

gboolean
on_main_window_configure_event         (GtkWidget       *widget,
                                        GdkEventConfigure *event,
                                        gpointer         user_data)
{
	if (!cbt_buf)
		cbt_buf = gdk_pixmap_new( widget->window, cbt_game.width * PIXSIZE,
														  cbt_game.height * PIXSIZE,
															-1);
	game_draw_bg();
	game_draw_board();

  return FALSE;
}


void
on_main_window_draw                    (GtkWidget       *widget,
                                        GdkRectangle    *area,
                                        gpointer         user_data)
{

}


gboolean
on_mainarea_expose_event               (GtkWidget       *widget,
                                        GdkEventExpose  *event,
                                        gpointer         user_data)
{

	gdk_draw_pixmap( widget->window, widget->style->fg_gc[GTK_WIDGET_STATE(widget)], cbt_buf, event->area.x, event->area.y, event->area.x, event->area.y,
			event->area.width, event->area.height);

  return FALSE;
}


gboolean
main_window_exit                       (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
	// FIXME: Should call a "are you sure" dialog
	gtk_main_quit();

  return FALSE;
}



void
on_exit_menu_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

	main_window_exit(NULL, NULL, NULL);

}

