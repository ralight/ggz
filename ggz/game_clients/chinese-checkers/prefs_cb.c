#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

#include "prefs_cb.h"
#include "prefs_dlg.h"
#include "support.h"
#include "game.h"
#include "main_cb.h"


void
on_browse_button_clicked               (GtkButton       *button,
                                        gpointer         user_data)
{

}


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
	char *pixmap_dir;
	int beep;
	GtkWidget *entry, *toggle;

	entry = gtk_object_get_data(GTK_OBJECT(dlg_prefs), "dir_entry");
	toggle = gtk_object_get_data(GTK_OBJECT(dlg_prefs), "check_beep");

	pixmap_dir = gtk_entry_get_text(GTK_ENTRY(entry));
	if(GTK_TOGGLE_BUTTON(toggle)->active)
		beep = 1;
	else
		beep = 0;
	game_update_config(pixmap_dir, beep);
}


void
on_cancel_button_clicked               (GtkButton       *button,
                                        gpointer         user_data)
{
	gtk_widget_destroy(dlg_prefs);
}


void
on_default_button_clicked              (GtkButton       *button,
                                        gpointer         user_data)
{
	char *tmp;
	GtkWidget *entry;

	entry = gtk_object_get_data(GTK_OBJECT(dlg_prefs), "dir_entry");
	tmp = g_strdup_printf("%s/ccheckers/pixmaps/default", GAMEDIR);
	gtk_entry_set_text(GTK_ENTRY(entry), tmp);
	g_free(tmp);
}

