/* 
 * File: main.c
 * Author: Rich Gade, Jason Short
 * Project: GGZCards Client
 * Date: 08/14/2000
 * Desc: Main loop and core logic
 * $Id: main.c 6734 2005-01-19 01:58:21Z jdorje $
 *
 * Copyright (C) 2000-2002 Brent Hendricks.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <assert.h>
#include <gtk/gtk.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>

#include <ggz.h>	/* libggz */

#include "dlg_about.h"
#include "dlg_chat.h"
#include "dlg_players.h"
#include "ggzintl.h"
#include "ggz_gtk.h"
#include "menus.h"

#include "client.h"

#include "animation.h"
#include "dlg_main.h"
#include "dlg_bid.h"
#include "drawcard.h"
#include "game.h"
#include "layout.h"
#include "main.h"
#include "preferences.h"
#include "table.h"

GtkWidget *dlg_main = NULL;

static void initialize_debugging(void);
static void cleanup_debugging(void);
static void init_about_dialog(void);

static const char *font = "fixed";

int main(int argc, char *argv[])
{
	int ggz_fd;
	GIOChannel *channel;

	/* Standard initializations. */
	initialize_debugging();
	ggz_intl_init("ggzcards");
	ggz_fd = client_initialize();
	channel = g_io_channel_unix_new(ggz_fd);
	g_io_add_watch(channel, G_IO_IN, game_handle_ggz, NULL);
	gtk_init(&argc, &argv);
	load_preferences();

	/* This shouldn't go here, but I see no better place right now. The
	   message windows are supposed to use a fixed-width font. */
	fixed_font = pango_font_description_new();
	pango_font_description_set_family(fixed_font, font);

	/* Now some more initializations... */
	init_player_list(client_get_ggzmod());
	init_chat(client_get_ggzmod());
	init_about_dialog();
	dlg_main = create_dlg_main();
	ggz_game_main_window = GTK_WINDOW(dlg_main);	/* HACK */
	gtk_widget_show(dlg_main);
	table_initialize();
	game_init();

	/* Now run... */
	gtk_main();

	/* Now clean up and shut down. */
	ggz_debug(DBG_MAIN, "Cleaning up and quitting.");
	client_quit();
	table_cleanup();

	save_preferences();
	ggz_conf_cleanup();

	cleanup_debugging();
	return 0;
}

/* This function is called at the start of the program to initialize what
   types of debugging we're doing. */
static void initialize_debugging(void)
{
	/* Our debugging code uses libggz's ggz_debug() function, so we just
	   initialize the _types_ of debugging we want. */
#ifdef DEBUG
	const char *debugging_types[] =
	    { DBG_MAIN, DBG_TABLE, DBG_ANIM, DBG_CLIENT, NULL };
#else
	const char *debugging_types[] = { NULL };
#endif
	/* Debugging goes to ~/.ggz/ggzcards-gtk.debug */
	char *file_name =
	    g_strdup_printf("%s/.ggz/ggzcards-gtk.debug", getenv("HOME"));
	ggz_debug_init(debugging_types, file_name);
	g_free(file_name);

	ggz_debug(DBG_MAIN, "Starting GGZCards client.");
}

/* This function should be called at the end of the program to clean up
   debugging, as necessary. */
static void cleanup_debugging(void)
{
	/* ggz_cleanup_debug writes the data out to the file and does a memory 
	   check at the same time. */
	ggz_debug(DBG_MAIN, "Shutting down GGZCards client.");
#ifdef DEBUG
	ggz_debug_cleanup(GGZ_CHECK_MEM);
#else
	ggz_debug_cleanup(GGZ_CHECK_NONE);
#endif
}

