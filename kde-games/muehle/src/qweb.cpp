#include "qweb.h"
#include <iostream>
#include <math.h>

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

QList<QPoint> QWebPoint::peerlist()
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
		disttmp = abs(tmp->point().x() - p.x()) * abs(tmp->point().x() - p.x());
		disttmp += abs(tmp->point().y() - p.y()) * abs(tmp->point().y() - p.y());
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

QList<QWebPoint> QWeb::pointlist()
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

