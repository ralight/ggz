/*
 * File: table.c
 * Author: Rich Gade
 * Project: GGZ La Pocha Client
 * Date: 08/14/2000
 * Desc: Routines to handle the Gtk game table
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

#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>

#include <ggz.h>

#include "main.h"
#include "game.h"
#include "table.h"
#include "hand.h"
#include "dlg_bid.h"

/* The following would be hell to change */
#define CARDWIDTH	71
#define CARDHEIGHT	96

static GdkPixmap *table_buf, *table_buf_backup;
static GdkPixbuf *cards, *cards_b1, *cards_b2, *cards_b3, *cards_b4;
static GtkStyle *f1_style;
static GtkWidget *f1;
static GtkWidget *l_name[4], *l_bid[4], *l_tricks[4], *l_score[4], *l_trump;
static gboolean table_initialized = FALSE;
static struct {
	int	card;
	int	dest_x, dest_y;
	int	card_x, card_y;
	float	cur_x, cur_y;
	float	step_x, step_y;
	gint	cb_tag;
} anim;
static char table_c1=-1, table_c2=-1, table_c3=-1, table_c4=-1;

static void table_card_clicked(int);
static void table_card_select(int);
static void table_card_play(int);
static gint table_animation_callback(gpointer);
static void table_animation_trigger(int, int, int, int, int);
static void table_handle_table_click(int, int);

static GdkPixbuf *load_pixmap(GdkWindow *window, GdkBitmap **mask,
	    GdkColor *trans, const char *name)
{
	char *fullpath;
	GdkPixbuf *image;
	GError *error = NULL;

	fullpath = g_strdup_printf("%s/pixmaps/%s", GGZDATADIR, name);
	image = gdk_pixbuf_new_from_file(fullpath, &error);
	if(image == NULL)
		ggz_error_msg_exit("Can't load pixmap %s", fullpath);
	g_free(fullpath);

	return image;
}

#define render(target, gc, image, x1, y1, x2, y2, w, h) \
  gdk_pixbuf_render_to_drawable(image, target, gc, x1, y1, x2, y2, w, h, \
                                GDK_RGB_DITHER_NONE, 0, 0)

/* table_initialize()
 *   Setup and draw the table areas on the fixed1 dialog item
 */
