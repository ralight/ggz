/*******************************************************************
*
* Guru - functional example of a next-generation grubby
* Copyright (C) 2001 Josef Spillner, <dr_maux@users.sourceforge.net>
* Published under GNU GPL conditions - see 'COPYING' for details
*
********************************************************************/

#include "guru.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	int res;
	char *answer;
	char buffer[100];

	printf("Guru: initializing...\n");
	res = guru_init();
	if(!res)
	{
		printf("Guru initialization failed!\n");
		exit(-1);
	}
	printf("Guru: ready.\n");

	while(fgets(buffer, sizeof(buffer), stdin))
	{
		buffer[strlen(buffer) - 1] = 0;
		answer = guru_work(buffer);
		if(answer) printf("Guru answered: %s\n", answer);
	}

	guru_close();

	return 0;
}


