
#include <stdio.h>
#include <stdlib.h>
#include <easysock.h>

int fd = 3;

typedef enum {
	REQ_GAME_LAUNCH,	/**< sent on game launch */
	REQ_GAME_JOIN,		/**< sent on player join */
	REQ_GAME_LEAVE,		/**< sent on player leave */
	RSP_GAME_OVER,		/**< sent in response to a gameover */
	RSP_SEAT_CHANGE,	/**< response to REQ_SEAT_CHANGE */
} ControlToTable;

typedef enum {
	RSP_GAME_LAUNCH,	/**< sent in response to a game launch */
	RSP_GAME_JOIN,		/**< sent in response to a player join */
	RSP_GAME_LEAVE,		/**< sent in response to a player leave */
	MSG_LOG,		/**< a message to log */
	REQ_GAME_OVER,		/**< sent to tell of a game-over */
} TableToControl;

typedef enum {
	GGZ_SEAT_OPEN = -1,	   /**< The seat is open (unoccupied). */
	GGZ_SEAT_BOT = -2,	   /**< The seat has a bot (AI) in it. */
	GGZ_SEAT_RESV = -3,	   /**< The seat is reserved for a player. */
	GGZ_SEAT_NONE = -4,	   /**< This seat does not exist. */
	GGZ_SEAT_PLAYER = -5	   /**< The seat has a regular player in it. */
} GGZdModSeat;


int handle_game_launch()
{
	int i, num;
	char name[1024];
	GGZdModSeat type;
	
	es_read_int(fd, &num);
	printf("Receiving %d seats\n", num);
	
	for (i = 0; i < num; i++) {
		if (es_read_int(fd, &type) < 0)
			return -1;

		switch (type) {
		case GGZ_SEAT_OPEN:
			printf("Received open seat\n");
			break;
		case GGZ_SEAT_BOT: 
			printf("Received bot seat\n");
			break;
		case GGZ_SEAT_RESV: 
			printf("Received reserved seat\n");
			break;
		case GGZ_SEAT_NONE: 
			printf("Received none seat\n");
			break;
		case GGZ_SEAT_PLAYER:
			printf("Received player seat\n");
			break;

		}	
		if (type == GGZ_SEAT_RESV && es_read_string(fd, name, 1024) < 0) {
			printf("Error reading reserved player name\n");
			return -1;
		}
	}

	es_write_int(fd, RSP_GAME_LAUNCH);
	es_write_char(fd, 0);

	
	return 0;
}


int handle_game_join(void)
{
	int num, pfd;
	char *name;

	if (es_read_int(fd, &num) < 0
	    || es_read_string_alloc(fd, &name) < 0
	    || es_read_fd(fd, &pfd) < 0) {
		printf("Error reading from GGZ\n");
		return -1;
	}
	
	printf("Received %s on %d in seat %d\n", name, pfd, num);
	free(name);
	
	if (es_write_int(fd, RSP_GAME_JOIN) < 0
	    || es_write_char(fd, 0)) {
		printf("Error sending data to GGZ\n");
		return -1;
	}

	return 0;
}


int handle_game_leave(void)
{
	char *name;

	if (es_read_string_alloc(fd, &name) < 0) {
		printf("Error reading from GGZ\n");
		return -1;
	}
	
	printf("Removing %s\n", name);

	if (es_write_int(fd, RSP_GAME_LEAVE) < 0 
	    || es_write_char(fd, 0) < 0) {
		printf("Error sending data to GGZ\n");
		return -1;
	}

	return 0;
}


int handle_game_over()
{
	return -1;
}


int handle_seat_change()
{
	return -1;
}


int handle_opcode(int opcode)
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
	case RSP_GAME_OVER:
		printf("Received REQ_GAME_OVER\n");
		status = handle_game_over();
		break;
	case RSP_SEAT_CHANGE:
		printf("Received REQ_SEAT_CHANGE\n");
		status = handle_seat_change();
		break;
	default:
		printf("Error: received unknown opcode\n");
	}

	return 0;
}

int main(void)
{
	int opcode;
	char done = 0;
	
	while(!done) {
		if (es_read_int(fd, &opcode) < 0)
			done = 1;
		else {
			done = handle_opcode(opcode);
		}
	}

	return 0;
}
