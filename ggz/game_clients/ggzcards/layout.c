/*
 * File: layout.c
 * Author: Jason Short, Rich Gade
 * Project: GGZCards Client
 * Date: 06/21/2001
 * Desc: Routines to get the layout for the game table
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

#include "game.h"
#include "hand.h"
#include "layout.h"


/* this is the upper left corner of the 4 text box squares.
 * the width is CARDHEIGHT+2*XWIDTH for all of them. */
/*
int text_boxes[4][2] = { {XWIDTH,                           WINDOW_HEIGHT-3*XWIDTH-CARDHEIGHT},
			 {XWIDTH,                           XWIDTH},
			 {WINDOW_WIDTH-3*XWIDTH-CARDHEIGHT, XWIDTH},
			 {WINDOW_WIDTH-3*XWIDTH-CARDHEIGHT, WINDOW_HEIGHT-3*XWIDTH-CARDHEIGHT} };
*/

/* this includes the upper left corner of the 4 card boxes, plus the orientation (0->3) of each box.
 *   the height is CARDHEIGHT+2*XWIDTH and width is HAND_WIDTH+2*XWIDTH for all. */
/*
int card_boxes[4][3] = { {3*XWIDTH + CARDHEIGHT,                WINDOW_HEIGHT - 3*XWIDTH - CARDHEIGHT, 0},
			 {XWIDTH,                               3*XWIDTH+CARDHEIGHT,                   1},
			 {3*XWIDTH+CARDHEIGHT,                  XWIDTH,                                2},
			 {WINDOW_WIDTH - 3*XWIDTH - CARDHEIGHT, 3*XWIDTH + CARDHEIGHT,                 3} };
*/


int player_boxes[4][3] = { {XWIDTH,                                 WINDOW_HEIGHT - XWIDTH - TEXT_BOX_WIDTH, 0},
			   {XWIDTH,                                 XWIDTH,                                  1},
			   {2*XWIDTH + TEXT_BOX_WIDTH,              XWIDTH,                                  2},
			   {WINDOW_WIDTH - XWIDTH - TEXT_BOX_WIDTH, 2*XWIDTH + TEXT_BOX_WIDTH,               3} };

int orientation(int p)
{
	return player_boxes[p][2];
}

void get_text_box_pos(int p, int *x, int *y)
{
	int or = orientation(p);
	*x = player_boxes[p][0];
	*y = player_boxes[p][1];
	if (or == 2)
		*x = player_boxes[p][0] + 2*XWIDTH + HAND_WIDTH;
	else if (or == 3)
		*y = player_boxes[p][1] + 2*XWIDTH + HAND_WIDTH;
}

void get_card_box_pos(int p, int *x, int *y)
{
	int or = orientation(p);
	*x = player_boxes[p][0];
	*y = player_boxes[p][1];
	if (or == 0)
		*x = player_boxes[p][0] + 2*XWIDTH + CARDHEIGHT;
	else if (or == 1)
		*y = player_boxes[p][1] + 2*XWIDTH + CARDHEIGHT;
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
			ggz_debug("CLIENT BUG: get_card_box_dim");
	}
}

void get_full_card_area(int p,
		int *x, int *y, /* the (x, y) position of the upper left corner */
		int *w, int *h, /* the width and height of the box */
		int *xo, int *yo /* the x and y offsets for "selected" cards */)
{
	/* the actual card area is inset within the card box by
	 * XWIDTH units, and extends inwards (toward the table)
	 * by 2*XWIDTH units. */

	/* get the outer box area */
	get_card_box_pos(p, x, y);
	get_card_box_dim(p, w, h);

	/* take the inset within the box */
	*x += XWIDTH;
	*y += XWIDTH;
	*w -= 2*XWIDTH;
	*h -= 2*XWIDTH;

	*xo = 0;
	*yo = 0;

	/* now account for the offset */
	switch (orientation(p)) {
		case 0:
			*y -= 2*XWIDTH;
			*h += 2*XWIDTH;
			*yo = -2*XWIDTH;
			break;
		case 1:
			*w += 2*XWIDTH;
			*xo = 2*XWIDTH;
			break;
		case 2:
			*h += 2*XWIDTH;
			*yo = 2*XWIDTH;
			break;
		case 3:
			*x -= 2*XWIDTH;
			*w += 2*XWIDTH;
			*xo = -2*XWIDTH;
			break;
		default:
			ggz_debug("CLIENT BUG: get_full_card_area");
	}
}

void get_inner_card_area_pos(int p, int *x, int *y)
{
	get_card_box_pos(p, x, y);
	*x += XWIDTH;
	*y += XWIDTH;
}

void get_card_offset(int p, float *w, float *h)
{
	switch (orientation(p)) {
		case 0:
		case 2:
			*w = CARDWIDTH/4.0;
			*h = 0;
			break;
		case 1:
		case 3:
			*w = 0;
			*h = CARDWIDTH/4.0;
			break;
		default:
			ggz_debug("CLIENT BUG: get_card_size: unknown orientation %d.", orientation(p));
	}
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
			ggz_debug("CLIENT BUG: get_card_size: unknown orientation %d.", orientation);		
	}
}


