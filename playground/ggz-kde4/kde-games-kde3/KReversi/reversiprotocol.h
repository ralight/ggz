/***************************************************************************
                          reversiprotocol.h  -  description
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

#ifndef REVERSIPROTOCOL_H
#define REVERSIPROTOCOL_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <qobject.h>
#include <qsocket.h>
#include <qptrlist.h>

#define CHAR -1
#define INT -2
#define STRING -3

#define RVR_MSG_SEAT int(0)
#define RVR_MSG_PLAYERS int(1)
#define RVR_MSG_SYNC int(6)
#define RVR_REQ_SYNC int(7)
#define RVR_REQ_AGAIN int(8)
#define RVR_MSG_START int(5)
#define RVR_MSG_MOVE int(2)
#define RVR_MSG_GAMEOVER int(3)
#define RVR_REQ_MOVE int(4)

#define RVR_ERROR_INVALIDMOVE int(-1)
#define RVR_ERROR_WRONGTURN int(-2)
#define RVR_ERROR_CANTMOVE int(-3)

/**This class abstracts the protocol, using just a 
  *signal/slot mechanism to comunicate with the client
  *@author Ismael Orenstein
  */

class Network;

class ReversiProtocol : public QObject {
Q_OBJECT
public:
  ReversiProtocol();
  ~ReversiProtocol();
public:
  void send(int val);
  void send(char val);
private:
  int next;
  QSocket *sock;
  Network *network;
  QPtrList<void> args;
  void handleNewArg();
signals:
  void gotSeat(int seatNumber);
  void gotPlayers(char *player1, char *player2);
  void gotSync(char turn, char *board);
  void gotStart();
  void gotMove(int move);
  void gotGameover(int winner);
private slots:
  void readBuffer();
};



#endif