void table_initialize(void)
{
	GdkBitmap *mask;
	GtkWidget *label;

	/* This starts our drawing code */
	f1 = gtk_object_get_data(GTK_OBJECT(dlg_main), "fixed1");
	f1_style = gtk_widget_get_style(f1);
	cards = load_pixmap(f1->window, &mask,
			    &f1_style->bg[GTK_STATE_NORMAL],
			    "cards-1.xpm");
	cards_b1 = load_pixmap(f1->window, &mask,
			       &f1_style->bg[GTK_STATE_NORMAL],
			       "cards-b1.xpm");
	cards_b2 = load_pixmap(f1->window, &mask,
			       &f1_style->bg[GTK_STATE_NORMAL],
			       "cards-b2.xpm");
	cards_b3 = load_pixmap(f1->window, &mask,
			       &f1_style->bg[GTK_STATE_NORMAL],
			       "cards-b3.xpm");
	cards_b4 = load_pixmap(f1->window, &mask,
			       &f1_style->bg[GTK_STATE_NORMAL],
			       "cards-b4.xpm");
	table_buf = gdk_pixmap_new(f1->window,
			     f1->allocation.width,
			     f1->allocation.height,
			     -1);
	table_buf_backup = gdk_pixmap_new(f1->window,
			     f1->allocation.width,
			     f1->allocation.height,
			     -1);

	/* Clear our buffer to the style's background color */
	gdk_draw_rectangle(table_buf,
			   f1_style->bg_gc[GTK_WIDGET_STATE(f1)],
			   TRUE,
			   0, 0,
			   f1->allocation.width, f1->allocation.height);

	/* Draw card areas */
	gdk_draw_rectangle(table_buf,
			   f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
			   FALSE,
			   5, 5,
			   106, 106);
	gdk_draw_rectangle(table_buf,
			   f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
			   FALSE,
			   5, 111,
			   106, 241);
	gdk_draw_rectangle(table_buf,
			   f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
			   FALSE,
			   5, 357,
			   106, 106);
	gdk_draw_rectangle(table_buf,
			   f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
			   FALSE,
			   111, 357,
			   241, 106);
	gdk_draw_rectangle(table_buf,
			   f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
			   FALSE,
			   357, 357,
			   106, 106);
	gdk_draw_rectangle(table_buf,
			   f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
			   FALSE,
			   357, 116,
			   106, 241);
	gdk_draw_rectangle(table_buf,
			   f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
			   FALSE,
			   357, 5,
			   106, 106);
	gdk_draw_rectangle(table_buf,
			   f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
			   FALSE,
			   116, 5,
			   241, 106);

	/* Display the buffer */
	gdk_draw_pixmap(f1->window,
			f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
			table_buf,
			0, 0,
			0, 0,
			f1->allocation.width, f1->allocation.height);

	/* Add text labels to display */
	/* Name entries */
	l_name[0] = gtk_label_new("Empty Seat");
	gtk_fixed_put(GTK_FIXED(f1), l_name[0], 8, 358);
	gtk_widget_set_usize(l_name[0], 98, -1);
	gtk_widget_show(l_name[0]);
	l_name[1] = gtk_label_new("Empty Seat");
	gtk_fixed_put(GTK_FIXED(f1), l_name[1], 8, 6);
	gtk_widget_set_usize(l_name[1], 98, -1);
	gtk_widget_show(l_name[1]);
	l_name[2] = gtk_label_new("Empty Seat");
	gtk_fixed_put(GTK_FIXED(f1), l_name[2], 360, 6);
	gtk_widget_set_usize(l_name[2], 98, -1);
	gtk_widget_show(l_name[2]);
	l_name[3] = gtk_label_new("Empty Seat");
	gtk_fixed_put(GTK_FIXED(f1), l_name[3], 360, 358);
	gtk_widget_set_usize(l_name[3], 98, -1);
	gtk_widget_show(l_name[3]);

	/* Score entries */
	label = gtk_label_new("Score:");
	gtk_fixed_put(GTK_FIXED(f1), label, 8, 379);
	gtk_widget_show(label);
	label = gtk_label_new("Score:");
	gtk_fixed_put(GTK_FIXED(f1), label, 8, 30);
	gtk_widget_show(label);
	label = gtk_label_new("Score:");
	gtk_fixed_put(GTK_FIXED(f1), label, 360, 30);
	gtk_widget_show(label);
	label = gtk_label_new("Score:");
	gtk_fixed_put(GTK_FIXED(f1), label, 360, 382);
	gtk_widget_show(label);
	l_score[0] = gtk_label_new("0");
	gtk_fixed_put(GTK_FIXED(f1), l_score[0], 57, 379);
	gtk_widget_show(l_score[0]);
	l_score[1] = gtk_label_new("0");
	gtk_fixed_put(GTK_FIXED(f1), l_score[1], 57, 30);
	gtk_widget_show(l_score[1]);
	l_score[2] = gtk_label_new("0");
	gtk_fixed_put(GTK_FIXED(f1), l_score[2], 408, 30);
	gtk_widget_show(l_score[2]);
	l_score[3] = gtk_label_new("0");
	gtk_fixed_put(GTK_FIXED(f1), l_score[3], 408, 382);
	gtk_widget_show(l_score[3]);

	/* Bid entries */
	label = gtk_label_new("Bid:");
	gtk_fixed_put(GTK_FIXED(f1), label, 8, 404);
	gtk_widget_show(label);
	label = gtk_label_new("Bid:");
	gtk_fixed_put(GTK_FIXED(f1), label, 8, 54);
	gtk_widget_show(label);
	label = gtk_label_new("Bid:");
	gtk_fixed_put(GTK_FIXED(f1), label, 360, 54);
	gtk_widget_show(label);
	label = gtk_label_new("Bid:");
	gtk_fixed_put(GTK_FIXED(f1), label, 360, 406);
	gtk_widget_show(label);
	l_bid[0] = gtk_label_new(" ");
	gtk_fixed_put(GTK_FIXED(f1), l_bid[0], 57, 404);
	gtk_widget_show(l_bid[0]);
	l_bid[1] = gtk_label_new(" ");
	gtk_fixed_put(GTK_FIXED(f1), l_bid[1], 57, 54);
	gtk_widget_show(l_bid[1]);
	l_bid[2] = gtk_label_new(" ");
	gtk_fixed_put(GTK_FIXED(f1), l_bid[2], 408, 54);
	gtk_widget_show(l_bid[2]);
	l_bid[3] = gtk_label_new(" ");
	gtk_fixed_put(GTK_FIXED(f1), l_bid[3], 408, 406);
	gtk_widget_show(l_bid[3]);

	/* Trick entries */
	label = gtk_label_new("Tricks:");
	gtk_fixed_put(GTK_FIXED(f1), label, 8, 429);
	gtk_widget_show(label);
	label = gtk_label_new("Tricks:");
	gtk_fixed_put(GTK_FIXED(f1), label, 8, 78);
	gtk_widget_show(label);
	label = gtk_label_new("Tricks:");
	gtk_fixed_put(GTK_FIXED(f1), label, 360, 78);
	gtk_widget_show(label);
	label = gtk_label_new("Tricks:");
	gtk_fixed_put(GTK_FIXED(f1), label, 360, 430);
	gtk_widget_show(label);
	l_tricks[0] = gtk_label_new(" ");
	gtk_fixed_put(GTK_FIXED(f1), l_tricks[0], 57, 429);
	gtk_widget_show(l_tricks[0]);
	l_tricks[1] = gtk_label_new(" ");
	gtk_fixed_put(GTK_FIXED(f1), l_tricks[1], 57, 78);
	gtk_widget_show(l_tricks[1]);
	l_tricks[2] = gtk_label_new(" ");
	gtk_fixed_put(GTK_FIXED(f1), l_tricks[2], 408, 78);
	gtk_widget_show(l_tricks[2]);
	l_tricks[3] = gtk_label_new(" ");
	gtk_fixed_put(GTK_FIXED(f1), l_tricks[3], 408, 430);
	gtk_widget_show(l_tricks[3]);

	/* Current trump entry */
	l_trump = gtk_label_new("Trump is not set");
	gtk_fixed_put(GTK_FIXED(f1), l_trump, 8, 448);
	gtk_widget_set_usize(l_trump, 98, -1);
	gtk_widget_show(l_trump);

	/* Finally, build the fixed area for bidding */
	dlg_bid_setup(f1);

	table_initialized = TRUE;
}


