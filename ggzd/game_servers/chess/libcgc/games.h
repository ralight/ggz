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
 *  $Id: games.h 10219 2008-07-08 17:26:29Z jdorje $
 */

/* Board.h must be included before this */

struct _stat {
	int total_games;
	int cur_games;
};

struct game {
	int player1;
	int player2;
	struct UserList *observers;
	struct move *movelist;
	int gid;
	piece_t **board;
	int wkcastle;
	int wqcastle;
	int bkcastle;
	int bqcastle;
	int epossible;
	int onmove;
	int hmcount;
	struct posnode *postab[NHASH];
	piece_t wkr;		/* White king's rank */
	piece_t wkf;		/* White king's file */
	piece_t bkr;		/* Black king's rank */
	piece_t bkf;		/* Black king's file */
	struct game *next;
	struct game *prev;
	char lastmove[5];
};

#ifndef ASSERT
#ifndef STABLE
#define ASSERT(a) assert(a)
#else
#define ASSERT(a) while(0)
#endif
#endif

/* games.c */
int cgc_handle_move(struct game *curgame, char *move);
int cgc_join_table(struct game *joined, int color);
int cgc_game_over(struct game *curgame);
