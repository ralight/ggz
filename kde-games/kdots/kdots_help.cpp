///////////////////////////////////////////////////////////////
//
// KDots
// Connect the Dots game for KDE, using the Dots classes
// Copyright (C) 2001 Josef Spillner
// dr_maux@users.sourceforge.net
// The MindX Open Source Project
// http://mindx.sourceforge.net/games/kdots/
//
///////////////////////////////////////////////////////////////

#include "kdots_help.h"

#include <klocale.h>

#include <qlayout.h>
#include <qpushbutton.h>
#include <qpixmap.h>
#include <qpainter.h>

#include "config.h"

KDotsHelp::KDotsHelp(QWidget *parent, char *name)
: QWidget(parent, name, WStyle_Customize | WStyle_Tool | WStyle_DialogBorder)
{
	QPushButton *ok;
	QVBoxLayout *vbox;

	ok = new QPushButton("OK", this);

	m_pane = new QWidget(this);
	m_pane->setBackgroundColor(QColor(150.0, 0.0, 0.0));
	m_pane->setBackgroundPixmap(QPixmap(GGZDATADIR "/kdots/firedragon.png"));

	vbox = new QVBoxLayout(this, 5);
	vbox->add(m_pane);
	vbox->add(ok);

	connect(ok, SIGNAL(clicked()), SLOT(slotAccepted()));

	setCaption("KDots Help");
	setFixedSize(400, 300);
	show();
}

KDotsHelp::~KDotsHelp()
{
}

void KDotsHelp::slotAccepted()
{
	close();
}

void KDotsHelp::paintEvent(QPaintEvent *e)
{
	QPainter p;

	p.begin(m_pane);
	p.setPen(QColor(255.0, 255.0, 255.0));
	p.setFont(QFont("courier", 16, QFont::Normal));
	p.drawText(10, 40, i18n("Game help for KDots"));
	p.setFont(QFont("courier", 12, QFont::Normal));
	p.drawText(10, 80, i18n("This game consists of a game board with"));
	p.drawText(10, 100, i18n("dots which are to be connected. Whenever"));
	p.drawText(10, 120, i18n("you achieve it to fill one square by surrounding"));
	p.drawText(10, 140, i18n("it, it is marked as yours. If all dots are"));
	p.drawText(10, 160, i18n("connected, the player with the highest number"));
	p.drawText(10, 180, i18n("of fields has won. Tip: Prefer always an odd"));
	p.drawText(10, 200, i18n("number of dots to prevent a tie game!"));
	p.drawText(10, 230, i18n("Have fun!"));
	p.end();
}

