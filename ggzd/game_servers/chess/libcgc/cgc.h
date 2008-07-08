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
 *  $Id: cgc.h 10219 2008-07-08 17:26:29Z jdorje $
 */

#ifndef CGC_CGC_H
#define CGC_CGC_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct game game_t;
typedef struct move move_t;

#include "board.h"
#include "games.h"
#include "movecheck.h"

/* Everything is OK! */
#define OK 1

/*
 * The following are errors that a function may return
 */

/* Requested move is invalid  */
#define E_BADMOVE -1

/* Attempt to play even though both parties aren't in the game */
#define E_PLAYERALONE -2

/* Attempt to move a piece that does not belong to the player who's
 * move it is */
#define E_WRONGCOLOR -3

/* Syntax error */
#define E_SYNTAX -4

/* Referrenced a game that does not exist */
#define E_NOGAME -5

/* Attempted to join a game that already has two players */
#define E_GAMEFULL -6

/* Attempted to take either black or white when it was taken */
#define E_SEATFULL -7

/*
 * The following are ways a function may return indicating something
 * special about the game.
 */

#define DRAW_STALE 2
#define DRAW_MOVECOUNT 3
#define DRAW_POSREP 4
#define DRAW_INSUFFICIENT 5
#define CHECK 6
#define MATE 7

game_t *cgc_create_game(void);
int cgc_join_game(game_t * game, int color);
int cgc_make_move(game_t * game, char *move);
int cgc_has_sufficient(game_t * game, int color);
void cgc_free_game(game_t * game);

#ifdef __cplusplus
}
#endif
#endif
