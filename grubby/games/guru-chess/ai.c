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
#define C_ROOK 4
#define C_QUEEN 5
#define C_KING 6

#define C_FIGURE 0
#define C_COLOR 1

#define C_MOVE 0
#define C_MULTI 1

#define C_FROM 0
#define C_TO 1
#define C_VALUE 2
#define C_ORIGFIGURE 3
#define C_ORIGCOLOR 4

#define MAX_ITERATIONS 100
#define MAX_RECURSIONS 100

/* Global variables */
static int chess_ai_table[64][2];
static int movements[7][10][2];
static int chess_ai_queue[MAX_RECURSIONS][5];
static int chess_ai_taken[32][2];

static int chess_ai_queuepos;
static int best_from, best_to, best_value;

static int chess_ai_color;
static int chess_ai_depth;

/* Function prototypes */
static void chess_ai_init_board(void);
static void chess_ai_init_movements(void);
static void chess_ai_init_movementsinverse(void);
static int chess_ai_find_alphabeta(int color, int from, int to);
static int chess_ai_moveexceptions(int from, int to);

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

	chess_ai_table[0][C_FIGURE] = C_ROOK;
	chess_ai_table[7][C_FIGURE] = C_ROOK;
	chess_ai_table[56][C_FIGURE] = C_ROOK;
	chess_ai_table[63][C_FIGURE] = C_ROOK;

	chess_ai_table[1][C_FIGURE] = C_KNIGHT;
	chess_ai_table[6][C_FIGURE] = C_KNIGHT;
	chess_ai_table[57][C_FIGURE] = C_KNIGHT;
	chess_ai_table[62][C_FIGURE] = C_KNIGHT;

	chess_ai_table[2][C_FIGURE] = C_BISHOP;
	chess_ai_table[5][C_FIGURE] = C_BISHOP;
	chess_ai_table[58][C_FIGURE] = C_BISHOP;
	chess_ai_table[61][C_FIGURE] = C_BISHOP;

	chess_ai_table[3][C_FIGURE] = C_QUEEN;
	chess_ai_table[59][C_FIGURE] = C_QUEEN;

	chess_ai_table[4][C_FIGURE] = C_KING;
	chess_ai_table[60][C_FIGURE] = C_KING;
}

static void chess_ai_init_movements(void)
{
	movements[C_EMPTY][0][C_MOVE] = 0;

	movements[C_PAWN][0][C_MOVE] = 8;
	movements[C_PAWN][0][C_MULTI] = 0;
	movements[C_PAWN][1][C_MOVE] = 7;
	movements[C_PAWN][1][C_MULTI] = 0;
	movements[C_PAWN][2][C_MOVE] = 9;
	movements[C_PAWN][2][C_MULTI] = 0;
	movements[C_PAWN][3][C_MOVE] = 16;
	movements[C_PAWN][3][C_MULTI] = 0;
	movements[C_PAWN][4][C_MOVE] = 0;

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

	movements[C_ROOK][0][C_MOVE] = 8;
	movements[C_ROOK][0][C_MULTI] = 1;
	movements[C_ROOK][1][C_MOVE] = 1;
	movements[C_ROOK][1][C_MULTI] = 1;
	movements[C_ROOK][2][C_MOVE] = 0;

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
	movements[C_KING][2][C_MOVE] = 7;
	movements[C_KING][2][C_MULTI] = 0;
	movements[C_KING][3][C_MOVE] = 9;
	movements[C_KING][3][C_MULTI] = 0;
	movements[C_KING][4][C_MOVE] = 0;
}

static void chess_ai_init_movementsinverse(void)
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

