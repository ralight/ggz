#include <stdio.h>

#include "ggzdmod.h"
#include "ggz.h"

int main (void) {
	
	GGZdMod *mod;
	GGZdModType type;
	int test = 5, num;
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
		printf("-- Successful creation of type GGZDMOD_GGZ\n");
	else
		printf("-- Error: type %d != GGZDMOD_GGZ\n", type);
	ggzdmod_free(mod);

	/* Test game side object */
	mod = ggzdmod_new(GGZDMOD_GAME);
	type = ggzdmod_get_type(mod);
	if (type == GGZDMOD_GAME) 
		printf("-- Successful creation of type GGZDMOD_GAME\n");
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
		printf("-- Successful setting of gamedata to %p\n", &test);
	else
		printf("-- Error: set data to %p, got %p\n", &test, ptr);
	ggzdmod_set_gamedata(mod, NULL);
	ptr = ggzdmod_get_gamedata(mod);
	if (ptr == NULL)
		printf("-- Successful setting of gamedata to %p\n", NULL);
	else
		printf("-- Error: set data to %p, got %p\n", NULL, ptr);


	/* Set setting/getting num_seats */
	ggzdmod_set_num_seats(mod, 3);
	num = ggzdmod_get_num_seats(mod);
	if (num == 3)
		printf("-- Successful setting of seats to %d\n", 3);
	else
		printf("-- Error: set seats to %d, got  %d\n", 3, num);


	/* FIXME: test setting/getting of seats and handlers */

	ggzdmod_free(mod);

	/* FIXME: test all functions with a NULL object */

	ggz_debug_cleanup();
	ggz_memory_check();

	return 0;
}

