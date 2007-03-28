#include <stdio.h>

#include "omnicrack.h"

static void check(const char *password)
{
	int ret;

	printf("* Check %s...\n", password);
	ret = omnicrack_checkstrength(password, OMNICRACK_STRATEGY_ALL, NULL);
	printf("  ** Result: %s\n", (ret ? "unsafe" : "safe"));
}

int main(int argc, char *argv[])
{
	check("dull");         // Short English
	check("acquittance");  // Long English
	check("mäh");          // German sheep
	check("95 Klöße");     // German food (yummy), numbered
	check("ðłđ");          // Nordic and Polish
	check("_#§/X");        // ASCII
	check("උකජණ");         // Sinhala
	check("ЉЖЗ 朠朵末粍"); // Cyrillic and Chinese (Han) mixed

	return 0;
}

