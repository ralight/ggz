// Krosswater - Cross The Water for KDE
// Copyright (C) 2001, 2002 Josef Spillner, dr_maux@users.sourceforge.net
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

// Header file
#include "dlg_person.h"

// Configuration includes
#include "config.h"

// KDE includes
#include <klocale.h>

// Qt includes
#include <qpushbutton.h>
#include <qpainter.h>

// Constructor
DlgPerson::DlgPerson(QWidget *parent, const char *name)
: QWidget(parent, name)
{
	QPushButton *person1, *person2, *person3;

	person1 = new QPushButton("", this);
	person2 = new QPushButton("", this);
	person3 = new QPushButton("", this);

	person1->setPixmap(QPixmap(GGZDATADIR "/krosswater/gfx/franzl.png"));
	person2->setPixmap(QPixmap(GGZDATADIR "/krosswater/gfx/alien.png"));
	person3->setPixmap(QPixmap(GGZDATADIR "/krosswater/gfx/geek.png")),

	person1->setGeometry(150, 150, 40, 40);
	person2->setGeometry(150, 200, 40, 40);
	person3->setGeometry(150, 250, 40, 40);

	connect(person1, SIGNAL(clicked()), SLOT(slotPerson1()));
	connect(person2, SIGNAL(clicked()), SLOT(slotPerson2()));
	connect(person3, SIGNAL(clicked()), SLOT(slotPerson3()));

	setCaption("Character Selection");
	setFixedSize(200, 300);
	show();
}

// Destructor
DlgPerson::~DlgPerson()
{
}

// Player selected 'franzl'
void DlgPerson::slotPerson1()
{
	emit signalAccepted(0);
	close();
}

// Player selected 'alien'
void DlgPerson::slotPerson2()
{
	emit signalAccepted(1);
	close();
}

// Player selected 'geek'
void DlgPerson::slotPerson3()
{
	emit signalAccepted(2);
	close();
}

// Paint the dialog
void DlgPerson::paintEvent(QPaintEvent *e)
{
	QPainter p;

	Q_UNUSED(e);

	p.begin(this);
	p.setFont(QFont("arial", 10));
	p.setPen(QPen(QColor(255, 255, 0)));
	p.drawTiledPixmap(0, 0, 200, 300, QPixmap(GGZDATADIR "/krosswater/gfx/bg.png"));
	p.drawPixmap(10, 30, QPixmap(GGZDATADIR "/krosswater/gfx/title.png"));
	p.drawText(15, 90, i18n("Please select your"));
	p.drawText(15, 105, i18n("favorite character!"));
	p.drawText(15, 178, "Bavarian");
	p.drawText(15, 228, "Alien");
	p.drawText(15, 278, "Geek");
	p.end();
}

