/*
 * File: ai.c
 * Author: Rich Gade
 * Project: GGZ Chinese Checkers AI module
 * Date: 01/01/2001
 * Desc: Artificial Intelligence for Chinese Checkers
 * $Id: ai.c 3990 2002-04-15 07:23:26Z jdorje $
 *
 * Copyright (C) 2001 Richard Gade.
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

#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"


/* Private functions */
static float ai_calc_dist(int, int, int, int);
static float ai_find_max_for_marble(int, int, int, int, int, int *, int *);

static int homexy[6][2] = {
	{16, 12},
	{12, 0},
	{4, 0},
	{0, 12},
	{4, 24},
	{12, 24},
};

static int homes[6][6] = {
	{ 0, -1, -1, -1, -1, -1 },	/* One player game (just filler) */
	{ 0,  3, -1, -1, -1, -1 },
	{ 0,  2,  4, -1, -1, -1 },
	{ 0,  1,  3,  4, -1, -1 },
	{ 0,  1,  2,  3,  4, -1 },	/* Five player game also filler */
	{ 0,  1,  2,  3,  4,  5 }
};

void ai_move(unsigned char *ro, unsigned char *co,
	     unsigned char *rd, unsigned char *cd)
{
	int dest, dest_r, dest_c;
	int temp;
	int i, j;
	int or[10], oc[10];
	float max_dist, t_dist;
	int max_rd=0, max_cd=0, max_ro=0, max_co=0, t_rd, t_cd;

	/* Calculate the point we want to move to */
	dest = (homes[ggzdmod_get_num_seats(game.ggz)-1][(int)game.turn]+3) % 6;
	dest_r = homexy[dest][0];
	dest_c = homexy[dest][1];

	/* Find all of our marbles */
	/* (this assumes we've already lost them :) */
	temp = 0;
	for(i=0; i<17; i++)
		for(j=0; j<25; j++)
			if(game.board[i][j] == game.turn+1) {
				or[temp] = i;
				oc[temp] = j;
				if(++temp == 10)
					break;
			}
	assert(temp == 10);

	/* Randomize the order the marbles are checked */
	for(i=0; i<10; i++) {
		j = random() % 10;
		while(i == j)
			j = random() % 10;
		t_rd = or[j];
		t_cd = oc[j];
		or[j] = or[i];
		oc[j] = oc[i];
		or[i] = t_rd;
		oc[i] = t_cd;
	}
		

	/* Find the maximum distance we can go */
	max_dist = -9999;
	for(i=0; i<10; i++) {
		t_dist = ai_find_max_for_marble(0, or[i], oc[i],
						dest_r, dest_c,
						&t_rd, &t_cd);
		if(t_dist >= max_dist) {
			max_rd = t_rd;
			max_cd = t_cd;
			max_ro = or[i];
			max_co = oc[i];
			max_dist = t_dist;
		}
	}

	/* Update the game board with our move */
	game.board[max_ro][max_co] = 0;
	game.board[max_rd][max_cd] = game.turn+1;

	/* Return the marble rows and cols for the biggest gain */
	*ro = max_ro;
	*co = max_co;
	*rd = max_rd;
	*cd = max_cd;
}


