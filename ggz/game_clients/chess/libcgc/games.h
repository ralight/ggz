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
