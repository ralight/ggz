/*
 * File: ggz.c
 * Author: Brent Hendricks
 * Project: GGZ 
 * Date: 3/35/00
 * Desc: GGZ game module functions
 * $Id: ggz.c 2645 2001-11-04 01:57:23Z jdorje $
 *
 * Copyright (C) 2000 Brent Hendricks.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */

#include <config.h>

#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/un.h>

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <easysock.h>

#include "ggz_server.h"
#include "../../ggzdmod/protocol.h"


/* debug level for ggzd_debug() */
/* Moved out of ggz_server.h since it's not used outside the library.
 * Imported from ggzd; make sure it stays consistent!
 * A better alternative would be to #include ../../ggzd/err_func.h
 * --JDS */

/* The maximum length of a player name.  Does not include trailing \0. */
#define MAX_USER_NAME_LEN 16


/* Our storage of the player list */
struct ggzd_seat_t {
	ggzd_assign_t assign;	
	char name[MAX_USER_NAME_LEN +1];
	int fd;			
};

/* Local copies of necessary data */
static int ggzfd = -1;
static int gameover = 0;
static int gameover_status = 0;
static int num_seats = 0;
static struct ggzd_seat_t *seat_data = NULL;
static fd_set active_fd_set; /* set of active file descriptors */


/* These are accessor functions for seat data
 * right now, there's a lot of debugging code.  This will be cleaned
 * up later. */

ggzd_assign_t ggzd_get_seat_status(int seat)
{
	if (seat < 0 || seat >= num_seats) {
		ggzd_debug("GGZDMOD: ggzd_get_seat_status: invalid seat %d queried.", seat);
		return GGZ_SEAT_NONE;
	} else
		return seat_data[seat].assign;
}


int ggzd_set_seat_status(int seat, ggzd_assign_t status)
{
	int opcode, result;
	char *name;

	/* Make sure we can handle the request */
	if (seat < 0
	    || seat >= num_seats
	    || (seat_data[seat].assign != GGZ_SEAT_OPEN
		&& seat_data[seat].assign != GGZ_SEAT_BOT)) {
		ggzd_debug("GGZDMOD: ggzd_set_seat_status: invalid seat %d/%d accessed.", seat, status);
	}

	/* Now see if it's okay with GGZD. */
	if (es_write_int(ggzfd, REQ_SEAT_CHANGE) < 0 ||
	    es_write_int(ggzfd, seat) < 0 ||
	    es_write_int(ggzfd, status) < 0 ||
	    es_read_int(ggzfd, &opcode) < 0 ||
	    opcode != RSP_SEAT_CHANGE ||
	    es_read_int(ggzfd, &result) < 0 ||
	    result < 0)  {
		ggzd_debug("GGZDMOD: ggzd_set_seat_status: server won't set seat %d to %d.", seat, status);
		return -1;
	}

	/* It's okay so make the change. */
	seat_data[seat].assign = status;
	switch (status) {
		case GGZ_SEAT_BOT:
			name = "Bot";
			break;
		default:
			name = "";
			break;
	}
	snprintf(seat_data[seat].name, MAX_USER_NAME_LEN+1, "%s", name);

	return 0;
}

const char* ggzd_get_player_name(int seat)
{
	if (seat < 0
	    || seat >= num_seats
	    || seat_data[seat].assign == GGZ_SEAT_NONE) {
		ggzd_debug("GGZDMOD: ggzd_get_player_name: invalid seat %d queried.", seat);
		return "[out-of-range seat]";
	}
	return seat_data[seat].name;
}

int ggzd_set_player_name(int seat, char* name)
{
	/* you can set player names, but only for Bots
	 * which have artificial names anyway */
	if (!name
	    || seat < 0
	    || seat >= num_seats
	    || seat_data[seat].assign != GGZ_SEAT_BOT) {
		ggzd_debug("GGZDMOD: ggzd_set_player_name: invalid seat %d named.", seat);
		return -1;
	}

	strncpy(seat_data[seat].name, name, MAX_USER_NAME_LEN);
	return 0;
}

int ggzd_get_player_socket(int seat)
{
	if (seat < 0
	    || seat >= num_seats
	    || seat_data[seat].assign == GGZ_SEAT_NONE) {
		ggzd_debug("GGZDMOD: ggzd_get_player_socket: invalid seat %d queried.", seat);
		return -1;
	}

	return seat_data[seat].fd;
}

