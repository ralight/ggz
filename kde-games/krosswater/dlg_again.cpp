#include "dlg_again.h"

#include "config.h"

#include <qpixmap.h>
#include <qpainter.h>
#include <qpushbutton.h>

DlgAgain::DlgAgain(QWidget *parent, char *name)
: QWidget(parent, name)
{
	QPushButton *ok, *cancel;

	ok = new QPushButton("Yes", this);
	cancel = new QPushButton("No", this);

	ok->setGeometry(30, 140, 100, 30);
	cancel->setGeometry(170, 140, 100, 30);

	setCaption("Game Over");
	setFixedSize(300, 180);
	show();
}

DlgAgain::~DlgAgain()
{
}

void DlgAgain::paintEvent(QPaintEvent *e)
{
	QPainter p;

	p.begin(this);
	p.drawTiledPixmap(0, 0, 300, 300, QPixmap(GGZDATADIR "/krosswater/gfx/bg.png"));
	p.drawPixmap(60, 30, QPixmap(GGZDATADIR "/krosswater/gfx/title.png"));
	p.setPen(QPen(QColor(255.0, 255.0, 0.0)));
	p.setFont(QFont("arial", 10));
	p.drawText(15, 90, "The game is over. You won|lost.");
	p.drawText(15, 103, "Play another game?");
	p.end();
}
