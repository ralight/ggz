///////////////////////////////////////////////////////////////
//
// Dots
// C++ Class Set for Connect the Dots games
// Copyright (C) 2001 Josef Spillner
// dr_maux@users.sourceforge.net
// The MindX Open Source Project
// http://mindx.sourceforge.net/games/kdots/
//
///////////////////////////////////////////////////////////////

#ifndef QDOTS_H
#define QDOTS_H

#include "vdots.h"
#include <qwidget.h>
#include <qevent.h>

class QDots : public QWidget, public VDots
{
	Q_OBJECT
	public:
		QDots(QWidget* parent = 0, char* name = 0);
		~QDots();
		void refreshBoard();
	signals:
		void signalTurn(int x, int y, int direction);
	protected:
		void resizeEvent(QResizeEvent *e);
		void paintEvent(QPaintEvent *e);
		void mousePressEvent(QMouseEvent *e);
	private:
		int m_xoffset, m_yoffset;
};

#endif
