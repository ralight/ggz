#include "hasher.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* Secondary hash function */
static unsigned int hashstring2(const char *s)
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

/* Primary hash function */
static unsigned int hashstring1(const char *s)
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

int hashfile(const char *wordlistfilename, const char *hashfilename)
{
	int hash1[256];
	int hash2[256][256];
	int hash2size[256][256];
	int hash2pos[256][256];
	char *hashstrings[256][256];
	FILE *f, *fout;
	char string[128];
	int i, j;
	char *ret;
	unsigned int hashvalue, hashvalue2;
	int len, oldlen;
	int oldpos, possize;
#if DEBUG
	int totals;
#endif

	for(i = 0; i < 256; i++)
	{
		hash1[i] = 0;
		for(j = 0; j < 256; j++)
		{
			hash2[i][j] = 0;
			hash2size[i][j] = 0;
			hash2pos[i][j] = 0;
			hashstrings[i][j] = NULL;
		}
	}

	f = fopen(wordlistfilename, "r");
	if(!f)
	{
#if DEBUG
		fprintf(stderr, "Error: Could not read '%s'\n", wordlistfilename);
#endif
		return -1;
	}

	while(!feof(f))
	{
		ret = fgets(string, sizeof(string), f);
		if(ret)
		{
			len = strlen(string) - 1;
			string[len] = '\0';

			hashvalue = hashstring1(string);
			/*printf(":: %s: %i\n", string, hashvalue);*/
			hash1[hashvalue]++;

			hashvalue2 = hashstring2(string);
			/*printf(" >> %i\n", hashvalue2);*/
			hash2[hashvalue][hashvalue2]++;

			oldlen = hash2size[hashvalue][hashvalue2];
			hashstrings[hashvalue][hashvalue2] = (char*)realloc(hashstrings[hashvalue][hashvalue2],
				oldlen + len + sizeof(int));
			memcpy(hashstrings[hashvalue][hashvalue2] + oldlen, &len, sizeof(int));
			memcpy(hashstrings[hashvalue][hashvalue2] + oldlen + sizeof(int), string, len);

			hash2size[hashvalue][hashvalue2] += (len + sizeof(int));
		}
	}

#if DEBUG
	totals = 0;
	for(i = 0; i < 256; i++)
	{
		printf("{%i}: %i\n", i, hash1[i]);
		totals += hash1[i];
	}
	printf("Total strings: %i\n", totals);
#endif

	oldpos = 256 * 256 * sizeof(int);
	for(i = 0; i < 256; i++)
	{
		for(j = 0; j < 256; j++)
		{
			hash2pos[i][j] = oldpos;
			possize = hash2size[i][j];
			oldpos += possize;
#if DEBUG
			printf("count <%i:{%i}>: %i\n", i, j, hash2[i][j]);
#endif
		}
	}

	fout = fopen(hashfilename, "w");

	for(i = 0; i < 256; i++)
	{
		for(j = 0; j < 256; j++)
		{
#if DEBUG
			printf("pos <%i:{%i}>: %i\n", i, j, hash2pos[i][j]);
#endif

			fwrite(&hash2pos[i][j], sizeof(int), 1, fout);
		}
	}

	for(i = 0; i < 256; i++)
	{
		for(j = 0; j < 256; j++)
		{
			fwrite(hashstrings[i][j], hash2size[i][j], 1, fout);

			free(hashstrings[i][j]);
		}
	}

	fclose(fout);

	fclose(f);

	return 1;
}

int findword(const char *hashfilename, const char *word)
{
	FILE *f;
	unsigned int hashvalue1, hashvalue2;
	int pos, nextpos, i;
	int len;
	char *str;
	int found;

	hashvalue1 = hashstring1(word);
	hashvalue2 = hashstring2(word);
#if DEBUG
	printf("Word hash indices: <%i:{%i}>\n", hashvalue1, hashvalue2);
#endif

	f = fopen(hashfilename, "r");
	if(!f)
	{
#if DEBUG
		fprintf(stderr, "Error: Could not read '%s'\n", hashfilename);
#endif
		return -1;
	}

	fseek(f, (hashvalue1 * 256 + hashvalue2) * sizeof(int), SEEK_SET);
	fread(&pos, sizeof(int), 1, f);
	fread(&nextpos, sizeof(int), 1, f);
#if DEBUG
	printf("Word '%s' is in list at position '%i'\n", word, pos);
	printf("Next position is '%i'\n", nextpos);
#endif

	fseek(f, pos, SEEK_SET);
	i = pos;
	found = 0;
	while(i < nextpos)
	{
		fread(&len, sizeof(int), 1, f);
#if DEBUG
		printf("Word length: %i\n", len);
#endif
		/* FIXME: Emergency break??? */
		if(len < 1048576)
		{
			str = (char*)malloc(len + 1);
			fread(str, len, 1, f);
			str[len] = '\0';
#if DEBUG
			printf("=> Check '%s'\n", str);
#endif
			if(!strcmp(word, str))
			{
				found = 1;
			}
			free(str);
			if(found);
		}

		i += len + sizeof(int);
	}

	fclose(f);

	return found;
}

