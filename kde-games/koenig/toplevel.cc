#include <kaction.h>
#include <kstdaction.h>
#include <klistbox.h>
#include <kmessagebox.h>

#include <qlayout.h>
#include <qmultilineedit.h>

#include "toplevel.h"
#include "toplevel.moc"

#include "boardcontainer.h"
#include "options.h"
#include "game.h"
#include "kexttabctl.h"

TopLevel::TopLevel(const char *name)
	: KMainWindow(0, name)
{
	chessBoard = NULL;
	game = NULL;

	KAction *a;

	//KStdAction::openNew(this, SLOT(newGame()), actionCollection()); // don't handle standalone games yet
	KStdAction::close(this, SLOT(closeGame()), actionCollection());
	KStdAction::quit(qApp, SLOT(quit()), actionCollection());

	a = new KAction("Quit", "gamequit", 0, qApp, SLOT(quit()), actionCollection());
	a = new KAction("Show moves", "showmovetable", 0, qApp, SLOT(quit()), actionCollection());

	a = new KAction("Request sync", "gamesync", 0, this, SLOT(slotSync()), actionCollection());
	a = new KAction("Call flag", "gameflag", 0, this, SLOT(slotFlag()), actionCollection());
	a = new KAction("Request draw", "gamedraw", 0, this, SLOT(slotDraw()), actionCollection()); // HUH? how does this work?

	createGUI();

	//newGame(); // don't start yet
	//initGameSocket(); // done by --ggz

	ctl = new KExtTabCtl(this);
	tab1 = new QMultiLineEdit(ctl);
	tab2 = new KListBox(ctl);
	tab2->insertItem("(Koenig launched)");
	ctl->addTab(tab2, "Messages");
	ctl->addTab(tab1, "Moves");
	setCentralWidget(ctl);

	resize(400, 200);
	setCaption("Control Panel");

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
	options = new Options(NULL, "Options");
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
	chessBoard->root()->resetBoard((seat ? COLOR_BLACK : COLOR_WHITE));
}

void TopLevel::slotSync()
{
}

void TopLevel::slotFlag()
{
}

void TopLevel::slotDraw()
{
	// FIXME: merge with slotNetDraw
	game->answerDraw(1); // LOL
}

void TopLevel::slotNetDraw()
{
	int answer;

	answer = KMessageBox::questionYesNo(this,
		"The other player requests a draw. Do you agree?", "Draw requested");
	game->answerDraw(answer);
}

void TopLevel::slotDoMove(int x, int y, int x2, int y2)
{
	chessBoard->root()->moveFigure(x, y, x2, y2);
}

