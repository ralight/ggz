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
