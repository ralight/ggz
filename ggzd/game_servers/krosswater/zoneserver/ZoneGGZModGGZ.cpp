#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>
#include <easysock.h>
#include <ZoneGGZModGGZ.h>
#include <errno.h>
#include <ZoneProtocols.h>

#include <iostream>

char* ZoneGGZModGGZ::name;
int ZoneGGZModGGZ::fd;
int ZoneGGZModGGZ::seats;
struct ggz_seat_t* ZoneGGZModGGZ::ggz_seats;

/* Initialize data structures*/
int ZoneGGZModGGZ::ggz_init(char* game_name)
{
	int len;

	len = strlen(game_name);
	if ((name = (char*)malloc(len+1)) == NULL)
		return -1;
	strcpy(name, game_name);

	cout << "Game name: " << name << endl;

	return 0;
}


/* Connect to Unix domain socket */
int ZoneGGZModGGZ::ggz_connect(void)
{
	int sock, len;
	char* fd_name;
	struct sockaddr_un addr;

	if((sock = socket(PF_LOCAL, SOCK_STREAM, 0)) < 0)
	{
		return -1;
	}

	len = strlen(name) + strlen(TMPDIR) + 16;
	if((fd_name = (char*)malloc(len)) == NULL)
	{
		return -1;
	}
	sprintf(fd_name, "%s/%s.%d", TMPDIR, name, getpid());

printf("Creating game: %s (%s)\n", name, fd_name);
	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_LOCAL;
	strcpy(addr.sun_path, fd_name);
	free(fd_name);

	if(::connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
	{
printf("Error! Could not bind!\n");
printf("Reason: (%s)\n", strerror(errno));
		return -1;
	}

	fd = sock;
	ggz_debug("%s game started", name);

	return sock;
}


int ZoneGGZModGGZ::ggz_game_launch(void)
{
	int i, len;
	char status = 0;


	if (es_read_int(fd, &seats) < 0)
		return -1;

	len = sizeof(struct ggz_seat_t);
	if ( (ggz_seats = (struct ggz_seat_t*)calloc(seats, len)) == NULL)
		return -1;

	for (i = 0; i < seats; i++) {
		if (es_read_int(fd, &ggz_seats[i].assign) < 0)
			return -1;
		ggz_seats[i].fd = -1;  /* always set to 0 */
		if (ggz_seats[i].assign == GGZ_SEAT_RESV
		    && es_read_string(fd, ggz_seats[i].name,
				      (MAX_USER_NAME_LEN+1)))
			return -1;
	}

	for (i = 0; i < seats; i++)
		switch (ggz_seats[i].assign) {
		case GGZ_SEAT_OPEN:
			ggz_debug("Seat %d is open", i);
			break;
		case GGZ_SEAT_BOT:
			ggz_debug("Seat %d is a bot", i);
			strcpy(ggz_seats[i].name, "bot");
			break;
		case GGZ_SEAT_RESV:
			ggz_debug("Seat %d reserved for %s", i,
				    ggz_seats[i].name);
			break;
		}

	if (es_write_int(fd, RSP_GAME_LAUNCH) < 0
	    || es_write_char(fd, status) < 0)
		return -1;

	return 0;
}


int ZoneGGZModGGZ::ggz_player_join(int* p_seat, int* p_fd)
{
	int seat;
	char status = 0;

	if (es_read_int(fd, &seat) < 0
	    || es_read_string(fd, ggz_seats[seat].name,
			      (MAX_USER_NAME_LEN+1)) < 0
	    || es_read_fd(fd, &ggz_seats[seat].fd) < 0
	    || es_write_int(fd, RSP_GAME_JOIN) < 0
	    || es_write_char(fd, status) < 0)
		return -1;

	ggz_seats[seat].assign = GGZ_SEAT_PLAYER;
	ggz_debug("%s on %d in seat %d", ggz_seats[seat].name
		  ,ggz_seats[seat].fd, seat);

	*p_seat = seat;
	*p_fd = ggz_seats[seat].fd;

	return 0;
}


int ZoneGGZModGGZ::ggz_player_leave(int* p_seat, int* p_fd)
{
	int i;
	char status = 0;
	char name[MAX_USER_NAME_LEN+1];

	if (es_read_string(fd, name, (MAX_USER_NAME_LEN+1)) < 0)
		return -1;

	for (i = 0; i < 4; i++)
		if (!strcmp(name, ggz_seats[i].name))
			break;
	if (i == 4) /* Player not found */
		status = -1;
	else {
		*p_seat = i;
		*p_fd = ggz_seats[i].fd;
		ggz_seats[i].fd = -1;
		ggz_seats[i].assign = GGZ_SEAT_OPEN;
		status = 0;
		ggz_debug("Removed %s from seat %d", ggz_seats[i].name, i);
	}

	if (es_write_int(fd, RSP_GAME_LEAVE) < 0
	    || es_write_char(fd, status) < 0)
		return -1;

	return status;
}


void ZoneGGZModGGZ::ggz_debug(const char *fmt, ...)
{
	char buf[4096];
	va_list ap;

	va_start(ap, fmt);
        vsprintf(buf, fmt, ap);
	es_write_int(fd, MSG_DBG);
	es_write_int(fd, GGZ_DBG_TABLE);
	es_write_string(fd, buf);
	va_end(ap);
}



int ZoneGGZModGGZ::ggz_seats_open(void)
{
	int i, count = 0;
	for (i = 0; i < seats; i++)
		if (ggz_seats[i].assign == GGZ_SEAT_OPEN)
			count++;
	return count;
}


int ZoneGGZModGGZ::ggz_seats_num(void)
{
	int i;
	for (i = 0; i < seats; i++)
		if (ggz_seats[i].assign == GGZ_SEAT_NONE)
			break;
	return i;
}


int ZoneGGZModGGZ::ggz_seats_bot(void)
{
	int i, count = 0;
	for (i = 0; i < seats; i++)
		if (ggz_seats[i].assign == GGZ_SEAT_BOT)
			count++;
	return count;
}


int ZoneGGZModGGZ::ggz_seats_reserved(void)
{
	int i, count = 0;
	for (i = 0; i < seats; i++)
		if (ggz_seats[i].assign == GGZ_SEAT_RESV)
			count++;
	return count;
}


int ZoneGGZModGGZ::ggz_seats_human(void)
{
	int i, count = 0;
	for (i = 0; i < seats; i++)
		if (ggz_seats[i].assign >= 0 )
			count++;

	return count;
}


int ZoneGGZModGGZ::ggz_fd_max(void)
{
	int i, max = fd;

	for (i = 0; i < ggz_seats_num(); i++)
		if (ggz_seats[i].fd > max)
			max = ggz_seats[i].fd;


	return max;
}


int ZoneGGZModGGZ::ggz_done(void)
{
	/* FIXME: Should send actual statistics */
	if (es_write_int(fd, REQ_GAME_OVER) < 0
	    || es_write_int(fd, 0) < 0)
		return -1;
	       
	return 0;
}


void ZoneGGZModGGZ::ggz_quit(void)
{
	free(name);
	free(ggz_seats);
}
