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
 *  $Id: movecheck.c 6293 2004-11-07 05:51:47Z jdorje $
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>	/* Site-specific config */
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "cgc.h"

static int valid_knight(struct game *curgame, int fs, int rs, int fd,
			int rd, int promote);
static int valid_bishop(struct game *curgame, int fs, int rs, int fd,
			int rd, int promote);
static int valid_rook(struct game *curgame, int fs, int rs, int fd, int rd,
		      int promote);
static int valid_queen(struct game *curgame, int fs, int rs, int fd,
		       int rd, int promote);
static int valid_king(struct game *curgame, int fs, int rs, int fd, int rd,
		      int promote);
static int check_castle(struct game *curgame, int fs, int rs, int fd,
			int rd);
static int can_be_occupied(struct game *curgame, int f, int r, int pcol);
static int check_attackers(struct game *curgame, int f, int r, int *af,
			   int *ar, int pcol, int check_occupied);
static int safe_square(struct game *curgame, int f, int r);
static int safe_from_color(struct game *curgame, int f, int r, int col);
static int find_attacker(struct game *curgame, int f, int r, int *af,
			 int *ar);
static int check_knight_attackers(struct game *curgame, int f, int r,
				  int *af, int *ar, int pcol);
static int valid_pawn(struct game *curgame, int fs, int rs, int fd, int rd,
		      int promote);
static int king_has_move(struct game *curgame, int kf, int kr);
static int check_rook_moves(piece_t ** board, int f, int r);
static int check_bishop_moves(piece_t ** board, int f, int r);
static int check_knight_moves(piece_t ** board, int f, int r);
static int can_move(struct game *curgame, int f, int r, int kf, int kr);
static int onboard(int f, int r);
static int bishop_type(int f, int r);

int
cgc_valid_move(struct game *curgame, int fs, int rs, int fd, int rd,
	       int promote)
{
	piece_t actpiece;
	piece_t tmp;
	int retval;
	int kf, kr;

	struct pinfo {
		int name;
		int (*cgc_valid_move) (struct game *, int, int, int, int,
				       int);
	} piece[] = {
		{
		KNIGHT, valid_knight}, {
		BISHOP, valid_bishop}, {
		ROOK, valid_rook}, {
		QUEEN, valid_queen}, {
		KING, valid_king}, {
		PAWN, valid_pawn}, {
		0, NULL}
	};

	struct pinfo *cptr;

	/* Basic sanity checks */

	if (!onboard(fs, rs) || !onboard(fd, rd))
		return E_BADMOVE;

	if (!curgame->player1 || !curgame->player2)
		return E_PLAYERALONE;

	if (color(curgame->board[fs][rs]) != curgame->onmove)
		return E_WRONGCOLOR;

	if (curgame->board[fs][rs] == EMPTY)
		return E_BADMOVE;

	/*
	 * Because of pawn promotion, It's acceptable for say, A7A4
	 * or A2A2. valid_pawn will do its own handling on this.
	 */

	actpiece = cgc_piece_type(curgame->board[fs][rs]);

	if (fd == fs && rd == rs)
		return E_BADMOVE;

	if (color(curgame->board[fd][rd]) == color(curgame->board[fs][rs]))
		return E_BADMOVE;

	for (cptr = piece; cptr->name; cptr++)
		if (cptr->name == actpiece)
			break;

	assert(cptr->name);

	if (!cptr->cgc_valid_move(curgame, fs, rs, fd, rd, promote))
		return E_BADMOVE;


	if (curgame->onmove == WHITE) {
		kf = curgame->wkf;
		kr = curgame->wkr;
	} else {
		kf = curgame->bkf;
		kr = curgame->bkr;
	}

	/* We can't end the move in check. */

	tmp = curgame->board[fd][rd];
	curgame->board[fd][rd] = curgame->board[fs][rs];
	curgame->board[fs][rs] = EMPTY;

	if (cgc_piece_type(curgame->board[fd][rd]) != KING) {
		retval = safe_square(curgame, kf, kr);
	} else {
		retval = safe_square(curgame, fd, rd);
	}

	curgame->board[fs][rs] = curgame->board[fd][rd];
	curgame->board[fd][rd] = tmp;

	if (!retval)
		return E_BADMOVE;

	return OK;
}