void listen_for_server(bool listen)
{
	static guint server_socket_tag;
	static GIOChannel *channel = NULL;
	/* Invariant: (channel != NULL) <=> listening */

	ggz_debug(DBG_MAIN, "%s server.",
		  listen ? "Listening for" : "Ignoring");

	if (listen && !channel) {
		int fd = client_get_fd();

		channel = g_io_channel_unix_new(fd);
		assert(fd >= 0);
		server_socket_tag = g_io_add_watch(channel, G_IO_IN,
						   game_handle_io, NULL);
	} else if (!listen && channel) {
		g_source_remove(server_socket_tag);
		g_io_channel_unref(channel);
		channel = NULL;
	}
}

/* Initialize data for the "about" dialog */
static void init_about_dialog(void)
{
	const char *about_content =
	    _("Authors:\n"
	      "        Gtk+ Client:\n"
	      "            Rich Gade        <rgade@users.sourceforge.net>\n"
	      "            Jason Short      <jdorje@users.sourceforge.net>\n"
	      "\n"
	      "        Game Server:\n"
	      "            Jason Short      <jdorje@users.sourceforge.net>\n"
	      "            Rich Gade        <rgade@users.sourceforge.net>\n"
	      "\n"
	      "        Game Modules:\n"
	      "            Jason Short      <jdorje@users.sourceforge.net>\n"
	      "            Rich Gade        <rgade@users.sourceforge.net>\n"
	      "            Ismael Orenstein <perdig@users.sourceforge.net>\n"
	      "\n"
	      "        AI Modules:\n"
	      "            Jason Short      <jdorje@users.sourceforge.net>\n"
	      "            Brent Hendricks  <bmh@users.sourceforge.net>\n"
	      "\n"
	      "Website:\n"
	      "        http://www.ggzgamingzone.org/games/ggzcards/");
	char *about_header;

	about_header = g_strdup_printf(_("GGZ Gaming Zone\n"
					 "GGZ Cards Version %s"), VERSION);
	init_dlg_about(_("About GGZCards"), about_header, about_content);
	g_free(about_header);
}


/* 
 * The following functions don't really fit in this file, but have kind of
 * grown up here.  Eventually I'll move them...
 */

/* This puts up the given character message onto the "status bar".  The status 
   bar is the lower of the two bars at the bottom of the window. Statusbar
   messages are generated internally by the client when certain events happen. 
 */
void statusbar_message(const char *msg)
{
	static GtkWidget *sb = NULL;
	static guint sb_context;

	if (sb == NULL) {
		/* Retrieve the statusbar widget, which was allocated in
		   create_dlg_main(). */
		sb = g_object_get_data(G_OBJECT(dlg_main), "statusbar1");
		sb_context =
		    gtk_statusbar_get_context_id(GTK_STATUSBAR(sb),
						 "Game Messages");
	}

	(void)gtk_statusbar_pop(GTK_STATUSBAR(sb), sb_context);
	(void)gtk_statusbar_push(GTK_STATUSBAR(sb), sb_context, msg);
	ggz_debug(DBG_TABLE, "Put up statusbar message: '%s'", msg);
}

/* This puts up the given character message onto the "message bar".  The
   message bar is the upper of the two bars at the bottom of the window.
   Messagebar messages are generated by the server and sent as-is to the
   clients. */
void messagebar_message(const char *msg)
{
	static GtkWidget *sb = NULL;
	static guint sb_context;

	if (sb == NULL) {
		sb = g_object_get_data(G_OBJECT(dlg_main), "messagebar");
		sb_context =
		    gtk_statusbar_get_context_id(GTK_STATUSBAR(sb),
						 "Game Messages");
	}

	(void)gtk_statusbar_pop(GTK_STATUSBAR(sb), sb_context);
	(void)gtk_statusbar_push(GTK_STATUSBAR(sb), sb_context, msg);
	ggz_debug(DBG_TABLE, "Put up messagebar message: '%s'", msg);
}


static GtkWidget *msg_menu = NULL;

