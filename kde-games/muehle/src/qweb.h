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

#ifndef QWEB_H
#define QWEB_H

// Qt includes
#include <qptrlist.h>
#include <qpoint.h>

// Class representing one single point in a QWeb
class QWebPoint
{
	public:
		QWebPoint(QPoint p);
		~QWebPoint();
		void addPeer(QPoint p);
		QPtrList<QPoint> peerlist();
		QPoint point();
		void setData(int data);
		int data();

	private:
		QPoint m_point;
		QPtrList<QPoint> m_peerlist;
		int m_data;
};

// Class holding many QWebPoints
class QWeb
{
	public:
		QWeb();
		~QWeb();
		void addPoint(QPoint p);
		void addPeer(QPoint p, QPoint p2);
		QPoint getPoint(QPoint p);
		QPtrList<QWebPoint> pointlist();
		void setScale(float factor);
		float scale();
		void setData(QPoint p, int data);
		int data(QPoint p);

	private:
		QPtrList<QWebPoint> m_pointlist;
		QWebPoint *tmp;
		float m_scale;
};

#endif

