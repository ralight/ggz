#ifndef AI_H
#define AI_H

/* Find a suitable move for the given player */
/* Returns the recommended position, or -1 on error */
int ai_findmove(int player, int difficulty, char b[9]);

#endif