static float ai_find_max_for_marble(int from, int ro, int co,
					      int rd, int cd,
					      int *max_rd, int *max_cd)
{
	int r, c;
	float dist;
	static float initial_dist, max_dist;
	static char visited[17][25];

	/* This bootstraps the recursion process */
	if(from == 0) {
		/* Zero out the max and array of nodes we've visited */
		max_dist = -9999;
		for(r=0; r<17; r++)
			for(c=0; c<25; c++)
				visited[r][c] = 0;

		/* Find what our current distance is */
		initial_dist = ai_calc_dist(ro, co, rd, cd);
		if(game.turn_count > 8 && initial_dist > 19)
			initial_dist *= 1.3;
		if(game.turn_count > 15 && initial_dist > 15)
			initial_dist *= 1.6;

		/* Dir 1, no jump */
		if(co-2 >= 0
		   && game.board[ro][co-2] == 0) {
			dist = initial_dist - ai_calc_dist(ro, co-2, rd, cd);
			if(dist >= max_dist) {
				*max_rd = ro;
				*max_cd = co-2;
				max_dist = dist;
			}
		}

		/* Dir 2, no jump */
		if(ro-1 >= 0 && co-1 >= 0
		   && game.board[ro-1][co-1] == 0) {
			dist = initial_dist - ai_calc_dist(ro-1, co-1, rd, cd);
			if(dist >= max_dist) {
				*max_rd = ro-1;
				*max_cd = co-1;
				max_dist = dist;
			}
		}

		/* Dir 3, no jump */
		if(ro-1 >=0 && co+1 < 25
		   && game.board[ro-1][co+1] == 0) {
			dist = initial_dist - ai_calc_dist(ro-1, co+1, rd, cd);
			if(dist >= max_dist) {
				*max_rd = ro-1;
				*max_cd = co+1;
				max_dist = dist;
			}
		}

		/* Dir 4, no jump */
		if(co+2 < 25
		   && game.board[ro][co+2] == 0) {
			dist = initial_dist - ai_calc_dist(ro, co+2, rd, cd);
			if(dist >= max_dist) {
				*max_rd = ro;
				*max_cd = co+2;
				max_dist = dist;
			}
		}

		/* Dir 5, no jump */
		if(ro+1 < 17 && co+1 < 25
		   && game.board[ro+1][co+1] == 0) {
			dist = initial_dist - ai_calc_dist(ro+1, co+1, rd, cd);
			if(dist >= max_dist) {
				*max_rd = ro+1;
				*max_cd = co+1;
				max_dist = dist;
			}
		}

		/* Dir 6, no jump */
		if(ro+1 < 17 && co-1 >= 0
		   && game.board[ro+1][co-1] == 0) {
			dist = initial_dist - ai_calc_dist(ro+1, co-1, rd, cd);
			if(dist >= max_dist) {
				*max_rd = ro+1;
				*max_cd = co-1;
				max_dist = dist;
			}
		}

		/* Check all six possible jump directions recursively */
		visited[ro][co] = 1;
		if((co-4 >= 0)
		   && (game.board[ro][co-2] > 0)
		   && (game.board[ro][co-4] == 0))
			(void) ai_find_max_for_marble(1, ro, co-4,
							 rd, cd,
							 max_rd, max_cd);
		if((co-2 >= 0 && ro-2 >= 0)
		   && (game.board[ro-1][co-1] > 0)
		   && (game.board[ro-2][co-2] == 0))
			(void) ai_find_max_for_marble(2, ro-2, co-2,
							 rd, cd,
							 max_rd, max_cd);
		if((co+2 < 25 && ro-2 >= 0)
		   && (game.board[ro-1][co+1] > 0)
		   && (game.board[ro-2][co+2] == 0))
			(void) ai_find_max_for_marble(3, ro-2, co+2,
							 rd, cd,
							 max_rd, max_cd);
		if((co+4 < 25)
		   && (game.board[ro][co+2] > 0)
		   && (game.board[ro][co+4] == 0))
			(void) ai_find_max_for_marble(4, ro, co+4,
							 rd, cd,
							 max_rd, max_cd);
		if((co+2 < 25 && ro+2 < 17)
		   && (game.board[ro+1][co+1] > 0)
		   && (game.board[ro+2][co+2] == 0))
			(void) ai_find_max_for_marble(5, ro+2, co+2,
							 rd, cd,
							 max_rd, max_cd);
		if((co-2 >= 0 && ro+2 < 17)
		   && (game.board[ro+1][co-1] > 0)
		   && (game.board[ro+2][co-2] == 0))
			(void) ai_find_max_for_marble(6, ro+2, co-2,
							 rd, cd,
							 max_rd, max_cd);

		/* We should now have our max gain stored in max_dist */
		return max_dist;
	}

	/* Check to make sure we haven't visited here before */
	if(visited[ro][co])
		return 0;
	visited[ro][co] = 1;

	/* Check and store our current distance if it's a good one */
	dist = initial_dist - ai_calc_dist(ro, co, rd, cd);
	if(dist >= max_dist) {
		*max_rd = ro;
		*max_cd = co;
		max_dist = dist;
	}

	/* Now check all the six jump directions recursively */
	/* EXCEPT the one we came from */
	if((from != 4)
	   && (co-4 >= 0)
	   && (game.board[ro][co-2] > 0)
	   && (game.board[ro][co-4] == 0))
		(void) ai_find_max_for_marble(1, ro, co-4,
						 rd, cd,
						 max_rd, max_cd);
	if((from != 5)
	   && (co-2 >= 0 && ro-2 >= 0)
	   && (game.board[ro-1][co-1] > 0)
	   && (game.board[ro-2][co-2] == 0))
		(void) ai_find_max_for_marble(1, ro-2, co-2,
						 rd, cd,
						 max_rd, max_cd);
	if((from != 6)
	   && (co+2 < 25 && ro-2 >= 0)
	   && (game.board[ro-1][co+1] > 0)
	   && (game.board[ro-2][co+2] == 0))
		(void) ai_find_max_for_marble(1, ro-2, co+2,
						 rd, cd,
						 max_rd, max_cd);
	if((from != 1)
	   && (co+4 < 25)
	   && (game.board[ro][co+2] > 0)
	   && (game.board[ro][co+4] == 0))
		(void) ai_find_max_for_marble(1, ro, co+4,
						 rd, cd,
						 max_rd, max_cd);
	if((from != 2)
	   && (co+2 < 25 && ro+2 < 17)
	   && (game.board[ro+1][co+1] > 0)
	   && (game.board[ro+2][co+2] == 0))
		(void) ai_find_max_for_marble(1, ro+2, co+2,
						 rd, cd,
						 max_rd, max_cd);
	if((from != 3)
	   && (co-2 >= 0 && ro+2 < 17)
	   && (game.board[ro+1][co-1] > 0)
	   && (game.board[ro+2][co-2] == 0))
		(void) ai_find_max_for_marble(1, ro+2, co-2,
						 rd, cd,
						 max_rd, max_cd);

	/* It really doesn't matter what we return */
	return 0;
}


static float ai_calc_dist(int r1, int c1, int r2, int c2)
{
	if(r1 == r2 && c1 == c2)
		return 0;

	/* 1.47 = ratio that rows are > in dist than cols */
	return hypot( (r1-r2)*1.47, (c1-c2) );
}
