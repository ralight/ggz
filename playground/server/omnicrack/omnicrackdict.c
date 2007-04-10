#include <stdio.h>
#include <string.h>

#include "hasher.h"

int main(int argc, char *argv[])
{
	int error = 0;
	char *mode;
	int ret;

	if(argc >= 2)
	{
		mode = argv[1];
		if(!strcmp(mode, "create"))
		{
			if(argc == 4)
			{
				ret = hashfile(argv[3], argv[2]);
				if(ret == 1)
				{
					printf("Conversion succeeded.\n");
				}
				else
				{
					fprintf(stderr, "Error: something went wrong!\n");
				}
			}
			else error = 1;
		}
		else if(!strcmp(mode, "check"))
		{
			if(argc == 4)
			{
				ret = findword(argv[2], argv[3]);
				if(ret == 1)
				{
					printf("Word '%s' was found!\n", argv[3]);
				}
				else if(ret == 0)
				{
					printf("Word '%s' not found.\n", argv[3]);
				}
				else
				{
					fprintf(stderr, "Error: something went wrong!\n");
				}
			}
			else error = 1;
		}
		else error = 1;
	}
	else error = 1;

	if(error)
	{
		fprintf(stderr, "Error, check the syntax!\n");
		fprintf(stderr, "- omnicrack_dict create <hashfile> <wordlistfile>\n");
		fprintf(stderr, "- omnicrack_dict check <hashfile> <word>\n");
		return -1;
	}

	return 0;
}

