#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include "ggz.h"

void test_strcmp(char *s1, char *s2, int result);
int signum(int x);

int main(void)
{
	test_strcmp(NULL, NULL, 0);
	test_strcmp(NULL, "str", -1);
	test_strcmp("str", NULL, 1);

	test_strcmp("str", "str", 0);
	test_strcmp("strA", "strB", -1);
	test_strcmp("strB", "strA", 1);

	return 0;
}


void test_strcmp(char *s1, char *s2, int result)
{
	int test;

	test = ggz_strcmp(s1, s2);
	if (result == signum(test))
		printf("Success\n");
	else
		printf("Error: ggz_strcmp(%s, %s) = %d\n", s1, s2, result);
}


int signum(int x)
{
	if (x < 0)
		return -1;
	else if (x == 0)
		return 0;
	else
		return 1;
}
