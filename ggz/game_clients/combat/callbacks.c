#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>
#include <stdio.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"
#include "game.h"
#include "combat.h"

extern GdkPixmap *cbt_buf;
extern combat_game cbt_game;
extern GtkWidget *main_win;

gboolean
on_main_window_configure_event         (GtkWidget       *widget,
                                        GdkEventConfigure *event,
                                        gpointer         user_data)
{
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


gboolean
on_mainarea_configure_event            (GtkWidget       *widget,
                                        GdkEventConfigure *event,
                                        gpointer         user_data)
{
	if (!cbt_buf)
		cbt_buf = gdk_pixmap_new( widget->window, cbt_game.width * (PIXSIZE+1)+1,
														  cbt_game.height * (PIXSIZE+1)+1,
															-1);
	game_draw_bg();
	game_draw_board();

  return FALSE;
}


gboolean
on_mainarea_button_press_event         (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
	int x = event->x/(PIXSIZE+1) + 1;
	int y = event->y/(PIXSIZE+1) + 1;


	switch (cbt_game.state) {
		case CBT_STATE_WAIT:
			if (!cbt_game.map)
				break;
		case CBT_STATE_SETUP:
			game_handle_setup(CART(x,y,cbt_game.width));
			break;
		case CBT_STATE_PLAYING:
			game_handle_move(CART(x,y,cbt_game.width));
			break;
	}

  return FALSE;
}

void callback_sendbutton_set_enabled(gboolean mode) {
	GtkWidget *button;
	printf("Enablind/Disabling send button (%d)\n", mode);
	button = gtk_object_get_data(GTK_OBJECT(main_win), "send_setup");
	gtk_widget_set_sensitive (button, mode);
	if (mode == FALSE)
		gtk_widget_hide(button);
	else
		gtk_widget_show(button);
}

void
on_send_setup_clicked                  (GtkButton       *button,
                                        gpointer         user_data)
{
	game_send_setup();

}

