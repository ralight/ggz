/*	$Id: ggz.c 2215 2001-08-24 04:16:58Z jdorje $	*/
/*
 * File: ggz.c
 * Author: Brent Hendricks
 * Project: GGZ 
 * Date: 3/35/00
 * Desc: GGZ game module functions
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


/* debug level for ggzdmod_debug() */
/* Moved out of ggz_server.h since it's not used outside the library.
 * Imported from ggzd; make sure it stays consistent!
 * A better alternative would be to #include ../../ggzd/err_func.h
 * --JDS */
#define GGZ_DBG_TABLE	(unsigned) 0x00000010


/* Our storage of the player list */
struct ggz_seat_t* ggz_seats=NULL;
int ggzfd = -1;

#define GGZ_SOCKET_FD 3

/* Local copies of necessary data */
static int seats;


/* Connect to GGZ server (ggzd) */
/* It should be safe to keep just this one connect function.
 * No extra manditory data should ever be required to connect, so
 * a lazy server can get started with GGZ quickly.  --JDS */
int ggzdmod_connect(void)
{
	/* TODO: check that the socket is real -
	 * send dummy message, perhaps? */
	ggzfd = GGZ_SOCKET_FD;
	ggzdmod_debug("GGZDMOD: Connected to GGZ server.");
	return ggzfd;
}


int ggzdmod_disconnect(void)
{
	int response;

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
		ggzdmod_debug("GGZDMOD: ERROR: "
			      "Got %d response while waiting for RSP_GAME_OVER.",
			      response);
	}

	ggzdmod_debug("GGZDMOD: Disconnected from GGZ server.");

	/* close the socket */
	close(ggzfd);
	ggzfd = -1;

	if(ggz_seats) free(ggz_seats);
	
	return 0;
}


int ggzdmod_game_launch(void)
{
	int i, len;
	char status = 0;


	if (es_read_int(ggzfd, &seats) < 0)
		return -1;

	len = sizeof(struct ggz_seat_t);
	if ( (ggz_seats = calloc(seats, len)) == NULL)
		return -1;
	
	for (i = 0; i < seats; i++) {
		if (es_read_int(ggzfd, &ggz_seats[i].assign) < 0)
			return -1;
		ggz_seats[i].fd = -1;  /* always set to 0 */
		if (ggz_seats[i].assign == GGZ_SEAT_RESV
		    && es_read_string(ggzfd, ggz_seats[i].name, 
				      (MAX_USER_NAME_LEN+1)))
			return -1;
	}
				      
	for (i = 0; i < seats; i++)
		switch (ggz_seats[i].assign) {
		case GGZ_SEAT_OPEN:
			ggzdmod_debug("Seat %d is open", i);
			break;
		case GGZ_SEAT_BOT:
			ggzdmod_debug("Seat %d is a bot", i);
			strcpy(ggz_seats[i].name, "bot");
			break;
		case GGZ_SEAT_RESV:
			ggzdmod_debug("Seat %d reserved for %s", i, ggz_seats[i].name);
			break;
		}

	if (es_write_int(ggzfd, RSP_GAME_LAUNCH) < 0
	    || es_write_char(ggzfd, status) < 0)
		return -1;

	return 0;
}


int ggzdmod_player_join(int* p_seat, int* p_fd)
{
	int seat;
	char status = 0;
	
	if (es_read_int(ggzfd, &seat) < 0
	    || es_read_string(ggzfd, ggz_seats[seat].name,
			      (MAX_USER_NAME_LEN+1)) < 0
	    || es_read_fd(ggzfd, &ggz_seats[seat].fd) < 0
	    || es_write_int(ggzfd, RSP_GAME_JOIN) < 0
	    || es_write_char(ggzfd, status) < 0)
		return -1;

	ggz_seats[seat].assign = GGZ_SEAT_PLAYER;	
	ggzdmod_debug("%s on %d in seat %d", ggz_seats[seat].name ,ggz_seats[seat].fd, seat);
	
	*p_seat = seat;
	*p_fd = ggz_seats[seat].fd;

	return 0;
}


int ggzdmod_player_leave(int* p_seat, int* p_fd)
{
	int i;
	char status = 0;
	char name[MAX_USER_NAME_LEN+1];
	
	if (es_read_string(ggzfd, name, (MAX_USER_NAME_LEN+1)) < 0)
		return -1;

	for (i = 0; i < seats; i++)
		if (!strcmp(name, ggz_seats[i].name))
			break;
	if (i == seats) /* Player not found */
		status = -1;
	else {
		*p_seat = i;
		*p_fd = ggz_seats[i].fd;
		ggz_seats[i].fd = -1;
		ggz_seats[i].assign = GGZ_SEAT_OPEN;
		status = 0;
		ggzdmod_debug("Removed %s from seat %d", ggz_seats[i].name, i);
	}

	if (es_write_int(ggzfd, RSP_GAME_LEAVE) < 0
	    || es_write_char(ggzfd, status) < 0)
		return -1;

	return status;
}


int ggzdmod_debug(const char *fmt, ...)
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



