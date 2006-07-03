#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ggz.h>

static int testconversion(const char *orig)
{
	char *enc, *dec;

	enc = ggz_base64_encode(orig, strlen(orig));
	dec = ggz_base64_decode(enc, strlen(enc));

	printf("Original: %s\n", orig);
	printf("Encoded : %s\n", enc);
	printf("Decoded : %s\n", dec);

	if(strcmp(orig, dec)) return 1;

	free(enc);
	free(dec);

	return 0;
}

int main(int argc, char *argv[])
{
	int ret = 0;

	ret |= testconversion("");
	ret |= testconversion("a");
	ret |= testconversion("ab");
	ret |= testconversion("abc");
	ret |= testconversion("test");

	return ret;
}

