/* 
 * File: table.c
 * Author: Rich Gade, Jason Short
 * Project: GGZCards Client
 * Date: 08/14/2000
 * Desc: Routines to handle the Gtk game table
 * $Id: table.c 2384 2001-09-07 08:40:00Z jdorje $
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

#include "main.h"
#include "game.h"
#include "table.h"
#include "layout.h"
#include "hand.h"
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

static GdkPixmap *table_buf = NULL;
static GdkPixmap *card_fronts[4];
static GdkPixmap *card_backs[4];
static GtkStyle *f1_style;
static GtkWidget *f1;
static GtkWidget *l_name[MAX_NUM_PLAYERS] = { NULL };
static GtkWidget *label[MAX_NUM_PLAYERS] = { NULL };	/* player labels; put 
							   in place of old
							   bid/tricks/score */
/* static GtkWidget *msglabel = NULL; *//* global label; put in place of old l_trump */
static gboolean table_initialized = FALSE;

static int selected_card = -1;	/* the card currently selected from the
				   playing hand */

#ifdef ANIMATION
static struct {
	card_t card;
	int dest_x, dest_y;
	int card_x, card_y;
	float cur_x, cur_y;
	float step_x, step_y;
	gint cb_tag;
} anim;
#endif /* ANIMATION */

static void table_card_clicked(int);
static void table_card_play(int, int);

static void table_show_card(int, card_t);
static void draw_card(card_t, int, int, int);

#ifdef ANIMATION
static gint table_animation_callback(gpointer);
static void table_animation_trigger(card_t, int, int, int, int);
#endif /* ANIMATION */

static void table_show_table(int x, int y, int w, int h)
{
	/* Display the buffer */
	gdk_draw_pixmap(f1->window,
			f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
			table_buf, x, y, x, y, w, h);
}

static void draw_text_box(int p)
{
	int x, y;
	get_text_box_pos(p, &x, &y);
	gdk_draw_rectangle(table_buf,
			   f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
			   FALSE, x, y, TEXT_BOX_WIDTH, TEXT_BOX_WIDTH);
}

static void draw_card_box(int p)
{
	int x, y, w, h;

	get_card_box_pos(p, &x, &y);
	get_card_box_dim(p, &w, &h);
	gdk_draw_rectangle(table_buf,
			   f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
			   FALSE, x, y, w, h);
}

static void draw_card_areas()
{
	int p;
	/* Clear our buffer to the style's background color */
	gdk_draw_rectangle(table_buf,
			   f1_style->bg_gc[GTK_WIDGET_STATE(f1)],
			   TRUE, 0, 0, get_table_width(), get_table_height());

	/* Draw card areas */
	for (p = 0; p < game.num_players; p++) {
		draw_text_box(p);
		draw_card_box(p);
	}
}

/* table_initialize() Setup and draw the table areas on the fixed1 dialog
   item */
void table_initialize(void)
{
	GdkBitmap *mask;
	card_t card = { ACE_HIGH, SPADES, 0 };

	gchar **xpm_fronts[4] =
		{ cards_xpm, cards_2_xpm, cards_3_xpm, cards_4_xpm };
	gchar **xpm_backs[4] =
		{ cards_b1_xpm, cards_b2_xpm, cards_b3_xpm, cards_b4_xpm };
	int i;

	/* This starts our drawing code */
	f1 = gtk_object_get_data(GTK_OBJECT(dlg_main), "fixed1");
	f1_style = gtk_widget_get_style(f1);
	gtk_widget_show(f1);

	/* build pixmaps from the xpms */
	for (i = 0; i < 4 /* 4 orientations */ ; i++) {
		card_fronts[i] =
			gdk_pixmap_create_from_xpm_d(f1->window, &mask,
						     &f1_style->
						     bg[GTK_STATE_NORMAL],
						     (gchar **)
						     xpm_fronts[i]);
		card_backs[i] =
			gdk_pixmap_create_from_xpm_d(f1->window, &mask,
						     &f1_style->
						     bg[GTK_STATE_NORMAL],
						     (gchar **) xpm_backs[i]);
		if (!card_fronts[i] || !card_backs[i])
			client_debug
				("ERROR: couldn't load card pixmaps for orientation %d.",
				 i);
	}


	if (game.num_players > 0) {
		table_setup();
	} else {
		table_buf = gdk_pixmap_new(f1->window,
					   get_table_width(),
					   get_table_height(), -1);
		/* this draws a "splash screen */
		gdk_draw_rectangle(table_buf,
				   f1_style->bg_gc[GTK_WIDGET_STATE(f1)],
				   TRUE,
				   0, 0,
				   get_table_width(), get_table_height());
		draw_card(card, 0,
			  (get_table_width() - CARDWIDTH) / 2,
			  (get_table_height() - CARDHEIGHT) / 2);
		table_show_table(0, 0, get_table_width(), get_table_height());
	}

	table_initialized = TRUE;
}

