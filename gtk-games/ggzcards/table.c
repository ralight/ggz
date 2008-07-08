/* 
 * File: table.c
 * Author: Rich Gade, Jason Short
 * Project: GGZCards Client
 * Date: 08/14/2000
 * Desc: Routines to handle the Gtk game table
 * $Id: table.c 10188 2008-07-08 03:20:24Z jdorje $
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ggz.h>
#include "client.h"

#include "animation.h"
#include "cb_main.h"
#include "dlg_bid.h"
#include "dlg_main.h"
#include "dlg_players.h"
#include "drawcard.h"
#include "game.h"
#include "layout.h"
#include "main.h"
#include "table.h"

PangoFontDescription *fixed_font;

/* Table data */
GtkWidget *table = NULL;	/* widget containing the whole table */
GtkWidget *table_drawing_area = NULL;	/* widget for actually drawing the
					   table on */
GtkStyle *table_style;		/* Style for the table */
static GdkPixmap *table_buf = NULL;	/* backing store for the table */

static GtkWidget *player_list = NULL;

static const char *player_names[MAX_NUM_PLAYERS] = { NULL };
static const char *player_messages[MAX_NUM_PLAYERS] = { NULL };

static gboolean table_ready = FALSE;

/* The card currently selected from the playing hand.  Note that this has a
   slightly different meaning depending on whether preferences.collapse_hand
   is set or not. */
static int selected_card = -1;

static void draw_text_box(int p);
static void draw_card_box(int p);
static void draw_card_areas(int write_to_screen);
static void table_clear_table(int write_to_screen);
static void table_card_clicked(int card_num);


void table_draw_table(GdkPixmap * pixmap, int x, int y, int w, int h)
{
	if (pixmap == NULL)
		pixmap = table_drawing_area->window;

	/* Display the buffer */
	gdk_draw_drawable(pixmap,
			  table_style->fg_gc[GTK_WIDGET_STATE(table)],
			  table_buf, x, y, x, y, w, h);
}

/* FIXME: maybe this function should be dropped... */
void table_show_table(int x, int y, int w, int h)
{
	table_draw_table(NULL, x, y, w, h);
}

/* Draw the box around the player text */
static void draw_text_box(int p)
{
	int x, y;
	get_text_box_pos(p, &x, &y);
	gdk_draw_rectangle(table_buf,
			   table_style->fg_gc[GTK_WIDGET_STATE(table)],
			   FALSE, x, y, TEXT_BOX_WIDTH, TEXT_BOX_WIDTH);
}

/* Draw the box around the cards */
static void draw_card_box(int p)
{
	int x, y, w, h;

	get_card_box_pos(p, &x, &y);
	get_card_box_dim(p, &w, &h);
	gdk_draw_rectangle(table_buf,
			   table_style->fg_gc[GTK_WIDGET_STATE(table)],
			   FALSE, x, y, w, h);
}

static void draw_card_areas(int write_to_screen)
{
	int p;

	assert(table_ready && game_started);
	assert(!write_to_screen);

	/* Draw card areas */
	for (p = 0; p < ggzcards.num_players; p++) {
		draw_text_box(p);
		draw_card_box(p);
	}
}

static void table_clear_table(int write_to_screen)
{
	assert(table_buf && table_style);

	/* There's no real reason why write_to_screen shouldn't be used, but
	   it's probably not a good idea. */
	assert(!write_to_screen);

	/* Clear the buffer to the style's background color */
	gdk_draw_rectangle(table_buf,
			   table_style->bg_gc[GTK_WIDGET_STATE(table)],
			   TRUE, 0, 0, get_table_width(),
			   get_table_height());

	if (write_to_screen)
		table_show_table(0, 0, get_table_width(),
				 get_table_height());
}

/* Draws a "splash screen" that is shown before the game is initialized. */
static void draw_splash_screen(void)
{
#if 0
	card_t card = { ACE_HIGH, SPADES, 0 };
#endif

	ggz_debug(DBG_TABLE, "Drawing splash screen.");

	assert(!game_started && !table_ready);
	assert(table_buf);

	table_clear_table(FALSE);

#if 0
	/* This is temporarily disabled until I can figure out how to get it
	   to work with the player list. */
	draw_card(card, 0,
		  (get_table_width() - CARDWIDTH) / 2,
		  (get_table_height() - CARDHEIGHT) / 2, table_buf);
#endif

	table_show_table(0, 0, get_table_width(), get_table_height());
}

