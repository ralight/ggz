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
#include "qweb.h"

// System includes
#include <iostream>
#include <math.h>
#include <cstdlib>

QWebPoint::QWebPoint(QPoint p)
{
	m_peerlist.setAutoDelete(true);
	m_point = p;
}

QWebPoint::~QWebPoint()
{
}

void QWebPoint::addPeer(QPoint p)
{
	m_peerlist.append(new QPoint(p));
	//cout << "peer: " << p.x() << "," << p.y() << endl;
}

QPtrList<QPoint> QWebPoint::peerlist()
{
	return m_peerlist;
}

QPoint QWebPoint::point()
{
	return m_point;
}

void QWebPoint::setData(int data)
{
	m_data = data;
}

int QWebPoint::data()
{
	return m_data;
}

QWeb::QWeb()
{
	m_scale = 1.0;
	m_pointlist.setAutoDelete(true);
}

QWeb::~QWeb()
{
}

void QWeb::addPoint(QPoint p)
{
	for(tmp = m_pointlist.first(); tmp; tmp = m_pointlist.next())
		if(tmp->point() == p)
			return;

	tmp = new QWebPoint(p);
	m_pointlist.append(tmp);
	//cout << "create new point: " << p.x() << "," << p.y() << endl;
}

void QWeb::addPeer(QPoint p, QPoint p2)
{
	addPoint(p);
	addPoint(p2);

	for(tmp = m_pointlist.first(); tmp; tmp = m_pointlist.next())
	{
		if(tmp->point() == p)
			tmp->addPeer(p2);
		if(tmp->point() == p2)
			tmp->addPeer(p);
	}
}

QPoint QWeb::getPoint(QPoint p)
{
	QPoint ret;
	unsigned int dist, disttmp;

	ret = QPoint(0, 0);
	//dist = 0xFFFFFFFF;
	dist = 5;
	for(tmp = m_pointlist.first(); tmp; tmp = m_pointlist.next())
	{
		disttmp = std::abs(tmp->point().x() - p.x()) * std::abs(tmp->point().x() - p.x());
		disttmp += std::abs(tmp->point().y() - p.y()) * std::abs(tmp->point().y() - p.y());
		disttmp = (unsigned int)sqrt(disttmp);
		//cout << "Evaluate: " << disttmp << endl;
		if(disttmp < dist)
		{
			dist = disttmp;
			ret = tmp->point();
		}
	}
	return ret;
}

QPtrList<QWebPoint> QWeb::pointlist()
{
	return m_pointlist;
}

void QWeb::setScale(float factor)
{
	m_scale = factor;
}

float QWeb::scale()
{
	return m_scale;
}

void QWeb::setData(QPoint p, int data)
{
	for(tmp = m_pointlist.first(); tmp; tmp = m_pointlist.next())
	{
		if(tmp->point() == p)
		{
			tmp->setData(data);
		}
	}
}

int QWeb::data(QPoint p)
{
	for(tmp = m_pointlist.first(); tmp; tmp = m_pointlist.next())
	{
		if(tmp->point() == p)
		{
			return tmp->data();
		}
	}
	return 0;
}

