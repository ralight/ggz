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

#include "config.h"
#include "reversiview.h"
#include "reversiprotocol.h"
#include "kreversi.h"
#include <klocale.h>
#include <kconfig.h>
#include <kaction.h>
#include <kmessagebox.h>
#include <dirent.h>
#include <kstdaction.h>
#include <kapplication.h>

#define T2S(a) (a==BLACK?0:1)
#define S2T(a) (a==0?BLACK:WHITE)
#define BOARD(x,y) ( (x<0||y<0||x>=8||y>=8)?OPEN:board[x][y] )

int select_dirs(const struct dirent *d);

KReversi::KReversi(QWidget *parent, const char *name) : KMainWindow(parent, name)
{
  // Create the protocol
  protocol = new ReversiProtocol();

  /* Handle the UI */
  // Create the KActions
  KStdAction::quit (kapp, SLOT(closeAllWindows()), actionCollection());
  requestSyncAct = new KAction(i18n("&Request sync"), 0, this, SLOT(requestSync()), actionCollection(), "requestSync");
  playAgainAct = new KAction(i18n("Play again"), 0, this, SLOT(playAgain()), actionCollection(), "playAgain");
  themes = new KActionMenu(i18n("Select theme"), actionCollection(), "themes");
  createGUI();

}

int KReversi::initAll() {
  char mboard[8][8];
  int x, y;

  // Create the game
  initGame();


  if (scanThemeDir() < 0)
    return -1;

	// Create the view
	view = new ReversiView(kapp->config()->readEntry("Default", "default"), this);

  if (!view->disc_img)
    return -1;

  for (x = 0; x < 8; x++) {
    for (y = 0; y < 8; y++) {
      mboard[x][y] = board[x][y];
    }
  }
  maskBoard(turn, mboard);
  view->updateBoard(mboard);

  // Create the status bar
  statusBar()->insertItem(i18n("Wait..."), 1, 1);
  statusBar()->insertItem(view->getPlayer(0) + ": 2", 2, 2);
  statusBar()->insertItem(view->getPlayer(1) + ": 2", 3, 2);

  // It's the main view
  this->setCentralWidget(view);

	// Init the canvasView
  view->setFixedSize( 50*8, 50*8 );
  view->setVScrollBarMode( QScrollView::AlwaysOff );
  view->setHScrollBarMode( QScrollView::AlwaysOff );

  this->setFixedSize( this->width(), this->height() );

  // Connect the protocol actions
  connect( protocol, SIGNAL(gotSeat(int)), this, SLOT(seatSlot(int)) );
  connect( protocol, SIGNAL(gotPlayers(char *, char *)), this, SLOT(playersSlot(char *, char*)) );
  connect( protocol, SIGNAL(gotSync(char, char *)), this, SLOT(syncSlot(char, char *)) );
  connect( protocol, SIGNAL(gotStart()), this, SLOT(startSlot()) );
  connect( protocol, SIGNAL(gotMove(int)), this, SLOT(moveSlot(int)) );
  connect( protocol, SIGNAL(gotGameover(int)), this, SLOT(gameoverSlot(int)) );

  // Connect the view actions
  connect( view, SIGNAL(playerMove(int, int)), this, SLOT(playerMoveSlot(int, int)) );

  this->show();

  return 0;

}

KReversi::~KReversi() {
  if (view)
    delete view;
  if (protocol)
    delete protocol;
}

int KReversi::scanThemeDir() {
  struct dirent **namelist;
  KRadioAction *theme_act;
  QString default_theme;
  int i;

  QString theme_dir(GGZDATADIR "/kreversi/pixmaps/");
  /* Set the config group for the desired theme */
  kapp->config()->setGroup("Themes");
  default_theme = kapp->config()->readEntry("Default", "default");

  theme_num = scandir(theme_dir, &namelist, select_dirs, alphasort);
  if (theme_num <= 0) {
    /* Couldn't find any theme! */
    KMessageBox::error(this, i18n("Couldn't find any theme in " + theme_dir +" ! Check your installation!"));
    return -1;
  }
  for (i = 0; i < theme_num; i++) {
    theme_act = new KRadioAction(namelist[i]->d_name, 0, this, SLOT(changeTheme()), actionCollection(), namelist[i]->d_name);
    theme_act->setExclusiveGroup("theme_group");
    theme_list.append( theme_act );
    themes->insert( theme_act );
    if (default_theme == namelist[i]->d_name)
      theme_act->setChecked(true);
  }

  return theme_num;

}