int
cgc_do_move(struct game *curgame, int fs, int rs, int fd, int rd,
	    int promote)
{
	piece_t kf, kr;
	int epossible;
	int kcastle, qcastle;

	epossible = -1;	/* If we aren't replaced, then chance is lost for ep */

	if (curgame->onmove == WHITE) {
		kcastle = curgame->wkcastle;
		qcastle = curgame->wqcastle;
		kf = curgame->wkf;
		kr = curgame->wkr;
	} else {
		kcastle = curgame->bkcastle;
		qcastle = curgame->bqcastle;
		kf = curgame->bkf;
		kr = curgame->bkr;
	}

	/* Point of no return- We're going with it! */

	/* Castling */
	if (cgc_piece_type(curgame->board[fs][rs]) == KING
	    && abs(fd - fs) > 1) {
		if (fd == 2) {
			curgame->board[3][rd] = (ROOK | curgame->onmove);
			curgame->board[0][rd] = EMPTY;
		}
		if (fd == 6) {
			curgame->board[5][rd] = (ROOK | curgame->onmove);
			curgame->board[7][rd] = EMPTY;
		}
		kcastle = qcastle = 0;
	}

	if (cgc_piece_type(curgame->board[fs][rs]) == KING) {
		kf = fd;
		kr = rd;
		kcastle = qcastle = 0;
	}

	if (cgc_piece_type(curgame->board[fs][rs]) == ROOK) {
		if (fs == 0)
			qcastle = 0;
		if (fs == 7)
			kcastle = 0;
	}

	if (cgc_piece_type(curgame->board[fs][rs]) == PAWN) {

		/* Promotion */
		if (color(curgame->board[fs][rs]) == WHITE && rs == 6) {
			curgame->board[fs][rs] = (promote | WHITE);
			rd = 7;
		} else if (color(curgame->board[fs][rs]) == BLACK
			   && rs == 1) {
			curgame->board[fs][rs] = (promote | BLACK);
			rd = 0;
		}

		/* Check to see if we're endangered for ep */
		if (abs(rs - rd) > 1)	/* They can do ep! */
			epossible = fd;

		/* Check to see if we're doing ep
		 * This is a bit hackish as we clear the rd above
		 * and below where we are, but one is guaranteed
		 * to be the one we're caputring and the other
		 * really should be (or else uh-oh) empty :)
		 */
		if (curgame->board[fd][rd] == EMPTY && fd != fs) {
			curgame->board[fd][rd - 1] = EMPTY;
			curgame->board[fd][rd + 1] = EMPTY;
		}
	}

	curgame->board[fd][rd] = curgame->board[fs][rs];
	curgame->board[fs][rs] = EMPTY;

	assert(safe_square(curgame, kf, kr));

	curgame->epossible = epossible;

	if (curgame->onmove == WHITE) {
		curgame->wkf = kf;
		curgame->wkr = kr;
		curgame->wkcastle = kcastle;
		curgame->wqcastle = qcastle;
	} else {
		curgame->bkf = kf;
		curgame->bkr = kr;
		curgame->bkcastle = kcastle;
		curgame->wqcastle = qcastle;
	}

	curgame->onmove = !curgame->onmove ? WHITE : BLACK;

	return VALID;
}

static int
valid_knight(struct game *curgame, int fs, int rs, int fd, int rd,
	     int promote)
{
	switch (abs(rs - rd)) {
	case 2:
		if (abs(fs - fd) == 1)
			return VALID;
		break;
	case 1:
		if (abs(fs - fd) == 2)
			return VALID;
		break;
	}

	return INVALID;
}

