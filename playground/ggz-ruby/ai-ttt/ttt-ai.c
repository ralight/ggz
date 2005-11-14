/* AI for Tic-Tac-Toe based on the TTT server 'ggzd.tictactoe' */

/*static int ai_findmove(int num);*/
static void game_rotate_board(char b[9]);

/* Do bot moves */
int ai_findmove(int player, int difficulty, char b[9])
{
	int i, move = -1;
	int me = player;
	int him = 1 - me;
	char board[9];
	int c;

	/* Local copy of the boaard to rotate*/
	for (i = 0; i < 9; i++)
		board[i] = b[i];

	/* Checking for win */
	/* Four possible board rotations to check */
	for (i = 3; i >= 0; i--) {
		
		game_rotate_board(board);
		
		/* If three squares of interest filled, try again */
		if (board[0] != -1 && board[1] != -1 && board[3] != -1)
			continue;
		
		/* 5 patterns to check for win */
		if (board[0] == -1 && board[1] == me && board[2] == me) {
			move = 0;
			break;
		}
			
		if (board[0] == -1 && board[3] == me && board[6] == me) {
			move = 0;
			break;
		}

		if (board[0] == -1 && board[4] == me && board[8] == me) {
			move = 0;
			break;
		}

		if (board[0] == me && board[1] == -1 && board[2] == me) {
			move = 1;
			break;
		}

		if (board[1] == -1 && board[4] == me && board[7] == me) {
			move = 1;
			break;
		}
	}
	
	/* We found a move.  Now "unrotate" it */
	if (move != -1) {
		while (i-- > 0)
			move = 2 + 3 * (move % 3) - move / 3;
		return move;
	}

	/* Checking for immediate block */
	for (i = 3; i >= 0; i--) {

		game_rotate_board(board);

		/* If three squares of interest filled, try again */
		if (board[0] != -1 && board[1] != -1 && board[3] != -1)
			continue;
		
		/* 5 patterns to check for immediate block */
		if (board[0] == -1 && board[1] == him && board[2] == him) {
			move = 0;
			break;
		}

		if (board[0] == -1 && board[3] == him && board[6] == him) {
			move = 0;
			break;
		}

		if (board[0] == -1 && board[4] == him && board[8] == him) {
			move = 0;
			break;
		}

		if (board[0] == him && board[1] == -1 && board[2] == him) {
			move = 1;
			break;
		}
		
		if (board[1] == -1 && board[4] == him && board[7] == him) {
			move = 1;
			break;
		}
	}

	/* FIXME: If not playing perfect, move here */
	/* We found a move.  Now "unrotate" it */
	if (move != -1) {
		while (i-- > 0)
			move = 2 + 3 * (move % 3) - move / 3;
		return move;
	}

	/* Avoid 'holy cow' weakness */
	if (difficulty == 1) {
		c = board[4];
		if (c != -1)
		{
			for (i = 0; i < 9; i += 2) {
				if (i == 4) continue;
				if ((board[i] == c)
				&& (board[8 - i] != c)
				&& (board[8 - i] != -1)) {
					/*printf("holy cow! at: %i\n", i);*/
					if (board[abs(i - 6)] == -1) {
						return abs(i - 6);
					} else if (board[abs(i - 2)] == -1) {
						return abs(i - 2);
					}
				}
			}
		}
	}

	/* Checking for future block */
	for (i = 3; i >= 0; i--) {

		game_rotate_board(board);

		/* If three squares of interest filled, try again */
		if (board[0] != -1 && board[1] != -1 && board[3] != -1)
			continue;
		
		/* 6 patterns to check for future block */
		if (board[0] == -1 && board[1] == -1 && board[2] == him
		    && board[3] == -1 && board[4] == me && board[5] == -1
		    && board[6] == him && board[7] == -1 && board[8] == -1) {
			move = 1;
			break;
		}

		if (board[0] == -1 && board[1] == him && board[2] == -1
		    && board[3] == him && board[4] == me && board[6] == -1) {
			move = 0;
			break;
		}

		if (board[0] == -1 && board[1] == -1 && board[2] == him
		    && board[3] == him && board[4] == me && board[6] == -1) {
			move = 0;
			break;
		}

		if (board[0] == -1 && board[1] == -1 && board[2] == him
		    && board[3] == -1 && board[4] == me && board[6] == him) {
			move = 0;
			break;
		}

		if (board[0] == -1 && board[4] == him && board[8] == -1) {
			move = 0;
			break;
		}

		if (board[1] == -1 && board[4] == him && board[7] == -1) {
			move = 1;
			break;
		}
	}
	
	/* We found a move.  Now "unrotate" it */
	if (move != -1) {
		while (i-- > 0)
			move = 2 + 3 * (move % 3) - move / 3;
	} else {
		/* If we didn't match a pattern, just pick something*/
		if (board[4] == -1) {
			move = 4;
		}
		else
			for (i = 0; i < 9; i++)
				if (board[i] == -1) {
					move = i;
					break;
				}
	}

	return move;
}

static void game_rotate_board(char b[9])
{
	int i, j;
	char tmp[9];
	
	for (i = 0; i < 9; i++)
		tmp[i] = b[i];
	for (i = 0; i < 3; i++)
		for (j = 0; j < 3; j++)
			b[3*i+j] = tmp[3*(2-j)+i];
}

