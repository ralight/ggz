#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

#include "cb_main.h"
#include "dlg_main.h"
#include "support.h"
#include "table.h"


void
on_mnu_exit_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
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
on_dlg_main_delete_event               (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
	gtk_main_quit();

	return FALSE;
}


gboolean
on_fixed1_button_press_event           (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
	if(event->button == 1)
		table_handle_click_event(event);

	return FALSE;
}


gboolean
on_fixed1_expose_event                 (GtkWidget       *widget,
                                        GdkEventExpose  *event,
                                        gpointer         user_data)
{
	table_handle_expose_event(event);

	return FALSE;
}