static void chess_ai_init_taken(void)
{
	int i;

	for(i = 0; i < 32; i++)
	{
		chess_ai_taken[i][C_FIGURE] = C_EMPTY;
		chess_ai_taken[i][C_COLOR] = C_NONE;
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
	chess_ai_init_taken();
}

static int chess_ai_moveexceptions(int from, int to)
{
	int allowed, newpos;
	int k, l;
	int figure;
	int factor;

	figure = chess_ai_table[from][C_FIGURE];
	allowed = 1;

	if(figure == C_PAWN)
	{
		/* RULE exception: pawn moves forward, beats diagonally */
		if((!(abs(from - to) % 8)) && (chess_ai_table[to][C_FIGURE] != C_EMPTY)) allowed = 0;
		if((abs(from - to) % 8) && (chess_ai_table[to][C_FIGURE] == C_EMPTY)) allowed = 0;
		/* RULE exception: force pawn to beat if possible */
		/*if((!(abs(from - to) % 8)) && (from > 7) && (from < 56))
		{
			factor = 1;
			if(chess_ai_table[from][C_COLOR] == C_BLACK) factor = -1;
			if(((to % 8) > 0) && (chess_ai_table[to - 1][C_FIGURE] != C_EMPTY))
				if(chess_ai_table[to - 1][C_COLOR] != chess_ai_table[from][C_COLOR]) allowed = 0;
			if(((to % 8) < 7) && (chess_ai_table[to + 1][C_FIGURE] != C_EMPTY))
				if(chess_ai_table[to + 1][C_COLOR] != chess_ai_table[from][C_COLOR]) allowed = 0;
		}*/
		/* RULE exception: pawn can move forward 2 fields only at the beginning */
		if((abs(from - to) == 16) && (from > 15) && (chess_ai_table[from][C_COLOR] == C_WHITE)) allowed = 0;
		if((abs(from - to) == 16) && (from < 48) && (chess_ai_table[from][C_COLOR] == C_BLACK)) allowed = 0;
	}
	if(figure == C_KING)
	{
		/* RULE exception: two kings must never meet each other */
		for(k = to / 8 - 1; k <= to / 8 + 1; k++)
			for(l = to % 8 - 1; l <= to % 8 + 1; l++)
			{
				newpos = k * 8 + l;
				if((newpos == from) || (newpos == to)) continue;
				if(chess_ai_table[newpos][C_FIGURE] == C_KING) allowed = 0;
			}
	}

	return allowed;
}

static int chess_ai_value(int figure)
{
	int val;

	val = 1;
	if(figure == C_KING) val = 5000;
	if(figure == C_QUEEN) val = 100;
	if(figure == C_ROOK) val = 50;
	if(figure == C_KNIGHT) val = 35;
	if(figure == C_BISHOP) val = 35;
	if(figure == C_PAWN) val = 10;

	return val;
}

int chess_ai_checkmate(void)
{
	int i;

	for(i = 0; chess_ai_taken[i][C_FIGURE] != C_EMPTY; i++)
	{
		if(chess_ai_taken[i][C_FIGURE] == C_KING)
		{
			return 1;
		}
	}
	return 0;
}

static void chess_ai_take(int color, int figure, int takeback)
{
	int i, j;

/*printf("**TAKE** COLOR %i FIGURE %i TAKEBACK %i\n", color, figure, takeback);*/
	for(i = 0; chess_ai_taken[i][C_FIGURE] != C_EMPTY; i++)
	{
		if((takeback) && (chess_ai_taken[i][C_FIGURE] == figure) && (chess_ai_taken[i][C_COLOR] == color))
		{
/*printf("**TAKE** remove at %i\n", i);*/
			for(j = i + 1; chess_ai_taken[j][C_FIGURE] != C_EMPTY; j++)
			{
				chess_ai_taken[j - 1][C_FIGURE] = chess_ai_taken[j][C_FIGURE];
				chess_ai_taken[j - 1][C_COLOR] = chess_ai_taken[j][C_COLOR];
			}
/*printf("**TAKE** empty at %i\n", j - 1);*/
			chess_ai_taken[j - 1][C_FIGURE] = C_EMPTY;
			chess_ai_taken[j - 1][C_COLOR] = C_NONE;
			break;
		}
	}
	if(!takeback)
	{
/*printf("**TAKE** insert at %i\n", i);*/
		chess_ai_taken[i][C_FIGURE] = figure;
		chess_ai_taken[i][C_COLOR] = color;
	}
}

int chess_ai_exchange(int pos, int *figure)
{
	int color, tmpfigure, oldfigure;
	int i, value;

	if((pos < 0) || (pos > 63)) return 0;
	if((pos >= 8) && (pos < 56)) return 0;

	oldfigure = chess_ai_table[pos][C_FIGURE];
	if(oldfigure != C_PAWN) return 0;

	color = chess_ai_table[pos][C_COLOR];
	if((pos < 8) && (color != C_BLACK)) return 0;
	if((pos >= 56) && (color != C_WHITE)) return 0;

	oldfigure = C_PAWN;
	value = chess_ai_value(oldfigure);
	for(i = 0; chess_ai_taken[i][C_FIGURE] != C_EMPTY; i++)
	{
		if(chess_ai_taken[i][C_COLOR] != color) continue;
		tmpfigure = chess_ai_taken[i][C_FIGURE];
		if(chess_ai_value(tmpfigure) > value)
		{
			oldfigure = tmpfigure;
			value = chess_ai_value(oldfigure);
/*printf("take-consider: figure %i has higher value %i\n", oldfigure, value);*/
		}
	}
	if(oldfigure == C_PAWN) return 0;

	chess_ai_take(color, C_PAWN, 0);
	chess_ai_take(color, oldfigure, 1);

	chess_ai_table[pos][C_FIGURE] = oldfigure;
	chess_ai_table[pos][C_COLOR] = color;

	*figure = oldfigure;
	return 1;
}

int chess_ai_rochade(int color, int which)
{
	int from, to;
	int i, step;

	step = 1;
	if(color == C_WHITE)
	{
		from = 4;
		to = 7;
	}
	else if(color == C_BLACK)
	{
		from = 60;
		to = 63;
	}
	else return 0;

	if(which)
	{
		step = -1;
		if(color == C_WHITE) to = 0;
		else if(color == C_BLACK) to = 56;
	}

// FIXME: not allowed to ever have moved king or rook
	if(chess_ai_table[from][C_FIGURE] != C_KING) return 0;
	if(chess_ai_table[to][C_FIGURE] != C_ROOK) return 0;

	for(i = from + step; i != to; i += step)
		if(chess_ai_table[i][C_FIGURE] != C_EMPTY) return 0;

	return 1;
}

int chess_ai_move(int from, int to, int force)
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
				if(pos != to) break;
			}
			if(abs((pos % 8) - (oldpos % 8)) > 2) break;
			if(pos == to)
			{
				allowed = chess_ai_moveexceptions(from, to);
				break;
			}
		}
		if(allowed) break;
	}
	if((!allowed) && (!force)) return 0;

	if(chess_ai_table[to][C_FIGURE] != C_EMPTY)
		chess_ai_take(chess_ai_table[to][C_COLOR], chess_ai_table[to][C_FIGURE], 0);

	chess_ai_table[to][C_FIGURE] = chess_ai_table[from][C_FIGURE];
	chess_ai_table[to][C_COLOR] = chess_ai_table[from][C_COLOR];
	chess_ai_table[from][C_FIGURE] = 0;
	chess_ai_table[from][C_COLOR] = C_NONE;

	return allowed;
}

