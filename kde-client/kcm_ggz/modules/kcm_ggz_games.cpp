#include "kcm_ggz_games.h"
#include "kcm_ggz_games.moc"

#include <klistview.h>
#include <qlayout.h>
#include <qpixmap.h>
#include <qpopupmenu.h>
#include <kpushbutton.h>
#include <ksimpleconfig.h>
#include <kapp.h>
#include "config.h"

KCMGGZGames::KCMGGZGames(QWidget *parent, const char *name)
: KCMGGZPane(parent, name)
{
	QVBoxLayout *vbox;
	KPushButton *look;

	popup = NULL;

	view = new KListView(this);
	view->addColumn("Game");
	view->addColumn("Version");
	view->addColumn("Protocol");
	view->addColumn("Homepage");
	view->addColumn("Author(s)");
	view->setRootIsDecorated(true);

	add("Local", "KaBoom!!", "KDE", "Josef Spillner & Tobias K�nig", "http://games.kde.org", "0.1", "1");

	look = new KPushButton("Update local game information", this);

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
	KSimpleConfig conf("/usr/local/etc/ggz.modules");

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
			add("System", (*it), frontend, author, homepage, version, protocol);
		}
	}
}

void KCMGGZGames::save()
{
}

const char *KCMGGZGames::caption()
{
	return "Available games";
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
	item->setPixmap(0, QPixmap(QString(KGGZ_DIRECTORY) + "/kcmggz/icons/game.png"));
}

void KCMGGZGames::slotSelected(QListViewItem *item, const QPoint& point, int column)
{
	if(!item) return;
	if(!popup)
	{
		popup = new QPopupMenu(this);
		popup->insertItem("Visit the project home page", menuhomepage);
		popup->insertItem("Show all information", menuinformation);
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
	switch(index)
	{
		case menuhomepage:
			kapp->invokeBrowser("http://ggz.sourceforge.net");
			break;
		case menuinformation:
			break;
	}
}


