#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ggz.h>

#include "ggzdmod.h"
#include "ggzdmod-ggz.h"

static void print_seat(GGZSeat seat);
static char * seat_desc(GGZSeatType type);
static int seatcmp(GGZSeat seat1, GGZSeat seat2);
static int safe_strcmp(const char *str1, const char *str2);
static int compare_seats(GGZdMod *mod, int num, GGZSeat *seats);

static void test_handler(GGZdMod *mod, GGZdModEvent e, const void *data) { }

char* test_args[] = {"Arg1", "Arg2", NULL};

int main (void) {
	
	GGZdMod *mod;
	GGZdModType type;
	GGZSeat control[8];
	int test = 5, num, i;
	void * ptr;

	/* Only enable memory debugging */
	ggz_debug_init(NULL, NULL);
	ggz_debug_enable("MEMCHECK");

	printf("-- Testing object creation -- \n");

	/* Test with an invalid parameter */
	mod = ggzdmod_new(-1);
	if (mod != NULL)
		printf("-- Error : ggzdmod_new(-1) returns %p\n", mod);
	else
		printf("-- Success: ggzdmod_new(-1) returns NULL.\n");

	/* Test ggz side object */
	mod = ggzdmod_new(GGZDMOD_GGZ);
	type = ggzdmod_get_type(mod);
	if (type == GGZDMOD_GGZ) 
		printf("-- Success: creation of type GGZDMOD_GGZ\n");
	else
		printf("-- Error: type %d != GGZDMOD_GGZ\n", type);
	ggzdmod_free(mod);

	/* Test game side object */
	mod = ggzdmod_new(GGZDMOD_GAME);
	type = ggzdmod_get_type(mod);
	if (type == GGZDMOD_GAME) 
		printf("-- Success: creation of type GGZDMOD_GAME\n");
	else
		printf("-- Error: type %d != GGZDMOD_GAME\n", type);
	ggzdmod_free(mod);

	
	/* Test various accessor functions */
	mod = ggzdmod_new(GGZDMOD_GGZ);
	if (ggzdmod_get_state(mod) != GGZDMOD_STATE_CREATED)
		printf("-- Error: initial state: %d\n",
		       ggzdmod_get_state(mod));
	if (ggzdmod_get_fd(mod) != -1)
		printf("-- Error: initial fd: %d\n",
		       ggzdmod_get_fd(mod));
	if (ggzdmod_get_num_seats(mod) > 0)
		printf("-- Error: initial seats: %d\n",
		       ggzdmod_get_num_seats(mod));
	if (ggzdmod_get_gamedata(mod) != NULL)
		printf("-- Error: initial gamedata: %p\n",
		       ggzdmod_get_gamedata(mod));
	

	/* Set setting/getting game data */
	ggzdmod_set_gamedata(mod, &test);
	ptr = ggzdmod_get_gamedata(mod);
	if (ptr == &test)
		printf("-- Success: setting of gamedata to %p\n", &test);
	else
		printf("-- Error: set data to %p, got %p\n", &test, ptr);
	ggzdmod_set_gamedata(mod, NULL);
	ptr = ggzdmod_get_gamedata(mod);
	if (ptr == NULL)
		printf("-- Success: setting of gamedata to %p\n", NULL);
	else
		printf("-- Error: set data to %p, got %p\n", NULL, ptr);


	/* Set setting/getting num_seats */
	ggzdmod_set_num_seats(mod, 3);
	num = ggzdmod_get_num_seats(mod);
	if (num == 3)
		printf("-- Success: setting of seats to %d\n", 3);
	else
		printf("-- Error: set seats to %d, got  %d\n", 3, num);

	/* Check initial value of seats */
	for (i = 0; i < 8; i++) {
		control[i].num = i;
		control[i].type = GGZ_SEAT_OPEN;
		control[i].name = NULL;
		control[i].fd = -1;
	}
	compare_seats(mod, 3, control);
	
	/* Test seat setting/getting */
	control[0].num = 0;
	control[0].type = GGZ_SEAT_PLAYER;
	control[0].name = "Test 0";
	control[0].fd = 17;
	ggzdmod_set_seat(mod, &control[0]);

	control[1].num = 1;
	control[1].type = GGZ_SEAT_BOT;
	control[1].name = "Test 1";
	control[1].fd = 32;
	ggzdmod_set_seat(mod, &control[1]);

	control[2].num = 2;
	control[2].type = GGZ_SEAT_RESERVED;
	control[2].name = "Test 2";
	control[2].fd = 80;
	ggzdmod_set_seat(mod, &control[2]);

	compare_seats(mod, 3, control);
	
	/* Pass NULL in to each function. */
	printf(" -- Testing passing ggzdmod=NULL to functions --\n");
	ggzdmod_free(NULL);
	if (ggzdmod_get_fd(NULL) != -1)
		printf(" -- Error: ggzdmod_get_fd(NULL) == %d\n", ggzdmod_get_fd(NULL));
	(void)ggzdmod_get_type(NULL);
	(void)ggzdmod_get_state(NULL);
	if (ggzdmod_get_num_seats(NULL) > 0)
		printf(" -- Error: ggzdmod_get_num_seats(NULL) == %d\n", ggzdmod_get_num_seats(NULL));
	(void)ggzdmod_get_seat(NULL, 0);
	if (ggzdmod_get_gamedata(NULL) != NULL)
		printf(" -- Error: ggzdmod_get_gamedata(NULL) == %p\n", ggzdmod_get_gamedata(NULL));
	ggzdmod_set_num_seats(NULL, 8);
	ggzdmod_set_handler(NULL, GGZDMOD_EVENT_PLAYER_DATA, test_handler);
	ggzdmod_set_module(NULL, "<GAME>", "/tmp", test_args);
	if (ggzdmod_set_seat(NULL, &control[0]) >= 0)
		printf(" -- Error: ggzdmod_set_seat(NULL, ...) claimed to succeed.\n");
	if (ggzdmod_dispatch(NULL) >= 0)
		printf(" -- Error: ggzdmod_dispatch(NULL) claimed to succeed.\n");
	if (ggzdmod_loop(NULL) >= 0)
		printf(" -- Error: ggzdmod_loop(NULL) claimed to succeed.\n");
	if (ggzdmod_set_state(NULL, GGZDMOD_STATE_DONE) >= 0)
		printf(" -- Error: ggzdmod_set_state(NULL, GGZDMOD_STATE_DONE) claimed to succeed.\n");
	if (ggzdmod_connect(NULL) >= 0)
		printf(" -- Error: ggzdmod_connect(NULL) claimed to succeed.\n");
	if (ggzdmod_disconnect(NULL) >= 0)
		printf(" -- Error: ggzdmod_disconnect(NULL) claimed to succeed.\n");
	if (ggzdmod_log(NULL, "Test message") >= 0)
		printf(" -- Error: ggzdmod_log(NULL, ...) claimed to succeed.\n");
	
	
	ggzdmod_free(mod);

	/* FIXME: test all functions with a NULL object */
	/* FIXME: generally test functions with bad parameter values */

	ggz_debug_cleanup(GGZ_CHECK_MEM);
	
	printf(" -- All tests complete -- \n");

	return 0;
}


