// Koenig - KDE client for the GGZ chess game
// Copyright (C) 2001 Tobias König, tokoe82@yahoo.de
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

#ifndef CHESS_BOARD_H
#define CHESS_BOARD_H

#include <qevent.h>
#include <qwidget.h>
#include <qpixmap.h>

class ChessBoard : public QWidget
{
	Q_OBJECT
	public:
		ChessBoard(QWidget *parent = 0, const char *name = 0);
		~ChessBoard();

		void resetBoard(int color);
		void setTheme(QString theme);

		enum Figures
		{
			pawn_white = 1,
			pawn_black = 2,
			bishop_white = 3,
			bishop_black = 4,
			knight_white = 5,
			knight_black = 6,
			rook_white = 7,
			rook_black = 8,
			queen_white = 9,
			queen_black = 10,
			king_white = 11,
			king_black = 12
		};

		enum Colors
		{
			color_white = 1,
			color_black = 2,
			color_inactive = 3
		};

	public slots:
		void moveFigure(int xfrom, int yfrom, int xto, int yto);

	signals:
		void figureMoved(int xfrom, int yfrom, int xto, int yto);

	protected:
		virtual void paintEvent(QPaintEvent *e);
		virtual void mouseMoveEvent(QMouseEvent *e);
		virtual void dragEnterEvent(QDragEnterEvent *e);
		virtual void dropEvent(QDropEvent *e);

	private:
		int board[8][8];
		QPixmap pixmaps[13];
		QPixmap dragPixmap;
		QPoint dragStart;
		bool mouseDrag;

		int activeColor;

		QPixmap scalePixmap(const QPixmap&);
		QPixmap svgPixmap(QString filename);
};

#endif

