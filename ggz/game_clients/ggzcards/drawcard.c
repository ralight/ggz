/* 
 * File: drawcard.c
 * Author: Jason Short
 * Project: GGZCards Client
 * Date: 04/20/2002
 * Desc: Routines to display cards
 * $Id: drawcard.c 6236 2004-11-03 06:15:32Z jdorje $
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
#include <gtk/gtk.h>
#include <stdio.h>

#include <ggz.h>

#include "cards.h"
#include "drawcard.h"
#include "game.h"		/* For preferences information */
#include "table.h"		/* HACK: we need table_style */

/* Card pictures for each of the 4 orientations */
struct {
	GdkPixbuf *front;
	GdkPixbuf *back;
} cards[4];

static cardset_type_t client_card_type = UNKNOWN_CARDSET;

static void load_french_cardset(void);

static void get_card_coordinates(card_t card, orientation_t orientation,
				 int *x, int *y);
static void draw_french_card(card_t card, orientation_t orientation, int x,
			     int y, GdkPixmap * image);
static void draw_domino_card(card_t card, orientation_t orientation, int x,
			     int y, GdkPixmap * image);

static int get_card_width0(void);
static int get_card_height0(void);

static GdkPixbuf *load_pixmap(const char *name)
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

static void load_french_cardset(void)
{
	int i;

	/* build pixmaps from the images */
	for (i = 0; i < 4 /* 4 orientations */ ; i++) {
		char fronts[32], backs[32];

		snprintf(fronts, sizeof(fronts), "cards-%d.png", i + 1);
		snprintf(backs, sizeof(backs), "cards-b%d.png", i + 1);

		cards[i].front = load_pixmap(fronts);
		cards[i].back = load_pixmap(backs);
	}
}

void load_card_data(cardset_type_t cardset_type)
{
	/* Check and set the card type */
	if (client_card_type == cardset_type)
		return;
	client_card_type = cardset_type;

	switch (cardset_type) {
	case CARDSET_FRENCH:
		load_french_cardset();
		break;
	case CARDSET_DOMINOES:
		break;
	case UNKNOWN_CARDSET:
		assert(FALSE);
		break;
	}
}

/* Returns the coordinates of the card out of the pixmap. */
static void get_card_coordinates(card_t card, orientation_t orientation,
				 int *x, int *y)
{
	int xc = 0, yc = 0;
	int xp, yp;
	int height = (orientation % 2 == 0) ? CARDHEIGHT : CARDWIDTH;
	int width = (orientation % 2 == 0) ? CARDWIDTH : CARDHEIGHT;

	/* We don't care about the deck, but the rest had better be accurate. */
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

	/* y is measured from the top; x from the left.  This just rotates the 
	   grid as necessary. */
	switch (orientation) {
	case FACE_BOTTOM:
		xc = xp;
		yc = yp;
		break;
	case FACE_LEFT:
		xc = 13 - yp - 1;
		yc = xp;
		break;
	case FACE_TOP:		/* just mirror everything */
		xc = 4 - xp - 1;
		yc = 13 - yp - 1;
		break;
	case FACE_RIGHT:
		xc = yp;
		yc = 4 - xp - 1;
		break;
	}

	*x = xc * width;
	*y = yc * height;
}

static void draw_french_card(card_t card, orientation_t orientation,
			     int x, int y, GdkPixmap * image)
{
	int width, height;
	int xc = 0, yc = 0;
	int show = (card.suit != -1 && card.face != -1);
	GdkPixbuf *pixbuf;

	assert(orientation >= 0 && orientation < 4);

	/* First find the width/height the card will need at this orientation. 
	 */
	width = get_card_width(orientation);
	height = get_card_height(orientation);

	if (show)
		get_card_coordinates(card, orientation, &xc, &yc);
	else {
		/* based on there being 4 different backs */
		/* TODO: do different decks differently */
		int xy[4][2] = { {2 * CARDWIDTH, 0},
		{0, 2 * CARDWIDTH},
		{CARDWIDTH, 0},
		{0, CARDWIDTH}
		};
		xc = xy[orientation][0];
		yc = xy[orientation][1];
	}
	pixbuf = show ? cards[orientation].front : cards[orientation].back;
	gdk_pixbuf_render_to_drawable(pixbuf, image,
				      table_style->
				      fg_gc[GTK_WIDGET_STATE(table)],
				      xc, yc, x, y, width, height,
				      GDK_RGB_DITHER_NONE, 0, 0);
}

