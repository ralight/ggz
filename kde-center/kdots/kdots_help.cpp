///////////////////////////////////////////////////////////////
//
// KDots
// Connect the Dots game for KDE, using the Dots classes
// Copyright (C) 2001, 2002 Josef Spillner
// dr_maux@users.sourceforge.net
// The MindX Open Source Project
// http://mindx.sourceforge.net/games/kdots/
//
///////////////////////////////////////////////////////////////

#include "kdots_help.h"

#include <klocale.h>

#include <qpushbutton.h>
#include <qpixmap.h>
#include <qpainter.h>

#include "config.h"

KDotsHelp::KDotsHelp(QWidget *parent, const char *name)
: QWidget(parent, name, WStyle_Customize | WStyle_Tool | WStyle_DialogBorder)
{
	QPushButton *ok;

	m_font = QFont("courier", 16);
	m_repaint = 0;

	ok = new QPushButton(i18n("OK"), this);
	ok->move(200, 340);

	setErasePixmap(QPixmap(GGZDATADIR "/kdots/firedragon.png"));

	connect(ok, SIGNAL(clicked()), SLOT(slotAccepted()));

	setCaption(i18n("KDots Help"));
	setFixedSize(500, 398);
}

KDotsHelp::~KDotsHelp()
{
}

void KDotsHelp::slotAccepted()
{
	close();
}

QString KDotsHelp::measure(QString s)
{
	QFontMetrics m(m_font);
	while(m.width(s) > 460)
	{
		m_font.setPointSize(m_font.pointSize() - 1);
		m = QFontMetrics(m_font);
		m_repaint = 1;
	}

	return s;
}

void KDotsHelp::paintEvent(QPaintEvent *e)
{
	Q_UNUSED(e);

	QPainter p;

	p.begin(this);
	p.setPen(QColor(255, 255, 255));
	m_font.setBold(true);
	p.setFont(m_font);
	p.drawText(150, 60, measure(i18n("Game help for KDots")));
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

	if(m_repaint)
	{
		m_repaint = 0;
		repaint();
	}
	else show();
}

