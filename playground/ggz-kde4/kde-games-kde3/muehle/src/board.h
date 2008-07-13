// Muehle - KDE Muehle (Nine Men's Morris) game for GGZ
// Copyright (C) 2001, 2002 Josef Spillner, dr_maux@users.sourceforge.net
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#ifndef MUEHLE_BOARD_H
#define MUEHLE_BOARD_H

// Qt includes
#include <qwidget.h>
#include <qptrlist.h>

// Class declarations
class Stone;
class QWeb;
class QPixmap;
class Net;
class QPainter;

// The game board containing the Muehle
class Board : public QWidget
{
	Q_OBJECT
	public:
		Board(QWidget *parent = NULL, const char *name = NULL);
		~Board();
		void init();
		void remis();
		void loose();
		void check(int x, int y, Stone *stone);
		void setTheme(const QString &theme);
		void setVariant(const QString &variant);
		void enableNetwork(bool enabled);

	public slots:
		void slotInput();

	signals:
		void signalStatus(const QString &message);
		void signalScore(const QString &player, int num, int score, int stones);
		void signalEnd();

	protected:
		void paintEvent(QPaintEvent *e);
		void resizeEvent(QResizeEvent *e);
		void mousePressEvent(QMouseEvent *e);
		void timerEvent(QTimerEvent *e);

	private:
		void paintStone(QPixmap *tmp, QPainter *p, int x, int y, int owner);
		void resetStones();
		void updateStatus();

		enum Colors
		{
			colornone,
			colorwhite,
			colorblack,
			colorspectator
		};
		enum Phases
		{
			phasestart,
			phaseset,
			phasemove
		};

		QPtrList<Stone> stonelist;
		QWeb *web;
		QPixmap *bg;
		QPixmap *black, *white;
		Net *net;
		int m_color;
		int m_phase;
		int m_turn;
		int m_take;
		int m_wait;
		int m_whitescore, m_blackscore;
		int m_whitestones, m_blackstones;
		int m_repaint;
		int m_width, m_height;
};

#endif