/* table_style_change()
 *   Handle a redraw of necessary items when a Gtk style change
 *   is signaled.
 */
void table_style_change(void)
{
	if(!table_initialized)
		return;

	gtk_widget_grab_focus(dlg_main);

	f1_style = gtk_widget_get_style(f1);

	/* Clear our buffer to the style's background color */
	gdk_draw_rectangle(table_buf,
			   f1_style->bg_gc[GTK_WIDGET_STATE(f1)],
			   TRUE,
			   0, 0,
			   f1->allocation.width, f1->allocation.height);

	/* Draw card areas */
	gdk_draw_rectangle(table_buf,
			   f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
			   FALSE,
			   5, 5,
			   106, 106);
	gdk_draw_rectangle(table_buf,
			   f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
			   FALSE,
			   5, 111,
			   106, 241);
	gdk_draw_rectangle(table_buf,
			   f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
			   FALSE,
			   5, 357,
			   106, 106);
	gdk_draw_rectangle(table_buf,
			   f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
			   FALSE,
			   111, 357,
			   241, 106);
	gdk_draw_rectangle(table_buf,
			   f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
			   FALSE,
			   357, 357,
			   106, 106);
	gdk_draw_rectangle(table_buf,
			   f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
			   FALSE,
			   357, 116,
			   106, 241);
	gdk_draw_rectangle(table_buf,
			   f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
			   FALSE,
			   357, 5,
			   106, 106);
	gdk_draw_rectangle(table_buf,
			   f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
			   FALSE,
			   116, 5,
			   241, 106);

	/* If there's an animation in progress, zip it */
	if(game.state == LP_STATE_ANIM)
		table_animation_zip(FALSE);

	/* Redisplay any cards on table and in hands */
	table_display_hand();
	table_show_cards(table_c1, table_c2, table_c3, table_c4);

	/* There has GOT to be a better way to force the redraw! */
	gdk_window_hide(f1->window);
	gdk_window_show(f1->window);
}


/* table_handle_expose_event()
 *   Redraw our table areas that just got exposed, including
 *   all dependent widgets.
 */
void table_handle_expose_event(GdkEventExpose *event)
{
	gdk_draw_pixmap(f1->window,
			f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
			table_buf,
			event->area.x, event->area.y,
			event->area.x, event->area.y,
			event->area.width, event->area.height);
}


/* table_handle_click_event()
 *   Check for what card has been clicked and process it
 */
void table_handle_click_event(GdkEventButton *event)
{
	int target;
	int x;

	/* Real quick, see if we even care */
	if(game.state == LP_STATE_TRUMP) {
		table_handle_table_click(event->x, event->y);
		return;
	}
	if(event->x < 116 || event->x > 400
	   || event->y < 363 || event->y > 512
	   || game.state != LP_STATE_PLAY)
		return;

	/* Calculate our card target */
	for(target=0; target<9; target++) {
		x = 116.5 + ((target+1) * CARDWIDTH/4.0);
		if(event->x <= x)
			break;
	}
	/* This checks cards to the left if the clicked card isn't there */
	while(hand.card[target] == -1) {
		if(--target < 0)
			return;
		x = 167.5 + ((target+1) * CARDWIDTH/4.0);
		if(event->x > x)
			return;
	}

	table_card_clicked(target);
}


/* table_card_clicked()
 *   Handle a card that has been clicked by either popping it forward
 *   or playing it if it is already selected.
 */
static void table_card_clicked(int card)
{
	if(card == hand.selected_card) {
		/* Play this card */
		table_card_play(card);

		/* We save these values in case the server rejects the card */
		hand.in_play_card_num = card;
		hand.in_play_card_val = hand.card[card];
		hand.card[card] = -1;
		hand.selected_card = -1;

		/* Call the game function to notify server of play */
		game_play_card(card);
	} else {
		/* Pop the card forward and select it */
		table_card_select(card);
		hand.selected_card = card;
	}
}


/* table_card_select()
 *   Popout the newly selected card, possibly restoring table state if
 *   we are popping an old card back into the hand.
 */
