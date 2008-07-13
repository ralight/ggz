//
//    Keepalive Control
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

#include "create.h"

#include <klocale.h>
#include <kmessagebox.h>

#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qpushbutton.h>

Create::Create(QWidget *parent, const char *name)
: QDialog(parent, name, true)
{
	QVBoxLayout *vbox;
	QHBoxLayout *hbox;
	QLabel *label_world;
	QPushButton *button_connect, *button_cancel;

	label_world = new QLabel(i18n("World name"), this);

	input_world = new QLineEdit(this);

	button_connect = new QPushButton(i18n("Create"), this);
	button_cancel = new QPushButton(i18n("Cancel"), this);

	vbox = new QVBoxLayout(this, 5);
	vbox->add(label_world);
	vbox->add(input_world);
	hbox = new QHBoxLayout(vbox, 5);
	hbox->add(button_connect);
	hbox->add(button_cancel);

	connect(button_cancel, SIGNAL(clicked()), SLOT(close()));
	connect(button_connect, SIGNAL(clicked()), SLOT(slotWorld()));

	setCaption("Create...");
	show();
}

Create::~Create()
{
}

void Create::slotWorld()
{
	emit signalWorld(input_world->text());
	accept();
}

QString Create::world()
{
	return input_world->text();
}