static int
valid_bishop(struct game *curgame, int fs, int rs, int fd, int rd,
	     int promote)
{
	int cf, cr;
	int diag = 0;

	if (abs(fd - fs) != abs(rd - rs))
		return INVALID;	/* Capture the way off ones */

	if (fs > fd && rs > rd)
		diag = 3;
	if (fs > fd && rs < rd)
		diag = 2;
	if (fs < fd && rs > rd)
		diag = 1;

	cf = fs;
	cr = rs;

	for (;;) {
		switch (diag) {
		case 0:
			++cf, ++cr;
			break;
		case 1:
			++cf, --cr;
			break;
		case 2:
			--cf, ++cr;
			break;
		case 3:
			--cf, --cr;
			break;
		}

		if (!onboard(cf, cr))
			return INVALID;	/* Off the board, fail */

		if (cf == fd && cr == rd)
			return VALID;	/* We're here! pass */

		if (curgame->board[cf][cr] != EMPTY)
			break;	/* Walked into another piece, fail */
	}

	return INVALID;
}

static int
valid_rook(struct game *curgame, int fs, int rs, int fd, int rd,
	   int promote)
{
	int cf, cr;
	int diag = 0;

	if (rs != rd && fs != fd)
		return INVALID;	/* capture the way off ones */

	if (fd < fs)	/* If we're headed to the left */
		diag = 1;

	if (rd < rs)	/* If we're going down */
		diag = 3;

	if (fd > fs)	/* Going right */
		diag = 0;

	if (rd > rs)	/* Going up */
		diag = 2;

	cf = fs;
	cr = rs;

	for (;;) {
		switch (diag) {
		case 0:
			++cf;
			break;
		case 1:
			--cf;
			break;
		case 2:
			++cr;
			break;
		case 3:
			--cr;
			break;
		}
		if (!onboard(cf, cr))
			break;

		if (cf == fd && cr == rd)
			return VALID;
		if (curgame->board[cf][cr] != EMPTY)
			break;
	}

	return INVALID;
}

static int
valid_queen(struct game *curgame, int fs, int rs, int fd, int rd,
	    int promote)
{
	if (valid_rook(curgame, fs, rs, fd, rd, promote))
		return VALID;
	if (valid_bishop(curgame, fs, rs, fd, rd, promote))
		return VALID;
	return INVALID;
}

static int
valid_king(struct game *curgame, int fs, int rs, int fd, int rd,
	   int promote)
{

	int enemy;

	enemy = curgame->onmove ? BLACK : WHITE;

	if (abs(rs - rd) <= 1 && abs(fs - fd) <= 1)
		return VALID;

	if (abs(rs - rd) == 0 && (abs(fs - fd) == 2 || abs(fs - fd) == 3))
		return check_castle(curgame, fs, rs, fd, rd);

	return INVALID;
}

static int
check_castle(struct game *curgame, int fs, int rs, int fd, int rd)
{
	int enemy;

	enemy = curgame->onmove ? BLACK : WHITE;

	if (curgame->onmove == WHITE && rd != 0)
		return INVALID;
	if (curgame->onmove == BLACK && rd != 7)
		return INVALID;
	if (curgame->board[(fd + fs) / 2][rd] != EMPTY)
		return INVALID;
	if (curgame->board[fd][rd] != EMPTY)
		return INVALID;

	if (!safe_from_color(curgame, (fd + fs) / 2, rd, enemy))
		return INVALID;
	if (!safe_from_color(curgame, fs, rs, enemy))
		return INVALID;

	switch (fd) {
	case 2:
		if (curgame->onmove == WHITE && curgame->wqcastle)
			return VALID;
		if (curgame->onmove == BLACK && curgame->bqcastle)
			return VALID;
	case 6:
		if (curgame->onmove == WHITE && curgame->wkcastle)
			return VALID;
		if (curgame->onmove == BLACK && curgame->bkcastle)
			return VALID;
	}

	return INVALID;
}

static int can_be_occupied(struct game *curgame, int f, int r, int pcol)
{
	int af;
	int ar;
	int rval;
	int kf, kr;
	int tmp;

	if (curgame->onmove == WHITE) {
		kf = curgame->wkf;
		kr = curgame->wkr;
	} else {
		kf = curgame->bkf;
		kr = curgame->bkr;
	}

	rval = check_attackers(curgame, f, r, &af, &ar, pcol, 1);

	curgame->board[kf][kr] = (KING | pcol);

	if (rval == VALID) {
		tmp = curgame->board[af][ar];
		curgame->board[f][r] = tmp;
		curgame->board[af][ar] = EMPTY;

		if (safe_square(curgame, kf, kr)) {
			curgame->board[af][ar] = tmp;
			curgame->board[kf][kr] = EMPTY;
			curgame->board[f][r] = EMPTY;
			return rval;
		}
		curgame->board[af][ar] = tmp;
	}

	curgame->board[f][r] = EMPTY;
	curgame->board[kf][kr] = EMPTY;

	return INVALID;
}


