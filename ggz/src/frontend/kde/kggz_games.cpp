#include "kggz_games.h"
#include <klocale.h>

KGGZ_Games::KGGZ_Games(QWidget *parent, char *name)
: QTable(parent, name)
{
	QHeader *header;

	setNumRows(50);
	setNumCols(5);

	horizontalHeader()->addLabel(i18n("Name"));
	horizontalHeader()->addLabel(i18n("Author"));
	horizontalHeader()->addLabel(i18n("Homepage"));
	horizontalHeader()->addLabel(i18n("Version"));
	horizontalHeader()->addLabel(i18n("Frontend"));

	/*hideColumn(0);
	hideColumn(1);
	hideColumn(2);
	hideColumn(3);
	hideColumn(4);*/

	m_gamenum = 0;

	addGame("Ueberflieger", "Josef Spillner", "0.0.4", "Mesa/Glut", "http://mindx.sourceforge.net");
	addGame("Hastings1066", "Josef Spillner", "0.0.4", "Gtk+", "http://mindx.sourceforge.net");
}

KGGZ_Games::~KGGZ_Games()
{
}

void KGGZ_Games::addGame(char *name, char *author, char *version, char *frontend, char *homepage)
{
	setItem(m_gamenum, 0, new QTableItem(this, QTableItem::Never, name));
	setItem(m_gamenum, 1, new QTableItem(this, QTableItem::Never, author));
	setItem(m_gamenum, 2, new QTableItem(this, QTableItem::Never, homepage));
	setItem(m_gamenum, 3, new QTableItem(this, QTableItem::Never, version));
	setItem(m_gamenum, 4, new QTableItem(this, QTableItem::Never, frontend));
	m_gamenum++;
}
