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

TopLevel::TopLevel(const char *name)
	: KMainWindow(0, name)
{
	chessBoard = 0;
	KAction *a;
	KTabCtl *ctl;
	QMultiLineEdit *tab1;
	KListBox *tab2;

	KStdAction::openNew(this, SLOT(newGame()), actionCollection());
	KStdAction::close(this, SLOT(closeGame()), actionCollection());
	KStdAction::quit(qApp, SLOT(quit()), actionCollection());

	a = new KAction("Quit", "gamequit", 0, qApp, SLOT(quit()), actionCollection());
	a = new KAction("Show moves", "showmovetable", 0, qApp, SLOT(quit()), actionCollection());

	createGUI();

	newGame();

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
	options = new Options(NULL, "Options");
	options->show();
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