static void draw_domino_card(card_t card, orientation_t orientation,
			     int x, int y, GdkPixmap * image)
{
	const int h = get_card_height0() - 1;
	const int spot_radius = (h) / 8;
	const int spots[7][7][2] = { {},
	{{h / 2, h / 2}},
	{{h / 3, h / 3},
	 {(2 * h) / 3, (2 * h) / 3}},
	{{h / 4, h / 4},
	 {h / 2, h / 2},
	 {(3 * h) / 4, (3 * h) / 4}},
	{{h / 4, h / 4},
	 {(3 * h) / 4, (3 * h) / 4},
	 {h / 4, (3 * h) / 4},
	 {(3 * h) / 4, h / 4}},
	{{h / 2, h / 2},
	 {h / 5, h / 5},
	 {(4 * h) / 5, h / 5},
	 {h / 5, (4 * h) / 5},
	 {(4 * h) / 5, (4 * h) / 5}},
	{{h / 4, h / 5},
	 {h / 4, h / 2},
	 {h / 4, (4 * h) / 5},
	 {(3 * h) / 4, h / 5},
	 {(3 * h) / 4, h / 2},
	 {(3 * h) / 4, (4 * h) / 5}}
	};
	int i, j;
	int show = (card.suit != -1 && card.face != -1);
	const int xo = (orientation % 2 == 0) ? h : 0;
	const int yo = (orientation % 2 == 0) ? 0 : h;

	assert(get_card_width0() - 1 == 2 * h);

	gdk_draw_rectangle(image,
			   show ? table_style->bg_gc[GTK_WIDGET_STATE(table)]
			   : table_style->fg_gc[GTK_WIDGET_STATE(table)],
			   TRUE,
			   x, y,
			   get_card_width(orientation),
			   get_card_height(orientation));

	if (!show)
		return;

	gdk_draw_rectangle(image,
			   table_style->fg_gc[GTK_WIDGET_STATE(table)],
			   FALSE, x, y, h, h);
	gdk_draw_rectangle(image,
			   table_style->fg_gc[GTK_WIDGET_STATE(table)],
			   FALSE, x + xo, y + yo, h, h);

	for (i = 0; i < 2; i++) {
		int num_spots = i ? card.face : card.suit;
		int x0 = x + i * xo;
		int y0 = y + i * yo;

		for (j = 0; j < num_spots; j++) {
			/* FIXME: the spots should technically be rotated on
			   the domino to be in the correct position. */
			int spot_x = x0 + spots[num_spots][j][0];
			int spot_y = y0 + spots[num_spots][j][1];

			gdk_draw_arc(image,
				     table_style->
				     fg_gc[GTK_WIDGET_STATE(table)], TRUE,
				     spot_x - spot_radius,
				     spot_y - spot_radius, 2 * spot_radius,
				     2 * spot_radius, 0, 360 * 64);
		}
	}

}

/* Draws the given card at the given location with the given orientation. */
void draw_card(card_t card, orientation_t orientation, int x, int y,
	       GdkPixmap * image)
{
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
		if (preferences.large_dominoes)
			return 97;
		else
			return 65;
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
		return get_card_width0() / 2 + 1;
	default:
		return 0;
	}
}

int get_card_width(orientation_t orientation)
{
	if (orientation % 2 == 0)
		return get_card_width0();
	else
		return get_card_height0();
}

int get_card_height(orientation_t orientation)
{
	if (orientation % 2 == 0)
		return get_card_height0();
	else
		return get_card_width0();
}

float get_card_visibility_width(void)
{
	float width = (float) get_card_width0();
	switch (client_card_type) {
	case CARDSET_FRENCH:
		return width / 5.0;
	case CARDSET_DOMINOES:
		return width + (float) CARD_OFFSET;
	default:
		return 0.0;
	}
}

float get_card_visibility_height(void)
{
	float height = (float) get_card_height0();
	return height + (float) CARD_OFFSET;
}
