/* 
 * File: layout.c
 * Author: Jason Short
 * Project: GGZCards Client
 * Date: 06/21/2001
 * Desc: Routines to get the layout for the game table
 * $Id: layout.c 4048 2002-04-22 17:19:04Z jdorje $
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
#include <stdlib.h>

#include <ggz.h>
#include "client.h"

#include "game.h"
#include "drawcard.h"
#include "layout.h"
#include "table.h"

static int layout_min_text_width = 0;

int get_min_text_width(void)
{
	return layout_min_text_width;	
}

bool set_min_text_width(int min_text_width)
{
	if (min_text_width > layout_min_text_width) {
		layout_min_text_width = min_text_width;
		return TRUE;
	}
	return FALSE;
}

static void bottom_box4(int *x, int *y)
{
	*x = XWIDTH;
	*y = 2 * XWIDTH + TEXT_BOX_WIDTH + CARD_BOX_WIDTH;
}

static void left_box4(int *x, int *y)
{
	*x = XWIDTH;
	*y = XWIDTH;
}

static void top_box4(int *x, int *y)
{
	*x = 2 * XWIDTH + TEXT_BOX_WIDTH;
	*y = XWIDTH;
}

static void right_box4(int *x, int *y)
{
	*x = 2 * XWIDTH + TEXT_BOX_WIDTH + CARD_BOX_WIDTH;
	*y = 2 * XWIDTH + TEXT_BOX_WIDTH;
}

static void bottom_box6(int *x, int *y)
{
	*x = 2 * XWIDTH + TEXT_BOX_WIDTH;
	*y = 2 * XWIDTH + TEXT_BOX_WIDTH + 2 * CARD_BOX_WIDTH;
}

static void bl_box6(int *x, int *y)
{
	*x = XWIDTH;
	*y = 2 * XWIDTH + TEXT_BOX_WIDTH + CARD_BOX_WIDTH;
}

static void tl_box6(int *x, int *y)
{
	*x = XWIDTH;
	*y = XWIDTH;
}

static void top_box6(int *x, int *y)
{
	*x = 2 * XWIDTH + TEXT_BOX_WIDTH;
	*y = XWIDTH;
}

static void tr_box6(int *x, int *y)
{
	*x = 3 * XWIDTH + CARD_BOX_WIDTH + 2 * TEXT_BOX_WIDTH;
	*y = XWIDTH;
}

static void br_box6(int *x, int *y)
{
	*x = 3 * XWIDTH + CARD_BOX_WIDTH + 2 * TEXT_BOX_WIDTH;
	*y = 2 * XWIDTH + TEXT_BOX_WIDTH + CARD_BOX_WIDTH;
}

/* 
   #define BOTTOM_BOX {XWIDTH, 2 * XWIDTH + TEXT_BOX_WIDTH + CARD_BOX_WIDTH}
   #define LEFT_BOX {XWIDTH, XWIDTH} #define TOP_BOX {2 * XWIDTH +
   TEXT_BOX_WIDTH, XWIDTH} #define RIGHT_BOX {2 * XWIDTH + TEXT_BOX_WIDTH +
   CARD_BOX_WIDTH, 2 * XWIDTH + TEXT_BOX_WIDTH} */

typedef void (box_func) (int *, int *);
typedef struct layout_t {
	int orientations[MAX_NUM_PLAYERS];
	box_func *player_boxes[MAX_NUM_PLAYERS];
} layout_t;

static layout_t layout_2 = {
	{0, 2},
	{&bottom_box4, &top_box4}
};

static layout_t layout_3 = {
	{0, 1, 2},
	{&bottom_box4, &left_box4, &top_box4}
};

static layout_t layout_4 = {
	{0, 1, 2, 3},
	{&bottom_box4, &left_box4, &top_box4, &right_box4}
};


static layout_t layout_5 = {
	{0, 1, 1, 2, 3},
	{&bottom_box6, &bl_box6, &tl_box6, &top_box6, &tr_box6}
};

static layout_t layout_6 = {
	{0, 1, 1, 2, 3, 3},
	{&bottom_box6, &bl_box6, &tl_box6, &top_box6, &tr_box6, &br_box6}
};

static layout_t *layouts[MAX_NUM_PLAYERS + 1] =
	{ NULL, NULL, &layout_2, &layout_3, &layout_4, &layout_5, &layout_6 };

#define LAYOUT (layouts[ggzcards.num_players])
#define BOX(p) (LAYOUT->player_boxes[p])


int get_table_width(void)
{
	if (ggzcards.num_players == 0)
		return 300;
	if (ggzcards.num_players <= 4)
		return (3 * XWIDTH + 2 * TEXT_BOX_WIDTH + CARD_BOX_WIDTH);
	return 4 * XWIDTH + 3 * TEXT_BOX_WIDTH + CARD_BOX_WIDTH;
}

int get_table_height(void)
{
	if (ggzcards.num_players <= 4)
		return get_table_width();
	return 3 * XWIDTH + 2 * TEXT_BOX_WIDTH + 2 * CARD_BOX_WIDTH;
}

int orientation(int p)
{
	assert(LAYOUT);
	return LAYOUT->orientations[p];
}


