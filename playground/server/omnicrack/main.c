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
#ifdef DEBUG
	check("dull", NULL);         // Short English
	check("acquittance", NULL);  // Long English
	check("mäh", NULL);          // German sheep
	check("95 Klöße", NULL);     // German food (yummy), numbered
	check("ðłđ", NULL);          // Nordic and Polish
	check("_#§/X", NULL);        // ASCII
	check("උකජණ", NULL);         // Sinhala
	check("ЉЖЗ 朠朵末粍", NULL); // Cyrillic and Chinese (Han) mixed
#endif

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

