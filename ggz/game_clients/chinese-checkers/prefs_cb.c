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
	GtkWidget *toggle;
	GtkWidget *tree = lookup_widget(dlg_prefs, "theme_list");
	GtkTreeSelection *select
	  = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree));
	GtkTreeModel *model;
	GtkTreeIter iter;

	if (gtk_tree_selection_get_selected(select, &model, &iter)) {
		gtk_tree_model_get(model, &iter, THEME_COLUMN_NAME,
				   &theme, -1);
	} else {
		theme = g_strdup("default");
	}

	toggle = g_object_get_data(G_OBJECT(dlg_prefs), "check_beep");

	if(GTK_TOGGLE_BUTTON(toggle)->active)
		beep = 1;
	else
		beep = 0;

	game_update_config(theme, beep);

	g_free(theme);
}


void
on_cancel_button_clicked               (GtkButton       *button,
                                        gpointer         user_data)
{
	gtk_widget_destroy(dlg_prefs);
}

void on_theme_list_select_changed(GtkTreeSelection *select, gpointer data)
{

}