static void table_card_select(int card)
{
	int i, x1, y1, x2, y2;

	if(hand.selected_card >= 0)
		/* Restore the table image */
		gdk_draw_pixmap(table_buf,
				f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
				table_buf_backup,
				112, 348,
				112, 348,
				239, 114);
	else
		/* Backup the table image */
		gdk_draw_pixmap(table_buf_backup,
				f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
				table_buf,
				112, 348,
				112, 348,
				239, 114);

	/* Clean the card area */
	gdk_draw_rectangle(table_buf,
		   	f1_style->bg_gc[GTK_WIDGET_STATE(f1)],
		   	TRUE,
		   	112, 358,
		   	239, 104);

	/* Draw the cards, with our selected card popped forward */
	for(i=0; i<hand.hand_size; i++) {
		if(hand.card[i] < 0)
			continue;
		x1 = (hand.card[i] / 13) * CARDWIDTH;
		y1 = (hand.card[i] % 13) * CARDHEIGHT;
		x2 = 116.5 + (i * CARDWIDTH/4.0);
		y2 = 363;
		if(i == card)
			y2 -= 10;
		render(table_buf,
		       f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
		       cards,
		       x1, y1,
		       x2, y2,
		       CARDWIDTH, CARDHEIGHT);
	}

	/* Refresh the on-screen image */
	gdk_draw_pixmap(f1->window,
			f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
			table_buf,
			112, 348,
			112, 348,
			239, 114);
}


/* table_card_play()
 *   Move the selected card out onto the playing area.
 */
static void table_card_play(int card)
{
	int i, x1, y1, x2, y2;

	/* Start by restoring gfx to a clean state */
	gdk_draw_pixmap(table_buf,
			f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
			table_buf_backup,
			112, 348,
			112, 348,
			239, 114);

	/* Clean the card area */
	gdk_draw_rectangle(table_buf,
		   	f1_style->bg_gc[GTK_WIDGET_STATE(f1)],
		   	TRUE,
		   	112, 358,
		   	239, 104);

	/* Draw the cards, eliminating the card in play */
	for(i=0; i<hand.hand_size; i++) {
		if(hand.card[i] < 0)
			continue;
		x1 = (hand.card[i] / 13) * CARDWIDTH;
		y1 = (hand.card[i] % 13) * CARDHEIGHT;
		x2 = 116.5 + (i * CARDWIDTH/4.0);
		y2 = 363;
		if(i == card)
			continue;
		render(table_buf,
		       f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
		       cards,
		       x1, y1,
		       x2, y2,
		       CARDWIDTH, CARDHEIGHT);
	}

	/* Backup entire table image */
	gdk_draw_pixmap(table_buf_backup,
			f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
			table_buf,
			0, 0,
			0, 0,
			f1->allocation.width, f1->allocation.height);

	/* Now draw the card on the table */
	x1 = (hand.card[card] / 13) * CARDWIDTH;
	y1 = (hand.card[card] % 13) * CARDHEIGHT;
	x2 = 116.5 + (card * CARDWIDTH/4.0);
	y2 = 363;
	render(table_buf,
	       f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
	       cards,
	       x1, y1,
	       x2, y2,
	       CARDWIDTH, CARDHEIGHT);

	/* And refresh the on-screen image */
	gdk_draw_pixmap(f1->window,
			f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
			table_buf,
			112, 348,
			112, 348,
			239, 114);

	/* Setup and trigger the card animation */
	table_animation_trigger(hand.card[card], x2, y2, 199, 242);

	table_c1 = hand.card[card];
}


/* table_animation_trigger()
 *   Function to setup and trigger a card animation
 */
void table_animation_trigger(int card, int x1, int y1, int x2, int y2)
{
	#define FRAMES		15
	#define DURATION	500	/* In milliseconds */

	/* Store all our info */
	anim.card = card;
	anim.card_x = (card / 13) * CARDWIDTH;
	anim.card_y = (card % 13) * CARDHEIGHT;
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

	game.state = LP_STATE_ANIM;
}


/* table_animation_callback()
 *   Handle one frame of card animation, this is triggered by a GtkTimeout
 *   setup in table_animation_trigger().
 */
