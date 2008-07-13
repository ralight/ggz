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
#include "dlg_again.h"

// Configuration includes
#include "config.h"

// KDE includes
#include <klocale.h>

// Qt includes
#include <qpixmap.h>
#include <qpainter.h>
#include <qpushbutton.h>

// Constructor
DlgAgain::DlgAgain(QWidget *parent, const char *name)
: QWidget(parent, name)
{
	QPushButton *ok, *cancel;

	ok = new QPushButton(i18n("Yes"), this);
	cancel = new QPushButton(i18n("No"), this);

	ok->setGeometry(30, 140, 100, 30);
	cancel->setGeometry(170, 140, 100, 30);

	m_result = i18n("(result unknown)");

	connect(cancel, SIGNAL(clicked()), SLOT(close()));
	connect(ok, SIGNAL(clicked()), SIGNAL(signalAgain()));

	setCaption(i18n("Game Over"));
	setFixedSize(300, 180);
	show();
}

// Destructor
DlgAgain::~DlgAgain()
{
}

// Paint the dialog
void DlgAgain::paintEvent(QPaintEvent *e)
{
	QPainter p;

	Q_UNUSED(e);

	p.begin(this);
	p.drawTiledPixmap(0, 0, 300, 300, QPixmap(GGZDATADIR "/krosswater/gfx/bg.png"));
	p.drawPixmap(60, 30, QPixmap(GGZDATADIR "/krosswater/gfx/title.png"));
	p.setPen(QPen(QColor(255, 255, 0)));
	p.setFont(QFont("arial", 10));
	p.drawText(15, 90, i18n("The game is over."));
	p.drawText(15, 105, m_result);
	p.drawText(15, 120, i18n("Play another game?"));
	p.end();
}

// Change the result which is displayed
void DlgAgain::setResult(const char *result)
{
	m_result = QString(result);
}

