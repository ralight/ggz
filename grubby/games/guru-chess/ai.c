/*
 * Chess game module for Guru
 * Copyright (C) 2004 Josef Spillner, josef@ggzgamingzone.org
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/* Header files */
#include <stdio.h>
#include <stdlib.h>
#include "ai.h"

/* Constant definitions */
#define C_EMPTY 0
#define C_PAWN 1
#define C_BISHOP 2
#define C_KNIGHT 3
#define C_ROCK 4
#define C_QUEEN 5
#define C_KING 6

#define C_FIGURE 0
#define C_COLOR 1

#define C_MOVE 0
#define C_MULTI 1

#define C_FROM 0
#define C_TO 1
#define C_VALUE 2

#define MAX_ITERATIONS 100
#define MAX_RECURSIONS 100

/* Global variables */
static int chess_ai_table[64][2];
static int movements[7][10][2];
static int chess_ai_queue[MAX_RECURSIONS][3];
static int chess_ai_queuepos;
static int best_from, best_to, best_value;

static int chess_ai_color;
static int chess_ai_depth;

/* Function prototypes */
static void chess_ai_init_board(void);
static void chess_ai_init_movements(void);
static void chess_ai_init_movementsinverse(void);
static int chess_ai_find_alphabeta(int color, int from, int to);

/*
 * 00 01 02 03 04 05 06 07
 * ...
 * 56 57 58 59 60 61 62 63
*/

static void chess_ai_init_board(void)
{
	int field;

	for(field = 0; field < 64; field++)
		chess_ai_table[field][C_FIGURE] = C_EMPTY;
	for(field = 0; field < 64; field++)
		chess_ai_table[field][C_COLOR] = C_NONE;

	for(field = 0; field <= 15; field++)
		chess_ai_table[field][C_COLOR] = C_WHITE;
	for(field = 48; field <= 63; field++)
		chess_ai_table[field][C_COLOR] = C_BLACK;

	for(field = 8; field <= 15; field++)
		chess_ai_table[field][C_FIGURE] = C_PAWN;
	for(field = 48; field <= 57; field++)
		chess_ai_table[field][C_FIGURE] = C_PAWN;

	chess_ai_table[0][C_FIGURE] = C_ROCK;
	chess_ai_table[7][C_FIGURE] = C_ROCK;
	chess_ai_table[56][C_FIGURE] = C_ROCK;
	chess_ai_table[63][C_FIGURE] = C_ROCK;

	chess_ai_table[1][C_FIGURE] = C_KNIGHT;
	chess_ai_table[6][C_FIGURE] = C_KNIGHT;
	chess_ai_table[57][C_FIGURE] = C_KNIGHT;
	chess_ai_table[62][C_FIGURE] = C_KNIGHT;

	chess_ai_table[2][C_FIGURE] = C_BISHOP;
	chess_ai_table[5][C_FIGURE] = C_BISHOP;
	chess_ai_table[58][C_FIGURE] = C_BISHOP;
	chess_ai_table[61][C_FIGURE] = C_BISHOP;

	chess_ai_table[3][C_FIGURE] = C_QUEEN;
	chess_ai_table[60][C_FIGURE] = C_QUEEN;

	chess_ai_table[4][C_FIGURE] = C_KING;
	chess_ai_table[59][C_FIGURE] = C_KING;
}

