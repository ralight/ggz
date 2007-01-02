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
#include "toplevel.h"

// KDE includes
#include <kstandarddirs.h>
#include <kdebug.h>
#include <klocale.h>
#include <kmessagebox.h>

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
	net = NULL;

	bg = NULL;
	black = NULL;
	white = NULL;
	setTheme("standard");

	m_color = colorwhite;
	m_turn = 1;
	m_take = 0;
	m_wait = 0;
	m_whitescore = 0;
	m_blackscore = 0;
	m_phase = phasestart;

	web = NULL;
	setVariant("classic");

	stonelist.setAutoDelete(true);

	m_repaint = 0;
}

// Destructor
Board::~Board()
{
	if(web) delete web;
	delete bg;
	if(net) delete net;
}

// Initialize to a null-board
void Board::init()
{
	emit signalStatus(i18n("Starting new game."));

	m_turn = 1;
	m_take = 0;
	m_repaint = 1;

	stonelist.clear();
	repaint();
}

// Check network
void Board::timerEvent(QTimerEvent *e)
{
	Q_UNUSED(e);

	net->poll();
}

// draw one Muehle stone
void Board::paintStone(QPixmap *tmp, QPainter *p, int x, int y, int owner)
{
	QPixmap pix;
	QImage im, im2;
	int r, g, b, a;
	QRgb rgba;
	/*QRgb rgba2;
	int r2, g2, b2;*/

	Q_UNUSED(tmp);

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

	im = pix.convertToImage();
	im = im.smoothScale(height() / 10, width() / 10);
	pix.convertFromImage(im);

	p->drawPixmap(x - pix.width() / 2, y - pix.height() / 2, pix);
}

// draw board with all stones
void Board::paintEvent(QPaintEvent *e)
{
	QExtPainter p;
	QPixmap tmp;
	int smaller;

	Q_UNUSED(e);

	if(width() != height())
	{
		smaller = width();
		if(height() < smaller) smaller = height();
		resize(smaller, smaller);
		return;
	}

	if(!web) return;
	if(!m_repaint) return;

	m_repaint = 0;

	tmp.resize(width(), height());
	p.begin(&tmp);
	p.setPen(QPen(QColor(0, 0, 0), 3));

	p.drawTiledPixmap(0, 0, width(), height(), *bg);
	p.drawWeb(*web);

	float x = web->scale();
	for(Stone *s = stonelist.first(); s; s = stonelist.next())
		paintStone(&tmp, &p, (int)(x * s->x()),
			(int)(x * s->y()), s->owner());

	p.end();

	setErasePixmap(tmp);
}

// Resize the board properly
void Board::resizeEvent(QResizeEvent *e)
{
	Q_UNUSED(e);

	if(width() == height())	
	{
		m_repaint = 1;
		move((parentWidget()->width() - width()) / 2, (parentWidget()->height() - height()) / 2);
		if(web) web->setScale(width() / 100.0);
	}
}