int ggzd_get_player_udp_socket(int seat)
{
	return -1; /* TODO: this isn't implemented yet */
}


/* Connect to GGZ server (ggzd) */
/* It should be safe to keep just this one connect function.
 * No extra manditory data should ever be required to connect, so
 * a lazy server can get started with GGZ quickly.  --JDS */
int ggzd_connect(void)
{
	/* GGZD sets us up with FD 3 as a usable
	 * socket.  If only it were always this easy... */
	ggzfd = 3;

	/* we test if the socket works by sending this message. */
	/* FIXME: is this legit?  Will it work w/o debugging? */
	if (ggzd_debug("GGZDMOD: Connected to GGZ server.") < 0) {
		ggzfd = -1;
		return -1;
	}

	/* add the main GGZ FD to our FD set */
	FD_ZERO(&active_fd_set);
	FD_SET(ggzfd, &active_fd_set);

	/* the game's not over, obviously... */
	gameover = 0;

	return ggzfd;
}


int ggzd_disconnect(void)
{
	int response, p;

	/* first send a gameover message (request) */
	if (es_write_int(ggzfd, REQ_GAME_OVER) < 0)
		return -1;

	/* The server will send a message in response; we should wait for it. */
	if (es_read_int(ggzfd, &response) < 0)
		return -1;
	if (response != RSP_GAME_OVER) {
		/* what else can we do? */
		ggzd_debug("GGZDMOD: ERROR: "
			      "Got %d response while waiting for RSP_GAME_OVER.",
			      response);
	}

	ggzd_debug("GGZDMOD: Disconnected from GGZ server.");

	/* close all file descriptors */
	for (p = 0; p < ggzd_seats_num(); p++)
		if (seat_data[p].fd != -1)
			close(seat_data[p].fd);
	close(ggzfd);
	ggzfd = -1;
	FD_ZERO(&active_fd_set);

	free(seat_data);
	
	return 0;
}


/* If anyone is left out or would prefer a different name, go right
 * ahead and change it.  No longer than 13 characters.  --JDS */
static char* bot_names[] = {
	"bcox",     "Crouton", "Boffo", "Bugg",  "DJH",    "Dobey",
	"Dr. Maux", "jDorje",  "Jzaun", "Oojah", "Perdig", "RGade",
	"riq",      "rpd"
};

/* This function randomizes the order of the names assigned to bots.
 * It is ENTIRELY UNNECESSARY but entertaining.  --JDS */
static void randomize_names(char** names,
			    char** randnames,
			    int num)
{
	char* rnames2[num];
	int i, choice;

	/* copy names array to rnames2 array */
	for (i=0; i<num; i++) rnames2[i] = names[i];

	/* now pick names from rnames2 */
	for(i=0; i<num; i++) {
		choice = random() % (num-i);
		randnames[i] = rnames2[choice];
		rnames2[choice] = rnames2[num-i-1];
	}
}


static int ggzdmod_game_launch(void)
{
	int i, status = 0, bots = 0;
#define NUM_BOT_NAMES (sizeof(bot_names)/sizeof(bot_names[0]))
	char* rand_bot_names[NUM_BOT_NAMES];

	randomize_names(bot_names, rand_bot_names, NUM_BOT_NAMES);

	if (es_read_int(ggzfd, &num_seats) < 0)
		return -1;
	if (num_seats <= 0) {
		ggzd_debug("GGZDMOD: ERROR: %d seats sent upon launch.", num_seats);
		return -1;
	}

	seat_data = calloc(num_seats, sizeof(*seat_data));
	if (seat_data == NULL)
		return -1;
	
	for (i = 0; i < num_seats; i++) {
		if (es_read_int(ggzfd, &seat_data[i].assign) < 0)
			return -1;
		seat_data[i].fd = -1;
		if (seat_data[i].assign == GGZ_SEAT_RESV
		    && es_read_string(ggzfd, seat_data[i].name,
				      (MAX_USER_NAME_LEN+1)))
			return -1;
	}
				      
	for (i = 0; i < num_seats; i++)
		switch (seat_data[i].assign) {
		case GGZ_SEAT_OPEN:
			ggzd_debug("GGZDMOD: Seat %d is open", i);
			break;
		case GGZ_SEAT_BOT:
			/* FIXME: we should truncate the name not the AI */
			snprintf(seat_data[i].name, MAX_USER_NAME_LEN+1, "%s-AI", rand_bot_names[bots]);
			ggzd_debug("GGZDMOD: Seat %d is a bot named %s", i, rand_bot_names[bots]);
			bots++;
			break;
		case GGZ_SEAT_RESV:
			ggzd_debug("GGZDMOD: Seat %d reserved for %s", i, seat_data[i].name);
			break;
		default: /* prevent compiler warning */
			/* We ignore other values (?)  --JDS */
		}

	if (es_write_int(ggzfd, RSP_GAME_LAUNCH) < 0
	    || es_write_char(ggzfd, status) < 0)
		return -1;

	return 0;
}


