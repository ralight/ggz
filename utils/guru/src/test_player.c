#include "player.h"
#include <stdio.h>
#include <stdlib.h>

#define EXAMPLE "gis"

int main(int argc, char *argv[])
{
	Player *p, *q;

	printf("Testing the player handling...\n");

	p = guru_player_lookup(EXAMPLE);
	if(!p)
	{
		printf("ERROR: %s not found.\n", EXAMPLE);
		exit(-1);
	}

	printf("Last seen %s: %i\n", EXAMPLE, p->lastseen);
	p->lastseen = 12345;
	guru_player_save(p);
	p->lastseen = 500;

	q = guru_player_lookup(EXAMPLE);
	if(!q)
	{
		printf("ERROR: %s not found.\n", EXAMPLE);
		exit(-1);
	}
	/* expect 12345 as answer; real time is wrong and 500 is wrong too */
	printf("Now last seen %s: %i\n", EXAMPLE, q->lastseen);

	guru_player_free(p);
	guru_player_free(q);

	return 0;
}