/* Setup some of the table data structures.  This is called just once
   immediately upon startup. */
void table_initialize(void)
{
	static int call_count = 0;

	/* Just a sanity check; we don't want to call this function twice. */
	assert(call_count == 0);
	call_count++;

	ggz_debug(DBG_TABLE, "Initializing table.");

	/* This starts our drawing code */
	table = g_object_get_data(G_OBJECT(dlg_main), "fixed1");
	table_style = gtk_widget_get_style(table);
	gtk_widget_show(table);

	table_drawing_area = gtk_drawing_area_new();
	gtk_widget_set_size_request(table_drawing_area,
				    get_table_width(), get_table_height());
	gtk_fixed_put(GTK_FIXED(table), table_drawing_area, 0, 0);
	gtk_widget_show(table_drawing_area);
	g_signal_connect(table_drawing_area, "expose_event",
			 GTK_SIGNAL_FUNC(on_table_expose_event), NULL);

	assert(table_drawing_area->window);
	assert(get_table_width() > 0 && get_table_height > 0);
	table_buf = gdk_pixmap_new(table->window,
				   get_table_width(), get_table_height(),
				   -1);
	assert(table_buf);

	/* Redraw and display the table. */
	table_redraw();

	table_show_player_list();
}

/* Setup all table data that's not initialized by table_initialize.  This may
   be called multiple times (for instance, to resize the table), and it's not
   called until the server tells us how big the table must be. */
void table_setup(void)
{
	/* TODO: we really should draw something before this point, since the
	   player needs to see who's playing.  However, for now this will
	   work.  The problem is that before you choose what game you're
	   playing, the server doesn't know how many seats there are so it
	   just tells us 0 - even if there are players already connected. */
	if (ggzcards.num_players == 0) {
		ggz_error_msg("table_setup: num_players is zero.");
		return;
	}
	if (get_max_hand_size() == 0) {
		ggz_error_msg("table_setup: max hand size is zero.");
		return;
	}
	if (get_card_width(0) == 0 || get_card_height(0) == 0) {
		ggz_error_msg("table_setup: max card size is zero.");
		return;
	}

	if (!game_started) {
		/* If we join a game in progress, this can happen.  Probably
		   it should be fixed... */
		ggz_error_msg("ERROR - table_setup() called "
			      "without a game started.");
		game_started = TRUE;
	}

	ggz_debug(DBG_TABLE,
		  "Setting up table." "  Width and height are %d."
		  "  %d players.", get_table_width(),
		  ggzcards.num_players);

	/* We may need to resize the table */
	gtk_widget_set_size_request(table, get_table_width(),
				    get_table_height());
	gtk_widget_set_size_request(table_drawing_area,
				    get_table_width(), get_table_height());

	/* And resize the table buffer... */
	/* Note: I'm not entirely sure how reference counts work for gdk. I
	   assume that when I create a new pixmap (as below), it starts with a 
	   refcount of 1.  In that case, this code should correctly free the
	   pixmap when it is discarded. */
	assert(table_buf);
	g_object_unref(table_buf);
	table_buf = gdk_pixmap_new(table->window,
				   get_table_width(), get_table_height(),
				   -1);

	/* Resize the animation buffer. */
	anim_setup();

	/* _Now_ we're ready to draw stuff. */
	table_ready = TRUE;

	/* Revert to having no selected card. */
	selected_card = -1;

	/* Redraw and display the table. */
	table_redraw();
}


void table_show_player_list(void)
{
	assert(table != NULL);

	if (player_list == NULL) {
		player_list = create_playerlist_widget();
		/* This positioning is just an approximation.  I'd like to
		   center it automatically, but that doesn't seem to work for
		   some reason... */
		gtk_fixed_put(GTK_FIXED(table), player_list, 60, 60);
	}

	gtk_widget_show_all(player_list);

	if (table_ready) {
		int tw = table->allocation.width;
		int th = table->allocation.height;

		int mw = player_list->allocation.width;
		int mh = player_list->allocation.height;

		int x = tw / 2 - mw / 2;
		int y = th / 2 - mh / 2;

		gtk_fixed_move(GTK_FIXED(table), player_list, x, y);
	}

	/* Yeah, the drawing tends to get erased by this clist... */
	table_redraw();
}


