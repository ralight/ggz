// Krosswater - Cross The Water for KDE
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

#ifndef QCW_H
#define QCW_H

// Qt includes
#include <qwidget.h>
#include <qevent.h>
#include <qtimer.h>

// Forward declarations
class QPixmap;

// Generic Krosswater game board
class QCw : public QWidget
{
	Q_OBJECT
	public:
		QCw(QWidget* parent = NULL, const char* name = NULL);
		~QCw();
		void setSize(int width, int height);
		void setStone(int x, int y, int value);
		void setStoneState(int x, int y, int state);
		void resetPlayers();		
		void addPlayer(int x, int y);
		void setPlayerPixmap(int player, int pixmap);
		void setPlayerTurn(int player);
		void disable();
		void enable();
		enum StoneStates
		{
			inactive = 0,
			active = 1,
			active2 = 2,
			path = 3,
			fromframe = 4,
			toframe = 5
		};

	signals:
		void signalMove(int xfrom, int yfrom, int xto, int yto);

	protected:
		void paintEvent(QPaintEvent *e);
		void mousePressEvent(QMouseEvent *e);
		void timerEvent(QTimerEvent *e);

	private:
		enum states
		{
			normal,
			selected
		};
		int m_width, m_height;
		int ***m_board;
		int m_x, m_y;
		int m_state;
		int m_players[4][3];
		int m_numplayers;
		QPixmap *m_pix;
		int m_update;
		int m_allupdate;
		int m_enabled;
		int m_turn;
};

#endif

