#include <kaction.h>
#include <kstdaction.h>
#include <ktabctl.h>
#include <klistbox.h>
#include <kmessagebox.h>

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

	//KStdAction::openNew(this, SLOT(newGame()), actionCollection()); // don't handle standalone games yet
	KStdAction::close(this, SLOT(closeGame()), actionCollection());
	KStdAction::quit(qApp, SLOT(quit()), actionCollection());

	a = new KAction("Quit", "gamequit", 0, qApp, SLOT(quit()), actionCollection());
	a = new KAction("Show moves", "showmovetable", 0, qApp, SLOT(quit()), actionCollection());

	createGUI();

	//newGame(); // don't start yet
	//initGameSocket(); // done by --ggz

	ctl = new KTabCtl(this);
	tab1 = new QMultiLineEdit(ctl);
	tab2 = new KListBox(ctl);
	tab2->insertItem("(Koenig launched)");
	ctl->addTab(tab2, "Messages");
	ctl->addTab(tab1, "Moves");
	setCentralWidget(ctl);

	resize(400, 200);
	setCaption("Control Panel");

	chessBoard = new ChessBoard(NULL, "ChessBoard");
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
	if(game) connect(chessBoard, SIGNAL(figureMoved(int, int, int, int)), game, SLOT(slotMove(int, int, int, int)));
	options = new Options(NULL, "Options");
	options->show();
	connect(options, SIGNAL(signalTime(int)), SLOT(slotTime(int)));
}

void TopLevel::initGameSocket(void)
{
	game = new Game();
	connect(game, SIGNAL(signalNewGame()), SLOT(newGame()));
	connect(game, SIGNAL(signalMessage(QString)), SLOT(slotMessage(QString)));
	connect(game, SIGNAL(signalMove(QString)), SLOT(slotMove(QString)));
	connect(game, SIGNAL(signalStart(int)), SLOT(slotStart(int)));
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
	if(game) game->setTime(time);
	options->hide();
}

void TopLevel::slotMessage(QString msg)
{
	tab2->insertItem(msg);
	//KMessageBox::information(this, QString("The server said:\n") + msg, "Message from chess server"); // FIXME: OPTIONAL!
}

void TopLevel::slotMove(QString msg)
{
	tab1->append(msg);
}

void TopLevel::slotStart(int seat)
{
	chessBoard->resetBoard((seat ? COLOR_BLACK : COLOR_WHITE));
}

