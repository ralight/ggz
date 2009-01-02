/*
 * File: ai.c
 * Author: Rich Gade
 * Project: GGZ Connect the Dots AI module
 * Date: 05/11/2000
 * Desc: Artificial Intelligence for CtD
 * $Id: ai.c 10784 2009-01-02 13:58:54Z oojah $
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
#  include <config.h>			/* Site-specific config */
#endif

#include <stdlib.h>
#include <string.h>

#include "ai.h"
#include "game.h"

/* Private functions */
static char ai_find_scoring_line(unsigned char *, unsigned char *);
static char ai_find_safe_line(unsigned char *, unsigned char *);
static char ai_find_best_giveaway(unsigned char *, unsigned char *);
#if 0
static char ai_find_random_line(unsigned char *, unsigned char *);
#endif /* #if 0 */
static unsigned char ai_lookahead(unsigned char, unsigned char, unsigned char);
static void ai_handle_move(int, unsigned char *, unsigned char *);


unsigned char ai_move(unsigned char *x, unsigned char *y)
{
	char dir;

	/* Find a score first */
	ggzdmod_log(dots_game.ggz, "AI: Looking for a score");
	dir = ai_find_scoring_line(x, y);

	/* If that fails... */
	if(dir == -1) {
		/* Find a safe move */
		ggzdmod_log(dots_game.ggz, "AI: Looking for a safe move");
		dir = ai_find_safe_line(x, y);
	}

	/* If we have no safe moves... */
	if(dir == -1) {
		/* Find the one that gives the least squares away */
		ggzdmod_log(dots_game.ggz, "AI: Looking for least painful move");
		dir = ai_find_best_giveaway(x, y);
	}

	ai_handle_move(dir, x, y);

	return dir;
}


/* This tries to find a move which results in a score */
static char ai_find_scoring_line(unsigned char *x, unsigned char *y)
{
	char try_dir;
	unsigned char try_x=0, try_y=0;

	try_dir = -1;
	for(try_x=0; try_x<dots_game.board_width && try_dir==-1; try_x++)
	    for(try_y=0; try_y<dots_game.board_height-1 &&try_dir==-1; try_y++)
		if(!dots_game.vert_board[try_x][try_y]) {
			if(try_x != 0
			   && try_y != dots_game.board_height-1
 	 	  	   && dots_game.vert_board[try_x-1][try_y]
  		   	   && dots_game.horz_board[try_x-1][try_y]
  		   	   && dots_game.horz_board[try_x-1][try_y+1])
				try_dir = 0;
			if(try_x != dots_game.board_width-1
			   && try_y != dots_game.board_height-1
		   	   && dots_game.vert_board[try_x+1][try_y]
		   	   && dots_game.horz_board[try_x][try_y]
		   	   && dots_game.horz_board[try_x][try_y+1])
				try_dir = 0;
		}

	if(try_dir == 0) {
		*x = try_x-1;
		*y = try_y-1;
		return try_dir;
	}

	for(try_x=0; try_x<dots_game.board_width-1 && try_dir==-1; try_x++)
	    for(try_y=0; try_y<dots_game.board_height && try_dir==-1; try_y++)
		if(!dots_game.horz_board[try_x][try_y]) {
			if(try_y != 0
			   && try_x != dots_game.board_width-1
 	 	  	   && dots_game.horz_board[try_x][try_y-1]
  		   	   && dots_game.vert_board[try_x][try_y-1]
  		   	   && dots_game.vert_board[try_x+1][try_y-1])
				try_dir = 1;
			if(try_y != dots_game.board_height-1
			   && try_x != dots_game.board_width-1
		   	   && dots_game.horz_board[try_x][try_y+1]
		   	   && dots_game.vert_board[try_x][try_y]
		   	   && dots_game.vert_board[try_x+1][try_y])
				try_dir = 1;
		}

	if(try_dir == 1) {
		*x = try_x-1;
		*y = try_y-1;
	}

	return try_dir;
}


