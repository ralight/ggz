//#include <kiconloader.h>
#include "config.h"

#include <kdebug.h>

#include <qdragobject.h>
#include <qimage.h>
#include <qpainter.h>
#include <qpoint.h>
#include <qwidget.h>
#include <qwmatrix.h>

#include "board.h"
#include "board.moc"

ChessBoard::ChessBoard(QWidget *parent, const char *name)
	: QWidget(parent, name)
{
	kdDebug(12101) << "ChessBoard::ChessBoard()" << endl;

	setMinimumSize(240, 240);

	setAcceptDrops(true);

	// load figure pixmaps
	//KIconLoader loader;
	//pixmaps[1] = loader.loadIcon("pawn_w", KIcon::Desktop);
	//pixmaps[2] = loader.loadIcon("pawn_b", KIcon::Desktop);
	//pixmaps[3] = loader.loadIcon("bishop_w", KIcon::Desktop);
	//pixmaps[4] = loader.loadIcon("bishop_b", KIcon::Desktop);
	//pixmaps[5] = loader.loadIcon("knight_w", KIcon::Desktop);
	//pixmaps[6] = loader.loadIcon("knight_b", KIcon::Desktop);
	//pixmaps[7] = loader.loadIcon("rook_w", KIcon::Desktop);
	//pixmaps[8] = loader.loadIcon("rook_b", KIcon::Desktop);
	//pixmaps[9] = loader.loadIcon("queen_w", KIcon::Desktop);
	//pixmaps[10] = loader.loadIcon("queen_b", KIcon::Desktop);
	//pixmaps[11] = loader.loadIcon("king_w", KIcon::Desktop);
	//pixmaps[12] = loader.loadIcon("king_b", KIcon::Desktop);

	pixmaps[1] = QPixmap(GGZDATADIR "/koenig/pics/pawn_w.xpm");
	pixmaps[2] = QPixmap(GGZDATADIR "/koenig/pics/pawn_b.xpm");
	pixmaps[3] = QPixmap(GGZDATADIR "/koenig/pics/bishop_w.xpm");
	pixmaps[4] = QPixmap(GGZDATADIR "/koenig/pics/bishop_b.xpm");
	pixmaps[5] = QPixmap(GGZDATADIR "/koenig/pics/pawn_w.xpm");
	pixmaps[6] = QPixmap(GGZDATADIR "/koenig/pics/pawn_b.xpm");
	pixmaps[7] = QPixmap(GGZDATADIR "/koenig/pics/knight_w.xpm");
	pixmaps[8] = QPixmap(GGZDATADIR "/koenig/pics/knight_b.xpm");
	pixmaps[9] = QPixmap(GGZDATADIR "/koenig/pics/queen_w.xpm");
	pixmaps[10] = QPixmap(GGZDATADIR "/koenig/pics/queen_b.xpm");
	pixmaps[11] = QPixmap(GGZDATADIR "/koenig/pics/king_w.xpm");
	pixmaps[12] = QPixmap(GGZDATADIR "/koenig/pics/king_b.xpm");

	// I like QuickHacks ;)
	mouseDrag = true;
	
	resetBoard(COLOR_BLACK); // FIXME: invent COLOR_INACTIVE !!!

	setCaption("Chess Board - inactive");
}

ChessBoard::~ChessBoard(void)
{
}