static void print_seat(GGZSeat seat)
{
	printf("\n -- Begin GGZSeat --\n");
	printf("   Num: %d\n", seat.num);
	printf("  Type: %s\n", seat_desc(seat.type));
	printf("  Name: %s\n", seat.name);
	printf("    FD: %d\n", seat.fd);
	printf(" -- End GGZSeat --\n");
}


static char * seat_desc(GGZSeatType type)
{
	switch (type) {
	case GGZ_SEAT_OPEN:
		return "Open";
	case GGZ_SEAT_BOT:
		return "Bot";
	case GGZ_SEAT_RESERVED:
		return "Reserved";
	case GGZ_SEAT_NONE: 
		return "None";
	case GGZ_SEAT_PLAYER: 
		return "Player";
	case GGZ_SEAT_ABANDONED:
		return "Abandoned";
	}

	return "**Error**";
}



static int seatcmp(GGZSeat seat1, GGZSeat seat2)
{
	if (seat1.num == seat2.num 
	    && seat1.type == seat2.type
	    && seat1.fd == seat2.fd
	    && safe_strcmp(seat1.name, seat2.name) == 0)
		return 0;
	else
		return -1;
}


static int compare_seats(GGZdMod *mod, int num, GGZSeat *seats)
{
	int count, i;
	GGZSeat test;
	

	count = ggzdmod_get_num_seats(mod);
	if (count == num)
		printf("-- Success: number of seats is %d\n", num);
	else {
		printf("-- Error: number of seats is %d, got %d\n", num, count);
		return -1;
	}
	
	for (i = 0; i < num; i++) {
		test = ggzdmod_get_seat(mod, i);
		if (seatcmp(test, seats[i]) == 0)
			printf("-- Success: seat %d matches\n", i);
		else {
			printf("-- Error: seat %d is actually:", i);
			print_seat(seats[i]);
			printf("but we got:");
			print_seat(test);
			return -1;
		}
	}
	
	return 0;
}


static int safe_strcmp(const char *str1, const char *str2)
{
	if (!str1) {
		if (!str2)
			return 0;
		else
			return 1;
	}
	else if (!str2) 
		return -1;
	else
		return strcmp(str1, str2);
}
