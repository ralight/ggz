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
 *  $Id: board.h 10219 2008-07-08 17:26:29Z jdorje $
 */

#ifndef CGC_BOARD_H
#define CGC_BOARD_H

#include <sys/types.h>
#ifdef HAVE_STDINT_H
#  include <stdint.h>
#endif

#ifndef HAVE_UINT8_T
typedef unsigned char uint8_t;
#define HAVE_UINT8_T
#endif

typedef uint8_t piece_t;

#define NHASH 12
#define MULTIPLIER  31

struct posnode {
	char position[75];
	int count;
	struct posnode *next;
};

/* board.c */
game_t *cgc_setup_table(void);
int setup_board(piece_t ** board);
piece_t **cgc_get_empty_board(void);
int cgc_free_board(piece_t ** board);
int color(piece_t piece);
piece_t cgc_piece_type(piece_t piece);
unsigned int cgc_hash(char *str);
void cgc_free_postab(struct posnode *postab[]);
struct posnode *cgc_add_pos(struct posnode *postab[], char position[]);
int cgc_getboard(char *output, piece_t ** board);

#endif