void table_hide_player_list(void)
{
	assert(player_list);
	gtk_widget_hide(player_list);
}

void table_cleanup(void)
{
	int p;

	/* This doesn't clean up all the GTK stuff, only the ggz_malloc'd
	   stuff so that the memory check will be clean. */

	for (p = 0; p < MAX_NUM_PLAYERS; p++) {
		if (player_names[p] != NULL)
			ggz_free(player_names[p]);
		if (player_messages[p] != NULL)
			ggz_free(player_messages[p]);
		player_names[p] = player_messages[p] = NULL;
	}
}

static void table_show_player_box(int player, int write_to_screen)
{
	int x, y, w, h;
	const char *name = player_names[player];
	const char *message = player_messages[player];
	int string_y;
	int max_width = 0;
	static PangoLayout *layout = NULL;
	PangoRectangle rect;
	GdkGC *gc = table_style->fg_gc[GTK_WIDGET_STATE(table)];

	static int max_height = 0;

	assert(table_ready);

	if (!layout) {
		/* This variable is static so we only allocate it once. */
		layout = pango_layout_new(gdk_pango_context_get());
	}

	pango_layout_set_font_description(layout, table_style->font_desc);

	get_text_box_pos(player, &x, &y);
	x++;
	y++;

	/* Clear the text box */
	gdk_draw_rectangle(table_buf,
			   table_style->bg_gc[GTK_WIDGET_STATE(table)],
			   TRUE, x, y, TEXT_BOX_WIDTH - 1,
			   TEXT_BOX_WIDTH - 1);

	x += XWIDTH;
	y += XWIDTH;
	w = h = TEXT_WIDTH;

	string_y = y;	/* The y values we're going to draw at. */

	/* Draw the name. */
	if (name) {
		assert(strchr(name, '\n') == NULL);

		pango_layout_set_text(layout, name, -1);
		pango_layout_get_pixel_extents(layout, NULL, &rect);

		max_width = MAX(max_width, rect.width);
		max_height = MAX(max_height, rect.height);

		gdk_draw_layout(table_buf, gc,
				x + (w - rect.width) / 2, string_y,
				layout);

		string_y += max_height + 5;
	}

	/* Draw player message. */
	if (message) {
		char *my_message = ggz_strdup(message);
		char *next = my_message;

		/* This is so ugly!! Is there no better way?? */
		do {
			char *next_after_this = strchr(next, '\n');

			if (next_after_this) {
				*next_after_this = '\0';
				next_after_this++;
			}

			string_y += 3;

			pango_layout_set_text(layout, next, -1);
			pango_layout_get_pixel_extents(layout, NULL,
						       &rect);

			max_height = MAX(max_height, rect.height);
			max_width = MAX(max_width, rect.width);

			gdk_draw_layout(table_buf, gc,
					x + 3, string_y, layout);

			string_y += rect.height;

			next = next_after_this;
		} while (next && *next);

		ggz_free(my_message);
	}

	/* FIXME: we shouldn't call table_setup() from *within* the drawing
	   code */
	if (set_min_text_width(string_y - y)
	    || set_min_text_width(max_width)) {
		table_setup();
	}

	if (write_to_screen)
		table_show_table(x, y, TEXT_BOX_WIDTH - 1,
				 TEXT_BOX_WIDTH - 1);
}

/* Display's a player's name on the table. */
void table_set_name(int player, const char *name)
{
	ggz_debug(DBG_TABLE, "Setting player name: %d => %s.", player,
		  name);

	if (player_names[player])
		ggz_free(player_names[player]);

	player_names[player] = ggz_strdup(name);

	if (table_ready)
		table_show_player_box(player, TRUE);
}

/* Displays a player's message on the table. */
void table_set_player_message(int player, const char *message)
{
	ggz_debug(DBG_TABLE, "Setting player message for %d.", player);

	if (player_messages[player])
		ggz_free(player_messages[player]);

	player_messages[player] = ggz_strdup(message);

	if (table_ready)
		table_show_player_box(player, TRUE);
}

