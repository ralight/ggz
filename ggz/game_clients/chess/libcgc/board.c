/*-
 * Copyright (c) 2000 Dan Papasian.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  $Id: board.c 6293 2004-11-07 05:51:47Z jdorje $
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>	/* Site-specific config */
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "cgc.h"

int setup_board(piece_t ** board)
{
	int f, r;

	for (f = 0; f < 8; f++)
		for (r = 2; r < 6; r++)
			board[f][r] = EMPTY;

	for (f = 0; f < 8; f++)
		board[f][1] = W_PAWN;
	for (f = 0; f < 8; f++)
		board[f][6] = B_PAWN;

	board[0][0] = W_ROOK;
	board[1][0] = W_KNIGHT;
	board[2][0] = W_BISHOP;
	board[3][0] = W_QUEEN;
	board[4][0] = W_KING;
	board[5][0] = W_BISHOP;
	board[6][0] = W_KNIGHT;
	board[7][0] = W_ROOK;
	board[0][7] = B_ROOK;
	board[1][7] = B_KNIGHT;
	board[2][7] = B_BISHOP;
	board[3][7] = B_QUEEN;
	board[4][7] = B_KING;
	board[5][7] = B_BISHOP;
	board[6][7] = B_KNIGHT;
	board[7][7] = B_ROOK;

	return 0;

}

piece_t **cgc_get_empty_board(void)
{
	/*
	 * Before you all jump down my throat for magic numbers,
	 * the "8" refers to the amount of rows and columns
	 * on a chess board.  Ok?  Now leave me alone ;)
	 */

	piece_t **board = malloc(8 * sizeof(piece_t *));
	int i;

	board[0] = malloc(8 * 8 * sizeof(piece_t));
	for (i = 1; i < 8; i++)
		board[i] = board[0] + i * 8;

	return board;
}

int cgc_free_board(piece_t ** board)
{
	free(board[0]);
	free(board);

	return 0;
}

int color(piece_t piece)
{
	if (piece == EMPTY)
		return -1;
	return (piece & WHITE);
}

piece_t cgc_piece_type(piece_t piece)
{
	return piece & ~WHITE;
}

unsigned int cgc_hash(char *str)
{
	unsigned int h;
	unsigned char *p;

	h = 0;
	for (p = (unsigned char *)str; *p != '\0'; p++)
		h = MULTIPLIER * h + *p;
	return h % NHASH;

}

void cgc_free_postab(struct posnode *postab[])
{

	int i;
	struct posnode *p, *n;

	for (i = 0; i < NHASH; i++) {
		p = postab[i];
		for (; p != NULL; p = n) {
			n = p->next;
			memset(p, 0, sizeof(struct posnode));	/* no leftovers */
			free(p);
		}
		postab[i] = NULL;	/* reinitialize postab */
	}
	return;

}

struct posnode *cgc_add_pos(struct posnode *postab[], char position[])
{
	/**
	 * Add a new posnode to cgc_hash table and return pointer to it,
	 * if the board position already exists, increment counter.
	 **/
	int h;
	struct posnode *pos;

	h = cgc_hash(position);
	for (pos = postab[h]; pos != NULL; pos = pos->next)
		if (strncmp(position, pos->position, 9 * 8) == 0) {
			pos->count++;
			return pos;
		}

	pos = malloc(sizeof(struct posnode));
	strncpy(pos->position, position, 9 * 8);
	pos->count = 1;
	pos->next = postab[h];
	postab[h] = pos;
	return pos;
}

int cgc_getboard(char *output, piece_t ** board)
{
	char s[10];
	int f, r;

	output[0] = '\0';

	for (r = 7; r != -1; r--) {
		for (f = 0; f < 8; ++f) {
			switch (board[f][r]) {
			case W_PAWN:
				s[f] = 'P';
				break;
			case B_PAWN:
				s[f] = 'p';
				break;
			case W_ROOK:
				s[f] = 'R';
				break;
			case B_ROOK:
				s[f] = 'r';
				break;
			case W_KNIGHT:
				s[f] = 'N';
				break;
			case B_KNIGHT:
				s[f] = 'n';
				break;
			case W_BISHOP:
				s[f] = 'B';
				break;
			case B_BISHOP:
				s[f] = 'b';
				break;
			case W_QUEEN:
				s[f] = 'Q';
				break;
			case B_QUEEN:
				s[f] = 'q';
				break;
			case W_KING:
				s[f] = 'K';
				break;
			case B_KING:
				s[f] = 'k';
				break;
			case EMPTY:
				s[f] = '-';
				break;
			}
		}
		s[8] = ' ';
		s[9] = '\0';
		strcat(output, s);
	}

	return 0;

}
