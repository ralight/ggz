// Koenig - KDE client for the GGZ chess game
// Copyright (C) 2001 Tobias König, tokoe82@yahoo.de
// Copyright (C) 2001 - 2004 Josef Spillner <josef@ggzgamingzone.org>
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

#include "config.h"

#include <kdebug.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <ksimpleconfig.h>

#include <qdragobject.h>
#include <qimage.h>
#include <qpainter.h>
#include <qpoint.h>
#include <qwidget.h>
#include <qwmatrix.h>
#include <qcursor.h>
#include <qfile.h>

#ifdef HAVE_RSVG
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <librsvg/rsvg.h>
#endif

#include "board.h"
#include "board.moc"

ChessBoard::ChessBoard(QWidget *parent, const char *name)
	: QWidget(parent, name)
{
	KStandardDirs d;

	kdDebug(12101) << "ChessBoard::ChessBoard()" << endl;

	// load figure pixmaps
	QString pixmapdir = d.findResource("data", "koenig/pics/");
	setTheme(pixmapdir);

	// I like QuickHacks ;)
	mouseDrag = true;

	// initialize the board
	resetBoard(color_inactive);
}

ChessBoard::~ChessBoard(void)
{
}

void ChessBoard::setTheme(QString pixmapdir)
{
	pixmapdir += "/";

	if(QFile::exists(pixmapdir + "theme.conf"))
	{
		KSimpleConfig conf(pixmapdir + "theme.conf");

		conf.setGroup("2DWhite");
		pixmaps[pawn_white] = svgPixmap(pixmapdir + conf.readEntry("Pawn"));
		pixmaps[bishop_white] = svgPixmap(pixmapdir + conf.readEntry("Bishop"));
		pixmaps[rook_white] = svgPixmap(pixmapdir + conf.readEntry("Rook"));
		pixmaps[knight_white] = svgPixmap(pixmapdir + conf.readEntry("Knight"));
		pixmaps[queen_white] = svgPixmap(pixmapdir + conf.readEntry("Queen"));
		pixmaps[king_white] = svgPixmap(pixmapdir + conf.readEntry("King"));

		conf.setGroup("2DBlack");
		pixmaps[pawn_black] = svgPixmap(pixmapdir + conf.readEntry("Pawn"));
		pixmaps[bishop_black] = svgPixmap(pixmapdir + conf.readEntry("Bishop"));
		pixmaps[rook_black] = svgPixmap(pixmapdir + conf.readEntry("Rook"));
		pixmaps[knight_black] = svgPixmap(pixmapdir + conf.readEntry("Knight"));
		pixmaps[queen_black] = svgPixmap(pixmapdir + conf.readEntry("Queen"));
		pixmaps[king_black] = svgPixmap(pixmapdir + conf.readEntry("King"));
	}
	else
	{
		pixmaps[pawn_white] = svgPixmap(pixmapdir + "pawn_w.svg");
		pixmaps[pawn_black] = svgPixmap(pixmapdir + "pawn_b.svg");
		pixmaps[bishop_white] = svgPixmap(pixmapdir + "bishop_w.svg");
		pixmaps[bishop_black] = svgPixmap(pixmapdir + "bishop_b.svg");
		pixmaps[rook_white] = svgPixmap(pixmapdir + "rook_w.svg");
		pixmaps[rook_black] = svgPixmap(pixmapdir + "rook_b.svg");
		pixmaps[knight_white] = svgPixmap(pixmapdir + "knight_w.svg");
		pixmaps[knight_black] = svgPixmap(pixmapdir + "knight_b.svg");
		pixmaps[queen_white] = svgPixmap(pixmapdir + "queen_w.svg");
		pixmaps[queen_black] = svgPixmap(pixmapdir + "queen_b.svg");
		pixmaps[king_white] = svgPixmap(pixmapdir + "king_w.svg");
		pixmaps[king_black] = svgPixmap(pixmapdir + "king_b.svg");
	}

	update();
}

QPixmap ChessBoard::svgPixmap(QString filename)
{
	QPixmap tmp;
#ifdef HAVE_RSVG
	GError *error = NULL;
	const int width = 128;
	const int height = 128;

	g_type_init();
	GdkPixbuf *buf = rsvg_pixbuf_from_file_at_size(filename.latin1(), width, height, &error);

	if(buf)
	{
		guchar *foo = gdk_pixbuf_get_pixels(buf);
		for(int i = 0; i < width; i++)
			for(int j = 0; j < height; j++)
			{
				unsigned int pixel = *(unsigned int*)(foo + (j * width + i) * 4);
				int a, r, g, b;
				r = (pixel >> 24) & 0xFF;
				g = (pixel >> 16) & 0xFF;
				b = (pixel >> 8) & 0xFF;
				a = (pixel >> 0) & 0xFF;
				pixel = (a << 24) + (r << 16) + (g << 8) + (b << 0);
				*(unsigned int*)(foo + (j * width + i) * 4) = pixel;
			}

		QImage im(gdk_pixbuf_get_pixels(buf), gdk_pixbuf_get_width(buf), gdk_pixbuf_get_height(buf),
			gdk_pixbuf_get_bits_per_sample(buf) * 4, 0, 0, QImage::IgnoreEndian);
		im.setAlphaBuffer(true);
		tmp.convertFromImage(im);
	}
	else tmp = QPixmap(filename + ".png");
#else
	tmp = QPixmap(filename + ".png");
#endif
	if(tmp.isNull()) tmp = QPixmap(filename.replace("svg", "png"));
	return tmp;
}

