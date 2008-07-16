/////////////////////////////////////////////////////////////////////
//
// KDots: Connect the Dots game for KDE 4
// http://www.ggzgamingzone.org/gameclients/kdots/
//
// Copyright (C) 2001 - 2008 Josef Spillner <josef@ggzgamingzone.org>
// Published under the conditions of the GNU GPL, see COPYING
//
/////////////////////////////////////////////////////////////////////

#ifndef KDOTS_HELP_H
#define KDOTS_HELP_H

#include <qwidget.h>
#include <qevent.h>

class QPixmap;

class KDotsHelp : public QWidget
{
	Q_OBJECT
	public:
		KDotsHelp();
		~KDotsHelp();
	protected:
		void paintEvent(QPaintEvent *e);
	private:
		QString measure(QString s);

		bool m_setup;
		QFont m_font;
		QPixmap *m_bg;
};

#endif