// Handle a mouse click
void Board::mousePressEvent(QMouseEvent *e)
{
	int x, y;
	QPoint final;
	int count, active;
	Stone *stone, *astone;

	if(!web) return;
	if(m_color == colornone)
	{
		KMessageBox::sorry(this, i18n("The server doesn't have assigned a color to you yet."), i18n("Client message"));
		return;
	}
	if(!m_turn)
	{
		KMessageBox::sorry(this, i18n("It's not your turn - please wait for the opponent."), i18n("Client message"));
		return;
	}
	if(m_turn < 0)
	{
		KMessageBox::sorry(this, i18n("This game has already finished."), i18n("Client message"));
		return;
	}
	m_repaint = 1;

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
			if(m_color == colorwhite)
			{
				if((s->owner() == Stone::white) || (s->owner() == Stone::whiteactive) || (s->owner() == Stone::whitemuehle))
					count++;
				if(s->owner() == Stone::whiteactive)
				{
					astone = s;
					active++;
				}
			}
			else if(m_color == colorblack)
			{
				if((s->owner() == Stone::black) || (s->owner() == Stone::blackactive) || (s->owner() == Stone::blackmuehle))
					count++;
				if(s->owner() == Stone::blackactive)
				{
					astone = s;
					active++;
				}
			}
		}

		// Take the stone if allowed
		if(m_take)
		{
			if((stone) &&
			(((m_color == colorwhite) && (stone->owner() == Stone::black))
			|| ((m_color == colorblack) && (stone->owner() == Stone::white))))
			{
				stonelist.remove(stone);
				m_take = 0;
				m_turn = 0;
				emit signalStatus(i18n("Opponent's turn"));

				if(net) net->output(QString("[%1,%1].").arg(x).arg(y));

				resetStones();
			}
			else
			{
				KMessageBox::sorry(this, i18n("Please take one of your opponent's stones"), i18n("Client message"));
			}
			return;
		}

		// If click on stone, change it
		if(stone)
		{
			if(m_phase == phasemove)
			{
				if(m_color == colorwhite)
				{
					if((stone->owner() == Stone::white) && (!active))
						stone->assign(Stone::whiteactive);
					else if(stone->owner() == Stone::whiteactive)
						stone->assign(Stone::white);
				}
				else if(m_color == colorblack)
				{
					if((stone->owner() == Stone::black) && (!active))
						stone->assign(Stone::blackactive);
					else if(stone->owner() == Stone::blackactive)
						stone->assign(Stone::black);
				}
			}
			else KMessageBox::sorry(this, i18n("Moves aren't allowed while placing the stones initially"), i18n("Client message"));
		}
		else
		{
			// If click on empty field, fill it
			if((((m_color == colorwhite) && (m_whitestones))
			|| ((m_color == colorblack) && (m_blackstones)))
			|| (astone))
			{
				stone = new Stone();
				stone->move(x, y);
				if(m_color == colorwhite)
				{
					stone->assign(Stone::white);
					if(m_phase == phaseset) m_whitestones--;
					if((!m_whitestones) && (m_color == colorwhite)) m_phase = phasemove;
				}
				else if(m_color == colorblack)
				{
					stone->assign(Stone::black);
					if(m_phase == phaseset) m_blackstones--;
					if((!m_blackstones) && (m_color == colorblack)) m_phase = phasemove;
				}
				stonelist.append(stone);

				updateStatus();

				if(astone)
				{
					if(net)
					{
						// Move a stone
						net->output(QString("[%1,%2,%3,%4].").arg(astone->x()).arg(astone->y()).arg(x).arg(y));
						m_turn = 0;
						emit signalStatus(i18n("Opponent's turn"));
					}
					stonelist.remove(astone);
				}
				else
				{
					if(net)
					{
						// Place a new stone
						net->output(QString("[%1,%2].").arg(x).arg(y));
						m_turn = 0;
						emit signalStatus(i18n("Opponent's turn"));
					}
				}
			}
			else
			{
				if(m_phase == phaseset)
				{
					m_phase = phasemove;
					KMessageBox::information(this, i18n("You must now move your stones."), i18n("Client message"));
				}
				return;
			}
		}

		check(x, y, stone);
	}
}

void Board::check(int x, int y, Stone *stone)
{
	QWebPoint *wp;
	QPtrList<QWebPoint> pointlist;
	QPtrList<QPoint> peerlist;
	int xrow, yrow, xrows, yrows;
	int color;

	// Check for turn-win situation (muehle)
	xrow = x;
	yrow = y;
	xrows = 0;
	yrows = 0;
	pointlist = web->pointlist();
	QWebPoint *xp = NULL;
	for(QWebPoint *p = pointlist.first(); p; p = pointlist.next())
		if((p->point().x() == x) && (p->point().y() == y)) xp = p;
	if(!xp) return;
	peerlist = xp->peerlist();
	wp = xp;
	color = stone->owner();

	// Count all stones which are on one row with the current one
	QWebPath *path;
	path = new QWebPath(web, xp);
	path->create(2);
	pointlist = path->pathlist();

	// Look for longest stones-in-a-row
	for(QWebPoint *p = pointlist.first(); p; p = pointlist.next())
	{
		if(p->point().x() == xrow)
			for(Stone *s2 = stonelist.first(); s2; s2 = stonelist.next())
				if((p->point().x() == s2->x()) && (p->point().y() == s2->y()) && (color == s2->owner()))
					xrows++;
		if(p->point().y() == yrow)
			for(Stone *s2 = stonelist.first(); s2; s2 = stonelist.next())
				if((p->point().x() == s2->x()) && (p->point().y() == s2->y()) && (color == s2->owner()))
					yrows++;
	}

	// If 3 stones on a row, player has won one turn; display it
	kdDebug(12101) << xrows << " - " << yrows << endl;
	for(QWebPoint *p = pointlist.first(); p; p = pointlist.next())
	{
		if(((xrows == 3) && (p->point().x() == xrow)) || ((yrows == 3) && (p->point().y() == yrow)))
		{
			for(Stone *s2 = stonelist.first(); s2; s2 = stonelist.next())
				if((p->point().x() == s2->x()) && (p->point().y() == s2->y()))
				{
					if(color == Stone::white)
						s2->assign(Stone::whitemuehle);
					else if(color == Stone::black)
						s2->assign(Stone::blackmuehle);
				}
		}
	}

	// Muehle!
	if((xrows == 3) || (yrows == 3))
	{
		if(((m_color == colorwhite) && (stone->owner() == Stone::whitemuehle))
		|| ((m_color == colorblack) && (stone->owner() == Stone::blackmuehle)))
		{
			emit signalStatus(i18n("Take a stone from your opponent"));
			m_turn = 1;
			m_take = 1;
			kdDebug(12101) << "TAKE" << endl;
		}
		else
		{
			emit signalStatus(i18n("Waiting for opponent's action"));
			m_turn = 0;
			m_wait = 1;
			kdDebug(12101) << "GIVE" << endl;
		}

		if(stone->owner() == Stone::whitemuehle) m_whitescore++;
		else if(stone->owner() == Stone::blackmuehle) m_blackscore++;

		if(((m_color == colorwhite) && (m_whitescore == 6))
		|| ((m_color == colorblack) && (m_blackscore == 6)))
		{
			net->output("gameover.");
			KMessageBox::information(this, i18n("You have won the game!"), i18n("Client message"));
			m_turn = -1;
			emit signalEnd();
		}
		updateStatus();
	}

	delete path;

	m_repaint = 1;
	repaint();
}

