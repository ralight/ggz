/* 
 * File: animation.c
 * Author: Rich Gade, Jason Short
 * Project: GGZCards Client
 * Date: 12/18/2001
 * Desc: Animation code for GTK table
 * $Id: animation.c 5277 2002-12-08 08:41:19Z jdorje $
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

#include <assert.h>
#include <stdlib.h>
#include <string.h>		/* for memset */

#include <ggz.h>

#include "animation.h"
#include "drawcard.h"
#include "game.h"
#include "layout.h"
#include "main.h"
#include "table.h"


/* The number of frames in an animation sequence. */
#define FRAMES		(preferences.smoother_animation ? 50 : 15)

/* Intended duration of an animation sequence, in milliseconds. */
#define DURATION	(preferences.faster_animation ? 50 : 500)

/* The delay before clearing cards off of the table (milliseconds). */
#define TABLE_CLEARING_DELAY	\
	(preferences.no_clearing_delay ? 100 \
	 : (preferences.longer_clearing_delay ? 2000 : 1000))

/* A backing store used only for animation.  It basically sits on top of
   table_buf. */
static GdkPixmap *anim_buf = NULL;

static int animating = FALSE;
static guint anim_tag;
static int dest_when_done = -1;

static struct {
	int animating;
	card_t card;

	int destination;	/* -1 or player # */

	int start_x, start_y;
	int dest_x, dest_y;

	int cur_x, cur_y;
	int cur_frame;		/* 0..FRAMES */
} anim[MAX_NUM_PLAYERS];

static gint start_offtable_animation(gpointer winner);
static gint animation_callback(gpointer ignored);

/* Setup the animation data, on startup or when the table is resized. */
void anim_setup(void)
{
	/* Get rid of old buffer. */
	if (anim_buf)
		gdk_pixmap_unref(anim_buf);

	anim_buf = gdk_pixmap_new(table->window,
				  get_table_width(), get_table_height(), -1);

	/* If this happened while we were animating, the animation would
	 * be fubar.  But this should be impossible. */
	assert(!animating);

	/* Necessary because we may setup the animation multiple times. */
	memset(anim, 0, sizeof(anim));
}

