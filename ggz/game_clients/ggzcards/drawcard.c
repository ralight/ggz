/*
 * File: drawcard.c
 * Author: Jason Short
 * Project: GGZCards Client
 * Date: 04/20/2002
 * Desc: Routines to display cards
 * $Id: drawcard.c 4034 2002-04-21 06:30:53Z jdorje $
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
#include "drawcard.h"
#include "table.h" /* HACK: we need table_style */

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

static enum card_type_enum client_card_type = -1;

static void get_card_coordinates(card_t card, int orientation, int *x, int *y);
static void draw_french_card(card_t card, int orientation,
                             int x, int y, GdkPixmap *image);
static void draw_domino_card(card_t card, int orientation,
                             int x, int y, GdkPixmap *image);

static int get_card_width0(void);
static int get_card_height0(void);

void load_card_data(enum card_type_enum card_type)
{               	
	int i;
	GdkBitmap *mask;
	gchar **xpm_fronts[4] =
		{ cards_xpm, cards_2_xpm, cards_3_xpm, cards_4_xpm };
	gchar **xpm_backs[4] =
		{ cards_b1_xpm, cards_b2_xpm, cards_b3_xpm, cards_b4_xpm };

	/* Check and set the card type */
#if 0
	assert(client_card_type == -1);
#endif
	client_card_type = card_type;

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
static void get_card_coordinates(card_t card, int orientation, int *x, int *y)
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

static void draw_french_card(card_t card, int orientation,
                             int x, int y, GdkPixmap *image)
{
	int width, height;
	int xc = 0, yc = 0;
	int show = (card.suit != -1 && card.face != -1);

	assert(orientation >= 0 && orientation < 4);

	/* First find the width/height the card will need at this
	   orientation. */
	width = get_card_width(orientation);
	height = get_card_height(orientation);

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

static void draw_domino_card(card_t card, int orientation,
                             int x, int y, GdkPixmap *image)
{
	const int h = get_card_height0();
	const int spot_radius = (h) / 8;
	const int spots[7][7][2] = { {},
	                             { {h / 2,       h / 2} },
	                             { {h / 3,       h / 3},
	                               {(2 * h) / 3, (2 * h) / 3} },
	                             { {h / 4,       h / 4},
	                               {h / 2,       h / 2},
	                               {(3 * h) / 4, (3 * h) / 4} },
	                             { {h / 4,       h / 4},
	                               {(3 * h) / 4, (3 * h) / 4},
	                               {h / 4,       (3 * h) / 4},
	                               {(3 * h) / 4, h / 4} },
	                             { {h / 2,       h / 2},
	                               {h / 5,       h / 5},
	                               {(4 * h) / 5, h / 5},
	                               {h / 5,       (4 * h) / 5},
	                               {(4 * h) / 5, (4 * h) / 5} },
	                             { {h / 3,       h / 5},
	                               {h / 3,       h / 2},
	                               {h / 3,       (4 * h) / 5},
	                               {(2 * h) / 3, h / 5},
	                               {(2 * h) / 3, h / 2},
	                               {(2 * h) / 3, (4 * h) / 5} } };
	int i, j;
	
	assert(get_card_width0() == 2 * get_card_height0());
	
	gdk_draw_rectangle(image,
	                   table_style->fg_gc[GTK_WIDGET_STATE(table)],
	                   FALSE,
	                   x, y,
	                   h, h);
	gdk_draw_rectangle(image,
	                   table_style->fg_gc[GTK_WIDGET_STATE(table)],
	                   FALSE,
	                   x + h + 1, y,
	                   h, h);
	
	for (i = 0; i < 2; i++) {
		char num_spots = i ? card.face : card.suit;
		int x0 = x + i * h;
		int y0 = y;
		
		for (j = 0; j < num_spots; j++) {
			int spot_x = x0 + spots[num_spots - 1][j][0];
			int spot_y = y0 + spots[num_spots - 1][j][1];
			
			gdk_draw_arc(image,
			             table_style->fg_gc[GTK_WIDGET_STATE(table)],
			             TRUE,
			             spot_x - spot_radius, spot_y - spot_radius,
			             2 * spot_radius, 2 * spot_radius,
			             0, 360 * 64);
		}
	}
	
}

/* Draws the given card at the given location with the given orientation. */
void draw_card(card_t card, int orientation, int x, int y, GdkPixmap * image)
{
	assert(card.type == client_card_type);
	
	switch (client_card_type) {
	case CARDSET_FRENCH:
		draw_french_card(card, orientation, x, y, image);
		break;
	case CARDSET_DOMINOES:
		draw_domino_card(card, orientation, x, y, image);
		break;
	default:
		assert(FALSE);
		break;
	}
}

/* The following will eventually be allowed to change. */
#define MY_CARDWIDTH	71
#define MY_CARDHEIGHT	96

static int get_card_width0(void)
{
	switch (client_card_type) {
	case CARDSET_FRENCH:
		return MY_CARDWIDTH;
	case CARDSET_DOMINOES:
		return 96;
	default:
		return 0;	
	}
}

static int get_card_height0(void)
{
	switch (client_card_type) {
	case CARDSET_FRENCH:
		return MY_CARDHEIGHT;
	case CARDSET_DOMINOES:
		return 48;
	default:
		return 0;	
	}
}

int get_card_width(int orientation)
{
	if (orientation % 2 == 0)
		return get_card_width0();
	else
		return get_card_height0();
}

int get_card_height(int orientation)
{
	if (orientation % 2 == 0)
		return get_card_height0();
	else
		return get_card_width0();
}

float get_card_visibility(void)
{
	float width = (float)get_card_width0();
	switch (client_card_type) {
	case CARDSET_FRENCH:
		return width / 4.0;
	case CARDSET_DOMINOES:
		return width * 1.1;
	default:
		return 0.0;
	}
}
