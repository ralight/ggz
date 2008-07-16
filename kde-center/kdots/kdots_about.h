/////////////////////////////////////////////////////////////////////
//
// KDots: Connect the Dots game for KDE 4
// http://www.ggzgamingzone.org/gameclients/kdots/
//
// Copyright (C) 2001 - 2008 Josef Spillner <josef@ggzgamingzone.org>
// Published under the conditions of the GNU GPL, see COPYING
//
/////////////////////////////////////////////////////////////////////

#ifndef KDOTS_ABOUT_H
#define KDOTS_ABOUT_H

#include <qwidget.h>
#include <qevent.h>

class QPixmap;

class KDotsAbout : public QWidget
{
	Q_OBJECT
	public:
		KDotsAbout();
		~KDotsAbout();
	protected:
		void paintEvent(QPaintEvent *e);
		void mouseMoveEvent(QMouseEvent *e);
		void mousePressEvent(QMouseEvent *e);
	private:
		QString measure(QString s);

		bool m_setup;
		bool m_highlight;
		QFont m_font;
		QPixmap *m_bg;
};

#endif
