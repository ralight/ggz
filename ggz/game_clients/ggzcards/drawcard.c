/*
 * File: drawcard.c
 * Author: Jason Short
 * Project: GGZCards Client
 * Date: 04/20/2002
 * Desc: Routines to display cards
 * $Id: drawcard.c 4027 2002-04-21 01:36:44Z jdorje $
 *
 * Copyright (C) 2002 GGZ Development Team.
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

#include <ggz.h>

#include "cards.h"
#include "table.h" /* HACK: we need table_style */

#include "drawcard.h"

#include "cards-1.xpm"
#include "cards-2.xpm"
#include "cards-3.xpm"
#include "cards-4.xpm"

#include "cards-b1.xpm"
#include "cards-b2.xpm"
#include "cards-b3.xpm"
#include "cards-b4.xpm"


/* Card front pictures for each of the 4 orientations */
GdkPixmap *card_fronts[4];
static GdkPixmap *card_backs[4];

void load_card_data(void)
{               	
	int i;
	GdkBitmap *mask;
	gchar **xpm_fronts[4] =
		{ cards_xpm, cards_2_xpm, cards_3_xpm, cards_4_xpm };
	gchar **xpm_backs[4] =
		{ cards_b1_xpm, cards_b2_xpm, cards_b3_xpm, cards_b4_xpm };


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
				  "couldn't load card pixmaps "
				  "for orientation %d.",
				  i);
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
			{ {2 * CARDWIDTH, 0},
			  {0, 2 * CARDWIDTH},
			  {CARDWIDTH, 0},
			  {0, CARDWIDTH} };
		xc = xy[orientation][0];
		yc = xy[orientation][1];
	}
	gdk_draw_pixmap(image,
			table_style->fg_gc[GTK_WIDGET_STATE(table)],
			show ? card_fronts[orientation] :
			card_backs[orientation], xc, yc, x, y, width, height);
}

void get_card_size(int orientation, int *w, int *h)
{
	switch (orientation) {
	case 0:
	case 2:
		*w = CARDWIDTH;
		*h = CARDHEIGHT;
		break;
	case 1:
	case 3:
		*w = CARDHEIGHT;
		*h = CARDWIDTH;
		break;
	default:
		ggz_debug("table",
			  "CLIENT BUG: get_card_size: unknown orientation %d.",
			  orientation);
	}
}