gint table_animation_callback(gpointer ignored)
{
	float new_x, new_y;
	int ref_x, ref_y;

	/* Calculate our next move */
	new_x = anim.cur_x - anim.step_x;
	if(abs(new_x - anim.dest_x) < 2)
		new_x = anim.dest_x;
	new_y = anim.cur_y - anim.step_y;
	if(abs(new_y - anim.dest_y) < 2)
		new_y = anim.dest_y;

	/* Restore table area around old card image from backup */
	gdk_draw_pixmap(table_buf,
			f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
			table_buf_backup,
			anim.cur_x, anim.cur_y,
			anim.cur_x, anim.cur_y,
			CARDWIDTH, CARDHEIGHT);

	/* Draw our new card position */
	render(table_buf,
	       f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
	       cards,
	       anim.card_x, anim.card_y,
	       new_x, new_y,
	       CARDWIDTH, CARDHEIGHT);

	/* And refresh the on-screen image */
	if(new_x < anim.cur_x)
		ref_x = new_x;
	else
		ref_x = anim.cur_x;
	if(new_y < anim.cur_y)
		ref_y = new_y;
	else
		ref_y = anim.cur_y;
	gdk_draw_pixmap(f1->window,
			f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
			table_buf,
			ref_x, ref_y,
			ref_x, ref_y,
			CARDWIDTH + abs(anim.step_x) + 2,
			CARDHEIGHT + abs(anim.step_y) + 2);

	/* Update our information for next time */
	anim.cur_x = new_x;
	anim.cur_y = new_y;

	/* If we are there, stop the animation process */
	if(new_x == anim.dest_x && new_y == anim.dest_y) {
		game.state = LP_STATE_WAIT;
		return FALSE;
	}

	/* Continue animating */
	return TRUE;
}


/* table_animation_abort()
 *   Exposed function to abort the animation process
 */
void table_animation_abort(void)
{
	int i, x1, y1, x2, y2;

	/* First, kill off the animation callback */
	if(game.state == LP_STATE_ANIM) {
		gtk_timeout_remove(anim.cb_tag);
		game.state = LP_STATE_WAIT;
	}

	/* Restore table area around old card image from backup */
	gdk_draw_pixmap(table_buf,
			f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
			table_buf_backup,
			anim.cur_x, anim.cur_y,
			anim.cur_x, anim.cur_y,
			CARDWIDTH, CARDHEIGHT);
	gdk_draw_pixmap(f1->window,
			f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
			table_buf,
			anim.cur_x, anim.cur_y,
			anim.cur_x, anim.cur_y,
			CARDWIDTH, CARDHEIGHT);

	/* The caller is assumed to have restored the card to the hand */
	/* so we can redraw the full hand and should be done */
	for(i=0; i<hand.hand_size; i++) {
		if(hand.card[i] < 0)
			continue;
		x1 = (hand.card[i] / 13) * CARDWIDTH;
		y1 = (hand.card[i] % 13) * CARDHEIGHT;
		x2 = 116.5 + (i * CARDWIDTH/4.0);
		y2 = 363;
		render(table_buf,
		        	f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
				cards,
				x1, y1,
				x2, y2,
				CARDWIDTH, CARDHEIGHT);
	}
	gdk_draw_pixmap(f1->window,
			f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
			table_buf,
			112, 348,
			112, 348,
			239, 114);
}


/* table_animation_zip
 *   Exposed function to zip to the finish of an animation sequence
 *   so that a new one may be started
 */
void table_animation_zip(gboolean restore)
{
	/* First, kill off the animation callback */
	if(game.state == LP_STATE_ANIM) {
		gtk_timeout_remove(anim.cb_tag);
		game.state = LP_STATE_WAIT;
	}

	/* Restore table area around old card image from backup */
	if(restore) {
		gdk_draw_pixmap(table_buf,
				f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
				table_buf_backup,
				anim.cur_x, anim.cur_y,
				anim.cur_x, anim.cur_y,
				CARDWIDTH, CARDHEIGHT);
		gdk_draw_pixmap(f1->window,
				f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
				table_buf,
				anim.cur_x, anim.cur_y,
				anim.cur_x, anim.cur_y,
				CARDWIDTH, CARDHEIGHT);
	}

	/* And move the card to it's final resting place */
	render(table_buf,
			f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
			cards,
			anim.card_x, anim.card_y,
			anim.dest_x, anim.dest_y,
			CARDWIDTH, CARDHEIGHT);
	gdk_draw_pixmap(f1->window,
			f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
			table_buf,
			anim.dest_x, anim.dest_y,
			anim.dest_x, anim.dest_y,
			CARDWIDTH, CARDHEIGHT);
}


/* Convenience macro, wraps a player number to a visual seat position */
#define SEAT_POS(x)	((4 + x - game.me) % 4)
/* Vice versa, maps position to seat number */
#define POS_SEAT(x)	((x + game.me) % 4)

/* table_set_name()
 *   Exposed function to set name on display
 */
void table_set_name(int p, char *name)
{
	gtk_label_set_text(GTK_LABEL(l_name[SEAT_POS(p)]), name);
}


/* table_set_dealer()
 *   Notes the dealer's position with a (D) on the screen and first bidder
 *   with a (B) - might as well take care of that here too
 */
void table_set_dealer(void)
{
	int i;
	char *name_temp;

	for(i=0; i<4; i++) {
		if(i == game.dealer)
			name_temp = g_strdup_printf("%s (D)", game.names[i]);
		else if(i == (game.dealer + 1) % 4)
			name_temp = g_strdup_printf("%s (B)", game.names[i]);
		else
			name_temp = g_strdup(game.names[i]);
		gtk_label_set_text(GTK_LABEL(l_name[SEAT_POS(i)]), name_temp);
		g_free(name_temp);
	}
}


