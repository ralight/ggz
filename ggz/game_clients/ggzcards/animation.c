/* 
 * File: animation.c
 * Author: Rich Gade, Jason Short
 * Project: GGZCards Client
 * Date: 12/18/2001
 * Desc: Animation code for GTK table
 * $Id: animation.c 3352 2002-02-13 09:53:05Z jdorje $
 *
 * Copyright (C) 2001-2002 GGZ Development Team.
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

#include <stdlib.h>

#include <assert.h>
#include <ggz.h>

#include "animation.h"
#include "game.h"
#include "layout.h"
#include "table.h"


/* The number of frames in an animation sequence. */
#define FRAMES		(preferences.smoother_animation ? 50 : 15)

/* Intended duration of an animation sequence, in milliseconds. */
#define DURATION	(preferences.faster_animation ? 50 : 500)	


static int animating = 0;

/* A backing store used only for animation.  It basically sits
   on top of table_buf. */
static GdkPixmap *anim_buf = NULL;	

static struct {
	int player;
	card_t card;

	int start_x, start_y;
	int dest_x, dest_y;
	
	int card_x, card_y;	/* Position of the card in the XPM */
	
	int cur_x, cur_y;
	int cur_frame;		/* 0..FRAMES */
	
	guint cb_tag;
} anim = {-1};

static gint animation_callback(gpointer ignored);

void anim_alloc_table(void)
{
	/* Get rid of old buffer. */
	if (anim_buf)
		gdk_pixmap_unref(anim_buf);
		
	anim_buf = gdk_pixmap_new(table->window,
				   get_table_width(), get_table_height(), -1);
}

/* Function to setup and trigger a card animation */
void animation_start(int player, card_t card, int card_num)
{
	ggz_debug("animation", "Setting up animation for player %d", player);
	
	assert(player >= 0 && player < ggzcards.num_players);
	assert(anim_buf);

	/* We don't currently support animation for more than one player at a 
	   time. */
	if (animating) {
		if (player == anim.player && card.suit == anim.card.suit
		    && card.face == anim.card.face)
			return;
		else
			animation_stop(TRUE);
	}
	
	/* If the card was _already_ placed out on the table, we don't want
	   to do it again.  This is ugly, because the caller has to be careful
	   not to update table_cards until _after_ calling animation_start(). */
	if (card.suit == table_cards[player].suit &&
	    card.face == table_cards[player].face &&
	    card.deck == table_cards[player].deck)
		return;

	/* If we don't have animation enabled, then we simply never start an
	   animation. */
	if (!preferences.animation)
		return;
		
	/*
	 * Build the anim structure.
	 */

	/* Find the initial position of the animation. */
	get_card_pos(player, card_num, &anim.start_x, &anim.start_y);
	if (orientation(player) % 2 == 1) {
		/* The player's cards are horizontal, but we're going to
		   animate vertically.  So we recenter. */
		anim.start_x = anim.start_x + CARDHEIGHT / 2 - CARDWIDTH / 2;
		anim.start_y = anim.start_y + CARDWIDTH / 2 - CARDHEIGHT / 2;			
	}
	
	/* Find the ending position of the animation. */
	get_tablecard_pos(player, &anim.dest_x, &anim.dest_y);
	
	/* Find the coordinates of the card itself in the cards1 pixmap */
	get_card_coordinates(card, 0, &anim.card_x, &anim.card_y);
	
	anim.card = card;
	anim.player = player;
	anim.cur_frame = 0;
	anim.cur_x = anim.start_x;
	anim.cur_y = anim.start_y;

	/* This sets up our timeout callback */
	anim.cb_tag = gtk_timeout_add(DURATION / FRAMES,
				      animation_callback, NULL);

	animating = TRUE;
}


/* Handle one frame of card animation, this is triggered by a GtkTimeout
   setup in animation_start(). */
static gint animation_callback(gpointer ignored)
{
	int new_x, new_y;
	
	int draw_x, draw_y, draw_x_width, draw_y_width;

	assert(animating);
	
	anim.cur_frame++;
	
	/* Calculate our new position */
	new_x = anim.start_x, new_y = anim.start_y;
	new_x += (anim.dest_x - anim.start_x) * anim.cur_frame / FRAMES;
	new_y += (anim.dest_y - anim.start_y) * anim.cur_frame / FRAMES;
	
		
	/*
	 * First we draw the table onto the animation buffer, then we draw
	 * the card onto the animation buffer, then we draw the animation
	 * buffer to the window.
	 */
		
	/* We have to figure out the "surrounding rectangle" of the changed
	   graphics. */
	draw_x = MIN(anim.cur_x, new_x);
	draw_y = MIN(anim.cur_y, new_y);
	draw_x_width = CARDWIDTH + ABS(anim.cur_x - new_x);
	draw_y_width = CARDHEIGHT + ABS(anim.cur_y - new_y);

	/* Draw over the old animation and surrounding areas. */
	table_draw_table(anim_buf, draw_x, draw_y, draw_x_width, draw_y_width);

	/* Draw our new card position */
	gdk_draw_pixmap(anim_buf,
			table_style->fg_gc[GTK_WIDGET_STATE(table)],
			card_fronts[0],
			anim.card_x, anim.card_y,
			new_x, new_y, CARDWIDTH, CARDHEIGHT);
			
	/* Now draw from the animation buffer to the screen.  This could
	   probably be done more easily if we just drew the _whole screen_,
	   but that could have bad side effects. */
	/* Note also, because we use float values (ugh!) we must do some
	   approximation to make sure we don't miss anything... */
	gdk_draw_pixmap(table->window,
			table_style->fg_gc[GTK_WIDGET_STATE(table)],
			anim_buf,
			draw_x, draw_y,
			draw_x, draw_y, draw_x_width, draw_y_width);
			
				
	/* If we are done, stop the animation process and draw the card "for
	   real".  Note that FRAMES might change mid-animation, so we must
	   be careful with this check. */
	if (anim.cur_frame >= FRAMES) {
		table_show_card(anim.player, anim.card, TRUE);
		animating = 0;
		return FALSE;
	}

	/* Update our information for next time */
	anim.cur_x = new_x;
	anim.cur_y = new_y;

	/* Continue animating */
	return TRUE;
}


/* Function to stop the animation process. "success" is true if the card
   movement should continue to completion, false if it must be undone. */
void animation_stop(int success)
{
	ggz_debug("animation", "Stopping animation (%d).", success);

	if (!animating)
		return;

	/* First, kill off the animation callback */
	table_draw_table(table->window, anim.cur_x, anim.cur_y, CARDWIDTH, CARDHEIGHT);
	gtk_timeout_remove(anim.cb_tag);

	if (success) {
		/* And move the card to it's final resting place */
		table_show_card(anim.player, anim.card, TRUE);
	} else {
		/* The caller is assumed to have restored the card to the
		   hand so we can redraw the full hand and should be done.
		   However, if the animation was completed then the card will 
		   be drawn on the table as well, so we'll need to clean that 
		   up. */
		table_display_hand(anim.player, TRUE);
		table_show_cards(TRUE);
	}

	animating = 0;
}
