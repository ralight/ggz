///////////////////////////////////////////////////////////////
//
// KDots
// Connect the Dots game for KDE, using the Dots classes
// Copyright (C) 2001, 2002 Josef Spillner
// dr_maux@users.sourceforge.net
// The MindX Open Source Project
// http://mindx.sourceforge.net/gameclients/kdots/
//
///////////////////////////////////////////////////////////////

#include "kdots_about.h"

#include <kapplication.h>
#include <klocale.h>

#include <qpushbutton.h>
#include <qpixmap.h>
#include <qpainter.h>
#include <qpen.h>

#include "config.h"

KDotsAbout::KDotsAbout(QWidget *parent, const char *name)
: QWidget(parent, name, WStyle_Customize | WStyle_Tool | WStyle_DialogBorder)
{
	QPushButton *ok;

	m_font = QFont("courier", 16);
	m_repaint = 0;
	m_highlight = 0;

	ok = new QPushButton(i18n("OK"), this);
	ok->move(150, 250);

	connect(ok, SIGNAL(clicked()), SLOT(slotAccepted()));

	setMouseTracking(true);

	m_bg = new QPixmap(GGZDATADIR "/kdots/snowdragon.png");
	setErasePixmap(*m_bg);
	setCaption(i18n("About KDots"));
	setFixedSize(400, 305);
}

KDotsAbout::~KDotsAbout()
{
}

void KDotsAbout::slotAccepted()
{
	close();
}

QString KDotsAbout::measure(QString s)
{
	QFontMetrics m(m_font);
	while(m.width(s) > 360)
	{
		m_font.setPointSize(m_font.pointSize() - 1);
		m = QFontMetrics(m_font);
		m_repaint = 1;
	}

	return s;
}

void KDotsAbout::mousePressEvent(QMouseEvent *e)
{
	Q_UNUSED(e);

	if(m_highlight)
	{
		kapp->invokeBrowser("http://www.ggzgamingzone.org/gameclients/kdots/");
	}
}

void KDotsAbout::mouseMoveEvent(QMouseEvent *e)
{
	if((e->y() > 140) && (e->y() < 170))
	{
		if(!m_highlight)
		{
			m_highlight = 1;
			m_repaint = 0;
			repaint();
		}
	}
	else if(m_highlight)
	{
		m_highlight = 0;
		m_repaint = 0;
		repaint();
	}
}

void KDotsAbout::paintEvent(QPaintEvent *e)
{
	QPainter p;
	QPen pen;
	const QPixmap *pix;

	Q_UNUSED(e);

	if(m_repaint == -1) return;

	pix = new QPixmap(*m_bg);
	p.begin(pix);
	m_font.setBold(true);
	p.setFont(m_font);
	p.drawText(100, 40, measure(i18n("Connect The Dots for KDE")));
	m_font.setBold(false);
	p.setFont(m_font);
	p.drawText(20, 80, measure("Copyright (C) 2001 - 2004 Josef Spillner"));
	p.drawText(20, 100, measure("josef@ggzgamingzone.org"));
	p.drawText(20, 140, measure(i18n("This game is part of the GGZ Gaming Zone.")));
	if(m_highlight)
	{
		pen = p.pen();
		p.setPen(QColor(255, 0, 0));
	}
	p.drawText(20, 160, measure("http://www.ggzgamingzone.org/gameclients/kdots/"));
	if(m_highlight) p.setPen(pen);
	p.drawText(20, 200, measure(i18n("Thanks to www.drachenburg.de")));
	p.drawText(20, 220, measure(i18n("for their dragon images!")));
	p.end();
	setErasePixmap(*pix);

	delete pix;

	if(m_repaint)
	{
		m_repaint = 0;
		repaint();
	}
	else
	{
		m_repaint = -1;
		show();
	}
}

