#include <stdlib.h>
#include <stdio.h>

#include "ggzmod.h"
#include "ggzmod-ggz.h"

int main(int argc, char **argv)
{
	GGZMod *ggz = ggzmod_new(GGZMOD_GGZ);
	char *args[] = {"./ggzmodgametest", NULL};


	printf("Checking type.\n");
	if (ggzmod_get_type(ggz) != GGZMOD_GGZ) {
		printf("Wrong type!\n");
		return -1;
	}

	printf("Setting module.\n");
	ggzmod_set_module(ggz, ".", args);

	printf("Connecting.\n");
	if (ggzmod_connect(ggz) < 0) {
		printf("Can't connect!\n");
		return -1;
	}

	printf("Cleaning up.\n");
	ggzmod_free(ggz);

	printf("Test succeeded.\n");
	return 0;
}