/* Function to setup and trigger a card animation */
int animation_start(int player, card_t card, int card_num, int destination)
{
	int start_x, start_y, dest_x, dest_y;

	ggz_debug(DBG_ANIM, "Setting up animation for player %d", player);

	/* 
	 * First we do a _lot_ of checks.
	 */

	assert(player >= 0 && player < ggzcards.num_players);
	assert(anim_buf);

	assert(preferences.animation);

	/* The function could conceivably be called twice; in that case just
	   return immediately. */
	if (anim[player].animating && destination == anim[player].destination)
		return FALSE;

	/* If the card was _already_ placed out on the table, we don't want to 
	   do it again.  This is ugly, because the caller has to be careful
	   not to update table_cards until _after_ calling animation_start(). */
	if (destination < 0 &&
	    card.suit == table_cards[player].suit &&
	    card.face == table_cards[player].face &&
	    card.deck == table_cards[player].deck)
		return FALSE;

	if (animating && !preferences.multiple_animation && destination < 0) {
		int p;
		/* If someone _else_ is animating, make them stop. This
		   doesn't apply to off-table animation. */
		/* FIXME: this is ugly... */
		for (p = 0; p < ggzcards.num_players; p++)
			if (anim[p].animating && anim[p].destination < 0) {
				animation_stop(TRUE);
				break;
			}
	}

	/* This shouldn't be possible, since we stop listening to the server
	   while we slide the cards off the table. */
	assert(!(anim[player].animating
		 && anim[player].destination >= 0 && destination < 0));

	if (destination >= 0) {
		int p;
		/* This is a bit of a hack: we'll just ignore the server while 
		   we clear the cards off of the table.  But it should make up 
		   for any lag problems... */
		listen_for_server(FALSE);

		/* This is also a bit of a hack: if anyone's to-table
		   animation hasn't completed, we wait for it to complete and
		   _then_ do the off-table animation. */
		for (p = 0; p < ggzcards.num_players; p++)
			if (anim[p].animating && anim[p].destination < 0) {
				dest_when_done = destination;
				return FALSE;
			}
	}


	/* 
	 * Build the anim structure.
	 */

	/* Find the initial position of the animation. */
	if (destination < 0) {
		/* We're moving the card out of the player's hand. */
		get_card_pos(player, card_num,
			     !preferences.single_click_play,
			     &start_x, &start_y);
		if (orientation(player) % 2 == 1) {
			/* The player's cards are horizontal, but we're going
			   to animate vertically.  So we recenter. */
			start_x += CARDHEIGHT / 2 - CARDWIDTH / 2;
			start_y += CARDWIDTH / 2 - CARDHEIGHT / 2;
		}
	} else {
		/* We're moving the card off of the table (or wherever it
		   happens to be). */
		if (anim[player].animating) {
			/* Is this really the right thing to do??? */
			start_x = anim[player].cur_x;
			start_y = anim[player].cur_y;
		} else {
			/* Move the card _off_ the table. */
			get_tablecard_pos(player, &start_x, &start_y);
		}
	}

	/* Find the ending position of the animation. */
	if (destination < 0) {
		/* We're moving the card to the table. */
		get_tablecard_pos(player, &dest_x, &dest_y);
	} else {
		/* We're moving the card to the player. */
		int x, y;
		/* FIXME: this should be its own layout function. */

		get_card_box_pos(destination, &x, &y);

		switch (orientation(destination)) {
		case FACE_BOTTOM:
			dest_x = x + CARD_BOX_WIDTH / 2;
			dest_y = y + TEXT_BOX_WIDTH;
			break;
		case FACE_LEFT:
			dest_x = x;
			dest_y = y + CARD_BOX_WIDTH / 2;
			break;
		case FACE_TOP:
			dest_x = x + CARD_BOX_WIDTH / 2;
			dest_y = y;
			break;
		case FACE_RIGHT:
			dest_x = x + TEXT_BOX_WIDTH;
			dest_y = y + CARD_BOX_WIDTH / 2;
			break;
		}

		dest_x -= CARDWIDTH / 2;
		dest_y -= CARDHEIGHT / 2;
	}

	anim[player].animating = TRUE;
	anim[player].destination = destination;
	anim[player].card = card;
	anim[player].start_x = start_x;
	anim[player].start_y = start_y;
	anim[player].dest_x = dest_x;
	anim[player].dest_y = dest_y;
	anim[player].cur_x = start_x;
	anim[player].cur_y = start_y;
	anim[player].cur_frame = 0;

	if (!animating)
		/* This sets up our timeout callback */
		anim_tag = gtk_timeout_add(DURATION / FRAMES,
					   animation_callback, NULL);

	animating = TRUE;

	return TRUE;
}

static gint start_offtable_animation(gpointer winner)
{
	int p;
	int winning_player = GPOINTER_TO_INT(winner);

	ggz_debug(DBG_ANIM, "Starting offtable animation.");

	assert(!preferences.animation || !animating);

	for (p = 0; p < ggzcards.num_players; p++) {
		card_t card = table_cards[p];

		if (preferences.animation && card.suit >= 0 && card.face >= 0)
			if (!animation_start(p, card, -1, winning_player))
				assert(0);

		table_cards[p] = UNKNOWN_CARD;
	}

	table_show_cards(!preferences.animation);

	if (!preferences.animation) {
		/* If animation is enabled, we don't start listening until the 
		   off-table animation completes.  But without animation we
		   must start listening again here. */
		listen_for_server(TRUE);
	}

	return FALSE;
}

void animate_cards_off_table(int winner)
{
	if (!preferences.animation || !animating) {
		/* This sets up our timeout callback */
		anim_tag = gtk_timeout_add(TABLE_CLEARING_DELAY,
					   start_offtable_animation,
					   GINT_TO_POINTER(winner));
	} else {
		int p;

		for (p = 0; p < ggzcards.num_players; p++) {
			card_t card = table_cards[p];
			if (anim[p].animating)
				assert(anim[p].destination < 0);

			/* Animate the card off the table - but only if there
			   _is_ a card there. */
			if (card.suit >= 0 && card.face >= 0)
				animation_start(p, card, -1, winner);
		}
	}

	listen_for_server(FALSE);
}


/* Handle one frame of card animation, this is triggered by a GtkTimeout setup 
   in animation_start(). */
