///////////////////////////////////////////////////////////////
//
// KDots
// Connect the Dots game for KDE, using the Dots classes
// Copyright (C) 2001 Josef Spillner
// dr_maux@users.sourceforge.net
// The MindX Open Source Project
// http://mindx.sourceforge.net/games/kdots/
//
///////////////////////////////////////////////////////////////

#ifndef KDOTS_HELP_H
#define KDOTS_HELP_H

#include <qwidget.h>
#include <qevent.h>

class KDotsHelp : public QWidget
{
	Q_OBJECT
	public:
		KDotsHelp(QWidget *parent, char *name);
		~KDotsHelp();
	public slots:
		void slotAccepted();
	protected:
		void paintEvent(QPaintEvent *e);
	private:
		QWidget *m_pane;
};

#endif
