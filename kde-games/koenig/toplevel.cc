// Koenig - KDE client for the GGZ chess game
// Copyright (C) 2001 Tobias König, tokoe82@yahoo.de
// Copyright (C) 2001, 2002 Josef Spillner, dr_maux@users.sourceforge.net
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#include <kaction.h>
#include <kstdaction.h>
#include <klistbox.h>
#include <klistview.h>
#include <kmessagebox.h>
#include <klocale.h>
#include <klistview.h>

#include <qlayout.h>
#include <qmultilineedit.h>

#include "toplevel.h"
#include "toplevel.moc"

#include "boardcontainer.h"
#include "options.h"
#include "game.h"
#include "kexttabctl.h"

TopLevel::TopLevel(const char *name)
: KMainWindow(NULL, name)
{
	KListViewItem *tmp;
	KAction *a;

	chessBoard = NULL;
	game = NULL;

	//KStdAction::openNew(this, SLOT(newGame()), actionCollection()); // don't handle standalone games yet
	KStdAction::close(this, SLOT(closeGame()), actionCollection());
	KStdAction::quit(this, SLOT(close()), actionCollection());

	a = new KAction(i18n("Quit"), "gamequit", 0, qApp, SLOT(quit()), actionCollection());
	a = new KAction(i18n("Show moves"), "showmovetable", 0, qApp, SLOT(quit()), actionCollection());

	a = new KAction(i18n("Request sync"), "warnmessage", 0, this, SLOT(slotSync()), actionCollection(), "gamesync");
	a = new KAction(i18n("Call flag"), "warnmessage", 0, this, SLOT(slotFlag()), actionCollection(), "gameflag");
	a = new KAction(i18n("Request draw"), "warnmessage", 0, this, SLOT(slotDraw()), actionCollection(), "gamedraw");

	a = new KAction(i18n("Show warnings as message boxes"), "warnmessage", 0, this, SLOT(slotWarnmessages()), actionCollection(), "optionwarnmessages");
	a = new KAction(i18n("Display board frame"), "warnmessage", 0, this, SLOT(slotBoardframe()), actionCollection(), "optionboardframe");

	a = new KAction(i18n("Show move table"), "warnmessage", 0, this, SLOT(slotMoveTable()), actionCollection(), "showmovetable");
	a = new KAction(i18n("Show chess board"), "warnmessage", 0, this, SLOT(slotChessBoard()), actionCollection(), "showchessboard");

	createGUI();

	//newGame(); // don't start yet
	//initGameSocket(); // done by --ggz

	ctl = new KExtTabCtl(this);
	tab1 = new QMultiLineEdit(ctl);
	tab2 = new KListBox(ctl);
	tab2->insertItem(i18n("(Koenig launched)"));
	tab3 = new KListView(ctl);
	tab3->setRootIsDecorated(true);
	tab3->addColumn(i18n("Koenig Highscores"));
	tmp = new KListViewItem(tab3, i18n("Local scores"));
	tmp = new KListViewItem(tab3, i18n("Worldwide"));
	ctl->addTab(tab2, i18n("Messages"));
	ctl->addTab(tab1, i18n("Moves"));
	ctl->addTab(tab3, i18n("Highscores"));
	setCentralWidget(ctl);

	resize(400, 200);
	setCaption(i18n("Control Panel"));

	chessBoard = new ChessBoardContainer(NULL, "ChessBoardContainer");
	chessBoard->show();
}

TopLevel::~TopLevel(void)
{
	if (chessBoard)
		delete chessBoard;
	chessBoard = NULL;

	if (game)
		delete game;
	game = NULL;
}

void TopLevel::newGame(void)
{
	initGameData();
	//initGameSocket(); // taken over by --ggz events
}

void TopLevel::initGameData(void)
{
	//chessBoard = new ChessBoard(NULL, "ChessBoard"); // can't be here, else only player 0 would see it
	//chessBoard->show();
	//if(game) connect(chessBoard, SIGNAL(figureMoved(int, int, int, int)), game, SLOT(slotMove(int, int, int, int)));
	options = new Options(NULL, i18n("Options"));
	options->show();
	connect(options, SIGNAL(signalTime(int, int)), SLOT(slotTime(int, int)));
}

void TopLevel::initGameSocket(void)
{
	game = new Game();
	connect(game, SIGNAL(signalNewGame()), SLOT(newGame()));
	connect(game, SIGNAL(signalMessage(QString)), SLOT(slotMessage(QString)));
	connect(game, SIGNAL(signalMove(QString)), SLOT(slotMove(QString)));
	connect(game, SIGNAL(signalDoMove(int, int, int, int)), SLOT(slotDoMove(int, int, int, int)));
	connect(game, SIGNAL(signalStart(int)), SLOT(slotStart(int)));
	connect(game, SIGNAL(signalDraw()), SLOT(slotNetDraw()));

	connect(chessBoard->root(), SIGNAL(figureMoved(int, int, int, int)), game, SLOT(slotMove(int, int, int, int)));
}

void TopLevel::closeGame(void)
{
	if (chessBoard)
		delete chessBoard;
	chessBoard = NULL;
}

void TopLevel::slotTime(int timeoption, int time)
{
	if(game) game->setTime(timeoption, time);
	options->hide();
}

void TopLevel::slotMessage(QString msg)
{
	tab2->insertItem(msg);
	//KMessageBox::information(this, QString("The server said:\n") + msg, "Message from chess server"); // FIXME: OPTIONAL!
	tab2->ensureCurrentVisible();
	ctl->showTab(0);
}

void TopLevel::slotMove(QString msg)
{
	tab1->append(msg);
	tab1->setCursorPosition(32000, 0); // FIXME!
	ctl->showTab(1);
}

void TopLevel::slotStart(int seat)
{
	chessBoard->root()->resetBoard((seat ? ChessBoard::color_black : ChessBoard::color_white));
}

void TopLevel::slotSync()
{
}

void TopLevel::slotFlag()
{
}

void TopLevel::slotDraw()
{
	if(game)
		game->answerDraw(1);
}

void TopLevel::slotNetDraw()
{
	int answer;

	answer = KMessageBox::questionYesNo(this,
		i18n("The other player requests a draw. Do you agree?"), i18n("Draw requested"));
	if(answer == KMessageBox::Yes)
		game->answerDraw(1);
	else
		game->answerDraw(0);
}

void TopLevel::slotDoMove(int x, int y, int x2, int y2)
{
	chessBoard->root()->moveFigure(x, y, x2, y2);
}

void TopLevel::slotWarnmessages()
{
}

void TopLevel::slotBoardframe()
{
}

void TopLevel::slotMoveTable()
{
}

void TopLevel::slotChessBoard()
{
	if(chessBoard->isVisible()) chessBoard->hide();
	else chessBoard->show();
}

bool TopLevel::queryClose()
{
	int ret;

	ret = KMessageBox::questionYesNo(this, i18n("Do you really want to quit?"), i18n("Quit game"));

	if(ret == KMessageBox::Yes) return true;
	return false;
}