/* Can a piece belonging to pcol move to the given square? */
static int
check_attackers(struct game *curgame, int f, int r, int *af, int *ar,
		int pcol, int check_occupied)
{
	int cf, cr;		/* The file and rank we look for the attacker in */
	int diag;		/* Which of 8 diagonals we are operating on */
	int dist;		/* How far are we from origin? */
	piece_t tmp;		/* We could be checking an empty sq */

	dist = diag = 0;

	tmp = curgame->board[f][r];

	if (check_knight_attackers(curgame, f, r, af, ar, pcol))
		return VALID;

	for (;;) {
		cf = f;
		cr = r;

		for (;;) {
			switch (diag) {
			case 0:
				++cf, ++cr;	/* up right */
				break;
			case 1:
				++cf, --cr;	/* down right */
				break;
			case 2:
				--cf, ++cr;	/* up left */
				break;
			case 3:
				--cf, --cr;	/* down left */
				break;
			case 4:
				++cf;	/* right */
				break;
			case 5:
				--cf;	/* left */
				break;
			case 6:
				++cr;	/* up */
				break;
			case 7:
				--cr;	/* down */
				break;
			}

			if (af != NULL && ar != NULL)
				*af = cf, *ar = cr;

			/* If we're off the board, try another diag */
			if (!onboard(cf, cr))
				break;

			++dist;

			/* Our own piece can't hurt us */
			if (curgame->board[cf][cr] != EMPTY)
				if (color(curgame->board[cf][cr]) != pcol)
					break;

			/* Stop for blockers */
			if (curgame->board[cf][cr] == BLOCKER)
				break;

			switch (curgame->board[cf][cr]) {
			case W_QUEEN:
			case B_QUEEN:
				return VALID;
				break;
			case B_PAWN:
				if (!check_occupied)
					if (curgame->board[f][r] == EMPTY)
						curgame->board[f][r] = 60;

				if (valid_pawn(curgame, cf, cr, f, r, 0)) {
					curgame->board[f][r] = tmp;
					return VALID;
				}
				curgame->board[f][r] = tmp;
				break;
			case W_PAWN:
				if (!check_occupied)
					if (curgame->board[f][r] == EMPTY)
						curgame->board[f][r] = 50;

				if (valid_pawn(curgame, cf, cr, f, r, 0)) {
					curgame->board[f][r] = tmp;
					return VALID;
				}
				curgame->board[f][r] = tmp;
				break;
			case W_KING:
			case B_KING:
				if (dist == 1)
					return VALID;
				break;
			case W_BISHOP:
			case B_BISHOP:
				if (diag <= 3)
					return VALID;
				break;
			case W_ROOK:
			case B_ROOK:
				if (diag >= 4)
					return VALID;
				break;
			}

			if (curgame->board[cf][cr] != EMPTY)
				break;
		}

		if (++diag > 7)
			break;

		dist = 0;	/* here we go again */
	}

	return INVALID;
}


/* is square safe from enemy?  non-zero if true, zero if false */
static int safe_square(struct game *curgame, int f, int r)
{
	int enemy;

	enemy = (color(curgame->board[f][r]) ? BLACK : WHITE);

	return safe_from_color(curgame, f, r, enemy);
}

/* Is the square threatened by col?  No new functionality here,
 * but it is easier to parse than check_attackers itself.
 */

static int safe_from_color(struct game *curgame, int f, int r, int col)
{
	if (check_attackers(curgame, f, r, NULL, NULL, col, 0))
		return INVALID;

	return VALID;
}

