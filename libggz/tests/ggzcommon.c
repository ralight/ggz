#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ggz.h"
#include "ggz_common.h"

char *tests[] = {"2 3 5..7",
		 "1..3",
		 NULL};
int values[][2][6] = { { {2, 3, 5, 6, 7, 0}, {1, 4, 8, 32, 1000, 0} },
			{ { 1, 2, 3, 0}, {4, 5, 9, 32, 34, 0} } };

int main()
{
	int i, result = 0;

	for (i = 0; tests[i]; i++) {
	  GGZNumberList list = ggz_numberlist_read(tests[i]);
	  char *str = ggz_numberlist_write(&list);
	  int j;
	  int *yes = values[i][0];
	  int *no = values[i][1];

	  for (j = 0; yes[j] > 0; j++)
	    if (!ggz_numberlist_isset(&list, yes[j])) {
	      printf("Error: %d should be included in %s.\n",
		     yes[j], tests[i]);
	      result = 1;
	    }

	  for (j = 0; no[j] > 0; j++)
	    if (ggz_numberlist_isset(&list, no[j])) {
	      printf("Error: %d shouldn't be included in %s.\n",
		     no[j], tests[i]);
	      result = 1;
	    }

	  if (strcmp(str, tests[i])) {
	    printf("Error: numberlist '%s' gave '%s' on output.\n",
		   tests[i], str);
	    result = 1;
	  }

	  ggz_free(str);
	}

	return result;
}