void get_tablecard_pos(int p, int *x, int *y)
{
	int offset = CARDHEIGHT / 12;
	int mx = get_table_width() / 2, my = get_table_height() / 2;
	if (ggzcards.num_players <= 4) {
		int positions[4][2] = { {mx - CARDWIDTH / 2, my + offset},
		{mx - CARDWIDTH - offset, my - CARDHEIGHT / 2},
		{mx - CARDWIDTH / 2, my - offset - CARDHEIGHT},
		{mx + offset, my - CARDHEIGHT / 2}
		};
		*x = positions[p][0];
		*y = positions[p][1];
	} else {
		int positions[6][2] =
			{ {mx - CARDWIDTH / 2, my + offset + CARDHEIGHT / 2},
		{mx - CARDWIDTH - offset, my - CARDHEIGHT / 4},
		{mx - CARDWIDTH - offset, my - 3 * CARDHEIGHT / 4},
		{mx - CARDWIDTH / 2, my - offset - 3 * CARDHEIGHT / 2},
		{mx + offset, my - 3 * CARDHEIGHT / 4},
		{mx + offset, my - CARDHEIGHT / 4}
		};
		*x = positions[p][0];
		*y = positions[p][1];
	}
}

void get_table_dim(int *x, int *y, int *w, int *h)
{
	if (ggzcards.num_players <= 4) {
		*w = 2 * CARDWIDTH + CARDHEIGHT / 6;
		*h = 2 * CARDHEIGHT + CARDHEIGHT / 6;
	} else {
		*w = 2 * CARDWIDTH + CARDHEIGHT / 6;
		*h = 3 * CARDHEIGHT + CARDHEIGHT / 6;
	}
	*x = (get_table_width() - *w) / 2;
	*y = (get_table_height() - *h) / 2;
}

void get_fulltable_dim(int *x, int *y, int *w, int *h)
{
	*x = XWIDTH + TEXT_BOX_WIDTH;
	*y = XWIDTH + TEXT_BOX_WIDTH;
	*w = get_table_width() - 2 * (XWIDTH + TEXT_BOX_WIDTH);
	*h = get_table_height() - 2 * (XWIDTH + TEXT_BOX_WIDTH);
}


void get_text_box_pos(int p, int *x, int *y)
{
	int or = orientation(p);
	assert(LAYOUT);
	BOX(p) (x, y);
	if (or == 2)
		*x += CARD_BOX_WIDTH;
	else if (or == 3)
		*y += CARD_BOX_WIDTH;
}

void get_card_box_pos(int p, int *x, int *y)
{
	int or = orientation(p);
	assert(LAYOUT);
	BOX(p) (x, y);
	if (or == 0)
		*x += TEXT_BOX_WIDTH;
	else if (or == 1)
		*y += TEXT_BOX_WIDTH;
}

void get_card_box_dim(int p, int *w, int *h)
{
	switch (orientation(p)) {
	case 0:
	case 2:
		*w = CARD_BOX_WIDTH;
		*h = TEXT_BOX_WIDTH;
		break;
	case 1:
	case 3:
		*w = TEXT_BOX_WIDTH;
		*h = CARD_BOX_WIDTH;
		break;
	default:
		ggz_debug("table", "CLIENT BUG: get_card_box_dim");
	}
}

/* (*x,*y) is the position of the upper left corner.  (w,h) is the width and
   heigh of the box.  (*xo,*yo) is the offset vector to be used for
   "selected" cards. */
void get_full_card_area(int p, int *x, int *y, int *w, int *h, int *xo,
			int *yo)
{
	/* the actual card area is inset within the card box by XWIDTH units,
	   and extends inwards (toward the table) by 2*XWIDTH units. */

	/* get the outer box area */
	get_card_box_pos(p, x, y);
	get_card_box_dim(p, w, h);

	/* take the inset within the box */
	*x += XWIDTH;
	*y += XWIDTH;
	*w -= 2 * XWIDTH;
	*h -= 2 * XWIDTH;

	*xo = 0;
	*yo = 0;

	/* now account for the offset */
	switch (orientation(p)) {
	case 0:
		*y -= 2 * XWIDTH;
		*h += 2 * XWIDTH;
		*yo = -2 * XWIDTH;
		break;
	case 1:
		*w += 2 * XWIDTH;
		*xo = 2 * XWIDTH;
		break;
	case 2:
		*h += 2 * XWIDTH;
		*yo = 2 * XWIDTH;
		break;
	case 3:
		*x -= 2 * XWIDTH;
		*w += 2 * XWIDTH;
		*xo = -2 * XWIDTH;
		break;
	default:
		ggz_debug("table", "CLIENT BUG: get_full_card_area");
	}
}

void get_inner_card_area_pos(int p, int *x, int *y)
{
	get_card_box_pos(p, x, y);
	switch (orientation(p)) {
	case 0:
	case 2:
		*x += XWIDTH;
		*y += TEXT_BOX_WIDTH / 2 - get_card_height(0) / 2;
		break;
	case 1:
	case 3:
		*x += TEXT_BOX_WIDTH / 2 - get_card_width(1) / 2;
		*y += XWIDTH;
		break;
	}
}

void get_card_pos(int p, int card_num, int *x, int *y)
{
	int x0, y0;
	float w, h;
	get_inner_card_area_pos(p, &x0, &y0);
	get_card_offset(p, &w, &h);
	*x = x0 + (((float) card_num * w) + 0.5);
	*y = y0 + (((float) card_num * h) + 0.5);
}

void get_card_offset(int p, float *w, float *h)
{
	switch (orientation(p)) {
	case 0:
	case 2:
		*w = CARD_VISIBILITY;
		*h = 0;
		break;
	case 1:
	case 3:
		*w = 0;
		*h = CARD_VISIBILITY;
		break;
	default:
		ggz_debug("table",
			  "CLIENT BUG: get_card_size: unknown orientation %d.",
			  orientation(p));
	}
}
