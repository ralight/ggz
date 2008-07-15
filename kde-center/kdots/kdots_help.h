///////////////////////////////////////////////////////////////
//
// KDots
// Connect the Dots game for KDE, using the Dots classes
// Copyright (C) 2001, 2002 Josef Spillner
// dr_maux@users.sourceforge.net
// The MindX Open Source Project
// http://mindx.sourceforge.net/games/kdots/
//
///////////////////////////////////////////////////////////////

#ifndef KDOTS_HELP_H
#define KDOTS_HELP_H

#include <qwidget.h>
#include <QEvent>

class KDotsHelp : public QWidget
{
	Q_OBJECT
	public:
		KDotsHelp();
		~KDotsHelp();
	public Q_SLOTS:
		void slotAccepted();
	protected:
		void paintEvent(QPaintEvent *e);
	private:
		QString measure(QString s);
		int m_repaint;
		QFont m_font;
};

#endif

