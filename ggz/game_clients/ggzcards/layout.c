/* 
 * File: layout.c
 * Author: Jason Short
 * Project: GGZCards Client
 * Date: 06/21/2001
 * Desc: Routines to get the layout for the game table
 * $Id: layout.c 6293 2004-11-07 05:51:47Z jdorje $
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

/* This is the maximum hand size we can sustain.  The client-common code will
   tell us when we need to increase it. */
static int table_max_hand_size = 0;

int get_num_card_rows(void)
{
	switch (get_cardset_type()) {
	case CARDSET_DOMINOES:
		return 2;
	default:
		return 1;
	}
}

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


int get_max_hand_size(void)
{
	return table_max_hand_size;
}

void set_max_hand_size(int max_hand_size)
{
	table_max_hand_size = max_hand_size;

	do {
		/* the inner table must be at least large enough. So, if it's
		   not we make the hand sizes larger. */
		/* NOTE: get_table_dim/get_fulltable_size depends on
		   table_max_hand_size, so we must increment it directly in
		   this loop. */
		int x, y, w, h, w1, h1, d;
		get_table_dim(&x, &y, &w, &h);
		get_fulltable_dim(&d, &d, &w1, &h1);
		if (w1 > w && h1 > h)
			break;
		table_max_hand_size++;
	} while (1);
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

typedef enum {
	TEXT_ON_LEFT,
	TEXT_ON_RIGHT
} rotation_t;
typedef void (box_func) (int *, int *);
typedef struct layout_t {
	orientation_t orientations[MAX_NUM_PLAYERS];
	rotation_t rotations[MAX_NUM_PLAYERS];
	box_func *player_boxes[MAX_NUM_PLAYERS];
} layout_t;

static layout_t layout_2 = {
	{FACE_BOTTOM, FACE_TOP},
	{TEXT_ON_LEFT, TEXT_ON_LEFT},
	{&bottom_box4, &top_box4}
};

static layout_t layout_3 = {
	{FACE_BOTTOM, FACE_LEFT, FACE_TOP},
	{TEXT_ON_LEFT, TEXT_ON_LEFT, TEXT_ON_LEFT},
	{&bottom_box4, &left_box4, &top_box4}
};

static layout_t layout_4 = {
	{FACE_BOTTOM, FACE_LEFT, FACE_TOP, FACE_RIGHT},
	{TEXT_ON_LEFT, TEXT_ON_LEFT, TEXT_ON_LEFT, TEXT_ON_LEFT},
	{&bottom_box4, &left_box4, &top_box4, &right_box4}
};


static layout_t layout_5 = {
	{FACE_BOTTOM, FACE_LEFT, FACE_LEFT, FACE_TOP, FACE_RIGHT},
	{TEXT_ON_LEFT, TEXT_ON_RIGHT, TEXT_ON_LEFT, TEXT_ON_LEFT,
	 TEXT_ON_RIGHT},
	{&bottom_box6, &bl_box6, &tl_box6, &top_box6, &tr_box6}
};

static layout_t layout_6 = {
	{FACE_BOTTOM, FACE_LEFT, FACE_LEFT, FACE_TOP, FACE_RIGHT,
	 FACE_RIGHT},
	{TEXT_ON_LEFT, TEXT_ON_RIGHT, TEXT_ON_LEFT, TEXT_ON_LEFT,
	 TEXT_ON_RIGHT, TEXT_ON_LEFT},
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

int get_hand_width(void)
{
	int num_cards_wide = (get_max_hand_size() + NUM_CARD_ROWS - 1)
	    / NUM_CARD_ROWS;
	float extra_width = ((float)num_cards_wide - 1.0) * CARD_VISIBILITY
	    + 0.5;
	return CARDWIDTH + (int)extra_width;
}

int get_text_width(void)
{
	int text_width = get_min_text_width();
	float extra_height = ((float)NUM_CARD_ROWS - 1.0) *
	    get_card_visibility_height() + 0.5;

	return MAX(CARDHEIGHT + (int)extra_height, text_width);
}

orientation_t orientation(int p)
{
	assert(LAYOUT);
	return LAYOUT->orientations[p];
}

static rotation_t rotation(int p)
{
	assert(LAYOUT);
	return LAYOUT->rotations[p];
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
	orientation_t or = orientation(p);
	assert(LAYOUT);
	BOX(p) (x, y);

	if (rotation(p) == TEXT_ON_RIGHT)
		or = (or + 2) % 4;	/* hack */
	if (or == FACE_TOP)
		*x += CARD_BOX_WIDTH;
	else if (or == FACE_RIGHT)
		*y += CARD_BOX_WIDTH;
}

void get_card_box_pos(int p, int *x, int *y)
{
	orientation_t or = orientation(p);
	assert(LAYOUT);
	BOX(p) (x, y);

	if (rotation(p) == TEXT_ON_RIGHT)
		or = (or + 2) % 4;	/* hack */
	if (or == FACE_BOTTOM)
		*x += TEXT_BOX_WIDTH;
	else if (or == FACE_LEFT)
		*y += TEXT_BOX_WIDTH;
}

void get_card_box_dim(int p, int *w, int *h)
{
	switch (orientation(p)) {
	case FACE_BOTTOM:
	case FACE_TOP:
		*w = CARD_BOX_WIDTH;
		*h = TEXT_BOX_WIDTH;
		break;
	case FACE_LEFT:
	case FACE_RIGHT:
		*w = TEXT_BOX_WIDTH;
		*h = CARD_BOX_WIDTH;
		break;
	default:
		ggz_debug(DBG_TABLE, "CLIENT BUG: get_card_box_dim");
	}
}

static int get_card_selection_offset_x(int p)
{
	switch (orientation(p)) {
	case FACE_BOTTOM:
	case FACE_TOP:
		return 0;
	case FACE_LEFT:
		return 2 * XWIDTH;
	case FACE_RIGHT:
		return -2 * XWIDTH;
	}
	assert(FALSE);
	return 0;
}

static int get_card_selection_offset_y(int p)
{
	switch (orientation(p)) {
	case FACE_BOTTOM:
		return -2 * XWIDTH;
	case FACE_LEFT:
	case FACE_RIGHT:
		return 0;
	case FACE_TOP:
		return 2 * XWIDTH;
	}
	assert(FALSE);
	return 0;
}

/* (*x,*y) is the position of the upper left corner.  (w,h) is the width and
   heigh of the box.  (*xo,*yo) is the offset vector to be used for "selected" 
   cards. */
void get_full_card_area(int p, int *x, int *y, int *w, int *h)
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

	/* now account for the offset */
	switch (orientation(p)) {
	case FACE_BOTTOM:
		*y -= 2 * XWIDTH;
		*h += 2 * XWIDTH;
		break;
	case FACE_LEFT:
		*w += 2 * XWIDTH;
		break;
	case FACE_TOP:
		*h += 2 * XWIDTH;
		break;
	case FACE_RIGHT:
		*x -= 2 * XWIDTH;
		*w += 2 * XWIDTH;
		break;
	default:
		ggz_debug(DBG_TABLE, "CLIENT BUG: get_full_card_area");
	}
}

void get_inner_card_area_pos(int p, int *x, int *y)
{
	float extra_card_height =
	    ((float)NUM_CARD_ROWS - 1.0) * get_card_visibility_height();
	int card_area_height = get_card_height(0) + (int)extra_card_height;

	int wdiff = XWIDTH;
	int hdiff = TEXT_BOX_WIDTH / 2 - card_area_height / 2;

	get_card_box_pos(p, x, y);

	switch (orientation(p)) {
	case FACE_BOTTOM:
	case FACE_TOP:
		*x += wdiff;
		*y += hdiff;
		break;
	case FACE_LEFT:
	case FACE_RIGHT:
		*x += hdiff;
		*y += wdiff;
		break;
	}
}

static void get_card_coloffset(int p, float *w, float *h)
{
	switch (orientation(p)) {
	case FACE_BOTTOM:
	case FACE_TOP:
		*w = CARD_VISIBILITY;
		*h = 0;
		break;
	case FACE_LEFT:
	case FACE_RIGHT:
		*w = 0;
		*h = CARD_VISIBILITY;
		break;
	}
}

static void get_card_rowoffset(int p, float *w, float *h)
{
	switch (orientation(p)) {
	case FACE_BOTTOM:
	case FACE_TOP:
		*w = 0;
		*h = get_card_visibility_height();
		break;
	case FACE_LEFT:
	case FACE_RIGHT:
		*w = get_card_visibility_height();
		*h = 0;
		break;
	}
}

void get_card_pos(int p, int card_num, bool selected, int *x, int *y)
{
	int x0, y0;
	float w, h;

	/* FIXME */
	int cards_per_row =
	    (table_max_hand_size + NUM_CARD_ROWS - 1) / NUM_CARD_ROWS;
	int row = card_num / cards_per_row;
	float col = (float)(card_num % cards_per_row);
	int num_cards_on_row;

	if (row < NUM_CARD_ROWS - 1)
		num_cards_on_row = cards_per_row;
	else
		num_cards_on_row =
		    table_max_hand_size - (NUM_CARD_ROWS -
					   1) * cards_per_row;

	col += (float)(cards_per_row - num_cards_on_row) / 2.0;

	get_inner_card_area_pos(p, &x0, &y0);

	get_card_coloffset(p, &w, &h);
	x0 += (int)(((float)col * w) + 0.5);
	y0 += (int)(((float)col * h) + 0.5);

	if (orientation(p) == 1 || orientation(p) == 2)
		row = NUM_CARD_ROWS - row - 1;

	get_card_rowoffset(p, &w, &h);
	x0 += (int)(((float)row * w) + 0.5);
	y0 += (int)(((float)row * h) + 0.5);

	if (selected) {
		x0 += get_card_selection_offset_x(p);
		y0 += get_card_selection_offset_y(p);
	}

	*x = x0;
	*y = y0;
}
