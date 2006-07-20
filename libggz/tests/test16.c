#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ggz.h>

int main(int argc, char *argv[])
{
	char *enc;
	char plain[7];
	char expected[10];

	strncpy(plain, "hello", 7);
	strncpy(expected, "68656c6c6f", 12);

	enc = ggz_base16_encode(plain, strlen(plain));
	printf("Original: %s\n", plain);
	printf("Encoded:  %s\n", enc);
	printf("Expected: %s\n", expected);

	if(strcmp(enc, expected)) return 1;

	free(enc);

	return 0;
}

