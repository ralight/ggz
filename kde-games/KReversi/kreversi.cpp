/***************************************************************************
                          KReversi.cpp  -  description
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

#include "reversiview.h"
#include "reversiprotocol.h"
#include "kreversi.h"
#include <klocale.h>
#include <kaction.h>
#include <kmessagebox.h>
#include <kstdaction.h>

#define T2S(a) (a==BLACK?0:1)
#define S2T(a) (a==0?BLACK:WHITE)
#define BOARD(x,y) ( (x<0||y<0||x>=8||y>=8)?OPEN:board[x][y] )

KReversi::KReversi(QWidget *parent, const char *name) : KMainWindow(parent, name)
{

  /* Handle the UI */
  // Create the KActions
  KStdAction::quit (kapp, SLOT(closeAllWindows()), actionCollection());
  requestSyncAct = new KAction(i18n("&Request sync"), 0, this, SLOT(requestSync()), actionCollection(), "requestSync");
  playAgainAct = new KAction(i18n("Play again"), 0, this, SLOT(playAgain()), actionCollection(), "playAgain");
  createGUI();

  // Create the status bar
  statusBar()->insertItem(i18n("Wait..."), 1, 1);
  statusBar()->insertItem(i18n("Blue: 2"), 2, 2);
  statusBar()->insertItem(i18n("Red: 2"), 3, 2);

  // Create the game
  initGame();

	// Create the view
	view = new ReversiView(this);
  view->updateBoard(board);

  // Create the protocol
  protocol = new ReversiProtocol();

  // It's the main view
  this->setCentralWidget(view);

	// Init the canvasView
  view->setFixedSize( 50*8, 50*8 );
  view->setVScrollBarMode( QScrollView::AlwaysOff );
  view->setHScrollBarMode( QScrollView::AlwaysOff );

  this->setFixedSize( this->width(), this->height() );

  connect( protocol, SIGNAL(gotSeat(int)), this, SLOT(seatSlot(int)) );
  connect( protocol, SIGNAL(gotPlayers(char *, char *)), this, SLOT(playersSlot(char *, char*)) );
  connect( protocol, SIGNAL(gotSync(char, char *)), this, SLOT(syncSlot(char, char *)) );
  connect( protocol, SIGNAL(gotStart()), this, SLOT(startSlot()) );
  connect( protocol, SIGNAL(gotMove(int)), this, SLOT(moveSlot(int)) );
  connect( protocol, SIGNAL(gotGameover(int)), this, SLOT(gameoverSlot(int)) );

  connect( view, SIGNAL(playerMove(int, int)), this, SLOT(playerMoveSlot(int, int)) );


}

KReversi::~KReversi()
{
}

/** Tell the player what is his color */
void KReversi::seatSlot(int _seat){
  seat = _seat;
  updateScore();
}

/** Tells the player who is his oponnent */
void KReversi::playersSlot(char *_name1, char *_name2){
  p_name[0] = _name1;
  p_name[1] = _name2;
  updateScore();
  playing = 0;
  requestSyncAct->setEnabled(false);
  playAgainAct->setEnabled(false);
}

/** Tell the player that the view has started */
void KReversi::startSlot(){
  QString message;
  moveSlot(-1);
  statusMsg(i18n("Game has started"));
  playing = 1;
  requestSyncAct->setEnabled(true);
  playAgainAct->setEnabled(false);
}

void KReversi::moveSlot(int move) {
  QString message;
  doMove(move);
  if (T2S(turn) == seat)
    message = i18n("Your turn!");
  else
    message = i18n("%1's turn").arg(p_name[T2S(turn)]);
  statusBar()->changeItem(message, 1);
  updateScore();
}

/** Tells the player that the view is over */
void KReversi::gameoverSlot(int winner){
  if (winner == S2T(seat))
    statusMsg(i18n("Congratulations! You won!"));
  else if (winner == -S2T(seat))
    statusMsg(i18n("That's too bad... you lost"));
  else if (winner == OPEN)
    statusMsg(i18n("That game was a draw"));
  playing = 0;
  requestSyncAct->setEnabled(false);
  playAgainAct->setEnabled(true);
  if (KMessageBox::questionYesNo(this, i18n("Do you want to play again?\n\nIf you select \"No\" now, you can request\na new game by selecting the\n\"Game|Play again\" menu"), i18n("Play again?")) == KMessageBox::Yes)
    playAgain();
}

/** Displays a status bar message */
void KReversi::statusMsg(QString msg){
  statusBar()->message(msg, 1200);
}

