#ifndef QWEB_H
#define QWEB_H

#include <qlist.h>
#include <qpoint.h>

class QWebPoint
{
	public:
		QWebPoint(QPoint p);
		~QWebPoint();
		void addPeer(QPoint p);
		QList<QPoint> peerlist();
		QPoint point();
		void setData(int data);
		int data();
	private:
		QPoint m_point;
		QList<QPoint> m_peerlist;
		int m_data;
};

class QWeb
{
	public:
		QWeb();
		~QWeb();
		void addPoint(QPoint p);
		void addPeer(QPoint p, QPoint p2);
		QPoint getPoint(QPoint p);
		QList<QWebPoint> pointlist();
		void setScale(float factor);
		float scale();
		void setData(QPoint p, int data);
		int data(QPoint p);
	private:
		QList<QWebPoint> m_pointlist;
		QWebPoint *tmp;
		float m_scale;
};

#endif