/* same as safe_square but don't discard af/ar */
static int
find_attacker(struct game *curgame, int f, int r, int *af, int *ar)
{
	int enemy;

	enemy = (color(curgame->board[f][r]) ? BLACK : WHITE);

	if (check_attackers(curgame, f, r, af, ar, enemy, 0))
		return VALID;	/* Found the attacker */

	return INVALID;	/* No attacker, it's safe */
}

static int
check_knight_attackers(struct game *curgame, int f, int r, int *af,
		       int *ar, int pcol)
{
	int cf;
	int cr;
	piece_t enemy;
	int line;

	enemy = KNIGHT | pcol;

	for (line = 0; line < 8; ++line) {
		cf = f;
		cr = r;
		switch (line) {
		case 0:
			cf += 2;
			cr += 1;
			break;
		case 1:
			cf += 2;
			cr -= 1;
			break;
		case 2:
			cf -= 2;
			cr += 1;
			break;
		case 3:
			cf -= 2;
			cr -= 1;
			break;
		case 4:
			cf += 1;
			cr += 2;
			break;
		case 5:
			cf += 1;
			cr -= 2;
			break;
		case 6:
			cf -= 1;
			cr += 2;
			break;
		case 7:
			cf -= 1;
			cr -= 2;
			break;
		}

		if (!onboard(cf, cr))
			continue;

		if (curgame->board[cf][cr] == enemy) {
			if (af != NULL && ar != NULL) {
				*af = cf;
				*ar = cr;
			}
			return VALID;
		}
	}

	return INVALID;
}

static int
valid_pawn(struct game *curgame, int fs, int rs, int fd, int rd,
	   int promote)
{
	switch (curgame->board[fs][rs]) {
	case W_PAWN:
		/* Basic sanity checks */

		/* We must be going forward */
		if (rd <= rs)
			return INVALID;

		/* Can never move more than 1 unit diagonally */
		if (abs(fd - fs) > 1)
			return INVALID;

		/* Cannot walk into a piece */
		if (curgame->board[fd][rd] != EMPTY && fd == fs)
			return INVALID;


		/* Check for non-capture pawn pushes */
		if (fd == fs) {

			/* Look for an inital 2 space advancement */
			if (rs == 1 && curgame->board[fd][rs + 1] == EMPTY)
				if (abs(rd - rs) == 2)
					return VALID;

			/* Check for a simple pawn push */
			if (abs(rd - rs) == 1)
				return VALID;

			return INVALID;
		}


		if (abs(fd - fs) != 1)
			return INVALID;

		/* Check our diagonal moves, ep and attack */

		/* No diagonal move traverses more than one rank */
		if (abs(rd - rs) != 1)
			return INVALID;

		if (rd == 5 && fd == curgame->epossible)
			return VALID;	/*ep */
		if (curgame->board[fd][rd] != EMPTY)
			return VALID;	/* attack */

		break;
	case B_PAWN:
		/* Basic sanity checks */

		/* Must be going forward */
		if (rd >= rs)
			return INVALID;

		/* Can never move more than one diag */
		if (abs(fd - fs) > 1)
			return INVALID;

		/* Can't walk forward into a piece */
		if (curgame->board[fd][rd] != EMPTY && fd == fs)
			return INVALID;


		/* Check for non-capture pawn pushes */
		if (fd == fs) {

			/* Check for inital 2 space pawn movement */
			if (rs == 6 && curgame->board[fd][rs - 1] == EMPTY)
				if (abs(rd - rs) == 2)
					return VALID;

			/* Check for a simple pawn push */
			if (abs(rd - rs) == 1)
				return VALID;

			return INVALID;
		}


		/* Check our diagonal moves, ep and attack */

		/* No diagonal move goes moves more than one rank */
		if (abs(rd - rs) != 1)
			return INVALID;

		if (rd == 2 && fd == curgame->epossible)
			return VALID;	/*ep */
		if (curgame->board[fd][rd] != EMPTY)
			return VALID;	/* attack */

		break;
	}

	return INVALID;


}

