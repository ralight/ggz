/* 
 * File: main.c
 * Author: Rich Gade
 * Project: GGZCards Client
 * Date: 08/14/2000
 * Desc: Main loop and core logic
 * $Id: main.c 3030 2002-01-09 12:41:08Z dr_maux $
 *
 * Copyright (C) 2000 Brent Hendricks.
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
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <easysock.h>
#include <ggz.h>		/* libggz */
#include "common.h"

#include "animation.h"
#include "main.h"
#include "dlg_main.h"
#include "dlg_bid.h"
#include "table.h"
#include "game.h"
#include "layout.h"

GtkWidget *dlg_main = NULL;

static void initialize_debugging(void);
static void access_settings(int save);
static void cleanup_debugging(void);

int main(int argc, char *argv[])
{
	int fd;

	/* Standard initializations. */
	initialize_debugging();
	fd = client_initialize();
	gtk_init(&argc, &argv);
	access_settings(0);
	gdk_input_add(fd, GDK_INPUT_READ, game_handle_io, NULL);

	/* This shouldn't go here, but I see no better place right now. The
	   message windows are supposed to use a fixed-width font. */
	fixed_font_style = gtk_rc_style_new();
	fixed_font_style->fontset_name =
		"-*-fixed-medium-r-normal--14-*-*-*-*-*-*-*,*-r-*";

	/* Now some more initializations... */
	dlg_main = create_dlg_main();
	gtk_widget_show(dlg_main);
	table_initialize();
	game_init();

	/* Now run... */
	gtk_main();

	/* Now clean up and shut down. */
	ggz_debug("main", "Cleaning up and quitting.");
	client_quit();

	access_settings(1);
	ggz_conf_cleanup();

	cleanup_debugging();
	return 0;
}

static void initialize_debugging(void)
{
#ifdef DEBUG
	const char *debugging_types[] =
		{ "main", "table", "animation", NULL };
#else
	const char *debugging_types[] = { NULL };
#endif
	char *file_name =
		g_strdup_printf("%s/.ggz/ggzcards-gtk.debug", getenv("HOME"));
	ggz_debug_init(debugging_types, file_name);
	g_free(file_name);

	ggz_debug("main", "Starting GGZCards client.");
}

/* save==0 => load; save==0 => save */
static void access_settings(int save)
{
	static int file = -1;

	if (file < 0) {
		char *name = g_strdup_printf("%s/.ggz/ggzcards-gtk.rc",
					     getenv("HOME"));
		file = ggz_conf_parse(name, GGZ_CONF_RDWR | GGZ_CONF_CREATE);

		if (file < 0)
			ggz_debug("main", "Couldn't open conf file '%s'.",
				  name);

		g_free(name);
	}

	if (save) {
		/* Save. */
		ggz_conf_write_int(file, "BOOLEAN", "animation",
				   preferences.animation);
		ggz_conf_write_int(file, "BOOLEAN", "cardlists",
				   preferences.cardlists);
		ggz_conf_write_int(file, "BOOLEAN", "autostart",
				   preferences.autostart);
		ggz_conf_commit(file);
	} else {
		/* Load. */
		preferences.animation =
			ggz_conf_read_int(file, "BOOLEAN", "animation", 1);
		preferences.cardlists =
			ggz_conf_read_int(file, "BOOLEAN", "cardlists", 1);
		preferences.autostart =
			ggz_conf_read_int(file, "BOOLEAN", "autostart", 0);
	}
}

static void cleanup_debugging(void)
{
	ggz_debug("main", "Shutting down GGZCards client.");
	ggz_debug_cleanup(GGZ_CHECK_MEM);
}

void statusbar_message(char *msg)
{
	static GtkWidget *sb = NULL;
	static guint sb_context;

	if (sb == NULL) {
		sb = gtk_object_get_data(GTK_OBJECT(dlg_main), "statusbar1");
		sb_context = gtk_statusbar_get_context_id(GTK_STATUSBAR(sb),
							  "Game Messages");
	}

	gtk_statusbar_push(GTK_STATUSBAR(sb), sb_context, msg);
	ggz_debug("table", "Put up statusbar message: '%s'", msg);
}

void messagebar_message(const char *msg)
{
	static GtkWidget *sb = NULL;
	static guint sb_context;

	if (sb == NULL) {
		sb = gtk_object_get_data(GTK_OBJECT(dlg_main), "messagebar");
		sb_context = gtk_statusbar_get_context_id(GTK_STATUSBAR(sb),
							  "Game Messages");
	}

	gtk_statusbar_push(GTK_STATUSBAR(sb), sb_context, msg);
	ggz_debug("table", "Put up messagebar message: '%s'", msg);
}

/* TODO: this stuff should go in its own file */
static GtkWidget *msg_menu = NULL;

void on_mnu_messages_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	char *mark = user_data;
	GtkWidget *dlg = gtk_object_get_data(GTK_OBJECT(msg_menu), mark);

	ggz_debug("table", "Activating dialog for mark %s.", mark);

	assert(dlg);

	gtk_widget_show(dlg);
	gdk_window_show(dlg->window);
	gdk_window_raise(dlg->window);
}

static void verify_msg_menu(void)
{
	if (msg_menu == NULL) {
		msg_menu =
			gtk_object_get_data(GTK_OBJECT(dlg_main),
					    "mnu_messages_menu");
		assert(msg_menu);
	}
}