// Send remis to the server
void Board::remis()
{
	if(net) net->output("remis.");
}

// Send wish to loose to the server
void Board::loose()
{
	if(net) net->output("loose.");
}

// Change the active theme
void Board::setTheme(const QString &theme)
{
	KStandardDirs *d = KGlobal::dirs();

	if(bg) delete bg;
	if(black) delete black;
	if(white) delete white;
	bg = new QPixmap(d->findResource("data", QString("muehle/themes/%1/bg.png").arg(theme)));
	black = new QPixmap(d->findResource("data", QString("muehle/themes/%1/black.png").arg(theme)));
	white = new QPixmap(d->findResource("data", QString("muehle/themes/%1/white.png").arg(theme)));

	parentWidget()->setErasePixmap(*bg);

	m_repaint = 1;
	repaint();
}

// Change the active variant
void Board::setVariant(const QString &variant)
{
	KStandardDirs *d = KGlobal::dirs();
	QString s;
	int x1, y1, x2, y2;

	stonelist.clear();
	if(web)
	{
		delete web;
		web = NULL;
	}

	if(!net)
	{
		m_turn = 1;
	}

	s = d->findResource("data", QString("muehle/%1").arg(variant));
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
			s.replace(QRegExp("\\("), "");
			s.replace(QRegExp("\\)"), "");
			s.replace(QRegExp("\\ "), "");
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

	m_repaint = 1;
	repaint();
}

// Enable network mode
void Board::enableNetwork(bool enabled)
{
	//QString s;
	//s = net->input();
	//kdDebug(12101) << "Net input: " << s << endl;

	if(!enabled)
	{
		m_blackstones = 9;
		m_whitestones = 9;

		emit signalScore(i18n("You"), Toplevel::statusplayer, 0, 9);
		emit signalScore(i18n("AI"), Toplevel::statusopponent, 0, 9);
		emit signalStatus(i18n("Good luck."));

		init();
	}
	else
	{
		m_color = colornone;
		m_turn = 0;

		net = new Net();
		net->output(i18n("KDE Muehle Game"));

		connect(net, SIGNAL(signalInput()), SLOT(slotInput()));

		startTimer(100);
	}
}

