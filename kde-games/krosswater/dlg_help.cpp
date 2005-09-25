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
#include "dlg_help.h"

// Configuration includes
#include "config.h"

// KDE includes
#include <klocale.h>

// Qt includes
#include <qpixmap.h>
#include <qpainter.h>
#include <qpushbutton.h>

// Constructor
DlgHelp::DlgHelp(QWidget *parent, const char *name)
: QWidget(parent, name)
{
	QPushButton *ok;

	ok = new QPushButton(i18n("OK"), this);
	connect(ok, SIGNAL(clicked()), SLOT(close()));
	ok->setGeometry(200, 270, 100, 20);

	setCaption(i18n("Krosswater Help"));
	setFixedSize(500, 300);
	show();
}

// Destructor
DlgHelp::~DlgHelp()
{
}

// Paint the dialog
void DlgHelp::paintEvent(QPaintEvent *e)
{
	QPainter p;

	Q_UNUSED(e);

	p.begin(this);
	p.drawTiledPixmap(0, 0, 500, 400, QPixmap(GGZDATADIR "/krosswater/gfx/bg.png"));
	p.drawPixmap(150, 30, QPixmap(GGZDATADIR "/krosswater/gfx/title.png"));
	p.setPen(QPen(QColor(255, 255, 0)));
	p.setFont(QFont("arial", 10));
	p.drawText(15, 90, i18n("As one of 2 to 4 players on the board, your task is to"));
	p.drawText(15, 105, i18n("cross the water from the left to the right. This can be accomplished"));
	p.drawText(15, 120, i18n("by clicking on a stone tile, and then clicking on a water tile to move"));
	p.drawText(15, 135, i18n("the stone tile there."));
	p.drawText(15, 150, i18n("The more players participate, the more fun it is to steal stone tiles that"));
	p.drawText(15, 165, i18n("have been placed by the opponents for their own purpose."));
	p.drawText(15, 180, i18n("The game ends if the first player has build a track of stones from his"));
	p.drawText(15, 195, i18n("starting point to somewhere on the right side."));
	p.end();
}

