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
 *  $Id: games.c 981 2001-01-05 18:27:38Z perdig $
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "cgc.h"

struct game *gamelist = NULL;

static void remove_game(struct game *delgame);

struct _stat stats;

int
cgc_handle_move(struct game *curgame, char *move)
{
	struct posnode *posn = NULL;
	int fs, fd;
	int rs, rd;
	int status = 0;
	int retval;
	piece_t ekf, ekr; /* Enemy king file and rank */
	char pos[75];	

	if(curgame == NULL)
		return E_NOGAME;

	fs = move[0] - 'A';
	rs = move[1] - '0' - 1;
	fd = move[2] - 'A';
	rd = move[3] - '0' - 1;

	status = OK;

	if(curgame->onmove == WHITE)
		ekr = curgame->bkr, ekf = curgame->bkf;
	else
		ekr = curgame->wkr, ekf = curgame->wkf;

	/*
	 * The flow here first sends it to cgc_valid_move, which validates
	 * the move. If cgc_valid_move works, then it'll go and cgc_do_move()
	 * which will perform the actual move operation.
	 */

	if((retval = cgc_valid_move(curgame, fs, rs, fd, rd)) != VALID)
		return retval;

		
	/* Make the move */

	cgc_register_move(curgame, fs, rs, fd, rd);
	strcpy(curgame->lastmove, move);
	cgc_do_move(curgame, fs, rs, fd, rd);
	++curgame->hmcount;

	switch(cgc_check_state(curgame, ekf, ekr)) {
	case MATE:
		status = MATE;
		break;
	case CHECK:
		status = CHECK;
		break;
	default:
		if(cgc_is_stable(curgame, ekf, ekr))
			status = DRAW_STALE;
		break;
	}
	
	if(curgame->hmcount == 100) {
		status = DRAW_MOVECOUNT;
	}

	cgc_getboard(pos, curgame->board);

	posn = cgc_add_pos(curgame->postab, pos);

	if(posn->count >= 3)
		status = DRAW_POSREP;

	switch(status) {
	case CHECK:
	case OK:
		return status;
		break;
	default:
		return status;
		break;
	}

	/* NOTREACHED */
}

int
cgc_join_table(struct game *joined, int player, int color)
{
	if(joined == NULL)
		return E_NOGAME;
	
	switch(color) {
	case BLACK:
		if(joined->player2 == EMPTY)
			joined->player2 = player;
		else if(joined->player1 != EMPTY)
			return E_SEATFULL;
		else
			return E_GAMEFULL;
		break;
	case WHITE:
		if(joined->player1 == EMPTY)
			joined->player1 = player;
		else if(joined->player2 != EMPTY)
			return E_SEATFULL;
		else
			return E_GAMEFULL;
		break;
	}

	stats.cur_games++;
	stats.total_games++;
	return OK;
}

static struct game *
new_gameslot(void)
{
	struct game *newgame;
	int i;

	newgame = malloc(sizeof(struct game));

	memset(newgame, 0, sizeof(struct game));

	newgame->board = cgc_get_empty_board();

	for( i=0; i < NHASH; i++)
		newgame->postab[i] = NULL;

	if(gamelist == NULL) {
		gamelist = newgame;
		gamelist->next = gamelist->prev = NULL;
		gamelist->gid = 1;
	} else {
		newgame->next = gamelist;
		newgame->prev = NULL;
		gamelist = newgame;
		gamelist->gid = gamelist->next->gid + 1; 
	}
	
	return newgame;
}

static void
remove_game(struct game *delgame)
{
	struct move *walker;

	if(delgame->prev) {  /* If we aren't the first */
		if(delgame->next) { /* And we aren't the last */
			delgame->prev->next = delgame->next;
			delgame->next->prev = delgame->prev;
		} else { /* If we aren't the first but are the last */
			delgame->prev->next = NULL;
		}
	} else { /* If we are the first */
		if(delgame->next) { /* But aren't the last */
			delgame->next->prev = NULL;
			gamelist = delgame->next;
		} else { /* We're the first and the last */
			gamelist = NULL;
		}
	}

	if(delgame->movelist && delgame->movelist->next) {
		walker = delgame->movelist->next;
		while(walker) {
			free(walker->prev);
			walker->prev = NULL;
			walker = walker->next;
		}
	} else if(delgame->movelist)
		free(delgame->movelist);

	cgc_free_postab(delgame->postab);
	cgc_free_board(delgame->board);
	free(delgame);
}

game_t *
cgc_setup_table(void)
{
	struct game *slot;

	slot = new_gameslot();

	slot->player1 = EMPTY;
	slot->player2 = EMPTY;

	slot->movelist = NULL;
	slot->observers = NULL;

	setup_board(slot->board);

	slot->onmove = WHITE;

	slot->wkr = 0;
	slot->wkf = 4;
	slot->bkr = 7;
	slot->bkf = 4;

	slot->wkcastle = slot->wqcastle = 1;
	slot->bkcastle = slot->bqcastle = 1;

	strcpy(slot->lastmove,"none");

	return slot;
}

int
cgc_game_over(struct game *curgame)
{
        
        remove_game(curgame);
        stats.cur_games--;

        return 0;
}


