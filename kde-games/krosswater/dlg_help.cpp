#include "dlg_help.h"

#include "config.h"

#include <qpixmap.h>
#include <qpainter.h>
#include <qpushbutton.h>

DlgHelp::DlgHelp(QWidget *parent, char *name)
: QWidget(parent, name)
{
	QPushButton *ok;

	ok = new QPushButton("OK", this);
	connect(ok, SIGNAL(clicked()), SLOT(close()));
	ok->setGeometry(150, 370, 100, 20);

	setCaption("Krosswater Help");
	setFixedSize(400, 400);
	show();
}

DlgHelp::~DlgHelp()
{
}

void DlgHelp::paintEvent(QPaintEvent *e)
{
	QPainter p;

	p.begin(this);
	p.drawTiledPixmap(0, 0, 400, 400, QPixmap(GGZDATADIR "/krosswater/gfx/bg.png"));
	p.drawPixmap(60, 30, QPixmap(GGZDATADIR "/krosswater/gfx/title.png"));
	p.setPen(QPen(QColor(255, 255, 0)));
	p.setFont(QFont("arial", 10));
	p.drawText(15, 90, "As one of 2 to 4 players on the board, your task is to");
	p.drawText(15, 103, "cross the water from the left to the right. This can be accomplished");
	p.drawText(15, 116, "by clicking on a stone tile, and then clicking on a water tile to move");
	p.drawText(15, 129, "the stone tile there.");
	p.drawText(15, 142, "The more players participate, the more fun it is to steal stone tiles that");
	p.drawText(15, 155, "have been placed by the opponents for their own purpose.");
	p.drawText(15, 168, "The game ends if the first player has build a track of stones from his");
	p.drawText(15, 181, "starting point to somewhere on the right side.");
	p.end();
}
