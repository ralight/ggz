#include "kggz_startup.h"
#include <qpushbutton.h>
#include <qlayout.h>

QWidget *logo;
QPushButton *button;

KGGZ_Startup::KGGZ_Startup(QWidget *parent, char *name)
: QWidget(parent, name)
{
	QVBoxLayout *vbox1, *vbox2, *vbox3;;

	logo = new QWidget(this);
	logo->setFixedSize(500, 400);

	button = new QPushButton("Restore last session", this);
	button->setFixedSize(100, 20);

	vbox1 = new QVBoxLayout(this, 0);
	vbox2 = new QVBoxLayout(vbox1, 5);
	vbox3 = new QVBoxLayout(vbox1, 5);
	vbox2->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	vbox2->add(logo);
	vbox3->setAlignment(Qt::AlignLeft | Qt::AlignBottom);
	vbox3->add(button);
}

KGGZ_Startup::~KGGZ_Startup()
{
}

void KGGZ_Startup::setBackgroundPixmap(QPixmap pixmap)
{
	logo->setBackgroundPixmap(pixmap);
}