// Input from network
void Board::slotInput()
{
	QString s;
	int error;
	int ret;
	int x, y, x2, y2;

	error = 0;

	s = net->input();
	kdDebug(12101) << "Board input: " << s << endl;

	if ((s == "spectator.") && (m_color == colornone))
	{
		m_color = colorspectator;
		m_turn = 0;
		signalStatus(i18n("Game spectator"));
	}
	else if((s == "white.") && (m_color == colornone))
	{
		m_color = colorwhite;
		KMessageBox::information(this, i18n("Your turn, you play with the white stones."), i18n("Server message"));
		m_turn = 1;
		m_phase = phaseset;
		m_whitestones = 9;
		m_blackstones = 9;
		signalStatus(i18n("Your turn"));
		updateStatus();
	}
	else if((s == "black.") && (m_color == colornone))
	{
		m_color = colorblack;
		KMessageBox::information(this, i18n("You play with the black stones, but wait for your opponent first."), i18n("Server message"));
		m_turn = 0;
		m_phase = phaseset;
		m_whitestones = 9;
		m_blackstones = 9;
		emit signalStatus(i18n("Opponent's turn"));
		updateStatus();
	}
	else if((s.left(1) == "[") && (s.right(2) == "].") && (m_color != colornone) && ((!m_turn) || (m_wait)))
	{
		QStringList l;
		Stone *stone;

		l = l.split(",", s.mid(1, s.length() - 3));
		if(l.count() < 2)
		{
			kdDebug(12101) << "Wrong input data!" << endl;
			return;
		}

		kdDebug(12101) << "Parts: " << l.count() << " in " << s.mid(1, s.length() - 3) << endl;

		x = (*(l.at(0))).toInt();
		y = (*(l.at(1))).toInt();

		switch(l.count())
		{
			case 2:
				if(!m_wait)
				{
					stone = new Stone();
					stone->move(x, y);
					if(m_color == colorwhite)
					{
						stone->assign(Stone::black);
						m_blackstones--;
					}
					else if(m_color == colorblack)
					{
						stone->assign(Stone::white);
						m_whitestones--;
					}
					else if(m_color == colorspectator)
					{
						// FIXME: toggle between black and white
						stone->assign(Stone::white);
						m_whitestones--;
					}
					stonelist.append(stone);
					updateStatus();

					check(x, y, stone);
				}
				else
				{
					for(stone = stonelist.first(); stone; stone = stonelist.next())
						if((stone->x() == x) && (stone->y() == y))
							stonelist.remove(stone);
					m_wait = 0;
					resetStones();
					m_repaint = 1;
					repaint();
				}
				if((!m_wait) && (m_color != colorspectator))
				{
					m_turn = 1;
					emit signalStatus(i18n("Your turn"));
				}
				break;
			case 4:
				x2 = (*(l.at(2))).toInt();
				y2 = (*(l.at(3))).toInt();
				for(stone = stonelist.first(); stone; stone = stonelist.next())
					if((stone->x() == x) && (stone->y() == y))
					{
						m_turn = 1;
						emit signalStatus(i18n("Your turn"));
						stone->move(x2, y2);
						check(x2, y2, stone);
					}
				break;
			case 6:
				break;
			default:
				error = 1;
				break;
		}
	}
	else if(s == "invalid.")
	{
		error = 1;
	}
	else if(s == "remis.")
	{
		ret = KMessageBox::questionYesNo(this, i18n("Your opponent wants to get a remis. Do you accept this?"), i18n("Opponent message"));
		if(ret == KMessageBox::Yes)
		{
			net->output("loose.");
			m_turn = -1;
			emit signalEnd();
		}
		else
		{
			net->output("reject.");
		}
	}
	else if(s == "reject.")
	{
		KMessageBox::sorry(this, i18n("Your opponent refuses to give you remis."), i18n("Opponent message"));
	}
	else if(s == "loose.")
	{
		if(m_turn != -1)
		{
			KMessageBox::information(this, i18n("Your opponent has given up."), i18n("Server message"));
			m_turn = -1;
			emit signalEnd();
		}
	}
	else if(s == "gameover.")
	{
		KMessageBox::information(this, i18n("Your opponent has won the game! You have lost!"), i18n("Server message"));
		m_turn = -1;
		emit signalEnd();
	}
	else error = 1;

	if(error)
	{
		KMessageBox::error(this, i18n("Aborting game due to unhandled network input."), i18n("Network error"));
		m_turn = -1;
		emit signalEnd();
	}
}

void Board::resetStones()
{
	// Reset the muehle to normal colors
	for(Stone *s = stonelist.first(); s; s = stonelist.next())
	{
		if(s->owner() == Stone::whitemuehle)
			s->assign(Stone::white);
		if(s->owner() == Stone::blackmuehle)
			s->assign(Stone::black);
	}
}

void Board::updateStatus()
{
	if(m_color == colorblack)
	{
		emit signalScore(i18n("Black"), Toplevel::statusplayer, m_blackscore, m_blackstones);
		emit signalScore(i18n("White"), Toplevel::statusopponent, m_whitescore, m_whitestones);
	}
	else if(m_color == colorwhite)
	{
		emit signalScore(i18n("Black"), Toplevel::statusopponent, m_blackscore, m_blackstones);
		emit signalScore(i18n("White"), Toplevel::statusplayer, m_whitescore, m_whitestones);
	}
}