static int ggzdmod_player_join(int* p_seat, int* p_fd)
{
	int seat;
	char status = 0;
	
	if (es_read_int(ggzfd, &seat) < 0
	    || es_read_string(ggzfd, seat_data[seat].name,
			      (MAX_USER_NAME_LEN+1)) < 0
	    || es_read_fd(ggzfd, &seat_data[seat].fd) < 0
	    || es_write_int(ggzfd, RSP_GAME_JOIN) < 0
	    || es_write_char(ggzfd, status) < 0)
		return -1;

	seat_data[seat].assign = GGZ_SEAT_PLAYER;	
	ggzd_debug("%s on %d in seat %d", seat_data[seat].name, seat_data[seat].fd, seat);

	FD_SET(seat_data[seat].fd, &active_fd_set);

	*p_seat = seat;
	*p_fd = seat_data[seat].fd;

	return 0;
}


static int ggzdmod_player_leave(int* p_seat, int* p_fd)
{
	int i;
	char status = 0;
	char name[MAX_USER_NAME_LEN+1];
	
	if (es_read_string(ggzfd, name, (MAX_USER_NAME_LEN+1)) < 0)
		return -1;

	for (i = 0; i < num_seats; i++)
		if (!strcmp(name, seat_data[i].name))
			break;
	if (i == num_seats) /* Player not found */
		status = -1;
	else {
		*p_seat = i;
		*p_fd = seat_data[i].fd;
		FD_CLR(*p_fd, &active_fd_set);
		seat_data[i].fd = -1;
		seat_data[i].assign = GGZ_SEAT_OPEN;
		status = 0;
		ggzd_debug("Removed %s from seat %d", seat_data[i].name, i);
	}

	if (es_write_int(ggzfd, RSP_GAME_LEAVE) < 0
	    || es_write_char(ggzfd, status) < 0)
		return -1;

	return status;
}


int ggzd_debug(const char *fmt, ...)
{
	char buf[4096];
	va_list ap;
	int status = 0;

	va_start(ap, fmt);
	vsnprintf(buf, sizeof(buf), fmt, ap);
	va_end(ap);

	if (ggzfd >= 0) {
		if (es_write_int(ggzfd, MSG_LOG) < 0 ||
		    es_write_string(ggzfd, buf) < 0)
			status = -1;
	} else {
		/* We could store the buffer to be sent later, but
		 * this should be good enough. */
		fprintf(stderr, "%s\n", buf);
	}
	return status;
}


int ggzd_get_seats_num(void)
{
	int i;
	for (i = 0; i < num_seats; i++)
		if (seat_data[i].assign == GGZ_SEAT_NONE)
			break;
	return i;
}


int ggzd_get_seat_count(ggzd_assign_t status)
{
	int i, count = 0;
	for (i = 0; i < num_seats; i++)
		if (seat_data[i].assign == status)
			count++;
	return count;
}


static int ggzd_fd_max(void)
{
	/* FIXME: why not just track this regularly instead of
	 * recalculating it each time? */
	int i, max = ggzfd;
	
	for (i = 0; i < ggzd_seats_num(); i++)
		if (seat_data[i].fd > max)
			max = seat_data[i].fd;
	
	
	return max;
}


void ggzd_gameover(int status)
{
	ggzd_debug("GGZDMOD: gameover called with status %d.", status);
	gameover = 1;
	gameover_status = status;
}