/* table_set_bidder()
 *   Notes the current bidder with a (B) on the screen, and clears
 *   the previous bidder's flag.  Note that the (int bidder) argument
 *   is the seat of the PREVIOUS bidder (response from MSG_BID).
 */
void table_set_bidder(int bidder)
{
	int seat;
	char *name_temp;

	/* Clear previous bidder flag */
	gtk_label_set_text(GTK_LABEL(l_name[SEAT_POS(bidder)]),
			   game.names[bidder]);

	/* If last bid was by dealer, then we are done bidding */
	/* Display the current turn for first trick */
	if(bidder == game.dealer) {
		seat = (bidder + 1) % 4;
		name_temp = g_strdup_printf("* %s *", game.names[seat]);
		gtk_label_set_text(GTK_LABEL(l_name[SEAT_POS(seat)]),name_temp);
		g_free(name_temp);

		return;
	}

	/* Set current bidder flag */
	seat = (bidder + 1) % 4;
	if(seat == game.dealer)
		name_temp = g_strdup_printf("%s (DB)", game.names[seat]);
	else
		name_temp = g_strdup_printf("%s (B)", game.names[seat]);
	gtk_label_set_text(GTK_LABEL(l_name[SEAT_POS(seat)]), name_temp);
	g_free(name_temp);
}


/* table_set_turn()
 *   Notes the current turn with a * NAME * on the screen, and clears
 *   the previous player's flag.  Note that the (int turn) argument
 *   is the seat of the PREVIOUS player (response from MSG_PLAY).
 */
void table_set_turn(int turn)
{
	int seat;
	char *name_temp;

	/* Clear previous players flag */
	gtk_label_set_text(GTK_LABEL(l_name[SEAT_POS(turn)]), game.names[turn]);

	/* If last turn was by "leader-1", then we are done */
	if(turn == (game.lead + 3) % 4)
		return;

	/* Set current player flag */
	seat = (turn + 1) % 4;
	name_temp = g_strdup_printf("* %s *", game.names[seat]);
	gtk_label_set_text(GTK_LABEL(l_name[SEAT_POS(seat)]), name_temp);
	g_free(name_temp);
}


/* table_set_bid()
 *   Exposed function to set bid on display
 */
void table_set_bid(int p, int bid)
{
	char *t_str;

	if(bid != -1) {
		t_str = g_strdup_printf("%d", bid);
		gtk_label_set_text(GTK_LABEL(l_bid[SEAT_POS(p)]), t_str);
		g_free(t_str);
		gtk_label_set_text(GTK_LABEL(l_tricks[SEAT_POS(p)]), "0");
	}
}


/* table_set_tricks()
 *   Exposed function to set tricks on display
 */
void table_set_tricks(int p, int tricks)
{
	char *t_str;

	t_str = g_strdup_printf("%d", tricks);
	gtk_label_set_text(GTK_LABEL(l_tricks[SEAT_POS(p)]), t_str);
	g_free(t_str);
}


/* table_set_score()
 *   Exposed function to set score on display
 */
void table_set_score(int p, int score)
{
	char *t_str;

	t_str = g_strdup_printf("%d", score);
	gtk_label_set_text(GTK_LABEL(l_score[SEAT_POS(p)]), t_str);
	g_free(t_str);
	gtk_label_set_text(GTK_LABEL(l_tricks[SEAT_POS(p)]), " ");
	gtk_label_set_text(GTK_LABEL(l_bid[SEAT_POS(p)]), " ");
}


/* table_set_trump()
 *   Exposed function to set trump on display
 */
void table_set_trump(void)
{
	char *t_str;
	char *suits[] = { "Clubs", "Diamonds", "Hearts", "Spades" };

	if(game.trump_suit != -1) {
		t_str = g_strdup_printf("Trump is %s", suits[(int)game.trump_suit]);
		gtk_label_set_text(GTK_LABEL(l_trump), t_str);
		g_free(t_str);
	} else
		gtk_label_set_text(GTK_LABEL(l_trump), "Trump is not set");
}


/* table_display_hand()
 *   Exposed function to show the players' hands
 */
