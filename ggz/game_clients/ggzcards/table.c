/* 
 * File: table.c
 * Author: Rich Gade, Jason Short
 * Project: GGZCards Client
 * Date: 08/14/2000
 * Desc: Routines to handle the Gtk game table
 * $Id: table.c 3349 2002-02-13 07:32:47Z jdorje $
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
#include "common.h"

#include "animation.h"
#include "main.h"
#include "game.h"
#include "table.h"
#include "layout.h"
#include "dlg_bid.h"
#include "dlg_main.h"

#include "cards-1.xpm"
#include "cards-2.xpm"
#include "cards-3.xpm"
#include "cards-4.xpm"

#include "cards-b1.xpm"
#include "cards-b2.xpm"
#include "cards-b3.xpm"
#include "cards-b4.xpm"

GtkRcStyle *fixed_font_style = NULL;

/* Table data */
GtkWidget *table;		/* widget containing the whole table */
GtkStyle *table_style;		/* Style for the table */
static GdkPixmap *table_buf = NULL;	/* backing store for the table */


/* Card front pictures for each of the 4 orientations */
GdkPixmap *card_fronts[4];
static GdkPixmap *card_backs[4];

static GtkWidget *l_name[MAX_NUM_PLAYERS] = { NULL };	/* player names */
static GtkWidget *label[MAX_NUM_PLAYERS] = { NULL };	/* player labels */

static gboolean table_ready = FALSE;

static int selected_card = -1;	/* the card currently selected from the
				   playing hand */

void table_show_table(int x, int y, int w, int h);
static void draw_text_box(int p);
static void draw_card_box(int p);
static void draw_card_areas(int write_to_screen);
static void table_clear_table(int write_to_screen);
static void table_card_clicked(int card_num);
static void table_display_all_hands(int write_to_screen);

void table_show_table(int x, int y, int w, int h)
{
	/* Display the buffer */
	gdk_draw_pixmap(table->window,
			table_style->fg_gc[GTK_WIDGET_STATE(table)],
			table_buf, x, y, x, y, w, h);
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
			   TRUE, 0, 0, get_table_width(), get_table_height());
	
	if (write_to_screen)
		table_show_table(0, 0, get_table_width(), get_table_height());
}

/* Draws a "splash screen" that is shown before the game is initialized. */
static void draw_splash_screen(void)
{
	card_t card = { ACE_HIGH, SPADES, 0 };

	ggz_debug("table", "Drawing splash screen.");

	assert(!game_started && !table_ready);
	assert(table_buf);
	assert(ggzcards.num_players == 0);

	table_clear_table(FALSE);
	draw_card(card, 0,
		  (get_table_width() - CARDWIDTH) / 2,
		  (get_table_height() - CARDHEIGHT) / 2, table_buf);
		
	table_show_table(0, 0, get_table_width(), get_table_height());
}

/* Setup some of the table data structures.  This is called just once
   immediately upon startup. */
void table_initialize(void)
{
	GdkBitmap *mask;
	gchar **xpm_fronts[4] =
		{ cards_xpm, cards_2_xpm, cards_3_xpm, cards_4_xpm };
	gchar **xpm_backs[4] =
		{ cards_b1_xpm, cards_b2_xpm, cards_b3_xpm, cards_b4_xpm };
	int i;
	static int call_count = 0;

	/* Just a sanity check; we don't want to call this function twice. */
	assert(call_count == 0);
	call_count++;

	ggz_debug("table", "Initializing table.");

	/* This starts our drawing code */
	table = gtk_object_get_data(GTK_OBJECT(dlg_main), "fixed1");
	table_style = gtk_widget_get_style(table);
	gtk_widget_show(table);

	/* build pixmaps from the xpms */
	for (i = 0; i < 4 /* 4 orientations */ ; i++) {
		card_fronts[i] =
			gdk_pixmap_create_from_xpm_d(table->window, &mask,
						     &table_style->
						     bg[GTK_STATE_NORMAL],
						     (gchar **)
						     xpm_fronts[i]);
		card_backs[i] =
			gdk_pixmap_create_from_xpm_d(table->window, &mask,
						     &table_style->
						     bg[GTK_STATE_NORMAL],
						     (gchar **) xpm_backs[i]);
		if (!card_fronts[i] || !card_backs[i])
			ggz_debug("table", "ERROR: "
				  "couldn't load card pixmaps for orientation %d.",
				  i);
	}

	assert(table->window);
	assert(get_table_width() > 0 && get_table_height > 0);
	table_buf = gdk_pixmap_new(table->window,
				   get_table_width(), get_table_height(), -1);
	assert(table_buf);
	
	/* Redraw and display the table. */
	table_redraw();
}