static void on_mnu_messages_activate(GtkMenuItem * menuitem,
				     gpointer user_data)
{
	char *mark = user_data;
	GtkWidget *dlg = g_object_get_data(G_OBJECT(msg_menu), mark);

	ggz_debug(DBG_TABLE, "Activating dialog for mark %s.", mark);

	assert(dlg);

	gtk_widget_show(dlg);
	gdk_window_show(dlg->window);
	gdk_window_raise(dlg->window);
}

static void verify_msg_menu(void)
{
	if (msg_menu == NULL) {
		msg_menu = get_menu_item(_("<main>/Messages"));
		assert(msg_menu);
	}
}

static GtkWidget *get_message_dialog(const char *mark)
{
	GtkWidget *menu_item, *dlg;

	menu_item = g_object_get_data(G_OBJECT(dlg_main), mark);
	if (!menu_item)
		return NULL;

	dlg = g_object_get_data(G_OBJECT(msg_menu), mark);
	assert(dlg);
	return dlg;
}

static GtkWidget *new_message_dialog(const char *mark)
{
	GtkWidget *menu_item, *dialog, *vbox;

	ggz_debug(DBG_TABLE, "Making new thingy for mark %s.", mark);

	/* 
	 * Make the menu item
	 */
	menu_item = gtk_menu_item_new_with_label(mark);
	gtk_widget_set_name(menu_item, mark);
	gtk_widget_ref(menu_item);
	g_object_set_data_full(G_OBJECT(dlg_main), mark,
			       menu_item,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(menu_item);
	gtk_container_add(GTK_CONTAINER(msg_menu), menu_item);
	(void)g_signal_connect(GTK_OBJECT(menu_item), "activate",
			       GTK_SIGNAL_FUNC(on_mnu_messages_activate),
			       (gpointer) g_strdup(mark));

	/* 
	 * Make the dialog window
	 */
	dialog = gtk_dialog_new_with_buttons(mark, GTK_WINDOW(dlg_main), 0,
					     GTK_STOCK_CLOSE,
					     GTK_RESPONSE_CLOSE, NULL);
	gtk_widget_ref(dialog);
	g_object_set_data(G_OBJECT(msg_menu), mark, dialog);
	gtk_window_set_resizable(GTK_WINDOW(dialog), TRUE);
	g_signal_connect(dialog, "delete_event",
			 GTK_SIGNAL_FUNC(gtk_widget_hide), NULL);

	/* dialog->vbox is used for placing the message data in; however, this 
	   data depends on the _type_ of message so we leave that for later. */
	vbox = GTK_DIALOG(dialog)->vbox;
	gtk_container_set_border_width(GTK_CONTAINER(vbox), 10);
	gtk_widget_show(vbox);

	/* 
	 * Make the "close" button
	 */
	g_signal_connect(dialog, "response",
			 GTK_SIGNAL_FUNC(gtk_widget_hide), NULL);

	return dialog;
}

void menubar_text_message(const char *mark, const char *msg)
{
	/* the first time a global message is received, we make a dialog
	   window for it. each time it is changed, we simply go into the
	   window and change the label. the window isn't shown until it's
	   activated by the menu item (above), and it's not destroyed when
	   it's closed - just hidden. */
	GtkWidget *dlg, *label, *vbox;

	verify_msg_menu();
	assert(msg && mark);

	dlg = get_message_dialog(mark);
	if (dlg == NULL) {
		dlg = new_message_dialog(mark);

		vbox = GTK_DIALOG(dlg)->vbox;

		label = gtk_label_new(msg);
		gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_LEFT);
		gtk_widget_ref(label);
		g_object_set_data_full(G_OBJECT(dlg), "label", label,
				       (GtkDestroyNotify)
				       gtk_widget_unref);
		gtk_widget_show(label);

		gtk_container_add(GTK_CONTAINER(GTK_DIALOG(dlg)->vbox),
				  label);

		gtk_widget_modify_font(label, fixed_font);

		/* in theory, the window *can't* be destroyed. */
	} else {
		label = g_object_get_data(G_OBJECT(dlg), "label");
		assert(label);
		gtk_label_set_text(GTK_LABEL(label), msg);
	}
}

