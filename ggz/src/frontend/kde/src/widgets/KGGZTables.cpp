#include "KGGZTables.h"

#include "KGGZWidgetCommon.h"

KGGZTables::KGGZTables(QWidget *parent, char *name)
: QIconView(parent, name)
{
}

KGGZTables::~KGGZTables()
{
}


void KGGZTables::reset()
{
	clear();
}

void KGGZTables::add(char *name, int used, int total)
{
	QIconViewItem *tmp;
	char buffer[128];

	sprintf(buffer, "(TicTacToe)\nExample game\n(%i seats, %i open)", total, total - used);
	tmp = new QIconViewItem(this, buffer, QPixmap(KGGZ_DIRECTORY "/images/icons/ggz.png"));
}
