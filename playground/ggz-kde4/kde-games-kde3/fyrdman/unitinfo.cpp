#include "unitinfo.h"
#include "map.h"

#include <klocale.h>

#include <qlayout.h>
#include <qframe.h>
#include <qpushbutton.h>
#include <qpainter.h>
#include <qlabel.h>

#include "config.h"

UnitInfo::UnitInfo(int num, QWidget *parent, const char *name)
: QDialog(parent, name, true)
{
	QVBoxLayout *lay;
	QHBoxLayout *lay2;
	QPushButton *ok;
	QLabel *l, *l2;

	l = new QLabel(i18n("This is the unit type assigned to you."), this);
	l2 = new QLabel(i18n("You are player %1").arg(num), this);

	frame = new QFrame(this);
	frame->setFixedSize(64, 64);
	frame->setFrameStyle(QFrame::Panel | QFrame::Sunken);

	ok = new QPushButton(i18n("Close"), this);

	lay = new QVBoxLayout(this, 5);
	lay->add(l);
	lay->add(l2);
	lay2 = new QHBoxLayout(lay, 5);
	lay2->addStretch(1);
	lay2->add(frame);
	lay2->addStretch(1);
	lay->add(ok);

	display(num);

	connect(ok, SIGNAL(clicked()), SLOT(close()));

	setCaption(i18n("Unit information"));
}

UnitInfo::~UnitInfo()
{
}

void UnitInfo::display(int num)
{
	int x = num;
	QPainter p;
	QPixmap pix, bg;
	int angle, ypos;

	angle = 5;
	ypos = 32;

	if(x >= 0)
	{
		if(x % 2) pix = QPixmap(GGZDATADIR "/fyrdman/knight2.png");
		else pix = QPixmap(GGZDATADIR "/fyrdman/knight1.png");
		frame->setFixedSize(pix.width(), pix.height());
		bg.resize(pix.width(), pix.height());
		bg.fill(QColor(0, 0, 0));

		p.begin(&bg);

		p.drawPixmap(QPoint(0, 0), pix);

		p.fillRect(angle, ypos + 2 * angle, frame->width() - 2 * angle, 7, QBrush(QColor(0, 0, 0)));
		QColor c;
		if(x % 2) c = QColor(0, 0, 255);
		else c = QColor(255, 0, 0);
		p.fillRect(angle + 1, ypos + 2 * angle + 1, frame->width() - 2 * angle - 2, 2, QBrush(c));
		c = Map::colour(x);
		p.fillRect(angle + 1, ypos + 2 * angle + 4, frame->width() - 2 * angle - 2, 2, QBrush(c));

		p.end();

		frame->setPaletteBackgroundPixmap(bg);
	}
}

