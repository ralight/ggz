#include "toplevel.h"
#include "board.h"
#include <kpopupmenu.h>
#include <kmenubar.h>
#include <ksimpleconfig.h>
#include <kstddirs.h>

Toplevel::Toplevel()
: KMainWindow()
{
	KPopupMenu *menu_game, *menu_theme, *menu_player, *menu_variants;
	int counter;
	KStandardDirs d;
	QString s;

	menu_game = new KPopupMenu(this);
	menu_game->insertItem("New", menugamenew);
	menu_game->insertSeparator();
	menu_game->insertItem("Quit", menugamequit);

	menu_variants = new KPopupMenu(this);
	s = d.findResource("data", "muehle/rc");
	KSimpleConfig conf(s);
	conf.setGroup("Muehle");
	variantslist = conf.readListEntry("Variants");
	for(QStringList::Iterator it = variantslist.begin(); it != variantslist.end(); it++)
	{
		conf.setGroup((*it));
		QString file = conf.readEntry("file");
		QString description = conf.readEntry("description");
		int width = conf.readNumEntry("width");
		int height = conf.readNumEntry("height");
		menu_variants->insertItem(description, menuvariants + counter++);
	}

	counter = 0;
	menu_theme = new KPopupMenu(this);
	s = d.findResource("data", "muehle/themerc");
	KSimpleConfig tconf(s);
	tconf.setGroup("Themes");
	themelist = tconf.readListEntry("Themes");
	for(QStringList::Iterator it = themelist.begin(); it != themelist.end(); it++)
	{
		tconf.setGroup((*it));
		QString name = tconf.readEntry("Name");
		menu_theme->insertItem(name, menuthemes + counter++);
	}

	menu_player = new KPopupMenu(this);
	menu_player->insertItem("Offer remis", menuplayerremis);
	menu_player->insertItem("Give up", menuplayerloose);

	menuBar()->insertItem("Game", menu_game);
	menuBar()->insertItem("Variants", menu_variants);
	menuBar()->insertItem("Player", menu_player);
	menuBar()->insertItem("Theme", menu_theme);
	menuBar()->insertItem("Help", helpMenu());

	statusBar()->insertItem("josef: 2", 0, 2);
	statusBar()->insertItem("pHr3ak: 3", 1, 2);
	statusBar()->insertItem("Place a stone onto the board.", 2, 2);

	board = new Board(this);
	setCentralWidget(board);
	show();

	connect(menu_game, SIGNAL(activated(int)), SLOT(slotMenu(int)));
	connect(menu_player, SIGNAL(activated(int)), SLOT(slotMenu(int)));
	connect(menu_theme, SIGNAL(activated(int)), SLOT(slotMenu(int)));
	connect(menu_variants, SIGNAL(activated(int)), SLOT(slotMenu(int)));

	resize(600, 620);
}

Toplevel::~Toplevel()
{
}

void Toplevel::slotMenu(int id)
{
	switch(id)
	{
		case menugamenew:
			board->init();
			break;
		case menugamequit:
			close();
			break;
		case menuplayerremis:
			board->remis();
			break;
		case menuplayerloose:
			board->loose();
			break;
		default:
			if((id >= menuthemes) && (id < menuvariants))
			{
				board->setTheme(themelist[id - menuthemes]);
			}
			else if(id >= menuvariants)
			{
				board->setVariant(variantslist[id - menuvariants]);
			}
	}
}

