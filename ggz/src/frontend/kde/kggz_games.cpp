#include "kggz_games.h"
#include <klocale.h>

#include <qlabel.h>

#include "config.h"

KGGZ_Games::KGGZ_Games(QWidget *parent, char *name)
: QTable(parent, name)
{
	QHeader *header;

	setNumRows(50);
	setNumCols(5);

	for(int i = 0; i < 6; i++)
		horizontalHeader()->removeLabel(0);
	horizontalHeader()->addLabel(i18n("Icon"));
	horizontalHeader()->addLabel(i18n("Name"));
	horizontalHeader()->addLabel(i18n("Author"));
	horizontalHeader()->addLabel(i18n("Frontend"));
	horizontalHeader()->addLabel(i18n("Version"));
	horizontalHeader()->addLabel(i18n("Homepage"));

	m_gamenum = 0;

	addGame("TicTacToe", "Brent Hendricks", "0.0.4", "Gtk+", "http://ggz.sourceforge.net");
	addGame("NetSpades", "Brent Hendricks", "0.0.4", "Gtk+", "http://ggz.sourceforge.net");
	addGame("LaPocha", "Rich Gade", "0.0.4", "Gtk+", "http://ggz.sourceforge.net");
	addGame("Connect The Dots", "Rich Gade", "0.0.4", "Gtk+", "http://ggz.sourceforge.net");
	addGame("Hastings1066", "Josef Spillner", "0.0.4", "Gtk+", "http://mindx.sourceforge.net");
	addGame("Reversi", "Ismael Orenstein", "0.0.4", "Gtk+", "http://ggz.sourceforge.net");
	addGame("Combat", "Ismael Orenstein", "0.0.4", "Gtk+", "http://ggz.sourceforge.net");
	addGame("Ueberflieger", "Josef Spillner", "0.0.4", "Mesa/Glut", "http://mindx.sourceforge.net");

	for(int i = 0; i < 6; i++)
		adjustColumn(i);
}

KGGZ_Games::~KGGZ_Games()
{
}

void KGGZ_Games::addGame(char *name, char *author, char *version, char *frontend, char *homepage)
{
	setItem(m_gamenum, 0, new QTableItem(this, QTableItem::Never, NULL, QPixmap(QString(instdir) + "images/games/hastings.png")));
	setItem(m_gamenum, 1, new QTableItem(this, QTableItem::Never, name));
	setItem(m_gamenum, 2, new QTableItem(this, QTableItem::Never, author));
	setItem(m_gamenum, 3, new QTableItem(this, QTableItem::Never, frontend));
	setItem(m_gamenum, 4, new QTableItem(this, QTableItem::Never, version));
	setItem(m_gamenum, 5, new QTableItem(this, QTableItem::Never, homepage));
	setCellWidget(m_gamenum, 5, new QLabel(QString("<A HREF='") + QString(homepage) + QString("'>") + QString(homepage) + QString("<\\A>"), this));
	m_gamenum++;
}
