#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdio.h>
#include <gtk/gtk.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"
#include "game.h"


void
on_mnu_resign_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_mnu_exit_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	/* Are you sure? / Cleanup */
	/* See also on_window_delete_event */
	gtk_main_quit();
}


void
on_mnu_preferences_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_mnu_about_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}

gboolean
on_window_delete_event                 (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
	/* Are you sure? / Cleanup */
	/* See also on_mnu_exit_activate */
	gtk_main_quit();
	return FALSE;
}

gboolean
on_board_expose_event                  (GtkWidget       *widget,
                                        GdkEventExpose  *event,
                                        gpointer         user_data)
{
	board_handle_expose_event(widget, event);
	return FALSE;
}


gboolean
on_board_button_press_event            (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
	if(event->button == 1)
		board_handle_click(widget, event);

	return TRUE;
}


gboolean
on_p1b_expose_event                    (GtkWidget       *widget,
                                        GdkEventExpose  *event,
                                        gpointer         user_data)
{
	board_handle_pxb_expose();
	return FALSE;
}


gboolean
on_p2b_expose_event                    (GtkWidget       *widget,
                                        GdkEventExpose  *event,
                                        gpointer         user_data)
{
	board_handle_pxb_expose();
	return FALSE;
}

