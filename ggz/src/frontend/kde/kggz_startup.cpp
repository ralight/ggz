#include "kggz_startup.h"
#include <qpushbutton.h>
#include <qlayout.h>

QWidget *logo;
QPushButton *button;

KGGZ_Startup::KGGZ_Startup(QWidget *parent, char *name)
: QWidget(parent, name)
{
	QVBoxLayout *vbox1;

	logo = new QWidget(this);
	logo->setFixedSize(500, 400);

	button = new QPushButton("Restore last session", logo);
	button->setGeometry(180, 370, 140, 20);

	vbox1 = new QVBoxLayout(this, 0);
	vbox1->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	vbox1->add(logo);
}

KGGZ_Startup::~KGGZ_Startup()
{
}

void KGGZ_Startup::setBackgroundPixmap(QPixmap pixmap)
{
	logo->setBackgroundPixmap(pixmap);
}