int ggzd_get_gameover(void)
{
	return gameover;
}

/*
 * Event-driven interface by Perdig
 */

/* IO: Hold the handlers here */
/* this is all hard-coded in conjunction with the
 * GGZ_EVENT_*** definitions.  Not good, but it'll do.  --JDS */
static GGZDHandler handlers[6] = {NULL, NULL, NULL, NULL, NULL, NULL};

void ggzd_set_handler(ggzd_event_t event_id, const GGZDHandler handler)
{
	/* regular case */
	if (event_id >= sizeof(handlers)/sizeof(handlers[0])
	    || event_id < 0) {
		ggzd_debug("GGZDMOD: ERROR: handler registered for bad event %d.", event_id);
		return;
	}
	handlers[event_id] = handler;
}


void ggzd_dispatch(void)
{
        int op, seat, fd;

	if (es_read_int(ggzfd, &op) < 0) {
		ggzd_gameover(-1);
		return;
	}

	switch (op) {
		case REQ_GAME_LAUNCH:
			if (ggzdmod_game_launch() == 0
			    && handlers[GGZ_EVENT_LAUNCH] != NULL)
				(*handlers[GGZ_EVENT_LAUNCH])
					(GGZ_EVENT_LAUNCH, NULL);
			break;
		case REQ_GAME_JOIN:
			if (ggzdmod_player_join(&seat, &fd) == 0) {
				if (handlers[GGZ_EVENT_JOIN] != NULL)
					(*handlers[GGZ_EVENT_JOIN])
						(GGZ_EVENT_JOIN,
						 &seat);
			}
			break;
		case REQ_GAME_LEAVE:
			if (ggzdmod_player_leave(&seat, &fd) == 0) {
				if (handlers[GGZ_EVENT_LEAVE] != NULL)
					(*handlers[GGZ_EVENT_LEAVE])
						(GGZ_EVENT_LEAVE,
						 &seat);
			}
			break;
		case RSP_GAME_OVER:
			ggzd_gameover(0);
			if (handlers[GGZ_EVENT_QUIT] != NULL)
				(*handlers[GGZ_EVENT_QUIT])
					(GGZ_EVENT_QUIT, NULL);
			break;
	}
}


/* FIXME: this hasn't really been tested.  --JDS */
int ggzd_io_is_pending(void)
{
	fd_set read_fd_set = active_fd_set;
	int status;
	struct timeval timeout;

	/* is this really portable? */
	timeout.tv_sec = 0;
	timeout.tv_usec = 0;

	status = select(ggzd_fd_max() + 1,
			&read_fd_set,
			NULL, NULL, &timeout); /* FIXME */
	if (status <= 0) {
		if (errno != EINTR)
			ggzd_gameover(-1);
	}

	/* the return value indicates the #
	 * of FD's with pending data */
	return (status > 0);	
}


void ggzd_io_read_all(void)
{
	fd_set read_fd_set;
	int i, fd, status;

	read_fd_set = active_fd_set;

	/* TODO: We may want to just poll and return.  Polling is
	 * useful for real-time games (easier than using
	 * threads).  --JDS */
	/* note - we have to select so that we can determine what file
	 * descriptors are waiting to be read. */
	status = select(ggzd_fd_max() + 1,
			&read_fd_set,
			NULL, NULL, NULL);
	if (status <= 0) {
		if (errno != EINTR)
			ggzd_gameover(-1);
		return;
	}

	status = 0;

	/* Check for message from GGZ server */
	if (FD_ISSET(ggzfd, &read_fd_set))
		ggzd_dispatch();

	/* Check for message from player */
	for (i = 0; i < ggzd_seats_num(); i++) {
		fd = seat_data[i].fd;
		if (fd != -1
		    && FD_ISSET(fd, &read_fd_set)
		    && handlers[GGZ_EVENT_PLAYER] != NULL) {
			(*handlers[GGZ_EVENT_PLAYER])
				(GGZ_EVENT_PLAYER, &i);
		}
	}
}


/* return values as of now (not finalized):
 * 0 => success
 * -1 => can't connect
 */
int ggzd_main_loop(void)
{
	if (ggzd_connect() < 0)
		return -1;

	do
		ggzd_io_read_all();
	while (!gameover);

	(void)ggzd_disconnect();

	return 0;
}
