#include "qwebpath.h"
#include "qweb.h"
#include <iostream>

using namespace std;

QWebPath::QWebPath(QWeb *web, QWebPoint *point)
{
	m_web = web;
	m_wp = point;
}

QWebPath::~QWebPath()
{
	m_pathlist.clear();
}

void QWebPath::create(int length)
{
	m_pathlist.clear();

	findRecursive(NULL, m_wp, length);
}

QList<QWebPoint> QWebPath::pathlist()
{
	return m_pathlist;
}

void QWebPath::findRecursive(QWebPoint *old, QWebPoint *origin, int length)
{
	QList<QWebPoint> pointlist;
	QList<QPoint> peerlist;
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
			cout << "invalid point: " << p->x() << "," << p->y() << endl;
			continue;
		}

		if(wp != old)
		{
			cout << "level " << length << ", peer: " << p->x() << "," << p->y() << endl;
			findRecursive(origin, wp, length - 1);
		}
		else cout << "left out: " << p->x() << "," << p->y() << endl;
	}
}