void KReversi::changeTheme() {
  KRadioAction *theme_act;
  for (theme_act = theme_list.first(); theme_act; theme_act = theme_list.next()) {
    if (theme_act->isChecked()) {
      view->loadTheme(theme_act->name());
      kapp->config()->setGroup("Themes");
      kapp->config()->writeEntry("Default", theme_act->name());
      kapp->config()->sync();
      updateScore();
    }
  }
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
  if (KMessageBox::questionYesNo(this, i18n("Do you want to play again? If you select \"No\" now, you can request a new game by selecting the \"Game|Play again\" menu"), i18n("Play again?")) == KMessageBox::Yes)
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
  msg = i18n("%1 (%2): %3").arg(p_name[0]).arg(view->getPlayer(0)).arg(score[0]);
  statusBar()->changeItem(msg, 2);
  msg = i18n("%1 (%2): %3").arg(p_name[1]).arg(view->getPlayer(1)).arg(score[1]);
  statusBar()->changeItem(msg, 3);
}

void KReversi::playerMoveSlot(int x, int y) {
  if (!playing)
    return;
  if (turn != S2T(seat)) {
    statusMsg("That's not your turn!");
    return;
  }
  if (!isValid(turn, x, y)) {
    statusMsg("Invalid move!");
    return;
  }
  statusBar()->changeItem("Sending move...", 1);
  protocol->send(RVR_REQ_MOVE);
  protocol->send(y*8 + x);
}

void KReversi::doMove(int move) {
  int mx, my;
  int x, y;
  /* mboard is the mask of the board, that is sent to ReversiView */
  char mboard[8][8];
  if (move == RVR_ERROR_INVALIDMOVE || move == RVR_ERROR_WRONGTURN)
    return;
  /* Translate board */
  for (x = 0; x < 8; x++) {
    for (y = 0; y < 8; y++) {
      switch(board[x][y]) {
        case OPEN:
          mboard[x][y] = VIEW_OPEN;
          break;
        case BLACK:
          mboard[x][y] = VIEW_BLACK;
          break;
        case WHITE:
          mboard[x][y] = VIEW_WHITE;
          break;
      }
    }
  }
  if (move == RVR_ERROR_CANTMOVE) {
    turn=-turn;
		maskBoard(turn, mboard);
		view->updateBoard(mboard);
    return;
  }
  mx = move % 8;
  my = move / 8;
  board[mx][my] = turn;
  mboard[mx][my] = 3*turn;
  // Mark up
  markBoard(turn, mx, my, 0, -1, mboard);
  // Mark down
  markBoard(turn, mx, my, 0, 1, mboard);
  // Mark right
  markBoard(turn, mx, my, 1, 0, mboard);
  // Mark left
  markBoard(turn, mx, my, -1, 0, mboard);
  // Mark diagonal
  markBoard(turn, mx, my, 1, 1, mboard);
  markBoard(turn, mx, my, 1, -1, mboard);
  markBoard(turn, mx, my, -1, 1, mboard);
  markBoard(turn, mx, my, -1, -1, mboard);

  maskBoard(-turn, mboard);

  /* Update the board mask */
  view->updateBoard(mboard);
  turn = -turn;
}

void KReversi::maskBoard(int player, char board[8][8]) {
  int x, y;
  /* Check possible moves */
  for (x = 0; x < 8; x++) {
    for (y = 0; y < 8; y++) {
      if (isValid(player, x, y))
        board[x][y] = VIEW_POSSIBLE;
    }
  }
}


int KReversi::markBoard(int player, int mx, int my, int dx, int dy, char mboard[8][8]) {
  // Recursivaly marks the board
  if (mx+dx < 0 || my+dy < 0 || mx+dx >= 8 || my+dy >= 8)
    return 0;
  int b = board[mx+dx][my+dy];
  if (b == -player) {
    if (markBoard(player, mx+dx, my+dy, dx, dy, mboard)) {
      board[mx+dx][my+dy] = player;
      mboard[mx+dx][my+dy] = 2*player;
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
  char mboard[8][8];
  turn = _turn;
  for (y = 0; y < 8; y++) {
    for (x = 0; x < 8; x++) {
      board[x][y] = *a++;
      mboard[x][y] = board[x][y];
    }
  }
  moveSlot(-1);
  updateScore();
  statusMsg(i18n("Syncing game!"));
  maskBoard(turn, mboard);
  view->updateBoard(mboard);
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
  int x, y;
  char mboard[8][8];
  if (playing)
    return;
  protocol->send(RVR_REQ_AGAIN);
  initGame();
  for (x = 0; x < 8; x++) {
    for (y = 0; y < 8; y++) {
      mboard[x][y] = board[x][y];
    }
  }
  maskBoard(turn, mboard);
  view->updateBoard(mboard);
}

  /* Check if a move is valid */
bool KReversi::isValid(int player, int mx, int my) {
  int dx, dy;
  /* Check if it's open */
  if (BOARD(mx, my) != OPEN)
    return false;
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