/** Update the value on the statusbar */
void KReversi::updateScore(){
  QString msg;
  int x, y;
  score[0] = 0;
  score[1] = 0;
  for (y = 0; y < 8; y++) {
    for (x = 0; x < 8; x++) {
      if (board[x][y] == BLACK)
        score[0]++;
      else if (board[x][y] == WHITE)
        score[1]++;
    }
  }
  msg = i18n("%1 (blue): %2").arg(p_name[0]).arg(score[0]);
  statusBar()->changeItem(msg, 2);
  msg = i18n("%1 (red): %2").arg(p_name[1]).arg(score[1]);
  statusBar()->changeItem(msg, 3);
}

void KReversi::playerMoveSlot(int x, int y) {
  if (!playing)
    return;
  if (!isValid(turn, x, y)) {
    statusMsg("Invalid move!");
    return;
  }
  protocol->send(RVR_REQ_MOVE);
  protocol->send(y*8 + x);
}

void KReversi::doMove(int move) {
  int mx, my;
  if (move == RVR_ERROR_INVALIDMOVE || move == RVR_ERROR_WRONGTURN)
    return;
  if (move == RVR_ERROR_CANTMOVE) {
    turn=-turn;
    return;
  }
  mx = move % 8;
  my = move / 8;
  board[mx][my] = turn;
  
  // Mark up
  markBoard(turn, mx, my, 0, -1);
  // Mark down
  markBoard(turn, mx, my, 0, 1);
  // Mark right
  markBoard(turn, mx, my, 1, 0);
  // Mark left
  markBoard(turn, mx, my, -1, 0);
  // Mark diagonal
  markBoard(turn, mx, my, 1, 1);
  markBoard(turn, mx, my, 1, -1);
  markBoard(turn, mx, my, -1, 1);
  markBoard(turn, mx, my, -1, -1);

  view->updateBoard(board);
  /* Has updated, now clean it up */
  for (mx = 0; mx < 8; mx++) {
    for (my = 0; my < 8; my++) {
      if (board[mx][my] > 0)
        board[mx][my] = 1;
      else if (board[mx][my] < 0)
        board[mx][my] = -1;
      else
        board[mx][my] = 0;
    }
  }
  turn = -turn;
}

int KReversi::markBoard(int player, int mx, int my, int dx, int dy) {
  // Recursivaly marks the board
  if (mx+dx < 0 || my+dy < 0 || mx+dx >= 8 || my+dy >= 8)
    return 0;
  int b = board[mx+dx][my+dy];
  if (b == -player || b == -2*player) {
    if (markBoard(player, mx+dx, my+dy, dx, dy)) {
      board[mx+dx][my+dy] = 2*player;
      return 1;
    }
  } else if (b == player || b == 2*player) {
    return 1;
  }

  return 0;
}

void KReversi::syncSlot(char _turn, char *_board) {
  int x, y;
  char *a = _board;
  turn = _turn;
  for (y = 0; y < 8; y++) {
    for (x = 0; x < 8; x++) {
      board[x][y] = *a++;
    }
  }
  moveSlot(-1);
  updateScore();
  statusMsg(i18n("Syncing game!"));
  view->updateBoard(board);
}

void KReversi::requestSync() {
  if (!playing)
    return;
  protocol->send(RVR_REQ_SYNC);
}

void KReversi::initGame() {
  int x, y;
  // Init the board
  for (y = 0; y < 8; y++) {
    for (x = 0; x < 8; x++) {
      board[x][y] = OPEN;
    }
  }
  board[3][3] = WHITE;
  board[4][4] = WHITE;
  board[3][4] = BLACK;
  board[4][3] = BLACK;
  turn = BLACK;
  playing = 0;
  requestSyncAct->setEnabled(false);
  playAgainAct->setEnabled(false);
}

void KReversi::playAgain() {
  if (playing)
    return;
  protocol->send(RVR_REQ_AGAIN);
  initGame();
  view->updateBoard(board);
}

  /* Check if a move is valid */
bool KReversi::isValid(int player, int mx, int my) {
  int dx, dy;
  // Check in all directions
  for (dy = -1; dy <= 1; dy++) {
    for (dx = -1; dx <= 1; dx++) {
      if (BOARD(mx+dx, my+dy) == -player && checkValid(player, mx+dx, my+dy, dx, dy)) {
        debug("*** Good move: %d, %d ***", dx, dy);
        return true;
      }
    }
  }

  return false;
}

bool KReversi::checkValid(int player, int mx, int my, int dx, int dy) {
  int a = BOARD(mx+dx, my+dy);
  if (a == player)
    return true;
  else if (a == -player)
    return checkValid(player, mx+dx, my+dy, dx, dy);
  else if (a == OPEN)
    return false;

  return false;
}