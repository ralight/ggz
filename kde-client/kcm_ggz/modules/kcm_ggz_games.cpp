#include "kcm_ggz_games.h"
#include "kcm_ggz_games.moc"

#include <klistview.h>
#include <qlayout.h>
#include <qpixmap.h>
#include <qpopupmenu.h>
#include <qpushbutton.h>
#include "config.h"

KCMGGZGames::KCMGGZGames(QWidget *parent, const char *name)
: KCMGGZPane(parent, name)
{
	QVBoxLayout *vbox;
	QPushButton *look;

	view = new KListView(this);
	view->addColumn("Game");
	view->addColumn("Version");
	view->addColumn("Protocol");
	view->addColumn("Homepage");
	view->addColumn("Author(s)");
	view->setRootIsDecorated(true);

	add("System", "TicTacToe", "KDE", "Josef Spillner", "http://ggz.sourceforge.net", "0.0.5pre", "1");
	add("System", "TicTacToe", "Gtk+", "Brent Hendricks", "http://ggz.sourceforge.net", "0.0.5pre", "1");
	add("System", "LaPocha", "Gtk+", "Rich Gade", "http://ggz.sourceforge.net", "0.1", "1");
	add("Local", "KaBoom!!", "KDE", "Josef Spillner & Tobias König", "http://games.kde.org", "0.1", "1");

	look = new QPushButton("Update local game information", this);

	vbox = new QVBoxLayout(this, 5);
	vbox->add(view);
	vbox->add(look);

	connect(view, SIGNAL(rightButtonPressed(QListViewItem*, const QPoint&, int)), SLOT(slotSelected(QListViewItem*, const QPoint&, int)));

	load();
}

KCMGGZGames::~KCMGGZGames()
{
}

void KCMGGZGames::load()
{
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

void KCMGGZGames::slotSelected(QListViewItem *item, const QPoint &point, int column)
{
	QPopupMenu *popup;

	if(!item) return;
	popup = new QPopupMenu(this);
	popup->insertItem("Visit the project home page");
	popup->insertItem("Show all information");
	popup->popup(point);
}

