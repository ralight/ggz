#include <crack.h>
#include <stdio.h>

/*
 * Password strength checker
 *
 * cracklib2 has several issues:
 * - not unicode-safe
 *   - some tools at least
 *   - does casefolding at times
 * - literal results instead of symbolic ones
 * - no fine tuning of checks possible
 *   (e.g. minimum/maximum length)
 * - assumes system login (looks up /etc/passwd)
 *
 * We need to find something better!
 *
 * Using cat words.txt | crack_packer seems to be unicode-safe but leads
 * to corrupt index files. Using update-cracklib only handles ASCII.
 *
 * The function name FascistCheck is also less than optimal.
 */

static void check(const char *s)
{
	const char *res;

	res = FascistCheck(s, CRACKLIB_DICTPATH);

	printf("String '%s': %s\n", s, res);
}

int main(int argc, char *argv[])
{
	check("");
	check("grubby");
	check("josef|nada");

	check("!#_3");
	check("!#_34D");
	check("!#_34DZG9³§");
	check("!#_34DZG9³§\n§$DF_");

	check("house");
	check("haus");
	check("casa");

	check("householder");
	check("haushälterin");
	check("dona de casa");

	return 0;
}

