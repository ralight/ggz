#ifndef _chess_board_h
#define _chess_board_h

#include <qevent.h>
#include <qwidget.h>
#include <qpixmap.h>

#define PAWN_WHITE	1	// Bauer
#define PAWN_BLACK	2
#define BISHOP_WHITE	3	// Laeufer
#define BISHOP_BLACK	4
#define KNIGHT_WHITE	5	// Springer
#define KNIGHT_BLACK	6
#define ROOK_WHITE	7	// Turm
#define ROOK_BLACK	8
#define QUEEN_WHITE	9	// Dame
#define QUEEN_BLACK	10
#define KING_WHITE	11	// Koenig
#define KING_BLACK	12

#define COLOR_WHITE	1
#define COLOR_BLACK	2

class ChessBoard : public QWidget
{
	Q_OBJECT
public:
	ChessBoard(QWidget *parent = 0, const char *name = 0);
	~ChessBoard(void);

	void resetBoard(int = COLOR_WHITE);

public slots:
	void moveFigure(int xfrom, int yfrom, int xto, int yto);

signals:
	void figureMoved(int xfrom, int yfrom, int xto, int yto);

protected:
	virtual void paintEvent(QPaintEvent *);
	virtual void resizeEvent(QResizeEvent *);
	virtual void mouseMoveEvent(QMouseEvent *);
	virtual void dragEnterEvent(QDragEnterEvent *);
	virtual void dropEvent(QDropEvent *);

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
