/*
 * File: ggz.c
 * Author: Brent Hendricks
 * Project: GGZ 
 * Date: 3/35/00
 * Desc: GGZ game module functions
 * $Id: ggz.c 2253 2001-08-25 23:53:19Z jdorje $
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
#include "ggz_protocols.h"


/* debug level for ggzd_debug() */
/* Moved out of ggz_server.h since it's not used outside the library.
 * Imported from ggzd; make sure it stays consistent!
 * A better alternative would be to #include ../../ggzd/err_func.h
 * --JDS */
#define GGZ_DBG_TABLE	(unsigned) 0x00000010


/* Our storage of the player list */
struct ggzd_seat_t* ggzd_seats=NULL;
int ggzfd = -1;

/* Local copies of necessary data */
static int seats;

/* the set of active GGZ file descriptors */
static fd_set active_fd_set;


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

	return ggzfd;
}


int ggzd_disconnect(void)
{
	int response, p;

	/* first send a gameover message (request) */
	if (es_write_int(ggzfd, REQ_GAME_OVER) < 0)
		return -1;

	/* now send the game's statistics */
	/* FIXME: Should send actual statistics */
	if (es_write_int(ggzfd, 0) < 0)
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
		if (ggzd_seats[p].fd != -1)
			close(ggzd_seats[p].fd);
	close(ggzfd);
	ggzfd = -1;
	FD_ZERO(&active_fd_set);

	free(ggzd_seats);
	
	return 0;
}


static int ggzdmod_game_launch(void)
{
	int i, status = 0;


	if (es_read_int(ggzfd, &seats) < 0)
		return -1;

	ggzd_seats = calloc(seats, sizeof(*ggzd_seats));
	if (ggzd_seats == NULL)
		return -1;
	
	for (i = 0; i < seats; i++) {
		if (es_read_int(ggzfd, &ggzd_seats[i].assign) < 0)
			return -1;
		ggzd_seats[i].fd = -1;  /* always set to 0 */
		if (ggzd_seats[i].assign == GGZ_SEAT_RESV
		    && es_read_string(ggzfd, ggzd_seats[i].name,
				      (MAX_USER_NAME_LEN+1)))
			return -1;
	}
				      
	for (i = 0; i < seats; i++)
		switch (ggzd_seats[i].assign) {
		case GGZ_SEAT_OPEN:
			ggzd_debug("GGZDMOD: Seat %d is open", i);
			break;
		case GGZ_SEAT_BOT:
			ggzd_debug("GGZDMOD: Seat %d is a bot", i);
			strcpy(ggzd_seats[i].name, "bot");
			break;
		case GGZ_SEAT_RESV:
			ggzd_debug("GGZDMOD: Seat %d reserved for %s", i, ggzd_seats[i].name);
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
	    || es_read_string(ggzfd, ggzd_seats[seat].name,
			      (MAX_USER_NAME_LEN+1)) < 0
	    || es_read_fd(ggzfd, &ggzd_seats[seat].fd) < 0
	    || es_write_int(ggzfd, RSP_GAME_JOIN) < 0
	    || es_write_char(ggzfd, status) < 0)
		return -1;

	ggzd_seats[seat].assign = GGZ_SEAT_PLAYER;	
	ggzd_debug("%s on %d in seat %d", ggzd_seats[seat].name, ggzd_seats[seat].fd, seat);

	FD_SET(ggzd_seats[seat].fd, &active_fd_set);

	*p_seat = seat;
	*p_fd = ggzd_seats[seat].fd;

	return 0;
}