/* Setup all table data that's not initialized by table_initialize.  This may
   be called multiple times (for instance, to resize the table), and it's not
   called until the server tells us how big the table must be. */
void table_setup(void)
{
	int x, y, p;

	/* TODO: we really should draw something before this point, since the
	   player needs to see who's playing.  However, for now this will
	   work.  The problem is that before you choose what game you're
	   playing, the server doesn't know how many seats there are so it
	   just tells us 0 - even if there are players already connected. */
	if (ggzcards.num_players == 0 || table_max_hand_size == 0)
		return;
		
	if (!game_started) {
		/* If we join a game in progress, this can happen.  Probably
		   it should be fixed... */
		ggz_error_msg("ERROR - table_setup() called "
		              "without a game started.");
		game_started = TRUE;	
	}

	ggz_debug("table", "Setting up table." "  Width and height are %d."
		  "  %d players.", get_table_width(), ggzcards.num_players);

	/* We may need to resize the table */
	gtk_widget_set_usize(table, get_table_width(), get_table_height());

	/* And resize the table buffer... */
	/* Note: I'm not entirely sure how reference counts work for gdk.
	   I assume that when I create a new pixmap (as below), it starts
	   with a refcount of 1.  In that case, this code should correctly
	   free the pixmap when it is discarded. */
	assert(table_buf);
	gdk_pixmap_unref(table_buf);
	table_buf = gdk_pixmap_new(table->window,
				   get_table_width(), get_table_height(), -1);

	/* _Now_ we're ready to draw stuff. */
	table_ready = TRUE;

	/* Revert to having no selected card. */
	selected_card = -1;

	/* Add text labels to display */
	for (p = 0; p < ggzcards.num_players; p++) {
		get_text_box_pos(p, &x, &y);

		/* Name entries */
		if (l_name[p]) {
			gtk_widget_hide(l_name[p]);
			gtk_widget_destroy(l_name[p]);
		}
		l_name[p] = gtk_label_new(ggzcards.players[p].name);
		gtk_fixed_put(GTK_FIXED(table), l_name[p], x + 3, y + 1);
		gtk_widget_set_usize(l_name[p], TEXT_BOX_WIDTH - 6, -1);
		gtk_widget_show(l_name[p]);

		/* TODO: get the old label before we update */
		if (label[p]) {
			gtk_widget_hide(label[p]);
			gtk_widget_destroy(label[p]);
		}
		label[p] = gtk_label_new(NULL);
		gtk_fixed_put(GTK_FIXED(table), label[p], x + 3, y + 20);
		gtk_widget_set_usize(label[p], TEXT_BOX_WIDTH - 6, -1);
		gtk_label_set_justify(GTK_LABEL(label[p]), GTK_JUSTIFY_LEFT);
		gtk_widget_show(label[p]);
	}

	/* Redraw and display the table. */
	table_redraw();
}

/* Display's a player's name on the table. */
void table_set_name(int player, const char *name)
{
	ggz_debug("table", "Setting player name: %d => %s.", player, name);
	if (l_name[player] != NULL)
		gtk_label_set_text(GTK_LABEL(l_name[player]), name);
}

/* Displays a player's message on the table. */
void table_set_player_message(int player, const char *message)
{
	ggz_debug("table", "Setting player message for %d.", player);
	if (label[player] != NULL)
		gtk_label_set_text(GTK_LABEL(label[player]), message);
}

