/*
 * File: popup.c
 * Author: Ismael Orenstein
 * Project: GGZ Chess game module
 * Date: 09/17/2000
 * Desc: Chess client popup dialogs
 *
 * Copyright (C) 2001 Ismael Orenstein.
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

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include "popup.h"
#include "support.h"
#include "board.h"
#include "chess.h"
#include "net.h"

GtkWidget *create_draw_dialog(void)
{
	GtkWidget *draw_dialog;
	GtkWidget *dialog_vbox1;
	GtkWidget *draw_message;
	GtkWidget *dialog_action_area1;
	GtkWidget *hbuttonbox1;
	GtkWidget *draw;
	GtkWidget *cancel;

	draw_dialog = gtk_dialog_new();
	g_object_set_data(G_OBJECT(draw_dialog), "draw_dialog",
			  draw_dialog);
	gtk_window_set_title(GTK_WINDOW(draw_dialog),
			     _("Do you want do draw the game ?"));

	dialog_vbox1 = GTK_DIALOG(draw_dialog)->vbox;
	g_object_set_data(G_OBJECT(draw_dialog), "dialog_vbox1",
			  dialog_vbox1);
	gtk_widget_show(dialog_vbox1);

	draw_message =
	    gtk_label_new(_
			  ("The server wants to draw the game. That means that either one of the drawing condition has happened (like repetition of the same position, or 50 moves without a capture), which means that the game will end as a tie if either you or your opponent agrees with a draw, or your oponent has asked for a draw, which will only happen if you agree about it.\n\nSo, do you want do draw the game ?"));
	gtk_widget_ref(draw_message);
	g_object_set_data_full(G_OBJECT(draw_dialog), "draw_message",
			       draw_message,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(draw_message);
	gtk_box_pack_start(GTK_BOX(dialog_vbox1), draw_message, FALSE,
			   FALSE, 0);
	gtk_label_set_line_wrap(GTK_LABEL(draw_message), TRUE);

	dialog_action_area1 = GTK_DIALOG(draw_dialog)->action_area;
	g_object_set_data(G_OBJECT(draw_dialog), "dialog_action_area1",
			  dialog_action_area1);
	gtk_widget_show(dialog_action_area1);
	gtk_container_set_border_width(GTK_CONTAINER(dialog_action_area1),
				       10);

	hbuttonbox1 = gtk_hbutton_box_new();
	gtk_widget_ref(hbuttonbox1);
	g_object_set_data_full(G_OBJECT(draw_dialog), "hbuttonbox1",
			       hbuttonbox1,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbuttonbox1);
	gtk_box_pack_start(GTK_BOX(dialog_action_area1), hbuttonbox1, TRUE,
			   TRUE, 0);

	draw = gtk_button_new_with_label(_("Draw"));
	gtk_widget_ref(draw);
	g_object_set_data_full(G_OBJECT(draw_dialog), "draw", draw,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(draw);
	gtk_container_add(GTK_CONTAINER(hbuttonbox1), draw);
	GTK_WIDGET_SET_FLAGS(draw, GTK_CAN_DEFAULT);

	cancel = gtk_button_new_with_label(_("Don't Draw"));
	gtk_widget_ref(cancel);
	g_object_set_data_full(G_OBJECT(draw_dialog), "cancel", cancel,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(cancel);
	gtk_container_add(GTK_CONTAINER(hbuttonbox1), cancel);
	GTK_WIDGET_SET_FLAGS(cancel, GTK_CAN_DEFAULT);

	g_signal_connect_data(draw, "clicked",
			      GTK_SIGNAL_FUNC(gtk_widget_destroy),
			      draw_dialog, NULL,
			      G_CONNECT_AFTER | G_CONNECT_SWAPPED);
	g_signal_connect(GTK_OBJECT(draw), "clicked",
			 GTK_SIGNAL_FUNC(board_request_draw), NULL);
	g_signal_connect_swapped(GTK_OBJECT(cancel), "clicked",
				 GTK_SIGNAL_FUNC(gtk_widget_destroy),
				 GTK_OBJECT(draw_dialog));

	return draw_dialog;
}

extern GtkWidget *main_win;	/* FIXME */

