#ifndef QWEBPATH_H
#define QWEBPATH_H

#include <qlist.h>
#include <qpoint.h>

class QWeb;
class QWebPoint;

class QWebPath
{
	public:
		QWebPath(QWeb *web, QWebPoint *point);
		~QWebPath();
		void create(int length);
		QList<QWebPoint> pathlist();

	private:
		void findRecursive(QWebPoint *old, QWebPoint *origin, int length);

		QList<QWebPoint> m_pathlist;
		QWeb *m_web;
		QWebPoint *m_wp;
};

#endif

