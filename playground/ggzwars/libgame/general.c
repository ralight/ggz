#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *stralloc (char *string1, char *string2)
{
	char *temp;
	temp = (char*)malloc ((strlen(string1)+strlen(string2)+1));
	strcpy (temp, string1);
	strcat (temp, string2);
	return temp;
}
