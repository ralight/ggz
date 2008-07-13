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
#include "qwebpath.h"

// QWeb includes
#include "qweb.h"

// System includes
#include <iostream>

using namespace std;

// Constructor: initialize with firts point on the web
QWebPath::QWebPath(QWeb *web, QWebPoint *point)
{
	m_web = web;
	m_wp = point;
}

// Destructor: clean up search results
QWebPath::~QWebPath()
{
	m_pathlist.clear();
}

// Flood fill with the fiven maximum radius
void QWebPath::create(int length)
{
	m_pathlist.clear();

	findRecursive(NULL, m_wp, length);
}

// Return the pathlist found by the flood fill
QPtrList<QWebPoint> QWebPath::pathlist()
{
	return m_pathlist;
}

// Recursive flood fill-type search function
void QWebPath::findRecursive(QWebPoint *old, QWebPoint *origin, int length)
{
	QPtrList<QWebPoint> pointlist;
	QPtrList<QPoint> peerlist;
	QWebPoint *wp;

	m_pathlist.append(origin);
	if(!length) return;

	peerlist = origin->peerlist();
	for(QPoint *p = peerlist.first(); p; p = peerlist.next())
	{
		pointlist = m_web->pointlist();
		wp = NULL;
		for(QWebPoint *tmp = pointlist.first(); tmp; tmp = pointlist.next())
			if((tmp->point().x() == p->x()) && (tmp->point().y() == p->y()))
				wp = tmp;
		if(!wp)
		{
			//cout << "invalid point: " << p->x() << "," << p->y() << endl;
			continue;
		}

		if(wp != old)
		{
			//cout << "level " << length << ", peer: " << p->x() << "," << p->y() << endl;
			findRecursive(origin, wp, length - 1);
		}
		//else cout << "left out: " << p->x() << "," << p->y() << endl;
	}
}