void table_display_hand(void)
{
	int i, x1, y1, x2, y2;

	/* Clean the card areas */
	gdk_draw_rectangle(table_buf,
			   f1_style->bg_gc[GTK_WIDGET_STATE(f1)],
			   TRUE,
			   112, 358,
			   239, 104);
	gdk_draw_rectangle(table_buf,
			   f1_style->bg_gc[GTK_WIDGET_STATE(f1)],
			   TRUE,
			   6, 112,
			   104, 239);
	gdk_draw_rectangle(table_buf,
			   f1_style->bg_gc[GTK_WIDGET_STATE(f1)],
			   TRUE,
			   117, 6,
			   239, 104);
	gdk_draw_rectangle(table_buf,
			   f1_style->bg_gc[GTK_WIDGET_STATE(f1)],
			   TRUE,
			   358, 117,
			   104, 239);

	/* Draw my cards, skipping any missing cards (syncing player) */
	for(i=0; i<hand.hand_size; i++) {
		if(hand.card[i] < 0)
			continue;
		x1 = (hand.card[i] / 13) * CARDWIDTH;
		y1 = (hand.card[i] % 13) * CARDHEIGHT;
		x2 = 116.5 + (i * CARDWIDTH/4.0);
		y2 = 363;
		render(table_buf,
		        	f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
				cards,
				x1, y1,
				x2, y2,
				CARDWIDTH, CARDHEIGHT);
	}

	/* Left Player card backs */
	for(i=0; i<game.num_cards[POS_SEAT(1)]; i++) {
		x1 = 10;
		y1 = 116.5 + (i * CARDWIDTH/4.0);
		render(table_buf,
			        f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
				cards_b2,
				0, 142,
				x1, y1,
				CARDHEIGHT, CARDWIDTH);
	}
	/* Across Player */
	for(i=9; i>(9-game.num_cards[POS_SEAT(2)]); i--) {
		x1 = 121.5 + (i * CARDWIDTH/4.0);
		y1 = 10;
		render(table_buf,
			        f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
				cards_b3,
				71, 0,
				x1, y1,
				CARDWIDTH, CARDHEIGHT);
	}
	/* Right Player */
	for(i=9; i>(9-game.num_cards[POS_SEAT(3)]); i--) {
		x1 = 363;
		y1 = 121.5 + (i * CARDWIDTH/4.0);
		render(table_buf,
			        f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
				cards_b4,
				0, 71,
				x1, y1,
				CARDHEIGHT, CARDWIDTH);
	}

	/* And refresh the on-screen image for card areas */
	gdk_draw_pixmap(f1->window,
			f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
			table_buf,
			112, 358,
			112, 358,
			239, 104);
	gdk_draw_pixmap(f1->window,
			f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
			table_buf,
			6, 112,
			6, 112,
			104, 239);
	gdk_draw_pixmap(f1->window,
			f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
			table_buf,
			117, 6,
			117, 6,
			239, 104);
	gdk_draw_pixmap(f1->window,
			f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
			table_buf,
			358, 117,
			358, 117,
			104, 239);
}


/* table_animation_opponent()
 *   Exposed function to setup an opponent animation
 */
void table_animation_opponent(char p_num, char card)
{
	int i, x1, y1, x2, y2, xc, yc;

	/* Calculate start and final position */
	switch(SEAT_POS(p_num)) {
		case 1:
			table_c2 = card;
			gdk_draw_rectangle(table_buf,
				   f1_style->bg_gc[GTK_WIDGET_STATE(f1)],
				   TRUE,
				   6, 112,
				   104, 239);
			game.num_cards[POS_SEAT(1)]--;
			for(i=0; i<game.num_cards[POS_SEAT(1)]; i++) {
				x1 = 10;
				y1 = 116.5 + (i * CARDWIDTH/4.0);
				render(table_buf,
		        		f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
					cards_b2,
					0, 142,
					x1, y1,
					CARDHEIGHT, CARDWIDTH);
			}
			gdk_draw_pixmap(f1->window,
					f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
					table_buf,
					6, 112,
					6, 112,
					104, 239);
			x1 = 0;
			y1 = 146;
			x2 = 120;
			y2 = 186;
			break;
		case 2:
			table_c3 = card;
			gdk_draw_rectangle(table_buf,
				   f1_style->bg_gc[GTK_WIDGET_STATE(f1)],
				   TRUE,
				   117, 6,
				   239, 104);
			game.num_cards[POS_SEAT(2)]--;
			for(i=9; i>(9-game.num_cards[POS_SEAT(2)]); i--) {
				x1 = 121.5 + (i * CARDWIDTH/4.0);
				y1 = 10;
				render(table_buf,
				        f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
					cards_b3,
					71, 0,
					x1, y1,
					CARDWIDTH, CARDHEIGHT);
			}
			gdk_draw_pixmap(f1->window,
					f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
					table_buf,
					117, 6,
					117, 6,
					239, 104);
			x1 = 159;
			y1 = 0;
			x2 = 199;
			y2 = 130;
			break;
		case 3:
		default:
			table_c4 = card;
			gdk_draw_rectangle(table_buf,
				   f1_style->bg_gc[GTK_WIDGET_STATE(f1)],
				   TRUE,
				   358, 117,
				   104, 239);
			game.num_cards[POS_SEAT(3)]--;
			for(i=9; i>(9-game.num_cards[POS_SEAT(3)]); i--) {
				x1 = 363;
				y1 = 121.5 + (i * CARDWIDTH/4.0);
				render(table_buf,
				        f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
					cards_b4,
					0, 71,
					x1, y1,
					CARDHEIGHT, CARDWIDTH);
			}
			gdk_draw_pixmap(f1->window,
					f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
					table_buf,
					358, 117,
					358, 117,
					104, 239);
			x1 = f1->allocation.width - CARDWIDTH;
			y1 = 226;
			x2 = 280;
			y2 = 186;
			break;
	}

	/* Backup entire table image */
	gdk_draw_pixmap(table_buf_backup,
			f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
			table_buf,
			0, 0,
			0, 0,
			f1->allocation.width, f1->allocation.height);

	/* Draw the card on the table */
	xc = (hand.card[(int)card] / 13) * CARDWIDTH;
	yc = (hand.card[(int)card] % 13) * CARDHEIGHT;
	render(table_buf,
			f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
			cards,
			xc, yc,
			x1, y1,
			CARDWIDTH, CARDHEIGHT);
	gdk_draw_pixmap(f1->window,
			f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
			table_buf,
			x1, y1,
			x1, y1,
			CARDWIDTH, CARDHEIGHT);

	table_animation_trigger(card, x1, y1, x2, y2);
}


