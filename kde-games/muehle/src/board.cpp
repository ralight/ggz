#include "board.h"
#include <qpixmap.h>
#include "qextpainter.h"
#include <qpen.h>
#include <kdebug.h>
#include "stone.h"
#include "qweb.h"
#include "net.h"
#include <qimage.h>
#include "qwebpath.h"

Board::Board(QWidget *parent, const char *name)
: QWidget(parent, name)
{
	net = new Net();
	net->output("KDE Muehle Game");

	//setBackgroundPixmap(QPixmap("../pics/bg.png"));
	bg = new QPixmap("../pics/bg.png");

	web = new QWeb();
	// connections outer ring
	web->addPeer(QPoint(10, 10), QPoint(50, 10));
	web->addPeer(QPoint(50, 10), QPoint(90, 10));
	web->addPeer(QPoint(90, 10), QPoint(90, 50));
	web->addPeer(QPoint(90, 50), QPoint(90, 90));
	web->addPeer(QPoint(90, 90), QPoint(50, 90));
	web->addPeer(QPoint(50, 90), QPoint(10, 90));
	web->addPeer(QPoint(10, 90), QPoint(10, 50));
	web->addPeer(QPoint(10, 50), QPoint(10, 10));
	// connections middle ring
	web->addPeer(QPoint(20, 20), QPoint(50, 20));
	web->addPeer(QPoint(50, 20), QPoint(80, 20));
	web->addPeer(QPoint(80, 20), QPoint(80, 50));
	web->addPeer(QPoint(80, 50), QPoint(80, 80));
	web->addPeer(QPoint(80, 80), QPoint(50, 80));
	web->addPeer(QPoint(50, 80), QPoint(20, 80));
	web->addPeer(QPoint(20, 80), QPoint(20, 50));
	web->addPeer(QPoint(20, 50), QPoint(20, 20));
	// connections inner ring
	web->addPeer(QPoint(30, 30), QPoint(50, 30));
	web->addPeer(QPoint(50, 30), QPoint(70, 30));
	web->addPeer(QPoint(70, 30), QPoint(70, 50));
	web->addPeer(QPoint(70, 50), QPoint(70, 70));
	web->addPeer(QPoint(70, 70), QPoint(50, 70));
	web->addPeer(QPoint(50, 70), QPoint(30, 70));
	web->addPeer(QPoint(30, 70), QPoint(30, 50));
	web->addPeer(QPoint(30, 50), QPoint(30, 30));
	// connections transring
	web->addPeer(QPoint(10, 50), QPoint(20, 50));
	web->addPeer(QPoint(20, 50), QPoint(30, 50));
	web->addPeer(QPoint(70, 50), QPoint(80, 50));
	web->addPeer(QPoint(80, 50), QPoint(90, 50));
	web->addPeer(QPoint(50, 10), QPoint(50, 20));
	web->addPeer(QPoint(50, 20), QPoint(50, 30));
	web->addPeer(QPoint(50, 70), QPoint(50, 80));
	web->addPeer(QPoint(50, 80), QPoint(50, 90));

	stonelist.setAutoDelete(true);
}

Board::~Board()
{
	delete web;
	delete bg;
	delete net;
}

void Board::init()
{
	stonelist.clear();
	repaint();
}

void Board::paintStone(QPixmap *tmp, QPainter *p, int x, int y, int owner)
{
	QPixmap pix;
	QImage im, im2;
	int r, g, b, a;
	QRgb rgba, rgba2;
	int r2, g2, b2;

	QPixmap black("../pics/black.png");
	QPixmap white("../pics/white.png");

	if((owner == Stone::white) || (owner == Stone::whiteactive) || (owner == Stone::whitemuehle))
		pix = white;
	if((owner == Stone::black) || (owner == Stone::blackactive) || (owner == Stone::blackmuehle))
		pix = black;

	if((owner == Stone::whiteactive) || (owner == Stone::blackactive)
	|| (owner == Stone::whitemuehle) || (owner == Stone::blackmuehle))
	{
		im = pix.convertToImage();
		//im2 = tmp->convertToImage();
		for(int j = 0; j < im.height(); j++)
			for(int i = 0; i < im.width(); i++)
			{
				rgba = im.pixel(i, j);
				r = qRed(rgba);
				g = qGreen(rgba);
				b = qBlue(rgba);
				a = qAlpha(rgba);
				/*if(a)
				{
					rgba2 = im2.pixel(i, j);
					r2 = qRed(rgba2);
					g2 = qGreen(rgba2);
					b2 = qBlue(rgba2);
					r = (int)(r / 2.0 + r2 / 2.0);
					g = (int)(g / 2.0 + g2 / 2.0);
					b = (int)(b / 2.0 + b2 / 2.0);
				}*/
				r = (int)(r / 2.0);
				g = (int)(g / 2.0);
				b = (int)(b / 2.0);
				im.setPixel(i, j, qRgba(r, g, b, a));
			}
		pix.convertFromImage(im);
	}

	p->drawPixmap(x, y, pix);
}

