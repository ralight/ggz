#include "toplevel.h"
#include "board.h"
#include <kpopupmenu.h>
#include <kmenubar.h>

Toplevel::Toplevel()
: KMainWindow()
{
	Board *board;
	KPopupMenu *menu_game, *menu_theme;

	menu_game = new KPopupMenu(this);
	menu_game->insertItem("New", menugamenew);
	menu_game->insertSeparator();
	menu_game->insertItem("Quit", menugamequit);

	menu_theme = new KPopupMenu(this);
	menu_theme->insertItem("Theme 1", menutheme1);
	menu_theme->insertItem("Theme 2", menutheme2);
	menu_theme->insertItem("Theme 3", menutheme3);

	menuBar()->insertItem("Game", menu_game);
	menuBar()->insertItem("Theme", menu_theme);
	menuBar()->insertItem("Help", helpMenu());

	statusBar()->insertItem("josef: 2", 0, 2);
	statusBar()->insertItem("pHr3ak: 3", 1, 2);
	statusBar()->insertItem("Place a stone onto the board.", 2, 2);

	board = new Board(this);
	setCentralWidget(board);
	show();

	connect(menu_game, SIGNAL(activated(int)), SLOT(slotMenu(int)));

	resize(600, 600);

	setCaption("Muehle");
}

Toplevel::~Toplevel()
{
}

void Toplevel::slotMenu(int id)
{
	switch(id)
	{
		case menugamenew:
			break;
		case menugamequit:
			close();
			break;
		case menutheme1:
			break;
		case menutheme2:
			break;
		case menutheme3:
			break;
	}
}

