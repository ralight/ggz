#include "dlg_about.h"

#include "config.h"

#include <qpixmap.h>
#include <qpainter.h>
#include <qpushbutton.h>

DlgAbout::DlgAbout(QWidget *parent, char *name)
: QWidget(parent, name)
{
	QPushButton *ok;

	ok = new QPushButton("OK", this);
	connect(ok, SIGNAL(clicked()), SLOT(close()));
	ok->setGeometry(100, 270, 100, 20);

	setCaption("About Krosswater");
	setFixedSize(300, 300);
	show();
}

DlgAbout::~DlgAbout()
{
}

void DlgAbout::paintEvent(QPaintEvent *e)
{
	QPainter p;

	p.begin(this);
	p.drawTiledPixmap(0, 0, 300, 300, QPixmap(GGZDATADIR "/krosswater/gfx/bg.png"));
	p.drawPixmap(60, 30, QPixmap(GGZDATADIR "/krosswater/gfx/title.png"));
	p.setPen(QPen(QColor(255.0, 255.0, 0.0)));
	p.setFont(QFont("arial", 10));
	p.drawText(15, 90, "Try to cross the water the most efficient way!");
	p.drawText(15, 103, "This game is part of the GGZ Gaming Zone.");
	p.drawText(15, 116, "http://ggz.sourceforge.net");
	p.drawText(15, 135, "Copyright (C) 2001 Josef Spillner");
	p.drawText(15, 148, "dr_maux@users.sourceforge.net");
	p.drawText(15, 161, "The MindX Open Source Project");
	p.drawText(15, 174, "http://mindx.sourceforge.net");
	p.drawPixmap(15, 190, QPixmap(GGZDATADIR "/krosswater/gfx/mindx.png"));
	//p.drawPixmap(122, 250, QPixmap(GGZDATADIR "/krosswater/gfx/ok.png"));
	p.end();
}

void DlgAbout::mousePressEvent(QMouseEvent *e)
{
	if(!e) return;
	//if((e->x() > 122) && (e->x() < 177) && (e->y() > 250) && (e->y() < 270)) close();
}

