#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

#include "main_cb.h"
#include "main_dlg.h"
#include "support.h"
#include "display.h"


gboolean
on_dlg_main_delete_event               (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
	gtk_main_quit();
	return FALSE;
}


void
on_exit_menu_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	gtk_main_quit();
}


gboolean
on_draw_area_expose_event              (GtkWidget       *widget,
                                        GdkEventExpose  *event,
                                        gpointer         user_data)
{
	display_handle_expose_event(event);
	return FALSE;
}


gboolean
on_draw_area_button_press_event        (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
	if(event->button == 1)
		display_handle_click_event(event);
	return TRUE;
}

