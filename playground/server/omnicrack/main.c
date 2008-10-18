#include <stdio.h>

#include "omnicrack.h"

static void check(const char *password, const char *dictpath)
{
	int ret;

	printf("* Check %s...\n", password);
	ret = omnicrack_checkstrength(password, OMNICRACK_STRATEGY_ALL, dictpath);
	printf("  ** Result: %s\n", (ret ? "unsafe" : "safe"));
}

int main(int argc, char *argv[])
{
	if((argc < 2) || (argc > 3))
	{
		fprintf(stderr, "Error, check syntax:\n");
		fprintf(stderr, "- omnicrack <word> [<hashfile>]\n");
		return -1;
	}

	if(argc == 3)
		check(argv[1], argv[2]);
	else
		check(argv[1], NULL);

	return 0;
}