GtkWidget *create_clock_dialog(void)
{
	GtkWidget *clock_dialog;
	GtkWidget *dialog_vbox2;
	GtkWidget *vbox1;
	GSList *time_option_group = NULL;
	GtkWidget *no_clock;
	GtkWidget *server_clock;
	GtkWidget *server_lag_clock;
	GtkWidget *client_clock;
	GtkWidget *hbox1;
	GtkWidget *label2;
	GtkWidget *hbox2;
	GtkObject *minutes_adj;
	GtkWidget *minutes;
	GtkWidget *label3;
	GtkObject *seconds_adj;
	GtkWidget *seconds;
	GtkWidget *label4;
	GtkWidget *dialog_action_area2;
	GtkWidget *hbuttonbox2;
	GtkWidget *send_option;
	GtkWidget *cancel;
	GtkTooltips *tooltips;

	tooltips = gtk_tooltips_new();

	clock_dialog = gtk_dialog_new();
	gtk_window_set_transient_for(GTK_WINDOW(clock_dialog),
				     GTK_WINDOW(main_win));
	g_object_set_data(G_OBJECT(clock_dialog), "clock_dialog",
			  clock_dialog);
	gtk_window_set_title(GTK_WINDOW(clock_dialog),
			     _("Select your clock option"));

	dialog_vbox2 = GTK_DIALOG(clock_dialog)->vbox;
	g_object_set_data(G_OBJECT(clock_dialog), "dialog_vbox2",
			  dialog_vbox2);
	gtk_widget_show(dialog_vbox2);

	vbox1 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox1);
	g_object_set_data_full(G_OBJECT(clock_dialog), "vbox1", vbox1,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox1);
	gtk_box_pack_start(GTK_BOX(dialog_vbox2), vbox1, TRUE, TRUE, 0);

	no_clock =
	    gtk_radio_button_new_with_label(time_option_group,
					    _("No clock"));
	time_option_group =
	    gtk_radio_button_get_group(GTK_RADIO_BUTTON(no_clock));
	gtk_widget_ref(no_clock);
	g_object_set_data_full(G_OBJECT(clock_dialog), "no_clock",
			       no_clock,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(no_clock);
	gtk_box_pack_start(GTK_BOX(vbox1), no_clock, FALSE, FALSE, 0);
	gtk_tooltips_set_tip(tooltips, no_clock,
			     _
			     ("If you select this option, this game won't have a time limit"),
			     NULL);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(no_clock), TRUE);

	server_clock =
	    gtk_radio_button_new_with_label(time_option_group,
					    _("Server clock"));
	time_option_group =
	    gtk_radio_button_get_group(GTK_RADIO_BUTTON(server_clock));
	gtk_widget_ref(server_clock);
	g_object_set_data_full(G_OBJECT(clock_dialog), "server_clock",
			       server_clock,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(server_clock);
	gtk_box_pack_start(GTK_BOX(vbox1), server_clock, FALSE, FALSE, 0);
	gtk_tooltips_set_tip(tooltips, server_clock,
			     _
			     ("Selecting this option, the server will keep track of the time of each player. So, the time a player takes to complete a move will start the moment the server sends a message to him requesting his move, until the time where the server receives his move back. Although it is very hard to cheat using this model, the players who have a very lagged connection will be at a strong disadvantage. You should only use it on a local LAN or if both you and your oponnent have similar connection speeds."),
			     NULL);

	server_lag_clock =
	    gtk_radio_button_new_with_label(time_option_group,
					    _
					    ("Server clock with lag meter"));
	time_option_group =
	    gtk_radio_button_get_group(GTK_RADIO_BUTTON(server_lag_clock));
	gtk_widget_ref(server_lag_clock);
	g_object_set_data_full(G_OBJECT(clock_dialog), "server_lag_clock",
			       server_lag_clock,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(server_lag_clock);
	gtk_box_pack_start(GTK_BOX(vbox1), server_lag_clock, FALSE, FALSE,
			   0);
	gtk_widget_set_sensitive(server_lag_clock, FALSE);
	gtk_tooltips_set_tip(tooltips, server_lag_clock,
			     _
			     ("This option works just like the server clock, except that it uses ggz's builtin lag meter to compensate for lagged connections. It provides a good balance between security (the strong point in the server clock) and fairness (the strong point in the client clock)"),
			     NULL);

	client_clock =
	    gtk_radio_button_new_with_label(time_option_group,
					    _("Client clock"));
	time_option_group =
	    gtk_radio_button_get_group(GTK_RADIO_BUTTON(client_clock));
	gtk_widget_ref(client_clock);
	g_object_set_data_full(G_OBJECT(clock_dialog), "client_clock",
			       client_clock,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(client_clock);
	gtk_box_pack_start(GTK_BOX(vbox1), client_clock, FALSE, FALSE, 0);
	gtk_tooltips_set_tip(tooltips, client_clock,
			     _
			     ("Selecting this option, each client will keep track of its current time. This is the most fair option, as it reduces to zero the effect of lagged connections. However, as we keep too much trust in the client, a bad user can use a cheated client that won't report the time as expected. So, only use this option if you trust your oponnent."),
			     NULL);

	hbox1 = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(hbox1);
	g_object_set_data_full(G_OBJECT(clock_dialog), "hbox1", hbox1,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox1);
	gtk_box_pack_start(GTK_BOX(vbox1), hbox1, TRUE, TRUE, 0);

	label2 = gtk_label_new(_("Game time :"));
	gtk_widget_ref(label2);
	g_object_set_data_full(G_OBJECT(clock_dialog), "label2", label2,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label2);
	gtk_box_pack_start(GTK_BOX(hbox1), label2, FALSE, FALSE, 25);

	hbox2 = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(hbox2);
	g_object_set_data_full(G_OBJECT(clock_dialog), "hbox2", hbox2,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox2);
	gtk_box_pack_start(GTK_BOX(hbox1), hbox2, TRUE, TRUE, 0);

	minutes_adj = gtk_adjustment_new(15, 0, 60, 1, 10, 10);
	minutes = gtk_spin_button_new(GTK_ADJUSTMENT(minutes_adj), 1, 0);
	gtk_widget_ref(minutes);
	g_object_set_data_full(G_OBJECT(clock_dialog), "minutes", minutes,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(minutes);
	gtk_box_pack_start(GTK_BOX(hbox2), minutes, TRUE, TRUE, 0);
	gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(minutes), TRUE);

	label3 = gtk_label_new(_("min : "));
	gtk_widget_ref(label3);
	g_object_set_data_full(G_OBJECT(clock_dialog), "label3", label3,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label3);
	gtk_box_pack_start(GTK_BOX(hbox2), label3, FALSE, FALSE, 5);

	seconds_adj = gtk_adjustment_new(0, 0, 59, 1, 10, 10);
	seconds = gtk_spin_button_new(GTK_ADJUSTMENT(seconds_adj), 1, 0);
	gtk_widget_ref(seconds);
	g_object_set_data_full(G_OBJECT(clock_dialog), "seconds", seconds,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(seconds);
	gtk_box_pack_start(GTK_BOX(hbox2), seconds, TRUE, TRUE, 0);
	gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(seconds), TRUE);

	label4 = gtk_label_new(_("sec"));
	gtk_widget_ref(label4);
	g_object_set_data_full(G_OBJECT(clock_dialog), "label4", label4,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label4);
	gtk_box_pack_start(GTK_BOX(hbox2), label4, FALSE, FALSE, 5);

	dialog_action_area2 = GTK_DIALOG(clock_dialog)->action_area;
	g_object_set_data(G_OBJECT(clock_dialog), "dialog_action_area2",
			  dialog_action_area2);
	gtk_widget_show(dialog_action_area2);
	gtk_container_set_border_width(GTK_CONTAINER(dialog_action_area2),
				       10);

	hbuttonbox2 = gtk_hbutton_box_new();
	gtk_widget_ref(hbuttonbox2);
	g_object_set_data_full(G_OBJECT(clock_dialog), "hbuttonbox2",
			       hbuttonbox2,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbuttonbox2);
	gtk_box_pack_start(GTK_BOX(dialog_action_area2), hbuttonbox2, TRUE,
			   TRUE, 0);

	send_option = gtk_button_new_with_label(_("Send option"));
	gtk_widget_ref(send_option);
	g_object_set_data_full(G_OBJECT(clock_dialog), "send_option",
			       send_option,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(send_option);
	gtk_container_add(GTK_CONTAINER(hbuttonbox2), send_option);
	GTK_WIDGET_SET_FLAGS(send_option, GTK_CAN_DEFAULT);

	cancel = gtk_button_new_with_label(_("Cancel"));
	gtk_widget_ref(cancel);
	g_object_set_data_full(G_OBJECT(clock_dialog), "cancel", cancel,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(cancel);
	gtk_container_add(GTK_CONTAINER(hbuttonbox2), cancel);
	GTK_WIDGET_SET_FLAGS(cancel, GTK_CAN_DEFAULT);

	g_signal_connect(send_option, "clicked",
			 GTK_SIGNAL_FUNC(clock_option_select),
			 clock_dialog);
	g_signal_connect_data(send_option, "clicked",
			      GTK_SIGNAL_FUNC(gtk_widget_destroy),
			      clock_dialog, NULL,
			      G_CONNECT_SWAPPED | G_CONNECT_AFTER);
	g_signal_connect(cancel, "clicked",
			 GTK_SIGNAL_FUNC(clock_option_cancel), NULL);
	g_signal_connect_swapped(cancel, "clicked",
				 GTK_SIGNAL_FUNC(gtk_widget_destroy),
				 clock_dialog);

	return clock_dialog;
}


GtkWidget *create_promote_dialog(void)
{
	GtkWidget *promote_dialog;
	GtkWidget *dialog_vbox3;
	GtkWidget *vbox2;
	GSList *promote_group = NULL;
	GtkWidget *queen;
	GtkWidget *rook;
	GtkWidget *bishop;
	GtkWidget *knight;
	GtkWidget *dialog_action_area3;
	GtkWidget *ok;

	promote_dialog = gtk_dialog_new();
	g_object_set_data(G_OBJECT(promote_dialog), "promote_dialog",
			  promote_dialog);
	gtk_window_set_title(GTK_WINDOW(promote_dialog), _("Promote"));
	gtk_window_set_modal(GTK_WINDOW(promote_dialog), TRUE);

	dialog_vbox3 = GTK_DIALOG(promote_dialog)->vbox;
	g_object_set_data(G_OBJECT(promote_dialog), "dialog_vbox3",
			  dialog_vbox3);
	gtk_widget_show(dialog_vbox3);

	vbox2 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox2);
	g_object_set_data_full(G_OBJECT(promote_dialog), "vbox2", vbox2,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox2);
	gtk_box_pack_start(GTK_BOX(dialog_vbox3), vbox2, TRUE, TRUE, 0);

	queen = gtk_radio_button_new_with_label(promote_group, _("Queen"));
	promote_group =
	    gtk_radio_button_get_group(GTK_RADIO_BUTTON(queen));
	gtk_widget_ref(queen);
	g_object_set_data_full(G_OBJECT(promote_dialog), "queen", queen,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(queen);
	gtk_box_pack_start(GTK_BOX(vbox2), queen, FALSE, FALSE, 0);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(queen), TRUE);

	rook = gtk_radio_button_new_with_label(promote_group, _("Rook"));
	promote_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(rook));
	gtk_widget_ref(rook);
	g_object_set_data_full(G_OBJECT(promote_dialog), "rook", rook,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(rook);
	gtk_box_pack_start(GTK_BOX(vbox2), rook, FALSE, FALSE, 0);

	bishop =
	    gtk_radio_button_new_with_label(promote_group, _("Bishop"));
	promote_group =
	    gtk_radio_button_get_group(GTK_RADIO_BUTTON(bishop));
	gtk_widget_ref(bishop);
	g_object_set_data_full(G_OBJECT(promote_dialog), "bishop", bishop,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(bishop);
	gtk_box_pack_start(GTK_BOX(vbox2), bishop, FALSE, FALSE, 0);

	knight =
	    gtk_radio_button_new_with_label(promote_group, _("Knight"));
	promote_group =
	    gtk_radio_button_get_group(GTK_RADIO_BUTTON(knight));
	gtk_widget_ref(knight);
	g_object_set_data_full(G_OBJECT(promote_dialog), "knight", knight,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(knight);
	gtk_box_pack_start(GTK_BOX(vbox2), knight, FALSE, FALSE, 0);

	dialog_action_area3 = GTK_DIALOG(promote_dialog)->action_area;
	g_object_set_data(G_OBJECT(promote_dialog), "dialog_action_area3",
			  dialog_action_area3);
	gtk_widget_show(dialog_action_area3);
	gtk_container_set_border_width(GTK_CONTAINER(dialog_action_area3),
				       10);

	ok = gtk_button_new_with_label(_("Promote"));
	gtk_widget_ref(ok);
	g_object_set_data_full(G_OBJECT(promote_dialog), "ok", ok,
			       (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(ok);
	gtk_box_pack_start(GTK_BOX(dialog_action_area3), ok, FALSE, FALSE,
			   0);

	g_signal_connect(ok, "clicked",
			 GTK_SIGNAL_FUNC(promote_piece), promote_dialog);
	g_signal_connect_data(ok, "clicked",
			      GTK_SIGNAL_FUNC(gtk_widget_destroy),
			      promote_dialog, NULL,
			      G_CONNECT_SWAPPED | G_CONNECT_AFTER);

	return promote_dialog;
}

/* Callbacks */

void clock_option_select(GtkButton * button, gpointer user_data)
{
	GtkWidget *radio, *time_w;
	int clock = 0, time;
	/* No clock ? */
	radio = lookup_widget(user_data, "no_clock");
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio)))
		clock = CHESS_CLOCK_NOCLOCK;
	/* Client */
	radio = lookup_widget(user_data, "client_clock");
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio)))
		clock = CHESS_CLOCK_CLIENT;
	/* Server */
	radio = lookup_widget(user_data, "server_clock");
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio)))
		clock = CHESS_CLOCK_SERVER;
	/* Server + Lag */
	radio = lookup_widget(user_data, "server_lag_clock");
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio)))
		clock = CHESS_CLOCK_SERVERLAG;
	/* Now get the time */
	time_w = lookup_widget(user_data, "minutes");
	time =
	    60 * gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(time_w));
	time_w = lookup_widget(user_data, "seconds");
	time += gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(time_w));

	if (clock == CHESS_CLOCK_NOCLOCK)
		net_send_time(0);
	else
		net_send_time((clock << 24) + time);

}


void clock_option_cancel(GtkButton * button, gpointer user_data)
{
	net_send_time(0);

}
