/* 
 * File: main.c
 * Author: Rich Gade
 * Project: GGZCards Client
 * Date: 08/14/2000
 * Desc: Main loop and core logic
 * $Id: main.c 2385 2001-09-07 08:43:35Z jdorje $
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
#include <ggz_client.h>
#include "common.h"

#include "main.h"
#include "dlg_main.h"
#include "dlg_bid.h"
#include "table.h"
#include "game.h"
#include "hand.h"

GtkWidget *dlg_main = NULL;


int main(int argc, char *argv[])
{
	int fd;

	gtk_init(&argc, &argv);

	fd = client_initialize();

	gdk_input_add(fd, GDK_INPUT_READ, game_handle_io, NULL);

	/* this shouldn't go here, but I see no better place right now */
	fixed_font_style = gtk_rc_style_new();
	fixed_font_style->fontset_name =
		"-*-fixed-medium-r-normal--14-*-*-*-*-*-*-*,*-r-*";

	dlg_main = create_dlg_main();

	gtk_widget_show(dlg_main);

	table_initialize();
	game_init();

	gtk_main();

	client_quit();

	return 0;
}

void table_get_newgame()
{
	/* FIXME: we should ask the player first! */
	client_send_newgame();
}

void table_get_play(int hand)
{
#ifdef ANIMATION
	if (game.state == WH_STATE_ANIM)
		table_animation_zip(TRUE);
#endif /* ANIMATION */

	if (hand == 0)
		statusbar_message(_("Your turn to play a card"));
	else {
		char buf[100];
		snprintf(buf, sizeof(buf),
			 _("Your turn to play a card from %s's hand."),
			 game.players[hand].name);
		statusbar_message(buf);
	}
}


void table_alert_player_name(int player, char *name)
{
	if (player != 0 && game.num_players &&
	    strcmp(name, game.players[player].name)) {
		char *temp = g_strdup_printf(_("%s joined the table"), name);
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
			/* TODO: better grammar */
			snprintf(msg + strlen(msg), sizeof(msg) - strlen(msg),
				 "%s ", game.players[winners[i]].name);
		}
		snprintf(msg + strlen(msg), sizeof(msg) - strlen(msg),
			 _("won the game"));
	}

	statusbar_message(msg);
}

void table_get_bid(int possible_bids, char **bid_choices)
{
	dlg_bid_display(possible_bids, bid_choices);
	statusbar_message(_("Your turn to bid"));
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
	client_debug("     Put up statusbar message: '%s'", msg);
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
	client_debug("     Put up messagebar message: '%s'", msg);
}

/* TODO: this stuff should go in its own file */
static GtkWidget *msg_menu = NULL;

void on_mnu_messages_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	char *mark = user_data;
	GtkWidget *dlg = gtk_object_get_data(GTK_OBJECT(msg_menu), mark);

	client_debug("Activating dialog for mark %s.", mark);

	assert(dlg);

	gtk_widget_show(dlg);
	gdk_window_show(dlg->window);
	gdk_window_raise(dlg->window);
}

void menubar_message(const char *mark, const char *msg)
{
	GtkWidget *menu_item, *dlg, *label, *vbox, *ok_button;

	assert(msg && mark);

	if (msg_menu == NULL) {
		msg_menu =
			gtk_object_get_data(GTK_OBJECT(dlg_main),
					    "mnu_messages_menu");
		assert(msg_menu);
	}

	/* the first time a global message is received, we make a dialog
	   window for it. each time it is changed, we simply go into the
	   window and change the label. the window isn't shown until it's
	   activated by the menu item (above), and it's not destroyed when
	   it's closed - just hidden. */

	menu_item = gtk_object_get_data(GTK_OBJECT(dlg_main), mark);
	if (menu_item == NULL) {
		client_debug("Making new thingy for mark %s.", mark);
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
		/* gtk_object_set_data (GTK_OBJECT (dlg), "dlg_messages",
		   dlg_about); */
		gtk_window_set_title(GTK_WINDOW(dlg), mark);
		GTK_WINDOW(dlg)->type = GTK_WINDOW_DIALOG;
		gtk_window_set_policy(GTK_WINDOW(dlg), TRUE, TRUE, FALSE);

		vbox = GTK_DIALOG(dlg)->vbox;
		gtk_widget_show(vbox);

		label = gtk_label_new(msg);
		gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_LEFT);
		gtk_widget_ref(label);
		gtk_object_set_data_full(GTK_OBJECT(dlg), "label", label,
					 (GtkDestroyNotify) gtk_widget_unref);
		gtk_widget_show(label);

		gtk_box_pack_start(GTK_BOX(vbox), label, TRUE, TRUE, 0);

		ok_button = gtk_button_new_with_label("OK");
		gtk_widget_ref(ok_button);
		gtk_object_set_data_full(GTK_OBJECT(dlg), "ok_button",
					 ok_button,
					 (GtkDestroyNotify) gtk_widget_unref);
		gtk_widget_show(ok_button);
		gtk_box_pack_start(GTK_BOX(vbox), ok_button, FALSE, FALSE, 0);
		gtk_widget_set_usize(ok_button, 64, -2);

		gtk_signal_connect_object(GTK_OBJECT(dlg), "delete_event",
					  GTK_SIGNAL_FUNC(gtk_widget_hide),
					  GTK_OBJECT(dlg));
		gtk_signal_connect_object(GTK_OBJECT(ok_button), "clicked",
					  GTK_SIGNAL_FUNC(gtk_widget_hide),
					  GTK_OBJECT(dlg));

		gtk_widget_modify_style(label, fixed_font_style);

		/* in theory, the window *can't* be destroyed. */
	} else {
		dlg = gtk_object_get_data(GTK_OBJECT(msg_menu), mark);
		assert(dlg);
		label = gtk_object_get_data(GTK_OBJECT(dlg), "label");
		assert(label);
		gtk_label_set_text(GTK_LABEL(label), msg);
	}
}


void table_alert_badplay(char *err_msg)
{
#ifdef ANIMATION
	table_animation_abort();
#endif /* ANIMATION */

	/* redraw cards */
	table_display_hand(game.play_hand);

	statusbar_message(err_msg);
	sleep(1);		/* just a delay? */
}

void table_alert_play(int player, card_t card)
{
#ifdef ANIMATION
	if (game.state == WH_STATE_ANIM)
		table_animation_zip(TRUE);
#endif /* ANIMATION */

	table_display_hand(player);
	table_play_card(player, card);
}


void table_alert_table()
{
	table_show_cards();
}

void table_alert_trick(int player)
{
	char *t_str;

#ifdef ANIMATION
	if (game.state == WH_STATE_ANIM)
		table_animation_zip(TRUE);
#endif /* ANIMATION */

	t_str = g_strdup_printf(_("%s won the trick"),
				game.players[player].name);
	statusbar_message(t_str);
	g_free(t_str);

	/* TODO: make this sleep optional (a preference) */
	sleep(1);

	table_clear_table();
}
