#include <string.h>
#include <stdio.h>
#include <stdlib.h>

unsigned int hashstring2(const char *s)
{
	int i;
	unsigned int hashvalue = 0;

	for(i = 0; s[i] && s[i + 1]; i += 2)
	{
		hashvalue += (s[i] - s[i + 1]);
	}

	hashvalue = hashvalue % 256;

	return hashvalue;
}

unsigned int hashstring(const char *s)
{
	int i;
	unsigned int hashvalue = 0;

	for(i = 0; s[i]; i++)
	{
		hashvalue += s[i];
	}

	hashvalue = hashvalue % 256;

	return hashvalue;
}

void hashfile(const char *filename)
{
	int hash[256];
	int hash2[256];
	FILE *f;
	char string[128];
	int i;
	char *ret;
	unsigned int hashvalue;
	int totals;

	for(i = 0; i < 256; i++)
	{
		hash[i] = 0;
		hash2[i] = 0;
	}

	f = fopen(filename, "r");
	if(!f)
	{
		fprintf(stderr, "Error: Could not read '%s'\n", filename);
		return;
	}

	while(!feof(f))
	{
		ret = fgets(string, sizeof(string), f);
		if(ret)
		{
			string[strlen(string) - 1] = '\0';
			hashvalue = hashstring(string);
			printf(":: %s: %i\n", string, hashvalue);
			hash[hashvalue]++;

			if(hashvalue == 255)
			{
				hashvalue = hashstring2(string);
				printf(" >> %i\n", hashvalue);
				hash2[hashvalue]++;
			}
		}
	}

	totals = 0;
	for(i = 0; i < 256; i++)
	{
		printf("{%i}: %i\n", i, hash[i]);
		totals += hash[i];
	}
	printf("Total strings: %i\n", totals);

	for(i = 0; i < 256; i++)
	{
		printf("<255:{%i}>: %i\n", i, hash2[i]);
	}

	fclose(f);
}

int main(int argc, char *argv[])
{
	if(argc != 2)
	{
		fprintf(stderr, "Error: needs argument <wordlistfile>\n");
		return -1;
	}

	//hashfile("/usr/share/dict/words");
	hashfile(argv[1]);

	return 0;
}