void chess_ai_init_movements(void)
{
	movements[C_EMPTY][0][C_MOVE] = 0;

	movements[C_PAWN][0][C_MOVE] = 8;
	movements[C_PAWN][0][C_MULTI] = 0;
	movements[C_PAWN][1][C_MOVE] = 7;
	movements[C_PAWN][1][C_MULTI] = 0;
	movements[C_PAWN][2][C_MOVE] = 9;
	movements[C_PAWN][2][C_MULTI] = 0;
	movements[C_PAWN][3][C_MOVE] = 0;

	movements[C_BISHOP][0][C_MOVE] = 7;
	movements[C_BISHOP][0][C_MULTI] = 1;
	movements[C_BISHOP][1][C_MOVE] = 9;
	movements[C_BISHOP][1][C_MULTI] = 1;
	movements[C_BISHOP][2][C_MOVE] = 0;

	movements[C_KNIGHT][0][C_MOVE] = 6;
	movements[C_KNIGHT][0][C_MULTI] = 0;
	movements[C_KNIGHT][1][C_MOVE] = 10;
	movements[C_KNIGHT][1][C_MULTI] = 0;
	movements[C_KNIGHT][2][C_MOVE] = 15;
	movements[C_KNIGHT][2][C_MULTI] = 0;
	movements[C_KNIGHT][3][C_MOVE] = 17;
	movements[C_KNIGHT][3][C_MULTI] = 0;
	movements[C_KNIGHT][4][C_MOVE] = 0;

	movements[C_ROCK][0][C_MOVE] = 8;
	movements[C_ROCK][0][C_MULTI] = 1;
	movements[C_ROCK][1][C_MOVE] = 1;
	movements[C_ROCK][1][C_MULTI] = 1;
	movements[C_ROCK][2][C_MOVE] = 0;

	movements[C_QUEEN][0][C_MOVE] = 8;
	movements[C_QUEEN][0][C_MULTI] = 1;
	movements[C_QUEEN][1][C_MOVE] = 1;
	movements[C_QUEEN][1][C_MULTI] = 1;
	movements[C_QUEEN][2][C_MOVE] = 7;
	movements[C_QUEEN][2][C_MULTI] = 1;
	movements[C_QUEEN][3][C_MOVE] = 9;
	movements[C_QUEEN][3][C_MULTI] = 1;
	movements[C_QUEEN][4][C_MOVE] = 0;

	movements[C_KING][0][C_MOVE] = 8;
	movements[C_KING][0][C_MULTI] = 0;
	movements[C_KING][1][C_MOVE] = 1;
	movements[C_KING][1][C_MULTI] = 0;
	movements[C_KING][2][C_MOVE] = 0;
}

void chess_ai_init_movementsinverse(void)
{
	int i, figure, max;

	for(figure = C_NONE; figure <= C_KING; figure++)
	{
		if(figure == C_PAWN) continue;

		for(max = 0; movements[figure][max][C_MOVE]; max++);
		/*printf("#moves: %i\n", max);*/
		for(i = max; i >= 0; i--)
		{
			movements[figure][i + max][C_MOVE] = -movements[figure][i][C_MOVE];
			movements[figure][i + max][C_MULTI] = movements[figure][i][C_MULTI];
			/*printf("figure %i: move %i to %i\n", figure, i, i + max);*/
		}
	}
}

void chess_ai_init(int color, int depth)
{
	if((color != C_WHITE) && (color != C_BLACK)) color = C_WHITE;
	if((depth < 0) || (depth >= MAX_RECURSIONS)) depth = 3;

	chess_ai_color = color;
	chess_ai_depth = depth;

	chess_ai_init_board();
	chess_ai_init_movements();
	chess_ai_init_movementsinverse();
}

int chess_ai_move(int from, int to)
{
	int figure, color;
	int i, j, max;
	int allowed, factor;
	int pos, oldpos;

	if((from < 0) || (from > 63)) return 0;
	if((to < 0) || (to > 63)) return 0;

	allowed = 0;
	figure = chess_ai_table[from][C_FIGURE];
	color = chess_ai_table[from][C_COLOR];

	factor = 1;
	if(color == C_BLACK) factor = -1;

	for(i = 0; movements[figure][i][C_MOVE]; i++)
	{
		/*printf("figure %i: movement %i\n", figure, i);*/
		max = 1;
		if(movements[figure][i][C_MULTI]) max = 8;
		pos = from;
		for(j = 1; j <= max; j++)
		{
			oldpos = pos;
			pos = from + movements[figure][i][C_MOVE] * factor * j;
			/*printf("try out %i->%i\n", from, pos);*/
			if((pos < 0) || (pos > 63)) break;
			if(chess_ai_table[pos][C_FIGURE] != C_EMPTY)
			{
				/*if(chess_ai_table[pos][C_COLOR] == color) break;
				if(chess_ai_table[pos][C_COLOR] == C_NONE) break;*/
				if(chess_ai_table[pos][C_COLOR] == color) break;
			}
			if(abs((pos % 8) - (oldpos % 8)) > 2) break;
			if(pos == to)
			{
				allowed = 1;
				if(figure == C_PAWN)
				{
					if((abs(from - to) == 8) && (chess_ai_table[pos][C_FIGURE] != C_EMPTY)) allowed = 0;
					if((abs(from - to) != 8) && (chess_ai_table[pos][C_FIGURE] == C_EMPTY)) allowed = 0;
				}
				break;
			}
		}
		if(allowed) break;
	}
	if(!allowed) return 0;

	chess_ai_table[to][C_FIGURE] = chess_ai_table[from][C_FIGURE];
	chess_ai_table[to][C_COLOR] = chess_ai_table[from][C_COLOR];
	chess_ai_table[from][C_FIGURE] = 0;
	chess_ai_table[from][C_COLOR] = C_NONE;

	return 1;
}