/* This tries to find a safe move which prevents opponent from scoring */
static char ai_find_safe_line(unsigned char *x, unsigned char *y)
{
	char try_dir=0;
	unsigned char try_x=0, try_y=0;
	int holes;
	int ok = 0;
	char not_valid = 1;
	char ok_vert[dots_game.board_width][dots_game.board_height-1];
	char ok_horz[dots_game.board_width-1][dots_game.board_height];

	for(try_x=0; try_x<dots_game.board_width; try_x++)
	    for(try_y=0; try_y<dots_game.board_height-1; try_y++)
		if(!dots_game.vert_board[try_x][try_y]) {
			ok_vert[try_x][try_y] = 1;
			holes = 0;
			if(try_x != 0
			   && try_y != dots_game.board_height-1) {
 	 	  		if(!dots_game.vert_board[try_x-1][try_y])
					holes++;
  		   		if(!dots_game.horz_board[try_x-1][try_y])
					holes++;
  		   		if(!dots_game.horz_board[try_x-1][try_y+1])
					holes++;
				if(holes == 1)
					ok_vert[try_x][try_y] = 0;
			}
			holes = 0;
			if(try_x != dots_game.board_width-1
			   && try_y != dots_game.board_height-1) {
		   		if(!dots_game.vert_board[try_x+1][try_y])
					holes++;
		   		if(!dots_game.horz_board[try_x][try_y])
					holes++;
		   		if(!dots_game.horz_board[try_x][try_y+1])
					holes++;
				if(holes == 1)
					ok_vert[try_x][try_y] = 0;
			}
			if(ok_vert[try_x][try_y])
				ok++;
		} else
			ok_vert[try_x][try_y] = 0;

	for(try_x=0; try_x<dots_game.board_width-1; try_x++)
	    for(try_y=0; try_y<dots_game.board_height; try_y++)
		if(!dots_game.horz_board[try_x][try_y]) {
			ok_horz[try_x][try_y] = 1;
			holes = 0;
			if(try_y != 0
			   && try_x != dots_game.board_width-1) {
 	 	  		if(!dots_game.horz_board[try_x][try_y-1])
					holes++;
  		   		if(!dots_game.vert_board[try_x][try_y-1])
					holes++;
  		   		if(!dots_game.vert_board[try_x+1][try_y-1])
					holes++;
				if(holes == 1)
					ok_horz[try_x][try_y] = 0;
			}
			holes = 0;
			if(try_y != dots_game.board_height-1
			   && try_x != dots_game.board_width-1) {
		   		if(!dots_game.horz_board[try_x][try_y+1])
					holes++;
		   		if(!dots_game.vert_board[try_x][try_y])
					holes++;
		   		if(!dots_game.vert_board[try_x+1][try_y])
					holes++;
				if(holes == 1)
					ok_horz[try_x][try_y] = 0;
			}
			if(ok_horz[try_x][try_y])
				ok++;
		} else
			ok_horz[try_x][try_y] = 0;

	/* Are there any good moves? */
	if(!ok)
		/* Nopers */
		return -1;

	/* Pick one of our good moves at random */
	while(not_valid) {
		try_dir = random() % 2;
		if(try_dir == 0) {
			try_x = random() % dots_game.board_width;
			try_y = random() % (dots_game.board_height-1);
			if(ok_vert[try_x][try_y])
				not_valid = 0;
		} else {
			try_x = random() % (dots_game.board_width-1);
			try_y = random() % dots_game.board_height;
			if(ok_horz[try_x][try_y])
				not_valid = 0;
		}
	}

	*x = try_x;
	*y = try_y;
	return try_dir;
}


/* This chooses the move that gives away the fewest squares */
static char ai_find_best_giveaway(unsigned char *x, unsigned char *y)
{
	unsigned char try_dir, try_x, try_y, try_squares;
	unsigned char best_dir=0, best_x=0, best_y=0, best_squares=255;

	try_dir = 0;
	for(try_x=0; try_x<dots_game.board_width; try_x++)
		for(try_y=0; try_y<dots_game.board_height-1; try_y++) {
			if(!dots_game.vert_board[try_x][try_y]) {
				try_squares = ai_lookahead(try_dir,try_x,try_y);
				if(try_squares < best_squares) {
					best_dir = try_dir;
					best_x = try_x;
					best_y = try_y;
					best_squares = try_squares;
				}
				/* If only 1 square, it's as good as it gets */
				if(best_squares == 1)
					goto accept_move;
			}
		}

	try_dir = 1;
	for(try_x=0; try_x<dots_game.board_width-1; try_x++)
		for(try_y=0; try_y<dots_game.board_height; try_y++) {
			if(!dots_game.horz_board[try_x][try_y]) {
				try_squares = ai_lookahead(try_dir,try_x,try_y);
				if(try_squares < best_squares) {
					best_dir = try_dir;
					best_x = try_x;
					best_y = try_y;
					best_squares = try_squares;
				}
				/* If only 1 square, it's as good as it gets */
				if(best_squares == 1)
					goto accept_move;
			}
		}

accept_move:
	*x = best_x;
	*y = best_y;
	return best_dir;
}