/* Handle a redraw of necessary items, for instance when a Gtk style change is 
   signaled. */
void table_redraw(void)
{
	ggz_debug(DBG_TABLE, "Redrawing table. ");
	if (table_ready) {
		int p;

		/* Complete (zip) any animation in process */
		animation_stop(TRUE);

		/* I really don't know why these are necessary... */
		gtk_widget_grab_focus(dlg_main);
		table_style = gtk_widget_get_style(table);

		/* Redraw everything to the buffer */
		table_clear_table(FALSE);
		draw_card_areas(FALSE);
		table_display_all_hands(FALSE);
		table_show_cards(FALSE);
		for (p = 0; p < ggzcards.num_players; p++)
			table_show_player_box(p, FALSE);

		/* Then draw the whole buffer to the window */
		table_show_table(0, 0, get_table_width(),
				 get_table_height());

		/* There has GOT to be a better way to force the redraw! */
		gdk_window_hide(table_drawing_area->window);
		gdk_window_show(table_drawing_area->window);
	} else {	/* not if (table_ready) */
		if (table_buf)
			draw_splash_screen();
	}
}


/* Redraw our table areas that just got exposed, including all dependent
   widgets. */
void table_handle_expose_event(GdkEventExpose * event)
{
	table_show_table(event->area.x, event->area.y,
			 event->area.width, event->area.height);
}


/* Check for what card has been clicked and process it */
gboolean table_handle_cardclick_event(GdkEventButton * event)
{
	/* this function is tricky.  There are lots of different variables: x, 
	   y, w, h describe the card area itself, including the "selected
	   card" area.  xo and yo describe the offset given by the "selected
	   card".  x1 and y1 are specific coordinates of a card. xdiff and
	   ydiff is the overlapping offset for cards in hand.  There are so
	   many variables because hands can be facing any direction, and it's
	   possible to be playing from *any* hand (at least in theory). */
	int target;
	int which = -1;
	int p = ggzcards.play_hand;	/* player whose hand it is */
	int card_width, card_height;
	int hand_size;

	/* If it's not our turn to play, we don't care. */
	if (ggzcards.state != STATE_PLAY)
		return FALSE;

	assert(p >= 0 && p < ggzcards.num_players);

	ggz_debug(DBG_TABLE,
		  "table_handle_click_event: " "click at %f %f.", event->x,
		  event->y);

	/* This gets all of the layout information from the layout engine.
	   Unfortunately, it's very dense code. */
	card_width = get_card_width(orientation(p));
	card_height = get_card_height(orientation(p));

	/* Calculate our card target */
	hand_size = preferences.collapse_hand
	    ? ggzcards.players[p].hand.hand_size
	    : ggzcards.players[p].u_hand_size;
	for (target = 0; target < hand_size; target++) {
		int x, y;

		if (!preferences.collapse_hand &&
		    !ggzcards.players[p].u_hand[target].is_valid)
			continue;

		get_card_pos(p, target, target == selected_card, &x, &y);

		if (event->x >= x && event->x <= x + card_width
		    /* TODO: generalize for any orientation */
		    && event->y >= y && event->y <= y + card_height)
			which = target;
	}

	if (which == -1)
		/* The click wasn't actually on a card. */
		return FALSE;

	/* Handle the click. */
	table_card_clicked(which);
	return TRUE;
}


/* Right-clicking on the table can pop up a menu. */
gboolean table_handle_menuclick_event(GdkEventButton * event)
{
	int p;

	for (p = 0; p < ggzcards.num_players; p++) {
		int seat_num = ggzcards.players[p].ggzseat;
		GGZSeat seat;
		int w = get_text_width();
		int x, y;

		if (seat_num < 0)
			continue;

		get_text_box_pos(p, &x, &y);
		x += XWIDTH;
		y += XWIDTH;
		if (event->x < x
		    || event->x >= x + w
		    || event->y < y || event->y >= y + w)
			continue;

		seat = ggzmod_get_seat(client_get_ggzmod(), seat_num);
		popup_player_menu(&seat, NULL, event->button);
		return TRUE;
	}

	return FALSE;
}


/* Handle a card that is clicked by either popping it forward or playing it if 
   it is already selected. */