static gint animation_callback(gpointer ignored)
{
	int continuations = 0;
	int player;

	int min_x = get_table_width(), min_y = get_table_height();
	int max_x = 0, max_y = 0;

	assert(animating);

	/* 
	 * First we draw the table onto the animation buffer, then we draw
	 * all the cards onto the animation buffer, then we draw the
	 * animation buffer to the window.
	 */

	/* Draw over the old animation and surrounding areas.  It's not really 
	   necessary to redraw the _whole thing_, but it's certainly easier. */
	table_draw_table(anim_buf, 0, 0, get_table_width(),
			 get_table_height());

	for (player = 0; player < ggzcards.num_players; player++) {
		if (!anim[player].animating)
			continue;

		anim[player].cur_frame++;

		/* Make sure we draw over the old position. */
		min_x = MIN(min_x, anim[player].cur_x);
		min_y = MIN(min_y, anim[player].cur_y);
		max_x = MAX(max_x, anim[player].cur_x + CARDWIDTH);
		max_y = MAX(max_y, anim[player].cur_y + CARDHEIGHT);

		/* Calculate our new position */
		anim[player].cur_x = anim[player].start_x +
			(anim[player].dest_x - anim[player].start_x)
			* anim[player].cur_frame / FRAMES;
		anim[player].cur_y = anim[player].start_y +
			(anim[player].dest_y - anim[player].start_y)
			* anim[player].cur_frame / FRAMES;

		/* Make sure we draw over the new position, too... */
		min_x = MIN(min_x, anim[player].cur_x);
		min_y = MIN(min_y, anim[player].cur_y);
		max_x = MAX(max_x, anim[player].cur_x + CARDWIDTH);
		max_y = MAX(max_y, anim[player].cur_y + CARDHEIGHT);

		/* If we are done, stop the animation process and draw the
		   card "for real" (if applicable).  Note that FRAMES might
		   change mid-animation, so we must be careful with this
		   check. */
		if (anim[player].cur_frame >= FRAMES) {
			anim[player].animating = FALSE;

			if (anim[player].destination < 0) {
				/* We'd better not draw to the screen here! */
				table_show_card(player, anim[player].card,
						FALSE);
			} else {
				/* Hack: we don't want to draw the card, and
				   we don't need to do anything else, so just
				   continue. */
				continue;
			}
		} else
			continuations++;

		/* Draw our new card position to the animation buffer */
		draw_card(anim[player].card, 0,
			  anim[player].cur_x, anim[player].cur_y, anim_buf);
	}



	/* Now draw from the animation buffer to the screen.  This could
	   probably be done more easily if we just drew the _whole screen_,
	   but that could have bad side effects. */
	gdk_draw_pixmap(table_drawing_area->window,
			table_style->fg_gc[GTK_WIDGET_STATE(table)],
			anim_buf,
			min_x, min_y,
			min_x, min_y, max_x - min_x, max_y - min_y);


	animating = (continuations > 0);

	if (!animating) {
		if (dest_when_done >= 0) {
			/* This sets up our timeout callback */
			anim_tag = gtk_timeout_add(TABLE_CLEARING_DELAY,
						   start_offtable_animation,
						   GINT_TO_POINTER
						   (dest_when_done));

			dest_when_done = -1;

			ggz_debug(DBG_MAIN,
				  "Delaying for off-table animation "
				  "from animation_callback.");
		} else {
			/* See the listen_for_server(FALSE) call in
			   animation_start. */
			listen_for_server(TRUE);
		}
	}

	return animating;
}


/* Function to stop the animation process. "success" is true if the card
   movement should continue to completion, false if it must be undone. */
void animation_stop(int success)
{
	int player;

	if (!animating)
		return;

	ggz_debug(DBG_ANIM, "Stopping animation (%d).", success);

	/* First, kill off the animation callback */
	gtk_timeout_remove(anim_tag);

	for (player = 0; player < ggzcards.num_players; player++) {

		if (!anim[player].animating)
			continue;

		table_draw_table(NULL,
				 anim[player].cur_x, anim[player].cur_y,
				 CARDWIDTH, CARDHEIGHT);

		anim[player].animating = FALSE;

		if (success) {
			/* And move the card to it's final resting place */
			table_show_card(player, anim[player].card, TRUE);
		} else {
			/* The caller is assumed to have restored the card to
			   the hand so we can redraw the full hand and should
			   be done.  However, if the animation was completed
			   then the card will be drawn on the table as well, so 
			   we'll need to clean that up. */
			table_display_hand(player, TRUE);
			table_show_cards(TRUE);
		}
	}

	animating = FALSE;
}
