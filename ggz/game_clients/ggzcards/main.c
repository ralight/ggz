/* 
 * File: main.c
 * Author: Rich Gade
 * Project: GGZCards Client
 * Date: 08/14/2000
 * Desc: Main loop and core logic
 * $Id: main.c 2868 2001-12-10 23:03:45Z jdorje $
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

#include "main.h"
#include "dlg_main.h"
#include "dlg_bid.h"
#include "table.h"
#include "game.h"
#include "hand.h"
#include "layout.h"

GtkWidget *dlg_main = NULL;

int main(int argc, char *argv[])
{
	int fd;
#ifdef DEBUG
	const char *debugging_types[] = { "main", "table", NULL };
#else
	const char *debugging_types[] = { NULL };
#endif

	ggz_debug_init(debugging_types, NULL);
	ggz_debug("main", "Starting GGZCards client.");

	gtk_init(&argc, &argv);

	fd = client_initialize();

	gdk_input_add(fd, GDK_INPUT_READ, game_handle_io, NULL);

	/* This shouldn't go here, but I see no better place right now. The
	   message windows are supposed to use a fixed-width font. */
	fixed_font_style = gtk_rc_style_new();
	fixed_font_style->fontset_name =
		"-*-fixed-medium-r-normal--14-*-*-*-*-*-*-*,*-r-*";

	dlg_main = create_dlg_main();

	gtk_widget_show(dlg_main);

	table_initialize();
	game_init();

	gtk_main();

	client_quit();

	ggz_debug("main", "Shutting down GGZCards client.");
	ggz_debug_cleanup(GGZ_CHECK_MEM);

	return 0;
}

void table_get_newgame(void)
{
	GtkWidget *menu =
		gtk_object_get_data(GTK_OBJECT(dlg_main), "mnu_startgame");
	statusbar_message(_("Select \"Start Game\" to begin the game."));
	gtk_widget_set_sensitive(menu, TRUE);
}

void table_send_newgame(void)
{
	GtkWidget *menu =
		gtk_object_get_data(GTK_OBJECT(dlg_main), "mnu_startgame");
	client_send_newgame();
	statusbar_message(_("Waiting for the other players..."));
	gtk_widget_set_sensitive(menu, FALSE);
}

void table_get_play(int hand)
{
#ifdef ANIMATION
	if (ggzcards.state == STATE_ANIM)
		table_animation_zip(TRUE);
#endif /* ANIMATION */

	if (hand == 0)
		statusbar_message(_("Your turn to play a card"));
	else {
		char buf[100];
		snprintf(buf, sizeof(buf),
			 _("Your turn to play a card from %s's hand."),
			 ggzcards.players[hand].name);
		statusbar_message(buf);
	}
}


void table_alert_player(int player, GGZdModSeat status, const char *name)
{
	char *temp = NULL;
	switch (status) {
	case GGZ_SEAT_PLAYER:
		temp = g_strdup_printf(_("%s joined the table"), name);
		break;
	case GGZ_SEAT_OPEN:
		name = _("Empty Seat");
		if (ggzcards.players[player].name)
			temp = g_strdup_printf(_("%s left the table"),
					       ggzcards.players[player].name);
		break;
	default:
		/* any other handling? */
		break;
	}
	if (temp) {
		statusbar_message(temp);
		g_free(temp);
	}

	table_set_name(player, name);
}


void table_handle_gameover(int num_winners, int *winners)
{
	char msg[4096] = "";
	int i;

	/* handle different cases */
	if (num_winners == 0)
		snprintf(msg, sizeof(msg), _("There was no winner"));
	else {
		for (i = 0; i < num_winners; i++) {
			char *fmt;
			if (i == num_winners - 1)
				fmt = "%s ";
			else if (i == num_winners - 2)
				fmt = "%s and ";	/* not quite perfect
							   grammar... */
			else
				fmt = "%s, ";
			snprintf(msg + strlen(msg), sizeof(msg) - strlen(msg),
				 fmt, ggzcards.players[winners[i]].name);
		}
		snprintf(msg + strlen(msg), sizeof(msg) - strlen(msg),
			 _("won the game."));
	}

	/* This hack places this message in place of the global game message. 
	   It prevents it from being overwritten by the upcoming newgame
	   request. */
	messagebar_message(msg);
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
	ggz_debug("table", "     Put up statusbar message: '%s'", msg);
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
	ggz_debug("table", "     Put up messagebar message: '%s'", msg);
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
	GtkWidget *menu_item, *dlg, *ok_button;

	ggz_debug("table", "Making new thingy for mark %s.", mark);
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

	dlg = gtk_dialog_new();
	gtk_widget_ref(dlg);
	gtk_object_set_data(GTK_OBJECT(msg_menu), mark, dlg);
	/* gtk_object_set_data (GTK_OBJECT (dlg), "dlg_messages", dlg_about); 
	 */
	gtk_window_set_title(GTK_WINDOW(dlg), mark);
	GTK_WINDOW(dlg)->type = GTK_WINDOW_DIALOG;
	gtk_window_set_policy(GTK_WINDOW(dlg), TRUE, TRUE, FALSE);


	ok_button = gtk_button_new_with_label("OK");
	gtk_widget_ref(ok_button);
	gtk_object_set_data_full(GTK_OBJECT(dlg), "ok_button",
				 ok_button,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(ok_button);
	gtk_widget_set_usize(ok_button, 64, -2);
	gtk_signal_connect_object(GTK_OBJECT(ok_button), "clicked",
				  GTK_SIGNAL_FUNC(gtk_widget_hide),
				  GTK_OBJECT(dlg));

	gtk_container_add(GTK_CONTAINER(GTK_DIALOG(dlg)->action_area),
			  ok_button);

	gtk_signal_connect_object(GTK_OBJECT(dlg), "delete_event",
				  GTK_SIGNAL_FUNC(gtk_widget_hide),
				  GTK_OBJECT(dlg));

	return dlg;
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
		gtk_widget_show(vbox);

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


void table_alert_badplay(char *err_msg)
{
#ifdef ANIMATION
	table_animation_abort();
#endif /* ANIMATION */

	/* redraw cards */
	table_display_hand(ggzcards.play_hand);

	statusbar_message(err_msg);
	sleep(1);		/* just a delay? */
}

void table_alert_play(int player, card_t card)
{
#ifdef ANIMATION
	if (ggzcards.state == STATE_ANIM)
		table_animation_zip(TRUE);
#endif /* ANIMATION */

	table_display_hand(player);
	table_play_card(player, card);
}


void table_alert_table()
{
	ggz_debug("table", "Handling table update alert.");
	table_show_cards();
}

void table_alert_trick(int player)
{
	char *t_str;

#ifdef ANIMATION
	if (ggzcards.state == STATE_ANIM)
		table_animation_zip(TRUE);
#endif /* ANIMATION */

	t_str = g_strdup_printf(_("%s won the trick"),
				ggzcards.players[player].name);
	statusbar_message(t_str);
	g_free(t_str);

	/* TODO: make this sleep optional (a preference) */
	sleep(1);

	table_clear_table();
}

int table_handle_game_message(int fd, int game, int size)
{
	return 0;
}
