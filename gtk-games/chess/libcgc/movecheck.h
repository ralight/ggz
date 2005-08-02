/*-
 * Copyright (c) 2000, 2001 Dan Papasian.  All rights reserved.
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
 *  $Id: movecheck.h 6293 2004-11-07 05:51:47Z jdorje $
 */

#ifndef _CGC_MOVECHECK_H
#define _CGC_MOVECHECK_H

#include <sys/types.h>

#define INVALID 0
#define VALID 1

#define SAFE 3

enum { BLACK = 0x00, WHITE = 0x40 };
enum { EMPTY = 0x01, PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING, BLOCKER };
enum { W_PAWN =
	    (PAWN | WHITE), W_KNIGHT, W_BISHOP, W_ROOK, W_QUEEN, W_KING };
enum { B_PAWN =
	    (PAWN | BLACK), B_KNIGHT, B_BISHOP, B_ROOK, B_QUEEN, B_KING };
enum { STANDARD, Q_CASTLE, K_CASTLE, ENPASSANT };

struct move {
	int fs;
	int fd;
	int rs;
	int rd;
	int promote;
	struct game *curgame;
	piece_t captured;
	struct move *next;
	struct move *prev;
};

/* movecheck.c */
int cgc_valid_move(struct game *curgame, int fs, int rs, int fd, int rd,
		   int promote);
int cgc_do_move(struct game *curgame, int fs, int rs, int fd, int rd,
		int promote);
int cgc_register_move(struct game *curgame, int fs, int rs, int fd, int rd,
		      int promote);
int cgc_check_state(struct game *curgame, int kf, int kr);
int cgc_is_stale(struct game *curgame, int kf, int kr);

#endif