void menubar_cardlist_message(const char *mark, int *lengths,
			      card_t ** cardlist)
{
	GtkWidget *dlg, *canvas, *layout, **name_labels;
	GdkPixmap *image = NULL;
	int p, i, max_len = 0;
	int width, height, w, h;
	extern GtkWidget *table;	/* Damn, why can't I figure out
					   another way to do this? */
	extern GtkStyle *table_style;	/* And I do it twice!! */

	/* determine dimensions */
	for (p = 0; p < ggzcards.num_players; p++)
		if (lengths[p] > max_len)
			max_len = lengths[p];
	height = CARDHEIGHT * ggzcards.num_players;
	width = CARDWIDTH + (max_len - 1) * CARD_VISIBILITY;;

	verify_msg_menu();

	dlg = get_message_dialog(mark);
	if (!dlg) {
		assert(max_len > 0);

		dlg = new_message_dialog(mark);

		image = gdk_pixmap_new(table->window, width, height, -1);
		g_object_set_data(G_OBJECT(dlg), "image", image);

		canvas = gtk_pixmap_new(image, NULL);
		gtk_widget_ref(canvas);
		g_object_set_data(G_OBJECT(dlg), "canvas", canvas);

#if 1
		layout = gtk_table_new(ggzcards.num_players, 2, FALSE);
		gtk_table_attach(GTK_TABLE(layout), canvas, 1, 2, 0,
				 ggzcards.num_players, 0, 0, 0, 0);

		name_labels =
		    g_malloc(ggzcards.num_players * sizeof(*name_labels));
		g_object_set_data(G_OBJECT(dlg), "names", name_labels);
		for (p = 0; p < ggzcards.num_players; p++) {
			name_labels[p] =
			    gtk_label_new(ggzcards.players[p].name);
			gtk_table_attach(GTK_TABLE(layout), name_labels[p],
					 0, 1, p, p + 1, 0, 0, 0, 0);
		}
		gtk_container_add(GTK_CONTAINER(GTK_DIALOG(dlg)->vbox),
				  layout);
		gtk_widget_show_all(layout);

#else
		gtk_container_add(GTK_CONTAINER(GTK_DIALOG(dlg)->vbox),
				  canvas);
#endif

	}

	/* Retrieve data.  If the cardlist maxlength has changed, we're in
	   trouble. */
	image = g_object_get_data(G_OBJECT(dlg), "image");
	canvas = g_object_get_data(G_OBJECT(dlg), "canvas");
	name_labels = g_object_get_data(G_OBJECT(dlg), "names");
	assert(image && canvas && name_labels);

	gdk_drawable_get_size(image, &w, &h);
	if (w != width || h != height) {
		GdkPixmap *old_image = image;
		image = gdk_pixmap_new(table->window, width, height, -1);
		g_object_set_data(G_OBJECT(dlg), "image", image);

		g_object_unref(old_image);
	}

	/* Redraw image */
	gdk_draw_rectangle(image,
			   table_style->bg_gc[GTK_WIDGET_STATE(table)],
			   TRUE, 0, 0, width, height);
	for (p = 0; p < ggzcards.num_players; p++) {
		for (i = 0; i < lengths[p]; i++) {
			draw_card(cardlist[p][i], 0, i * CARD_VISIBILITY,
				  p * CARDHEIGHT, image);
		}
		gtk_label_set_text(GTK_LABEL(name_labels[p]),
				   ggzcards.players[p].name);
	}


	/* Update widget. Ugly. */
	gtk_pixmap_set(GTK_PIXMAP(canvas), image, NULL);
	gtk_widget_hide(canvas);
	gtk_widget_show(canvas);
}
