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

void KGGZTables::add(char *gametype, char *name, int used, int total)
{
	QIconViewItem *tmp;
	char buffer[128];

	sprintf(buffer, "(%s) %s\n(%i seats, %i open)", gametype, name, total, total - used);
	tmp = new QIconViewItem(this, buffer, QPixmap(KGGZ_DIRECTORY "/images/icons/ggz.png"));
}

int KGGZTables::tablenum()
{
	QIconViewItem *tmp;
	int index;

	tmp = currentItem();
	if(!tmp) return -1;
	index = tmp->index();
	KGGZDEBUG("Selected table to join is: %i\n", index);
	return index;
}