void ChessBoard::resetBoard(int color)
{
	// cleanup the virtual chessboard
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			board[i][j] = 0;
		}
	}

	board[0][0] = ROOK_WHITE;
	board[1][0] = KNIGHT_WHITE;
	board[2][0] = BISHOP_WHITE;
	board[3][0] = QUEEN_WHITE;
	board[4][0] = KING_WHITE;
	board[5][0] = BISHOP_WHITE;
	board[6][0] = KNIGHT_WHITE;
	board[7][0] = ROOK_WHITE;

	for (int i = 0; i < 8; i++) {
		board[i][1] = PAWN_WHITE;
		board[i][6] = PAWN_BLACK;
	}

	board[0][7] = ROOK_BLACK;
	board[1][7] = KNIGHT_BLACK;
	board[2][7] = BISHOP_BLACK;
	board[3][7] = QUEEN_BLACK;
	board[4][7] = KING_BLACK;
	board[5][7] = BISHOP_BLACK;
	board[6][7] = KNIGHT_BLACK;
	board[7][7] = ROOK_BLACK;

	activeColor = color;
	if(color == COLOR_WHITE)
		setCaption("Chess Board - White");
	else
		setCaption("Chess Board - Black");

	update();
}

void ChessBoard::resizeEvent(QResizeEvent *event)
{
	int size = (event->size().width()+event->size().height())/2/8;

	resize(size * 8, size * 8);
}

void ChessBoard::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	
	painter.setClipRect(event->rect());

	int cellSize = width()/8;

	bool dark = true;
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if (dark) {
				painter.setBrush(QColor(0x67, 0x55, 0x3F));
				dark = false;
			} else {
				painter.setBrush(QColor(0xC8, 0xB5, 0x72));
				dark = true;
			}			
			painter.fillRect(i*cellSize, j*cellSize, i*cellSize + cellSize, j*cellSize + cellSize, painter.brush());
			if (board[i][j] != 0)
				painter.drawPixmap(i*cellSize, j*cellSize, scalePixmap(pixmaps[board[i][j]]));
		}
		dark = !dark;
	}
}

void ChessBoard::dragEnterEvent(QDragEnterEvent *event)
{

	event->accept();
}

void ChessBoard::mouseMoveEvent(QMouseEvent *)
{
	if (mouseDrag) {
		dragStart = mapFromGlobal(QCursor::pos())/(width()/8);
		if (((board[dragStart.x()][dragStart.y()]%2) == (activeColor%2)) && (board[dragStart.x()][dragStart.y()] != 0)) {
			dragPixmap = scalePixmap(pixmaps[board[dragStart.x()][dragStart.y()]]);
			mouseDrag = false;
		} else
			return;
	}

	QImage image;
	QDragObject *drag_img = new QImageDrag(image, this);
	drag_img->setPixmap(dragPixmap, QPoint(32*(1.0/(64.0/(width()/8))), 32*(1.0/(64.0/(width()/8)))));
	drag_img->dragMove();
}

void ChessBoard::dropEvent(QDropEvent *event)
{
	QPoint point = mapFromGlobal(QCursor::pos())/(width()/8);
	if ((((board[point.x()][point.y()])%2) != ((board[dragStart.x()][dragStart.y()])%2)) || (board[point.x()][point.y()] == 0)) {
		board[point.x()][point.y()] = board[dragStart.x()][dragStart.y()];
		board[dragStart.x()][dragStart.y()] = 0;

		int cellSize = width()/8;
		repaint(dragStart.x()*cellSize, dragStart.y()*cellSize, cellSize, cellSize, false);
		repaint(point.x()*cellSize, point.y()*cellSize, cellSize, cellSize, false);

		emit figureMoved(dragStart.x(), dragStart.y(), point.x(), point.y());
	}
	mouseDrag = true;
}

void ChessBoard::moveFigure(int xfrom, int yfrom, int xto, int yto)
{
	board[xto][yto] = board[xfrom][yfrom];
	board[xfrom][yfrom] = 0;

	int cellSize = width()/8;
	repaint(xfrom*cellSize, yfrom*cellSize, cellSize, cellSize, false);
	repaint(xto*cellSize, yto*cellSize, cellSize, cellSize, false);
}

QPixmap ChessBoard::scalePixmap(const QPixmap& pixmap)
{
	QWMatrix matrix;
	QPixmap tmp = pixmap;
	int cellSize = width()/8;

	matrix.scale(1.0/(64.0/cellSize), 1.0/(64.0/cellSize)); 
	return tmp.xForm(matrix);
}
