#include <stdio.h>

#include "ggzdmod.h"
#include "ggz.h"

void print_seat(GGZSeat seat);
char * seat_desc(GGZdModSeat type);
int seatcmp(GGZSeat seat1, GGZSeat seat2);
int safe_strcmp(char *str1, char *str2);
int compare_seats(GGZdMod *mod, int num, GGZSeat *seats);

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
	printf("-- GGZdMod(-1) : %p\n", mod);

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
	
	printf("-- Initial state: %d\n", ggzdmod_get_state(mod));
	printf("-- Initial fd: %d\n", ggzdmod_get_fd(mod));
	printf("-- Initial seats: %d\n", ggzdmod_get_num_seats(mod));
	printf("-- Initial gamedata: %p\n", ggzdmod_get_gamedata(mod));
	

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
	control[2].type = GGZ_SEAT_RESV;
	control[2].name = "Test 2";
	control[2].fd = 80;
	ggzdmod_set_seat(mod, &control[2]);

	compare_seats(mod, 3, control);
	

	
	
	ggzdmod_free(mod);

	/* FIXME: test all functions with a NULL object */
	/* FIXME: generally test functions with bad parameter values */

	ggz_debug_cleanup();
	ggz_memory_check();

	return 0;
}


void print_seat(GGZSeat seat)
{
	printf("\n -- Begin GGZSeat --\n");
	printf("   Num: %d\n", seat.num);
	printf("  Type: %s\n", seat_desc(seat.type));
	printf("  Name: %s\n", seat.name);
	printf("    FD: %d\n", seat.fd);
	printf(" -- End GGZSeat --\n");
}


char * seat_desc(GGZdModSeat type)
{
	switch (type) {
	case GGZ_SEAT_OPEN:
		return "Open";
	case GGZ_SEAT_BOT:
		return "Bot";
	case GGZ_SEAT_RESV: 
		return "Reserved";
	case GGZ_SEAT_NONE: 
		return "None";
	case GGZ_SEAT_PLAYER: 
		return "Player";
	}

	return "**Error**";
}



int seatcmp(GGZSeat seat1, GGZSeat seat2)
{
	if (seat1.num == seat2.num 
	    && seat1.type == seat2.type
	    && seat1.fd == seat2.fd
	    && safe_strcmp(seat1.name, seat2.name) == 0)
		return 0;
	else
		return -1;
}


int compare_seats(GGZdMod *mod, int num, GGZSeat *seats)
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


int safe_strcmp(char *str1, char *str2)
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