/* Handle a redraw of necessary items, for instance when a Gtk style change
   is signaled. */
void table_redraw(void)
{
	ggz_debug("table", "Redrawing table. ");
	if (table_ready) {
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

		/* Then draw the whole buffer to the window */
		table_show_table(0, 0, get_table_width(), get_table_height());
		
		/* There has GOT to be a better way to force the redraw! */
		gdk_window_hide(table->window);
		gdk_window_show(table->window);
	} else {		/* not if (table_ready) */
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
void table_handle_click_event(GdkEventButton * event)
{
	/* this function is tricky.  There are lots of different variables:
	   x, y, w, h describe the card area itself, including the "selected
	   card" area.  xo and yo describe the offset given by the "selected
	   card".  x1 and y1 are specific coordinates of a card. xdiff and
	   ydiff is the overlapping offset for cards in hand.  There are so
	   many variables because hands can be facing any direction, and it's
	   possible to be playing from *any* hand (at least in theory). */
	int target;
	int which = -1;
	int x_outer, y_outer, w_outer, h_outer, xo, yo;
	int x, y, x1, y1;
	int p = ggzcards.play_hand;	/* player whose hand it is */
	float xdiff, ydiff;
	int card_width, card_height;

	/* If it's not our turn to play, we don't care. */
	if (ggzcards.state != STATE_PLAY)
		return;
		
	assert(p >= 0 && p < ggzcards.num_players);

	ggz_debug("table", "table_handle_click_event: " "click at %f %f.",
		  event->x, event->y);

	/* This gets all of the layout information from the layout engine.
	   Unfortunately, it's very dense code. */
	get_full_card_area(p, &x_outer, &y_outer, &w_outer, &h_outer, &xo,
			   &yo);
	get_card_offset(p, &xdiff, &ydiff);
	get_inner_card_area_pos(p, &x, &y);
	get_card_size(orientation(p), &card_width, &card_height);

	/* make sure it's within the card area */
	if (event->x < x_outer || event->x > x_outer + w_outer
	    || event->y < y_outer || event->y > y_outer + h_outer)
		return;

	/* Calculate our card target */
	for (target = 0; target < ggzcards.players[p].hand.hand_size;
	     target++) {
		x1 = x + .5 + (target * xdiff);
		y1 = y + .5 + (target * ydiff);
		if (target == selected_card) {
			/* account for the selected card being offset */
			x1 += xo;
			y1 += yo;
		}
		if (event->x >= x1 && event->x <= x1 + card_width
		    /* TODO: generalize for any orientation */
		    && event->y >= y1 && event->y <= y1 + card_height)
			which = target;
	}

	if (which == -1)
		/* The click wasn't actually on a card. */
		return;

	/* Handle the click. */
	table_card_clicked(which);
}


/* Handle a card that is clicked by either popping it forward or playing it
   if it is already selected. */
static void table_card_clicked(int card_num)
{
	ggz_debug("table", "table_card_clicked: Card %d clicked.", card_num);

	if (card_num == selected_card) {
		/* If you click on the already selected card, it gets played. */
		selected_card = -1;
		game_play_card(card_num);
	} else {
		/* Pop the card forward and select it */
		selected_card = card_num;
		table_display_hand(ggzcards.play_hand, TRUE);
	}
}

/* Returns the coordinates of the card out of the XPM file. */
void get_card_coordinates(card_t card, int orientation, int *x, int *y)
{
	int xc = 0, yc = 0;
	int xp, yp;
	int height = (orientation % 2 == 0) ? CARDHEIGHT : CARDWIDTH;
	int width = (orientation % 2 == 0) ? CARDWIDTH : CARDHEIGHT;
	
	/* We don't care about the deck, but the rest had better
	   be accurate. */
	assert(card.face >= ACE_LOW && card.face <= ACE_HIGH
	       && card.suit >= CLUBS && card.suit <= SPADES
	       && orientation >= 0 && orientation < 4);

	/* This hack converts the "face" value of the card into the 0-12
	   range. */
	yp = card.face;
	if (yp == ACE_HIGH)
		yp = ACE_LOW;
	yp--;

	xp = card.suit;

	/* y is measured from the top; x from the left.  This just rotates
	   the grid as necessary. */
	switch (orientation) {
	case 0:
		xc = xp;
		yc = yp;
		break;
	case 1:
		xc = 13 - yp - 1;
		yc = xp;
		break;
	case 2:		/* just mirror everything */
		xc = 4 - xp - 1;
		yc = 13 - yp - 1;
		break;
	case 3:
		xc = yp;
		yc = 4 - xp - 1;
		break;
	}

	*x = xc * width;
	*y = yc * height;
}

/* Draws the given card at the given location with the given orientation. */
void draw_card(card_t card, int orientation, int x, int y, GdkPixmap * image)
{
	int width, height;
	int xc = 0, yc = 0;
	int show = (card.suit != -1 && card.face != -1);

	assert(orientation >= 0 && orientation < 4);

	/* First find the width/height the card will need at this
	   orientation. */
	get_card_size(orientation, &width, &height);

	if (show)
		get_card_coordinates(card, orientation, &xc, &yc);
	else {
		/* based on there being 4 different backs */
		/* TODO: do different decks differently */
		int xy[4][2] =
			{ {2 * CARDWIDTH, 0}, {0, 2 * CARDWIDTH}, {CARDWIDTH,
								   0}, {0,
									CARDWIDTH}
		};
		xc = xy[orientation][0];
		yc = xy[orientation][1];
	}
	gdk_draw_pixmap(image,
			table_style->fg_gc[GTK_WIDGET_STATE(table)],
			show ? card_fronts[orientation] :
			card_backs[orientation], xc, yc, x, y, width, height);
}

/* Exposed function to show one player's hand. */
void table_display_hand(int p, int write_to_screen)
{
	int i, x, y;
	int x_outer, y_outer;
	int cx, cy, cw, ch, cxo, cyo;
	float ow, oh;
	card_t table_card = table_cards[p];

#if 0
	/* It looks like the server violates this, although it's probably a
	   bug in the server. */
	assert(table_ready && game_started);
#endif

	/* The server may send out a hand of size 0 when we first connect,
	   but we just want to ignore it. */
	if (!table_ready)
		return;

	ggz_debug("table", "Displaying hand for player %d.", p);

	/* get layout information */
	get_full_card_area(p, &x_outer, &y_outer, &cw, &ch, &cxo, &cyo);
	get_inner_card_area_pos(p, &cx, &cy);
	get_card_offset(p, &ow, &oh);

	/* Clean the total drawing area */
	gdk_draw_rectangle(table_buf,
			   table_style->bg_gc[GTK_WIDGET_STATE(table)],
			   TRUE, x_outer, y_outer, cw, ch);

	/* redraw outer rectangle */
	draw_card_box(p);

	/* Draw the cards */
	for (i = 0; i < ggzcards.players[p].hand.hand_size; i++) {
		card_t card = ggzcards.players[p].hand.card[i];
		if (card.face >= 0 && card.face == table_card.face &&
		    card.suit >= 0 && card.suit == table_card.suit &&
		    card.deck >= 0 && card.deck == table_card.deck)
			/* if the player has a card on the table _and_ it
			   matches this card, skip over it. */
			continue;
		x = cx + 0.5 + (i * ow);
		y = cy + 0.5 + (i * oh);
		if (i == selected_card && p == ggzcards.play_hand) {
			x += cxo;
			y += cyo;
		}
		draw_card(card, orientation(p), x, y, table_buf);
	}

	/* And refresh the on-screen image for card areas */
	if (write_to_screen)
		table_show_table(x_outer, y_outer, cw, ch);
}

/* table_display_all_hands exposed function to show all players' hands */
static void table_display_all_hands(int write_to_screen)
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
