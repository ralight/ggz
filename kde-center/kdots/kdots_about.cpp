/////////////////////////////////////////////////////////////////////
//
// KDots: Connect the Dots game for KDE 4
// http://www.ggzgamingzone.org/gameclients/kdots/
//
// Copyright (C) 2001 - 2008 Josef Spillner <josef@ggzgamingzone.org>
// Published under the conditions of the GNU GPL, see COPYING
//
/////////////////////////////////////////////////////////////////////

#include "kdots_about.h"

#include <kapplication.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <ktoolinvocation.h>
#include <kaboutdata.h>
#include <kdebug.h>

#include <qpushbutton.h>
#include <qpixmap.h>
#include <qpainter.h>
#include <qpen.h>

KDotsAbout::KDotsAbout()
: QWidget()
{
	QPushButton *ok;
	KStandardDirs d;

	setAttribute(Qt::WA_NoSystemBackground);

	m_font = QFont("courier", 20);
	m_highlight = false;
	m_setup = true;

	setMouseTracking(true);

	setWindowTitle(i18n("About KDots"));
	setFixedSize(600, 457);

	QPixmap bg(d.findResource("data", "kdots/snowdragon.png"));
	m_bg = new QPixmap(bg.scaled(size(), Qt::IgnoreAspectRatio));

	ok = new QPushButton(i18n("Close"), this);
	ok->move((width() - ok->width()) / 2, height() - 50);

	connect(ok, SIGNAL(clicked()), SLOT(close()));
}

KDotsAbout::~KDotsAbout()
{
	delete m_bg;
}

QString KDotsAbout::measure(QString s)
{
	QFontMetrics m(m_font);
	while(m.width(s) > width() - 20)
	{
		m_font.setPointSize(m_font.pointSize() - 1);
		m = QFontMetrics(m_font);
	}

	return s;
}

void KDotsAbout::mousePressEvent(QMouseEvent *e)
{
	if(e->button() == Qt::LeftButton)
	{
		if(m_highlight)
		{
			KToolInvocation::invokeBrowser(KGlobal::mainComponent().aboutData()->homepage());
		}
	}
}

void KDotsAbout::mouseMoveEvent(QMouseEvent *e)
{
	if((e->y() > 140) && (e->y() < 170))
	{
		if(!m_highlight)
		{
			m_highlight = true;
			update();
		}
	}
	else if(m_highlight)
	{
		m_highlight = false;
		update();
	}
}

void KDotsAbout::paintEvent(QPaintEvent *e)
{
	QPainter p;
	QPen pen;
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
	m_font.setBold(true);
	p.setFont(m_font);
	p.drawText(20, 40, measure(i18n("Connect The Dots for KDE 4")));
	m_font.setBold(false);
	p.setFont(m_font);
	p.drawText(20, 80, measure("Copyright (C) 2001 - 2008 Josef Spillner"));
	p.drawText(20, 100, measure("josef@ggzgamingzone.org"));
	p.drawText(20, 140, measure(i18n("This game is part of the GGZ Gaming Zone.")));
	if(m_highlight)
	{
		pen = p.pen();
		p.setPen(QColor(255, 0, 0));
	}
	p.drawText(20, 160, measure("http://www.ggzgamingzone.org/gameclients/kdots/"));
	if(m_highlight)
		p.setPen(pen);
	p.drawText(20, 200, measure(i18n("Thanks to www.drachenburg.de")));
	p.drawText(20, 220, measure(i18n("for their dragon images!")));
	p.end();

	if(m_setup)
	{
		m_setup = false;
		update();
	}
}

