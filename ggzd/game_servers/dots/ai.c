/*
 * File: ai.c
 * Author: Rich Gade
 * Project: GGZ Connect the Dots AI module
 * Date: 05/11/2000
 * Desc: Artificial Intelligence for CtD
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

#include <stdlib.h>

#include <ggz.h>
#include <game.h>
#include <protocols.h>

/* Global game variables */
struct dots_game_t dots_game;


/* Private functions */
static unsigned char ai_find_random_line(unsigned char *, unsigned char *);
static char ai_find_scoring_line(unsigned char *, unsigned char *);
static char ai_find_safe_line(unsigned char *, unsigned char *);
static void ai_handle_move(int, unsigned char *, unsigned char *);


unsigned char ai_move(unsigned char *x, unsigned char *y)
{
	char dir;

	/* Find a score first */
	dir = ai_find_scoring_line(x, y);

	/* If that fails... */
	if(dir == -1)
		/* Find a safe move */
		dir = ai_find_safe_line(x, y);

	/* If all else fails... */
	if(dir == -1)
		/* Move like a dumdum */
		dir = ai_find_random_line(x, y);

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


/* This chooses a random line to draw */
static unsigned char ai_find_random_line(unsigned char *x, unsigned char *y)
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


static void ai_handle_move(int dir, unsigned char *x, unsigned char *y)
{
	int num = dots_game.turn;
	char status = 0;

	ggz_debug("Handling AI move");

	if(dir == 0) {
		/* A vertical move */
		dots_game.vert_board[*x][*y] = 1;
		if(*x != 0
  	  	   && dots_game.vert_board[*x-1][*y]
   	   	   && dots_game.horz_board[*x-1][*y]
   	   	   && dots_game.horz_board[*x-1][*y+1]) {
			status++;
			dots_game.owners_board[*x-1][*y] = num;
		}
		if(*x != dots_game.board_width-1
	   	   && dots_game.vert_board[*x+1][*y]
	   	   && dots_game.horz_board[*x][*y]
	   	   && dots_game.horz_board[*x][*y+1]) {
			status++;
			dots_game.owners_board[*x][*y] = num;
		}
	} else {
		/* Horizontal move */
		dots_game.horz_board[*x][*y] = 1;
		if(*y != 0
	   	   && dots_game.horz_board[*x][*y-1]
	   	   && dots_game.vert_board[*x][*y-1]
	   	   && dots_game.vert_board[*x+1][*y-1]) {
			status++;
			dots_game.owners_board[*x][*y-1] = num;
		}
		if(*y != dots_game.board_height-1
	   	   && dots_game.horz_board[*x][*y+1]
	   	   && dots_game.vert_board[*x][*y]
	   	   && dots_game.vert_board[*x+1][*y]) {
			status++;
			dots_game.owners_board[*x][*y] = num;
		}
	}

	/* We make a note who our opponent is, easier on the update func */
	dots_game.opponent = (num + 1) % 2;
	if(status > 0)
		/* They scored, go again */
		dots_game.score[num] += status;
	else
		/* No score, turn passes */
		dots_game.turn = (dots_game.turn + 1) % 2;
}
