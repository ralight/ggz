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

// Header file
#include "board.h"

// Muehle includes
#include "qextpainter.h"
#include "stone.h"
#include "qweb.h"
#include "net.h"
#include "qwebpath.h"

// KDE includes
#include <kstddirs.h>
#include <kdebug.h>

// Qt includes
#include <qfile.h>
#include <qregexp.h>
#include <qtextstream.h>
#include <qpen.h>
#include <qimage.h>
#include <qpixmap.h>

// Configuration includes
#include "config.h"

// Constructor: load 'default' theme with 'classic' variant
Board::Board(QWidget *parent, const char *name)
: QWidget(parent, name)
{
	net = new Net();
	net->output("KDE Muehle Game");

	bg = NULL;
	black = NULL;
	white = NULL;
	setTheme("standard");

	web = NULL;
	setVariant("classic");

	stonelist.setAutoDelete(true);
}

// Destructor
Board::~Board()
{
	if(web) delete web;
	delete bg;
	delete net;
}

// Initialize to a null-board
void Board::init()
{
	stonelist.clear();
	repaint();
}

// draw one Muehle stone
void Board::paintStone(QPixmap *tmp, QPainter *p, int x, int y, int owner)
{
	QPixmap pix;
	QImage im, im2;
	int r, g, b, a;
	QRgb rgba, rgba2;
	int r2, g2, b2;

	if((owner == Stone::white) || (owner == Stone::whiteactive) || (owner == Stone::whitemuehle))
		pix = *white;
	if((owner == Stone::black) || (owner == Stone::blackactive) || (owner == Stone::blackmuehle))
		pix = *black;

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

// draw board with all stones
void Board::paintEvent(QPaintEvent *e)
{
	QExtPainter p;
	QPixmap tmp;

	if(!web) return;

	//bg->resize(width(), height());
	tmp.resize(width(), height());
	p.begin(&tmp);
	p.setPen(QPen(QColor(0, 0, 0), 3));
	p.drawTiledPixmap(0, 0, width(), height(), *bg);

	p.drawWeb(*web);

	float x = web->scale();
	for(Stone *s = stonelist.first(); s; s = stonelist.next())
		//p.drawPixmap(x * s->x() - 32, x * s->y() - 32, (s->owner() ? black : white));
		paintStone(&tmp, &p, (int)(x * s->x() - 32), (int)(x * s->y() - 32), s->owner());

	p.end();

	setBackgroundPixmap(tmp);
}

// Resize the board properly
void Board::resizeEvent(QResizeEvent *e)
{
	if(width() != height()) resize(width(), width());
	else
	{
		repaint();
	}

	if(web) web->setScale(width() / 100.0);
}

// Handle a mouse click
void Board::mousePressEvent(QMouseEvent *e)
{
	int x, y, xrow, yrow, xrows, yrows;
	QPoint final;
	int count, active;
	Stone *stone, *astone;
	QList<QWebPoint> pointlist;
	QList<QPoint> peerlist;
	QWebPoint *wp;

	if(!web) return;

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

// Send remis to the server
void Board::remis()
{
	net->output("remis.");
}

// Send wish to loose to the server
void Board::loose()
{
	net->output("loose.");
}

// Change the active theme
void Board::setTheme(QString theme)
{
	KStandardDirs d;

	if(bg) delete bg;
	if(black) delete black;
	if(white) delete white;
	d.addResourceDir("data", GGZDATADIR);
	bg = new QPixmap(d.findResource("data", QString("muehle/themes/%1/bg.png").arg(theme)));
	black = new QPixmap(d.findResource("data", QString("muehle/themes/%1/black.png").arg(theme)));
	white = new QPixmap(d.findResource("data", QString("muehle/themes/%1/white.png").arg(theme)));

	repaint();
}

// Change the active variant
void Board::setVariant(QString variant)
{
	KStandardDirs d;
	QString s;
	int x1, y1, x2, y2;

	stonelist.clear();
	if(web)
	{
		delete web;
		web = NULL;
	}

	d.addResourceDir("data", GGZDATADIR);
	s = d.findResource("data", QString("muehle/%1").arg(variant));
	kdDebug(12101) << "Load variant: " << variant << endl;
	if(s.isNull())
	{
		kdDebug(12101) << "Couldn't load variant!" << endl;
		return;
	}

	web = new QWeb();
	QFile f(s);
	f.open(IO_ReadOnly);
	QTextStream t(&f);
	while(!t.eof())
	{
		s = t.readLine();
		// Format: "(x1, y1), (x2, y2)\n"
		if(s.startsWith("("))
		{
			s.replace(QRegExp("("), "");
			s.replace(QRegExp(")"), "");
			s.replace(QRegExp(" "), "");
			//s.remove(s.length() - 1, 1);
			// Format is now: "x1,y1,x2,y2"
			QStringList list = QStringList::split(",", s);
			x1 = list[0].toInt();
			y1 = list[1].toInt();
			x2 = list[2].toInt();
			y2 = list[3].toInt();
			web->addPeer(QPoint(x1, y1), QPoint(x2, y2));
			kdDebug(12101) << "web->addPeer(QPoint(" << x1 << ", " << y1 << "), QPoint(" << x2 << ", " << y2 << "));" << endl;
		}
	}
	f.close();
	web->setScale(width() / 100.0);
}