static int chess_ai_find_alphabeta(int color, int from, int to)
{
	int val, tmp;

	if(chess_ai_queuepos >= chess_ai_depth) return 0;

	tmp = chess_ai_table[to][C_FIGURE];
	chess_ai_table[to][C_FIGURE] = chess_ai_table[from][C_FIGURE];
	chess_ai_table[to][C_COLOR] = chess_ai_table[from][C_COLOR];
	chess_ai_table[from][C_FIGURE] = tmp;
	chess_ai_table[from][C_COLOR] = C_NONE;

	val = 1;
	if(tmp == C_KING) val = 500;
	if(tmp == C_QUEEN) val = 100;
	if(tmp == C_ROCK) val = 50;
	if(tmp == C_KNIGHT) val = 35;
	if(tmp == C_BISHOP) val = 35;
	if(tmp == C_PAWN) val = 10;

	chess_ai_queue[chess_ai_queuepos][C_VALUE] = val;
	chess_ai_queue[chess_ai_queuepos][C_FROM] = from;
	chess_ai_queue[chess_ai_queuepos][C_TO] = to;
	/*printf("++ (%i) %i->%i\n", chess_ai_queuepos, from, to);*/
	chess_ai_queuepos++;

	return 1;
}

int chess_ai_find(int color, int *from, int *to)
{
	int max;
	int figure;
	int i, j, k;
	int pos, oldpos, frompos, topos;
	int nextcolor, ret;
	int factor;
	int tmp;

	int tempqueue[MAX_ITERATIONS][3];
	int queuepos, maxqueue;

	factor = 1;
	if(color == C_BLACK) factor = -1;

	if((from) && (to))
	{
		best_value = 0;
		chess_ai_queuepos = 0;
		for(pos = 0; pos < MAX_RECURSIONS; pos++)
		{
			chess_ai_queue[pos][C_FROM] = 0;
			chess_ai_queue[pos][C_TO] = 0;
			chess_ai_queue[pos][C_VALUE] = 0;
		}
	}

	maxqueue = 0;
	for(i = 0; i < 64; i++)
	{
		if((chess_ai_table[i][C_FIGURE] != C_EMPTY)
		&& (chess_ai_table[i][C_COLOR] == color))
		{
			figure = chess_ai_table[i][C_FIGURE];
			/*printf("=== figure %i: at %i\n", figure, i);*/
			for(j = 0; movements[figure][j][C_MOVE]; j++)
			{
				/*printf("movement %i\n", j);*/
				max = 1;
				if(movements[figure][j][C_MULTI]) max = 8;
				pos = i;
				for(k = 1; k <= max; k++)
				{
					oldpos = pos;
					pos = i + movements[figure][j][C_MOVE] * factor * k;

					/*printf("try out %i->%i\n", i, pos);*/
					if(abs((pos % 8) - (oldpos % 8)) > 2) break;
					if((pos < 0) || (pos > 63)) break;
					if(chess_ai_table[pos][C_FIGURE] != C_EMPTY)
					{
						/*if(chess_ai_table[pos][C_COLOR] == color) break;
						if(chess_ai_table[pos][C_COLOR] == C_NONE) break;*/
						if(chess_ai_table[pos][C_COLOR] == color) break;
					}
					if(figure == C_PAWN)
					{
						if((abs(i - pos) == 8) && (chess_ai_table[pos][C_FIGURE] != C_EMPTY)) break;
						if((abs(i - pos) != 8) && (chess_ai_table[pos][C_FIGURE] == C_EMPTY)) break;
					}

					/*printf("store at %i\n", maxqueue);*/
					tempqueue[maxqueue][C_FROM] = i;
					tempqueue[maxqueue][C_TO] = pos;
					maxqueue++;
					if(maxqueue == MAX_ITERATIONS)
					{
						printf("Warning: reached limit of %i iterations!\n", MAX_ITERATIONS);
						break;
					}
				}
				if(maxqueue == MAX_ITERATIONS) break;
			}
		}
		if(maxqueue == MAX_ITERATIONS) break;
	}

	/*printf("---- OK, run %i parallel iterations\n", maxqueue);*/
	for(queuepos = 0; queuepos < maxqueue; queuepos++)
	{
		frompos = tempqueue[queuepos][C_FROM];
		topos = tempqueue[queuepos][C_TO];
		figure = chess_ai_table[frompos][C_FIGURE];

		/*printf("alphabeta (%i) (%i->%i)!\n", figure, frompos, topos);*/
		ret = chess_ai_find_alphabeta(color, frompos, topos);

		if(!ret)
		{
			/*printf("(iteration %i) looped %i recursions\n", queuepos, chess_ai_queuepos);*/
			break;
		}
		else
		{
			if(color == C_WHITE) nextcolor = C_BLACK;
			else nextcolor = C_WHITE;

			/*printf("(iteration %i) recursion level %i\n", queuepos, chess_ai_queuepos);*/
			chess_ai_find(nextcolor, NULL, NULL);
		}

		if((from) && (to))
		{
			if(chess_ai_queue[queuepos - 1][C_VALUE] > best_value)
			{
				best_from = chess_ai_queue[queuepos - 1][C_FROM];
				best_to = chess_ai_queue[queuepos - 1][C_TO];
				best_value = chess_ai_queue[queuepos - 1][C_VALUE];
				/*printf("assign: %i for %i->%i\n", best_value, best_from, best_to);*/
			}
		}
	}

	if(chess_ai_queuepos >= 0)
	{
		frompos = chess_ai_queue[chess_ai_queuepos - 1][C_FROM];
		topos = chess_ai_queue[chess_ai_queuepos - 1][C_TO];
		tmp = chess_ai_table[frompos][C_FIGURE];
		chess_ai_table[frompos][C_FIGURE] = chess_ai_table[topos][C_FIGURE];
		chess_ai_table[frompos][C_COLOR] = chess_ai_table[topos][C_COLOR];
		chess_ai_table[topos][C_FIGURE] = tmp;
		chess_ai_table[topos][C_COLOR] = color;
		/*printf("-- (%i) %i<-%i\n", chess_ai_queuepos - 1, frompos, topos);*/
		chess_ai_queuepos--;
		/*printf("(iteration %i) back to recursion level %i\n", queuepos, chess_ai_queuepos);*/
	}

	if((from) && (to))
	{
		/*printf("result: %i for %i->%i\n", best_value, best_from, best_to);*/
		*from = best_from;
		*to = best_to;
		return 1;
	}

	return 0;
}

void chess_ai_output(void)
{
	int i;
	char c;

	printf("---------------\n");
	for(i = 0; i < 64; i++)
	{
		c = ' ';
		switch(chess_ai_table[i][C_FIGURE])
		{
			case C_PAWN:
				c = '.';
				break;
			case C_BISHOP:
				c = 'I';
				break;
			case C_KNIGHT:
				c = 'P';
				break;
			case C_ROCK:
				c = 'H';
				break;
			case C_QUEEN:
				c = '*';
				break;
			case C_KING:
				c = '$';
				break;
		}
		printf("%c ", c);
		if((i + 1) % 8 == 0) printf("\n");
	}
	printf("---------------\n");
	fflush(NULL);
}

