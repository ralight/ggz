#include "dlg_person.h"

#include "config.h"

#include <qpushbutton.h>
#include <qpainter.h>

DlgPerson::DlgPerson(QWidget *parent, char *name)
: QWidget(parent, name)
{
	QPushButton *person1, *person2, *person3;

	person1 = new QPushButton("", this);
	person2 = new QPushButton("", this);
	person3 = new QPushButton("", this);

	person1->setPixmap(QPixmap(GGZDATADIR "/krosswater/gfx/franzl.png"));
	person2->setPixmap(QPixmap(GGZDATADIR "/krosswater/gfx/alien.png"));
	person3->setPixmap(QPixmap(GGZDATADIR "/krosswater/gfx/geek.png")),

	person1->setGeometry(150, 150, 40, 40);
	person2->setGeometry(150, 200, 40, 40);
	person3->setGeometry(150, 250, 40, 40);

	connect(person1, SIGNAL(clicked()), SLOT(slotPerson1()));
	connect(person2, SIGNAL(clicked()), SLOT(slotPerson2()));
	connect(person3, SIGNAL(clicked()), SLOT(slotPerson3()));

	setCaption("Character Selection");
	setFixedSize(200, 300);
	show();
}

DlgPerson::~DlgPerson()
{
}

void DlgPerson::slotPerson1()
{
	emit signalAccepted(0);
	close();
}

void DlgPerson::slotPerson2()
{
	emit signalAccepted(1);
	close();
}

void DlgPerson::slotPerson3()
{
	emit signalAccepted(2);
	close();
}

void DlgPerson::paintEvent(QPaintEvent *e)
{
	QPainter p;

	p.begin(this);
	p.setFont(QFont("arial", 10));
	p.setPen(QPen(QColor(255.0, 255.0, 0.0)));
	p.drawTiledPixmap(0, 0, 200, 300, QPixmap(GGZDATADIR "/krosswater/gfx/bg.png"));
	p.drawPixmap(10, 30, QPixmap(GGZDATADIR "/krosswater/gfx/title.png"));
	p.drawText(15, 90, "Please select your favorite character!");
	p.drawText(15, 178, "Bavarian");
	p.drawText(15, 228, "Alien");
	p.drawText(15, 278, "Geek");
	p.end();
}
