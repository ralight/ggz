/*
 * File: animation.c
 * Author: Rich Gade, Jason Short
 * Project: GGZCards Client
 * Date: 12/18/2001
 * Desc: Animation code for GTK table
 * $Id: animation.c 2941 2001-12-18 22:29:24Z jdorje $
 *
 * Copyright (C) 2001 GGZ Development Team.
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

#include <stdlib.h>

#include <ggz.h>

#include "animation.h"
#include "layout.h"
#include "table.h"


#ifdef ANIMATION

int animating = 0;

static struct {
	int player;
	card_t card;

	int dest_x, dest_y;
	int card_x, card_y;
	float cur_x, cur_y;
	float step_x, step_y;
	gint cb_tag;
} anim;

/* Function to setup and trigger a card animation */
void animation_start(int player, card_t card, int card_num)
{
	int start_x, start_y;
	int end_x, end_y;

	get_card_pos(player, card_num, &start_x, &start_y);
	get_tablecard_pos(player, &end_x, &end_y);

	ggz_debug("main",
		  "Setting up animation for player %d from (%d, %d) to (%d, %d).",
		  player, start_x, start_y, end_x, end_y);

#define FRAMES		15
#define DURATION	500	/* In milliseconds */

	/* Store all our info */
	anim.card = card;
	anim.player = player;
	get_card_coordinates(card, 0, &anim.card_x, &anim.card_y);
	anim.cur_x = start_x;
	anim.cur_y = start_y;
	anim.dest_x = end_x;
	anim.dest_y = end_y;

	/* This sets up 15 frames of animation */
	anim.step_x = (start_x - end_x) / (float) FRAMES;
	anim.step_y = (start_y - end_y) / (float) FRAMES;

	/* This sets up our timeout callback */
	anim.cb_tag = gtk_timeout_add(DURATION / FRAMES,
				      animation_callback, NULL);

	animating = TRUE;
}


/* Handle one frame of card animation, this is
   triggered by a GtkTimeout setup in animation_start(). */
gint animation_callback(gpointer ignored)
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

	/* And refresh the on-screen image */
	if (new_x < anim.cur_x)
		ref_x = new_x;
	else
		ref_x = anim.cur_x;
	if (new_y < anim.cur_y)
		ref_y = new_y;
	else
		ref_y = anim.cur_y;
	table_show_table(ref_x, ref_y,
			 CARDWIDTH + abs(anim.step_x) + 2,
			 CARDHEIGHT + abs(anim.step_y) + 2);

	/* Draw our new card position */
	gdk_draw_pixmap(table->window,
			table_style->fg_gc[GTK_WIDGET_STATE(table)],
			card_fronts[0],
			anim.card_x, anim.card_y,
			new_x, new_y, CARDWIDTH, CARDHEIGHT);

	/* Update our information for next time */
	anim.cur_x = new_x;
	anim.cur_y = new_y;

	/* If we are there, stop the animation process */
	if (new_x == anim.dest_x && new_y == anim.dest_y) {
		animating = 0;
		return FALSE;
	}

	/* Continue animating */
	return TRUE;
}


/* Function to abort the animation process */
void animation_abort(void)
{
	ggz_debug("table", "Aborting animation.");

	/* First, kill off the animation callback */
	if (animating) {
		gtk_timeout_remove(anim.cb_tag);
		animating = 0;
	}

	/* The caller is assumed to have restored the card to the hand */
	/* so we can redraw the full hand and should be done */
	table_display_hand(anim.player);
}


/* Function to zip to the finish of an animation
   sequence so that a new one may be started */
void animation_zip(gboolean restore)
{
	if (!animating)
		return;

	/* First, kill off the animation callback */
	gtk_timeout_remove(anim.cb_tag);

	/* And move the card to it's final resting place */
	ggzcards.players[anim.player].table_card = anim.card;
	table_show_cards();

	animating = 0;
}

#endif /* ANIMATION */
