//
//    Competition Calendar
//
//    Copyright (C) 2002, 2003 Josef Spillner <josef@ggzgamingzone.org>
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#include "conf.h"

#include <klocale.h>
#include <klistbox.h>
#include <kmessagebox.h>

#include <kdebug.h>
#include <kapplication.h>
#include <kconfig.h>

#include <qlayout.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qstringlist.h>

Conf::Conf(QWidget *parent, const char *name)
: KDialogBase(parent, name, true, QString::null, KDialogBase::Ok | KDialogBase::Cancel)
{
	QWidget *root;
	QVBoxLayout *vbox;
	QPushButton *add, *remove;

	root = makeMainWidget();

	resources = new QListBox(root);

	line = new QLineEdit(root);

	add = new QPushButton(i18n("Add resource"), root);
	remove = new QPushButton(i18n("Remove resource"), root);

	vbox = new QVBoxLayout(root, 5);
	vbox->add(resources);
	vbox->add(remove);
	vbox->add(line);
	vbox->add(add);

	connect(this, SIGNAL(okClicked()), SLOT(slotAccept()));
	connect(this, SIGNAL(cancelClicked()), SLOT(close()));
	connect(add, SIGNAL(clicked()), SLOT(slotAdd()));
	connect(remove, SIGNAL(clicked()), SLOT(slotRemove()));

	load();
}

Conf::~Conf()
{
}

void Conf::slotAccept()
{
	accept();
}

void Conf::load()
{
	KConfig *conf;
	QStringList list;
	QStringList::iterator it;

	conf = kapp->config();
	conf->setGroup("resources");

	list = conf->readListEntry("resources");
	for(it = list.begin(); it != list.end(); it++)
		resources->insertItem((*it));
}

void Conf::save()
{
	KConfig *conf;
	QStringList list;

	conf = kapp->config();
	conf->setGroup("resources");

	for(unsigned int i = 0; i < resources->count(); i++)
		list << resources->text(i);

	conf->writeEntry("resources", list);
	conf->sync();
}

void Conf::slotAdd()
{
	resources->insertItem(line->text());
	line->setText("");
	save();
}

void Conf::slotRemove()
{
	resources->removeItem(resources->currentItem());
	save();
}

