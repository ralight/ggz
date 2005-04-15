
#include <stdio.h>
#include <stdlib.h>

#include <ggz.h>
#include <ggz_common.h>

int fd = 3;

typedef enum {
	REQ_GAME_LAUNCH,	/**< sent on game launch */
	REQ_GAME_JOIN,		/**< sent on player join */
	REQ_GAME_LEAVE,		/**< sent on player leave */
	RSP_GAME_STATE,		/**< sent in response to a gameover */
} ControlToTable;

typedef enum {
	RSP_GAME_JOIN,		/**< sent in response to a player join */
	RSP_GAME_LEAVE,		/**< sent in response to a player leave */
	MSG_LOG,		/**< a message to log */
	REQ_GAME_STATE,		/**< sent to tell of a game-over */
} TableToControl;

typedef enum {
	GGZ_STATE_CREATED,	/**< Pre-launch; waiting for ggzdmod */
	GGZ_STATE_WAITING,	/**< Ready and waiting to play. */
	GGZ_STATE_PLAYING,	/**< Currently playing a game. */
	GGZ_STATE_DONE		/**< Table halted, prepping to exit. */
} GGZdModState;


static int handle_game_launch()
{
	int i, num;
	char name[1024];
	
	ggz_read_int(fd, &num);
	printf("Receiving %d seats\n", num);
	
	for (i = 0; i < num; i++) {
		GGZSeatType type;
		int operator;

		if (ggz_read_int(fd, &operator) < 0)
			return -1;
		type = operator;

		switch (type) {
		case GGZ_SEAT_OPEN:
			printf("Received open seat\n");
			break;
		case GGZ_SEAT_BOT: 
			printf("Received bot seat\n");
			break;
		case GGZ_SEAT_RESERVED:
			printf("Received reserved seat\n");
			break;
		case GGZ_SEAT_NONE: 
			printf("Received none seat\n");
			break;
		case GGZ_SEAT_PLAYER:
			printf("Received player seat\n");
			break;
		case GGZ_SEAT_ABANDONED:
			printf("Received abandoned seat\n");
			break;
		}	
		if (type == GGZ_SEAT_RESERVED
		    && ggz_read_string(fd, name, 1024) < 0) {
			printf("Error reading reserved player name\n");
			return -1;
		}
	}

	ggz_write_int(fd, REQ_GAME_STATE);
	ggz_write_char(fd, GGZ_STATE_WAITING);

	return 0;
}


static int handle_game_join(void)
{
	int num, pfd;
	char *name;

	if (ggz_read_int(fd, &num) < 0
	    || ggz_read_string_alloc(fd, &name) < 0
	    || ggz_read_fd(fd, &pfd) < 0) {
		printf("Error reading from GGZ\n");
		return -1;
	}
	
	printf("Received %s on %d in seat %d\n", name, pfd, num);
	ggz_free(name);
	
	if (ggz_write_int(fd, RSP_GAME_JOIN) < 0
	    || ggz_write_char(fd, 0)) {
		printf("Error sending data to GGZ\n");
		return -1;
	}

	return 0;
}


static int handle_game_leave(void)
{
	char *name;

	if (ggz_read_string_alloc(fd, &name) < 0) {
		printf("Error reading from GGZ\n");
		return -1;
	}
	
	printf("Removing %s\n", name);

	if (ggz_write_int(fd, RSP_GAME_LEAVE) < 0 
	    || ggz_write_char(fd, 0) < 0) {
		printf("Error sending data to GGZ\n");
		return -1;
	}

	return 0;
}


static int handle_game_state(void)
{
	printf("GGZ Acknowledged state change\n");

	return 0;
}


static int handle_opcode(int opcode)
{
	int status = 0;
	
	switch (opcode) {
	case REQ_GAME_LAUNCH:
		printf("Received REQ_GAME_LAUNCH\n");
		status = handle_game_launch();
		break;
	case REQ_GAME_JOIN:
		printf("Received REQ_GAME_JOIN\n");
		status = handle_game_join();
		break;
	case REQ_GAME_LEAVE:
		printf("Received REQ_GAME_LEAVE\n");
		status = handle_game_leave();
		break;
	case RSP_GAME_STATE:
		printf("Received RSP_GAME_STATE\n");
		status = handle_game_state();
		break;
	default:
		printf("Error: received unknown opcode\n");
	}

	return status;
}

int main(void)
{
	int opcode;
	char done = 0;
	
	while(!done) {
		if (ggz_read_int(fd, &opcode) < 0)
			done = 1;
		else {
			done = handle_opcode(opcode);
		}
	}

	return 0;
}
