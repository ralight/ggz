// Koenig - KDE client for the GGZ chess game
// Copyright (C) 2001 Tobias König, tokoe82@yahoo.de
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

#include <kdebug.h>
#include <klocale.h>

#include <qlayout.h>

#include "boardcontainer.h"

ChessBoardContainer::ChessBoardContainer(QWidget *parent, const char *name)
: QWidget(parent, name)
{
	QVBoxLayout *vbox;

	kdDebug(12101) << "ChessBoardContainer::ChessBoardContainer()" << endl;

	setMinimumSize(240, 240);
	resize(400, 400);

	m_root = new ChessBoard(this);
	vbox = new QVBoxLayout(this, 10);
	vbox->add(m_root);

	setEraseColor(QColor(255, 127, 0)); // FIXME: nice pixmap with wood :-)
	setCaption(i18n("Chess Board - inactive"));
}

ChessBoardContainer::~ChessBoardContainer()
{
}

void ChessBoardContainer::resizeEvent(QResizeEvent *event)
{
	int size = (event->size().width() + event->size().height()) / 2 / 8;

	resize(size * 8, size * 8);
	//root->setGeometry(BOARD_BORDER, BOARD_BORDER, size * 8 - BOARD_BORDER * 2, size * 8 - BOARD_BORDER * 2);
}

ChessBoard *ChessBoardContainer::root()
{
	return m_root;
}

