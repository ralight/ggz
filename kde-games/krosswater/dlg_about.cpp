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
#include "dlg_about.h"

// Configuration includes
#include "config.h"

// Qt includes
#include <qpixmap.h>
#include <qpainter.h>
#include <qpushbutton.h>

// Constructor
DlgAbout::DlgAbout(QWidget *parent, const char *name)
: QWidget(parent, name)
{
	QPushButton *ok;

	ok = new QPushButton("OK", this);
	connect(ok, SIGNAL(clicked()), SLOT(close()));
	ok->setGeometry(100, 270, 100, 20);

	setCaption("About Krosswater");
	setFixedSize(300, 300);
	show();
}

// Desctructor
DlgAbout::~DlgAbout()
{
}

// Draw the dialog
void DlgAbout::paintEvent(QPaintEvent *e)
{
	QPainter p;

	p.begin(this);
	p.drawTiledPixmap(0, 0, 300, 300, QPixmap(GGZDATADIR "/krosswater/gfx/bg.png"));
	p.drawPixmap(60, 30, QPixmap(GGZDATADIR "/krosswater/gfx/title.png"));
	p.setPen(QPen(QColor(255, 255, 0)));
	p.setFont(QFont("arial", 10));
	p.drawText(15, 90, "Try to cross the water the most efficient way!");
	p.drawText(15, 103, "This game is part of the GGZ Gaming Zone.");
	p.drawText(15, 116, "http://ggz.sourceforge.net");
	p.drawText(15, 135, "Copyright (C) 2001 Josef Spillner");
	p.drawText(15, 148, "dr_maux@users.sourceforge.net");
	p.drawText(15, 161, "The MindX Open Source Project");
	p.drawText(15, 174, "http://mindx.sourceforge.net");
	p.drawPixmap(15, 190, QPixmap(GGZDATADIR "/krosswater/gfx/mindx.png"));
	p.end();
}