int
cgc_register_move(struct game *curgame, int fs, int rs, int fd, int rd,
		  int promote)
{
	struct move *latest;
	struct move *walker;
	int irrev = 0;		/* is the move irreversable? */

	latest = malloc(sizeof(struct move));

	latest->fs = fs;
	latest->fd = fd;
	latest->rs = rs;
	latest->rd = rd;
	latest->promote = promote;

	/* All captures are irreversable */
	if (curgame->board[fd][rd] != EMPTY)
		irrev = 1;

	/* All pawn movements are irreversable */
	if (cgc_piece_type(curgame->board[fs][rs]) == PAWN)
		irrev = 1;

	latest->captured = cgc_piece_type(curgame->board[fd][rd]);

	if (curgame->movelist == NULL) {	/* If first move */
		latest->next = latest->prev = NULL;
		curgame->movelist = latest;
	} else {
		for (walker = curgame->movelist; walker->next;
		     walker = walker->next);

		/* walker is now the last move */

		latest->prev = walker;
		latest->next = NULL;
		walker->next = latest;

	}

	if (irrev) {
		cgc_free_postab(curgame->postab);
		curgame->hmcount = 0;
	}

	return 0;
}

int cgc_check_state(struct game *curgame, int kf, int kr)
{

	int cf, cr;
	int af, ar;		/* Attacker's file and rank */
	int diag = -1;
	int status = SAFE;
	piece_t **board;
	piece_t tmp;

	board = curgame->board;

	if (find_attacker(curgame, kf, kr, &af, &ar)) {	/* Check */
		status = CHECK;
	} else {
		return SAFE;
	}

	if (king_has_move(curgame, kf, kr))
		return CHECK;

	/*
	 * Now we see if we have only one attacker.  If
	 * we have two, then we are in checkmate because
	 * our king has no legal moves and there's no possible
	 * move that could save us from both attackers.
	 */

	/* There's no real reason we put a
	 * friendly bishop in place of the attacker,
	 * but we can't make it empty due to pieces
	 * behind the attacker that can be threatening
	 */

	tmp = board[af][ar];
	board[af][ar] = BISHOP | color(board[kf][kr]);

	if (!safe_square(curgame, kf, kr)) {	/* Still not safe, two attacker */
		board[af][ar] = tmp;
		return MATE;
	}

	board[af][ar] = tmp;

	/*
	 * We now see if we can capture the attacker.  If
	 * we can, we see if that removes the king from 
	 * check.  That is done because there may be
	 * two attackers, in which case there is mate.
	 * If there's only one attacker, and we can't
	 * capture it, we have to continue on to see
	 * if there can be an interposition.
	 */

	tmp = board[kf][kr];
	board[kf][kr] = BLOCKER;

	if (!safe_square(curgame, af, ar)) {	/* we can capture */
		board[kf][kr] = tmp;
		return CHECK;
	}

	board[kf][kr] = tmp;


	/*
	 * At this point, we've got an attacker who we can't
	 * capture.  To make matters worse, the king is trapped.
	 * Now the only possible way to escape is by
	 * interposition.. but first a quick check to make
	 * sure it's not a knight, because if it is, we're dead.
	 */

	if (cgc_piece_type(board[af][ar]) == KNIGHT)
		return MATE;

	if (kf > af && kr > ar)
		diag = 0;
	if (kf > af && kr < ar)
		diag = 1;
	if (kf < af && kr > ar)
		diag = 2;
	if (kf < af && kr < ar)
		diag = 3;
	if (kf > af && kr == ar)
		diag = 4;
	if (kf < af && kr == ar)
		diag = 5;
	if (kf == af && kr < ar)
		diag = 6;
	if (kf == af && kr > ar)
		diag = 7;

	ASSERT(diag != -1);


	cf = kf;
	cr = kr;

	/*
	 * The king obviously may not interpose, hence
	 * we remove him from the board and will put 
	 * him back on later.
	 */
	tmp = board[kf][kr];
	board[kf][kr] = EMPTY;

	for (;;) {
		switch (diag) {
		case 0:
			--cf, --cr;	/* down left */
			break;
		case 1:
			--cf, ++cr;	/* up left */
			break;
		case 2:
			++cf, --cr;	/* down right */
			break;
		case 3:
			++cf, ++cr;	/* up right */
			break;
		case 4:
			--cf;	/* left */
			break;
		case 5:
			++cf;	/* right */
			break;
		case 6:
			++cr;	/* up */
			break;
		case 7:
			--cr;	/* down */
			break;
		}

		if (cf == af && cr == ar) {
			status = MATE;
			break;
		}

		if (can_be_occupied(curgame, cf, cr, color(tmp))) {
			status = CHECK;
			break;
		}
	}

	board[kf][kr] = tmp;
	return status;
}