static int chess_ai_find_alphabeta(int color, int from, int to)
{
	int val, tmpfigure, tmpcolor;
	int i;

	if(chess_ai_queuepos >= chess_ai_depth) return 0;

	tmpfigure = chess_ai_table[to][C_FIGURE];
	tmpcolor = chess_ai_table[to][C_COLOR];
	chess_ai_table[to][C_FIGURE] = chess_ai_table[from][C_FIGURE];
	chess_ai_table[to][C_COLOR] = chess_ai_table[from][C_COLOR];
	chess_ai_table[from][C_FIGURE] = C_EMPTY;
	chess_ai_table[from][C_COLOR] = C_NONE;

	val = chess_ai_value(tmpfigure);

	chess_ai_queue[chess_ai_queuepos][C_FROM] = from;
	chess_ai_queue[chess_ai_queuepos][C_TO] = to;
	chess_ai_queue[chess_ai_queuepos][C_ORIGFIGURE] = tmpfigure;
	chess_ai_queue[chess_ai_queuepos][C_ORIGCOLOR] = tmpcolor;
	if(val > chess_ai_queue[chess_ai_queuepos][C_VALUE])
	{
		chess_ai_queue[chess_ai_queuepos][C_VALUE] = val;
		/*printf("in level %i, val is altered to %i (moves:", chess_ai_queuepos, val);
		for(i = 0; i < chess_ai_queuepos + 1; i++)
			printf(" %i->%i", chess_ai_queue[i][C_FROM], chess_ai_queue[i][C_TO]);
		printf(")\n");*/
		for(i = chess_ai_queuepos + 1; i < chess_ai_depth; i++)
			chess_ai_queue[i][C_VALUE] = 0;
	}
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
	int movevalue;

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
			chess_ai_queue[pos][C_ORIGCOLOR] = C_NONE;
			chess_ai_queue[pos][C_ORIGFIGURE] = C_EMPTY;
		}
	}

	maxqueue = 0;
	for(i = 0; i < 64; i++)
	{
		if((chess_ai_table[i][C_FIGURE] != C_EMPTY)
		&& (chess_ai_table[i][C_COLOR] == color))
		{
			figure = chess_ai_table[i][C_FIGURE];
			/*printf("=== figure <%i>: at %i\n", figure, i);*/
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

					if(!chess_ai_moveexceptions(i, pos)) continue;

					/*printf("store at %i\n", maxqueue);*/
					tempqueue[maxqueue][C_FROM] = i;
					tempqueue[maxqueue][C_TO] = pos;
					maxqueue++;
					if(maxqueue == MAX_ITERATIONS)
					{
						printf("Warning: reached limit of %i iterations!\n", MAX_ITERATIONS);
						break;
					}
					if(chess_ai_table[pos][C_FIGURE] != C_EMPTY)
						break;
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

		/*printf("alphabeta (<%i>) (%i->%i)!\n", figure, frompos, topos);*/
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
			if(chess_ai_queuepos < chess_ai_depth)
				chess_ai_find(nextcolor, NULL, NULL);

			frompos = chess_ai_queue[chess_ai_queuepos - 1][C_FROM];
			topos = chess_ai_queue[chess_ai_queuepos - 1][C_TO];
			chess_ai_table[frompos][C_FIGURE] = chess_ai_table[topos][C_FIGURE];
			chess_ai_table[frompos][C_COLOR] = chess_ai_table[topos][C_COLOR];
			chess_ai_table[topos][C_FIGURE] = chess_ai_queue[chess_ai_queuepos - 1][C_ORIGFIGURE];
			chess_ai_table[topos][C_COLOR] = chess_ai_queue[chess_ai_queuepos - 1][C_ORIGCOLOR];
			/*printf("-- (%i) %i<-%i\n", chess_ai_queuepos - 1, frompos, topos);*/
			chess_ai_queuepos--;
			/*printf("(iteration %i) back to recursion level %i\n", queuepos, chess_ai_queuepos);*/
		}

		if((from) && (to))
		{
			movevalue = 0;
			for(i = 0; i < chess_ai_depth; i++)
				movevalue += chess_ai_queue[i][C_VALUE] * (((i + 1) % 2) * 2 - 1);
			/*printf("iteration (%i: %i->%i) %i\n", queuepos, tempqueue[queuepos][C_FROM], tempqueue[queuepos][C_TO], movevalue);*/
			tempqueue[queuepos][C_VALUE] = movevalue;
			for(i = 0; i < MAX_RECURSIONS; i++)
				chess_ai_queue[i][C_VALUE] = 0;
		}
	}

	if((from) && (to) && (queuepos > 0))
	{
		best_from = tempqueue[0][C_FROM];
		best_to = tempqueue[0][C_TO];
		best_value = tempqueue[0][C_VALUE];
		for(queuepos = 0; queuepos < maxqueue; queuepos++)
		{
			if(tempqueue[queuepos][C_VALUE] > best_value)
			{
				best_from = tempqueue[queuepos][C_FROM];
				best_to = tempqueue[queuepos][C_TO];
				best_value = tempqueue[queuepos][C_VALUE];
			}
		}
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
			case C_ROOK:
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

