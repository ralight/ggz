#include <kaction.h>
#include <kstdaction.h>

#include <qlayout.h>

#include "toplevel.h"
#include "toplevel.moc"

#include "board.h"

TopLevel::TopLevel(const char *name)
	: KMainWindow(0, name)
{
	chessBoard = 0;
	KAction *a;

	KStdAction::openNew(this, SLOT(newGame()), actionCollection());
	KStdAction::close(this, SLOT(closeGame()), actionCollection());
	KStdAction::quit(qApp, SLOT(quit()), actionCollection());

	a = new KAction("Quit", "gamequit", 0, qApp, SLOT(quit()), actionCollection());
	a = new KAction("Show moves", "showmovetable", 0, qApp, SLOT(quit()), actionCollection());

	createGUI();

	newGame();

	setCaption("Control Panel");
}

TopLevel::~TopLevel(void)
{
	if (chessBoard)
		delete chessBoard;

	chessBoard = 0;
}

void TopLevel::newGame(void)
{
	initGameData();
}

void TopLevel::initGameData(void)
{
	chessBoard = new ChessBoard(0, "ChessBoard");
	chessBoard->show();
}

void TopLevel::initGameSocket(void)
{
}

void TopLevel::closeGame(void)
{
	if (chessBoard)
		delete chessBoard;

	chessBoard = 0;
}

void TopLevel::handleNetInput(void)
{
}