void table_setup()
{
	int x, y, p;

	/* TODO: we really _have_ to draw something before this point, since
	   the player needs to see who's playing.  However, for now this will
	   work */
	if (game.num_players == 0 || game.max_hand_size == 0)
		return;

	client_debug
		("Setting up table.  Width and height are %d.  %d players.",
		 get_table_width(), game.num_players);

	gtk_widget_set_usize(f1, get_table_width(), get_table_height());

	/* TODO: avoid memory leak */
	table_buf = gdk_pixmap_new(f1->window,
				   get_table_width(), get_table_height(), -1);

	draw_card_areas();

	/* Add text labels to display */
	for (p = 0; p < game.num_players; p++) {
		get_text_box_pos(p, &x, &y);

		/* Name entries */
		if (l_name[p]) {
			gtk_widget_hide(l_name[p]);
			gtk_widget_destroy(l_name[p]);
		}
		l_name[p] = gtk_label_new(game.players[p].name);
		gtk_fixed_put(GTK_FIXED(f1), l_name[p], x + 3, y + 1);
		gtk_widget_set_usize(l_name[p], TEXT_BOX_WIDTH - 6, -1);
		gtk_widget_show(l_name[p]);

		/* TODO: get the old label before we update */
		if (label[p]) {
			gtk_widget_hide(label[p]);
			gtk_widget_destroy(label[p]);
		}
		label[p] = gtk_label_new(NULL);
		gtk_fixed_put(GTK_FIXED(f1), label[p], x + 3, y + 20);
		gtk_widget_set_usize(label[p], TEXT_BOX_WIDTH - 6, -1);
		gtk_label_set_justify(GTK_LABEL(label[p]), GTK_JUSTIFY_LEFT);
		gtk_widget_show(label[p]);
	}

	/* Display the buffer */
	if (game.num_players > 0 && game.max_hand_size > 0)
		table_show_table(0, 0, get_table_width(), get_table_height());

	selected_card = -1;
}

void table_set_player_message(int p, const char *message)
{
	if (label[p] != NULL)
		gtk_label_set_text(GTK_LABEL(label[p]), message);
}

void table_set_global_message(const char *mark, const char *message)
{
	assert(table_initialized);
	if (!*mark)
		/* this is the "global" message */
		/* gtk_label_set_text(GTK_LABEL(msglabel), message); */
		messagebar_message(message);
	else if (!strcmp(mark, "game")) {
		/* this is the game's name; we just adjust the title bar */
		char title[50];
		snprintf(title, sizeof(title), _("GGZ Gaming Zone - %s"),
			 message);
		gtk_window_set_title(GTK_WINDOW(dlg_main), title);
	} else
		menubar_message(mark, message);
}

/* table_style_change() Handle a redraw of necessary items when a Gtk style
   change is signaled. */
void table_style_change(void)
{
	if (!table_initialized)
		return;

	gtk_widget_grab_focus(dlg_main);

	f1_style = gtk_widget_get_style(f1);

	draw_card_areas();

#ifdef ANIMATION
	/* If there's an animation in progress, zip it */
	if (game.state == WH_STATE_ANIM)
		table_animation_zip(FALSE);
#endif /* ANIMATION */

	/* Redisplay any cards on table and in hands */
	table_display_all_hands();
	table_show_cards();

	/* There has GOT to be a better way to force the redraw! */
	gdk_window_hide(f1->window);
	gdk_window_show(f1->window);
}


