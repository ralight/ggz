// Krosswater - Cross The Water for KDE
// Copyright (C) 2001, 2002 Josef Spillner, josef@ggzgamingzone.org
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

// KDE includes
#include <klocale.h>

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
	ok->setGeometry(125, 270, 100, 20);

	setCaption(i18n("About Krosswater"));
	setFixedSize(350, 300);
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
	p.drawTiledPixmap(0, 0, 350, 300, QPixmap(GGZDATADIR "/krosswater/gfx/bg.png"));
	p.drawPixmap(90, 30, QPixmap(GGZDATADIR "/krosswater/gfx/title.png"));
	p.setPen(QPen(QColor(255, 255, 0)));
	p.setFont(QFont("arial", 10));
	p.drawText(15, 90, i18n("Try to cross the water the most efficient way!"));
	p.drawText(15, 105, i18n("This game is part of the GGZ Gaming Zone."));
	p.drawText(15, 120, "http://www.ggzgamingzone.org/games/krosswater/");
	p.drawText(15, 140, "Copyright (C) 2001, 2002 Josef Spillner");
	p.drawText(15, 155, "josef@ggzgamingzone.org");
	p.drawText(15, 170, "The MindX Open Source Project");
	p.drawText(15, 185, "http://mindx.sourceforge.net");
	p.drawPixmap(140, 210, QPixmap(GGZDATADIR "/krosswater/gfx/mindx.png"));
	p.end();
}