void ChessBoard::resetBoard(int color)
{
	// cleanup the virtual chessboard
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
			board[i][j] = 0;

	// now fill up with the figures
	board[0][0] = rook_white;
	board[1][0] = knight_white;
	board[2][0] = bishop_white;
	board[3][0] = queen_white;
	board[4][0] = king_white;
	board[5][0] = bishop_white;
	board[6][0] = knight_white;
	board[7][0] = rook_white;

	for (int i = 0; i < 8; i++)
	{
		board[i][1] = pawn_white;
		board[i][6] = pawn_black;
	}

	board[0][7] = rook_black;
	board[1][7] = knight_black;
	board[2][7] = bishop_black;
	board[3][7] = queen_black;
	board[4][7] = king_black;
	board[5][7] = bishop_black;
	board[6][7] = knight_black;
	board[7][7] = rook_black;

	// assign the correct color
	activeColor = color;
	if(color == color_white)
		parentWidget()->setCaption(i18n("Chess Board - White"));
	else if(color == color_black)
		parentWidget()->setCaption(i18n("Chess Board - Black"));
	else
		parentWidget()->setCaption(i18n("Chess Board - No color assigned yet"));

	// accept drag'n'drop actions
	if(color != color_inactive)
		setAcceptDrops(true);

	// update the whole scenary
	update();
}

void ChessBoard::paintEvent(QPaintEvent *e)
{
	QPainter painter(this);
	
	painter.setClipRect(e->rect());

	int cellSize = width() / 8;

	bool dark = true;
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			if (dark)
				painter.setBrush(QColor(0x67, 0x55, 0x3F));
			else
				painter.setBrush(QColor(0xC8, 0xB5, 0x72));
			dark = !dark;
			painter.fillRect(i * cellSize, j * cellSize, i * cellSize + cellSize, j * cellSize + cellSize, painter.brush());
			if (board[i][j] != 0)
				painter.drawPixmap(i * cellSize, j * cellSize, scalePixmap(pixmaps[board[i][j]]));
		}
		dark = !dark;
	}
}

void ChessBoard::dragEnterEvent(QDragEnterEvent *e)
{
	e->accept();
}

void ChessBoard::mouseMoveEvent(QMouseEvent *e)
{
	int x, y;

	if (mouseDrag)
	{
		dragStart = mapFromGlobal(QCursor::pos()) / (width() / 8);
		x = dragStart.x();
		y = dragStart.y();
		dragStart.setX(x);
		dragStart.setY(y);
		if (((board[x][y] % 2) == (activeColor % 2)) && (board[x][y] != 0))
		{
			dragPixmap = scalePixmap(pixmaps[board[x][y]]);
			mouseDrag = false;
		}
		else return;
	}

	QImage image;
	QDragObject *drag_img = new QImageDrag(image, this);
	drag_img->setPixmap(dragPixmap, QPoint((int)(32.0 / (64.0 / (width() / 8))),
		(int)(32.0 / (64.0 / (width() / 8)))));
	drag_img->dragMove();
}

void ChessBoard::dropEvent(QDropEvent *e)
{
	int x, y;

	QPoint point = mapFromGlobal(QCursor::pos()) / (width() / 8);
	x = point.x();
	y = point.y();

	if ((((board[dragStart.x()][dragStart.y()]) % 2) != ((board[x][y]) % 2)) || (board[x][y] == 0))
	{
		// Only accept moves from the network
		//board[x][y] = board[dragStart.x()][dragStart.y()];
		//board[dragStart.x()][dragStart.y()] = 0;

		int cellSize = width() / 8;
		repaint(dragStart.x() * cellSize, dragStart.y() * cellSize, cellSize, cellSize, false);
		repaint(x * cellSize, y * cellSize, cellSize, cellSize, false);

		kdDebug(12101) << "ChessBoard::dropEvent(); got move!" << endl;
		emit figureMoved(dragStart.x(), dragStart.y(), x, y);
	}
	mouseDrag = true;
}

void ChessBoard::moveFigure(int xfrom, int yfrom, int xto, int yto)
{
	kdDebug(12101) << "move figure: " << xfrom << "/" << yfrom << " -> " << xto << "/" << yto << endl;
	if(board[xfrom][yfrom])
	{
		board[xto][yto] = board[xfrom][yfrom];
		board[xfrom][yfrom] = 0;
	}
	// else it has already moved // FIXME!!! don't move until server says so,
	// or prevent double moves otherwise (same for tabctl 'moves')

	int cellSize = width() / 8;
	repaint(xfrom * cellSize, yfrom * cellSize, cellSize, cellSize, false);
	repaint(xto * cellSize, yto * cellSize, cellSize, cellSize, false);
}

QPixmap ChessBoard::scalePixmap(const QPixmap& pixmap)
{
	QWMatrix matrix;
	QPixmap tmp = pixmap;
	int cellSize = width() / 8;

	matrix.scale(1.0 / ((float)pixmap.width() / cellSize), 1.0 / ((float)pixmap.height() / cellSize));
	return tmp.xForm(matrix);
}

