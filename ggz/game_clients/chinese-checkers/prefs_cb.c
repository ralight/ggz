#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

#include "prefs_cb.h"
#include "prefs_dlg.h"
#include "support.h"
#include "game.h"
#include "main_cb.h"


int sel;

void
on_ok_button_clicked                   (GtkButton       *button,
                                        gpointer         user_data)
{
	on_apply_button_clicked(button, user_data);

	gtk_widget_destroy(dlg_prefs);
}


void
on_apply_button_clicked                (GtkButton       *button,
                                        gpointer         user_data)
{
	char *theme;
	int beep;
	GtkWidget *list, *toggle;

	list = g_object_get_data(G_OBJECT(dlg_prefs), "theme_list");
	toggle = g_object_get_data(G_OBJECT(dlg_prefs), "check_beep");

	gtk_clist_get_text(GTK_CLIST(list), sel, 0, &theme);

	if(GTK_TOGGLE_BUTTON(toggle)->active)
		beep = 1;
	else
		beep = 0;

	game_update_config(theme, beep);
}


void
on_cancel_button_clicked               (GtkButton       *button,
                                        gpointer         user_data)
{
	gtk_widget_destroy(dlg_prefs);
}

void
on_theme_list_select_row               (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
	sel = row;
}