static int ggzdmod_player_leave(int* p_seat, int* p_fd)
{
	int i;
	char status = 0;
	char name[MAX_USER_NAME_LEN+1];
	
	if (es_read_string(ggzfd, name, (MAX_USER_NAME_LEN+1)) < 0)
		return -1;

	for (i = 0; i < seats; i++)
		if (!strcmp(name, ggzd_seats[i].name))
			break;
	if (i == seats) /* Player not found */
		status = -1;
	else {
		*p_seat = i;
		*p_fd = ggzd_seats[i].fd;
		FD_CLR(*p_fd, &active_fd_set);
		ggzd_seats[i].fd = -1;
		ggzd_seats[i].assign = GGZ_SEAT_OPEN;
		status = 0;
		ggzd_debug("Removed %s from seat %d", ggzd_seats[i].name, i);
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
	if (es_write_int(ggzfd, MSG_DBG) < 0 ||
	    es_write_int(ggzfd, GGZ_DBG_TABLE) < 0 ||
	    es_write_string(ggzfd, buf) < 0)
		status = -1;
	va_end(ap);
	return status;
}



int ggzd_seats_open(void)
{
	int i, count = 0;
	for (i = 0; i < seats; i++)
		if (ggzd_seats[i].assign == GGZ_SEAT_OPEN)
			count++;
	return count;
}


int ggzd_seats_num(void)
{
	int i;
	for (i = 0; i < seats; i++)
		if (ggzd_seats[i].assign == GGZ_SEAT_NONE)
			break;
	return i;
}


int ggzd_seats_bot(void)
{
	int i, count = 0;
	for (i = 0; i < seats; i++)
		if (ggzd_seats[i].assign == GGZ_SEAT_BOT)
			count++;
	return count;
}


int ggzd_seats_reserved(void)
{
	int i, count = 0;
	for (i = 0; i < seats; i++)
		if (ggzd_seats[i].assign == GGZ_SEAT_RESV)
			count++;
	return count;
}


int ggzd_seats_human(void)
{
	int i, count = 0;
	for (i = 0; i < seats; i++)
		if (ggzd_seats[i].assign >= 0 )
			count++;
	
	return count;
}


int ggzd_fd_max(void)
{
	int i, max = ggzfd;
	
	for (i = 0; i < ggzd_seats_num(); i++)
		if (ggzd_seats[i].fd > max)
			max = ggzd_seats[i].fd;
	
	
	return max;
}

/*
 * these implement "chess's way", the event-driven interface
 */

/* IO: Hold the handlers here */
/* this is all hard-coded in conjunction with the
 * GGZ_EVENT_*** definitions.  Not good, but it'll do.  --JDS */
static GGZDHandler handlers[6] = {NULL, NULL, NULL, NULL, NULL, NULL};

void ggzd_set_handler(ggzd_event_t event_id, const GGZDHandler handler)
{
	/* regular case */
	if (event_id >= sizeof(handlers)/sizeof(handlers[0])
	    || event_id < 0)
		return;
	handlers[event_id] = handler;
}

/* return value:
 *  0 => normal
 *  1 => game over
 *  -1 => error
 */
int ggzd_dispatch(void)
{
        int op, seat, fd, gameover = 0;

	if (es_read_int(ggzfd, &op) < 0)
		return -1;

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
			gameover = 1;
			if (handlers[GGZ_EVENT_QUIT] != NULL)
				(*handlers[GGZ_EVENT_QUIT])
					(GGZ_EVENT_QUIT, NULL);
			break;
	}

	return gameover;
}

/* return value:
 * 0 => normal; game goes on
 * 1 => gameover
 * -1 => error
 */
int ggzd_read_data(void)
{
	fd_set read_fd_set;
	int i, fd, status;
	int gameover = 0;

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
		if (errno == EINTR)
			return 0;
		else
			return -1;
	}

	/* Check for message from GGZ server */
	if (FD_ISSET(ggzfd, &read_fd_set))
		status = ggzd_dispatch();
	if (status < 0)
		return -1;
	else if (status > 0)
		gameover = 1;

	/* Check for message from player */
	for (i = 0; i < ggzd_seats_num(); i++) {
		fd = ggzd_seats[i].fd;
		if (fd != -1 && FD_ISSET(fd, &read_fd_set)) {
			if (handlers[GGZ_EVENT_PLAYER] != NULL)
				(*handlers[GGZ_EVENT_PLAYER])
					(GGZ_EVENT_PLAYER, &i);
		}
	}

	/* A "tick" event is sent once each time through the loop */
	if (handlers[GGZ_EVENT_TICK] != NULL)
		(*handlers[GGZ_EVENT_TICK])(GGZ_EVENT_TICK, NULL);

	return gameover;
}

/* return values as of now (not finalized):
 * 0 => success
 * -1 => can't connect
 * -2 => error during game loop
 * -3 => error during disconnect
 */
int ggzd_main(void)
{
	int status = 0;

	if (ggzd_connect() < 0)
		return -1;

	while (status == 0)
		status = ggzd_read_data();

	if (status < 0)
		return -2;

	if (ggzd_disconnect() < 0)
		return -3;

	return 0;
}