/* table_handle_expose_event() Redraw our table areas that just got exposed,
   including all dependent widgets. */
void table_handle_expose_event(GdkEventExpose * event)
{
	table_show_table(event->area.x, event->area.y,
			 event->area.width, event->area.height);
}


/* table_handle_click_event() Check for what card has been clicked and
   process it */
void table_handle_click_event(GdkEventButton * event)
{
	/* this function is tricky.  There are lots of different variables:
	   x, y, w, h describe the card area itself, including the "selected
	   card" area.  xo and yo describe the offset given by the "selected
	   card".  x1 and y1 are specific coordinates of a card xdiff and
	   ydiff is the overlapping offset for cards in hand. there are so
	   many variables because hands can be facing any direction! */
	int target;
	int which = -1;
	int x_outer, y_outer, w_outer, h_outer, xo, yo;
	int x, y, x1, y1;
	int p = game.play_hand;	/* player whose hand it is */
	float xdiff, ydiff;
	int card_width, card_height;

	client_debug
		("table_handle_click_event: click at %d %d.  State is %d.",
		 event->x, event->y, game.state);

	/* Real quick, see if we even care */
	if (game.state != WH_STATE_PLAY)
		return;

	/* this gets all of the layout information... */
	get_full_card_area(p, &x_outer, &y_outer, &w_outer, &h_outer, &xo,
			   &yo);
	get_card_offset(p, &xdiff, &ydiff);
	get_inner_card_area_pos(p, &x, &y);
	get_card_size(orientation(p), &card_width, &card_height);

	/* make sure it's at least within the card area */
	if (event->x < x_outer)
		return;
	if (event->x > x_outer + w_outer)
		return;
	if (event->y < y_outer)
		return;
	if (event->y > y_outer + h_outer)
		return;

	/* Calculate our card target */
	for (target = 0; target < game.players[p].hand.hand_size; target++) {
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
		return;

	table_card_clicked(which);
}


/* table_card_clicked() Handle a card that has been clicked by either popping 
   it forward or playing it if it is already selected. */
static void table_card_clicked(int card)
{
	int p = game.play_hand;

	client_debug("table_card_clicked: Card %d clicked.", card);

	if (card == selected_card) {
		selected_card = -1;

		/* Start the graphic animation */
		table_card_play(p, card);

		/* We don't remove the card from our hand until we have
		   validation that it's been played. Graphically, the
		   table_card is skipped over when drawing the hand. */

		/* Call the game function to notify server of play */
		game_play_card(game.players[p].hand.card[card]);
	} else {
		/* Pop the card forward and select it */
		selected_card = card;
		table_display_hand(p);
	}
}

static void get_card_coordinates(card_t card, int orientation,	/* 0 to 3 */
				 int *x, int *y)
{
	int xc = 0, yc = 0;
	int xp, yp;
	int height = (orientation % 2 == 0) ? CARDHEIGHT : CARDWIDTH;
	int width = (orientation % 2 == 0) ? CARDWIDTH : CARDHEIGHT;

	/* this converts the "face" value of the card into the 0-12 range. */
	yp = card.face;
	if (yp == ACE_HIGH)
		yp = ACE_LOW;
	yp--;

	xp = card.suit;

	/* y is measured from the top; x from the left.  This just rotates
	   the grid. */
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

static void draw_card(card_t card, int orientation,	/* 0 to 3 */
		      int x, int y)
{
	int width, height;
	int xc = 0, yc = 0;
	int show = (card.suit != -1 && card.face != -1);

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
	gdk_draw_pixmap(table_buf,
			f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
			show ? card_fronts[orientation] :
			card_backs[orientation], xc, yc, x, y, width, height);
}

/* table_card_play() Move the selected card out onto the playing area. */
static void table_card_play(int p, int card)
{
	game.players[p].table_card = game.players[p].hand.card[card];

	/* Draw the cards, eliminating the card in play */
	table_display_hand(p);

	/* Now redraw the hand. */
	table_display_hand(p);

	/* And refresh the on-screen image */
	table_show_table(TEXT_BOX_WIDTH + XWIDTH,
			 get_table_height() - TEXT_BOX_WIDTH - 2 * XWIDTH,
			 CARD_BOX_WIDTH, CARDHEIGHT);

#ifdef ANIMATION
	/* Setup and trigger the card animation */
	table_animation_trigger(game.players[p].hand.card[card], x, y, 199,
				242);
#endif /* ANIMATION */
}


#ifdef ANIMATION
/* table_animation_trigger() Function to setup and trigger a card animation */
static void table_animation_trigger(card_t card, int x1, int y1, int x2,
				    int y2)
{
#define FRAMES		15
#define DURATION	500	/* In milliseconds */

	/* Store all our info */
	anim.card = card;
	get_card_coordinates(card, 0, &anim.card_x, &anim.card_y);
	anim.cur_x = x1;
	anim.cur_y = y1;
	anim.dest_x = x2;
	anim.dest_y = y2;

	/* This sets up 15 frames of animation */
	anim.step_x = (x1 - x2) / (float) FRAMES;
	anim.step_y = (y1 - y2) / (float) FRAMES;

	/* This sets up our timeout callback */
	anim.cb_tag = gtk_timeout_add(DURATION / FRAMES,
				      table_animation_callback, NULL);

	set_game_state(WH_STATE_ANIM);
}


/* table_animation_callback() Handle one frame of card animation, this is
   triggered by a GtkTimeout setup in table_animation_trigger(). */
gint table_animation_callback(gpointer ignored)
{
	float new_x, new_y;
	int ref_x, ref_y;

	/* Calculate our next move */
	new_x = anim.cur_x - anim.step_x;
	if (abs(new_x - anim.dest_x) < 2)
		new_x = anim.dest_x;
	new_y = anim.cur_y - anim.step_y;
	if (abs(new_y - anim.dest_y) < 2)
		new_y = anim.dest_y;

	/* Draw our new card position */
	gdk_draw_pixmap(table_buf,
			f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
			card_fronts[0],
			anim.card_x, anim.card_y,
			new_x, new_y, CARDWIDTH, CARDHEIGHT);

	/* And refresh the on-screen image */
	if (new_x < anim.cur_x)
		ref_x = new_x;
	else
		ref_x = anim.cur_x;
	if (new_y < anim.cur_y)
		ref_y = new_y;
	else
		ref_y = anim.cur_y;
	table_show_table(ref_x, ref, y,
			 CARDWIDTH + abs(anim.step_x) + 2,
			 CARDHEIGHT + abs(anim.step_y) + 2);

	/* Update our information for next time */
	anim.cur_x = new_x;
	anim.cur_y = new_y;

	/* If we are there, stop the animation process */
	if (new_x == anim.dest_x && new_y == anim.dest_y) {
		set_game_state(WH_STATE_WAIT);
		return FALSE;
	}

	/* Continue animating */
	return TRUE;
}


/* table_animation_abort() Exposed function to abort the animation process */
void table_animation_abort(void)
{
	/* First, kill off the animation callback */
	if (game.state == WH_STATE_ANIM) {
		gtk_timeout_remove(anim.cb_tag);
		set_game_state(WH_STATE_WAIT);
	}

	/* The caller is assumed to have restored the card to the hand */
	/* so we can redraw the full hand and should be done */
	table_display_hand(0);
}


/* table_animation_zip Exposed function to zip to the finish of an animation
   sequence so that a new one may be started */
void table_animation_zip(gboolean restore)
{
	/* First, kill off the animation callback */
	if (game.state == WH_STATE_ANIM) {
		gtk_timeout_remove(anim.cb_tag);
		set_game_state(WH_STATE_WAIT);
	}

	/* And move the card to it's final resting place */
	gdk_draw_pixmap(table_buf,
			f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
			card_fronts[0],
			anim.card_x, anim.card_y,
			anim.dest_x, anim.dest_y, CARDWIDTH, CARDHEIGHT);
	table_show_table(anim.dest_x, anim.dest_y, CARDWIDTH, CARDHEIGHT);
}
#endif /* ANIMATION */

/* table_set_name() Exposed function to set name on display */
void table_set_name(int p, char *name)
{
	if (l_name[p] != NULL)
		gtk_label_set_text(GTK_LABEL(l_name[p]), name);
}


/* table_set_p_message() Exposed function to set player message on display */
void table_set_p_message(int p, char *msg)
{
	gtk_label_set_text(GTK_LABEL(label[p]), msg);
}

/* table_display_hand() Exposed function to show one player's hand */
/* TODO: make your hand and other players' hands work the same way the way
   they should work: none of this -1 to represent an empty card empty cards
   are removed, UNKNOWN_CARD represents an unknown (unseen) card.  Draw all
   hands equally, but show only the backs for any unknown cards. */
void table_display_hand(int p)
{
	int i, x, y;
	int x_outer, y_outer;
	int cx, cy, cw, ch, cxo, cyo;
	float ow, oh;
	card_t table_card = game.players[p].table_card;

	client_debug("     Displaying hand for player %d.", p);

	/* get layout information */
	get_full_card_area(p, &x_outer, &y_outer, &cw, &ch, &cxo, &cyo);
	get_inner_card_area_pos(p, &cx, &cy);
	get_card_offset(p, &ow, &oh);

	/* Clean the total drawing area */
	gdk_draw_rectangle(table_buf,
			   f1_style->bg_gc[GTK_WIDGET_STATE(f1)],
			   TRUE, x_outer, y_outer, cw, ch);

	/* redraw outer rectangle */
	draw_card_box(p);

	/* Draw the cards */
	for (i = 0; i < game.players[p].hand.hand_size; i++) {
		card_t card = game.players[p].hand.card[i];
		if (table_card.face != -1 &&	/* is this an adequate check? 
						 */
		    !memcmp(&card, &table_card, sizeof(card_t)))
			/* if the player has a card on the table _and_ it
			   matches this card, skip over it. */
			continue;
		x = cx + 0.5 + (i * ow);
		y = cy + 0.5 + (i * oh);
		if (i == selected_card) {
			x += cxo;
			y += cyo;
		}
		draw_card(card, orientation(p), x, y);
	}

	/* And refresh the on-screen image for card areas */
	table_show_table(x_outer, y_outer, cw, ch);

	client_debug("     Done displaying hand.");
}

/* table_display_all_hands exposed function to show all players' hands */
void table_display_all_hands(void)
{
	int p;
	for (p = 0; p < game.num_players; p++)
		table_display_hand(p);
}


#ifdef ANIMATION
/* table_animation() Exposed function to setup an animation */
void table_animation(int p, card_t card)
{
	int pos[4][4] = { {0, 0, 0, 0},
	{0, 146, 120, 186},
	{159, 0, 199, 130},
	{get_table_width() - CARDWIDTH, 226, 280, 186}
	};

	table_animation_trigger(card, pos[p][0], pos[p][1], pos[p][2],
				pos[p][3]);
}
#endif /* ANIMATION */


void table_play_card(int p, card_t card)
{
#ifdef ANIMATION
	table_animation(p, card);
#else
	table_show_card(p, card);
#endif
}


/* table_clear_table() Exposed function to clear cards off the table area */
void table_clear_table(void)
{
	int i, x, y, w, h;
	get_table_dim(&x, &y, &w, &h);
	gdk_draw_rectangle(table_buf,
			   f1_style->bg_gc[GTK_WIDGET_STATE(f1)],
			   TRUE, x, y, w, h);

	table_show_table(x, y, w, h);

	for (i = 0; i < game.num_players; i++) {
		game.players[i].table_card = UNKNOWN_CARD;
	}
}


/* table_show_card Exposed function to show one player's cards on the table
   area */
static void table_show_card(int p, card_t c)
{
	int x, y;

	if (c.face == -1 || c.suit == -1)
		return;

	get_tablecard_pos(p, &x, &y);

	draw_card(c, 0, x, y);
	table_show_table(x, y, CARDWIDTH, CARDHEIGHT);
}

/* table_show_cards() Exposed function to show all four cards on the table
   area */
void table_show_cards()
{
	int p;
	for (p = 0; p < game.num_players; p++)
		table_show_card(p, game.players[p].table_card);
}
