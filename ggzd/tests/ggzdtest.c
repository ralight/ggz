#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ggz.h>

#include "ggzdmod.h"
#include "ggzdmod-ggz.h"


GGZSeat seats[] = {
	{0, GGZ_SEAT_OPEN, NULL, -1},
	{1, GGZ_SEAT_BOT, "Bob", -1},
	{2, GGZ_SEAT_OPEN, NULL, -1},
	{3, GGZ_SEAT_RESERVED, "Junior", -1}
};

static void callback(GGZdMod *mod, GGZdModEvent event, const void *data)
{
	switch (event) {
	case GGZDMOD_EVENT_JOIN:
		printf("GGZDMOD_EVENT_JOIN\n");
		break;
	case GGZDMOD_EVENT_LEAVE:
		printf("GGZDMOD_EVENT_LEAVE\n");
		break;
	default:
		printf("Yay.  A callback for event %d\n", event);
	}
}


static void handle_log(GGZdMod *mod, GGZdModEvent event, const void *data)
{
	printf("LOG: %s\n", (char*)data);
}


static void handle_error(GGZdMod *mod, GGZdModEvent event, const void *data)
{
	printf("GGZDMOD_EVENT_ERROR: %s\n", (char*)data);
	abort();
}


static void handle_state(GGZdMod *mod, GGZdModEvent event, const void *data)
{
	GGZdModState cur, prev;
	GGZSeat seat;
	char *states[] = {"CREATED","WAITING","PLAYING","DONE"};

	prev = *(GGZdModState*)data;
	cur = ggzdmod_get_state(mod);
	printf("GGZDMOD_EVENT_STATE: %s->%s\n", states[prev], states[cur]);

	switch (cur) {
	case GGZDMOD_STATE_WAITING:
		/* Waiting for players */
		printf("Game now waiting for players\n");
		seat.num = 0;
		seat.type = GGZ_SEAT_PLAYER;
		seat.name = "Jean Claude";
		seat.fd = 0;
		ggzdmod_set_seat(mod, &seat);

		seat.num = 2;
		seat.type = GGZ_SEAT_PLAYER;
		seat.name = "Larry";
		seat.fd = 0;
		ggzdmod_set_seat(mod, &seat);

		break;
		
	case GGZDMOD_STATE_PLAYING:
		printf("Game now in progress\n");
		break;

	case GGZDMOD_STATE_DONE:
		printf("Game over\n");
		break;
	default:
		printf("Unknown state: %d\n", cur);
	}
}


static int table_launch(void)
{
        int i;
	char cmd[1024];
	char *args[] = {cmd, NULL};
        GGZdMod *ggz;

	snprintf(cmd, sizeof(cmd), "%s/logmod", getenv("PWD"));

        /* Create new ggz game module object */
        ggz = ggzdmod_new(GGZDMOD_GGZ);

        /* Setup initial seats for game */
        ggzdmod_set_num_seats(ggz, 4);
        for (i = 0; i < 4; i++) {
		ggzdmod_set_seat(ggz, &seats[i]);
        }
        
        /* Setup handlers for game module events */
        ggzdmod_set_handler(ggz, GGZDMOD_EVENT_STATE, &handle_state);
        ggzdmod_set_handler(ggz, GGZDMOD_EVENT_JOIN, &callback);
        ggzdmod_set_handler(ggz, GGZDMOD_EVENT_LEAVE, &callback);
        ggzdmod_set_handler(ggz, GGZDMOD_EVENT_LOG, &handle_log);
        ggzdmod_set_handler(ggz, GGZDMOD_EVENT_ERROR, &handle_error);
	
	/* Set game module arguments */
	ggzdmod_set_module(ggz, "/tmp", args);
	
        /* Attempt to launch game */
        if (ggzdmod_connect(ggz) < 0) {
                /* Error starting up game */
                ggzdmod_free(ggz);
		printf("Error: couldn't connect\n");
                return -1;
        }
        
        /* Use game module's main loop */
        ggzdmod_loop(ggz);

        /* Free up ggz module object */ 
        ggzdmod_free(ggz);
        
        return 0;
}


int main(void)
{
	int status;
	char *types[] = {"GGZDMOD", NULL};

	ggz_debug_init((const char**)types, NULL);
	status = table_launch();
	ggz_debug_cleanup(GGZ_CHECK_MEM);
	
	return status;
}

