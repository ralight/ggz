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

#ifndef KDOTS_H
#define KDOTS_H

#include <qwidget.h>
#include <qevent.h>

class KDots : public QWidget
{
	Q_OBJECT
	public:
		KDots(QWidget *parent, char *name);
		~KDots();
	public slots:
		void slotOptions();
		void slotAbout();
		void slotQuit();
		void slotStart(int horizontal, int vertical);
		void slotHelp();
		void slotTurn(int x, int y, int direction);
	private:
		int m_rows;
		int m_cols;
		int m_running;
		int *m_field;
};

#endif
