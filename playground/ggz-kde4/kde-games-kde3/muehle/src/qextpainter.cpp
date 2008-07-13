// Muehle - KDE Muehle (Nine Men's Morris) game for GGZ
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
#include "qextpainter.h"

// Qt includes
#include <qptrlist.h>

// System includes
#include <iostream>

// Constructor
QExtPainter::QExtPainter()
: QPainter()
{
}

// Destructor
QExtPainter::~QExtPainter()
{
}

// Draw a complete QWeb on the default paint device
void QExtPainter::drawWeb(QWeb w)
{
	QPtrList<QWebPoint> wplist;
	QPtrList<QPoint> plist;
	int x, y, x2, y2;
	float s;

	s = w.scale();
	wplist = w.pointlist();
	for(QWebPoint *wp = wplist.first(); wp; wp = wplist.next())
	{
		//cout << "new point" << endl;
		plist = wp->peerlist();
		for(QPoint *p = plist.first(); p; p = plist.next())
		{
			x = (int)(p->x() * s);
			y = (int)(p->y() * s);
			x2 = (int)(wp->point().x() * s);
			y2 = (int)(wp->point().y() * s);
			//cout << "new peer; paint: " << x << "," << y << " to " << x2 << ", " << y2 << endl;
			drawLine(x, y, x2, y2);
		}
	}
}