int ggzdmod_seats_open(void)
{
	int i, count = 0;
	for (i = 0; i < seats; i++)
		if (ggz_seats[i].assign == GGZ_SEAT_OPEN)
			count++;
	return count;
}


int ggzdmod_seats_num(void)
{
	int i;
	for (i = 0; i < seats; i++)
		if (ggz_seats[i].assign == GGZ_SEAT_NONE)
			break;
	return i;
}


int ggzdmod_seats_bot(void)
{
	int i, count = 0;
	for (i = 0; i < seats; i++)
		if (ggz_seats[i].assign == GGZ_SEAT_BOT)
			count++;
	return count;
}


int ggzdmod_seats_reserved(void)
{
	int i, count = 0;
	for (i = 0; i < seats; i++)
		if (ggz_seats[i].assign == GGZ_SEAT_RESV)
			count++;
	return count;
}


int ggzdmod_seats_human(void)
{
	int i, count = 0;
	for (i = 0; i < seats; i++)
		if (ggz_seats[i].assign >= 0 )
			count++;
	
	return count;
}


int ggzdmod_fd_max(void)
{
	int i, max = ggzfd;
	
	for (i = 0; i < ggzdmod_seats_num(); i++)
		if (ggz_seats[i].fd > max)
			max = ggz_seats[i].fd;
	
	
	return max;
}

/*
 * these implement "chess's way", the event-driven interface
 */

/* IO: Hold the handlers here */
/* this is all hard-coded in conjunction with the
 * GGZ_EVENT_*** definitions.  Not good, but it'll do.  --JDS */
static GGZHandler handlers[6] = {NULL, NULL, NULL, NULL, NULL, NULL};

void ggzdmod_set_handler(int event_id, const GGZHandler handler)
{
	/* regular case */
	if (event_id >= sizeof(handlers)/sizeof(handlers[0])
	    || event_id < 0)
		return;
	handlers[event_id] = handler;
}

/* return values as of now (not finalized):
 * 0 => success
 * -1 => can't connect
 * -2 => error during game
 * -3 => error during disconnect
 */
int ggzdmod_main(void)
{
	char game_over = 0;
	int i, fd, status, ggz_sock, fd_max, op, seat;
	fd_set active_fd_set, read_fd_set;

	if ((ggz_sock = ggzdmod_connect()) < 0)
		return -1;

	FD_ZERO(&active_fd_set);
	FD_SET(ggz_sock, &active_fd_set);

	while (!game_over) {

		read_fd_set = active_fd_set;
		fd_max = ggzdmod_fd_max();

		status = select((fd_max + 1), &read_fd_set, NULL, NULL, NULL);

		if (status <= 0) {
			if (errno == EINTR)
				continue;
			else
				return -2;
		}

		/* TODO: errors should be handled within this loop; we should
		 * do a graceful exit on an error. --JDS */

		/* Check for message from GGZ server */
		if (FD_ISSET(ggz_sock, &read_fd_set)) {
			if (es_read_int(ggz_sock, &op) < 0)
				return -2;
			switch (op) {

			case REQ_GAME_LAUNCH:
				if (ggzdmod_game_launch() == 0
				    && handlers[GGZ_EVENT_LAUNCH] != NULL)
					(*handlers[GGZ_EVENT_LAUNCH])
						(GGZ_EVENT_LAUNCH, NULL);
				break;
			case REQ_GAME_JOIN:
				if (ggzdmod_player_join(&seat, &fd) == 0) {
					FD_SET(fd, &active_fd_set);
					if (handlers[GGZ_EVENT_JOIN] != NULL)
						(*handlers[GGZ_EVENT_JOIN])
							(GGZ_EVENT_JOIN,
							 &seat);
				}
				break;
			case REQ_GAME_LEAVE:
				if (ggzdmod_player_leave(&seat, &fd) == 0) {
					FD_CLR(fd, &active_fd_set);
					if (handlers[GGZ_EVENT_LEAVE] != NULL)
						(*handlers[GGZ_EVENT_LEAVE])
							(GGZ_EVENT_LEAVE,
							 &seat);
				}
				break;
			case RSP_GAME_OVER:
				game_over = 1;
				if (handlers[GGZ_EVENT_QUIT] != NULL)
					(*handlers[GGZ_EVENT_QUIT])
						(GGZ_EVENT_QUIT, NULL);
				break;
			}
		}

		/* Check for message from player */
		for (i = 0; i < ggzdmod_seats_num(); i++) {
			fd = ggz_seats[i].fd;
			if (fd != -1 && FD_ISSET(fd, &read_fd_set)) {
				if (handlers[GGZ_EVENT_PLAYER] != NULL)
					(*handlers[GGZ_EVENT_PLAYER])
						(GGZ_EVENT_PLAYER, &i);
			}
		}

		/* A "tick" event is sent once each time through the loop */
		if (handlers[GGZ_EVENT_TICK] != NULL)
			(*handlers[GGZ_EVENT_TICK])(GGZ_EVENT_TICK, NULL);
	}

	if (ggzdmod_disconnect() < 0)
		return -3;

	return 0;
}