static int king_has_move(struct game *curgame, int kf, int kr)
{
	int cf, cr;
	int enemy;
	int direction = 0;
	piece_t tmp;

	/* This prevents us from looking down a direction, then
	 * when going back to look thinking we're keeping
	 * ourselves out of check!  (Take my word on it)
	 */

	tmp = curgame->board[kf][kr];
	curgame->board[kf][kr] = EMPTY;

	enemy = color(tmp) ? BLACK : WHITE;

	for (direction = 0; direction <= 7; ++direction) {
		cf = kf;
		cr = kr;

		switch (direction) {
		case 0:
			++cf;
			break;
		case 1:
			++cr;
			break;
		case 2:
			++cf;
			++cr;
			break;
		case 3:
			--cf;
			break;
		case 4:
			--cr;
			break;
		case 5:
			--cr;
			--cf;
			break;
		case 6:
			--cf;
			++cr;
			break;
		case 7:
			++cf;
			--cr;
			break;
		}

		if (!onboard(cf, cr))
			continue;

		/* We can't have a valid move into friendly piece */
		if (color(curgame->board[cf][cr]) == color(tmp))
			continue;

		if (safe_from_color(curgame, cf, cr, enemy)) {
			curgame->board[kf][kr] = tmp;
			return VALID;
		}
	}

	curgame->board[kf][kr] = tmp;
	return INVALID;

}

int cgc_is_stale(struct game *curgame, int kf, int kr)
{
	int cf, cr;
	piece_t **board;

	board = curgame->board;
	cf = cr = 0;

	if (king_has_move(curgame, kf, kr))
		return INVALID;

	for (cf = 0; cf < 8; ++cf) {
		for (cr = 0; cr < 8; ++cr) {

			/* filter out pieces not on the board */
			if (!onboard(cf, cr))
				continue;

			/*
			 * we've checked this
			 * in king_has_move
			 */
			if (cgc_piece_type(board[cf][cr]) == KING)
				continue;

			/* Don't check empties */
			if (board[cf][cr] == EMPTY)
				continue;

			/* Filter out the enemy's pieces */
			if (color(board[cf][cr]) != color(board[kf][kr]))
				continue;

			/* Filter out pinned pieces */
			if (!can_move(curgame, cf, cr, kf, kr))
				continue;

			switch (cgc_piece_type(board[cf][cr])) {
			case BISHOP:
				if (check_bishop_moves(board, cf, cr))
					return INVALID;
				break;
			case KNIGHT:
				if (check_knight_moves(board, cf, cr))
					return INVALID;
				break;
			case PAWN:
				if (color(board[cf][cr]) == WHITE) {
					if (board[cf][cr + 1] == EMPTY)
						return INVALID;
				} else if (board[cf][cr - 1] == EMPTY)
					return INVALID;
				break;
			case ROOK:
				if (check_rook_moves(board, cf, cr))
					break;
				break;
			case QUEEN:
				if (check_rook_moves(board, cf, cr))
					return INVALID;
				if (check_bishop_moves(board, cf, cr))
					return INVALID;
				break;
			}
		}
	}

	return INVALID;
}

static int check_rook_moves(piece_t ** board, int f, int r)
{
	int diag;
	int cf, cr;


	for (diag = 0; diag < 4; ++diag) {
		cf = f;
		cr = r;

		switch (diag) {
		case 0:
			++cf;
			break;
		case 1:
			--cf;
			break;
		case 2:
			++cr;
			break;
		case 3:
			--cr;
			break;
		}

		if (!onboard(cf, cr))
			break;

		if (color(board[cf][cr]) == color(board[f][r]))
			break;

		return VALID;

	}

	return INVALID;
}