static void table_card_clicked(int card_num)
{
	ggz_debug(DBG_TABLE, "table_card_clicked: Card %d clicked.",
		  card_num);

	if (card_num == selected_card || preferences.single_click_play) {
		/* If you click on the already selected card, it gets played.
		   There's also an option so that you need only click once. */
		selected_card = -1;
		game_play_card(card_num);
	} else {
		/* Pop the card forward and select it */
		selected_card = card_num;
		table_display_hand(ggzcards.play_hand, TRUE);
	}
}

static void clear_card_area(int p)
{
	int cx, cy, cw, ch;

	/* get layout information */
	get_full_card_area(p, &cx, &cy, &cw, &ch);

	/* Clean the total drawing area */
	gdk_draw_rectangle(table_buf,
			   table_style->bg_gc[GTK_WIDGET_STATE(table)],
			   TRUE, cx, cy, cw, ch);
}

static void show_card_area(int p)
{
	int cx, cy, cw, ch;

	/* get layout information */
	get_full_card_area(p, &cx, &cy, &cw, &ch);

	table_show_table(cx, cy, cw, ch);
}

/* Exposed function to show one player's hand. */
void table_display_hand(int p, int write_to_screen)
{
	int i;
	card_t table_card = table_cards[p];
	int hand_size;

#if 0
	/* It looks like the server violates this, although it's probably a
	   bug in the server. */
	assert(table_ready && game_started);
#endif

	/* The server may send out a hand of size 0 when we first connect, but 
	   we just want to ignore it. */
	if (!table_ready)
		return;

	ggz_debug(DBG_TABLE, "Displaying hand for player %d.", p);

	/* redraw outer rectangle */
	clear_card_area(p);
	draw_card_box(p);

	/* Draw the cards */
	hand_size = preferences.collapse_hand
	    ? ggzcards.players[p].hand.hand_size
	    : ggzcards.players[p].u_hand_size;
	for (i = 0; i < hand_size; i++) {
		card_t card;
		int x, y;

		if (preferences.collapse_hand)
			card = ggzcards.players[p].hand.cards[i];
		else {
			if (!ggzcards.players[p].u_hand[i].is_valid)
				continue;
			card = ggzcards.players[p].u_hand[i].card;
		}

		if (card.face >= 0 && card.face == table_card.face &&
		    card.suit >= 0 && card.suit == table_card.suit &&
		    card.deck >= 0 && card.deck == table_card.deck)
			/* if the player has a card on the table _and_ it
			   matches this card, skip over it. */
			continue;
		get_card_pos(p, i,
			     p == ggzcards.play_hand && i == selected_card,
			     &x, &y);

		draw_card(card, orientation(p), x, y, table_buf);
	}

	/* And refresh the on-screen image for card areas */
	if (write_to_screen)
		show_card_area(p);
}

/* table_display_all_hands exposed function to show all players' hands */
void table_display_all_hands(int write_to_screen)
{
	int p;
	for (p = 0; p < ggzcards.num_players; p++)
		table_display_hand(p, write_to_screen);
}


/* Exposed function to show one player's cards on the table area. */
void table_show_card(int player, card_t card, int write_to_screen)
{
	int x, y;

	assert(table_ready && game_started);
	assert(card.face != -1 && card.suit != -1);

	get_tablecard_pos(player, &x, &y);
	draw_card(card, 0, x, y, table_buf);

	if (write_to_screen)
		table_show_table(x, y, CARDWIDTH, CARDHEIGHT);
}

/* Exposed function to show all four cards on the table area. */
void table_show_cards(int write_to_screen)
{
	int table_x, table_y, table_w, table_h, p;

	assert(table_ready && game_started);

	get_table_dim(&table_x, &table_y, &table_w, &table_h);
	gdk_draw_rectangle(table_buf,
			   table_style->bg_gc[GTK_WIDGET_STATE(table)],
			   TRUE, table_x, table_y, table_w, table_h);

	for (p = 0; p < ggzcards.num_players; p++) {
		card_t card = ggzcards.players[p].table_card;
		if (card.suit >= 0 && card.face >= 0) {
			int card_x, card_y;
			get_tablecard_pos(p, &card_x, &card_y);
			draw_card(card, 0, card_x, card_y, table_buf);
		}
	}

	if (write_to_screen)
		table_show_table(table_x, table_y, table_w, table_h);
}
