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
		printf("ERROR: %s not found -> create!\n", EXAMPLE);
		p = guru_player_new();
		p->name = EXAMPLE;
		p->firstseen = 88888;
		p->lastseen = 99999;
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
	/* Expect 12345 as answer; real time is wrong and 500 is wrong too for the
	 * first time. However, later calls should give 500 since it is not
	 * duplicated by default. */
	printf("Now last seen %s: %i\n", EXAMPLE, q->lastseen);

	guru_player_free(p);
	guru_player_free(q);

	return 0;
}