static int check_bishop_moves(piece_t ** board, int f, int r)
{
	int diag;
	int cf, cr;


	for (diag = 0; diag < 4; ++diag) {
		cf = f;
		cr = r;

		switch (diag) {
		case 0:
			++cf;
			--cr;
			break;
		case 1:
			--cf;
			++cr;
			break;
		case 2:
			++cf;
			++cr;
			break;
		case 3:
			--cf;
			--cr;
			break;
		}

		if (color(board[cf][cr]) == color(board[f][r]))
			break;

		return VALID;

	}

	return INVALID;
}

int cgc_has_sufficient(game_t * curgame, int ocolor)
{
	int f, r;
	int o_knight = 0;
	int e_dbishop, e_lbishop;
	int o_dbishop, o_lbishop;

	o_dbishop = o_lbishop = e_dbishop = e_lbishop = 0;

	for (f = 0; f < 8; ++f) {
		for (r = 0; r < 8; ++r) {
			if (curgame->board[f][r] == EMPTY)
				continue;
			if (color(curgame->board[f][r]) != ocolor &&
			    cgc_piece_type(curgame->board[f][r]) != BISHOP)
				continue;

			switch (cgc_piece_type(curgame->board[f][r])) {
			case PAWN:
			case QUEEN:
			case ROOK:
				return 1;
				break;
			case BISHOP:
				if (color(curgame->board[f][r]) != ocolor) {
					if (bishop_type(f, r) == 1)
						++e_dbishop;
					else
						++e_lbishop;
				} else {
					if (bishop_type(f, r) == 1)
						++o_dbishop;
					else
						++o_lbishop;
				}
				break;
			case KNIGHT:
				++o_knight;
				if (o_knight + o_lbishop + o_dbishop >= 2)
					return 1;
				break;
			}
		}
	}

	o_lbishop -= e_lbishop;
	o_dbishop -= e_dbishop;

	if (o_lbishop == 1 || o_dbishop == 1)
		return 1;

	return 0;
}

static int check_knight_moves(piece_t ** board, int f, int r)
{
	int diag;
	int cf, cr;


	for (diag = 0; diag < 8; ++diag) {
		cf = f;
		cr = r;

		switch (diag) {
		case 0:
			cf += 2;
			cr += 1;
			break;
		case 1:
			cf -= 2;
			cr += 1;
			break;
		case 2:
			cf += 2;
			cr -= 1;
			break;
		case 3:
			cf -= 2;
			cr -= 1;
			break;
		case 4:
			cf += 1;
			cr += 2;
			break;
		case 5:
			cf -= 1;
			cr += 2;
			break;
		case 6:
			cf += 1;
			cr -= 2;
			break;
		case 7:
			cf -= 1;
			cr -= 2;
			break;
		}

		if (color(board[cf][cr]) == color(board[f][r]))
			break;

		return VALID;

	}

	return INVALID;
}

/* Don't call can_move unless you're really sure what
 * you are doing.  If the king is in check to begin
 * with, you wont' get the result you want.  You
 * also may not get the desired result of the color
 * of the king passed and the piece passed aren't the same.
 */

static int can_move(struct game *curgame, int f, int r, int kf, int kr)
{
	piece_t tmp;

	tmp = curgame->board[f][r];
	curgame->board[f][r] = EMPTY;

	if (safe_square(curgame, kf, kr)) {
		curgame->board[f][r] = tmp;
		return VALID;
	}

	curgame->board[f][r] = tmp;

	return INVALID;	/* Yeah, we're pinned */

}

static int onboard(int f, int r)
{
	if (f > 7 || f < 0 || r > 7 || r < 0)
		return INVALID;
	else
		return VALID;
	/* NOTREACHED */
}

/* returns 1 for dark bishop, 0 for light */

static int bishop_type(int f, int r)
{
	if ((f % 2) == 0) {
		if ((r % 2) == 0)
			return 1;
		else
			return 0;
	}
	if ((f % 2) == 1) {
		if ((r % 2) == 0)
			return 0;
		else
			return 1;
	}

	return -1;

}
