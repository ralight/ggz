/////////////////////////////////////////////////////////////////////////////////////
//                                                                                 //
//    KGGZ - The KDE client for the GGZ Gaming Zone - Version 0.0.5pre             //
//    Copyright (C) 2000 - 2002 Josef Spillner - dr_maux@users.sourceforge.net     //
//    The MindX Open Source Project - http://mindx.sourceforge.net                 //
//    Published under GNU GPL conditions - view COPYING for details                //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////
//                                                                                 //
//    This program is free software; you can redistribute it and/or modify         //
//    it under the terms of the GNU General Public License as published by         //
//    the Free Software Foundation; either version 2 of the License, or            //
//    (at your option) any later version.                                          //
//                                                                                 //
//    This program is distributed in the hope that it will be useful,              //
//    but WITHOUT ANY WARRANTY; without even the implied warranty of               //
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                //
//    GNU General Public License for more details.                                 //
//                                                                                 //
//    You should have received a copy of the GNU General Public License            //
//    along with this program; if not, write to the Free Software                  //
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA    //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////
//                                                                                 //
// KGGZAbout: Display a nice window which honours all the GGZ developers in space. //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////

// Header file
#include "KGGZAbout.h"

// KGGZ includes
#include "KGGZCommon.h"

// KDE includes
#include <klocale.h>

// Qt includes
#include <qlayout.h>
#include <qpushbutton.h>
#include <qpainter.h>
#include <qtimer.h>
#include <qframe.h>

KGGZAbout::KGGZAbout(QWidget *parent, const char *name)
: QWidget(parent, name, WStyle_Customize | WStyle_Tool | WStyle_DialogBorder)
{
	QVBoxLayout *vbox;
	QPushButton *ok;

	m_font = QFont("helvetica", 16);
	m_font.setStyleHint(QFont::Serif);
	m_repaint = 0;

	m_frame = new QFrame(this);
	m_frame->setFrameStyle(QFrame::Panel | QFrame::Sunken);
	m_frame->setEraseColor(QColor(100, 0, 0));

	ok = new QPushButton("OK", this);

	vbox = new QVBoxLayout(this, 5);
	vbox->add(m_frame);
	vbox->add(ok);

	connect(ok, SIGNAL(clicked()), SLOT(close()));

	startTimer(70);

	m_frame->setMinimumSize(400, 250);
	setCaption(i18n("About the GGZ Gaming Zone"));
}

KGGZAbout::~KGGZAbout()
{
}

void KGGZAbout::timerEvent(QTimerEvent *e)
{
	repaint();
}

void KGGZAbout::resizeEvent(QResizeEvent *e)
{
	m_font.setPointSize(width() / 20);
	m_repaint = 1;
	repaint();
}

QString KGGZAbout::measure(QString s)
{
	QFontMetrics m(m_font);
	while(m.width(s) > m_frame->width() - 20)
	{
		m_font.setPointSize(m_font.pointSize() - 1);
		m = QFontMetrics(m_font);
		m_repaint = 1;
	}
	return s;
}

void KGGZAbout::paintEvent(QPaintEvent *e)
{
	QPainter p;
	static int x = 50;
	static int x2 = 100;
	QFont font;
	const QPixmap *pix;
	int h;
	int w, w2;
	const QString developers = QString("Brian Cox - Brent Hendricks - Doug Hudson - Rich Gade - "
		"Ismael Orenstein - Dan Papasian - Ricardo Quesada - Jason Short - Josef Spillner - Justin Zaun - ") +
		i18n("and many supporters...");

	pix = new QPixmap(m_frame->width(), m_frame->height());

	p.begin(pix);
	p.fillRect(0, 0, m_frame->width(), m_frame->height(), QBrush(QColor(100, 0, 0)));

	font.setPointSize(50);
	font.setWeight(QFont::Black);
	p.setFont(font);
	p.setPen(QColor(130, 0, 0));
	p.drawText(x, m_frame->height() - m_font.pointSize() * 2 - 20, "GGZ Gaming Zone " KGGZVERSION);
	QFontMetrics m(font);
	w = m.width("GGZ Gaming Zone" KGGZVERSION);
	if(x < -w) x = w;

	font.setPointSize(10);
	font.setWeight(QFont::Normal);
	p.setFont(font);
	p.setPen(QColor(210, 50, 50));
	p.drawText(x2, m_frame->height() - m_font.pointSize() * 2 - 20, developers);
	QFontMetrics m2(font);
	w2 = m2.width(developers);
	if(x2 < -w2) x2 = w2;

	font = QFont("helvetica", 18);
	font.setStyleHint(QFont::Serif);
	p.setFont(font);
	p.setPen(QColor(255, 255, 255));
	p.drawText(10, 25, measure(i18n("The GGZ Gaming Zone")));
	p.drawLine(10, 30, 330, 30);
	p.drawLine(10, 32, 330, 32);

	p.setFont(m_font);
	h = m_font.pointSize() + 5;
	p.drawText(10, 35 + h * 1, measure(i18n("Free Software shall dominate all Online Games!")));
	p.drawText(10, 35 + h * 2, measure(i18n("The GGZ Gaming Zone is a collaboration between several people who")));
	p.drawText(10, 35 + h * 3, measure(i18n("try to make public online game services available for free -")));
	p.drawText(10, 35 + h * 4, measure(i18n("including source code and professional help for both gamers and")));
	p.drawText(10, 35 + h * 5, measure(i18n("game developers. GGZ is available for many platforms and toolkits.")));
	p.drawText(10, m_frame->height() - m_font.pointSize() - 10, measure(i18n("Homepage: http://www.ggzgamingzone.org")));

	p.end();

	x--;
	x2 -= 3;

	if(m_repaint)
	{
		m_repaint = 0;
		repaint();
	}
	else
	{
		m_frame->setErasePixmap(*pix);
		show();
	}

	delete pix;
}

