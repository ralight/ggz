/////////////////////////////////////////////////////////////////////
//
// KDots: Connect the Dots game for KDE 4
// http://www.ggzgamingzone.org/gameclients/kdots/
//
// Copyright (C) 2001 - 2008 Josef Spillner <josef@ggzgamingzone.org>
// Published under the conditions of the GNU GPL, see COPYING
//
/////////////////////////////////////////////////////////////////////

#include "kdots_help.h"

#include <klocale.h>
#include <kstandarddirs.h>

#include <qpushbutton.h>
#include <qpixmap.h>
#include <qpainter.h>

KDotsHelp::KDotsHelp()
: QWidget()
{
	QPushButton *ok;
	KStandardDirs d;

	setAttribute(Qt::WA_NoSystemBackground);

	m_font = QFont("courier", 20);
	m_setup = true;

	setFixedSize(600, 477);

	QPixmap bg(d.findResource("data", "kdots/firedragon.png"));
	m_bg = new QPixmap(bg.scaled(size(), Qt::IgnoreAspectRatio));

	ok = new QPushButton(i18n("Close"), this);
	ok->move((width() - ok->width()) / 2, height() - 50);

	connect(ok, SIGNAL(clicked()), SLOT(close()));

	setWindowTitle(i18n("KDots Help"));
	show();
}

KDotsHelp::~KDotsHelp()
{
	delete m_bg;
}

QString KDotsHelp::measure(QString s)
{
	QFontMetrics m(m_font);
	while(m.width(s) > width() - 20)
	{
		m_font.setPointSize(m_font.pointSize() - 1);
		m = QFontMetrics(m_font);
	}

	return s;
}

void KDotsHelp::paintEvent(QPaintEvent *e)
{
	QPainter p;
	QPixmap dummy;
	QPaintDevice *dev;

	Q_UNUSED(e);

	dev = this;
	if(m_setup)
	{
		dummy = m_bg->copy();
		dev = &dummy;
	}

	p.begin(dev);
	p.drawImage(0, 0, m_bg->toImage());
	p.setPen(QColor(255, 255, 255));
	m_font.setBold(true);
	p.setFont(m_font);
	p.drawText(20, 60, measure(i18n("Game help for KDots")));
	m_font.setBold(false);
	p.setFont(m_font);
	p.drawText(20, 120, measure(i18n("This game consists of a game board with")));
	p.drawText(20, 140, measure(i18n("dots which are to be connected. Whenever")));
	p.drawText(20, 160, measure(i18n("you fill one square by surrounding it, it")));
	p.drawText(20, 180, measure(i18n("is marked as yours. If all dots are connected,")));
	p.drawText(20, 200, measure(i18n("the player with the highest number of squares")));
	p.drawText(20, 220, measure(i18n("has won.  Tip: Always prefer an odd number of")));
	p.drawText(20, 240, measure(i18n("dots to prevent a tie game!")));
	p.drawText(20, 290, measure(i18n("Have fun!")));
	p.end();

	if(m_setup)
	{
		m_setup = false;
		update();
	}
}

