/////////////////////////////////////////////////////////////////////
//
// Dots: C++ classes for Connect the Dots games
// http://www.ggzgamingzone.org/gameclients/kdots/
//
// Copyright (C) 2001 - 2008 Josef Spillner <josef@ggzgamingzone.org>
// Published under the conditions of the GNU GPL, see COPYING
//
/////////////////////////////////////////////////////////////////////

#ifndef QDOTS_H
#define QDOTS_H

#include "vdots.h"

#include <qwidget.h>
#include <qpixmap.h>

class QMouseEvent;
class QPaintEvent;
class QResizeEvent;

class QDots : public QWidget, public VDots
{
	Q_OBJECT
	public:
		QDots();
		~QDots();
		void refreshBoard();
		void setBackgroundImage(QPixmap bg);
	Q_SIGNALS:
		void signalTurn(int x, int y, int direction);
	protected:
		void resizeEvent(QResizeEvent *e);
		void paintEvent(QPaintEvent *e);
		void mousePressEvent(QMouseEvent *e);
	private:
		void grayscale(QPixmap *pix);

		int m_xoffset, m_yoffset;
		QPixmap *m_bg;
};

#endif

