#include <gtk/gtk.h>

#include <string.h>

#include <ggz.h>

#include "ggzintl.h"

#include "game.h"
#include "selector.h"

/* Global game variables */
extern struct game_state_t game;

/* Combo box for map selection */
static GtkWidget *list;

/* Map information box */
static GtkWidget *information;

/* List of maps */
static struct hastings_map_t *maplist;
static int mapcount;

static void activated(GtkWidget *w, gpointer data)
{
	const gchar *s;
	char buffer[256];
	struct hastings_map_t map;
	int i;

	s = gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(list)->entry));

	for(i = 0; i < mapcount; i++)
	{
		map = maplist[i];
		if(!strcmp(map.title, s))
		{
			g_snprintf(buffer, sizeof(buffer), _("Name: %s\nAuthor:%s\nVersion:%s\nWidth:%i\nHeight:%i\nPlayers: unknown\n"),
				map.title, map.author, map.version, map.width, map.height);
			gtk_text_set_point(GTK_TEXT(information), 0);
			gtk_text_forward_delete(GTK_TEXT(information), gtk_text_get_length(GTK_TEXT(information)));
			gtk_text_insert(GTK_TEXT(information), NULL, NULL, NULL, buffer, -1);
		}
	}
}

static void selected(GtkWidget *w, gpointer data)
{
	const gchar *s;

	s = gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(list)->entry));

	ggz_write_int(game.fd, HASTINGS_SND_MAP);
	ggz_write_string(game.fd, s);

	gtk_widget_destroy(data);
}

GtkWidget *selector()
{
	GtkWidget *window;
	GtkWidget *button;
	GtkWidget *box;
	GtkWidget *label;
	GList *maps;

	int count, i, j;

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_container_set_border_width(GTK_CONTAINER(window), 10);

	box = gtk_vbox_new(FALSE, 0);

	label = gtk_label_new(_("Please select a map:"));

	information = gtk_text_new(NULL, NULL);
	gtk_text_set_editable(GTK_TEXT(information), 0);

	button = gtk_button_new_with_label("Use map");

	list = gtk_combo_new();

	maps = NULL;
	ggz_read_int(game.fd, &count);
	maplist = malloc(count * sizeof(struct hastings_map_t));
	mapcount = count;

	for(i = 0; i < count; i++)
	{
		ggz_read_string_alloc(game.fd, &maplist[i].title);
		ggz_read_string_alloc(game.fd, &maplist[i].author);
		ggz_read_string_alloc(game.fd, &maplist[i].version);
		ggz_read_int(game.fd, &maplist[i].width);
		ggz_read_int(game.fd, &maplist[i].height);
		for(j = 0; j < maplist[i].height; j++)
			ggz_readn(game.fd, &maplist[i].board[j], 30);
		for(j = 0; j < maplist[i].height; j++)
			ggz_readn(game.fd, &maplist[i].boardmap[j], 30);
	}

	for(i = 0; i < mapcount; i++)
		maps = g_list_append(maps, maplist[i].title);
	gtk_combo_set_popdown_strings(GTK_COMBO(list), maps);
	g_list_free(maps);

	gtk_signal_connect(GTK_OBJECT(GTK_COMBO(list)->entry), "changed", GTK_SIGNAL_FUNC(activated), NULL);
	gtk_signal_connect(GTK_OBJECT(button), "clicked", GTK_SIGNAL_FUNC(selected), window);
	/*gtk_signal_connect_swapped(G_OBJECT(button), "clicked", G_CALLBACK(destroyed), G_OBJECT(window));*/
	activated(NULL, NULL);

	gtk_box_pack_start(GTK_BOX(box), label, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(box), list, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(box), information, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(box), button, TRUE, TRUE, 0);

	gtk_container_add(GTK_CONTAINER(window), box);

	gtk_widget_show(label);
	gtk_widget_show(list);
	gtk_widget_show(information);
	gtk_widget_show(button);
	gtk_widget_show(box);
	gtk_widget_show(window);

	return window;
}

