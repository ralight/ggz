#include "kcm_ggz_games.h"
#include "kcm_ggz_games.moc"

#include <klistview.h>
#include <qpushbutton.h>
#include <ksimpleconfig.h>
#include <kapp.h>
#include <klocale.h>

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

	add(i18n("Local"), "KaBoom!!", "KDE", "Josef Spillner & Tobias K�nig", "http://games.kde.org", "0.1", "1");

	look = new QPushButton(i18n("Update local game information"), this);

	vbox = new QVBoxLayout(this, 5);
	vbox->add(view);
	vbox->add(look);

	connect(view, SIGNAL(rightButtonPressed(QListViewItem*, const QPoint&, int)), SLOT(slotSelected(QListViewItem*, const QPoint&, int)));
	connect(look, SIGNAL(clicked()), SLOT(slotUpdate()));

	load();
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
	QListViewItem *loc, *item;
	QString value;
	QString pixname;

	loc = NULL;
	for(QListViewItem *i = view->firstChild(); i; i = i->nextSibling())
	{
		if(i->text(0) == location)
		{
			loc = i;
			break;
		}
	}
	if(!loc) loc = new QListViewItem(view, location);
	loc->setOpen(true);

	item = NULL;
	for(QListViewItem *i = loc->firstChild(); i; i = i->nextSibling())
	{
		if(i->text(0) == name)
		{
			item = i;
			break;
		}
	}
	if(!item) item = new QListViewItem(loc, name);
	item->setOpen(true);

	item = new QListViewItem(item, frontend, version, protocol, homepage, authors);

	pixname = "game.png";
	if((frontend == "gtk") || (frontend == "gnome")) pixname = "game_gnome.png";
	if((frontend == "qt") || (frontend == "kde")) pixname = "game_kde.png";
	item->setPixmap(0, QPixmap(QString(KGGZ_DIRECTORY) + "/kcmggz/icons/" + pixname));
}

void KCMGGZGames::slotSelected(QListViewItem *item, const QPoint& point, int column)
{
	if(!item) return;
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

	switch(index)
	{
		case menuhomepage:
			kapp->invokeBrowser(view->currentItem()->text(3));
			break;
		case menuinformation:
			break;
	}
}


