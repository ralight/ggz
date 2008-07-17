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
	const char *str;

	for (i = 0; tests[i]; i++) {
		GGZNumberList list = ggz_numberlist_read(tests[i]), list2;
		int j;
		int *yes = values[i][0];
		int *no = values[i][1];
		int max = values[i][2][0];
		char *str;

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

#define GGZ_SEAT_COUNT (GGZ_SEAT_ABANDONED + 1)
	for (i = 0; i < GGZ_SEAT_COUNT; i++) {
		str = ggz_seattype_to_string(i);
		if (i != ggz_string_to_seattype(str)) {
			printf("Error: seattype %d incorrectly matches "
			       "string %s.\n",
			       i, str);
			result = 1;
		}
	}
	str = ggz_seattype_to_string(GGZ_SEAT_COUNT);
	if (strcmp(str, ggz_seattype_to_string(-1)) != 0) {
		printf("Error: seattype %d incorrectly matches "
		       "string %s.\n",
		       GGZ_SEAT_COUNT, str);
		result = 1;
	}

#define GGZ_CHAT_COUNT (GGZ_CHAT_TABLE + 1)
	for (i = 0; i < GGZ_CHAT_COUNT; i++) {
		const char *str = ggz_chattype_to_string(i);

		if (i != ggz_string_to_chattype(str)) {
			printf("Error: chattype %d incorrectly matches "
			       "string %s.\n",
			       i, str);
			result = 1;
		}
	}
	str = ggz_chattype_to_string(GGZ_CHAT_COUNT);
	if (strcmp(str, ggz_chattype_to_string(-1)) != 0) {
		printf("Error: chattype %d incorrectly matches "
		       "string %s.\n",
		       GGZ_CHAT_COUNT, str);
		result = 1;
	}

#define GGZ_LEAVE_COUNT (GGZ_LEAVE_GAMEERROR + 1)
	for (i = 0; i < GGZ_LEAVE_COUNT; i++) {
		const char *str = ggz_leavetype_to_string(i);

		if (i != ggz_string_to_leavetype(str)) {
			printf("Error: leavetype %d incorrectly matches "
			       "string %s.\n",
			       i, str);
			result = 1;
		}
	}
	str = ggz_leavetype_to_string(GGZ_LEAVE_COUNT);
	if (strcmp(str, ggz_leavetype_to_string(-1)) != 0) {
		printf("Error: leavetype %d incorrectly matches "
		       "string %s.\n",
		       GGZ_LEAVE_COUNT, str);
		result = 1;
	}

#define GGZ_PLAYER_COUNT (GGZ_PLAYER_UNKNOWN + 1)
	for (i = 0; i < GGZ_PLAYER_COUNT; i++) {
		const char *str = ggz_playertype_to_string(i);

		if (i != ggz_string_to_playertype(str)) {
			printf("Error: playertype %d incorrectly matches "
			       "string %s.\n",
			       i, str);
			result = 1;
		}
	}
	str = ggz_playertype_to_string(GGZ_PLAYER_COUNT);
	if (strcmp(str, ggz_playertype_to_string(-1)) != 0) {
		printf("Error: playertype %d incorrectly matches "
		       "string %s.\n",
		       GGZ_PLAYER_COUNT, str);
		result = 1;
	}

#define GGZ_ADMIN_COUNT (GGZ_ADMIN_UNKNOWN + 1)
	for (i = 0; i < GGZ_ADMIN_COUNT; i++) {
		const char *str = ggz_admintype_to_string(i);

		if (i != ggz_string_to_admintype(str)) {
			printf("Error: admintype %d incorrectly matches "
			       "string %s.\n",
			       i, str);
			result = 1;
		}
	}
	str = ggz_admintype_to_string(GGZ_ADMIN_COUNT);
	if (strcmp(str, ggz_admintype_to_string(-1)) != 0) {
		printf("Error: admintype %d incorrectly matches "
		       "string %s.\n",
		       GGZ_ADMIN_COUNT, str);
		result = 1;
	}

#define E_LOWEST (E_BAD_PASSWORD - 1)
	for (i = 0; i > E_LOWEST; i--) {
		const char *str = ggz_error_to_string(i);

		if (i != ggz_string_to_error(str)) {
			printf("Error: errortype %d incorrectly matches "
			       "string %s.\n",
			       i, str);
			result = 1;
		}
	}
	str = ggz_error_to_string(E_LOWEST);
	if (strcmp(str, ggz_error_to_string(1000)) != 0) {
		printf("Error: error %d incorrectly matches "
		       "string %s.\n",
		       E_LOWEST, str);
		result = 1;
	}
	str = ggz_error_to_string(1);
	if (strcmp(str, ggz_error_to_string(1000)) != 0) {
		printf("Error: error %d incorrectly matches "
		       "string %s.\n",
		       1, str);
		result = 1;
	}

	for (i = 0; i <= GGZ_PERM_COUNT; i++) {
		const char *str = ggz_perm_to_string(i);

		if (i != ggz_string_to_perm(str)) {
			printf("Error: permission %d incorrectly matches "
			       "string %s.\n",
			       i, str);
			result = 1;
		}
	}

	return result;
}
