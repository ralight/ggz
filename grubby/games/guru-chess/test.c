#include <stdio.h>
#include <unistd.h>

#include "ai.h"

int main()
{
	int ret;
	int from, to;
	int color;

	chess_ai_init(C_WHITE, 5);

	chess_ai_output();

	ret = chess_ai_move(9, 17, 0);
	printf("Pawn move: %i\n", ret);

	chess_ai_output();

	color = C_BLACK;
	while(1)
	{
		ret = chess_ai_find(color, &from, &to);
		if(!ret) break;
		printf("Move found: %i (%i->%i)\n", ret, from, to);
		ret = chess_ai_move(from, to, 0);
		printf("Move validity: %i\n", ret);
		chess_ai_output();
		sleep(1);

		if(color == C_WHITE) color = C_BLACK;
		else color = C_WHITE;
	}

	return 0;
}

