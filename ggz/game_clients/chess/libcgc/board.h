#ifndef CGC_BOARD_H
#define CGC_BOARD_H

#include <sys/types.h>

typedef u_int8_t piece_t;

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
