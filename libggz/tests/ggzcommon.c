#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ggz.h"
#include "ggz_common.h"

char *tests[] = {"2 3 5..7",
		 "1..3",
		 "",
		 "\t ",
		 "2\t3  5\t 7..10",
		 NULL};
int values[][3][6] = { { {2, 3, 5, 6, 7, 0}, {1, 4, 8, 32, 1000, 0}, {7} },
		       { {1, 2, 3, 0}, {4, 5, 9, 32, 34, 0}, {3} },
		       { {0}, {1, 3, 7, 10, 1000, 0}, {0} },
		       { {0}, {1, 3, 7, 10, 1000, 0}, {0} },
		       { {2, 3, 5, 7, 9, 0}, {4, 6, 11, 15, 32, 0}, {10} } };

int main()
{
	int i, result = 0;

	for (i = 0; tests[i]; i++) {
		GGZNumberList list = ggz_numberlist_read(tests[i]), list2;
		char *str;
		int j;
		int *yes = values[i][0];
		int *no = values[i][1];
		int max = values[i][2][0];

		for (j = 0; yes[j] > 0; j++)
			if (!ggz_numberlist_isset(&list, yes[j])) {
				printf("Error: %d is included in %s.\n",
				       yes[j], tests[i]);
				result = 1;
			}

		for (j = 0; no[j] > 0; j++)
			if (ggz_numberlist_isset(&list, no[j])) {
				printf("Error: %d isn't included in %s.\n",
				       no[j], tests[i]);
				result = 1;
			}

		if (ggz_numberlist_get_max(&list) != max)
			printf("Error: max is %d, but we found %d in %s.\n",
			       max, ggz_numberlist_get_max(&list), tests[i]);

		str = ggz_numberlist_write(&list);
		list2 = ggz_numberlist_read(str);
		if (memcmp(&list, &list2, sizeof(list))) {
			printf("Error: write+read didn't match for %s.\n",
			       tests[i]);
			result = 1;
		}
		ggz_free(str);
	}

	return result;
}
