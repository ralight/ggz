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
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>
#include <easysock.h>
#include <errno.h>

#include "ggz.h"
#include "protocols.h"

/* Our storage of the player list */
struct ggz_seat_t* ggz_seats;

/* Local copies of necessary data */
static char* name;
static int fd;
static int seats;

/* IO: Hold the handlers here */
static GGZHandler handlers[5];

/* Initialize data structures*/
int ggz_init(char* game_name)
{
  int len;

  len = strlen(game_name);
  if ( (name = malloc(len+1)) == NULL)
    return -1;
  strcpy(name, game_name);

  /* IO: Init all the handlers
   * use the same len variable for laziness */
  for (len = 0; len < 5; len++)
    handlers[len] = NULL;
  
  return 0;
}


/* Connect to Unix domain socket */
int ggz_connect(void)
{
  int sock, len;
  char* fd_name;
  struct sockaddr_un addr;
  
  if ( (sock = socket(PF_LOCAL, SOCK_STREAM, 0)) < 0)
    return -1;

  len = strlen(name) + strlen(TMPDIR) + 7;
  if ( (fd_name = malloc(len)) == NULL)
    return -1;
  sprintf(fd_name, "%s/%s.%d", TMPDIR, name, getpid());
  
  memset(&addr, 0, sizeof(addr));
  addr.sun_family = AF_LOCAL;
  strcpy(addr.sun_path, fd_name);
  free(fd_name);

  if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    return -1;

  fd = sock;
  ggz_debug("%s game started", name);

  return sock;
}


int ggz_game_launch(void)
{
  int i, len;
  char status = 0;


  if (es_read_int(fd, &seats) < 0)
    return -1;

  len = sizeof(struct ggz_seat_t);
  if ( (ggz_seats = calloc(seats, len)) == NULL)
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


int ggz_player_join(int* p_seat, int* p_fd)
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


int ggz_player_leave(int* p_seat, int* p_fd)
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


void ggz_debug(const char *fmt, ...)
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



int ggz_seats_open(void)
{
  int i, count = 0;
  for (i = 0; i < seats; i++)
    if (ggz_seats[i].assign == GGZ_SEAT_OPEN)
      count++;
  return count;
}


int ggz_seats_num(void)
{
  int i;
  for (i = 0; i < seats; i++)
    if (ggz_seats[i].assign == GGZ_SEAT_NONE)
      break;
  return i;
}


int ggz_seats_bot(void)
{
  int i, count = 0;
  for (i = 0; i < seats; i++)
    if (ggz_seats[i].assign == GGZ_SEAT_BOT)
      count++;
  return count;
}


int ggz_seats_reserved(void)
{
  int i, count = 0;
  for (i = 0; i < seats; i++)
    if (ggz_seats[i].assign == GGZ_SEAT_RESV)
      count++;
  return count;
}


int ggz_seats_human(void)
{
  int i, count = 0;
  for (i = 0; i < seats; i++)
    if (ggz_seats[i].assign >= 0 )
      count++;
  
  return count;
}


int ggz_fd_max(void)
{
  int i, max = fd;
  
  for (i = 0; i < ggz_seats_num(); i++)
    if (ggz_seats[i].fd > max)
      max = ggz_seats[i].fd;
  
  
  return max;
}


int ggz_done(void)
{
  /* FIXME: Should send actual statistics */
  if (es_write_int(fd, REQ_GAME_OVER) < 0
      || es_write_int(fd, 0) < 0)
    return -1;
         
  return 0;
}


void ggz_quit(void)
{
  free(name);
  free(ggz_seats);
}

/* IO: From this point on, Brent isn't guilty anymore */
void ggz_set_handler(unsigned int event, const GGZHandler handler) {
  if (event > 4)
    return;
  handlers[event] = handler;
}

int ggz_main() {
  char game_over = 0;
  int i, fd, status, ggz_sock, fd_max, op, seat;
  fd_set active_fd_set, read_fd_set;

  if ( (ggz_sock = ggz_connect()) < 0)
    return -1;

  FD_ZERO(&active_fd_set);
  FD_SET(ggz_sock, &active_fd_set);

  while (!game_over) {

    read_fd_set = active_fd_set;
    fd_max = ggz_fd_max();
    
    status = select((fd_max+1), &read_fd_set, NULL, NULL, NULL);
    
    if (status <= 0) {
      if (errno == EINTR)
        continue;
      else
        return -1;
    }

    /* Check for message from GGZ server */
    if (FD_ISSET(ggz_sock, &read_fd_set)) {
      if (es_read_int(ggz_sock, &op) < 0)
        return -1;
      switch (op) {

        case REQ_GAME_LAUNCH:
          if (ggz_game_launch() == 0 && handlers[GGZ_EVENT_LAUNCH] != NULL)
            (*handlers[GGZ_EVENT_LAUNCH])(GGZ_EVENT_LAUNCH, NULL);
          break;
        case REQ_GAME_JOIN:
          if (ggz_player_join(&seat, &fd) == 0) {
            FD_SET(fd, &active_fd_set);
            if (handlers[GGZ_EVENT_JOIN] != NULL)
              (*handlers[GGZ_EVENT_JOIN])(GGZ_EVENT_JOIN, &seat);
          }
          break;
        case REQ_GAME_LEAVE:
          if (ggz_player_leave(&seat, &fd) == 0) {
            FD_CLR(fd, &active_fd_set);
            if (handlers[GGZ_EVENT_LEAVE] != NULL)
              (*handlers[GGZ_EVENT_LEAVE])(GGZ_EVENT_LEAVE, &seat);
          }
          break;
        case RSP_GAME_OVER:
          game_over = 1;
          if (handlers[GGZ_EVENT_QUIT] != NULL)
            (*handlers[GGZ_EVENT_QUIT])(GGZ_EVENT_QUIT, NULL);
          break;
      }
    }

    /* Check for message from player */
    for (i = 0; i < ggz_seats_num(); i++) {
      fd = ggz_seats[i].fd;
      if (fd != -1 && FD_ISSET(fd, &read_fd_set)) {
        if (handlers[GGZ_EVENT_PLAYER] != NULL)
          (*handlers[GGZ_EVENT_PLAYER])(GGZ_EVENT_PLAYER, &i);
      }
    }

  }

  ggz_quit();
  return 0;
}



