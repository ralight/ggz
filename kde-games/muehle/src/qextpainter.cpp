#include "qextpainter.h"
#include <iostream>

QExtPainter::QExtPainter()
: QPainter()
{
}

QExtPainter::~QExtPainter()
{
}

void QExtPainter::drawWeb(QWeb w)
{
	QList<QWebPoint> wplist;
	QList<QPoint> plist;
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

