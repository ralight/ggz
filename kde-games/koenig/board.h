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
		~ChessBoard(void);

		void resetBoard(int color);

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
};

#endif