/* This looks ahead at all opponent moves to determine how */
/* many squares we will end up giving away with this move  */
static unsigned char ai_lookahead(unsigned char dir,
				  unsigned char x, unsigned char y)
{
	unsigned char squares = 0;
	unsigned char vb_backup[MAX_BOARD_WIDTH][MAX_BOARD_HEIGHT-1];
	unsigned char hb_backup[MAX_BOARD_WIDTH-1][MAX_BOARD_HEIGHT];
	char try_dir = 0;
	unsigned char try_x, try_y;

	/* First we backup the game board so we can restore our state */
	memcpy(vb_backup, dots_game.vert_board, sizeof(vb_backup));
	memcpy(hb_backup, dots_game.horz_board, sizeof(hb_backup));

	/* Pretend we've made our move */
	if(dir == 0)
		dots_game.vert_board[x][y] = 1;
	else
		dots_game.horz_board[x][y] = 1;

	/* Count all moves which generate a score, keep */
	/* going until all scoring is done              */
	while(try_dir != -1) {
		try_dir = ai_find_scoring_line(&try_x, &try_y);
		if(try_dir == 0)
			dots_game.vert_board[try_x][try_y] = 1;
		else if(try_dir == 1)
			dots_game.horz_board[try_x][try_y] = 1;
		if(try_dir != -1)
			squares++;
	}

	/* Restore the board state to normal */
	memcpy(dots_game.vert_board, vb_backup, sizeof(vb_backup));
	memcpy(dots_game.horz_board, hb_backup, sizeof(hb_backup));

	/* Return the number of squares we would give away with this move */
	ggzdmod_log(dots_game.ggz, "AI: Considered %s move at %d, %d (%d squares)",
		  dir == 0 ? "vertical" : "horizontal", x, y, squares);
	return squares;
}


#if 0
/* This chooses a random line to draw */
static char ai_find_random_line(unsigned char *x, unsigned char *y)
{
	unsigned char try_dir, try_x, try_y;
	char not_valid = 1;

	while(not_valid) {
		try_dir = random() % 2;
		if(try_dir == 0) {
			try_x = random() % dots_game.board_width;
			try_y = random() % (dots_game.board_height-1);
			if(!dots_game.vert_board[try_x][try_y])
				not_valid = 0;
		} else {
			try_x = random() % (dots_game.board_width-1);
			try_y = random() % dots_game.board_height;
			if(!dots_game.horz_board[try_x][try_y])
				not_valid = 0;
		}
	}

	*x = try_x;
	*y = try_y;
	return try_dir;
}
#endif /* #if 0 */


static void ai_handle_move(int dir, unsigned char *x, unsigned char *y)
{
	int num = dots_game.turn;

	ggzdmod_log(dots_game.ggz, "Handling AI move");

	score = 0;

	if(dir == 0) {
		/* A vertical move */
		dots_game.vert_board[*x][*y] = 1;
		if(*x != 0
  	  	   && dots_game.vert_board[*x-1][*y]
   	   	   && dots_game.horz_board[*x-1][*y]
   	   	   && dots_game.horz_board[*x-1][*y+1]) {
			s_x[score] = *x-1;
			s_y[score] = *y;
			score++;
			dots_game.owners_board[*x-1][*y] = num;
		}
		if(*x != dots_game.board_width-1
	   	   && dots_game.vert_board[*x+1][*y]
	   	   && dots_game.horz_board[*x][*y]
	   	   && dots_game.horz_board[*x][*y+1]) {
			s_x[score] = *x;
			s_y[score] = *y;
			score++;
			dots_game.owners_board[*x][*y] = num;
		}
	} else {
		/* Horizontal move */
		dots_game.horz_board[*x][*y] = 1;
		if(*y != 0
	   	   && dots_game.horz_board[*x][*y-1]
	   	   && dots_game.vert_board[*x][*y-1]
	   	   && dots_game.vert_board[*x+1][*y-1]) {
			s_x[score] = *x;
			s_y[score] = *y-1;
			score++;
			dots_game.owners_board[*x][*y-1] = num;
		}
		if(*y != dots_game.board_height-1
	   	   && dots_game.horz_board[*x][*y+1]
	   	   && dots_game.vert_board[*x][*y]
	   	   && dots_game.vert_board[*x+1][*y]) {
			s_x[score] = *x;
			s_y[score] = *y;
			score++;
			dots_game.owners_board[*x][*y] = num;
		}
	}

	/* We make a note who our opponent is, easier on the update func */
	dots_game.opponent = (num + 1) % 2;
	if(score > 0)
		/* They scored, go again */
		dots_game.score[num] += score;
	else
		/* No score, turn passes */
		dots_game.turn = (dots_game.turn + 1) % 2;
}
