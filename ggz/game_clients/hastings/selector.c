#include <gtk/gtk.h>

#include "ggzintl.h"

static GtkWidget *list;

static void selected(GtkWidget *w, gpointer data)
{
	const gchar *s;

	s = gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(list)->entry));

	/* ... */
}

static void destroyed(GtkWidget *w, gpointer data)
{
	/* ... */
}

GtkWidget *selector()
{
	GtkWidget *window;
	GtkWidget *button;
	GtkWidget *box;
	GtkWidget *label;
	GList *maps;

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_container_set_border_width(GTK_CONTAINER(window), 10);

	box = gtk_vbox_new(FALSE, 0);

	label = gtk_label_new(_("Please select a map:"));

	button = gtk_button_new_with_label("Use map");

	list = gtk_combo_new();
	maps = NULL;
	maps = g_list_append(maps, _("Invasion Of Sweyn Forkbeard"));
	maps = g_list_append(maps, _("Earl Godwin Vs. Alfred In The City Of Ely"));
	maps = g_list_append(maps, _("Slaughter In The Normandy"));
	maps = g_list_append(maps, _("Melee At Dover"));
	maps = g_list_append(maps, _("Revolt At Northampton"));
	maps = g_list_append(maps, _("Fyrdmen At Fulford Gate"));
	maps = g_list_append(maps, _("YorkShire, Stamford Bridge"));
	maps = g_list_append(maps, _("The Battle Of Hastings"));
	gtk_combo_set_popdown_strings(GTK_COMBO(list), maps);
	g_list_free(maps);

	gtk_signal_connect(GTK_OBJECT(button), "clicked", GTK_SIGNAL_FUNC(selected), NULL);
	/*gtk_signal_connect_swapped(G_OBJECT(button), "clicked", G_CALLBACK(destroyed), G_OBJECT(window));*/

	gtk_box_pack_start(GTK_BOX(box), label, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(box), list, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(box), button, TRUE, TRUE, 0);

	gtk_container_add(GTK_CONTAINER(window), box);

	gtk_widget_show(label);
	gtk_widget_show(list);
	gtk_widget_show(button);
	gtk_widget_show(box);
	gtk_widget_show(window);

	return window;
}

