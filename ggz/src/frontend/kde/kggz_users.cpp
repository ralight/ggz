#include "kggz_users.h"

KGGZ_Users::KGGZ_Users(QWidget *parent, char *name)
: QListView(parent, name)
{
	QListViewItem *itemmain;

	itemmain = new QListViewItem(this, "Current table");
	itemmain->setOpen(TRUE);

	//TODO: real people
	//QListViewItem *tmp = new QListViewItem(itemmain, "josef2");
	//QListViewItem *tmp2 = new QListViewItem(itemmain, "yoshi");

	addColumn("Players");
	addColumn("Table");
	insertItem(itemmain);

	setRootIsDecorated(TRUE);
}

KGGZ_Users::~KGGZ_Users()
{
}

