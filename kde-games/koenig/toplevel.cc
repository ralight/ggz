#include <kaction.h>
#include <kstdaction.h>
#include <ktabctl.h>
#include <klistbox.h>

#include <qlayout.h>
#include <qmultilineedit.h>

#include "toplevel.h"
#include "toplevel.moc"

#include "board.h"
#include "options.h"
#include "game.h"

TopLevel::TopLevel(const char *name)
	: KMainWindow(0, name)
{
	chessBoard = NULL;
	game = NULL;

	KAction *a;
	KTabCtl *ctl;

	KStdAction::openNew(this, SLOT(newGame()), actionCollection());
	KStdAction::close(this, SLOT(closeGame()), actionCollection());
	KStdAction::quit(qApp, SLOT(quit()), actionCollection());

	a = new KAction("Quit", "gamequit", 0, qApp, SLOT(quit()), actionCollection());
	a = new KAction("Show moves", "showmovetable", 0, qApp, SLOT(quit()), actionCollection());

	createGUI();

	//newGame();
	initGameSocket();

	ctl = new KTabCtl(this);
	tab1 = new QMultiLineEdit(ctl);
	tab2 = new KListBox(ctl);
	tab2->insertItem("(Koenig launched)");
	ctl->addTab(tab1, "Moves");
	ctl->addTab(tab2, "Messages");
	setCentralWidget(ctl);

	setCaption("Control Panel");
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
	//initGameSocket();
}

void TopLevel::initGameData(void)
{
	chessBoard = new ChessBoard(NULL, "ChessBoard");
	chessBoard->show();
	options = new Options(NULL, "Options");
	options->show();
	connect(options, SIGNAL(signalTime(int)), SLOT(slotTime(int)));
}

void TopLevel::initGameSocket(void)
{
	game = new Game();
	connect(game, SIGNAL(signalNewGame()), SLOT(newGame()));
	connect(game, SIGNAL(signalMessage(QString)), SLOT(slotMessage(QString)));
}

void TopLevel::closeGame(void)
{
	if (chessBoard)
		delete chessBoard;
	chessBoard = NULL;
}

//void TopLevel::handleNetInput(void)
//{
//}

void TopLevel::slotTime(int time)
{
	game->setTime(time);
}

void TopLevel::slotMessage(QString msg)
{
	tab2->insertItem(msg);
	options->hide();
}

