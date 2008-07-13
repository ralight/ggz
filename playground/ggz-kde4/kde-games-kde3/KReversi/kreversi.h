/***************************************************************************
                          kreversi.h  -  description
                             -------------------
    begin                : Wed Mar  7 19:16:57 /etc/localtime 2001
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

#ifndef KREVERSI_H
#define KREVERSI_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <kapplication.h>
#include <kmainwindow.h>
#include <qcanvas.h>
#include <kstatusbar.h>
#include <kaction.h>
#include <qstringlist.h>
#include <qlayout.h>
#include <qptrlist.h>

class ReversiView;
class ReversiProtocol;


/** KReversi is the base class of the project */
class KReversi : public KMainWindow
{
Q_OBJECT 
public:
    /** construtor */
    KReversi(QWidget* parent=0, const char *name=0);
    /** destructor */
    ~KReversi();
    /** Create the view */
    int initAll();
  /** Displays a status bar message */
  void statusMsg(QString);
  /** Update the value on the statusbar */
  void updateScore();
  /* Check if a move is valid */
  bool isValid(int player, int mx, int my);
private: // Private attributes
  /** Handles the game stuff */
  ReversiView *view;
  /** Handles the protocol */
  ReversiProtocol *protocol;
  /** Game variables: seat number */
  int seat;
  /** Game variables: player name */
  QString p_name[2];
  /** Game variables: current turn */
  int turn;
  /** Game variables: score */
  int score[2];
  /** Game variables: board */
  char board[8][8];
  /** Playing */
  int playing;
  /** Actions: */
  KAction *requestSyncAct;
  KAction *playAgainAct;
  KActionMenu *themes;
  /** Themes */
  int theme_num;
  QPtrList<KRadioAction> theme_list;
  /** Init the board */
  void initGame();
  /** do a move */
  void doMove(int move);
  /** mark the board */
  int markBoard(int player, int mx, int my, int dx, int dy, char mboard[8][8]);
  /** Decoreate the board for ReversiView */
  void maskBoard(int player, char board[8][8]);
  /** Check for validity */
  bool checkValid(int player, int mx, int my, int dx, int dy);
  /** Search for themes */
  int scanThemeDir();
protected slots: // Protected slots
  /** Tells the player that the game is over */
  void gameoverSlot(int winner);
  /** Tell the player that the game has started */
  void startSlot();
  /** Change the current theme */
  void changeTheme();
  /** Tells the player who is his oponnent */
  void playersSlot(char *_name1, char *_name2);
  /** Tell the player what is his color */
  void seatSlot(int seat);
  /** Send move to server */
  void playerMoveSlot(int x, int y);
  /** Update the turn msg */
  void moveSlot(int move);
  /** Got sync */
  void syncSlot(char, char *);
  /** Request sync */
  void requestSync();
  /** Play again */
  void playAgain();
};

#endif
