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
	check("dull", NULL);         // Short English
	check("acquittance", NULL);  // Long English
	check("mäh", NULL);          // German sheep
	check("95 Klöße", NULL);     // German food (yummy), numbered
	check("ðłđ", NULL);          // Nordic and Polish
	check("_#§/X", NULL);        // ASCII
	check("උකජණ", NULL);         // Sinhala
	check("ЉЖЗ 朠朵末粍", NULL); // Cyrillic and Chinese (Han) mixed

	return 0;
}

