/***************************************************************************
                          reversiprotocol.cpp  -  description
                             -------------------
    begin                : Fri Mar 9 2001
    copyright            : (C) 2001 by Ismael Orenstein
    email                : perdig@linuxbr.com.br
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "reversiprotocol.h"
#include "network.h"

#include <stdlib.h>
#include <netinet/in.h>

#include <ggz_common.h>

ReversiProtocol::ReversiProtocol() {
  args.setAutoDelete( true );

  next = INT;

  sock = NULL;
  network = new Network();
  connect(network, SIGNAL(signalData()), SLOT(readBuffer()));
  network->doconnect();
};

ReversiProtocol::~ReversiProtocol() {
  /* Clean up the buffer */
  //readBuffer();
  delete network;
};

void ReversiProtocol::readBuffer() {
  void *arg = NULL;
  int *a;
  if(!sock) {
	sock = network->socket();
  }
  int old_size = sock->bytesAvailable();
  switch (next) {
    case CHAR:
      if (sock->bytesAvailable() >= 1) {
        arg = malloc(1);
        if (sock->readBlock((char *)arg, 1) < 1)
          return;
        next = 0;
        args.append(arg);
        debug("Got char: %d", *(char *)arg);
        handleNewArg();
      }
      break;
    case INT:
      if (sock->bytesAvailable() >= 4) {
        arg = malloc(4);
        if (sock->readBlock((char *)arg, 4) < 1)
          return;
        next = 0;
        a = (int *)malloc(sizeof(int));
        *a = ntohl( *(int*)arg );
        args.append(a);
        free(arg);
        debug("Got int: %d", *a);
        handleNewArg();
      }
      break;
    case STRING:
      if (sock->bytesAvailable() >= 5) {
        // Get size
        arg = malloc(4);
        if (sock->readBlock((char *)arg, 4) < 1)
          return;        
        next = 0;
        a = (int *)malloc(sizeof(int));
        *a = ntohl( *(int*)arg );
        debug("Got size: %d", *a);
        next = *a;
        free(a);
        free(arg);
      }
      break;
    default:
      if (next > 0 && (signed int)sock->bytesAvailable() >= next) {
        arg = malloc(next);
        if (sock->readBlock((char *)arg, next) < 1)
          return;
        next = 0;
        args.append(arg);
        debug("Got buffer: %s", (char *)arg);
        handleNewArg();
      } else {
        debug("Got data, but don't know what to do with it!");
      }
      break;
  }
  if ( (old_size - sock->bytesAvailable()) > 0 )
    // Still got data !
    readBuffer();
}

void ReversiProtocol::handleNewArg() {
  void *a, *b, *c, *d;
  debug("First: %p", args.first());
  while ( (a = args.next()) != NULL)
    debug("Next: %p", a);
  switch (*(int *)args.first()) {
    case RVR_MSG_SEAT:
      debug("Got a msg seat!");
      // Try to get the SEAT number
      a = args.next();
      if (!a) {
        // Ok, there is no SEAT number. Ask for it
        debug("Asking for a seat number");
        next = INT;
        return;
      }
      // Have a seat number
      debug("Seat number: %d", *(int *)a);
      emit gotSeat(*(int*)a);
      // Clean up the args
      args.clear();
      next = INT;
      break;
    case RVR_MSG_PLAYERS:
      debug("Got a msg players!");
      // Try to get the player1 id
      a = args.next();
      if (!a) {
        debug("Asking for player1_id");
        next = INT;
        return;
      }
      // What to do ?
      if (*(int*)a != GGZ_SEAT_OPEN) {
        // Try to get player1 string
        b = args.next();
        if (!b) {
          debug("Asking for player1_str");
          next = STRING;
          return;
        }
      } else {
        // No need for player1 string
        b = malloc(5);
        strcpy( (char*)b, "open");
      }
      // Now try to get player2 id
      c = args.next();
      if (!c) {
        debug("Asking for player2_id");
        next = INT;
        if (*(int*)a == GGZ_SEAT_OPEN)
          free(b);
        return;
      }
      // What will be d ?
      if (*(int*)c != GGZ_SEAT_OPEN) {
        d = args.next();
        if (!d) {
          debug("Asking for player2_str");
          next = STRING;
          if (*(int*)a == GGZ_SEAT_OPEN)
            free(b);
          return;
        }
      } else {
        d = malloc(5);
        strcpy( (char*)d, "open" );
      }
      // Ok, done !
      debug("%d->%s, %d->%s", *(int*)a, (char*)b, *(int*)c, (char*)d );
      emit gotPlayers( (char*)b, (char*)d );
      args.clear();
      next = INT;
      if (*(int*)a == GGZ_SEAT_OPEN)
        free(b);
      if (*(int*)c == GGZ_SEAT_OPEN)
        free(d);
      break;
    case RVR_MSG_SYNC:
      debug("Got a MSG_SYNC");
      // Trying to get the turn
      a = args.next();
      if (!a) {
        debug("Requesting turn");
        next = CHAR;
        return;
      }
      // Trying to get the board
      b = args.next();
      if (!b) {
        debug("Resquesting board");
        next = 64;
        return;
      }
      // Ok, got everything
      debug("Turn: %d, Board: %p", *(char*)a, b);
      emit gotSync( *(char*)a, (char *)b );
      next = INT;
      args.clear();
      break;
    case RVR_MSG_START:
      debug("Got a MSG_START");
      // No args... this is a easy one
      emit gotStart();
      next = INT;
      args.clear();
      break;
    case RVR_MSG_MOVE:
      debug("Got a MSG_MOVE");
      a = args.next();
      if (!a) {
        debug("Requesting move");
        next = INT;
        return;
      }
      debug("Move: %d", *(int*)a);
      emit gotMove( *(int*)a );
      next = INT;
      args.clear();
      break;
    case RVR_MSG_GAMEOVER:
      debug("Got a MSG_GAMEOVER");
      a = args.next();
      if (!a) {
        debug("Requesting winner");
        next = INT;
        return;
      }
      debug("Winner: %d", *(int*)a);
      emit gotGameover( *(int*)a );
      next = INT;
      args.clear();
      break;
  }
}

void ReversiProtocol::send(char val) {
  sock->writeBlock(&val, 1);
}

void ReversiProtocol::send(int val) {
  val = htonl(val);
  sock->writeBlock((char *)&val, 4);
}
