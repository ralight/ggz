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

#ifndef QWEBPATH_H
#define QWEBPATH_H

// Qt includes
#include <qptrlist.h>
#include <qpoint.h>

// Class declaration
class QWeb;
class QWebPoint;

// Pathfinding in a QWeb
class QWebPath
{
	public:
		QWebPath(QWeb *web, QWebPoint *point);
		~QWebPath();
		void create(int length);
		QPtrList<QWebPoint> pathlist();

	private:
		void findRecursive(QWebPoint *old, QWebPoint *origin, int length);

		QPtrList<QWebPoint> m_pathlist;
		QWeb *m_web;
		QWebPoint *m_wp;
};

#endif