/* table_clear_table()
 *   Exposed function to clear cards off the table area
 */
void table_clear_table(void)
{
	gdk_draw_rectangle(table_buf,
			   f1_style->bg_gc[GTK_WIDGET_STATE(f1)],
			   TRUE,
			   120, 130,
			   231, 208);
	gdk_draw_pixmap(f1->window,
			f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
			table_buf,
			120, 130,
			120, 130,
			231, 208);
	table_c1 = table_c2 = table_c3 = table_c4 = -1;
}


/* table_show_cards_pnum()
 *   Exposed function to call table_show_cards with proper ordering
 *   for cards in player order rather than position order
 */
void table_show_cards_pnum(char c1, char c2, char c3, char c4)
{
	switch(game.me) {
		case 0:
			table_show_cards(c1, c2, c3, c4);
			break;
		case 1:
			table_show_cards(c2, c3, c4, c1);
			break;
		case 2:
			table_show_cards(c3, c4, c1, c2);
			break;
		case 3:
			table_show_cards(c4, c1, c2, c3);
			break;
	}
}


/* table_show_cards()
 *   Exposed function to show four cards on the table area
 */
void table_show_cards(char c1, char c2, char c3, char c4)
{
	int xc, yc, x, y;

	/* Pos 0 (bottom) */
	if(c1 != -1) {
		xc = (c1 / 13) * CARDWIDTH;
		yc = (c1 % 13) * CARDHEIGHT;
		x = 199;
		y = 242;
		render(table_buf,
				f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
				cards,
				xc, yc,
				x, y,
				CARDWIDTH, CARDHEIGHT);
		gdk_draw_pixmap(f1->window,
				f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
				table_buf,
				x, y,
				x, y,
				CARDWIDTH, CARDHEIGHT);
	}

	/* Left player */
	if(c2 != -1) {
		xc = (c2 / 13) * CARDWIDTH;
		yc = (c2 % 13) * CARDHEIGHT;
		x = 120;
		y = 186;
		render(table_buf,
				f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
				cards,
				xc, yc,
				x, y,
				CARDWIDTH, CARDHEIGHT);
		gdk_draw_pixmap(f1->window,
				f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
				table_buf,
				x, y,
				x, y,
				CARDWIDTH, CARDHEIGHT);
	}

	/* Across player */
	if(c3 != -1) {
		xc = (c3 / 13) * CARDWIDTH;
		yc = (c3 % 13) * CARDHEIGHT;
		x = 199;
		y = 130;
		render(table_buf,
				f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
				cards,
				xc, yc,
				x, y,
				CARDWIDTH, CARDHEIGHT);
		gdk_draw_pixmap(f1->window,
				f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
				table_buf,
				x, y,
				x, y,
				CARDWIDTH, CARDHEIGHT);
	}

	/* Right player */
	if(c4 != -1) {
		xc = (c4 / 13) * CARDWIDTH;
		yc = (c4 % 13) * CARDHEIGHT;
		x = 280;
		y = 186;
		render(table_buf,
				f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
				cards,
				xc, yc,
				x, y,
				CARDWIDTH, CARDHEIGHT);
		gdk_draw_pixmap(f1->window,
				f1_style->fg_gc[GTK_WIDGET_STATE(f1)],
				table_buf,
				x, y,
				x, y,
				CARDWIDTH, CARDHEIGHT);
	}
}


/* table_handle_table_click()
 *   Handles a click on the table area
 */
static void table_handle_table_click(int x, int y)
{
	/* Decide which card was clicked, and call game_handle_table_click */
	if(x >= 199 && x < 199+CARDWIDTH
	   && y >= 242 && y < 242+CARDHEIGHT)
		game_handle_table_click(0);

	if(x >= 120 && x < 120+CARDWIDTH
	   && y >= 186 && y < 186+CARDHEIGHT)
		game_handle_table_click(1);

	if(x >= 199 && x < 199+CARDWIDTH
	   && y >= 130 && y < 130+CARDHEIGHT)
		game_handle_table_click(2);

	if(x >= 280 && x < 280+CARDWIDTH
	   && y >= 186 && y < 186+CARDHEIGHT)
		game_handle_table_click(3);
}