static GtkWidget *get_message_dialog(const char *mark)
{
	GtkWidget *menu_item, *dlg;

	menu_item = gtk_object_get_data(GTK_OBJECT(dlg_main), mark);
	if (!menu_item)
		return NULL;

	dlg = gtk_object_get_data(GTK_OBJECT(msg_menu), mark);
	assert(dlg);
	return dlg;
}

static GtkWidget *new_message_dialog(const char *mark)
{
	GtkWidget *menu_item, *dialog, *close_button, *vbox;

	ggz_debug("table", "Making new thingy for mark %s.", mark);

	/*
	 * Make the menu item
	 */
	menu_item = gtk_menu_item_new_with_label(mark);
	gtk_widget_set_name(menu_item, mark);
	gtk_widget_ref(menu_item);
	gtk_object_set_data_full(GTK_OBJECT(dlg_main), mark,
				 menu_item,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(menu_item);
	gtk_container_add(GTK_CONTAINER(msg_menu), menu_item);
	gtk_signal_connect(GTK_OBJECT(menu_item), "activate",
			   GTK_SIGNAL_FUNC(on_mnu_messages_activate),
			   (gpointer) g_strdup(mark));

	/*
	 * Make the dialog window
	 */
	dialog = gtk_dialog_new();
	gtk_widget_ref(dialog);
	gtk_object_set_data(GTK_OBJECT(msg_menu), mark, dialog);
	gtk_window_set_title(GTK_WINDOW(dialog), mark);
	GTK_WINDOW(dialog)->type = GTK_WINDOW_DIALOG;
	gtk_window_set_policy(GTK_WINDOW(dialog), TRUE, TRUE, FALSE);
	gtk_signal_connect_object(GTK_OBJECT(dialog), "delete_event",
				  GTK_SIGNAL_FUNC(gtk_widget_hide),
				  GTK_OBJECT(dialog));

	/* dialog->vbox is used for placing the message data in; however,
	   this data depends on the _type_ of message so we leave that
	   for later. */
	vbox = GTK_DIALOG(dialog)->vbox;
	gtk_container_set_border_width(GTK_CONTAINER(vbox), 10);
	gtk_widget_show(vbox);

	/*
	 * Make the "close" button
	 */
	close_button = gtk_button_new_with_label("Close");
	gtk_widget_ref(close_button);
	gtk_object_set_data_full(GTK_OBJECT(dialog), "close_button",
				 close_button,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(close_button);
	gtk_widget_set_usize(close_button, 64, -2);
	gtk_signal_connect_object(GTK_OBJECT(close_button), "clicked",
				  GTK_SIGNAL_FUNC(gtk_widget_hide),
				  GTK_OBJECT(dialog));
	gtk_container_add(GTK_CONTAINER(GTK_DIALOG(dialog)->action_area),
			  close_button);

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
		gtk_object_set_data_full(GTK_OBJECT(dlg), "label", label,
					 (GtkDestroyNotify) gtk_widget_unref);
		gtk_widget_show(label);

		gtk_container_add(GTK_CONTAINER(GTK_DIALOG(dlg)->vbox),
				  label);

		gtk_widget_modify_style(label, fixed_font_style);

		/* in theory, the window *can't* be destroyed. */
	} else {
		label = gtk_object_get_data(GTK_OBJECT(dlg), "label");
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
	width = CARDWIDTH + (max_len - 1) * CARDWIDTH / 4;

	verify_msg_menu();

	dlg = get_message_dialog(mark);
	if (!dlg) {
		assert(max_len > 0);

		dlg = new_message_dialog(mark);

		image = gdk_pixmap_new(table->window, width, height, -1);
		gtk_object_set_data(GTK_OBJECT(dlg), "image", image);

		canvas = gtk_pixmap_new(image, NULL);
		gtk_widget_ref(canvas);
		gtk_object_set_data(GTK_OBJECT(dlg), "canvas", canvas);

#if 1
		layout = gtk_table_new(ggzcards.num_players, 2, FALSE);
		gtk_table_attach(GTK_TABLE(layout), canvas, 1, 2, 0,
				 ggzcards.num_players, 0, 0, 0, 0);

		name_labels =
			g_malloc(ggzcards.num_players * sizeof(*name_labels));
		gtk_object_set_data(GTK_OBJECT(dlg), "names", name_labels);
		for (p = 0; p < ggzcards.num_players; p++) {
			name_labels[p] =
				gtk_label_new(ggzcards.players[p].name);
			gtk_table_attach(GTK_TABLE(layout), name_labels[p], 0,
					 1, p, p + 1, 0, 0, 0, 0);
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
	image = gtk_object_get_data(GTK_OBJECT(dlg), "image");
	canvas = gtk_object_get_data(GTK_OBJECT(dlg), "canvas");
	name_labels = gtk_object_get_data(GTK_OBJECT(dlg), "names");
	assert(image && canvas && name_labels);

	gdk_window_get_size(image, &w, &h);
	if (w != width || h != height) {
		GdkPixmap *old_image = image;
		image = gdk_pixmap_new(table->window, width, height, -1);
		gtk_object_set_data(GTK_OBJECT(dlg), "image", image);

		gdk_pixmap_unref(old_image);
	}

	/* Redraw image */
	gdk_draw_rectangle(image,
			   table_style->bg_gc[GTK_WIDGET_STATE(table)],
			   TRUE, 0, 0, width, height);
	for (p = 0; p < ggzcards.num_players; p++) {
		for (i = 0; i < lengths[p]; i++) {
			draw_card(cardlist[p][i], 0, i * CARDWIDTH / 4,
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
