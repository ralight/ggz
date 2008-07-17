#include <stdio.h>

#include "ggzmod.h"

int main(void)
{
	GGZMod *ggz = ggzmod_new(GGZMOD_GAME);

	if (ggzmod_connect(ggz) < 0) {
		printf("Game: can't connect\n");
		return -1;
	}
	return 0;
}
