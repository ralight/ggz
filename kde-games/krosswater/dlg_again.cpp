#include "dlg_again.h"

#include "config.h"

#include <klocale.h>

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

	m_result = "(result unknown)";

	connect(cancel, SIGNAL(clicked()), SLOT(close()));
	connect(ok, SIGNAL(clicked()), SIGNAL(signalAgain()));

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
	p.drawText(15, 90, i18n("The game is over."));
	p.drawText(15, 103, m_result);
	p.drawText(15, 116, i18n("Play another game?"));
	p.end();
}

void DlgAgain::setResult(const char *result)
{
	m_result = QString(result);
}

