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

#include "kdots_about.h"

#include <klocale.h>

#include <qpushbutton.h>
#include <qpixmap.h>
#include <qpainter.h>

#include "config.h"

KDotsAbout::KDotsAbout(QWidget *parent, const char *name)
: QWidget(parent, name, WStyle_Customize | WStyle_Tool | WStyle_DialogBorder)
{
	QPushButton *ok;

	ok = new QPushButton("OK", this);
	ok->setGeometry(100, 170, 100, 20);

	connect(ok, SIGNAL(clicked()), SLOT(slotAccepted()));

	setBackgroundPixmap(QPixmap(GGZDATADIR "/kdots/snowdragon.png"));
	setCaption("About KDots");
	setFixedSize(300, 200);
	show();
}

KDotsAbout::~KDotsAbout()
{
}

void KDotsAbout::slotAccepted()
{
	close();
}

void KDotsAbout::paintEvent(QPaintEvent *e)
{
	QPainter p;

	p.begin(this);
	p.setFont(QFont("courier", 12, QFont::Bold));
	p.drawText(50, 20, i18n("Connect The Dots for KDE"));
	p.setFont(QFont("courier", 12, QFont::Normal));
	p.drawText(20, 50, "Copyright (C) 2001 Josef Spillner");
	p.drawText(20, 70, "The MindX Open Source Project");
	p.drawText(20, 90, "dr_maux@users.sourceforge.net");
	p.drawText(20, 130, i18n("Thanks to drachenburg.de"));
	p.drawText(20, 150, i18n("for their dragon images!"));
	p.end();
}

