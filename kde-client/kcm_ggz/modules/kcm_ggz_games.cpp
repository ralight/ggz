#include "kcm_ggz_games.h"
#include "kcm_ggz_games.moc"

#include <klistview.h>
#include <qpushbutton.h>
#include <ksimpleconfig.h>
#include <kapplication.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kdebug.h>

#include <qlayout.h>
#include <qpixmap.h>
#include <qpopupmenu.h>

#include "config.h"

KCMGGZGames::KCMGGZGames(QWidget *parent, const char *name)
: KCMGGZPane(parent, name)
{
	QVBoxLayout *vbox;
	QPushButton *look;

	popup = NULL;

	view = new KListView(this);
	view->addColumn(i18n("Game"));
	view->addColumn(i18n("Version"));
	view->addColumn(i18n("Protocol"));
	view->addColumn(i18n("Homepage"));
	view->addColumn(i18n("Author(s)"));
	view->setRootIsDecorated(true);

	look = new QPushButton(i18n("Update local game information"), this);

	vbox = new QVBoxLayout(this, 5);
	vbox->add(view);
	vbox->add(look);

	connect(view, SIGNAL(rightButtonPressed(QListViewItem*, const QPoint&, int)), SLOT(slotSelected(QListViewItem*, const QPoint&, int)));
	connect(look, SIGNAL(clicked()), SLOT(slotUpdate()));
}

KCMGGZGames::~KCMGGZGames()
{
}

void KCMGGZGames::load()
{
	QStringList enginelist, gameslist;
	KSimpleConfig conf(GGZMODULECONFDIR "/ggz.modules");

	view->clear();

	conf.setGroup("Games");
	QString engine = conf.readEntry("*Engines*");
	enginelist = enginelist.split(" ", engine);
	for(QStringList::Iterator it = enginelist.begin(); it != enginelist.end(); it++)
	{
		conf.setGroup("Games");
		QString game = conf.readEntry((*it));
		gameslist = gameslist.split(" ", game);
		for(QStringList::Iterator it2 = gameslist.begin(); it2 != gameslist.end(); it2++)
		{
			conf.setGroup((*it2));
			QString author = conf.readEntry("Author");
			QString frontend = conf.readEntry("Frontend");
			QString version = conf.readEntry("Version");
			QString protocol = conf.readEntry("ProtocolVersion");
			QString homepage = conf.readEntry("Homepage");
			add(i18n("System"), (*it), frontend, author, homepage, version, protocol);
		}
	}

	// Use for local games later!
	//add(i18n("Local"), "Kamikaze", "kde", "Josef Spillner & Tobias König", "http://games.kde.org", "0.1", "1");
}

void KCMGGZGames::information(QString name, QString frontend)
{
	QStringList enginelist, gameslist;
	KSimpleConfig conf(GGZMODULECONFDIR "/ggz.modules");

	conf.setGroup("Games");
	QString engine = conf.readEntry("*Engines*");
	enginelist = enginelist.split(" ", engine);
	for(QStringList::Iterator it = enginelist.begin(); it != enginelist.end(); it++)
	{
		if((*it) == name)
		{
			conf.setGroup("Games");
			QString game = conf.readEntry((*it));
			gameslist = gameslist.split(" ", game);
			for(QStringList::Iterator it2 = gameslist.begin(); it2 != gameslist.end(); it2++)
			{
				conf.setGroup((*it2));
				QString tmp = conf.readEntry("Frontend");
				if(tmp == frontend)
				{
					QString author = conf.readEntry("Author");
					QString version = conf.readEntry("Version");
					QString protocol = conf.readEntry("ProtocolVersion");
					QString homepage = conf.readEntry("Homepage");
					QString engine = conf.readEntry("ProtocolEngine");
					QString commandline = conf.readEntry("CommandLine");
					QString environment = conf.readEntry("Environment", "xwindow");

					if(environment.isNull()) environment = "xwindow";

					if(environment == "passive") environment = i18n("Non-interactive (%1)").arg(environment);
					if(environment == "console") environment = i18n("Text console only (%1)").arg(environment);
					if(environment == "framebuffer") environment = i18n("Frame buffer (%1)").arg(environment);
					if(environment == "xwindow") environment = i18n("X11 window (%1)").arg(environment);
					if(environment == "xfullscreen") environment = i18n("Entire screen (%1)").arg(environment);

					KMessageBox::information(this,
						i18n("Name: ") + name + "\n" +
						i18n("Author: ") + author + "\n" +
						i18n("Frontend: ") + frontend + "\n" +
						i18n("Environment: ") + environment + "\n" +
						i18n("Version: ") + version + "\n" +
						i18n("Protocol version: ") + protocol + "\n" +
						i18n("Homepage: ") + homepage + "\n" +
						i18n("Engine: ") + engine + "\n" +
						i18n("Commandline: ") + commandline + "\n",
						i18n("Detailed information"));
					return;
				}
			}
		}
	}
}

void KCMGGZGames::save()
{
}

QString KCMGGZGames::caption()
{
	return i18n("Available games");
}

extern "C"
{
	KCMGGZPane *kcmggz_init(QWidget *parent, const char *name)
	{
		return new KCMGGZGames(parent, name);
	}
}

void KCMGGZGames::add(QString location, QString name, QString frontend, QString authors, QString homepage, QString version, QString protocol)
{
	KListViewItem *loc, *item;
	QString value;
	QString pixname;

	loc = NULL;
	for(QListViewItem *i = view->firstChild(); i; i = i->nextSibling())
	{
		if(i->text(0) == location)
		{
			loc = reinterpret_cast<KListViewItem*>(i);
			break;
		}
	}
	if(!loc) loc = new KListViewItem(view, location);
	loc->setOpen(true);

	item = NULL;
	for(QListViewItem *i = loc->firstChild(); i; i = i->nextSibling())
	{
		if(i->text(0) == name)
		{
			item = reinterpret_cast<KListViewItem*>(i);
			break;
		}
	}
	if(!item) item = new KListViewItem(loc, name);
	item->setOpen(true);

	item = new KListViewItem(item, frontend, version, protocol, homepage, authors);

	pixname = "game.png";
	if((frontend == "gtk") || (frontend == "gnome")) pixname = "game_gnome.png";
	if((frontend == "qt") || (frontend == "kde")) pixname = "game_kde.png";
	if(frontend == "sdl") pixname = "game_sdl.png";
	if(frontend == "console") pixname = "game_console.png";
	if(frontend == "guru") pixname = "game_grubby.png";
	item->setPixmap(0, QPixmap(QString(KGGZ_DIRECTORY) + "/kcmggz/icons/" + pixname));
}

void KCMGGZGames::slotSelected(QListViewItem *item, const QPoint& point, int column)
{
	if(!item) return;
	if(item->text(1) == QString::null) return;

	if(!popup)
	{
		popup = new QPopupMenu(this);
		popup->insertItem(i18n("Visit the project home page"), menuhomepage);
		popup->insertItem(i18n("Show all information"), menuinformation);
		connect(popup, SIGNAL(activated(int)), SLOT(slotActivated(int)));
	}

	popup->popup(point);
}

void KCMGGZGames::slotUpdate()
{
	load();
}

void KCMGGZGames::slotActivated(int index)
{
	if(!view->currentItem()) return;
	if(view->currentItem()->text(1) == QString::null) return;

	switch(index)
	{
		case menuhomepage:
			kapp->invokeBrowser(view->currentItem()->text(3));
			break;
		case menuinformation:
			information(view->currentItem()->parent()->text(0), view->currentItem()->text(0));
			break;
	}
}