void Board::paintEvent(QPaintEvent *e)
{
	QExtPainter p;
	QPixmap tmp;

	//bg->resize(width(), height());
	tmp.resize(width(), height());
	p.begin(&tmp);
	p.setPen(QPen(QColor(0, 0, 0), 3));
	p.drawTiledPixmap(0, 0, width(), height(), *bg);

	p.drawWeb(*web);

	float x = web->scale();
	for(Stone *s = stonelist.first(); s; s = stonelist.next())
		//p.drawPixmap(x * s->x() - 32, x * s->y() - 32, (s->owner() ? black : white));
		paintStone(&tmp, &p, x * s->x() - 32, x * s->y() - 32, s->owner());

	p.end();

	setBackgroundPixmap(tmp);
}

void Board::resizeEvent(QResizeEvent *e)
{
	if(width() != height()) resize(width(), width());
	else
	{
		repaint();
	}

	web->setScale(width() / 100.0);
}

void Board::mousePressEvent(QMouseEvent *e)
{
	int x, y, xrow, yrow, xrows, yrows;
	QPoint final;
	int count, active;
	Stone *stone, *astone;
	QList<QWebPoint> pointlist;
	QList<QPoint> peerlist;
	QWebPoint *wp;

	// Translate coordinates into qweb representation
	x = (int)(e->x() / web->scale());
	y = (int)(e->y() / web->scale());
	kdDebug(12101) << "Click at: " << x << ", " <<  y << endl;

	// Find out whether a point was hit or not
	final = web->getPoint(QPoint(x, y));
	if(!final.isNull())
	{
		x = final.x();
		y = final.y();
		kdDebug(12101) << "That is: " << x << ", " << y << endl;

		// Find out whether a stone is on this point and what properties it has
		stone = NULL;
		count = 0;
		active = 0;
		astone = NULL;
		for(Stone *s = stonelist.first(); s; s = stonelist.next())
		{
			if((s->x() == x) && (s->y() == y))
				stone = s;
			if((s->owner() == Stone::white) || (s->owner() == Stone::whiteactive) || (s->owner() == Stone::whitemuehle))
				count++;
			if(s->owner() == Stone::whiteactive)
			{
				astone = s;
				active++;
			}
		}

		// If click on stone, change it
		if(stone)
		{
			if((stone->owner() == Stone::white) && (!active))
				stone->assign(Stone::whiteactive);
			else if(stone->owner() == Stone::whiteactive)
				stone->assign(Stone::white);
		}
		else
		{
			// If click on empty field, fill it
			if((count < 9) || (astone))
			{
				stone = new Stone();
				stone->move(x, y);
				stone->assign(Stone::white);
				stonelist.append(stone);

				if(astone)
				{
					net->output(QString("[%1,%2,%3,%4].").arg(astone->x()).arg(astone->y()).arg(x).arg(y));
					stonelist.remove(astone);
					//delete astone;
				}
				else
				{
					net->output(QString("[%1,%2].").arg(x).arg(y));
				}
			}
		}

		// Check for turn-win situation (muehle)
		xrow = x;
		yrow = y;
		xrows = 0;
		yrows = 0;
		pointlist = web->pointlist();
		QWebPoint *xp;
		for(QWebPoint *p = pointlist.first(); p; p = pointlist.next())
			if((p->point().x() == x) && (p->point().y() == y)) xp = p;
		peerlist = xp->peerlist();
		wp = xp;

		// Count all stones which are on one row with the current one
		QWebPath *path;
		path = new QWebPath(web, xp);
		path->create(2);
		pointlist = path->pathlist();
		//delete path;

		for(QWebPoint *p = pointlist.first(); p; p = pointlist.next())
		{
			if(p->point().x() == xrow)
				for(Stone *s2 = stonelist.first(); s2; s2 = stonelist.next())
					if((p->point().x() == s2->x()) && (p->point().y() == s2->y()))
						xrows++;
			if(p->point().y() == yrow)
				for(Stone *s2 = stonelist.first(); s2; s2 = stonelist.next())
					if((p->point().x() == s2->x()) && (p->point().y() == s2->y()))
						yrows++;
		}

		// If 3 stones on a row, player has won one turn
		kdDebug(12101) << xrows << " - " << yrows << endl;
		for(QWebPoint *p = pointlist.first(); p; p = pointlist.next())
		{
			if(((xrows == 3) && (p->point().x() == xrow)) || ((yrows == 3) && (p->point().y() == yrow)))
				for(Stone *s2 = stonelist.first(); s2; s2 = stonelist.next())
					if((p->point().x() == s2->x()) && (p->point().y() == s2->y()))
						s2->assign(Stone::whitemuehle);
		}

		delete path;

		repaint();
	}
}

void Board::remis()
{
	net->output("remis.");
}

void Board::loose()
{
	net->output("loose.");
}

